// InputNumDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Label3D.h"
#include "InputNumDlg.h"
#include "afxdialogex.h"


// InputNumDlg �Ի���

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


// InputNumDlg ��Ϣ�������


BOOL InputNumDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// TODO:  �ڴ���Ӷ���ĳ�ʼ��
	roofNum = 1;
	UpdateData(FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // �쳣: OCX ����ҳӦ���� FALSE
}


void InputNumDlg::OnBnClickedOk()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	UpdateData(TRUE);
	CDialogEx::OnOK();
}
