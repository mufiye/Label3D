// InputNumDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Label3D.h"
#include "InputNumDlg.h"
#include "afxdialogex.h"


// InputNumDlg 对话框

IMPLEMENT_DYNAMIC(InputNumDlg, CDialogEx)

InputNumDlg::InputNumDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_INPUTNUM, pParent)
	, roofNum(0)
{

}

InputNumDlg::~InputNumDlg()
{
}

void InputNumDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDIT1, roofNum);
	DDV_MinMaxInt(pDX, roofNum, 1, 10);
}


BEGIN_MESSAGE_MAP(InputNumDlg, CDialogEx)
	ON_BN_CLICKED(IDOK, &InputNumDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// InputNumDlg 消息处理程序


BOOL InputNumDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  在此添加额外的初始化
	roofNum = 1;
	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // 异常: OCX 属性页应返回 FALSE
}


void InputNumDlg::OnBnClickedOk()
{
	// TODO: 在此添加控件通知处理程序代码
	UpdateData(TRUE);
	CDialogEx::OnOK();
}
