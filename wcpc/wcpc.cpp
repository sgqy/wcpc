// wcpc.cpp : 定义控制台应用程序的入口点。
// 全都用 wchar_t 版本了, 那个 _setmode 函数不适合用 _TCHAR

#include "stdafx.h"

PrintMessage_t PrintMessage; // 转门用于打印消息

int wmain(int argc, wchar_t* argv[])
{
    //SendMessageW( // 设置窗口图标, Just for entertainment
    //    GetConsoleWindow(), 
    //    WM_SETICON, 
    //    ICON_SMALL, 
    //    (LPARAM)LoadIconW(GetModuleHandleW(NULL), MAKEINTRESOURCEW(IDI_ICON1))
    //    );

    try {
        ProcessArgs(argc, argv); // 处理命令行参数

        ProcConversion(); // 执行转码

        return 0;
    } catch(const int& e) {
        return e; // 有了资源文件这里就简化了
    } catch(std::bad_alloc&) { // 简单捕捉内存分配问题
        PrintMessage.PrintIt(stderr, IDS_MEMORY_ALLOC_ERROR_STR, Args.getImportFile());
        throw EC_MEMORY_ALLOC_ERROR;
    } catch(...) {
        return EC_UNKNOWN_ERROR;
    }
}

