
// Label3D.h : Label3D Ӧ�ó������ͷ�ļ�
//
#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"       // ������


// CLabel3DApp:
// �йش����ʵ�֣������ Label3D.cpp
//

class CLabel3DApp : public CWinApp
{
public:
	CLabel3DApp();


// ��д
public:
	virtual BOOL InitInstance();

// ʵ��
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CLabel3DApp theApp;
