
// Label3DDoc.h : CLabel3DDoc 类的接口
//


#pragma once
#include "src\modelEditor.h"
#include "src/test.h"

class CControlView;
class CLabel3DView;

class CLabel3DDoc : public CDocument
{
protected: // 仅从序列化创建
	CLabel3DDoc();
	DECLARE_DYNCREATE(CLabel3DDoc)

// 特性
public:

// 操作
public:
	CControlView* GetControlView(void);
	CLabel3DView* GetLabel3DView(void);

	void SetSelectedItem(int id);
	bool SeletcTreeItem(int cateId, int insId);
	void AddNewListItem();
	void SetRadioSelect();	//设置单选框“选中”
	void SetRadioEdit();	//设置单选框“编辑”
	void SetRadioPivotPointType(PivotPointType ppt);	//设置单选框相机旋转方式选中状态
	//void UpdateLabelList();

// 重写
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
#ifdef SHARED_HANDLERS
	virtual void InitializeSearchContent();
	virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds);
#endif // SHARED_HANDLERS

// 实现
public:
	virtual ~CLabel3DDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// 生成的消息映射函数
protected:
	DECLARE_MESSAGE_MAP()

public:
	ModelEditor* modelEditor;

#ifdef SHARED_HANDLERS
	// 用于为搜索处理程序设置搜索内容的 Helper 函数
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS
	afx_msg void OnLoadmodel();
	afx_msg void OnLoadAnnocation();
	afx_msg void OnTest2();
	afx_msg void OnRemoveVerFaces();
	afx_msg void OnGenpanolabel();
};
