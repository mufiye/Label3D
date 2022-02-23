
// Label3DDoc.cpp : CLabel3DDoc 类的实现
//

#include "stdafx.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
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


// CLabel3DDoc 构造/析构

CLabel3DDoc::CLabel3DDoc()
{
	// TODO: 在此添加一次性构造代码
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

	// TODO: 在此添加重新初始化代码
	// (SDI 文档将重用该文档)

	return TRUE;
}




// CLabel3DDoc 序列化

void CLabel3DDoc::Serialize(CArchive& ar)
{
	if (ar.IsStoring())
	{
		// TODO: 在此添加存储代码
	}
	else
	{
		// TODO: 在此添加加载代码
	}
}

#ifdef SHARED_HANDLERS

// 缩略图的支持
void CLabel3DDoc::OnDrawThumbnail(CDC& dc, LPRECT lprcBounds)
{
	// 修改此代码以绘制文档数据
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

// 搜索处理程序的支持
void CLabel3DDoc::InitializeSearchContent()
{
	CString strSearchContent;
	// 从文档数据设置搜索内容。
	// 内容部分应由“;”分隔

	// 例如:     strSearchContent = _T("point;rectangle;circle;ole object;")；
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

// CLabel3DDoc 诊断

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


// CLabel3DDoc 命令

//将TCHAR类型转换为string类型
std::string TCHAR2STRING(TCHAR *STR)
{
	int iLen = WideCharToMultiByte(CP_ACP, 0, STR, -1, NULL, 0, NULL, NULL);
	char* chRtn = new char[iLen * sizeof(char)];
	WideCharToMultiByte(CP_ACP, 0, STR, -1, chRtn, iLen, NULL, NULL);
	std::string str(chRtn);

	return str;

}

//选择文件Dialog
bool GetFileName(string& fileName, const string& fileType)
{
	CString filePathName;
	CFileDialog dlg(TRUE, //TRUE为OPEN对话框，FALSE为SAVE AS对话框
		NULL,
		NULL,
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		//(LPCTSTR)_TEXT("Excel Files (*.obj)|*.OBJ|All Files (*.*)|*.*||"),
		(LPCTSTR)CString(fileType.c_str()),
		NULL);
	if (dlg.DoModal() == IDOK)
	{
		filePathName = dlg.GetPathName(); //路径名+文件名
		fileName = CT2A(filePathName.GetBuffer());
		return true;
	}
	return false;
}

//选择文件夹Dialog
bool GetDir(string &strDir)
{
	// 获取特定文件夹的LPITEMIDLIST，可以将之理解为HANDLE  
	// 所谓的特定文件夹,你可以用CSIDL_XXX来检索之。  
	LPITEMIDLIST rootLoation;
	SHGetSpecialFolderLocation(NULL, CSIDL_DESKTOP, &rootLoation);
	if (rootLoation == NULL) {
		// unkown error  
		return false;
	}
	// 配置对话框  
	BROWSEINFO bi;
	ZeroMemory(&bi, sizeof(bi));
	bi.pidlRoot = rootLoation; // 文件夹对话框之根目录，不指定的话则为我的电脑  
	bi.lpszTitle = _T("选择文件夹"); // 可以不指定  
								// bi.ulFlags = BIF_EDITBOX | BIF_RETURNONLYFSDIRS;  

								// 打开对话框, 有点像DoModal  
	LPITEMIDLIST targetLocation = SHBrowseForFolder(&bi);
	if (targetLocation != NULL) {
		TCHAR targetPath[MAX_PATH];
		SHGetPathFromIDList(targetLocation, targetPath);
		strDir = TCHAR2STRING(targetPath);
		return true;
	}

	return false;
}

//加载模型
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

//加载标注文件
void CLabel3DDoc::OnLoadAnnocation()
{
	string fileName;
	CString FilePathName;
	CFileDialog dlg(TRUE, //TRUE为OPEN对话框，FALSE为SAVE AS对话框
		NULL,
		NULL,
		OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		//(LPCTSTR)_TEXT("Excel Files (*.txt)|*.TXT|All Files (*.*)|*.*||"),
		(LPCTSTR)_TEXT("Excel Files (*.json)|*.JSON|All Files (*.*)|*.*||"),
		NULL);
	if (dlg.DoModal() == IDOK)
	{
		FilePathName = dlg.GetPathName(); //路径名+文件名
		fileName = CT2A(FilePathName.GetBuffer());
		bool isOk = modelEditor->LoadAnnocation(fileName);
		if (isOk)
		{
			std::cout << "加载标注文件完成" << endl;
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

//设置单选框“选中”
void CLabel3DDoc::SetRadioSelect()
{
	GetControlView()->SetRadioSelect();
}

//设置单选框“编辑”
void CLabel3DDoc::SetRadioEdit()
{
	GetControlView()->SetRadioEdit();
}

//设置单选框相机旋转方式选中状态
void CLabel3DDoc::SetRadioPivotPointType(PivotPointType ppt)
{
	GetControlView()->SetRadioPivotPointType(ppt);
}


//void CLabel3DDoc::UpdateLabelList()
//{
//	GetControlView()->UpdateLabelList();
//}

//测试2
void CLabel3DDoc::OnTest2()
{
	CMyTest test;
	//test.Test();
}

//去除竖直面片
void CLabel3DDoc::OnRemoveVerFaces()
{
	CMyTest test;
	//test.RemoveVerFaces();
	test.TestSwapIns();
}

//将3D标注文件转换为Panoptic全景分割训练标注格式文件
void CLabel3DDoc::OnGenpanolabel()
{
	CMyTest test;
	//test.GenPanoLabel();	//第一步
	test.MergePanopticLabels(); //第二步
}
