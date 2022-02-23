
// Label3DDoc.cpp : CLabel3DDoc ���ʵ��
//

#include "stdafx.h"
// SHARED_HANDLERS ������ʵ��Ԥ��������ͼ������ɸѡ�������
// ATL ��Ŀ�н��ж��壬�����������Ŀ�����ĵ����롣
#ifndef SHARED_HANDLERS
#include "Label3D.h"
#endif

#include "Label3DDoc.h"

#include <propkey.h>
#include "ControlView.h"
#include "Label3DView.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif

// CLabel3DDoc

IMPLEMENT_DYNCREATE(CLabel3DDoc, CDocument)

BEGIN_MESSAGE_MAP(CLabel3DDoc, CDocument)
	ON_COMMAND(ID_LoadModel, &CLabel3DDoc::OnLoadmodel)
	ON_COMMAND(ID_LoadAnnocation, &CLabel3DDoc::OnLoadAnnocation)
	ON_COMMAND(ID_Test2, &CLabel3DDoc::OnTest2)
	ON_COMMAND(ID_RemoveVerFaces, &CLabel3DDoc::OnRemoveVerFaces)
	ON_COMMAND(ID_GenPanoLabel, &CLabel3DDoc::OnGenpanolabel)
END_MESSAGE_MAP()


// CLabel3DDoc ����/����

CLabel3DDoc::CLabel3DDoc()
{
	// TODO: �ڴ����һ���Թ������
	modelEditor = NULL;
}

CLabel3DDoc::~CLabel3DDoc()
{
	if (modelEditor)
	{
		delete modelEditor;
	}
	modelEditor = NULL;
}

CControlView * CLabel3DDoc::GetControlView(void)
{
	for (POSITION pos = GetFirstViewPosition();pos != NULL;)
	{
		CView* pView = GetNextView(pos);
		if (pView->IsKindOf(RUNTIME_CLASS(CControlView)))
			return ((CControlView*)pView);
	} //End of for pos
	return NULL;
}

CLabel3DView * CLabel3DDoc::GetLabel3DView(void)
{
	for (POSITION pos = GetFirstViewPosition();pos != NULL;)
	{
		CView* pView = GetNextView(pos);
		if (pView->IsKindOf(RUNTIME_CLASS(CLabel3DView)))
			return ((CLabel3DView*)pView);
	} //End of for pos
	return NULL;
}

BOOL CLabel3DDoc::OnNewDocument()
{
	if (!CDocument::OnNewDocument())
		return FALSE;

	// TODO: �ڴ�������³�ʼ������
	// (SDI �ĵ������ø��ĵ�)

	return TRUE;
}




// CLabel3DDoc ���л�

void CLabel3DDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: �ڴ���Ӵ洢����
	}
	else
	{
		// TODO: �ڴ���Ӽ��ش���
	}
}

#ifdef SHARED_HANDLERS

// ����ͼ��֧��
void CLabel3DDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// �޸Ĵ˴����Ի����ĵ�����
	dc.FillSolidRect(lprcBounds, RGB(255, 255, 255));

	CString strText = _T("TODO: implement thumbnail drawing here");
	LOGFONT lf;

	CFont* pDefaultGUIFont = CFont::FromHandle((HFONT) GetStockObject(DEFAULT_GUI_FONT));
	pDefaultGUIFont->GetLogFont(&lf);
	lf.lfHeight = 36;

	CFont fontDraw;
	fontDraw.CreateFontIndirect(&lf);

	CFont* pOldFont = dc.SelectObject(&fontDraw);
	dc.DrawText(strText, lprcBounds, DT_CENTER | DT_WORDBREAK);
	dc.SelectObject(pOldFont);
}

// ������������֧��
void CLabel3DDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// ���ĵ����������������ݡ�
	// ���ݲ���Ӧ�ɡ�;���ָ�

	// ����:     strSearchContent = _T("point;rectangle;circle;ole object;")��
	SetSearchContent(strSearchContent);
}

void CLabel3DDoc::SetSearchContent(const CString& value)
{
	if (value.IsEmpty())
	{
		RemoveChunk(PKEY_Search_Contents.fmtid, PKEY_Search_Contents.pid);
	}
	else
	{
		CMFCFilterChunkValueImpl *pChunk = NULL;
		ATLTRY(pChunk = new CMFCFilterChunkValueImpl);
		if (pChunk != NULL)
		{
			pChunk->SetTextValue(PKEY_Search_Contents, value, CHUNK_TEXT);
			SetChunkValue(pChunk);
		}
	}
}

#endif // SHARED_HANDLERS

// CLabel3DDoc ���

#ifdef _DEBUG
void CLabel3DDoc::AssertValid() const
{
	CDocument::AssertValid();
}

void CLabel3DDoc::Dump(CDumpContext& dc) const
{
	CDocument::Dump(dc);
}
#endif //_DEBUG


// CLabel3DDoc ����

//��TCHAR����ת��Ϊstring����
std::string TCHAR2STRING(TCHAR *STR)
{
	int iLen = WideCharToMultiByte(CP_ACP, 0, STR, -1, NULL, 0, NULL, NULL);
	char* chRtn = new char[iLen * sizeof(char)];
	WideCharToMultiByte(CP_ACP, 0, STR, -1, chRtn, iLen, NULL, NULL);
	std::string str(chRtn);

	return str;

}

