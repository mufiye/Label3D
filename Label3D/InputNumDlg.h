#pragma once


// InputNumDlg �Ի���

class InputNumDlg : public CDialogEx
{
	DECLARE_DYNAMIC(InputNumDlg)

public:
	InputNumDlg(CWnd* pParent = NULL);   // ��׼���캯��
	virtual ~InputNumDlg();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_INPUTNUM };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()
public:
	int roofNum;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
};
