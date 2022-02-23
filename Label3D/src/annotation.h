#pragma once
#include <vector>
#include <string.h>
#include <Eigen/Dense>
#include "model.h"
#include "assistTools\filetool.h"
#include <json/json.h>

using namespace std;
using namespace Eigen;

class Annotation
{
public:
	Annotation();

	bool DeleteInstance(int insIndex);	//ɾ��ʵ��
	void UpdateInsFaces();	//���±�ע��Ϣ
	bool SaveToFile(const string& fileName, bool updateLabelInfo);	//�����ע��Ϣ���ļ�

	bool AddMesh(int faceIndex, int insIndex);	//��ʵ��������Ƭ
	bool DeleteMesh(int faceIndex);	//ɾ��ʵ����Ƭ
	bool AddNewInstance();	//����µ�ʵ��

public:
	int numFaces;	//��Ƭ��Ŀ
	int numInstance;	//ʵ����Ŀ������������
	vector<int> segInfo;	//��ע��Ϣ
	vector<vector<int>> insFaces;	//ÿ��ʵ����������Ƭ����,��0��Ϊ����
	vector<Vector3f> insColors;		//ʵ����ɫ
	vector <CBoundingBox> insBBox;	//ÿ��ʵ����Χ��
};

class LabelInfo
{
public:
	LabelInfo(GLMmodel *m);

	struct Instance
	{
		int instanceId;
		vector<int> insFaces;	//ʵ����������Ƭ����
		Vector3f insColor;		//ʵ����ɫ
		CBoundingBox insBBox;	//ʵ����Χ��
	};

	struct Category
	{
		int categoryId;
		string categoryName;
		vector<Instance> instances;	//ÿ��ʵ����������Ƭ����,��0��Ϊ����
		int newInsId = 0;	//��ǰʵ����ţ�����ʵ��ʱ�����ʵ�����,��ʼΪ0
		int isthing;
		string supercategory;
		Vector3f cateColor; //�����ɫ
	};

public:
	//�������Id��ʵ��Id����ʵ��
	Instance& SearchInstance(int cateId, int insId);

	//�������Id��ѯ������Ƿ�Ϊthing
	bool CheckCateIsthing(int cateId);

	//ɸѡ������[cateId, insId]ͬ��ͬʵ������Ƭ
	vector<int> SelectFaces(const vector<int> &faces, int cateId, int insId);

	//ɸѡ������[cateId, insId]��ͬ���ͬʵ������Ƭ
	vector<int> SelectFacesDifferent(const vector<int> &faces, int cateId, int insId);

	//ɸѡ������Ƭ
	vector<int> SelectBackFaces(const vector<int> &faces);

public:
	//ɾ����ʵ����������0����Ƭ��ʵ��
	void DeleteEmptyIns();
	//ɾ������0��ʵ�������
	void DeleteEmptyCategory();
	//�����Id��ʵ��Id���±��
	void RenumberCateInsIds();
	//����categories����segInfo
	void UpdateSegInfoWithCate();

public:
	//����ע��Ϣ����Ϊjson�ļ�
	bool SaveAsFile(const string& fileName);
	//��json�ļ���ȡ��ע��Ϣ
	bool ReadFromFile(const string& fileName);

	//���ñ�����ɫ
	inline void SetBackGroundColor(Vector3f color) { this->backGround.insColor = color; };
	//�����������ʵ����ɫ
	void SetInsColor();
	//���㱳����Χ��
	void CalcBackGroundBBox();
	//���㵥��ʵ����Χ��
	void CalcSingleBBox(int cateId, int insId);
	//��������ʵ����Χ��
	void CalcBBox();

public:
	//����ʵ��, cateId: ���Id;  ����������ʵ��Id
	int AddNewInstance(int cateId);
	//�������cateName���������; �������������Id
	int AddNewCategory(const string& cateName);
	//ɾ��ʵ��
	bool DeleteInstance(int cateId, int insId);
	//ɾ�����
	bool DeleteCategory(int cateId);

private:
	//��ӵ���ʵ����Ƭ
	bool AddMesh(int faceIndex, int cateId, int insId);
	//ɾ������ʵ����Ƭ
	bool DeleteMesh(int faceIndex);

public:
	//���ʵ����Ƭ����
	bool AddMesh(vector<int> faceIndices, int cateId, int insId);
	//ɾ��ʵ����Ƭ����
	bool DeleteMesh(vector<int> faceIndices);

public:
	GLMmodel *model;
	vector<Vector2i> segInfo;	//[categoryId, instanceId], segInfo[0] = -1��ʾ����, segInfo[1]��ʼ��Ϊ-1

	Instance backGround;
	vector<Category> categories;
	//int newCateId;	//��ǰ����ţ��������ʱ����������
};