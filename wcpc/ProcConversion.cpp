// ProcConversion.cpp : 转码的主函数

#include "stdafx.h"

#define SENTENCE_LEN 0x400000 // 每行最大长度 4 MiB // 后续版本将添加自定义缓存容量

inline void reverse_utf16(wchar_t* ws, const int wsl)
{
    for(int i = 0; i < wsl; ++i) {
        ws[i] = (ws[i] << 8) | (ws[i] >> 8);
    }
}

void cpc_internel(const int scp, const void* src, const int srcl,
                  const int tcp, void* tag, int& tagl,
                  const char dft, bool& islost)
{
    wchar_t* wstmp = new wchar_t[MEMPROTECT(SENTENCE_LEN * 2)]; // 建立缓存用于生成 UTF-16 序列

    int wslen = 0; // wstmp 中 UTF-16 序列的长度, 按照 wchar_t 计算, 和 WinAPI 保持一致

    switch(scp) { // 代码页
    case 1200: // UTF-16LE
        memcpy_s(wstmp, MEMPROTECT(SENTENCE_LEN * 2), src, srcl + 1);
        wslen = (srcl + 1) /2;
        break;
    case 1201: // UTF-16BE
        memcpy_s(wstmp, MEMPROTECT(SENTENCE_LEN * 2), src, srcl);
        wslen = srcl / 2;
        reverse_utf16(wstmp, wslen);
        break;
    default: // UTF-8 || ANSI
        wslen = MultiByteToWideChar(scp, 0, (LPCCH)src, srcl, NULL, 0);
        MultiByteToWideChar(scp, 0, (LPCCH)src, srcl, wstmp, wslen);
    }

    switch(tcp) {
    case 1200: // UTF-16LE
        memcpy_s(tag, MEMPROTECT(SENTENCE_LEN * 2), wstmp, tagl = wslen * 2);
        break;
    case 1201: // UTF-16BE
        reverse_utf16(wstmp, wslen);
        memcpy_s(tag, MEMPROTECT(SENTENCE_LEN * 2), wstmp, tagl = wslen * 2);
        break;
    default: // UTF-8 || ANSI
        int lost = 0;
        int* plost = (tcp == 65001) ? NULL : &lost; // UTF-8 的话有 BUG ...
        const char* pdft = (tcp == 65001) ? NULL : &dft;
        tagl = WideCharToMultiByte(tcp, 0, wstmp, wslen, NULL, NULL, pdft, plost);
        WideCharToMultiByte(tcp, 0, wstmp, wslen, (LPSTR)tag, tagl, pdft, plost);
        if(lost == TRUE) {
            islost = true;
        } else {
            islost = false;
        }
    }

    delete [] wstmp; // 清空缓存
}

inline void test_cp(const int cp)
{
    if(cp != NO_CODEPAGE && cp != 1200 && cp != 1201) {
        if(IsValidCodePage(cp) == FALSE) {
            // 没安装日文代码页的就请自己想办法吧ww
            // 都说了不要用精简版系统为什么就是不信233
            PrintMessage.PrintIt(stderr, IDS_CODEPAGE_INVALID_INT, cp);
            throw EC_CODEPAGE_INVALID;
        }
    }
}

void ProcConversion()
{
        F64ReadLine ImportFile;
        F64PushLine ExportFile;

        ImportFile.open(Args.getImportFile()); // 建立输入文件, 强制执行
        if(!ImportFile.good()) {
            PrintMessage.PrintIt(stderr, IDS_FILE_ERROR_STR, Args.getImportFile());
            throw EC_FILE_ERROR;
        }
        ImportFile.TestEnc(); // 分析输入文件编码
        test_cp(Args.getSourceCodepage()); // 测试编码可用性
        test_cp(Args.getTargetCodepage());

        if(Args.getExportFile()) { // 建立输出文件情况下 // 建立输出文件后不应该主动抛出异常
            ExportFile.open(Args.getExportFile());
            if(!ExportFile.good()) {
                PrintMessage.PrintIt(stderr, IDS_FILE_ERROR_STR, Args.getExportFile());
                throw EC_FILE_ERROR;
            }
            if(Args.getTargetCodepage() == 65001 && Args.getAddBOM()) { // 为输出文件建立 BOM
                ExportFile.pushline((char*)"\xEF\xBB\xBF", 3);
            } else if(Args.getTargetCodepage() == 1200) {
                ExportFile.pushline((char*)"\xFF\xFE", 2);
            } else if(Args.getTargetCodepage() == 1201) {
                ExportFile.pushline((char*)"\xFE\xFF", 2);
            }
        }
        /////////////////////////////////////////////////////////////////////////////
        int source_len = 0; // 读取长度(字节
        int target_len = 0; // 转换后长度(字节
        bool islost = false; // 丢失字符标记
        long long lost_count = 0; // 丢失字符的行数

        void* sentence = new char[MEMPROTECT(SENTENCE_LEN * 2)]; // 每行通用缓存
        ///---------------------------------------------------------------------------------------///
        for(long long i = 1; ; ++i) { // 开始转换

            source_len = ImportFile.readline(sentence, SENTENCE_LEN); // Step 1: 读取一句话
            if(source_len == -1) { // 行的字符数超过限度
                PrintMessage.PrintIt(stderr, IDS_LENGTH_ERROR_INT, i);
                PrintMessage.PrintIt(stderr, IDS_NAKED_STR, Args.getImportFile());
                throw EC_LENGTH_ERROR;
            }
            cpc_internel(Args.getSourceCodepage(), sentence, source_len, // Step 2: 得到行的无BOM数据后转码
                Args.getTargetCodepage(), sentence, target_len,
                Args.getDefaultChar(), islost);

            if(islost) { // Step 3: 分析丢失情况并提出警告
                ++lost_count;
                if(Args.getAnalyze()) { // 开启 -A 选项后报告字符丢失情况

                    PrintMessage.PrintIt(stdout, IDS_ANALYZE_SINGLE_INT, i);

                }
                islost = false;
            }

            if(Args.getExportFile()) { // Step 4: 把转码后的句子输出至文件
                ExportFile.pushline(sentence, target_len);
            }

            if(ImportFile.eof()) { // Step 5: 转码结束后保存文件, 退出循环
                if(Args.getExportFile()) {
                    ExportFile.flush();
                }
                break;
            }
        }
        ///---------------------------------------------------------------------------------------///
        delete [] sentence; // 清除缓存
        ////////////////////////////////////////////////////////////////////////////////
        if(Args.getAnalyze() || Args.getWarning()) { // 开启 -A 或 -W 时输出统计结果
            PrintMessage.PrintIt(stdout, IDS_ANALYZE_RESULT_INT, lost_count);
        }
        // 自动析构
}

