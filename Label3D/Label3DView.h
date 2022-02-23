
// Label3DView.h : CLabel3DView 类的接口
//

#pragma once
#include <iostream>
#include "src\modelEditor.h"

#define ROUND(a) (int)(a + 0.5)

class CLabel3DView : public CView
{
protected: // 仅从序列化创建
	CLabel3DView();
	DECLARE_DYNCREATE(CLabel3DView)

// 特性
public:
	CLabel3DDoc* GetDocument() const;

// 操作
public:

// 重写
public:
	virtual void OnDraw(CDC* pDC);  // 重写以绘制该视图
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL OnEraseBkgnd(CDC* pDC);
protected:

// 实现
public:
	virtual ~CLabel3DView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 生成的消息映射函数
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

private:
	bool OpenConsole();
	BOOL SetupPixelFormat();	//设置像素格式
	BOOL InitializeOpenGL();	//初始化OpenGL

	void RotateX(float angle);
	void RotateY(float angle);

public:
	void getMaxXY();
	void circlePoint(double x, double y, int ori_x, int ori_y);
	void rectPoint(const CPoint& p1, const CPoint& p2);
	void MbCircle(int ori_x, int ori_y, double r);

	//获取路径上所有经过的面片索引
	vector<int> GetAllFacesInRoot(const vector<CPoint>& points);

public:
	//ModelEditor* modelEditor;

	CLabel3DDoc* pDoc;	//文档指针
	HGLRC m_hRC;	//Rendering Context着色描述表
	CClientDC  *m_pDC; //Device Context设备描述表

	CPoint lastPos;	//鼠标移动时最后点位置,用于鼠标控制
	int m_width;	//视口的宽度
	int m_height;	//视口的高度

public:
	int maxX, maxY;

	//easy3d::vec3 m_pivotPoint; //视角旋转中心点

	vector<CPoint> pickedPoint;

public:
	afx_msg void OnDestroy();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnRButtonDown(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnRButtonUp(UINT nFlags, CPoint point);
};

#ifndef _DEBUG  // Label3DView.cpp 中的调试版本
inline CLabel3DDoc* CLabel3DView::GetDocument() const
   { return reinterpret_cast<CLabel3DDoc*>(m_pDocument); }
#endif

