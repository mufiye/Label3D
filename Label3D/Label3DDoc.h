
// Label3DDoc.h : CLabel3DDoc ��Ľӿ�
//


#pragma once
#include "src\modelEditor.h"
#include "src/test.h"

class CControlView;
class CLabel3DView;

class CLabel3DDoc : public CDocument
{
protected: // �������л�����
	CLabel3DDoc();
	DECLARE_DYNCREATE(CLabel3DDoc)

// ����
public:

// ����
public:
	CControlView* GetControlView(void);
	CLabel3DView* GetLabel3DView(void);

	void SetSelectedItem(int id);
	bool SeletcTreeItem(int cateId, int insId);
	void AddNewListItem();
	void SetRadioSelect();	//���õ�ѡ��ѡ�С�
	void SetRadioEdit();	//���õ�ѡ�򡰱༭��
	void SetRadioPivotPointType(PivotPointType ppt);	//���õ�ѡ�������ת��ʽѡ��״̬
	//void UpdateLabelList();

// ��д
public:
	virtual BOOL OnNewDocument();
	virtual void Serialize(CArchive& ar);
#ifdef SHARED_HANDLERS
	virtual void InitializeSearchContent();
	virtual void OnDrawThumbnail(CDC& dc, LPRECT lprcBounds);
#endif // SHARED_HANDLERS

// ʵ��
public:
	virtual ~CLabel3DDoc();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// ���ɵ���Ϣӳ�亯��
protected:
	DECLARE_MESSAGE_MAP()

public:
	ModelEditor* modelEditor;

#ifdef SHARED_HANDLERS
	// ����Ϊ����������������������ݵ� Helper ����
	void SetSearchContent(const CString& value);
#endif // SHARED_HANDLERS
	afx_msg void OnLoadmodel();
	afx_msg void OnLoadAnnocation();
	afx_msg void OnTest2();
	afx_msg void OnRemoveVerFaces();
	afx_msg void OnGenpanolabel();
};
