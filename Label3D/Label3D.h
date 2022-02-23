
// Label3D.h : Label3D 应用程序的主头文件
//
#pragma once

#ifndef __AFXWIN_H__
	#error "在包含此文件之前包含“stdafx.h”以生成 PCH 文件"
#endif

#include "resource.h"       // 主符号


// CLabel3DApp:
// 有关此类的实现，请参阅 Label3D.cpp
//

class CLabel3DApp : public CWinApp
{
public:
	CLabel3DApp();


// 重写
public:
	virtual BOOL InitInstance();

// 实现
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CLabel3DApp theApp;
