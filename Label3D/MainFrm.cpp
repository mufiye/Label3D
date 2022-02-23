
// MainFrm.cpp : CMainFrame 类的实现
//

#include "stdafx.h"
#include "Label3D.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CMainFrame

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_COMMAND(ID_Test, &CMainFrame::OnTest)
END_MESSAGE_MAP()



// CMainFrame 构造/析构

CMainFrame::CMainFrame()
{
	// TODO: 在此添加成员初始化代码
	m_ifChangeSplitter = false;
}

CMainFrame::~CMainFrame()
{
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	cs.style &= ~FWS_ADDTOTITLE;

	//cs.style = WS_OVERLAPPED | WS_CAPTION | FWS_ADDTOTITLE
	//	 | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU;

	return TRUE;
}

// CMainFrame 诊断

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}
#endif //_DEBUG


// CMainFrame 消息处理程序



int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码
	SetWindowText(L"Label3D");

	return 0;
}


void CMainFrame::OnSize(UINT nType, int cx, int cy)
{
	CFrameWnd::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
	if (m_ifChangeSplitter)
	{
		m_splitter.SetColumnInfo(0, cx*0.8, 0);
		m_splitter.SetColumnInfo(1, cx*0.2, 0);
		m_splitter.RecalcLayout();
	}
}


BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext)
{
	// TODO: 在此添加专用代码和/或调用基类
	CRect rc;
	// 获取框架窗口客户区的CRect对象 
	GetClientRect(&rc);


	if (m_splitter.CreateStatic(this, 1, 2) == NULL)
		return FALSE;
	m_splitter.CreateView(0, 0, RUNTIME_CLASS(CLabel3DView), CSize(rc.Width()*0.8, rc.Height()), pContext);	//在单文档中分类视图窗口大小
	m_splitter.CreateView(0, 1, RUNTIME_CLASS(CControlView), CSize(rc.Width() *0.2, rc.Height()), pContext);	//在单文档中添加对话框类

	m_pViewWnd = (CLabel3DView*)m_splitter.GetPane(0, 1);
	m_pControlWnd = (CControlView*)m_splitter.GetPane(0, 0);

	m_ifChangeSplitter = true;

	return TRUE;

	return CFrameWnd::OnCreateClient(lpcs, pContext);
}


void CMainFrame::OnTest()
{
	//CLabel3DDoc* pDoc = (CLabel3DDoc*)GetActiveDocument();
	//bool isOk = pDoc->modelEditor->LoadAnnocation("data/wangcun-annotation.txt");

	//if (isOk)
	//{
	//	pDoc->modelEditor->RenderScene();
	//}

	//m_pControlWnd->UpdateLabelList();
}
