// F64ReadLine.cpp

#include "stdafx.h"
#include "F64ReadLine.h"


F64ReadLine::~F64ReadLine(void)
{
    if(ringbuff) delete [] ringbuff;
    if(hFile != INVALID_HANDLE_VALUE) CloseHandle(hFile);
}


F64ReadLine::F64ReadLine()
    : is_good(true)
    , is_eof(false)
    , hFile(INVALID_HANDLE_VALUE)
    , ringbuff(0)
    , ringsz(0x100000)  // 环形圆圈大小为 1 MiB // 后续添加定制功能
    , ringidx(0)
    , blocksz(0)
    , codepage(0)
    , eof_flag(false)
{
    ringbuff = new char[MEMPROTECT(ringsz)];
}

void F64ReadLine::open(const wchar_t* fn)
{
    hFile = CreateFileW(fn, GENERIC_READ, FILE_SHARE_READ, 
        NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
    if(hFile == INVALID_HANDLE_VALUE) {
        is_good = false;
        return;
    }
    GetFileSizeEx(hFile, &filesz);
    setpointer(0);
    blocksz = filesz.QuadPart > ringsz ? ringsz : filesz.QuadPart;
}


int F64ReadLine::readline(void* cs, const int csl)
    // 将这个环形数组的每一圈看作一个有输入输出的容器
{
    if(!csl) return 0;
    if(eof()) return 0;

    int wl = 0; // 读取到的正行长度

    char* p = (char*)cs;

    while(1) {
        bool break_mark = false;
        if(!(ringidx = ringidx % ringsz)) {
            ReadFile(hFile, ringbuff, ringsz, (LPDWORD)&blocksz, NULL);
            if(blocksz < ringsz) {
                eof_flag = true; // 数组的输入端达到eof
            }
        }
        bool u16test = false;
        while(wl < csl && ringidx < blocksz) { // 将当前圈内的数据导出
            p[wl++] = ringbuff[ringidx++]; // 此表达式有且仅有一个, 否则严重 bug

            if(u16test && !p[wl-1]) {
                break_mark = true;
                break;
            }
            u16test = false; // 测试一次不成功必须关闭, 否则不论是否遇到 '\n', 一旦遇到 0 都会断开
            if(p[wl-1] == '\n') {

                if(codepage != 1200) {
                    break_mark = true;
                    break;
                }
                u16test = true;
            }

        }
        if(eof_flag && ringidx == blocksz) {
            is_eof = true; // 数组的输出端, 即本身达到了eof
            break_mark = true;
        }

        p[wl] = 0; // 截断

        if(break_mark) { // 退出此循环
            break;
        }
        if(wl == csl) { // 真糟糕, 这一行没见到行尾
            return -1;
        }
    }
    return wl; 
    // 一旦除以 2, UTF-16LE 编码的字符数会少 1
    // UTF16-BE 不受影响
}

void F64ReadLine::setpointer(const int i)
{
        LARGE_INTEGER offset = {0};
        offset.LowPart = i;
        SetFilePointerEx(hFile, offset, NULL, FILE_BEGIN);
}

bool F64ReadLine::isutf8(const char* str, const int len)
    // 代码来源 : http://blog.csdn.net/lufubo/article/details/8232234
{
    int i = 0;     
    int ssz = strlen(str);   
    int size = ssz < len ? ssz : len;
    while(i < size) {         
        int step = 0;         
        if((str[i] & 0x80) == 0x00) {
            step = 1;         
        }else if((str[i] & 0xe0) == 0xc0) {             
            if(i + 1 >= size) 
                return false;             
            if((str[i + 1] & 0xc0) != 0x80) 
                return false;               
            step = 2;         
        }else if((str[i] & 0xf0) == 0xe0) {             
            if(i + 2 >= size) 
                return false;             
            if((str[i + 1] & 0xc0) != 0x80) 
                return false;             
            if((str[i + 2] & 0xc0) != 0x80) 
                return false;               
            step = 3;        
        }else {             
            return false;         
        }          
        i += step;    
    }       
    if(i == size) 
        return true;      
    return false; 
}

void F64ReadLine::TestEnc(void)
{
    if(Args.getSourceCodepage() != NO_CODEPAGE) return;

    char* buf = new char[MEMPROTECT(32768)];
    int size = 0;
    setpointer(0);
    ReadFile(hFile, buf, 32768, (LPDWORD)&size, NULL);
    do {
        if(size > 2) { // 逆天等级的 bug 发现! 非 unsigned 情况下, 0xFF 和 '\xFF' 不同!
            if(buf[0] == '\xEF' && buf[1] == '\xBB' && buf[2] == '\xBF') {
                Args.setSourceCodepage(codepage = 65001);
                setpointer(3);
                break;
            }
        }
        if(size > 1) {
            if(buf[0] == '\xFF' && buf[1] == '\xFE') {
                Args.setSourceCodepage(codepage = 1200);
                setpointer(2);
                break;
            }
            if(buf[0] == '\xFE' && buf[1] == '\xFF') {
                Args.setSourceCodepage(codepage = 1201);
                setpointer(2);
                break;
            }
            if(TRUE == IsTextUnicode(buf, 32768, NULL)) {
                Args.setSourceCodepage(codepage = 1200);
                setpointer(2);
                break;
            }
        }
        if(isutf8(buf, 32768)) {
            Args.setSourceCodepage(codepage = 65001);
            setpointer(0);
            break;
        }
        Args.setSourceCodepage(codepage = GetACP());
        setpointer(0);
        break;
    } while(0);
    delete [] buf;
}


void F64ReadLine::close(void)
{
    if(hFile != INVALID_HANDLE_VALUE) {
        CloseHandle(hFile);
        hFile = INVALID_HANDLE_VALUE;
    }
}
