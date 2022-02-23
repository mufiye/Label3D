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
		int gt_id;	//��iou��Ӧ��groundTruth�е�ʵ�����
		int pred_id;	//��iou��Ӧ��perd�е�ʵ�����
	};

public:
	//����GroundTruth
	void LoadGroundTruthData(const string& gtFile);
	//����Ԥ����Prediction
	void LoadPredictionData(const string& predFile);
	//��������. gtFile����ʵֵ�ļ��� predFile��Ԥ���ļ�
	void LoadData(const string& gtFile, const string& predFile);
	//��ȡÿ��ʵ����������Ƭ����
	vector<vector<int>> GetInsFIndices(const vector<int>& label);
	//ȡ�������������Ľ���
	vector<int> GetIntersection(const vector<int>& a, const vector<int>& b);
	//ȡ�������������Ĳ���
	vector<int> GetUnion(const vector<int>& a, const vector<int>& b);

	//����Ƚϣ�ȡmaxElement��
	static bool iouCmp_asce(SIou a, SIou b);
	//����Ƚϣ�ȡminElement��
	static bool iouCmp_desc(SIou a, SIou b);
	//����ÿ��Ԥ���ʵ������ʵʵ��֮���iou,����siou[pred_count][gt_count]
	vector<vector<SIou>> CalcIou_Pred();
	//����ÿ����ʵʵ����Ԥ���ʵ��֮���iou,����siou[gt_count][pred_count]
	vector<vector<SIou>> CalcIou_GT();
	//����׼ȷ��Average Precision
	void CalcAverPrecision(float minIou);

	void TestGetMaxElement();

	//��ȡ��δʶ���ʵ����Χ��
	void GetErrorMissInsBox(vector<CBoundingBox> &errorBoxs, vector<CBoundingBox> &missBoxs, const GLMmodel *model, float minIou);

public:
	int numFaces;	//��Ƭ��Ŀ
	vector<int> groundTruth;	//��ʵֵ
	vector<vector<int>> insFaces_gt;	//��ʵֵÿ��ʵ����������Ƭ����
	vector<int> prediction;		//Ԥ��ֵ
	vector<vector<int>> insFaces_pred;	//Ԥ��ֵÿ��ʵ����������Ƭ����
	vector<double> faceArea;	//��Ƭ���
};