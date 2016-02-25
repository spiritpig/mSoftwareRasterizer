// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             //  从 Windows 头文件中排除极少使用的信息
// Windows 头文件:
#include <windows.h>

// C 运行时头文件
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <cmath>
#include <vector>
#include <fstream>

#include "resource.h"
#include "Vertex.h"
#include "mMatrix4x4.h"
#include "mCamera.h"

const float PI = 3.14159265358979f;
const float DEG_TO_RAD = PI / 180.0f;

#define SAFE_DELETE(p) { if(p) { delete p; p=NULL; } }
#define SAFE_DELARR(p) { if(p) { delete []p; p=NULL; } }

// TODO: 在此处引用程序需要的其他头文件
#include "baseMath.h"
#include "InputManager.h"
#include "mSoftwareRasterizer.h"

