// ControlView.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Label3D.h"
#include "ControlView.h"

extern std::string CString2string(CString input);
extern CString string2CString(std::string input);

// CControlView

IMPLEMENT_DYNCREATE(CControlView, CFormView)

CControlView::CControlView()
	: CFormView(IDD_CONTROLVIEW)
	, m_selInsIndex(-1)
	, m_selCateName(_T(""))
	, m_paintPixel(10)
{
}

CControlView::~CControlView()
{
}

void CControlView::DoDataExchange(CDataExchange* pDX)
{
	CFormView::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_SLIDERAmbiLightS, m_scAmbiLightStrength);
	DDX_Control(pDX, IDC_SLIDERDiffLightS, m_scDiffLightStrength);
	DDX_Control(pDX, IDC_SLIDERSpecLightS, m_scSpecLightStrength);
	DDX_Control(pDX, IDC_SLIDERAmbiMateri, m_scAmbiMateriStrength);
	DDX_Control(pDX, IDC_SLIDERDiffMateri, m_scDiffMateriStrength);
	DDX_Control(pDX, IDC_SLIDERSpecMateri, m_scSpecMateriStrength);
	DDX_Text(pDX, IDC_EDITAmbiLight, m_AmbiLightStrength);
	DDV_MinMaxInt(pDX, m_AmbiLightStrength, 1, 100);
	DDX_Text(pDX, IDC_EDITDiffLight, m_DiffLightStrength);
	DDV_MinMaxInt(pDX, m_DiffLightStrength, 1, 100);
	DDX_Text(pDX, IDC_EDITSpecLight, m_SpecLightStrength);
	DDV_MinMaxInt(pDX, m_SpecLightStrength, 1, 100);
	DDX_Text(pDX, IDC_EDITAmbiMateri, m_AmbiMateriStrength);
	DDV_MinMaxInt(pDX, m_AmbiMateriStrength, 1, 100);
	DDX_Text(pDX, IDC_EDITDiffMateri, m_DiffMateriStrength);
	DDV_MinMaxInt(pDX, m_DiffMateriStrength, 1, 100);
	DDX_Text(pDX, IDC_EDITSpecMateri, m_SpecMateriStrength);
	DDV_MinMaxInt(pDX, m_SpecMateriStrength, 1, 100);
	DDX_Control(pDX, IDC_LIST1, m_ListControl);
	DDX_Text(pDX, IDC_EDIT_SelIndex, m_selInsIndex);
	DDX_Control(pDX, IDC_RADIO1, m_Radio_Render);
	DDX_Control(pDX, IDC_Btn_CreateNewIns, m_Radio_Add);
	DDX_Control(pDX, IDC_TREE1, m_treeControl);
	DDX_Text(pDX, IDC_EDIT_SelCate, m_selCateName);
	DDX_Text(pDX, IDC_EDIT_PaintPixel, m_paintPixel);
	DDV_MinMaxInt(pDX, m_paintPixel, 5, 30);
	DDX_Control(pDX, IDC_SLIDER_PaintPixel, m_scPaintPixel);
}

BEGIN_MESSAGE_MAP(CControlView, CFormView)

	ON_WM_CREATE()
	ON_BN_CLICKED(IDC_CheckUseLight0, &CControlView::OnBnClickedCheckuselight0)
	ON_BN_CLICKED(IDC_CheckUseLight1, &CControlView::OnBnClickedCheckuselight1)
	ON_EN_CHANGE(IDC_EDITAmbiLight, &CControlView::OnEnChangeEditambilight)
	ON_EN_CHANGE(IDC_EDITDiffLight, &CControlView::OnEnChangeEditdifflight)
	ON_EN_CHANGE(IDC_EDITSpecLight, &CControlView::OnEnChangeEditspeclight)
	ON_EN_CHANGE(IDC_EDITAmbiMateri, &CControlView::OnEnChangeEditambimateri)
	ON_EN_CHANGE(IDC_EDITDiffMateri, &CControlView::OnEnChangeEditdiffmateri)
	ON_EN_CHANGE(IDC_EDITSpecMateri, &CControlView::OnEnChangeEditspecmateri)
	ON_WM_HSCROLL()
	ON_COMMAND(ID_Test2, &CControlView::OnTest2)
	//ON_NOTIFY(LVN_ITEMCHANGED, IDC_LIST1, &CControlView::OnLvnItemchangedList1)
	ON_NOTIFY(NM_RCLICK, IDC_LIST1, &CControlView::OnNMRClickList1)
	ON_COMMAND(ID_LIST_Delete, &CControlView::OnListDelete)
	ON_BN_CLICKED(IDC_SaveLabelFile, &CControlView::OnBnClickedSaveLabelFile)
	ON_BN_CLICKED(IDC_RADIO1, &CControlView::OnBnClickedRadio1)
	ON_BN_CLICKED(IDC_RADIO2, &CControlView::OnBnClickedRadio2)
	ON_BN_CLICKED(IDC_RADIO3, &CControlView::OnBnClickedRadio3)
	ON_BN_CLICKED(IDC_Btn_CreateNewIns, &CControlView::OnBnClickedBtnCreateNewIns)
	ON_BN_CLICKED(IDC_RADIO4, &CControlView::OnBnClickedRadio4)
	ON_BN_CLICKED(IDC_RADIO5, &CControlView::OnBnClickedRadio5)
	ON_BN_CLICKED(IDC_Btn_CreateNewInsRec, &CControlView::OnBnClickedBtnCreateNewInsRec)
	ON_BN_CLICKED(IDC_RADIO6, &CControlView::OnBnClickedRadio6)
	ON_BN_CLICKED(IDC_RADIO7, &CControlView::OnBnClickedRadio7)
	ON_BN_CLICKED(IDC_RADIO8, &CControlView::OnBnClickedRadio8)
	ON_BN_CLICKED(IDC_RADIO9, &CControlView::OnBnClickedRadio9)
	ON_BN_CLICKED(IDC_RADIO10, &CControlView::OnBnClickedRadio10)
	ON_BN_CLICKED(IDC_RADIO11, &CControlView::OnBnClickedRadio11)
	ON_BN_CLICKED(IDC_RADIO12, &CControlView::OnBnClickedRadio12)
	ON_COMMAND(ID_GetBackFaces, &CControlView::OnGetBackFaces)
	ON_BN_CLICKED(IDC_RADIO13, &CControlView::OnBnClickedRadio13)
	ON_BN_CLICKED(IDC_RADIO14, &CControlView::OnBnClickedRadio14)
	ON_COMMAND(ID_RemoveNoise, &CControlView::OnRemoveNoise)
	ON_COMMAND(ID_GetRoof, &CControlView::OnGetRoof)
	ON_BN_CLICKED(IDC_RADIO15, &CControlView::OnBnClickedRadio15)
	ON_NOTIFY(NM_CLICK, IDC_LIST1, &CControlView::OnNMClickList1)
	ON_COMMAND(ID_LIST_CalcArea, &CControlView::OnListCalcArea)
	ON_BN_CLICKED(IDC_ResetCamera, &CControlView::OnBnClickedResetCamera)
	ON_BN_CLICKED(IDC_RADIO16, &CControlView::OnBnClickedRadio16)
	ON_BN_CLICKED(IDC_RADIO17, &CControlView::OnBnClickedRadio17)
	ON_BN_CLICKED(IDC_RADIO18, &CControlView::OnBnClickedRadio18)
	ON_BN_CLICKED(IDC_RADIO19, &CControlView::OnBnClickedRadio19)
	ON_NOTIFY(NM_CLICK, IDC_TREE1, &CControlView::OnNMClickTree1)
	ON_COMMAND(ID_TREE_DELETE, &CControlView::OnTreeDelete)
	ON_NOTIFY(NM_RCLICK, IDC_TREE1, &CControlView::OnNMRClickTree1)
	ON_COMMAND(ID_TREE_GetBack, &CControlView::OnTreeGetBack)
	ON_COMMAND(ID_TREE_RemoveNoise, &CControlView::OnTreeRemoveNoise)
	ON_COMMAND(ID_32799, &CControlView::OnGraphCut)
	ON_BN_CLICKED(IDC_RADIO21, &CControlView::OnBnClickedRadio21)
	ON_BN_CLICKED(IDC_RADIO22, &CControlView::OnBnClickedRadio22)
	ON_BN_CLICKED(IDC_RADIO23, &CControlView::OnBnClickedRadio23)
	ON_COMMAND(ID_32802, &CControlView::OnTreeNewInstance)
	ON_BN_CLICKED(IDC_RADIO24, &CControlView::OnBnClickedRadio24)
	ON_BN_CLICKED(IDC_RADIO25, &CControlView::OnBnClickedRadio25)
