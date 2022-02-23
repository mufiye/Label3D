
// MainFrm.cpp : CMainFrame ���ʵ��
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



// CMainFrame ����/����

CMainFrame::CMainFrame()
{
	// TODO: �ڴ���ӳ�Ա��ʼ������
	m_ifChangeSplitter = false;
}

CMainFrame::~CMainFrame()
{
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	// TODO: �ڴ˴�ͨ���޸�
	//  CREATESTRUCT cs ���޸Ĵ��������ʽ

	cs.style &= ~FWS_ADDTOTITLE;

	//cs.style = WS_OVERLAPPED | WS_CAPTION | FWS_ADDTOTITLE
	//	 | WS_MINIMIZEBOX | WS_MAXIMIZEBOX | WS_SYSMENU;

	return TRUE;
}

// CMainFrame ���

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


// CMainFrame ��Ϣ�������



int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  �ڴ������ר�õĴ�������
	SetWindowText(L"Label3D");

	return 0;
}


void CMainFrame::OnSize(UINT nType, int cx, int cy)
{
	CFrameWnd::OnSize(nType, cx, cy);

	// TODO: �ڴ˴������Ϣ����������
	if (m_ifChangeSplitter)
	{
		m_splitter.SetColumnInfo(0, cx*0.8, 0);
		m_splitter.SetColumnInfo(1, cx*0.2, 0);
		m_splitter.RecalcLayout();
	}
}


BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext)
{
	// TODO: �ڴ����ר�ô����/����û���
	CRect rc;
	// ��ȡ��ܴ��ڿͻ�����CRect���� 
	GetClientRect(&rc);


	if (m_splitter.CreateStatic(this, 1, 2) == NULL)
		return FALSE;
	m_splitter.CreateView(0, 0, RUNTIME_CLASS(CLabel3DView), CSize(rc.Width()*0.8, rc.Height()), pContext);	//�ڵ��ĵ��з�����ͼ���ڴ�С
	m_splitter.CreateView(0, 1, RUNTIME_CLASS(CControlView), CSize(rc.Width() *0.2, rc.Height()), pContext);	//�ڵ��ĵ�����ӶԻ�����

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
