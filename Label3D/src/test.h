#pragma once
#include <json/json.h>
#include <string>
#include <iostream>
#include <fstream>
//#include <io.h>
#include <time.h>
#include "assistTools\jsontool.h"
#include "publicstruct.h"
#include <opencv2/core/core.hpp>  
#include <opencv2/highgui/highgui.hpp>  
#include <opencv2/opencv.hpp>
#include "model.h"
#include "evaluate.h"
//#include "openglTools\glm.h"
#include "assistTools\panoptic.h"

using namespace std;
using namespace cv;

class CMyTest
{
public:
	void Test();
	void TestVector();
	void TestJson();

	//��������׼ȷ������
	void EvaluateBuilding();

	//�ݶ�׼ȷ������
	void EvaluateRoof();

	//��ȡ��δʶ���ʵ����Χ��
	void GetErrorMissInsBox();

	bool WriteBBoxFile(const vector<CBoundingBox> &errorBoxs, const vector<CBoundingBox> &missBoxs, const string& fileName);
	bool ReadBBoxFile(vector<CBoundingBox> &errorBoxs, vector<CBoundingBox> &missBoxs, const string& fileName);

	//ȥ����ֱ��Ƭ
	bool RemoveVerFaces(const MeshGraph* meshGraph, const string &roofFile, const string &saveFile);

	//ȥ����ֱ��Ƭ
	bool RemoveVerFaces();

	void TestAddVec();

	void TestSwapIns();

	void TestReadBBoxFile();

	void TestCamera();

	//��ԭ��txt��ע�ļ�ת��Ϊjson
	void TransLabelFile();

	//����ά��עͶӰ����άͼ����; type��Ƭ���ͣ�{"Aerial", "OpenGL"}
	void Label3DTo2D();
	void Label3DTo2D(const string &modelFile, const string &photoPath, 
		const string &photoInfoFile, const string &LabelFile_3D,
		const string &photoSavePath, const string& photoType);

	//���Ա���2D Mask
	void EncodeMask();

	//��ԭ����json��ע�ļ�ת��Ϊ��ӦMSCOCO Panoptic��ע���͵�json�ļ�
	void TransLabelFile2COCO();

	//����segMap��sortedMap��insId_cateId_Table���ɵ���ͼƬ��panoptic��ע
	void SetSinglePanoLabel(panopticlabel::PanopticLabel* panoLabel, 
		const cv::Mat &segMap, const cv::Mat &sortedMap,
		const vector<Vector2i> &insId_cateId_Table);

	//��3D��ע�ļ�ת��ΪPanopticȫ���ָ�ѵ����ע��ʽ�ļ�
	void GenPanoLabel();

	//�ϲ�������Ƭ��panoptic��ע�ļ�
	void MergePanopticLabels();

	//����glm���ȡ����ģ��
	void TestGlmGroupModel();

	//��ȡ��Ƭ����
	void TestReadFaceTexture(const int faceIndex, const GLMmodel *model, const MeshGraph* meshGraph);
};