END_MESSAGE_MAP()


// CControlView ���

#ifdef _DEBUG
void CControlView::AssertValid() const
{
	CFormView::AssertValid();
}

#ifndef _WIN32_WCE
void CControlView::Dump(CDumpContext& dc) const
{
	CFormView::Dump(dc);
}
#endif
#endif //_DEBUG

CLabel3DDoc* CControlView::GetDocument() const // �ǵ��԰汾��������
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CLabel3DDoc)));
	return (CLabel3DDoc*)m_pDocument;
}


// CControlView ��Ϣ�������


int CControlView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFormView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  �ڴ������ר�õĴ�������
	this->pDoc = GetDocument();

	return 0;
}

//��ʼ��
void CControlView::OnInitialUpdate()
{
	CFormView::OnInitialUpdate();

	// TODO: �ڴ����ר�ô����/����û���
	m_ListControl.SetExtendedStyle(m_ListControl.GetExtendedStyle() | LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	CString head = TEXT("annocations");
	m_ListControl.InsertColumn(0, head, LVCFMT_LEFT, 300);
	m_ListSelIndex = -1;
	m_selInsIndex = -1;

	CheckRadioButton(IDC_RADIO1, IDC_RADIO15, IDC_RADIO1);	//Ĭ�ϲ�����ѡ��
	CheckRadioButton(IDC_RADIO4, IDC_RADIO5, IDC_RADIO5);	//Ĭ�ϲ����ƿ���
	CheckRadioButton(IDC_RADIO6, IDC_RADIO10, IDC_RADIO6);
	CheckRadioButton(IDC_RADIO11, IDC_RADIO19, IDC_RADIO11);	//Ĭ����ɫ��Ⱦ
	CheckRadioButton(IDC_RADIO16, IDC_RADIO18, IDC_RADIO16);	//Ĭ�ϵ�ѡ
	CheckRadioButton(IDC_RADIO21, IDC_RADIO23, IDC_RADIO21);	//Ĭ����ģ�����ĵ���ת
	CheckRadioButton(IDC_RADIO24, IDC_RADIO25, IDC_RADIO25);	//Ĭ�ϲ��޸��ѱ�ע��Ƭ

	InitTreeControl();
}

//��ʼ����ע���οؼ�
bool CControlView::InitTreeControl()
{
	CLabel3DDoc* m_pDoc = (CLabel3DDoc*)GetDocument();

	m_treeControl.DeleteAllItems();
	int numCategories = m_pDoc->modelEditor->allCategory.size();
	for (int ci = 0; ci < numCategories; ci++)
	{
		HTREEITEM hRoot = TVI_ROOT;     // ���ĸ��ڵ�ľ��   
		TVINSERTSTRUCT ts = { 0 };
		ts.hParent = hRoot;
		ts.hInsertAfter = TVI_LAST;

		int categoryId = m_pDoc->modelEditor->allCategory[ci].categoryId;
		string categoryName = m_pDoc->modelEditor->allCategory[ci].categoryName;
		wstring widstr;
		widstr = std::wstring(categoryName.begin(), categoryName.end());
		ts.item.pszText = (LPWSTR)widstr.c_str();
		ts.item.mask = TVIF_TEXT;
		HTREEITEM cateItem = m_treeControl.InsertItem(&ts);
		m_treeControl.SetItemData(cateItem, categoryId);
		m_treeControl.Expand(cateItem, TVE_EXPAND);
	}

	return true;
}

//����&���ù�Դ0
void CControlView::OnBnClickedCheckuselight0()
{
	CButton* pBtn = (CButton*)GetDlgItem(IDC_CheckUseLight0);
	int state = pBtn->GetCheck();
	if (state == 0)
	{
		pDoc->modelEditor->glRenderParam.useLight0 = false;
	}
	else if (state == 1)
	{
		pDoc->modelEditor->glRenderParam.useLight0 = true;
	}
	else
	{

	}

	pDoc->modelEditor->RefreshGLRenderParm();
	pDoc->modelEditor->RenderScene();
}

//����&���ù�Դ1
void CControlView::OnBnClickedCheckuselight1()
{
	CButton* pBtn = (CButton*)GetDlgItem(IDC_CheckUseLight1);
	int state = pBtn->GetCheck();
	if (state == 0)
	{
		pDoc->modelEditor->glRenderParam.useLight1 = false;
	}
	else if (state == 1)
	{
		pDoc->modelEditor->glRenderParam.useLight1 = true;
	}
	else
	{

	}

	pDoc->modelEditor->RefreshGLRenderParm();
	pDoc->modelEditor->RenderScene();
}

//���û�����ǿ��
void CControlView::OnEnChangeEditambilight()
{
	UpdateData(TRUE);
	m_scAmbiLightStrength.SetPos(m_AmbiLightStrength);
	pDoc->modelEditor->glRenderParam.ambiLightStrength = m_AmbiLightStrength / 100.0;
	//Invalidate();
	pDoc->modelEditor->RefreshGLRenderParm();
	pDoc->modelEditor->RenderScene();
}

//����ɢ���ǿ��
void CControlView::OnEnChangeEditdifflight()
{
	UpdateData(TRUE);
	m_scDiffLightStrength.SetPos(m_DiffLightStrength);
	pDoc->modelEditor->glRenderParam.diffLightStrength = m_DiffLightStrength / 100.0;
	pDoc->modelEditor->RefreshGLRenderParm();
	pDoc->modelEditor->RenderScene();
}

//���þ����ǿ��
void CControlView::OnEnChangeEditspeclight()
{
	UpdateData(TRUE);
	m_scSpecLightStrength.SetPos(m_SpecLightStrength);
	pDoc->modelEditor->glRenderParam.specLightStrength = m_SpecLightStrength / 100.0;
	pDoc->modelEditor->RefreshGLRenderParm();
	pDoc->modelEditor->RenderScene();
}

//���ò��ʻ����ⷴ������
void CControlView::OnEnChangeEditambimateri()
{
	UpdateData(TRUE);
	m_scAmbiMateriStrength.SetPos(m_AmbiMateriStrength);
	pDoc->modelEditor->glRenderParam.ambiMetriStrength = m_AmbiMateriStrength / 100.0;
	pDoc->modelEditor->RefreshGLRenderParm();
	pDoc->modelEditor->RenderScene();
}

//���ò���ɢ��ⷴ������
void CControlView::OnEnChangeEditdiffmateri()
{
	UpdateData(TRUE);
	m_scDiffMateriStrength.SetPos(m_DiffMateriStrength);
	pDoc->modelEditor->glRenderParam.diffMetriStrength = m_DiffMateriStrength / 100.0;
	pDoc->modelEditor->RefreshGLRenderParm();
	pDoc->modelEditor->RenderScene();
}

//���ò��ʾ���ⷴ������
void CControlView::OnEnChangeEditspecmateri()
{
	UpdateData(TRUE);
	m_scSpecMateriStrength.SetPos(m_SpecMateriStrength);
	pDoc->modelEditor->glRenderParam.specMetriStrength = m_SpecMateriStrength / 100.0;
	pDoc->modelEditor->RefreshGLRenderParm();
	pDoc->modelEditor->RenderScene();
}

//���黬���¼�
void CControlView::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
	if (!pScrollBar)
	{
		CFormView::OnHScroll(nSBCode, nPos, pScrollBar);
		return;
	}

	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	if (*pScrollBar == m_scAmbiLightStrength)
	{
		m_AmbiLightStrength = m_scAmbiLightStrength.GetPos();
		UpdateData(FALSE);
		pDoc->modelEditor->glRenderParam.ambiLightStrength = m_AmbiLightStrength / 100.0;
		pDoc->modelEditor->RefreshGLRenderParm();
		pDoc->modelEditor->RenderScene();
	}
	else if (*pScrollBar == m_scDiffLightStrength)
	{
		m_DiffLightStrength = m_scDiffLightStrength.GetPos();
		UpdateData(FALSE);
		pDoc->modelEditor->glRenderParam.diffLightStrength = m_DiffLightStrength / 100.0;
		pDoc->modelEditor->RefreshGLRenderParm();
		pDoc->modelEditor->RenderScene();
	}
	else if (*pScrollBar == m_scSpecLightStrength)
	{
		m_SpecLightStrength = m_scSpecLightStrength.GetPos();
		UpdateData(FALSE);
		pDoc->modelEditor->glRenderParam.specLightStrength = m_SpecLightStrength / 100.0;
		pDoc->modelEditor->RefreshGLRenderParm();
		pDoc->modelEditor->RenderScene();
	}
	else if (*pScrollBar == m_scAmbiMateriStrength)
	{
		m_AmbiMateriStrength = m_scAmbiMateriStrength.GetPos();
		UpdateData(FALSE);
		pDoc->modelEditor->glRenderParam.ambiMetriStrength = m_AmbiMateriStrength / 100.0;
		pDoc->modelEditor->RefreshGLRenderParm();
		pDoc->modelEditor->RenderScene();
	}
	else if (*pScrollBar == m_scDiffMateriStrength)
	{
		m_DiffMateriStrength = m_scDiffMateriStrength.GetPos();
		UpdateData(FALSE);
		pDoc->modelEditor->glRenderParam.diffMetriStrength = m_DiffMateriStrength / 100.0;
		pDoc->modelEditor->RefreshGLRenderParm();
		pDoc->modelEditor->RenderScene();
	}
	else if (*pScrollBar == m_scSpecMateriStrength)
	{
		m_SpecMateriStrength = m_scSpecMateriStrength.GetPos();
		UpdateData(FALSE);
		pDoc->modelEditor->glRenderParam.specMetriStrength = m_SpecMateriStrength / 100.0;
		pDoc->modelEditor->RefreshGLRenderParm();
		pDoc->modelEditor->RenderScene();
	}
	else if (*pScrollBar == m_scPaintPixel)
	{
		int pos = m_scPaintPixel.GetPos();
		m_paintPixel = 5 + floor(pos / 4);
		pDoc->modelEditor->paintPixel = m_paintPixel;
		UpdateData(FALSE);

		pDoc->modelEditor->RenderScene();
	}
}

