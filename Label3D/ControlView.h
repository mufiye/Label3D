#pragma once
#include <iostream>
#include "Label3DDoc.h"
#include "afxwin.h"
#include "resource.h"
#include <tchar.h>
#include "afxcmn.h"
#include "InputNumDlg.h"
#include "SelCategoryDlg.h"

// CControlView ������ͼ

class CControlView : public CFormView
{
	DECLARE_DYNCREATE(CControlView)

protected:
	CControlView();           // ��̬������ʹ�õ��ܱ����Ĺ��캯��
	virtual ~CControlView();

public:
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CONTROLVIEW };
#endif
#ifdef _DEBUG
	virtual void AssertValid() const;
#ifndef _WIN32_WCE
	virtual void Dump(CDumpContext& dc) const;
#endif
#endif

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

	DECLARE_MESSAGE_MAP()

// ����
public:
	CLabel3DDoc* GetDocument() const;

private:
	CSliderCtrl m_scAmbiLightStrength;
	CSliderCtrl m_scDiffLightStrength;
	CSliderCtrl m_scSpecLightStrength;
	CSliderCtrl m_scAmbiMateriStrength;
	CSliderCtrl m_scDiffMateriStrength;
	CSliderCtrl m_scSpecMateriStrength;
	int m_AmbiLightStrength;	//������ǿ��
	int m_DiffLightStrength;	//ɢ���ǿ��
	int m_SpecLightStrength;	//�����ǿ��
	int m_AmbiMateriStrength;	//���ʻ����ⷴ������
	int m_DiffMateriStrength;	//����ɢ��ⷴ������
	int m_SpecMateriStrength;	//���ʾ���ⷴ������

	CSliderCtrl m_scPaintPixel;
	int m_paintPixel;

public:
	CLabel3DDoc* pDoc;	//�ĵ�ָ��
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnBnClickedCheckuselight0();
	afx_msg void OnBnClickedCheckuselight1();
	afx_msg void OnEnChangeEditambilight();
	afx_msg void OnEnChangeEditdifflight();
	afx_msg void OnEnChangeEditspeclight();
	afx_msg void OnEnChangeEditambimateri();
	afx_msg void OnEnChangeEditdiffmateri();
	afx_msg void OnEnChangeEditspecmateri();
	afx_msg void OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar);

public:
	bool InitTreeControl();		//��ʼ����ע���οؼ�
	bool UpdateLabelList();	//���±�ע�б�
	bool SetSelectItem(int index);	//�����б�ѡ����
	void AddNewListItem();	//���б�����item
	void SetRadioSelect();	//���õ�ѡ��ѡ�С�
	void SetRadioEdit();	//���õ�ѡ�򡰱༭��
	void SetRadioPivotPointType(PivotPointType ppt);	//���õ�ѡ�������ת��ʽѡ��״̬

	bool SelectTreeItem(int cateId, int insId);
	bool UpdateTreeControl();	//���±�ע���οؼ�

private:


public:
	afx_msg void OnTest2();
private:
	CListCtrl m_ListControl;
	int m_selInsIndex;
	CString m_selCateName;
	int m_ListSelIndex;
public:
	virtual void OnInitialUpdate();
	afx_msg void OnLvnItemchangedList1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnNMRClickList1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnListDelete();
	afx_msg void OnBnClickedSaveLabelFile();
public:
	afx_msg void OnBnClickedRadio1();
	afx_msg void OnBnClickedRadio2();
	afx_msg void OnBnClickedRadio3();
	afx_msg void OnBnClickedBtnCreateNewIns();
	afx_msg void OnBnClickedRadio4();
	afx_msg void OnBnClickedRadio5();
private:
	CButton m_Radio_Render;
	CButton m_Radio_Add;
public:
	afx_msg void OnBnClickedBtnCreateNewInsRec();
	afx_msg void OnBnClickedRadio6();
	afx_msg void OnBnClickedRadio7();
	afx_msg void OnBnClickedRadio8();
	afx_msg void OnBnClickedRadio9();
	afx_msg void OnBnClickedRadio10();
	afx_msg void OnBnClickedRadio11();
	afx_msg void OnBnClickedRadio12();
	afx_msg void OnGetBackFaces();
	afx_msg void OnBnClickedRadio13();
	afx_msg void OnBnClickedRadio14();
	afx_msg void OnRemoveNoise();
	afx_msg void OnGetRoof();
	afx_msg void OnBnClickedRadio15();
	afx_msg void OnNMClickList1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnListCalcArea();
	afx_msg void OnBnClickedResetCamera();
	afx_msg void OnBnClickedRadio16();
	afx_msg void OnBnClickedRadio17();
	afx_msg void OnBnClickedRadio18();
	afx_msg void OnBnClickedRadio19();
private:
	CTreeCtrl m_treeControl;
public:
	afx_msg void OnNMClickTree1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTreeDelete();
	afx_msg void OnNMRClickTree1(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnTreeGetBack();
	afx_msg void OnTreeRemoveNoise();	
	afx_msg void OnGraphCut();
	afx_msg void OnBnClickedRadio21();
	afx_msg void OnBnClickedRadio22();
	afx_msg void OnBnClickedRadio23();
	afx_msg void OnTreeNewInstance();
	afx_msg void OnBnClickedRadio24();
	afx_msg void OnBnClickedRadio25();
};


