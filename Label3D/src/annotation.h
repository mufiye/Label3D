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

	bool DeleteInstance(int insIndex);	//删除实例
	void UpdateInsFaces();	//更新标注信息
	bool SaveToFile(const string& fileName, bool updateLabelInfo);	//保存标注信息到文件

	bool AddMesh(int faceIndex, int insIndex);	//往实例增加面片
	bool DeleteMesh(int faceIndex);	//删除实例面片
	bool AddNewInstance();	//添加新的实例

public:
	int numFaces;	//面片数目
	int numInstance;	//实例数目（包括背景）
	vector<int> segInfo;	//标注信息
	vector<vector<int>> insFaces;	//每个实例包含的面片索引,第0项为背景
	vector<Vector3f> insColors;		//实例颜色
	vector <CBoundingBox> insBBox;	//每个实例包围框
};

class LabelInfo
{
public:
	LabelInfo(GLMmodel *m);

	struct Instance
	{
		int instanceId;
		vector<int> insFaces;	//实例包含的面片索引
		Vector3f insColor;		//实例颜色
		CBoundingBox insBBox;	//实例包围框
	};

	struct Category
	{
		int categoryId;
		string categoryName;
		vector<Instance> instances;	//每个实例包含的面片索引,第0项为背景
		int newInsId = 0;	//当前实例编号，新增实例时赋予的实例编号,初始为0
		int isthing;
		string supercategory;
		Vector3f cateColor; //类别颜色
	};

public:
	//根据类别Id和实例Id查找实例
	Instance& SearchInstance(int cateId, int insId);

	//根据类别Id查询该类别是否为thing
	bool CheckCateIsthing(int cateId);

	//筛选与所给[cateId, insId]同类同实例的面片
	vector<int> SelectFaces(const vector<int> &faces, int cateId, int insId);

	//筛选与所给[cateId, insId]不同类或不同实例的面片
	vector<int> SelectFacesDifferent(const vector<int> &faces, int cateId, int insId);

	//筛选背景面片
	vector<int> SelectBackFaces(const vector<int> &faces);

public:
	//删除空实例，即包含0个面片的实例
	void DeleteEmptyIns();
	//删除包含0个实例的类别
	void DeleteEmptyCategory();
	//对类别Id和实例Id重新编号
	void RenumberCateInsIds();
	//根据categories更新segInfo
	void UpdateSegInfoWithCate();

public:
	//将标注信息保存为json文件
	bool SaveAsFile(const string& fileName);
	//从json文件读取标注信息
	bool ReadFromFile(const string& fileName);

	//设置背景颜色
	inline void SetBackGroundColor(Vector3f color) { this->backGround.insColor = color; };
	//随机设置所有实例颜色
	void SetInsColor();
	//计算背景包围框
	void CalcBackGroundBBox();
	//计算单个实例包围框
	void CalcSingleBBox(int cateId, int insId);
	//计算所有实例包围框
	void CalcBBox();

public:
	//新增实例, cateId: 类别Id;  返回新增的实例Id
	int AddNewInstance(int cateId);
	//新增类别，cateName：类别名称; 返回新增的类别Id
	int AddNewCategory(const string& cateName);
	//删除实例
	bool DeleteInstance(int cateId, int insId);
	//删除类别
	bool DeleteCategory(int cateId);

private:
	//添加单个实例面片
	bool AddMesh(int faceIndex, int cateId, int insId);
	//删除单个实例面片
	bool DeleteMesh(int faceIndex);

public:
	//添加实例面片数组
	bool AddMesh(vector<int> faceIndices, int cateId, int insId);
	//删除实例面片数组
	bool DeleteMesh(vector<int> faceIndices);

public:
	GLMmodel *model;
	vector<Vector2i> segInfo;	//[categoryId, instanceId], segInfo[0] = -1表示背景, segInfo[1]初始化为-1

	Instance backGround;
	vector<Category> categories;
	//int newCateId;	//当前类别编号，新增类别时赋予的类别编号
};