//���±�ע�б�
bool CControlView::UpdateLabelList()
{
	std::cout << "�����б��" << endl;
	CLabel3DDoc* m_pDoc = (CLabel3DDoc*)GetDocument();

	int numIns = m_pDoc->modelEditor->annotation->numInstance;
	std::cout << "ʵ����Ŀ��" << numIns << endl;

	if (numIns <= 0)
	{
		return false;
	}

	m_ListControl.DeleteAllItems();

	CString str;
	for (size_t i = 0; i < numIns; i++)
	{
		str.Format(_T("%d"), i);

		m_ListControl.InsertItem(i, str);
	}

	return true;
}

//����2
void CControlView::OnTest2()
{
	CString str;

	for (size_t i = 0; i < 20; i++)
	{
		str.Format(_T("%d"), i);

		m_ListControl.InsertItem(i, str);
	}
}

//�б�ؼ�ѡ�з����仯
void CControlView::OnLvnItemchangedList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	*pResult = 0;

	for (int i = 0; i < m_ListControl.GetItemCount(); i++)
	{
		if (m_ListControl.GetItemState(i, LVIS_SELECTED) == LVIS_SELECTED)
		{
			if (m_ListSelIndex != i)
			{
				CString str;
				str = m_ListControl.GetItemText(i, 0);//��ȡָ���У�Ϊ�ַ�����ʽ
				int insIndex = atoi(CT2A(str.GetBuffer()));

				if (this->pDoc->modelEditor->ifEdited)
				{
					this->pDoc->modelEditor->CalcInsBBox(this->pDoc->modelEditor->selInsIndex);
					this->pDoc->modelEditor->ifEdited = false;
				}

				pDoc->modelEditor->selInsIndex = insIndex;

				m_ListSelIndex = i;
				m_selInsIndex = i;
				std::cout << "ѡ�е� " << insIndex << " ��ʵ��" << "" << endl;

				//����ѡ��ʵ����Χ������camera�ӽ�
				//if (this->pDoc->modelEditor->editState == Edit_Render)
				//{
				//	m_pDoc->modelEditor->SetGLCameraWithInsBox(insIndex, View_Down);
				//}
				((CButton *)GetDlgItem(IDC_RADIO1))->SetCheck(FALSE);
				((CButton *)GetDlgItem(IDC_RADIO2))->SetCheck(FALSE);
				((CButton *)GetDlgItem(IDC_RADIO3))->SetCheck(FALSE);
				((CButton *)GetDlgItem(IDC_RADIO15))->SetCheck(TRUE);
				pDoc->modelEditor->SetStateEdit();

				pDoc->modelEditor->SetGLCameraWithInsBox(insIndex, View_Down);
				this->pDoc->modelEditor->SetCameraPivotPoint(PP_InsCenter);
				this->SetRadioPivotPointType(PP_InsCenter);
				pDoc->modelEditor->RenderScene();

				UpdateData(FALSE);
			}
		}
	}
}

//�б��Ҽ��¼�
void CControlView::OnNMRClickList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	*pResult = 0;

	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	if (pNMListView->iItem != -1)
	{
		DWORD dwPos = GetMessagePos();
		CPoint point(LOWORD(dwPos), HIWORD(dwPos));

		CMenu menu;
		VERIFY(menu.LoadMenu(IDR_MENU1));

		CMenu* popup = menu.GetSubMenu(0);
		ASSERT(popup != NULL);
		popup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this);
	}

	*pResult = 0;
}

//�б��Ҽ�ɾ���¼�
void CControlView::OnListDelete()
{
	int i, iState;
	int nItemSelected = m_ListControl.GetSelectedCount();
	int nItemCount = m_ListControl.GetItemCount();
	if (nItemSelected<1)
		return;
	for (i = nItemCount - 1;i >= 0;i--)
	{
		iState = m_ListControl.GetItemState(i, LVIS_SELECTED);
		if (iState != 0)
		{
			CString str;
			str = m_ListControl.GetItemText(i, 0);//��ȡָ���У�Ϊ�ַ�����ʽ
			int insIndex = atoi(CT2A(str.GetBuffer()));

			//ɾ��Itemǰ��ɾ��Annotation
			CLabel3DDoc* m_pDoc = (CLabel3DDoc*)GetDocument();
			m_pDoc->modelEditor->annotation->DeleteInstance(insIndex);
			m_pDoc->modelEditor->selInsIndex = -2;

			m_ListControl.DeleteItem(i);

			m_pDoc->modelEditor->RenderScene();
		}
	}
}

