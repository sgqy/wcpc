// Declare.h : 放置各种全局变量和跨文件函数声明

#pragma once

extern ProcArgs_t Args;
extern PrintMessage_t PrintMessage;
extern void PrintHelp();
extern void ProcessArgs(int, wchar_t*[]);
extern void ProcConversion();
