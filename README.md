Windows TextFile CodePage Converter

Console Program.

Filenames can be unicode char (which not fit with your current codepage).

Simple Usage: (from Shift_JIS to UTF-16)
```batch
for %i in ("my game\*.spt") do wcpc -i "%i" -o "%~ni.txt" -f 932
```
===
UI has two languages: CHS in CHS system; EN in other system.

---

Licence: LGPL
Code comment is written by Chinese, sorry for that.
