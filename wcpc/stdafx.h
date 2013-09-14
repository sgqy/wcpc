// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#include "targetver.h"

#include <stdio.h>
#include <tchar.h>
#include <io.h>
#include <fcntl.h>
#include <stdlib.h>
#ifndef _DEBUG
#include <afxwin.h> // 为了静态编译才包含的 Orz
#endif
#include <Windows.h>

#define STATIC_GETOPT
#include "getopt.h"

#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <stdexcept>

// TODO: 在此处引用程序需要的其他头文件
#include "GlobalDef.h"
#include "resource.h"

#include "PrintMessage.h"
#include "ProcArgs.h"
#include "F64PushLine.h"
#include "F64ReadLine.h"

#include "Declare.h"
