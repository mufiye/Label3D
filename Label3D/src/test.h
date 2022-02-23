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

	//完整建筑准确率评估
	void EvaluateBuilding();

	//屋顶准确率评估
	void EvaluateRoof();

	//获取误差、未识别的实例包围框
	void GetErrorMissInsBox();

	bool WriteBBoxFile(const vector<CBoundingBox> &errorBoxs, const vector<CBoundingBox> &missBoxs, const string& fileName);
	bool ReadBBoxFile(vector<CBoundingBox> &errorBoxs, vector<CBoundingBox> &missBoxs, const string& fileName);

	//去除竖直面片
	bool RemoveVerFaces(const MeshGraph* meshGraph, const string &roofFile, const string &saveFile);

	//去除竖直面片
	bool RemoveVerFaces();

	void TestAddVec();

	void TestSwapIns();

	void TestReadBBoxFile();

	void TestCamera();

	//将原本txt标注文件转换为json
	void TransLabelFile();

	//将三维标注投影到二维图像上; type照片类型：{"Aerial", "OpenGL"}
	void Label3DTo2D();
	void Label3DTo2D(const string &modelFile, const string &photoPath, 
		const string &photoInfoFile, const string &LabelFile_3D,
		const string &photoSavePath, const string& photoType);

	//测试编码2D Mask
	void EncodeMask();

	//将原本的json标注文件转换为适应MSCOCO Panoptic标注类型的json文件
	void TransLabelFile2COCO();

	//根据segMap、sortedMap、insId_cateId_Table生成单张图片的panoptic标注
	void SetSinglePanoLabel(panopticlabel::PanopticLabel* panoLabel, 
		const cv::Mat &segMap, const cv::Mat &sortedMap,
		const vector<Vector2i> &insId_cateId_Table);

	//将3D标注文件转换为Panoptic全景分割训练标注格式文件
	void GenPanoLabel();

	//合并所有照片的panoptic标注文件
	void MergePanopticLabels();

	//测试glm库读取分组模型
	void TestGlmGroupModel();

	//读取面片纹理
	void TestReadFaceTexture(const int faceIndex, const GLMmodel *model, const MeshGraph* meshGraph);
};