//�����ע�ļ�
void CControlView::OnBnClickedSaveLabelFile()
{
	string fileName;
	//TCHAR szFilter[] = _T("txt�ļ�(*.txt)|*.txt|�����ļ�(*.*)|*.*||");
	TCHAR szFilter[] = _T("json�ļ�(*.json)|*.json|�����ļ�(*.*)|*.*||");
	// ���챣���ļ��Ի���   
	CFileDialog fileDlg(FALSE, _T("doc"), _T("test"), OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, szFilter, this);
	CString strFilePath;

	// ��ʾ�����ļ��Ի���   
	if (IDOK == fileDlg.DoModal())
	{
		// ���������ļ��Ի����ϵġ����桱��ť����ѡ����ļ�·����ʾ���༭����   
		strFilePath = fileDlg.GetPathName();
		fileName = CT2A(strFilePath.GetBuffer());

		CLabel3DDoc* m_pDoc = (CLabel3DDoc*)GetDocument();
		//bool isOk = m_pDoc->modelEditor->annotation->SaveToFile(fileName, true);
		bool isOk = m_pDoc->modelEditor->labelInfo->SaveAsFile(fileName);


		if (isOk)
		{
			std::cout << "�����ע�ļ��ɹ���" << endl;
			//UpdateLabelList();
			//m_pDoc->modelEditor->selInsIndex = -2;
			UpdateTreeControl();
			m_pDoc->modelEditor->RenderScene();
		}
	}
}

//�����б�ѡ����
bool CControlView::SetSelectItem(int index)
{
	int nItemCount = m_ListControl.GetItemCount();
	if (index >= nItemCount)
	{
		return false;
	}

	if (m_ListSelIndex!= index)
	{
		m_ListControl.SetItemState(m_ListSelIndex, 0, -1);
		m_ListControl.SetItem(index, 0, LVIF_STATE, NULL, 0, LVIS_SELECTED, LVIS_SELECTED, 0);
		m_selInsIndex = index;
		m_ListSelIndex = index;
		UpdateData(FALSE);
	}
}

//���б�����item
void CControlView::AddNewListItem()
{
	int newInsIndex = pDoc->modelEditor->annotation->numInstance - 1;

	CString str;
	str.Format(_T("%d"), newInsIndex);
	m_ListControl.InsertItem(newInsIndex, str);

	if (m_ListSelIndex >= 0)
	{
		m_ListControl.SetItemState(m_ListSelIndex, 0, -1);
		m_ListControl.SetItem(newInsIndex, 0, LVIF_STATE, NULL, 0, LVIS_SELECTED, LVIS_SELECTED, 0);
		m_selInsIndex = newInsIndex;
		m_ListSelIndex = newInsIndex;
		pDoc->modelEditor->SetStateAddMesh();
		((CButton *)GetDlgItem(IDC_RADIO1))->SetCheck(TRUE);
		((CButton *)GetDlgItem(IDC_RADIO2))->SetCheck(FALSE);
		((CButton *)GetDlgItem(IDC_RADIO3))->SetCheck(FALSE);
		((CButton *)GetDlgItem(IDC_RADIO15))->SetCheck(FALSE);
		UpdateData(FALSE);
	}
	else
	{
		m_ListControl.SetItem(newInsIndex, 0, LVIF_STATE, NULL, 0, LVIS_SELECTED, LVIS_SELECTED, 0);
		m_selInsIndex = newInsIndex;
		m_ListSelIndex = newInsIndex;
		pDoc->modelEditor->SetStateAddMesh();
		((CButton *)GetDlgItem(IDC_RADIO1))->SetCheck(TRUE);
		((CButton *)GetDlgItem(IDC_RADIO2))->SetCheck(FALSE);
		((CButton *)GetDlgItem(IDC_RADIO3))->SetCheck(FALSE);
		((CButton *)GetDlgItem(IDC_RADIO15))->SetCheck(FALSE);
		UpdateData(FALSE);
	}
}

//ѡ����Ƭ
void CControlView::OnBnClickedRadio1()
{
	pDoc->modelEditor->SetStateRender();
}

//������Ƭ
void CControlView::OnBnClickedRadio2()
{
	pDoc->modelEditor->SetStateAddMesh();
}

//ɾ����Ƭ
void CControlView::OnBnClickedRadio3()
{
	pDoc->modelEditor->SetStateDeleteMesh();
}

//��������
void CControlView::OnBnClickedRadio13()
{
	pDoc->modelEditor->polygonPoints_2D.clear();
	pDoc->modelEditor->polygonPoints_3D.clear();
	pDoc->modelEditor->SetStateDeleteMesh_Batch();
}

//����ɾ��
void CControlView::OnBnClickedRadio14()
{
	pDoc->modelEditor->polygonPoints_2D.clear();
	pDoc->modelEditor->polygonPoints_3D.clear();
	pDoc->modelEditor->SetStateDeleteMesh_Batch();
}

//������ʵ��
void CControlView::OnBnClickedBtnCreateNewIns()
{
#if 0
	if (pDoc->modelEditor->annotation == NULL)
	{
		std::cout << "���ȼ��ر�ע�ļ�" << endl;
		return;
	}

	pDoc->modelEditor->polygonPoints_2D.clear();
	pDoc->modelEditor->polygonPoints_3D.clear();

	bool isOk = pDoc->modelEditor->annotation->AddNewInstance();
	if (isOk)
	{
		pDoc->modelEditor->selInsIndex = pDoc->modelEditor->annotation->numInstance - 1;
		AddNewListItem();
		pDoc->modelEditor->SetStateEdit();
		SetRadioEdit();
	}
#endif // ������½�ʵ��

	if (pDoc->modelEditor->model == nullptr)
	{
		std::cout << "���ȼ���ģ��" << endl;
		return;
	}

	vector<string> cateNames;
	vector<int> cateIds;
	for (size_t i = 0; i < pDoc->modelEditor->allCategory.size(); i++)
	{
		cateNames.push_back(pDoc->modelEditor->allCategory[i].categoryName);
		cateIds.push_back(pDoc->modelEditor->allCategory[i].categoryId);
	}
	SelCategoryDlg  dlg;
	dlg.SetLsit(cateNames, cateIds);
	if (dlg.DoModal() != IDOK)
		return;

	string cateName = dlg.selCatName;
	int cateId = dlg.selCatId;
	std::cout << "cateName: " << cateName << endl;
	std::cout << "cateId: " << cateId << endl;

	if (cateId >=0)
	{
		//����m_treeControl�ؼ�
		HTREEITEM hRoot = TVI_ROOT;
		HTREEITEM hChildItem = m_treeControl.GetChildItem(hRoot);
		while (hChildItem != NULL)
		{
			int nDat = m_treeControl.GetItemData(hChildItem);
			CString nText = m_treeControl.GetItemText(hChildItem);
			string nText_str = CString2string(nText);
			if (nDat == cateId && nText_str == cateName)
			{
				if (this->pDoc->modelEditor->ifEdited && pDoc->modelEditor->selIns[0] >= 0 && pDoc->modelEditor->selIns[1] >= 0)
				{
					this->pDoc->modelEditor->labelInfo->CalcSingleBBox(pDoc->modelEditor->selIns[0], pDoc->modelEditor->selIns[1]);
					this->pDoc->modelEditor->ifEdited = false;
				}

				//����pDoc->modelEditor->labelInfo
				int instanceId = pDoc->modelEditor->labelInfo->AddNewInstance(cateId);
				if (instanceId < 0)
				{
					std::cout << "����ʧ�ܣ�stuff��ֻ�ܰ���һ��ʵ��" << endl;
					return;
				}

				TVINSERTSTRUCT ts_ins = { 0 };
				ts_ins.hParent = hChildItem;
				ts_ins.hInsertAfter = TVI_LAST;
				wchar_t i_str[10];
				_itow_s(instanceId, i_str, 10);
				ts_ins.item.pszText = i_str;
				ts_ins.item.mask = TVIF_TEXT;
				HTREEITEM insItem = m_treeControl.InsertItem(&ts_ins);
				m_treeControl.SetItemData(insItem, instanceId);
				m_treeControl.SelectItem(insItem);

				pDoc->modelEditor->selIns = Vector2i(cateId, instanceId);
				m_selCateName = nText;
				m_selInsIndex = instanceId;

				((CButton *)GetDlgItem(IDC_RADIO1))->SetCheck(FALSE);
				((CButton *)GetDlgItem(IDC_RADIO2))->SetCheck(FALSE);
				((CButton *)GetDlgItem(IDC_RADIO3))->SetCheck(FALSE);
				((CButton *)GetDlgItem(IDC_RADIO15))->SetCheck(TRUE);
				pDoc->modelEditor->SetStateEdit();

				UpdateData(FALSE);

				break;
			}
			hChildItem = m_treeControl.GetNextItem(hChildItem, TVGN_NEXT);
		}
	}
	else
	{
		////����pDoc->modelEditor->labelInfo
		//int cateId = pDoc->modelEditor->labelInfo->AddNewCategory(cateName);
		//int insId = pDoc->modelEditor->labelInfo->AddNewInstance(cateId);

		////����m_treeControl�ؼ�
		////1.�ȴ������
		//HTREEITEM hRoot = TVI_ROOT;
		//TVINSERTSTRUCT ts_cate = { 0 };
		//ts_cate.hParent = hRoot;
		//ts_cate.hInsertAfter = TVI_LAST;
		//wstring widstr;
		//widstr = std::wstring(cateName.begin(), cateName.end());
		//ts_cate.item.pszText = (LPWSTR)widstr.c_str();
		//ts_cate.item.mask = TVIF_TEXT;
		//HTREEITEM cateItem = m_treeControl.InsertItem(&ts_cate);
		//m_treeControl.SetItemData(cateItem, cateId);

		////2.���ڸ�������½�ʵ��
		//TVINSERTSTRUCT ts_ins = { 0 };
		//ts_ins.hParent = cateItem;
		//ts_ins.hInsertAfter = TVI_LAST;
		//wchar_t i_str[10];
		//_itow_s(insId, i_str, 10);
		//ts_ins.item.pszText = i_str;
		//ts_ins.item.mask = TVIF_TEXT;
		//HTREEITEM insItem = m_treeControl.InsertItem(&ts_ins);
		//m_treeControl.SetItemData(insItem, insId);
		//m_treeControl.SelectItem(insItem);

		//pDoc->modelEditor->selIns = Vector2i(cateId, insId);
		//m_selCateName = (LPWSTR)widstr.c_str();
		//m_selInsIndex = insId;

		//((CButton *)GetDlgItem(IDC_RADIO1))->SetCheck(FALSE);
		//((CButton *)GetDlgItem(IDC_RADIO2))->SetCheck(FALSE);
		//((CButton *)GetDlgItem(IDC_RADIO3))->SetCheck(FALSE);
		//((CButton *)GetDlgItem(IDC_RADIO15))->SetCheck(TRUE);
		//pDoc->modelEditor->SetStateEdit();

		//UpdateData(FALSE);
	}
}

