// F64ReadLine.cpp

#include "stdafx.h"
#include "F64ReadLine.h"


F64ReadLine::~F64ReadLine(void)
{
    if(ringbuff) delete [] ringbuff;
    if(hFile != INVALID_HANDLE_VALUE) CloseHandle(hFile);
}


F64ReadLine::F64ReadLine(const wchar_t* fn)
    : is_good(true)
    , is_eof(false)
    , hFile(INVALID_HANDLE_VALUE)
    , ringbuff(0)
    , ringsz(0x100000) // 读入文件的环形缓冲区大小
    , ringidx(0)
    , blocksz(0)
    , codepage(0)
    , eof_flag(false)
{
    ringbuff = new char[MEMPROTECT(ringsz)];
    hFile = CreateFileW(fn, GENERIC_READ, FILE_SHARE_READ, 
        NULL, OPEN_EXISTING, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
    if(hFile == INVALID_HANDLE_VALUE) {
        is_good = false;
        return;
    }
    GetFileSizeEx(hFile, &filesz);
    LARGE_INTEGER p = {0};
    SetFilePointerEx(hFile, p, NULL, FILE_BEGIN);
    blocksz = filesz.QuadPart > ringsz ? ringsz : filesz.QuadPart;
}


int F64ReadLine::readline(void* cs, const int csl)
{
    if(!csl) return 0;
    if(eof()) return 0;

    int wl = 0;

    char* p = (char*)cs;

    while(1) {
        if(!(ringidx = ringidx % ringsz)) {
            ReadFile(hFile, ringbuff, ringsz, (LPDWORD)&blocksz, NULL);
            if(blocksz < ringsz) {
                eof_flag = true;
            }
        }
        while(wl < csl && ringidx < blocksz) {
            if('\n' == (p[wl++] = ringbuff[ringidx++])) {
                break;
            }
        }
        if(eof_flag && ringidx == blocksz) {
            is_eof = true;
        }

        if(p[wl-1] == '\n' || eof()) {
            break;
        }
        if(wl == csl) {
            return -1;
        }
        p[wl] = 0;
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
    int size = readline(buf, 32768);
    if(size > 2 && buf[0] == 0xEF && buf[1] == 0xBB && buf[2] == 0xBF) {
        Args.setSourceCodepage(codepage = 65001);
        setpointer(3);
    } else if(size > 1) {
        if(buf[0] == 0xFF && buf[1] == 0xFE) {
            Args.setSourceCodepage(codepage = 1200);
            setpointer(2);
        } else if(buf[0] == 0xFE && buf[1] == 0xFF) {
            Args.setSourceCodepage(codepage = 1201);
            setpointer(2);
        } else if(TRUE == IsTextUnicode(buf, 32768, NULL)) {
            Args.setSourceCodepage(codepage = 1200);
            setpointer(2);
        } else {
        
        }
    } else if(isutf8(buf, 32768)) {
        Args.setSourceCodepage(codepage = 65001);
        setpointer(0);
    } else {
        Args.setSourceCodepage(codepage = GetACP());
        setpointer(0);
    }
    delete [] buf;
}


void F64ReadLine::close(void)
{
    if(hFile != INVALID_HANDLE_VALUE) {
        CloseHandle(hFile);
        hFile = INVALID_HANDLE_VALUE;
    }
}