//ѡ���ļ�Dialog
bool GetFileName(string& fileName, const string& fileType)
{
	CString filePathName;
	CFileDialog dlg(TRUE, //TRUEΪOPEN�Ի���FALSEΪSAVE AS�Ի���
		NULL,
		NULL,
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		//(LPCTSTR)_TEXT("Excel Files (*.obj)|*.OBJ|All Files (*.*)|*.*||"),
		(LPCTSTR)CString(fileType.c_str()),
		NULL);
	if (dlg.DoModal() == IDOK)
	{
		filePathName = dlg.GetPathName(); //·����+�ļ���
		fileName = CT2A(filePathName.GetBuffer());
		return true;
	}
	return false;
}

//ѡ���ļ���Dialog
bool GetDir(string &strDir)
{
	// ��ȡ�ض��ļ��е�LPITEMIDLIST�����Խ�֮���ΪHANDLE  
	// ��ν���ض��ļ���,�������CSIDL_XXX������֮��  
	LPITEMIDLIST rootLoation;
	SHGetSpecialFolderLocation(NULL, CSIDL_DESKTOP, &rootLoation);
	if (rootLoation == NULL) {
		// unkown error  
		return false;
	}
	// ���öԻ���  
	BROWSEINFO bi;
	ZeroMemory(&bi, sizeof(bi));
	bi.pidlRoot = rootLoation; // �ļ��жԻ���֮��Ŀ¼����ָ���Ļ���Ϊ�ҵĵ���  
	bi.lpszTitle = _T("ѡ���ļ���"); // ���Բ�ָ��  
								// bi.ulFlags = BIF_EDITBOX | BIF_RETURNONLYFSDIRS;  

								// �򿪶Ի���, �е���DoModal  
	LPITEMIDLIST targetLocation = SHBrowseForFolder(&bi);
	if (targetLocation != NULL) {
		TCHAR targetPath[MAX_PATH];
		SHGetPathFromIDList(targetLocation, targetPath);
		strDir = TCHAR2STRING(targetPath);
		return true;
	}

	return false;
}

//����ģ��
void CLabel3DDoc::OnLoadmodel()
{
	string modelFileName;
	string fileType = "Excel Files (*.obj)|*.OBJ|All Files (*.*)|*.*||";
	if (GetFileName(modelFileName, fileType))
	{
		modelEditor->LoadModel(modelFileName);
		//GetControlView()->UpdateLabelList();
		modelEditor->RenderScene();
	}
}

//���ر�ע�ļ�
void CLabel3DDoc::OnLoadAnnocation()
{
	string fileName;
	CString FilePathName;
	CFileDialog dlg(TRUE, //TRUEΪOPEN�Ի���FALSEΪSAVE AS�Ի���
		NULL,
		NULL,
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		//(LPCTSTR)_TEXT("Excel Files (*.txt)|*.TXT|All Files (*.*)|*.*||"),
		(LPCTSTR)_TEXT("Excel Files (*.json)|*.JSON|All Files (*.*)|*.*||"),
		NULL);
	if (dlg.DoModal() == IDOK)
	{
		FilePathName = dlg.GetPathName(); //·����+�ļ���
		fileName = CT2A(FilePathName.GetBuffer());
		bool isOk = modelEditor->LoadAnnocation(fileName);
		if (isOk)
		{
			std::cout << "���ر�ע�ļ����" << endl;
			modelEditor->RenderScene();
			//GetControlView()->UpdateLabelList(); 
			GetControlView()->UpdateTreeControl();
		}
	}
}

void CLabel3DDoc::SetSelectedItem(int id)
{
	GetControlView()->SetSelectItem(id);
}

bool CLabel3DDoc::SeletcTreeItem(int cateId, int insId)
{
	return GetControlView()->SelectTreeItem(cateId, insId);
}

void CLabel3DDoc::AddNewListItem()
{
	GetControlView()->AddNewListItem();
}

//���õ�ѡ��ѡ�С�
void CLabel3DDoc::SetRadioSelect()
{
	GetControlView()->SetRadioSelect();
}

//���õ�ѡ�򡰱༭��
void CLabel3DDoc::SetRadioEdit()
{
	GetControlView()->SetRadioEdit();
}

//���õ�ѡ�������ת��ʽѡ��״̬
void CLabel3DDoc::SetRadioPivotPointType(PivotPointType ppt)
{
	GetControlView()->SetRadioPivotPointType(ppt);
}


//void CLabel3DDoc::UpdateLabelList()
//{
//	GetControlView()->UpdateLabelList();
//}

//����2
void CLabel3DDoc::OnTest2()
{
	CMyTest test;
	//test.Test();
}

//ȥ����ֱ��Ƭ
void CLabel3DDoc::OnRemoveVerFaces()
{
	CMyTest test;
	//test.RemoveVerFaces();
	test.TestSwapIns();
}

//��3D��ע�ļ�ת��ΪPanopticȫ���ָ�ѵ����ע��ʽ�ļ�
void CLabel3DDoc::OnGenpanolabel()
{
	CMyTest test;
	//test.GenPanoLabel();	//��һ��
	test.MergePanopticLabels(); //�ڶ���
}
