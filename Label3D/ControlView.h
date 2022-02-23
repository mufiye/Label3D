#pragma once
#include <iostream>
#include "Label3DDoc.h"
#include "afxwin.h"
#include "resource.h"
#include <tchar.h>
#include "afxcmn.h"
#include "InputNumDlg.h"
#include "SelCategoryDlg.h"

// CControlView 窗体视图

class CControlView : public CFormView
{
	DECLARE_DYNCREATE(CControlView)

protected:
	CControlView();           // 动态创建所使用的受保护的构造函数
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
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

	DECLARE_MESSAGE_MAP()

// 特性
public:
	CLabel3DDoc* GetDocument() const;

private:
	CSliderCtrl m_scAmbiLightStrength;
	CSliderCtrl m_scDiffLightStrength;
	CSliderCtrl m_scSpecLightStrength;
	CSliderCtrl m_scAmbiMateriStrength;
	CSliderCtrl m_scDiffMateriStrength;
	CSliderCtrl m_scSpecMateriStrength;
	int m_AmbiLightStrength;	//环境光强度
	int m_DiffLightStrength;	//散射光强度
	int m_SpecLightStrength;	//镜面光强度
	int m_AmbiMateriStrength;	//材质环境光反射属性
	int m_DiffMateriStrength;	//材质散射光反射属性
	int m_SpecMateriStrength;	//材质镜面光反射属性

	CSliderCtrl m_scPaintPixel;
	int m_paintPixel;

public:
	CLabel3DDoc* pDoc;	//文档指针
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
	bool InitTreeControl();		//初始化标注树形控件
	bool UpdateLabelList();	//更新标注列表
	bool SetSelectItem(int index);	//设置列表选中行
	void AddNewListItem();	//往列表新增item
	void SetRadioSelect();	//设置单选框“选中”
	void SetRadioEdit();	//设置单选框“编辑”
	void SetRadioPivotPointType(PivotPointType ppt);	//设置单选框相机旋转方式选中状态

	bool SelectTreeItem(int cateId, int insId);
	bool UpdateTreeControl();	//更新标注树形控件

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


