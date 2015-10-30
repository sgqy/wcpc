// F64PushLine.h : 用于按行输出文件

#pragma once
class F64PushLine
{
public:
    ~F64PushLine(void);
protected:
    bool is_good;
    HANDLE hFile;
    char* ringbuff;
    int ringsz;
    int ringidx;
public:
    F64PushLine();
    void open(const wchar_t* fn);
    bool good(void) {
        return is_good;
    }
    int pushline(const void* cs, const int csl);
    bool flush(void);
    void close(const bool isflush = true);
};