//�����߿�
void CControlView::OnBnClickedRadio4()
{
	pDoc->modelEditor->ifDrawLine = true;
	pDoc->modelEditor->RenderScene();
}

//�������߿�
void CControlView::OnBnClickedRadio5()
{
	pDoc->modelEditor->ifDrawLine = false;
	pDoc->modelEditor->RenderScene();
}

//�����ı����ĸ����㴴����ʵ��
void CControlView::OnBnClickedBtnCreateNewInsRec()
{
	pDoc->modelEditor->SetStateAddNewInsRec();
}

//��
void CControlView::OnBnClickedRadio6()
{
	//����ѡ��ʵ����Χ������camera�ӽ�
	if (pDoc->modelEditor->selInsIndex > 0)
	{
		pDoc->modelEditor->SetGLCameraWithInsBox(pDoc->modelEditor->selInsIndex, View_Down);
		pDoc->modelEditor->RenderScene();
	}
}

//ǰ
void CControlView::OnBnClickedRadio7()
{
	//����ѡ��ʵ����Χ������camera�ӽ�
	if (pDoc->modelEditor->selInsIndex > 0)
	{
		pDoc->modelEditor->SetGLCameraWithInsBox(pDoc->modelEditor->selInsIndex, View_Front);
		pDoc->modelEditor->RenderScene();
	}
}

//��
void CControlView::OnBnClickedRadio8()
{
	//����ѡ��ʵ����Χ������camera�ӽ�
	if (pDoc->modelEditor->selInsIndex > 0)
	{
		pDoc->modelEditor->SetGLCameraWithInsBox(pDoc->modelEditor->selInsIndex, View_Back);
		pDoc->modelEditor->RenderScene();
	}
}

//��
void CControlView::OnBnClickedRadio9()
{
	//����ѡ��ʵ����Χ������camera�ӽ�
	if (pDoc->modelEditor->selInsIndex > 0)
	{
		pDoc->modelEditor->SetGLCameraWithInsBox(pDoc->modelEditor->selInsIndex, View_Left);
		pDoc->modelEditor->RenderScene();
	}
}

//��
void CControlView::OnBnClickedRadio10()
{
	//����ѡ��ʵ����Χ������camera�ӽ�
	if (pDoc->modelEditor->selInsIndex > 0)
	{
		pDoc->modelEditor->SetGLCameraWithInsBox(pDoc->modelEditor->selInsIndex, View_Right);
		pDoc->modelEditor->RenderScene();
	}
}

//ʵ����Ⱦ
void CControlView::OnBnClickedRadio11()
{
	pDoc->modelEditor->drawType = DrawType::DrawColor;
	pDoc->modelEditor->RenderScene();
}

//������Ⱦ
void CControlView::OnBnClickedRadio12()
{
	pDoc->modelEditor->drawType = DrawType::DrawTexture;
	pDoc->modelEditor->RenderScene();
}

//�����ں�
void CControlView::OnBnClickedRadio19()
{
	pDoc->modelEditor->drawType = DrawType::DrawTextureColor;
	pDoc->modelEditor->RenderScene();
}

//�б��Ҽ�"��ȡ������"��
void CControlView::OnGetBackFaces()
{
	int i, iState;
	int nItemSelected = m_ListControl.GetSelectedCount();
	int nItemCount = m_ListControl.GetItemCount();
	if (nItemSelected<1)
		return;
	for (i = nItemCount - 1;i >= 0;i--)
	{
		iState = m_ListControl.GetItemState(i, LVIS_SELECTED);
		if (iState != 0)
		{
			CString str;
			str = m_ListControl.GetItemText(i, 0);//��ȡָ���У�Ϊ�ַ�����ʽ
			int insIndex = atoi(CT2A(str.GetBuffer()));

			pDoc->modelEditor->MergeBackForSingleIns(insIndex);
			pDoc->modelEditor->ifEdited = true;
			pDoc->modelEditor->RenderScene();
		}
	}
}

//�б��Ҽ�"ȥ������"�¼�
void CControlView::OnRemoveNoise()
{
	int i, iState;
	int nItemSelected = m_ListControl.GetSelectedCount();
	int nItemCount = m_ListControl.GetItemCount();
	if (nItemSelected<1)
		return;
	for (i = nItemCount - 1;i >= 0;i--)
	{
		iState = m_ListControl.GetItemState(i, LVIS_SELECTED);
		if (iState != 0)
		{
			CString str;
			str = m_ListControl.GetItemText(i, 0);//��ȡָ���У�Ϊ�ַ�����ʽ
			int insIndex = atoi(CT2A(str.GetBuffer()));

			pDoc->modelEditor->ClearNoiseForSingleIns(insIndex);
			pDoc->modelEditor->ifEdited = true;
			pDoc->modelEditor->RenderScene();
		}
	}
}

//�б��Ҽ�"��ȡ�ݶ�"�¼�
void CControlView::OnGetRoof()
{
	int i, iState;
	int nItemSelected = m_ListControl.GetSelectedCount();
	int nItemCount = m_ListControl.GetItemCount();
	if (nItemSelected<1)
		return;
	for (i = nItemCount - 1;i >= 0;i--)
	{
		iState = m_ListControl.GetItemState(i, LVIS_SELECTED);
		if (iState != 0)
		{
			CString str;
			str = m_ListControl.GetItemText(i, 0);//��ȡָ���У�Ϊ�ַ�����ʽ
			int insIndex = atoi(CT2A(str.GetBuffer()));

			InputNumDlg  Dlg;
			if (Dlg.DoModal() != IDOK)
				return;
			int roofNum = Dlg.roofNum;

			pDoc->modelEditor->GetRoofForSingleIns(insIndex, roofNum);
			pDoc->modelEditor->ifEdited = true;
			pDoc->modelEditor->RenderScene();
		}
	}
}

