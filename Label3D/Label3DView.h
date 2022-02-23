
// Label3DView.h : CLabel3DView ��Ľӿ�
//

#pragma once
#include <iostream>
#include "src\modelEditor.h"

#define ROUND(a) (int)(a + 0.5)

class CLabel3DView : public CView
{
protected: // �������л�����
	CLabel3DView();
	DECLARE_DYNCREATE(CLabel3DView)

// ����
public:
	CLabel3DDoc* GetDocument() const;

// ����
public:

// ��д
public:
	virtual void OnDraw(CDC* pDC);  // ��д�Ի��Ƹ���ͼ
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL OnEraseBkgnd(CDC* pDC);
protected:

// ʵ��
public:
	virtual ~CLabel3DView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// ���ɵ���Ϣӳ�亯��
protected:
	DECLARE_MESSAGE_MAP()
public:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);

private:
	bool OpenConsole();
	BOOL SetupPixelFormat();	//�������ظ�ʽ
	BOOL InitializeOpenGL();	//��ʼ��OpenGL

	void RotateX(float angle);
	void RotateY(float angle);

public:
	void getMaxXY();
	void circlePoint(double x, double y, int ori_x, int ori_y);
	void rectPoint(const CPoint& p1, const CPoint& p2);
	void MbCircle(int ori_x, int ori_y, double r);

	//��ȡ·�������о�������Ƭ����
	vector<int> GetAllFacesInRoot(const vector<CPoint>& points);

public:
	//ModelEditor* modelEditor;

	CLabel3DDoc* pDoc;	//�ĵ�ָ��
	HGLRC m_hRC;	//Rendering Context��ɫ������
	CClientDC  *m_pDC; //Device Context�豸������

	CPoint lastPos;	//����ƶ�ʱ����λ��,����������
	int m_width;	//�ӿڵĿ��
	int m_height;	//�ӿڵĸ߶�

public:
	int maxX, maxY;

	//easy3d::vec3 m_pivotPoint; //�ӽ���ת���ĵ�

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

#ifndef _DEBUG  // Label3DView.cpp �еĵ��԰汾
inline CLabel3DDoc* CLabel3DView::GetDocument() const
   { return reinterpret_cast<CLabel3DDoc*>(m_pDocument); }
#endif

