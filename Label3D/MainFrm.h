
// MainFrm.h : CMainFrame 类的接口
//

#pragma once
#include "resource.h"
#include "ControlView.h"
#include "Label3DDoc.h"
#include "Label3DView.h"

class CMainFrame : public CFrameWnd
{
	
protected: // 仅从序列化创建
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)

// 特性
public:

// 操作
public:

// 重写
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// 实现
public:
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

private:
	CSplitterWnd m_splitter;
	CLabel3DView* m_pViewWnd;
	CControlView* m_pControlWnd;

	bool m_ifChangeSplitter;


// 生成的消息映射函数
protected:
	DECLARE_MESSAGE_MAP()

public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);

	afx_msg void OnTest();
};


