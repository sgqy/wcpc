// ProcConversion.cpp : 转码的主函数

#include "stdafx.h"

#define SENTENCE_LEN 0x3200000 // 每行最大长度

void cpc_internel(int scp, void* src, const int srcl,
                  int tcp, void* tag, int& tagl,
                  const char dft, bool& islost)
{
    tagl = srcl;
    islost = true;
}

inline void test_cp(const int cp)
{
    if(cp != NO_CODEPAGE && cp != 1200 && cp != 1201) {
        if(IsValidCodePage(cp) == FALSE) {
            PrintMessage.PrintIt(stderr, IDS_CODEPAGE_INVALID_INT, cp);
            throw EC_CODEPAGE_INVALID;
        }
    }
}

void ProcConversion()
{
    try {
        F64ReadLine ImportFile(Args.getImportFile()); // 建立输入文件, 强制执行
        if(!ImportFile.good()) {
            PrintMessage.PrintIt(stderr, IDS_FILE_ERROR_STR, Args.getImportFile());
            throw EC_FILE_ERROR;
        }
//        ImportFile.TestEnc(); // 分析输入文件编码, 我只想说等 SetFilePointerEx 之类的问题解决后再启用
        test_cp(Args.getSourceCodepage()); // 测试编码可用性
        test_cp(Args.getTargetCodepage());

        F64PushLine ExportFile(Args.getExportFile()); // 作用域限定, 先创建
        if(Args.getExportFile()) { // 确实存在输出文件的情况下
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

        void* sentence = new char[MEMPROTECT(SENTENCE_LEN * 3)]; // 每行通用缓存

        for(long long i = 1; ; ++i) {

            source_len = ImportFile.readline(sentence, SENTENCE_LEN);
            if(source_len == -1) { // 行的字符数超过限度
                PrintMessage.PrintIt(stderr, IDS_LENGTH_ERROR_INT, i);
                PrintMessage.PrintIt(stderr, IDS_NAKED_STR, Args.getImportFile());
                throw EC_LENGTH_ERROR;
            }
            cpc_internel(Args.getSourceCodepage(), sentence, source_len, // 得到行的无BOM数据后转码
                Args.getTargetCodepage(), sentence, target_len,
                Args.getDefaultChar(), islost);

            if(Args.getAnalyze()) { // 开启 -A 选项后报告字符丢失情况
                if(islost) {
                    PrintMessage.PrintIt(stdout, IDS_ANALYZE_SINGLE_INT, i);
                    ++lost_count;
                    islost = false;
                }
            }

            if(Args.getExportFile()) { // 开启 -o 选项后输出至文件
                ExportFile.pushline(sentence, target_len);
            }

            if(ImportFile.eof()) { // 输入文件结束后退出转码循环
                if(Args.getExportFile()) {
                    ExportFile.flush();
                }
                break;
            }
        }

        delete [] sentence; // 清除缓存
        ////////////////////////////////////////////////////////////////////////////////
        if(Args.getAnalyze()) { // 开启 -A 时输出分析最终结果
            PrintMessage.PrintIt(stdout, IDS_ANALYZE_RESULT_INT, lost_count);
        }

        if(Args.getForceExit() && lost_count != 0) { // 开启 -E 后, 抛弃未保存的数据并删除输出文件
            if(Args.getExportFile()) {
                ExportFile.close(false);
                DeleteFileW(Args.getExportFile());
            }
            PrintMessage.PrintIt(stderr, IDS_LOSTCHAR_FORCE_EXIT_STR, Args.getImportFile());
            throw EC_LOSTCHAR_FORCE_EXIT;
        }

    } catch(std::bad_alloc&) { // 简单捕捉内存分配问题
        PrintMessage.PrintIt(stderr, IDS_MEMORY_ALLOC_ERROR_STR, Args.getImportFile());
        throw EC_MEMORY_ALLOC_ERROR;
    }
}

