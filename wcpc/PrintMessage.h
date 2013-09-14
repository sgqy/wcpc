// PrintMessage.h : 资源文件的打印装置

#pragma once
class PrintMessage_t
{
public:

    PrintMessage_t(void)
    {
        _setmode(_fileno(stdin),  _O_U16TEXT); // 将标准 I/O 改成 Unicode 模式
        _setmode(_fileno(stdout), _O_U16TEXT);
        _setmode(_fileno(stderr), _O_U16TEXT);
    }

    ~PrintMessage_t(void)
    {
    }

    template<typename PrEx>
    // 用于 wchar_t* 和 int
    // 以后可能会改成不定项参数形式
    void PrintIt(FILE* fp, int messageID, const PrEx extra)
    {
        const int bufferLen = 512;
        wchar_t msgBuffer[bufferLen];
        LoadStringW(GetModuleHandleW(NULL), messageID, msgBuffer, bufferLen);
        fwprintf_s(fp, msgBuffer, extra);
    }
};

