// ProcArgs.inl : 头文件的一部分


#pragma once

inline ProcArgs_t::ProcArgs_t(void)
    : import_file(nullptr)
    , export_file(nullptr)
    , source_codepage(NO_CODEPAGE)
    , target_codepage(1200)
    , default_char('?')
    , add_bom(false)
    , is_warning(false)
    , is_analyze(false)
    , is_force_exit(false)
    , _good(true)
{
}


inline ProcArgs_t::~ProcArgs_t(void)
{
}


inline void ProcArgs_t::setImportFile(wchar_t* s)
{
    import_file = s;
}


inline void ProcArgs_t::setExportFile(wchar_t* s)
{
    export_file = s;
}


inline void ProcArgs_t::setSourceCodepage(wchar_t* s)
{
    ConvertCodepageText(source_codepage, s);
}


inline void ProcArgs_t::setSourceCodepage(int i)
{
    source_codepage = i;
}


inline void ProcArgs_t::setTargetCodepage(wchar_t* s)
{
    ConvertCodepageText(target_codepage, s);
}


inline void ProcArgs_t::setDefaultChar(wchar_t* s)
{
    default_char = 0xFF & s[0];
}


inline void ProcArgs_t::setAddBOM(void)
{
    add_bom = true;
}


inline void ProcArgs_t::setWarning(void)
{
    is_warning = true;
}


inline void ProcArgs_t::setAnalyze(void)
{
    is_analyze = true;
}


inline void ProcArgs_t::setForceExit(void)
{
    is_force_exit = true;
}

inline void ProcArgs_t::setHelp(void)
{
    _good = false;
}

inline wchar_t* ProcArgs_t::getImportFile(void)
{
    return import_file;
}


inline wchar_t* ProcArgs_t::getExportFile(void)
{
    return export_file;
}


inline int ProcArgs_t::getSourceCodepage(void)
{
    return source_codepage;
}


inline int ProcArgs_t::getTargetCodepage(void)
{
    return target_codepage;
}


inline char ProcArgs_t::getDefaultChar(void)
{
    return default_char;
}


inline bool ProcArgs_t::getAddBOM(void)
{
    return add_bom;
}


inline bool ProcArgs_t::getWarning(void)
{
    return is_warning;
}


inline bool ProcArgs_t::getAnalyze(void)
{
    return is_analyze;
}


inline bool ProcArgs_t::getForceExit(void)
{
    return is_force_exit;
}


inline bool ProcArgs_t::good(void)
{
    return _good;
}


inline void ProcArgs_t::ConvertCodepageText(int& cpint, const wchar_t* cptext)
{
    if(wcslen(cptext) > 8) {
        _good = false;
        return;
    }
    cpint = ::_wtol(cptext);
}

inline void ProcArgs_t::TestGood(void)
{
    if(!import_file) _good = false;
    if(!export_file && !is_analyze) _good = false;
    if(target_codepage == 1200 || target_codepage == 1201 || target_codepage == 65001) is_warning = false;
}
