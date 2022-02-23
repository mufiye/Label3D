#pragma once


// InputNumDlg 对话框

class InputNumDlg : public CDialogEx
{
	DECLARE_DYNAMIC(InputNumDlg)

public:
	InputNumDlg(CWnd* pParent = NULL);   // 标准构造函数
	virtual ~InputNumDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_INPUTNUM };
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()
public:
	int roofNum;
	virtual BOOL OnInitDialog();
	afx_msg void OnBnClickedOk();
};
