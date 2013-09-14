// ProcArgs.h : 存储命令行参数
// 这里有点像 Java 风格, 虽然只要一个结构体就够了

#pragma once

#define NO_CODEPAGE (-1)

class ProcArgs_t
{
public:
    ProcArgs_t(void);
    ~ProcArgs_t(void);
private:
    wchar_t* import_file;
    wchar_t* export_file;
    int source_codepage;
    int target_codepage;
    char default_char;
    bool add_bom;
    bool is_warning;
    bool is_analyze;
    bool is_force_exit;
    bool _good;
public:
    void setImportFile(wchar_t* s);
    void setExportFile(wchar_t* s);
    void setSourceCodepage(wchar_t* s);
    void setSourceCodepage(int i);
    void setTargetCodepage(wchar_t* s);
    void setDefaultChar(wchar_t* s);
    void setAddBOM(void);
    void setWarning(void);
    void setAnalyze(void);
    void setForceExit(void);
    void setHelp(void);
    wchar_t* getImportFile(void);
    wchar_t* getExportFile(void);
    int getSourceCodepage(void);
    int getTargetCodepage(void);
    char getDefaultChar(void);
    bool getAddBOM(void);
    bool getWarning(void);
    bool getAnalyze(void);
    bool getForceExit(void);
    bool good(void);
    void TestGood(void);
private:
    void ConvertCodepageText(int& cpint, const wchar_t* cptext);
};

#include "ProcArgs.inl"