//�༭
void CControlView::OnBnClickedRadio15()
{
	pDoc->modelEditor->SetStateEdit();
}

//���õ�ѡ��ѡ�С�
void CControlView::SetRadioSelect()
{
	((CButton *)GetDlgItem(IDC_RADIO1))->SetCheck(TRUE);
	((CButton *)GetDlgItem(IDC_RADIO2))->SetCheck(FALSE);
	((CButton *)GetDlgItem(IDC_RADIO3))->SetCheck(FALSE);
	((CButton *)GetDlgItem(IDC_RADIO15))->SetCheck(FALSE);
	UpdateData(FALSE);
}

//���õ�ѡ�򡰱༭��
void CControlView::SetRadioEdit()
{
	((CButton *)GetDlgItem(IDC_RADIO1))->SetCheck(FALSE);
	((CButton *)GetDlgItem(IDC_RADIO2))->SetCheck(FALSE);
	((CButton *)GetDlgItem(IDC_RADIO3))->SetCheck(FALSE);
	((CButton *)GetDlgItem(IDC_RADIO15))->SetCheck(TRUE);
	UpdateData(FALSE);
}

//���õ�ѡ�������ת��ʽѡ��״̬
void CControlView::SetRadioPivotPointType(PivotPointType ppt)
{
	if (ppt == PP_ModelCenter)
	{
		((CButton *)GetDlgItem(IDC_RADIO21))->SetCheck(TRUE);
		((CButton *)GetDlgItem(IDC_RADIO22))->SetCheck(FALSE);
		((CButton *)GetDlgItem(IDC_RADIO23))->SetCheck(FALSE);
	}
	else if (ppt == PP_InsCenter)
	{
		((CButton *)GetDlgItem(IDC_RADIO21))->SetCheck(FALSE);
		((CButton *)GetDlgItem(IDC_RADIO22))->SetCheck(TRUE);
		((CButton *)GetDlgItem(IDC_RADIO23))->SetCheck(FALSE);
	}
	else
	{
		((CButton *)GetDlgItem(IDC_RADIO21))->SetCheck(FALSE);
		((CButton *)GetDlgItem(IDC_RADIO22))->SetCheck(FALSE);
		((CButton *)GetDlgItem(IDC_RADIO23))->SetCheck(TRUE);
	}
	UpdateData(FALSE);
}

//����ʵ���б��¼�
void CControlView::OnNMClickList1(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	*pResult = 0;

	for (int i = 0; i < m_ListControl.GetItemCount(); i++)
	{
		if (m_ListControl.GetItemState(i, LVIS_SELECTED) == LVIS_SELECTED)
		{
			if (m_ListSelIndex != i)
			{
				CString str;
				str = m_ListControl.GetItemText(i, 0);//��ȡָ���У�Ϊ�ַ�����ʽ
				int insIndex = atoi(CT2A(str.GetBuffer()));

				CLabel3DDoc* m_pDoc = (CLabel3DDoc*)GetDocument();

				if (this->pDoc->modelEditor->ifEdited)
				{
					this->pDoc->modelEditor->CalcInsBBox(this->pDoc->modelEditor->selInsIndex);
					this->pDoc->modelEditor->ifEdited = false;
				}

				m_pDoc->modelEditor->selInsIndex = insIndex;

				m_ListSelIndex = i;
				m_selInsIndex = i;
				std::cout << "ѡ�е� " << insIndex << " ��ʵ��" << "" << endl;

				//����ѡ��ʵ����Χ������camera�ӽ�
				//if (this->pDoc->modelEditor->editState == Edit_Render)
				//{
				//	m_pDoc->modelEditor->SetGLCameraWithInsBox(insIndex, View_Down);
				//}
				((CButton *)GetDlgItem(IDC_RADIO1))->SetCheck(FALSE);
				((CButton *)GetDlgItem(IDC_RADIO2))->SetCheck(FALSE);
				((CButton *)GetDlgItem(IDC_RADIO3))->SetCheck(FALSE);
				((CButton *)GetDlgItem(IDC_RADIO15))->SetCheck(TRUE);
				m_pDoc->modelEditor->SetStateEdit();

				//m_pDoc->modelEditor->SetGLCameraWithInsBox(insIndex, View_Down);
				m_pDoc->modelEditor->SetCameraWithInsBox(insIndex, View_Down);
				m_pDoc->modelEditor->RenderScene();

				UpdateData(FALSE);
			}
		}
	}
}

//�б��Ҽ�"�������"�¼�
void CControlView::OnListCalcArea()
{
	int i, iState;
	int nItemSelected = m_ListControl.GetSelectedCount();
	int nItemCount = m_ListControl.GetItemCount();
	if (nItemSelected<1)
		return;
	for (i = nItemCount - 1;i >= 0;i--)
	{
		iState = m_ListControl.GetItemState(i, LVIS_SELECTED);
		if (iState != 0)
		{
			CString str;
			str = m_ListControl.GetItemText(i, 0);//��ȡָ���У�Ϊ�ַ�����ʽ
			int insIndex = atoi(CT2A(str.GetBuffer()));

			double area = pDoc->modelEditor->CalcInsArea(insIndex);

			std::cout << "Area = " << area << endl;
		}
	}
}

//�����λ
void CControlView::OnBnClickedResetCamera()
{
	pDoc->modelEditor->myGlsl->ResetCamera();
	pDoc->modelEditor->RenderScene();
}

//��ѡ
void CControlView::OnBnClickedRadio16()
{
	pDoc->modelEditor->SetPickType(Pick_Point);
}

//��ˢ
void CControlView::OnBnClickedRadio17()
{
	pDoc->modelEditor->SetPickType(Pick_Circle);
}

//���ο�
void CControlView::OnBnClickedRadio18()
{
	pDoc->modelEditor->SetPickType(Pick_Rect);
}

//�������Id��ʵ��Idѡ�����οؼ���
bool CControlView::SelectTreeItem(int cateId, int insId)
{
	HTREEITEM hRoot = m_treeControl.GetRootItem();
	while ((hRoot != NULL))
	{
		int cateId_item = m_treeControl.GetItemData(hRoot);
		if (cateId_item == cateId)
		{
			CString cateName_item = m_treeControl.GetItemText(hRoot);
			HTREEITEM insNode = m_treeControl.GetChildItem(hRoot);
			while ((insNode != NULL))
			{
				int insId_item = m_treeControl.GetItemData(insNode);
				if (insId_item == insId)
				{
					m_treeControl.SelectItem(insNode);
					m_selCateName = cateName_item;
					m_selInsIndex = insId;
					UpdateData(FALSE);
					return true;
				}
				else
				{
					insNode = m_treeControl.GetNextSiblingItem(insNode);
				}
			}
		}
		else
		{
			hRoot = m_treeControl.GetNextSiblingItem(hRoot);
		}
	}

	return false;
}

