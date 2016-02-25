// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�
//

#pragma once

#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             //  �� Windows ͷ�ļ����ų�����ʹ�õ���Ϣ
// Windows ͷ�ļ�:
#include <windows.h>

// C ����ʱͷ�ļ�
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

// TODO: �ڴ˴����ó�����Ҫ������ͷ�ļ�
#include "baseMath.h"
#include "InputManager.h"
#include "mSoftwareRasterizer.h"

