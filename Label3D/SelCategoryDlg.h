#pragma once
#include "afxcmn.h"
#include "resource.h"
#include <vector>
#include <string>

using namespace std;

// SelCategoryDlg �Ի���

class SelCategoryDlg : public CDialogEx
{
	DECLARE_DYNAMIC(SelCategoryDlg)

public:
	SelCategoryDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~SelCategoryDlg();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_SELCATEGORY };
#endif

public:
	void SetLsit(const vector<string> &catNames, const vector<int> &catIds);

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
private:
	CListCtrl m_list;
	CString m_catName;

public:
	vector<string> catNames;
	vector<int> catIds;
	string selCatName;
	int selCatId;

public:
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
	afx_msg void OnNMClickList3(NMHDR *pNMHDR, LRESULT *pResult);
};
