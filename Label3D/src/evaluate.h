#pragma once
#include <iostream>
#include <vector>
#include "assistTools\filetool.h"
#include <iterator>
#include "model.h"

using namespace std;

class Evaluation
{
	struct SIou
	{
		double iou;
		int gt_id;	//此iou对应的groundTruth中的实例编号
		int pred_id;	//此iou对应的perd中的实例编号
	};

public:
	//加载GroundTruth
	void LoadGroundTruthData(const string& gtFile);
	//加载预测结果Prediction
	void LoadPredictionData(const string& predFile);
	//加载数据. gtFile：真实值文件； predFile：预测文件
	void LoadData(const string& gtFile, const string& predFile);
	//获取每个实例包含的面片索引
	vector<vector<int>> GetInsFIndices(const vector<int>& label);
	//取两个有序向量的交集
	vector<int> GetIntersection(const vector<int>& a, const vector<int>& b);
	//取两个有序向量的并集
	vector<int> GetUnion(const vector<int>& a, const vector<int>& b);

	//升序比较（取maxElement）
	static bool iouCmp_asce(SIou a, SIou b);
	//降序比较（取minElement）
	static bool iouCmp_desc(SIou a, SIou b);
	//计算每个预测的实例与真实实例之间的iou,返回siou[pred_count][gt_count]
	vector<vector<SIou>> CalcIou_Pred();
	//计算每个真实实例与预测的实例之间的iou,返回siou[gt_count][pred_count]
	vector<vector<SIou>> CalcIou_GT();
	//计算准确率Average Precision
	void CalcAverPrecision(float minIou);

	void TestGetMaxElement();

	//获取误差、未识别的实例包围框
	void GetErrorMissInsBox(vector<CBoundingBox> &errorBoxs, vector<CBoundingBox> &missBoxs, const GLMmodel *model, float minIou);

public:
	int numFaces;	//面片数目
	vector<int> groundTruth;	//真实值
	vector<vector<int>> insFaces_gt;	//真实值每个实例包含的面片索引
	vector<int> prediction;		//预测值
	vector<vector<int>> insFaces_pred;	//预测值每个实例包含的面片索引
	vector<double> faceArea;	//面片面积
};