//���±�ע���οؼ�
bool CControlView::UpdateTreeControl()
{
	CLabel3DDoc* m_pDoc = (CLabel3DDoc*)GetDocument();

	m_treeControl.DeleteAllItems();
	int numCategories = m_pDoc->modelEditor->labelInfo->categories.size();
	for (int ci = 0; ci < numCategories; ci++)
	{
		HTREEITEM hRoot = TVI_ROOT;     // ���ĸ��ڵ�ľ��   
		TVINSERTSTRUCT ts = { 0 };
		ts.hParent = hRoot;
		ts.hInsertAfter = TVI_LAST;

		int categoryId = m_pDoc->modelEditor->labelInfo->categories[ci].categoryId;
		string categoryName = m_pDoc->modelEditor->labelInfo->categories[ci].categoryName;
		wstring widstr;
		widstr = std::wstring(categoryName.begin(), categoryName.end());
		ts.item.pszText = (LPWSTR)widstr.c_str();
		ts.item.mask = TVIF_TEXT;
		HTREEITEM cateItem = m_treeControl.InsertItem(&ts);
		m_treeControl.SetItemData(cateItem, categoryId);
		m_treeControl.Expand(cateItem, TVE_EXPAND);

		int numInstace = m_pDoc->modelEditor->labelInfo->categories[ci].instances.size();
		for (int ii = 0; ii < numInstace; ii++)
		{
			int instanceId = m_pDoc->modelEditor->labelInfo->categories[ci].instances[ii].instanceId;
			TVINSERTSTRUCT ts_ins = { 0 };
			ts_ins.hParent = cateItem;
			ts_ins.hInsertAfter = TVI_LAST;
			wchar_t i_str[10];
			_itow_s(instanceId, i_str, 10);
			ts_ins.item.pszText = i_str;
			ts_ins.item.mask = TVIF_TEXT;
			HTREEITEM insItem = m_treeControl.InsertItem(&ts_ins);
			m_treeControl.SetItemData(insItem, instanceId);
			m_treeControl.Expand(insItem, TVE_EXPAND);
		}
	}

	return true;
}

//������οؼ�ѡ����
void CControlView::OnNMClickTree1(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CPoint pt = GetCurrentMessage()->pt;
	m_treeControl.ScreenToClient(&pt);
	UINT uFlags = 0;
	HTREEITEM hItem = m_treeControl.HitTest(pt, &uFlags);
	if ((hItem != NULL) && (TVHT_ONITEM & uFlags))
	{
		if (this->pDoc->modelEditor->ifEdited && pDoc->modelEditor->selIns[0] >=0 && pDoc->modelEditor->selIns[1] >= 0)
		{
			this->pDoc->modelEditor->labelInfo->CalcSingleBBox(pDoc->modelEditor->selIns[0], pDoc->modelEditor->selIns[1]);
			this->pDoc->modelEditor->ifEdited = false;
		}

		m_treeControl.SelectItem(hItem);
		HTREEITEM parent = m_treeControl.GetParentItem(hItem);
		if (parent == NULL)
		{
			int nDat = m_treeControl.GetItemData(hItem);
			CString nText = m_treeControl.GetItemText(hItem);
			std::cout << "ѡ�е� " << nDat << " �����" << CString2string(nText) << endl;
			pDoc->modelEditor->selIns[0] = -1;
			pDoc->modelEditor->selIns[1] = -1;

			((CButton *)GetDlgItem(IDC_RADIO1))->SetCheck(TRUE);
			((CButton *)GetDlgItem(IDC_RADIO2))->SetCheck(FALSE);
			((CButton *)GetDlgItem(IDC_RADIO3))->SetCheck(FALSE);
			((CButton *)GetDlgItem(IDC_RADIO15))->SetCheck(FALSE);
			pDoc->modelEditor->SetStateRender();
			m_selCateName = nText;
			m_selInsIndex = -1;
			UpdateData(FALSE);
		}
		else
		{
			int pDat = m_treeControl.GetItemData(parent);
			CString pText = m_treeControl.GetItemText(parent);
			int nDat = m_treeControl.GetItemData(hItem);
			std::cout << "ѡ�е� " << pDat << " �����[" << CString2string(pText) << "], �� " << nDat << " ��ʵ��" << endl;

			pDoc->modelEditor->selIns[0] = pDat;
			pDoc->modelEditor->selIns[1] = nDat;

			((CButton *)GetDlgItem(IDC_RADIO1))->SetCheck(FALSE);
			((CButton *)GetDlgItem(IDC_RADIO2))->SetCheck(FALSE);
			((CButton *)GetDlgItem(IDC_RADIO3))->SetCheck(FALSE);
			((CButton *)GetDlgItem(IDC_RADIO15))->SetCheck(TRUE);
			pDoc->modelEditor->SetStateEdit();

			//m_pDoc->modelEditor->SetGLCameraWithInsBox(insIndex, View_Down);
			//pDoc->modelEditor->SetCameraWithInsBox(pDat, nDat, View_Down);

			m_selCateName = pText;
			m_selInsIndex = nDat;
			UpdateData(FALSE);

			if (this->pDoc->modelEditor->labelInfo->CheckCateIsthing(pDoc->modelEditor->selIns[0]))
			{
				this->pDoc->modelEditor->SetCameraWithInsBox(pDoc->modelEditor->selIns[0], pDoc->modelEditor->selIns[1], View_Down);
				this->pDoc->modelEditor->SetCameraPivotPoint(PP_InsCenter);
				SetRadioPivotPointType(PP_InsCenter);
			}

			pDoc->modelEditor->RenderScene();
		}
	}
	*pResult = 0;
}

//���οؼ��Ҽ���������˵�
void CControlView::OnNMRClickTree1(NMHDR *pNMHDR, LRESULT *pResult)
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CPoint pt = GetCurrentMessage()->pt;
	m_treeControl.ScreenToClient(&pt);
	UINT uFlags = 0;
	HTREEITEM hItem = m_treeControl.HitTest(pt, &uFlags);
	if ((hItem != NULL) && (TVHT_ONITEM & uFlags))
	{
		m_treeControl.SelectItem(hItem);
		HTREEITEM parent = m_treeControl.GetParentItem(hItem);
		if (parent != NULL)
		{
			DWORD dwPos = GetMessagePos();
			CPoint point(LOWORD(dwPos), HIWORD(dwPos));

			CMenu menu;
			VERIFY(menu.LoadMenu(IDR_MENU2));

			CMenu* popup = menu.GetSubMenu(0);
			ASSERT(popup != NULL);
			popup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this);
		}
		else
		{
			DWORD dwPos = GetMessagePos();
			CPoint point(LOWORD(dwPos), HIWORD(dwPos));

			CMenu menu;
			VERIFY(menu.LoadMenu(IDR_MENU_NewInstance));

			CMenu* popup = menu.GetSubMenu(0);
			ASSERT(popup != NULL);
			popup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, this);
		}
	}

	*pResult = 0;
}

//���οؼ��Ҽ�"ɾ��"�¼�
void CControlView::OnTreeDelete()
{
	// TODO: �ڴ���������������
	HTREEITEM hItem = m_treeControl.GetSelectedItem();
	if ((hItem != NULL))
	{
		HTREEITEM parent = m_treeControl.GetParentItem(hItem);
		if (parent == NULL)
		{
			//int nDat = m_treeControl.GetItemData(hItem);
			//CString nText = m_treeControl.GetItemText(hItem);
			//std::cout << "ѡ�е� " << nDat << " �����" << CString2string(nText) << endl;

			//int result = MessageBox(TEXT("ȷ��ɾ��������µ�����ʵ����"), TEXT("ɾ����𾯸�!"), MB_YESNO);

			//switch (result)
			//{
			//	//ɾ������Լ��������ʵ��
			//	case IDYES:
			//		pDoc->modelEditor->DeleteCategory(nDat);
			//		pDoc->modelEditor->RenderScene();
			//		m_treeControl.DeleteItem(hItem);
			//		pDoc->modelEditor->selIns = Vector2i(-1, -1);
			//		m_selCateName = "";
			//		m_selInsIndex = -1;
			//		((CButton *)GetDlgItem(IDC_RADIO1))->SetCheck(TRUE);
			//		((CButton *)GetDlgItem(IDC_RADIO2))->SetCheck(FALSE);
			//		((CButton *)GetDlgItem(IDC_RADIO3))->SetCheck(FALSE);
			//		((CButton *)GetDlgItem(IDC_RADIO15))->SetCheck(FALSE);
			//		pDoc->modelEditor->SetStateRender();
			//		UpdateData(FALSE);
			//}
		}
		else
		{
			int pDat = m_treeControl.GetItemData(parent);
			CString pText = m_treeControl.GetItemText(hItem);
			int nDat = m_treeControl.GetItemData(hItem);
			std::cout << "ѡ�е� " << pDat << " �����[" << CString2string(pText) << "], �� " << nDat << " ��ʵ��" << endl;
			//ɾ��ʵ��
			pDoc->modelEditor->DeleteInstance(pDat, nDat);
			pDoc->modelEditor->RenderScene();
			m_treeControl.DeleteItem(hItem);

			pDoc->modelEditor->selIns = Vector2i(-1, -1);
			m_selCateName = "";
			m_selInsIndex = -1;
			((CButton *)GetDlgItem(IDC_RADIO1))->SetCheck(TRUE);
			((CButton *)GetDlgItem(IDC_RADIO2))->SetCheck(FALSE);
			((CButton *)GetDlgItem(IDC_RADIO3))->SetCheck(FALSE);
			((CButton *)GetDlgItem(IDC_RADIO15))->SetCheck(FALSE);
			pDoc->modelEditor->SetStateRender();
			UpdateData(FALSE);
		}
	}
}

