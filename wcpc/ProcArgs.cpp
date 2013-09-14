// ProcArgs.cpp : 

#include "stdafx.h"

ProcArgs_t Args; // 存储命令行参数

void PrintHelp()
    // 用了资源文件这里有些捉急了
{
    PrintMessage.PrintIt(stderr, IDS_HELP_MSG0, 0);
    ////////////////////////////////////////////////////////////////
    // 下方区域放置参数说明
    PrintMessage.PrintIt(stderr, IDS_HELP_MSG1, 0);
    PrintMessage.PrintIt(stderr, IDS_HELP_MSG2, 0);
    PrintMessage.PrintIt(stderr, IDS_HELP_MSG3, 0);
    PrintMessage.PrintIt(stderr, IDS_HELP_MSG4, 0);
    PrintMessage.PrintIt(stderr, IDS_HELP_MSG5, 0);
    PrintMessage.PrintIt(stderr, IDS_HELP_MSG6, 0);
    PrintMessage.PrintIt(stderr, IDS_HELP_MSG8, 0);
    PrintMessage.PrintIt(stderr, IDS_HELP_MSG9, 0);
    ////////////////////////////////////////////////////////////////
    // 下方放置参数解释
    //PrintMessage.PrintIt(stderr, IDS_HELP_MSG10, 0);
    //PrintMessage.PrintIt(stderr, IDS_HELP_MSG11, 0);
    PrintMessage.PrintIt(stderr, IDS_HELP_MSG12, 0);
    PrintMessage.PrintIt(stderr, IDS_HELP_MSG13, 0);
    PrintMessage.PrintIt(stderr, IDS_HELP_MSG14, 0);
}

void ProcessArgs(int argc, wchar_t* argv[])
    // 处理命令行参数. 这里借用了网上下载的 getopt 修改版
    // 具体的 getopt 用法此处不做注释
{
    int opt = opterr = 0;
    const wchar_t* optstr = L"i:o:f:t:c:bWAEh?";
    while(~(opt = getopt_w(argc, argv, optstr))) {
        switch (opt)
        {
        case L'i':
            Args.setImportFile(optarg_w);
            break;
        case L'o':
            Args.setExportFile(optarg_w);
            break;
        case L'f':
            Args.setSourceCodepage(optarg_w);
            break;
        case L't':
            Args.setTargetCodepage(optarg_w);
            break;
        case L'c':
            Args.setDefaultChar(optarg_w);
            break;
        case L'b':
            Args.setAddBOM();
            break;
        case L'W':
            Args.setWarning();
            break;
        case L'A':
            Args.setAnalyze();
            break;
        case L'E':
            Args.setForceExit();
            break;
        case L'h':
        case L'?':
            Args.setHelp();
            break;
        default:
            throw EC_INTERNEL_ERROR;
            break;
        }
    }
    Args.TestGood();
    if(!Args.good()) {
        PrintHelp();
        throw EC_INCORRECT_ARGUMENTS;
    }
}
