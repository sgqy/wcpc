// F64PushLine.cpp
#include "stdafx.h"
#include "F64PushLine.h"


F64PushLine::~F64PushLine(void)
{
    flush();
    if(hFile != INVALID_HANDLE_VALUE) CloseHandle(hFile);
    if(ringbuff) delete [] ringbuff;
}


F64PushLine::F64PushLine(const wchar_t* fn)
    : is_good(true)
    , hFile(INVALID_HANDLE_VALUE)
    , ringbuff(0)
    , ringsz(0x100000)
    , ringidx(0)
{
    ringbuff = new char[MEMPROTECT(ringsz)];
    hFile = CreateFileW(fn, GENERIC_WRITE, 0, 
        NULL, CREATE_ALWAYS, FILE_FLAG_SEQUENTIAL_SCAN, NULL);
    if(hFile == INVALID_HANDLE_VALUE) {
        is_good = false;
        return;
    }
}


int F64PushLine::pushline(const void* cs, const int csl)
{
    char* p = (char*)cs;
    int i = 0;
    while(i < csl) {
        ringbuff[ringidx++] = p[i++];
        if(ringidx == ringsz) {
            flush();
            ringidx = 0;
        }
    }
    return i;
}


bool F64PushLine::flush(void)
{
    BOOL ret = 0;
    int dummy;
    if(hFile != INVALID_HANDLE_VALUE) {
        ret = WriteFile(hFile, ringbuff, ringidx, (LPDWORD)&dummy, NULL);
        ringidx = 0;
    }
    return (ret == TRUE && ringidx == dummy);
}


void F64PushLine::close(const bool isflush)
{
    if(isflush) flush();
    if(hFile != INVALID_HANDLE_VALUE) {
        CloseHandle(hFile);
        hFile = INVALID_HANDLE_VALUE;
    }
}
