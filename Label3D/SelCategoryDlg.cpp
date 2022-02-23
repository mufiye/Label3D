// SelCategoryDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Label3D.h"
#include "SelCategoryDlg.h"
#include "afxdialogex.h"


// SelCategoryDlg �Ի���

IMPLEMENT_DYNAMIC(SelCategoryDlg, CDialogEx)

SelCategoryDlg::SelCategoryDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_SELCATEGORY, pParent)
	, m_catName(_T(""))
{

}

SelCategoryDlg::~SelCategoryDlg()
{
}

void SelCategoryDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST3, m_list);
	DDX_Text(pDX, IDC_EDIT1, m_catName);
}


BEGIN_MESSAGE_MAP(SelCategoryDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &SelCategoryDlg::OnBnClickedOk)
	ON_NOTIFY(NM_CLICK, IDC_LIST3, &SelCategoryDlg::OnNMClickList3)
END_MESSAGE_MAP()


// SelCategoryDlg ��Ϣ�������

CString string2CString(std::string input)
{
#ifdef UNICODE
	int len = MultiByteToWideChar(CP_ACP, 0, input.c_str(), -1, NULL, 0);
	wchar_t *wstr = new wchar_t[len];
	memset(wstr, 0, len * sizeof(wchar_t));
	MultiByteToWideChar(CP_ACP, 0, input.c_str(), -1, wstr, len);
	CStringW output = wstr;
	delete[] wstr;

	return output;
#else
	return CString(input.c_str());
#endif // !UNICODE
}

std::string CString2string(CString input)
{
#ifdef UNICODE
	int len = WideCharToMultiByte(CP_ACP, 0, LPCWSTR(input), -1, NULL, 0, NULL, NULL);
	char *str = new char[len];
	memset(str, 0, len);
	WideCharToMultiByte(CP_ACP, 0, LPCWSTR(input), -1, str, len, NULL, NULL);
	std::string output(str);
	delete[] str;
	return output;
#else
	return std::string((LPCSTR)input);
#endif // !UNICODE
}

void SelCategoryDlg::SetLsit(const vector<string> &catNames, const vector<int> &catIds)
{
	// TODO: �ڴ����ר�ô����/����û���
	this->catNames = catNames;
	this->catIds = catIds;
}


BOOL SelCategoryDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	m_list.SetExtendedStyle(m_list.GetExtendedStyle() | LVS_EX_GRIDLINES | LVS_EX_FULLROWSELECT);
	CString head = TEXT("Categories");
	m_list.InsertColumn(0, head, LVCFMT_LEFT, 300);

	for (int i = 0; i < catNames.size(); i++)
	{
		CString str = string2CString(catNames[i]);
		m_list.InsertItem(i, str);
		m_list.SetItemData(i, i);
	}

	return TRUE;  // return TRUE unless you set the focus to a control
				  // �쳣: OCX ����ҳӦ���� FALSE
}


void SelCategoryDlg::OnBnClickedOk()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	UpdateData(TRUE);
	string catName = CString2string(m_catName);
	if (catName=="")
	{
		AfxMessageBox(TEXT("������Ʋ���Ϊ�գ�"));
		return;
	}
	else
	{
		selCatName = catName;
	}

	CDialogEx::OnOK();
}


void SelCategoryDlg::OnNMClickList3(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMITEMACTIVATE pNMItemActivate = reinterpret_cast<LPNMITEMACTIVATE>(pNMHDR);
	// TODO: �ڴ���ӿؼ�֪ͨ����������

	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	for (int i = 0; i < m_list.GetItemCount(); i++)
	{
		if (m_list.GetItemState(i, LVIS_SELECTED) == LVIS_SELECTED)
		{
			CString catName;
			catName = m_list.GetItemText(i, 0);
			selCatId = m_list.GetItemData(i);

			m_catName = catName;
			selCatName = CString2string(catName);
			UpdateData(FALSE);
		}
	}
	*pResult = 0;
}