//���οؼ��Ҽ�"��ȡ����"�¼�
void CControlView::OnTreeGetBack()
{
	// TODO: �ڴ���������������
	HTREEITEM hItem = m_treeControl.GetSelectedItem();
	if ((hItem != NULL))
	{
		HTREEITEM parent = m_treeControl.GetParentItem(hItem);
		if (parent != NULL)
		{
			int cateId = m_treeControl.GetItemData(parent);
			CString cateName = m_treeControl.GetItemText(hItem);
			int insId = m_treeControl.GetItemData(hItem);
			std::cout << "ѡ�е� " << cateId << " �����[" << CString2string(cateName) << "], �� " << insId << " ��ʵ��" << endl;

			//if (this->pDoc->modelEditor->labelInfo->CheckCateIsthing(cateId))
			//{
			//	pDoc->modelEditor->MergeBackForSingleIns(cateId, insId);
			//	pDoc->modelEditor->ifEdited = true;
			//}

			pDoc->modelEditor->MergeBackForSingleIns(cateId, insId);
			pDoc->modelEditor->ifEdited = true;
			std::cout << "��ȡ�������" << endl << endl;
			pDoc->modelEditor->RenderScene();

		}
	}
}

//���οؼ��Ҽ�"ȥ������"�¼�
void CControlView::OnTreeRemoveNoise()
{
	// TODO: �ڴ���������������
	HTREEITEM hItem = m_treeControl.GetSelectedItem();
	if ((hItem != NULL))
	{
		HTREEITEM parent = m_treeControl.GetParentItem(hItem);
		if (parent != NULL)
		{
			int cateId = m_treeControl.GetItemData(parent);
			CString cateName = m_treeControl.GetItemText(hItem);
			int insId = m_treeControl.GetItemData(hItem);
			std::cout << "ѡ�е� " << cateId << " �����[" << CString2string(cateName) << "], �� " << insId << " ��ʵ��" << endl;
			//if (this->pDoc->modelEditor->labelInfo->CheckCateIsthing(cateId))
			//{
			//	pDoc->modelEditor->ClearNoiseForSingleIns(cateId, insId);
			//	pDoc->modelEditor->ifEdited = true;
			//}
			pDoc->modelEditor->ClearNoiseForSingleIns(cateId, insId);
			pDoc->modelEditor->ifEdited = true;
			std::cout << "ȥ���������" << endl << endl;
			pDoc->modelEditor->RenderScene();
		}
	}
}

//���οؼ��Ҽ�"ͼ��"�¼�
void CControlView::OnGraphCut()
{
	// TODO: �ڴ���������������
	HTREEITEM hItem = m_treeControl.GetSelectedItem();
	if ((hItem != NULL))
	{
		HTREEITEM parent = m_treeControl.GetParentItem(hItem);
		if (parent != NULL)
		{
			int cateId = m_treeControl.GetItemData(parent);
			CString cateName = m_treeControl.GetItemText(parent);
			int insId = m_treeControl.GetItemData(hItem);
			std::cout << "ѡ�е� " << cateId << " �����[" << CString2string(cateName) << "], �� " << insId << " ��ʵ��" << endl;
			bool isOk = false;
			if (cateId == 0 && cateName == "building")
			{
				isOk = pDoc->modelEditor->GraphCutForSingleIns(cateId, insId);
			}
			else if (cateId == 2 && cateName == "plant")
			{
				isOk = pDoc->modelEditor->PickPlantFaces(cateId, insId);
			}
			else if (cateId == 9 && cateName == "road")
			{
				isOk = pDoc->modelEditor->PickPlantFaces(cateId, insId);
			}
			if (isOk)
			{
				pDoc->modelEditor->ifEdited = true;
				pDoc->modelEditor->RenderScene();
			}
		}
	}
}

//���οؼ��Ҽ�"�½�ʵ��"�¼�
void CControlView::OnTreeNewInstance()
{
	// TODO: �ڴ���������������
	HTREEITEM hItem = m_treeControl.GetSelectedItem();
	if ((hItem != NULL))
	{
		HTREEITEM parent = m_treeControl.GetParentItem(hItem);
		if (parent == NULL)
		{
			if (this->pDoc->modelEditor->ifEdited && pDoc->modelEditor->selIns[0] >= 0 && pDoc->modelEditor->selIns[1] >= 0)
			{
				this->pDoc->modelEditor->labelInfo->CalcSingleBBox(pDoc->modelEditor->selIns[0], pDoc->modelEditor->selIns[1]);
				this->pDoc->modelEditor->ifEdited = false;
			}

			int cateId = m_treeControl.GetItemData(hItem);
			CString cateName = m_treeControl.GetItemText(hItem);
			std::cout << "ѡ�е� " << cateId << " �����[" << CString2string(cateName) << "] " << endl;
			//����pDoc->modelEditor->labelInfo
			int instanceId = pDoc->modelEditor->labelInfo->AddNewInstance(cateId);
			if (instanceId < 0)
			{
				std::cout << "����ʧ�ܣ�stuff��ֻ�ܰ���һ��ʵ��" << endl;
				return;
			}

			TVINSERTSTRUCT ts_ins = { 0 };
			ts_ins.hParent = hItem;
			ts_ins.hInsertAfter = TVI_LAST;
			wchar_t i_str[10];
			_itow_s(instanceId, i_str, 10);
			ts_ins.item.pszText = i_str;
			ts_ins.item.mask = TVIF_TEXT;
			HTREEITEM insItem = m_treeControl.InsertItem(&ts_ins);
			m_treeControl.SetItemData(insItem, instanceId);
			m_treeControl.SelectItem(insItem);

			pDoc->modelEditor->selIns = Vector2i(cateId, instanceId);
			m_selCateName = cateName;
			m_selInsIndex = instanceId;

			((CButton *)GetDlgItem(IDC_RADIO1))->SetCheck(FALSE);
			((CButton *)GetDlgItem(IDC_RADIO2))->SetCheck(FALSE);
			((CButton *)GetDlgItem(IDC_RADIO3))->SetCheck(FALSE);
			((CButton *)GetDlgItem(IDC_RADIO15))->SetCheck(TRUE);
			pDoc->modelEditor->SetStateEdit();

			UpdateData(FALSE);
		}
	}
}

//��ģ�����ĵ�
void CControlView::OnBnClickedRadio21()
{
	bool isOk = pDoc->modelEditor->SetCameraPivotPoint(PP_ModelCenter);
}

//��ʵ�����ĵ�
void CControlView::OnBnClickedRadio22()
{
	bool isOk = pDoc->modelEditor->SetCameraPivotPoint(PP_InsCenter);
}

//����Ļ���ĵ�
void CControlView::OnBnClickedRadio23()
{
	bool isOk = pDoc->modelEditor->SetCameraPivotPoint(PP_ScreenCenter);
}

//���ñ�עʱ�޸��ѱ�ע��Ƭ
void CControlView::OnBnClickedRadio24()
{
	pDoc->modelEditor->changeLabeledFace = true;
}

//���ñ�עʱ���޸��ѱ�ע��Ƭ
void CControlView::OnBnClickedRadio25()
{
	pDoc->modelEditor->changeLabeledFace = false;
}
