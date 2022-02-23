#include "test.h"
#include <easy3d\viewer\camera.h>
#include "annotation.h"
#include "assistTools\phototool.h"
#include "assistTools\projectiontool.h"

using namespace phototool;

#define PI 3.14159265358979323846 
inline double Random() { return rand() % 100 / (double)101; };

void CMyTest::Test()
{
	this->TestGlmGroupModel();
}

//测试glm库读取分组模型
void CMyTest::TestGlmGroupModel()
{
	string modelFile = "data/project5/dongkengkou.obj";
	GLMmodel* model = glmReadOBJ(modelFile.c_str());
	int numTriangles = model->numtriangles;
	int numGroups = model->numgroups;
	int numGroupTriangles = 0;
	for (size_t i = 0; i < numGroups; i++)
	{
		numGroupTriangles += model->groups[i].numtriangles;
	}

	if (numTriangles == numGroupTriangles)
	{
		return;
	}
}

void CMyTest::TestVector()
{
	//vector<double> a;
	//a.push_back(1.0);
	//a.push_back(1.0);
	//a.push_back(1.0);
	//a.push_back(1.0);

	//std::vector<double>::iterator biggest = std::max_element(std::begin(a), std::end(a));
	//std::cout << "Max element is " << *biggest << " at position " << std::distance(std::begin(a), biggest) << std::endl;

	Evaluation eval;
	eval.TestGetMaxElement();
}

void CMyTest::TestJson()
{
	string jsonFile = "res/ImgMasksTable.json";

	ifstream ifs(jsonFile);
	Json::CharReaderBuilder readerbuilder;
	//Json::Reader reader;
	Json::Value root;
	JSONCPP_STRING errs;
	bool ok = Json::parseFromStream(readerbuilder, ifs, &root, &errs);
	if (ok)
	{
		int imgSize = root.size();
		//maskPerImg.resize(imgSize);
		//sPerMaskPerImg.resize(imgSize);

		for (int i = 0; i < imgSize; i++)
		{
			int imageId = root[i]["ImageId"].asInt();
			Json::Value imgMasksRoot = root[i]["Masks"];

			int maskSize = imgMasksRoot.size();
			for (int j = 0; j < maskSize; j++)
			{
				std::cout << imgMasksRoot[j]["MaskId"].asInt() << " ";
				std::cout << imgMasksRoot[j]["MaskScore"].asFloat() << std::endl;
			}
		}
	}
}

void TestInterUnion()
{
	Evaluation eval;
	vector<int> a;
	a.push_back(1);
	a.push_back(3);
	a.push_back(4);
	a.push_back(7);
	a.push_back(10);
	a.push_back(12);

	vector<int> b;
	b.push_back(2);
	b.push_back(4);
	b.push_back(5);
	b.push_back(10);
	b.push_back(16);

	vector<int> intersection_v = eval.GetIntersection(a, b);
	vector<int> union_v = eval.GetUnion(a, b);

	std::cout << "a: ";
	for (size_t i = 0; i < a.size(); i++)
	{
		std::cout << a[i] << " ";
	}
	std::cout << endl;

	std::cout << "b: ";
	for (size_t i = 0; i < b.size(); i++)
	{
		std::cout << b[i] << " ";
	}
	std::cout << endl;

	std::cout << "交集: ";
	for (size_t i = 0; i < intersection_v.size(); i++)
	{
		std::cout << intersection_v[i] << " ";
	}
	std::cout << endl;

	std::cout << "并集: ";
	for (size_t i = 0; i < union_v.size(); i++)
	{
		std::cout << union_v[i] << " ";
	}
	std::cout << endl;
}

//完整建筑准确率评估
void CMyTest::EvaluateBuilding()
{
	string modelFile = "F:/StudyDemo/Label3D/Label3D/data/project5/dongkengkou.obj";	//wangcun	C_Part2	   Model_part2	dongkengkou
	string gtFile = "F:/StudyDemo/Label3D/Label3D/data/project5/building/buildings_gt.txt";
	string predFile_rgb = "F:/StudyDemo/Label3D/Label3D/data/project5/building/buildings_rgb.txt";
	string predFile_rgbh = "F:/StudyDemo/Label3D/Label3D/data/project5/building/buildings_rgbh.txt";
	string predFile_speK1 = "F:/StudyDemo/Label3D/Label3D/data/project5/building/buildings_spe_K1.txt";
	string predFile_speK2 = "F:/StudyDemo/Label3D/Label3D/data/project5/building/buildings_spe_K2.txt";

	string predFile_NoTheta = "F:/StudyDemo/Label3D/Label3D/data/project5/building/buildings_NoTheta.txt";
	string predFile_NoD = "F:/StudyDemo/Label3D/Label3D/data/project5/building/buildings_NoD.txt";


	long t1 = time(NULL);
	std::cout << "正在加载模型(" << modelFile << "):......" << endl;
	GLMmodel* model = glmReadOBJ((char*)modelFile.c_str());
	std::cout << "加载模型完成：" << round(time(NULL) - t1) << " seconds" << endl;

	int numFaces = model->numtriangles;
	vector<double> faceArea(numFaces);	//记录每个面片的面积
	for (int i = 0; i < numFaces; i++)
	{
		Vector3d v0(model->vertices[3 * model->triangles[i].vindices[0] + 0],
			model->vertices[3 * model->triangles[i].vindices[0] + 1],
			model->vertices[3 * model->triangles[i].vindices[0] + 2]);

		Vector3d v1(model->vertices[3 * model->triangles[i].vindices[1] + 0],
			model->vertices[3 * model->triangles[i].vindices[1] + 1],
			model->vertices[3 * model->triangles[i].vindices[1] + 2]);

		Vector3d v2(model->vertices[3 * model->triangles[i].vindices[2] + 0],
			model->vertices[3 * model->triangles[i].vindices[2] + 1],
			model->vertices[3 * model->triangles[i].vindices[2] + 2]);

		faceArea[i] = modeltool::CalcTriArea(v0, v1, v2);
	}

	Evaluation eval;
	eval.numFaces = numFaces;
	eval.LoadData(gtFile, predFile_rgb);
	eval.faceArea = faceArea;

	std::cout << endl;
	std::cout << "Building_RGB:" << endl;
	eval.CalcAverPrecision(0.5);
	std::cout << endl;
	eval.CalcAverPrecision(0.75);
	std::cout << endl;

	eval.LoadPredictionData(predFile_rgbh);
	std::cout << endl;
	std::cout << "Building_RGBH:" << endl;
	eval.CalcAverPrecision(0.5);
	std::cout << endl;
	eval.CalcAverPrecision(0.75);
	std::cout << endl;

	//eval.LoadPredictionData(predFile_speK1);
	//std::cout << endl;
	//std::cout << "Building_Spe_K1:" << endl;
	//eval.CalcAverPrecision(0.5);
	//std::cout << endl;
	//eval.CalcAverPrecision(0.75);
	//std::cout << endl;

	//eval.LoadPredictionData(predFile_speK2);
	//std::cout << endl;
	//std::cout << "Building_Spe_K2:" << endl;
	//eval.CalcAverPrecision(0.5);
	//std::cout << endl;
	//eval.CalcAverPrecision(0.75);
	//std::cout << endl;

	eval.LoadPredictionData(predFile_NoTheta);
	std::cout << endl;
	std::cout << "Building_NoTheta:" << endl;
	eval.CalcAverPrecision(0.5);
	std::cout << endl;
	eval.CalcAverPrecision(0.75);
	std::cout << endl;

	eval.LoadPredictionData(predFile_NoD);
	std::cout << endl;
	std::cout << "Building_NoD:" << endl;
	eval.CalcAverPrecision(0.5);
	std::cout << endl;
	eval.CalcAverPrecision(0.75);
	std::cout << endl;
}

//屋顶准确率评估
void CMyTest::EvaluateRoof()
{
	string modelFile = "F:/StudyDemo/Label3D/Label3D/data/project1/wangcun.obj";	//wangcun	C_Part2	   Model_part2	dongkengkou
	string gtFile = "F:/StudyDemo/Label3D/Label3D/data/project1/roof_opti/roofs_gt.txt";
	string predFile_rgb = "F:/StudyDemo/Label3D/Label3D/data/project1/roof_opti/roofs_rgb.txt";
	string predFile_rgbh = "F:/StudyDemo/Label3D/Label3D/data/project1/roof_opti/roofs_rgbh.txt";
	string predFile_speK1 = "F:/StudyDemo/Label3D/Label3D/data/project1/roof_opti/roofs_spe_k1.txt";
	string predFile_speK2 = "F:/StudyDemo/Label3D/Label3D/data/project1/roof_opti/roofs_spe_k2.txt";


	long t1 = time(NULL);
	std::cout << "正在加载模型(" << modelFile << "):......" << endl;
	GLMmodel* model = glmReadOBJ((char*)modelFile.c_str());
	std::cout << "加载模型完成：" << round(time(NULL) - t1) << " seconds" << endl;

	int numFaces = model->numtriangles;
	vector<double> faceArea(numFaces);	//记录每个面片的面积
	for (int i = 0; i < numFaces; i++)
	{
		Vector3d v0(model->vertices[3 * model->triangles[i].vindices[0] + 0],
			model->vertices[3 * model->triangles[i].vindices[0] + 1],
			model->vertices[3 * model->triangles[i].vindices[0] + 2]);

		Vector3d v1(model->vertices[3 * model->triangles[i].vindices[1] + 0],
			model->vertices[3 * model->triangles[i].vindices[1] + 1],
			model->vertices[3 * model->triangles[i].vindices[1] + 2]);

		Vector3d v2(model->vertices[3 * model->triangles[i].vindices[2] + 0],
			model->vertices[3 * model->triangles[i].vindices[2] + 1],
			model->vertices[3 * model->triangles[i].vindices[2] + 2]);

		faceArea[i] = modeltool::CalcTriArea(v0, v1, v2);
	}

	Evaluation eval;
	eval.numFaces = numFaces;
	eval.LoadData(gtFile, predFile_rgb);
	eval.faceArea = faceArea;

	std::cout << endl;
	std::cout << "Building_RGB:" << endl;
	eval.CalcAverPrecision(0.5);
	std::cout << endl;
	eval.CalcAverPrecision(0.75);
	std::cout << endl;

	eval.LoadPredictionData(predFile_rgbh);
	std::cout << endl;
	std::cout << "Building_RGBH:" << endl;
	eval.CalcAverPrecision(0.5);
	std::cout << endl;
	eval.CalcAverPrecision(0.75);
	std::cout << endl;

	eval.LoadPredictionData(predFile_speK1);
	std::cout << endl;
	std::cout << "Building_Spe_K1:" << endl;
	eval.CalcAverPrecision(0.5);
	std::cout << endl;
	eval.CalcAverPrecision(0.75);
	std::cout << endl;

	eval.LoadPredictionData(predFile_speK2);
	std::cout << endl;
	std::cout << "Building_Spe_K2:" << endl;
	eval.CalcAverPrecision(0.5);
	std::cout << endl;
	eval.CalcAverPrecision(0.75);
	std::cout << endl;
}

//获取误差、未识别的实例包围框
void CMyTest::GetErrorMissInsBox()
{
	string modelFile = "F:/StudyDemo/Label3D/Label3D/data/project1/wangcun.obj";	//wangcun	C_Part2	   Model_part2	dongkengkou
	string gtFile = "F:/StudyDemo/Label3D/Label3D/data/project1/building/buildings_gt.txt";
	string predFile_rgb = "F:/StudyDemo/Label3D/Label3D/data/project1/building/buildings_rgb2.txt";
	string predFile_rgbh = "F:/StudyDemo/Label3D/Label3D/data/project1/building/buildings_rgbh2.txt";
	string predFile_speK1 = "F:/StudyDemo/Label3D/Label3D/data/project1/building/buildings_spe_K1.txt";
	string predFile_speK2 = "F:/StudyDemo/Label3D/Label3D/data/project1/building/buildings_spe_K2.txt";

	string predFile_NoTheta = "F:/StudyDemo/Label3D/Label3D/data/project1/building/buildings_NoTheta.txt";
	string predFile_NoD = "F:/StudyDemo/Label3D/Label3D/data/project1/building/buildings_NoD.txt";


	long t1 = time(NULL);
	std::cout << "正在加载模型(" << modelFile << "):......" << endl;
	GLMmodel* model = glmReadOBJ((char*)modelFile.c_str());
	std::cout << "加载模型完成：" << round(time(NULL) - t1) << " seconds" << endl;

	int numFaces = model->numtriangles;
	vector<double> faceArea(numFaces);	//记录每个面片的面积
	for (int i = 0; i < numFaces; i++)
	{
		Vector3d v0(model->vertices[3 * model->triangles[i].vindices[0] + 0],
			model->vertices[3 * model->triangles[i].vindices[0] + 1],
			model->vertices[3 * model->triangles[i].vindices[0] + 2]);

		Vector3d v1(model->vertices[3 * model->triangles[i].vindices[1] + 0],
			model->vertices[3 * model->triangles[i].vindices[1] + 1],
			model->vertices[3 * model->triangles[i].vindices[1] + 2]);

		Vector3d v2(model->vertices[3 * model->triangles[i].vindices[2] + 0],
			model->vertices[3 * model->triangles[i].vindices[2] + 1],
			model->vertices[3 * model->triangles[i].vindices[2] + 2]);

		faceArea[i] = modeltool::CalcTriArea(v0, v1, v2);
	}


	vector<CBoundingBox> errorBoxs;
	vector<CBoundingBox> missBoxs;
	Evaluation eval;
	eval.numFaces = numFaces;
	eval.LoadData(gtFile, predFile_rgb);
	eval.faceArea = faceArea;
	
	string bBoxFile = "";

	std::cout << endl;
	std::cout << "Building_RGB:" << endl;
	eval.GetErrorMissInsBox(errorBoxs, missBoxs, model, 0.5);
	bBoxFile = "F:/StudyDemo/Label3D/Label3D/data/project1/building/ErrorMisBoxs/buildings_rgb_50.json";
	WriteBBoxFile(errorBoxs, missBoxs, bBoxFile);
	std::cout << endl;
	eval.GetErrorMissInsBox(errorBoxs, missBoxs, model, 0.75);
	bBoxFile = "F:/StudyDemo/Label3D/Label3D/data/project1/building/ErrorMisBoxs/buildings_rgb_75.json";
	WriteBBoxFile(errorBoxs, missBoxs, bBoxFile);
	std::cout << endl;

	eval.LoadPredictionData(predFile_rgbh);
	std::cout << endl;
	std::cout << "Building_RGBH:" << endl;
	eval.GetErrorMissInsBox(errorBoxs, missBoxs, model, 0.5);
	bBoxFile = "F:/StudyDemo/Label3D/Label3D/data/project1/building/ErrorMisBoxs/buildings_rgbh_50.json";
	WriteBBoxFile(errorBoxs, missBoxs, bBoxFile);
	std::cout << endl;
	eval.GetErrorMissInsBox(errorBoxs, missBoxs, model, 0.75);
	bBoxFile = "F:/StudyDemo/Label3D/Label3D/data/project1/building/ErrorMisBoxs/buildings_rgbh_75.json";
	WriteBBoxFile(errorBoxs, missBoxs, bBoxFile);
	std::cout << endl;

	eval.LoadPredictionData(predFile_speK1);
	std::cout << endl;
	std::cout << "Building_Spe_K1:" << endl;
	eval.GetErrorMissInsBox(errorBoxs, missBoxs, model, 0.5);
	bBoxFile = "F:/StudyDemo/Label3D/Label3D/data/project1/building/ErrorMisBoxs/buildings_speK1_50.json";
	WriteBBoxFile(errorBoxs, missBoxs, bBoxFile);
	std::cout << endl;
	eval.GetErrorMissInsBox(errorBoxs, missBoxs, model, 0.75);
	bBoxFile = "F:/StudyDemo/Label3D/Label3D/data/project1/building/ErrorMisBoxs/buildings_speK1_75.json";
	WriteBBoxFile(errorBoxs, missBoxs, bBoxFile);
	std::cout << endl;

	eval.LoadPredictionData(predFile_speK2);
	std::cout << endl;
	std::cout << "Building_Spe_K2:" << endl;
	eval.GetErrorMissInsBox(errorBoxs, missBoxs, model, 0.5);
	bBoxFile = "F:/StudyDemo/Label3D/Label3D/data/project1/building/ErrorMisBoxs/buildings_speK2_50.json";
	WriteBBoxFile(errorBoxs, missBoxs, bBoxFile);
	std::cout << endl;
	eval.GetErrorMissInsBox(errorBoxs, missBoxs, model, 0.75);
	bBoxFile = "F:/StudyDemo/Label3D/Label3D/data/project1/building/ErrorMisBoxs/buildings_speK2_75.json";
	WriteBBoxFile(errorBoxs, missBoxs, bBoxFile);
	std::cout << endl;

	eval.LoadPredictionData(predFile_NoTheta);
	std::cout << endl;
	std::cout << "Building_NoTheta:" << endl;
	eval.GetErrorMissInsBox(errorBoxs, missBoxs, model, 0.5);
	bBoxFile = "F:/StudyDemo/Label3D/Label3D/data/project1/building/ErrorMisBoxs/buildings_NoTheta_50.json";
	WriteBBoxFile(errorBoxs, missBoxs, bBoxFile);
	std::cout << endl;
	eval.GetErrorMissInsBox(errorBoxs, missBoxs, model, 0.75);
	bBoxFile = "F:/StudyDemo/Label3D/Label3D/data/project1/building/ErrorMisBoxs/buildings_NoTheta_75.json";
	WriteBBoxFile(errorBoxs, missBoxs, bBoxFile);
	std::cout << endl;

	eval.LoadPredictionData(predFile_NoD);
	std::cout << endl;
	std::cout << "Building_NoD:" << endl;
	eval.GetErrorMissInsBox(errorBoxs, missBoxs, model, 0.5);
	bBoxFile = "F:/StudyDemo/Label3D/Label3D/data/project1/building/ErrorMisBoxs/buildings_NoD_50.json";
	WriteBBoxFile(errorBoxs, missBoxs, bBoxFile);
	std::cout << endl;
	eval.GetErrorMissInsBox(errorBoxs, missBoxs, model, 0.75);
	bBoxFile = "F:/StudyDemo/Label3D/Label3D/data/project1/building/ErrorMisBoxs/buildings_NoD_75.json";
	WriteBBoxFile(errorBoxs, missBoxs, bBoxFile);
	std::cout << endl;
	std::cout << "输出全部包围框完成" << endl;
}

//去除竖直面片
bool CMyTest::RemoveVerFaces(const MeshGraph* meshGraph, const string &roofFile, const string &saveFile)
{
	vector<int> segInfo = filetool::ReadTxtFileInt(roofFile);
	if (meshGraph->numTriangles != segInfo.size())
	{
		return false;
	}
	for (size_t i = 0; i < meshGraph->numTriangles; i++)
	{
		if (meshGraph->triangles[i].ifVertical60)
		{
			segInfo[i] = -1;
		}
	}

	int maxIndex = *std::max_element(segInfo.begin(), segInfo.end());
	int instanceCount = maxIndex + 2;
	vector<vector<int>> instanceFaces(instanceCount);
	for (size_t i = 0; i < meshGraph->numTriangles; i++)
	{
		instanceFaces[segInfo[i] + 1].push_back(i);
	}

	vector<vector<int>> insFaces_noEmpty;	//每个实例包含的面片索引,第0项为背景
	for (size_t i = 0; i < instanceCount; i++)
	{
		if (instanceFaces[i].size() > 0)
		{
			insFaces_noEmpty.push_back(instanceFaces[i]);
		}
	}

	instanceCount = insFaces_noEmpty.size();
	vector<int> segInfo_new(meshGraph->numTriangles);	//标注信息
	for (size_t i = 0; i < meshGraph->numTriangles; i++)
	{
		segInfo_new[i] = -1;
	}
	if (instanceCount > 1)
	{
		for (size_t i = 1; i < instanceCount; i++)
		{
			int numInsFace = insFaces_noEmpty[i].size();
			for (size_t j = 0; j < numInsFace; j++)
			{
				segInfo_new[insFaces_noEmpty[i][j]] = i - 1;
			}
		}
	}

	bool isOk = filetool::WriteTXTFileInt(segInfo_new, saveFile);
	return isOk;
}

//去除竖直面片
bool CMyTest::RemoveVerFaces()
{
	string modelFile = "F:/StudyDemo/Label3D/Label3D/data/project1/wangcun.obj";
	string roofFile_gt = "F:/StudyDemo/Label3D/Label3D/data/project1/roof_opti/back/roofs_gt.txt";
	string saveFile_gt = "F:/StudyDemo/Label3D/Label3D/data/project1/roof_opti/roofs_gt.txt";
	string roofFile_rgb = "F:/StudyDemo/Label3D/Label3D/data/project1/roof_opti/back/roofs_rgb.txt";
	string saveFile_rgb = "F:/StudyDemo/Label3D/Label3D/data/project1/roof_opti/roofs_rgb.txt";
	string roofFile_rgbh = "F:/StudyDemo/Label3D/Label3D/data/project1/roof_opti/back/roofs_rgbh.txt";
	string saveFile_rgbh = "F:/StudyDemo/Label3D/Label3D/data/project1/roof_opti/roofs_rgbh.txt";
	string roofFile_sep_k1 = "F:/StudyDemo/Label3D/Label3D/data/project1/roof_opti/back/roofs_spe_k1.txt";
	string saveFile_sep_k1 = "F:/StudyDemo/Label3D/Label3D/data/project1/roof_opti/roofs_spe_k1.txt";
	string roofFile_sep_k2 = "F:/StudyDemo/Label3D/Label3D/data/project1/roof_opti/back/roofs_spe_k2.txt";
	string saveFile_sep_k2 = "F:/StudyDemo/Label3D/Label3D/data/project1/roof_opti/roofs_spe_k2.txt";



	GLMmodel* model = glmReadOBJ((char*)modelFile.c_str());
	MeshGraph* meshGraph = new MeshGraph();
	meshGraph->CreateGraph(model, true, false);

	try
	{
		RemoveVerFaces(meshGraph, roofFile_gt, saveFile_gt);
		RemoveVerFaces(meshGraph, roofFile_rgb, saveFile_rgb);
		RemoveVerFaces(meshGraph, roofFile_rgbh, saveFile_rgbh);
		RemoveVerFaces(meshGraph, roofFile_sep_k1, saveFile_sep_k1);
		RemoveVerFaces(meshGraph, roofFile_sep_k2, saveFile_sep_k2);

		std::cout << "去除竖直面片完成！" << endl;
		return true;
	}
	catch (const std::exception&)
	{
		std::cout << "去除竖直面片失败！" << endl;
		return false;
	}
}

//k1的131  = ours的167
//k2的121  = Compair_Ours-back2的114
void CMyTest::TestAddVec()
{
	string predFile_ours = "F:/StudyDemo/Label3D/Label3D/data/project1/compair/Compair_Ours-back2.txt";
	string predFile_spek2 = "F:/StudyDemo/Label3D/Label3D/data/project1/compair/Spe_K2.txt";

	string saveFile = "F:/StudyDemo/Label3D/Label3D/data/project1/compair/Compair_Ours.txt";

	vector<int> ours = filetool::ReadTxtFileInt(predFile_ours);
	vector<int> spek2 = filetool::ReadTxtFileInt(predFile_spek2);

	int numFaces = ours.size();

	for (size_t i = 0; i < numFaces; i++)
	{
		if (ours[i] == 114)
		{
			ours[i] = -1;
		}
	}
	for (size_t i = 0; i < numFaces; i++)
	{
		if (spek2[i] == 121)
		{
			ours[i] = 114;
		}
	}

	bool isOk = filetool::WriteTXTFileInt(ours, saveFile);
	std::cout << "完成！" << endl;
}

void CMyTest::TestSwapIns()
{
	string predFile_gt = "F:/StudyDemo/Label3D/Label3D/data/project5/building/buildings_gt.txt";

	string saveFile = "F:/StudyDemo/Label3D/Label3D/data/project5/building/buildings_gt2.txt";

	vector<int> gt = filetool::ReadTxtFileInt(predFile_gt);

	int numFaces = gt.size();

	for (size_t i = 0; i < numFaces; i++)
	{
		if (gt[i] == 17)
		{
			gt[i] = 270;
		}
	}


	bool isOk = filetool::WriteTXTFileInt(gt, saveFile);
	std::cout << "完成！" << endl;
}

//将顶点索引列表编码为字符串
string EncodeVer3d(const Vector3d &point)
{
	string s = "";
	for (int i = 0; i < 3; i++)
	{
		stringstream ss;
		ss << point[i];
		string s1 = ss.str();

		s = s + s1 + " ";
	}

	return s;
}

//将字符串解码为顶点索引列表
Vector3d DecodeVer3d(string &pString)
{
	vector<double> pList;
	vector<string> sList;

	filetool::SplitString(filetool::Trim(pString), sList, " ");
	for (int i = 0; i < sList.size(); i++)
	{
		pList.push_back(atof(sList[i].c_str()));
	}
	Vector3d point(pList[0], pList[1], pList[2]);
	return point;
}

bool CMyTest::WriteBBoxFile(const vector<CBoundingBox> &errorBoxs, const vector<CBoundingBox> &missBoxs, const string& fileName)
{
	//根节点
	Json::Value root;

	int errorCount = errorBoxs.size();
	for (int i = 0; i < errorCount; i++)
	{
		Json::Value errorBoxNode;
		for (size_t j = 0; j < errorBoxs[i].box.size(); j++)
		{
			Json::Value pointNode;
			pointNode = EncodeVer3d(errorBoxs[i].box[j]);
			errorBoxNode.append(pointNode);
		}
		root["errorBoxs"].append(errorBoxNode);
	}

	int missCount = missBoxs.size();
	for (int i = 0; i < missCount; i++)
	{
		Json::Value missBoxNode;
		for (size_t j = 0; j < missBoxs[i].box.size(); j++)
		{
			Json::Value pointNode;
			pointNode = EncodeVer3d(missBoxs[i].box[j]);
			missBoxNode.append(pointNode);
		}
		root["missBoxs"].append(missBoxNode);
	}

	Json::StreamWriterBuilder  builder;
	builder["commentStyle"] = "None";
	builder["indentation"] = "	";
	std::unique_ptr<Json::StreamWriter> writer(builder.newStreamWriter());

	try
	{
		std::ofstream ofs;
		ofs.open(fileName, std::ios::app);
		writer->write(root, &ofs);
	}
	catch (const std::exception& e)
	{
		return false;
	}

	return true;
}

bool CMyTest::ReadBBoxFile(vector<CBoundingBox> &errorBoxs, vector<CBoundingBox> &missBoxs, const string& fileName)
{
	errorBoxs.clear();
	missBoxs.clear();

	ifstream ifs(fileName);
	Json::CharReaderBuilder readerbuilder;
	//Json::Reader reader;
	Json::Value root;
	JSONCPP_STRING errs;
	bool ok = Json::parseFromStream(readerbuilder, ifs, &root, &errs);
	if (ok)
	{
		Json::Value& errorBox_Root = root["errorBoxs"];
		Json::Value& missBox_Root = root["missBoxs"];

		int errorCount = errorBox_Root.size();
		for (int i = 0; i < errorCount; i++)
		{
			CBoundingBox errBox;
			vector<Vector3d> boundingBox_3d;
			for (int j = 0; j < errorBox_Root[i].size(); j++)
			{
				boundingBox_3d.push_back(DecodeVer3d(errorBox_Root[i][j].asString()));
			}
			errBox.setBox(boundingBox_3d);
			errBox.CalcDirs();
			errBox.CalcPos();
			errorBoxs.push_back(errBox);
		}

		int missCount = missBox_Root.size();
		for (int i = 0; i < missCount; i++)
		{
			CBoundingBox missBox;
			vector<Vector3d> boundingBox_3d;
			for (int j = 0; j < missBox_Root[i].size(); j++)
			{
				boundingBox_3d.push_back(DecodeVer3d(missBox_Root[i][j].asString()));
			}
			missBox.setBox(boundingBox_3d);
			missBox.CalcDirs();
			missBox.CalcPos();
			missBoxs.push_back(missBox);
		}
	}
	return true;
}

void CMyTest::TestReadBBoxFile()
{
	vector<CBoundingBox> errorBoxs;
	vector<CBoundingBox> missBoxs;

	string bBoxFile = "F:/StudyDemo/Label3D/Label3D/data/project5/building/ErrorMisBoxs/buildings_NoD_75.json";
	ReadBBoxFile(errorBoxs, missBoxs, bBoxFile);

	int errBoxCount = errorBoxs.size();
}

void CMyTest::TestCamera()
{
	easy3d::Camera camera;

}

//将原本txt标注文件转换为json
void CMyTest::TransLabelFile()
{
	string txtFile = "data/project1/buildings_gt.txt";
	string jsonFile = "data/project1/buildings_gt.json";

	vector<int> segInfo_txt = filetool::ReadTxtFileInt(txtFile);

	int numFaces = segInfo_txt.size();
	vector<Vector2i> segInfo_json(numFaces);
	for (size_t i = 0; i < numFaces; i++)
	{
		segInfo_json[i] = Vector2i(-1, -1);
	}

	for (size_t fi = 0; fi < numFaces; fi++)
	{
		if (segInfo_txt[fi] >=0 )
		{
			segInfo_json[fi][0] = 0;
			segInfo_json[fi][1] = segInfo_txt[fi];
		}
	}

	//根节点
	Json::Value root;

	root["categories"] = Json::arrayValue;
	for (size_t ci = 0; ci < 1; ci++)
	{
		Json::Value cate;
		cate["categoryId"] = 0;
		cate["categoryName"] = "building";
		cate["numInstance"] = 400;
		root["categories"].append(cate);
	}

	root["numFaces"] = numFaces;
	root["segInfo"] = Json::arrayValue;
	for (int fi = 0; fi < numFaces; fi++)
	{
		Json::Value faceInfo;
		faceInfo.append(segInfo_json[fi][0]);
		faceInfo.append(segInfo_json[fi][1]);
		root["segInfo"].append(faceInfo);
	}

	Json::StreamWriterBuilder  builder;
	builder["commentStyle"] = "None";
	builder["indentation"] = "	";
	std::unique_ptr<Json::StreamWriter> writer(builder.newStreamWriter());

	try
	{
		std::ofstream ofs;
		//ofs.open(jsonFile, std::ios::app);
		ofs.open(jsonFile);
		writer->write(root, &ofs);
		std::cout << "保存文件成功！" << endl;
	}
	catch (const std::exception& e)
	{
		return;
	}
	return;
}

//合并原图image和分割结果图segMap
void MergeMats(Mat& image, const Mat& segMap)
{
	vector<Vector3f> colors;
	srand(time(0));
	for (int i = 0; i < 350; i++)
	{
		Vector3f color;
		color[0] = (float)Random();
		color[1] = (float)Random();
		color[2] = (float)Random();
		colors.push_back(color);
	}

	std::cout << "segMap.width = " << segMap.cols << endl;
	std::cout << "segMap.height = " << segMap.rows << endl;

	std::cout << "image.width = " << image.cols << endl;
	std::cout << "image.height = " << image.rows << endl;

	for (int row = 0; row < image.rows; row++)
	{
		for (int col = 0; col < image.cols; col++)
		{
			//取sgeMap[row][col]处对应的color
			int insIndex = segMap.at<int >(row, col);

			if (insIndex >= 0)
			{
				//insIndex = insIndex % 30;
				Vector3f insColor = colors[insIndex] * 255;
				//int cha = image.channels();
				for (int channel = 0; channel < image.channels(); channel++)
				{
					image.at<cv::Vec3b>(row, col)[channel] = image.at<cv::Vec3b>(row, col)[channel] * 0.3 + insColor[channel] * 0.7;
				}
			}
		}
	}
}

//将分割结果图SegMap中的实例使用RLE行程编码
void GetRLEMaskFromSegmap(const Mat& segMap, const string& photoName, const string& jsonSavePath)
{
	vector<RLE> masks;

	string folder;
	vector<string> strs1;
	SplitStringTo2(photoName, strs1, ".");
	string pic_firstName = strs1[0];
	folder = jsonSavePath + "/" + strs1[0];

	if (_access(folder.c_str(), 0) == -1) //如果文件夹不存在
	{
		_mkdir(folder.c_str());				//则创建

	}

	bool maskVisited[999] = { false };
	int imgHeight = segMap.rows;
	int imgWidth = segMap.cols;

	for (size_t row = 0; row < segMap.rows; row++)
	{
		for (size_t col = 0; col < segMap.cols; col++)
		{
			int insIndex = segMap.at<int >(row, col);
			if (insIndex < 0)
			{
				continue;
			}

			if (!maskVisited[insIndex])
			{
				Mat maskImg(imgHeight, imgWidth, CV_8U);
				unsigned char* sData = (unsigned char*)maskImg.data;
				//遍历segMap，将segMap中所有=insIndex的像素处对应的maskImg处的值赋值为1,否则赋值为0
				for (int i = 0; i<imgHeight; i++)
				{
					for (int j = 0; j<imgWidth; j++)
					{
						if (segMap.at<int >(i, j) == insIndex)
						{
							*(sData) = 1;
						}
						else
						{
							*(sData) = 0;
						}
						sData++;
					}
				}
				maskVisited[insIndex] = true;

				//将maskImg保存为灰度图
				stringstream id_str;
				id_str << insIndex;

				string photoFile = folder + "/" + pic_firstName + "-" + id_str.str() + ".jpg";
				cv::imwrite(photoFile, maskImg);
			}
		}
	}
}

//对segMap里的实例编号重新排序，从startId开始
Mat ResortSegMap(const Mat& segMap, int startId)
{
	int imgHeight = segMap.rows;
	int imgWidth = segMap.cols;

	Mat sortedMap(imgHeight, imgWidth, CV_32S);
	int* sortMapData = (int*)sortedMap.data;
	//初始化为0，表示所有像素都为背景
	for (int i = 0; i<imgHeight; i++)
	{
		for (int j = 0; j<imgWidth; j++)
		{
			*(sortMapData) = 0;
			sortMapData++;
		}
	}

	bool** mapVisited = new bool*[imgHeight];	//记录像素是否已经遍历
	for (size_t i = 0; i < imgHeight; i++)
	{
		mapVisited[i] = new bool[imgWidth];
	}
	for (int i = 0; i < imgHeight; i++)
	{
		for (int j = 0; j < imgWidth; j++)
		{
			mapVisited[i][j] = false;
		}
	}

	int* sData = (int*)segMap.data;
	sortMapData = (int*)sortedMap.data;
	for (int i = 0; i < imgHeight; i++)
	{
		for (int j = 0; j < imgWidth; j++)
		{
			if (mapVisited[i][j] == false)
			{
				int oldInsIndex = sData[i* imgWidth + j];
				if (oldInsIndex >= 0)
				{
					for (int rowIndex = 0; rowIndex < imgHeight; rowIndex++)
					{
						for (int colIndex = 0; colIndex < imgWidth; colIndex++)
						{
							if (mapVisited[rowIndex][colIndex] == false)
							{
								if (sData[rowIndex* imgWidth + colIndex] == oldInsIndex)
								{
									sortMapData[rowIndex* imgWidth + colIndex] = startId;
									mapVisited[rowIndex][colIndex] = true;
								}
							}
						}
					}
					startId++;
				}
			}
			mapVisited[i][j] == true;
		}
	}

	for (int i = 0; i < imgHeight; ++i)
	{
		delete[] mapVisited[i];
	}
	delete[]mapVisited;

	return sortedMap;
}

//将三维标注投影到二维图像上
void CMyTest::Label3DTo2D()
{
	long t1 = time(NULL);

	string modelFile = "data/project1/wangcun.obj";	//模型文件
	string photoPath = "data/project1/photos";	//照片路径
	string photoInfoFile = "data/project1/photos-info-pyr.xml";	//照片信息文件
	string LabelFile_3D = "data/project1/buildings_gt2.json";	//3D标注文件

	//1、读取照片列表
	vector<string> photos;
	filetool::GetJustCurrentFile(photoPath, photos);

	//2、读取照片相机信息
	vector<SPhotoGroup> photoGroups = phototool::RPhotoGroupFromXML(photoInfoFile);
	int numImages = photos.size();

	//3、读取模型
	GLMmodel* model = glmReadOBJ((char*)modelFile.c_str());
	std::cout << "Create model graph ......" << endl;
	MeshGraph *meshGraph = new MeshGraph();
	meshGraph->CreateGraph(model, false, false);	//创建模型面片的邻接图
	LabelInfo* labelInfo = new LabelInfo(model);

	//4、读取3D标注
	if (!labelInfo->ReadFromFile(LabelFile_3D))
	{
		return;
	}
	/*将labelInfo中的所有实例整合到一个大的实例列表
	例如将building-0,building-1,building-2 ...... building-50,
	car-0,car-1...... car-42
	people-0,people-1......people-34
	整合为：
	ins-0,ins-2......ins-50,ins-51......ins-91,ins-92......ins-125
	存入faceSegResult，(-1为背景)
	*/
	int numFaces = meshGraph->triangles.size();
	vector<int> faceSegResult(numFaces);
	for (size_t i = 0; i < numFaces; i++)
	{
		faceSegResult[i] = -1;
	}
	int currentInsIndex = 0;
	for (size_t ci = 0; ci < labelInfo->categories.size(); ci++)
	{
		for (size_t ii = 0; ii < labelInfo->categories[ci].instances.size(); ii++)
		{
			int numInsFaces = labelInfo->categories[ci].instances[ii].insFaces.size();
			for (size_t fi = 0; fi < numInsFaces; fi++)
			{
				faceSegResult[labelInfo->categories[ci].instances[ii].insFaces[fi]] = currentInsIndex;
			}
			currentInsIndex++;
		}
	}

	vector<int> compression_params;
	compression_params.push_back(CV_IMWRITE_JPEG_QUALITY);  //选择jpeg
	compression_params.push_back(100); //在这个填入你要的图片质量
	int startId = 1;
	for (int i = 0; i < numImages; i++)
	{
		long t2 = time(NULL);
		SPhotoInfo photoInfo;
		SCameraParam cameraParam;
		std::cout << "计算第" << i << "张图片" << endl;
		//根据照片名找到photoInfo
		bool ifFindPhotoInfo = false;
		for (int pg = 0; pg < photoGroups.size(); pg++)
		{
			for (int j = 0; j < photoGroups[pg].photoInfos.size(); j++)
			{
				if (photos[i] == photoGroups[pg].photoInfos[j].picName)
				{
					photoInfo = photoGroups[pg].photoInfos[j];
					cameraParam = photoGroups[pg].cameraParam;
					ifFindPhotoInfo = true;
					break;
				}
			}
		}
		if (!ifFindPhotoInfo)
		{
			std::cout << "未找到照片的相机参数:" << photos[i] << endl;
			continue;
		}

		//根据segMap,将其添加到原图上，生成分割图
		string imgName = photoPath + "/" + photos[i];
		Mat img = imread(imgName.c_str());

		/*将labelInfo中的所有实例整合到一个大的实例列表
		例如将building-1,building-2,building-3 ...... building-50,
			car-1,car-2...... car-42
			people-1,people-2......people-34
		整合为：
			ins-1,ins-2......ins-50,ins-51......ins-92,ins-93......ins-126
		*/

		Mat segMap = projectiontool::GetBackProjectedImgAerial(meshGraph, photoInfo, cameraParam, faceSegResult);

		//对segMap里的实例编号重新排序，从1开始
		Mat sortedMap = ResortSegMap(segMap, startId);
		
		string savedMaskImageName = "data/project1/photos-label2D-mask-sorted/" + filetool::GetFileNameNoSuffix(photos[i]) + ".png";
		cv::imwrite(savedMaskImageName, sortedMap, compression_params);

		//string jsonSavePath = projectRoot + "/JsonFile" + minMaskScoreStr + "/AccFile";
		//GetRLEMaskFromSegmap(segMap, photos[i], jsonSavePath);

		//MergeMats(img, segMap);
		//string savedImageName = "data/project1/photos-label2D/" + photos[i];
		//imwrite(savedImageName, img, compression_params);

		std::cout << "生成第 " << i << " 张照片完成：" << photos[i] << " : " << std::round(time(NULL) - t2) << " Seconds" << endl << endl;
	}
	std::cout << "任务完成！" << std::round(time(NULL) - t1) << " Seconds" << endl << endl;

	glmDelete(model);
}

Mat SortedMap2PanoMap(const Mat& SortedMap)
{
	int imgHeight = SortedMap.rows;
	int imgWidth = SortedMap.cols;

	Mat panopticMap(imgHeight, imgWidth, CV_32SC3);
	int* panoMapData = (int*)panopticMap.data;
	//初始化为0，表示所有像素都为背景
	for (int i = 0; i<imgHeight; i++)
	{
		for (int j = 0; j<imgWidth; j++)
		{
			for (int k = 0; k < 3; k++)
			{
				*(panoMapData) = 0;
				panoMapData++;
			}
		}
	}

	for (int i = 0; i < imgHeight; i++)
	{
		for (int j = 0; j < imgWidth; j++)
		{
			int segId = SortedMap.at<int>(i, j);
			int color_r = segId / (256 * 256);
			int color_g = (segId % (256 * 256)) / 256;
			int color_b = segId % 256;

			panopticMap.at<cv::Vec3i>(i, j)[0] = color_b;
			panopticMap.at<cv::Vec3i>(i, j)[1] = color_g;
			panopticMap.at<cv::Vec3i>(i, j)[2] = color_r;
		}
	}

	return panopticMap;
}

//根据segMap、sortedMap、insId_cateId_Table生成单张图片的panoptic标注
void CMyTest::SetSinglePanoLabel(panopticlabel::PanopticLabel* panoLabel,
	const Mat &segMap, const Mat &sortedMap,
	const vector<Vector2i> &insId_cateId_Table)
{
	panopticlabel::Annotation annotation;

	int maxSegId = 0;
	for (int row = 0; row < sortedMap.rows; row++)
	{
		for (int col = 0; col < sortedMap.cols; col++)
		{
			int segId = sortedMap.at<int >(row, col);
			maxSegId = maxSegId < segId ? segId : maxSegId;

		}
	}

	for (int si = 1; si < maxSegId; si++)
	{
		panopticlabel::SegmentInfo segInfo;
		segInfo.id = si;
		segInfo.iscrowd = 0;
		int categroyId = -1;
		for (int row = 0; row < sortedMap.rows && categroyId < 0; row++)
		{
			for (int col = 0; col < sortedMap.cols && categroyId < 0; col++)
			{
				if (sortedMap.at<int >(row, col) == si)
				{
					int insId = segMap.at<int >(row, col);
					for (int ci = 0; ci < insId_cateId_Table.size(); ci++)
					{
						if (insId_cateId_Table[ci][0] == insId)
						{
							categroyId = insId_cateId_Table[ci][1];
							break;
						}
					}
				}
			}
		}
		segInfo.category_id = categroyId;

		int area = 0;
		Vector2i minXY(sortedMap.cols, sortedMap.rows);
		Vector2i maxXY(0, 0);
		for (int row = 0; row < sortedMap.rows; row++)
		{
			for (int col = 0; col < sortedMap.cols; col++)
			{
				if (sortedMap.at<int>(row, col) == si)
				{
					area++;
					minXY[0] = minXY[0] > col ? col : minXY[0];
					minXY[1] = minXY[1] > row ? row : minXY[1];
					maxXY[0] = maxXY[0] < col ? col : maxXY[0];
					maxXY[1] = maxXY[1] < row ? row : maxXY[1];
				}
			}
		}
		segInfo.area = area;
		segInfo.bbox[0] = minXY[0];
		segInfo.bbox[1] = minXY[1];
		segInfo.bbox[2] = maxXY[0] - minXY[0] + 1;
		segInfo.bbox[3] = maxXY[1] - minXY[1] + 1;
		annotation.segments_info.push_back(segInfo);
	}
	panoLabel->annotations.push_back(annotation);
}

void CMyTest::Label3DTo2D(const string &modelFile, const string &photoPath,
	const string &photoInfoFile, const string &LabelFile_3D,
	const string &photoSavePath, const string& photoType)
{
	long t1 = time(NULL);

	//1、读取照片列表
	vector<string> photos;
	filetool::GetJustCurrentFile(photoPath, photos);

	//2、读取模型
	GLMmodel* model = glmReadOBJ((char*)modelFile.c_str());
	std::cout << "Create model graph ......" << endl;
	MeshGraph *meshGraph = new MeshGraph();
	meshGraph->CreateGraph(model, false, false);	//创建模型面片的邻接图
	LabelInfo* labelInfo = new LabelInfo(model);

	//3、读取3D标注
	if (!labelInfo->ReadFromFile(LabelFile_3D))
	{
		return;
	}
	/*将labelInfo中的所有实例整合到一个大的实例列表
	例如将building-0,building-1,building-2 ...... building-50,
	car-0,car-1...... car-42
	people-0,people-1......people-34
	整合为：
	seg-0,seg-1......seg-50,seg-51......seg-91,seg-92......seg-125
	存入faceSegResult，(-1为背景)
	*/
	int numFaces = meshGraph->triangles.size();
	vector<int> faceSegResult(numFaces);	//面片分割结果，背景为-1，segmentId从0开始
	vector<Vector2i> insId_cateId_Table;	//记录insId到类别Id对应查找表，背景为-1，insId从0开始的格式
	for (size_t i = 0; i < numFaces; i++)
	{
		faceSegResult[i] = -1;
	}
	int currentInsIndex = 0;
	for (size_t ci = 0; ci < labelInfo->categories.size(); ci++)
	{
		for (size_t ii = 0; ii < labelInfo->categories[ci].instances.size(); ii++)
		{
			int numInsFaces = labelInfo->categories[ci].instances[ii].insFaces.size();
			for (size_t fi = 0; fi < numInsFaces; fi++)
			{
				faceSegResult[labelInfo->categories[ci].instances[ii].insFaces[fi]] = currentInsIndex;
			}
			insId_cateId_Table.push_back(Vector2i(currentInsIndex, labelInfo->categories[ci].categoryId));
			currentInsIndex++;
		}
	}

	vector<int> compression_params;
	compression_params.push_back(CV_IMWRITE_JPEG_QUALITY);  //选择jpeg
	compression_params.push_back(100); //在这个填入你要的图片质量
	if (photoType == "Aerial")
	{
		//4、读取照片相机信息
		vector<SPhotoGroup> photoGroups = phototool::RPhotoGroupFromXML(photoInfoFile);
		int numImages = photos.size();
		for (int i = 0; i < numImages; i++)
		{
			long t2 = time(NULL);
			panopticlabel::PanopticLabel panoLabel;

			SPhotoInfo photoInfo;
			SCameraParam cameraParam;
			std::cout << "计算第" << i << "张图片" << endl;
			//根据照片名找到photoInfo
			bool ifFindPhotoInfo = false;
			for (int pg = 0; pg < photoGroups.size(); pg++)
			{
				for (int j = 0; j < photoGroups[pg].photoInfos.size(); j++)
				{
					if (photos[i] == photoGroups[pg].photoInfos[j].picName)
					{
						photoInfo = photoGroups[pg].photoInfos[j];
						cameraParam = photoGroups[pg].cameraParam;
						ifFindPhotoInfo = true;
						break;
					}
				}
			}
			if (!ifFindPhotoInfo)
			{
				std::cout << "未找到照片的相机参数:" << photos[i] << endl;
				continue;
			}

			//根据segMap,将其添加到原图上，生成分割图
			string imgName = photoPath + "/" + photos[i];
			Mat img = imread(imgName.c_str());

			/*将labelInfo中的所有实例整合到一个大的实例列表
			例如将building-1,building-2,building-3 ...... building-50,
			car-1,car-2...... car-42
			people-1,people-2......people-34
			整合为：
			ins-1,ins-2......ins-50,ins-51......ins-92,ins-93......ins-126
			*/

			Mat segMap = projectiontool::GetBackProjectedImgAerial(meshGraph, photoInfo, cameraParam, faceSegResult);

			//对segMap里的实例编号重新排序，从1开始
			Mat sortedMap = ResortSegMap(segMap, 1);
			//将单通道的sortedMap转换为三通道的panopticMap: 
			//sortedMap = panopticMap.R + panopticMap.G * 256 + panopticMap.B * 256 * 256
			Mat panopticMap = SortedMap2PanoMap(sortedMap);
			SetSinglePanoLabel(&panoLabel, segMap, sortedMap, insId_cateId_Table);
			string picPanoLabelFile = photoSavePath + filetool::GetFileNameNoSuffix(photos[i]) + ".json";
			panoLabel.WriteToFile(picPanoLabelFile);

			string savedMaskImageName = photoSavePath + filetool::GetFileNameNoSuffix(photos[i]) + ".png";
			cv::imwrite(savedMaskImageName, panopticMap, compression_params);

			std::cout << "生成第 " << i << " 张照片完成：" << photos[i] << " : " << std::round(time(NULL) - t2) << " Seconds" << endl << endl;
		}
	}
	else if (photoType == "OpenGL")
	{
		//4、读取照片相机信息
		vector<SGlCameraParam> cameInfos = phototool::RSPOpenGLCParamInfosJson(photoInfoFile);
		int numImages = cameInfos.size();
		for (int i = 0; i < numImages; i++)
		{
			long t2 = time(NULL);
			panopticlabel::PanopticLabel panoLabel;

			SGlCameraParam cameraParam;
			std::cout << "计算第" << i << "张图片" << endl;
			cameraParam = cameInfos[i];

			//根据segMap,将其添加到原图上，生成分割图
			string imgName = photoPath + "/" + photos[i];
			Mat img = imread(imgName.c_str());

			Mat segMap = projectiontool::GetBackProjectedImgOpenGL(meshGraph, cameraParam, faceSegResult);
			//对segMap里的实例编号重新排序，从1开始
			Mat sortedMap = ResortSegMap(segMap, 1);

			//将单通道的sortedMap转换为三通道的panopticMap: 
			//sortedMap = panopticMap.R + panopticMap.G * 256 + panopticMap.B * 256 * 256
			Mat panopticMap = SortedMap2PanoMap(sortedMap);
			SetSinglePanoLabel(&panoLabel, segMap, sortedMap, insId_cateId_Table);
			string picPanoLabelFile = photoSavePath + filetool::GetFileNameNoSuffix(photos[i]) + ".json";
			panoLabel.WriteToFile(picPanoLabelFile);

			string savedMaskImageName = photoSavePath + filetool::GetFileNameNoSuffix(photos[i]) + ".png";
			imwrite(savedMaskImageName, panopticMap, compression_params);

			std::cout << "生成第 " << i << " 张照片完成：" << photos[i] << " : " << std::round(time(NULL) - t2) << " Seconds" << endl << endl;

		}
	}
	std::cout << "任务完成！" << std::round(time(NULL) - t1) << " Seconds" << endl << endl;

	glmDelete(model);
}

//解析segMap，将segMap的Masks解析并分开保存
void readMaskMap(const Mat &segMap, const string &saveRoot)
{
	int imgHeight = segMap.rows;
	int imgWidth = segMap.cols;
	int channels = segMap.channels();

	bool** mapVisited = new bool*[imgHeight];	//记录像素是否已经遍历
	for (size_t i = 0; i < imgHeight; i++)
	{
		mapVisited[i] = new bool[imgWidth];
	}
	for (int i = 0; i < imgHeight; i++)
	{
		for (int j = 0; j < imgWidth; j++)
		{
			mapVisited[i][j] = false;
		}
	}

	vector<int> compression_params;
	compression_params.push_back(CV_IMWRITE_JPEG_QUALITY);  //选择jpeg
	compression_params.push_back(100); //在这个填入你要的图片质量
	int* sData = (int*)segMap.data;
	for (int row = 0; row < segMap.rows; row++)
	{
		for (int col = 0; col < segMap.cols; col++)
		{
			if (mapVisited[row][col] == false)
			{
				int insIndex = (int)segMap.at<uchar>(row, col);
				if (insIndex > 0)
				{
					Mat singleMaskMap(imgHeight, imgWidth, CV_32S);
					int *singleMaskMapData = (int*)singleMaskMap.data;
					for (int rowIndex = 0; rowIndex < imgHeight; rowIndex++)
					{
						for (int colIndex = 0; colIndex < imgWidth; colIndex++)
						{
							if ((int)segMap.at<uchar>(rowIndex, colIndex) == insIndex)
							{
								singleMaskMapData[rowIndex* imgWidth + colIndex] = 255;
								mapVisited[rowIndex][colIndex] = true;
							}
							else
							{
								singleMaskMapData[rowIndex* imgWidth + colIndex] = 0;
							}
						}
					}
					string saveMaskImgName = saveRoot + filetool::ItoS(insIndex) + ".png";
					imwrite(saveMaskImgName, singleMaskMap, compression_params);
					std::cout << "输出第 " << insIndex << " 个Mask照片完成..." << endl;
				}
				mapVisited[row][col] == true;
			}
		}
	}

	for (int i = 0; i < imgHeight; ++i)
	{
		delete[] mapVisited[i];
	}
	delete[]mapVisited;

	std::cout << "输出照片的所有Mask完成..." << endl;
}

//提取Mask边界
Mat GetMaskEdge(const Mat &maskMap)
{
	int imgHeight = maskMap.rows;
	int imgWidth = maskMap.cols;
	int channels = maskMap.channels();

	Mat edgeMap(imgHeight, imgWidth, CV_32S);
	int *edgeMapData = (int*)edgeMap.data;
	for (int i = 0; i < imgHeight; i++)
	{
		for (int j = 0; j < imgWidth; j++)
		{
			edgeMapData[i* imgWidth + j] = 0;
		}
	}

	for (int row = 0; row < imgHeight; row++)
	{
		for (int col = 0; col < imgWidth; col++)
		{
			if ((int)maskMap.at<uchar>(row, col) == 0)
			{
				continue;
			}
			bool isEdge = false;
			for (int i = row - 1; i < row + 1; i++)
			{
				for (int j = col - 1; j < col + 1; j++)
				{
					if (i >= 0 && i < imgHeight && j >= 0 && j < imgWidth)
					{
						if ((int)maskMap.at<uchar>(i, j) == 0)
						{
							isEdge = true;
						}
					}
				}
			}
			if (isEdge)
			{
				edgeMapData[row* imgWidth + col] = 255;
			}
		}
	}
	return edgeMap;
}

//测试编码2D Mask
void CMyTest::EncodeMask()
{
	string testPhoto = "A1 (191).png";
	string photoPath = "data/project1/photos/";	//照片路径
	string photoMaskPath = "data/project1/photos-label2D-mask-sorted/";	//照片Mask路径

	//提取单个mask
	string singlePhotoMasksPath = "data/project1/photos-singleMasks/A1 (191)/";
	Mat segMap = cv::imread(photoMaskPath + testPhoto, 0);
	readMaskMap(segMap, singlePhotoMasksPath);

	vector<int> compression_params;
	compression_params.push_back(CV_IMWRITE_JPEG_QUALITY);  //选择jpeg
	compression_params.push_back(100); //在这个填入你要的图片质量

	////提取mask边界
	//vector<string> singleMaskFiles;
	//filetool::GetJustCurrentFile(singlePhotoMasksPath, singleMaskFiles);
	//string singlePhotoMasksEdgePath = "data/project1/photos-singleMasks/A1 (167)-edge/";
	//for (size_t i = 0; i < singleMaskFiles.size(); i++)
	//{
	//	Mat singleMask = cv::imread(singlePhotoMasksPath + singleMaskFiles[i], 0);
	//	Mat singleMaskEdge = GetMaskEdge(singleMask);
	//	string maskEdgePhotoName = singlePhotoMasksEdgePath + singleMaskFiles[i];
	//	imwrite(maskEdgePhotoName, singleMaskEdge, compression_params);
	//	std::cout << "提取第 " << i << " 个Mask边缘照片完成..." << endl;
	//}
	//std::cout << "提取所有Mask边缘照片完成..." << endl;
}

//将原本的json标注文件转换为适应MSCOCO Panoptic标注类型的json文件
void CMyTest::TransLabelFile2COCO()
{
	string categoryFile = "data/label3d_categories.json";
	string resFile = "data/project5/buildings_gt.json";
	string dstFile = "data/project5/buildings_gt_coco.json";
	ifstream ifs(resFile);
	ifstream ifs_cate(categoryFile);
	Json::CharReaderBuilder readerbuilder;
	Json::CharReaderBuilder readerbuilder_cate;
	//Json::Reader reader;
	Json::Value root;
	Json::Value root_cate;
	JSONCPP_STRING errs;
	bool ok = Json::parseFromStream(readerbuilder, ifs, &root, &errs);
	bool ok_cate = Json::parseFromStream(readerbuilder_cate, ifs_cate, &root_cate, &errs);
	if (ok && ok_cate)
	{
		int numFaces = root["numFaces"].asInt();
		int num_buildings = root["categories"][0]["numInstance"].asInt();
		root["categories"] = root_cate["categories"];
		for (int i = 0; i < root["categories"].size(); i++)
		{
			root["categories"][i]["numInstance"] = 0;
		}
		root["categories"][0]["numInstance"] = num_buildings;
	}
	ifs.close();
	ifs_cate.close();

	Json::StreamWriterBuilder  builder;
	builder["commentStyle"] = "None";
	builder["indentation"] = "	";
	std::unique_ptr<Json::StreamWriter> writer(builder.newStreamWriter());
	try
	{
		std::ofstream ofs;
		//ofs.open(fileName, std::ios::app);	//不覆盖，在文件结尾写入
		ofs.open(dstFile);		//覆盖保存
		writer->write(root, &ofs);
		ofs.close();
		std::cout << "文件转换完成" << endl;
		return;
	}
	catch (const std::exception& e)
	{
		return;
	}
}

//将3D标注文件转换为Panoptic全景分割训练标注格式文件
void CMyTest::GenPanoLabel()
{
	string labelFile_3D = "data/project5/buildings_gt_coco.json";	//3D标注文件
	string photoPath = "data/project5/panoptic_label/photos";	//原始照片路径
	string photoPanoPath = "data/project5/panoptic_label/photos_panoptic/";	//生成的panoptic标注照片保存位置
	string photoInfoFile = "data/project5/panoptic_label/photos-info-pyr.json";	//照片信息文件
	string modelFile = "data/project5/dongkengkou.obj";	//模型文件

	Label3DTo2D(modelFile, photoPath, photoInfoFile, labelFile_3D, photoPanoPath, "OpenGL");
}

Mat ResetMatSegId(const Mat& panoMat, int offset)
{
	int imgHeight = panoMat.rows;
	int imgWidth = panoMat.cols;

	Mat panopticMap(imgHeight, imgWidth, CV_32SC3);
	int* panoMapData = (int*)panopticMap.data;
	//初始化为0，表示所有像素都为背景
	for (int i = 0; i<imgHeight; i++)
	{
		for (int j = 0; j<imgWidth; j++)
		{
			for (int k = 0; k < 3; k++)
			{
				*(panoMapData) = 0;
				panoMapData++;
			}
		}
	}

	for (int i = 0; i < imgHeight; i++)
	{
		for (int j = 0; j < imgWidth; j++)
		{
			int segId_r = panoMat.at<cv::Vec3b>(i, j)[2];
			int segId_g = panoMat.at<cv::Vec3b>(i, j)[1];
			int segId_b = panoMat.at<cv::Vec3b>(i, j)[0];
			int segId = segId_r + segId_g * 256 + segId_b * 256 * 256;
			if (segId > 0)
			{
				segId += offset;
				int color_r = segId / (256 * 256);
				int color_g = (segId % (256 * 256)) / 256;
				int color_b = segId % 256;

				panopticMap.at<cv::Vec3i>(i, j)[0] = color_b;
				panopticMap.at<cv::Vec3i>(i, j)[1] = color_g;
				panopticMap.at<cv::Vec3i>(i, j)[2] = color_r;
			}
		}
	}

	return panopticMap;
}

//合并所有照片的panoptic标注文件
void CMyTest::MergePanopticLabels()
{
	string photoPath = "data/project5/panoptic_label/photos";	//原始照片路径
	string photoInfoFile = "data/project5/panoptic_label/photos-info-pyr.json";	//照片信息文件
	string panoLabelFilePath = "data/project5/panoptic_label/photos_panoptic";	//panoptic标注文件位置
	string panoLabelPath_Merged = "data/project5/panoptic_label/photos_panoptic_merged/";
	string photoType = "OpenGL";

	//1、读取照片列表
	vector<string> photos;
	filetool::GetJustCurrentFile(photoPath, photos);
	int numImages = photos.size();

	vector<int> compression_params;
	compression_params.push_back(CV_IMWRITE_JPEG_QUALITY);  //选择jpeg
	compression_params.push_back(100); //在这个填入你要的图片质量

	panopticlabel::PanopticLabel panoLabel;
	if (photoType == "Aerial")
	{
		//4、读取照片相机信息
		vector<SPhotoGroup> photoGroups = phototool::RPhotoGroupFromXML(photoInfoFile);
		int segId = 1;
		for (int i = 0; i < numImages; i++)
		{
			long t2 = time(NULL);

			int segIdOffset = segId - 1;
			SPhotoInfo photoInfo;
			SCameraParam cameraParam;
			std::cout << "计算第" << i << "张图片" << endl;
			//根据照片名找到photoInfo
			bool ifFindPhotoInfo = false;
			for (int pg = 0; pg < photoGroups.size(); pg++)
			{
				for (int j = 0; j < photoGroups[pg].photoInfos.size(); j++)
				{
					if (photos[i] == photoGroups[pg].photoInfos[j].picName)
					{
						photoInfo = photoGroups[pg].photoInfos[j];
						cameraParam = photoGroups[pg].cameraParam;
						ifFindPhotoInfo = true;
						break;
					}
				}
			}

			panopticlabel::Image image;
			image.coco_url = "";
			image.date_captured = "2020";
			image.file_name = photoInfo.picName;
			image.flickr_url = "";
			image.height = photoInfo.height;
			image.id = i;
			image.license = 0;
			image.width = photoInfo.width;
			panoLabel.images.push_back(image);

			panopticlabel::License license;
			license.id = 0;
			license.name = "license_0";
			license.url = "";
			panoLabel.licenses.push_back(license);

			string annotationFile = panoLabelFilePath + "/" + filetool::GetFileNameNoSuffix(photos[i]) + ".json";
			panopticlabel::Annotation annotation = panopticlabel::ReadAnnotation(annotationFile);
			annotation.file_name = photos[i];
			annotation.image_id = i;
			for (int si = 0; si < annotation.segments_info.size(); si++)
			{
				annotation.segments_info[si].id = segId;
				segId++;
			}
			panoLabel.annotations.push_back(annotation);

			//更新panoptic标注图片每个像素的segId后重新保存
			string panoImg = panoLabelFilePath + "/" + filetool::GetFileNameNoSuffix(photos[i]) + ".png";
			cv::Mat panoMat = cv::imread(panoImg, 1);
			cv::Mat panoMat_new = ResetMatSegId(panoMat, segIdOffset);

			string panoImg_new = panoLabelPath_Merged + filetool::GetFileNameNoSuffix(photos[i]) + ".png";
			cv::imwrite(panoImg_new, panoMat_new, compression_params);

			std::cout << "合并第 " << i << " 张照片完成：" << photos[i] << " : " << std::round(time(NULL) - t2) << " Seconds" << endl << endl;
		}
	}
	else if (photoType == "OpenGL")
	{
		//4、读取照片相机信息
		vector<SGlCameraParam> cameInfos = phototool::RSPOpenGLCParamInfosJson(photoInfoFile);
		int segId = 1;
		for (int i = 0; i < numImages; i++)
		{
			long t2 = time(NULL);

			int segIdOffset = segId - 1;
			SGlCameraParam cameraParam;
			std::cout << "计算第" << i << "张图片" << endl;
			//根据照片名找到photoInfo
			bool ifFindPhotoInfo = false;
			for (int pg = 0; pg < cameInfos.size(); pg++)
			{
				if (photos[i] == cameInfos[pg].picName)
				{
					cameraParam = cameInfos[pg];
					break;
				}
			}

			panopticlabel::Image image;
			image.coco_url = "";
			image.date_captured = "2020";
			image.file_name = cameraParam.picName;
			image.flickr_url = "";
			image.height = cameraParam.imageHeight;
			image.id = i;
			image.license = 0;
			image.width = cameraParam.imageWidth;
			panoLabel.images.push_back(image);

			panopticlabel::License license;
			license.id = 0;
			license.name = "license_0";
			license.url = "";
			panoLabel.licenses.push_back(license);

			string annotationFile = panoLabelFilePath + "/" + filetool::GetFileNameNoSuffix(photos[i]) + ".json";
			panopticlabel::Annotation annotation = panopticlabel::ReadAnnotation(annotationFile);
			annotation.file_name = photos[i];
			annotation.image_id = i;
			for (int si = 0; si < annotation.segments_info.size(); si++)
			{
				annotation.segments_info[si].id = segId;
				segId++;
			}
			panoLabel.annotations.push_back(annotation);

			//更新panoptic标注图片每个像素的segId后重新保存
			string panoImg = panoLabelFilePath + "/" + filetool::GetFileNameNoSuffix(photos[i]) + ".png";
			cv::Mat panoMat = cv::imread(panoImg, 1);
			cv::Mat panoMat_new = ResetMatSegId(panoMat, segIdOffset);

			string panoImg_new = panoLabelPath_Merged + filetool::GetFileNameNoSuffix(photos[i]) + ".png";
			cv::imwrite(panoImg_new, panoMat_new, compression_params);

			std::cout << "合并第 " << i << " 张照片完成：" << photos[i] << " : " << std::round(time(NULL) - t2) << " Seconds" << endl << endl;
		}
	}

	//将panoLabel保存为json文件
	string panoLabelFile = "data/project5/panoptic_label/panoptic_merged_cityscape.json";
	panoLabel.WriteToFile(panoLabelFile);
	std::cout << "合并panoptic标注文件完成" << std::endl;
}

string getTexturePath(const string& modelPath, const string& textureName)
{
	string str1(modelPath);
	string str2(textureName);
	//string str3;
	vector<string> strs;
	filetool::SplitString(str1, strs, "\\");
	string sub_str = "";
	if (strs.size()>0)
	{
		sub_str = strs[strs.size() - 1];
	}
	string::size_type pos = 0;
	while ((pos = str1.find(sub_str)) != string::npos)   //替换所有指定子串
	{
		str1.replace(pos, sub_str.length(), str2);
	}
	return str1;
}

//读取面片纹理
void CMyTest::TestReadFaceTexture(const int faceIndex, const GLMmodel *model, const MeshGraph* meshGraph)
{
	int mapdiffuse = model->materials[model->triangles[faceIndex].material].map_diffuse;
	string textureName = getTexturePath(string(model->pathname), string(model->textures[mapdiffuse].name));

	cv::Mat textureMap = cv::imread(textureName);
	int imgWidth = textureMap.cols;
	int imgHeight = textureMap.rows;

	Eigen::Vector3i texcoordIndexs = meshGraph->triangles[faceIndex].tIndex;	//顶点纹理坐标索引
	cv::Point2d texcoords[3];	//顶点纹理坐标
	for (size_t i = 0; i < 3; i++)
	{
		texcoords[i] = cv::Point2d(meshGraph->texcoords[texcoordIndexs[i]][0],
			meshGraph->texcoords[texcoordIndexs[i]][1]);

		//转换为图像像素坐标
		texcoords[i].x = texcoords[i].x * imgWidth;
		texcoords[i].y = (1 - texcoords[i].y) * imgWidth;
		texcoords[i] = cv::Point2d(round(texcoords[i].x), round(texcoords[i].y));
	}
	
	//在图像上绘制纹理坐标区域
	cv::line(textureMap, texcoords[0], texcoords[1], cv::Scalar(256, 0, 0), 1);
	cv::line(textureMap, texcoords[0], texcoords[2], cv::Scalar(256, 0, 0), 1);
	cv::line(textureMap, texcoords[1], texcoords[2], cv::Scalar(256, 0, 0), 1);

	//裁剪纹理区域便于观察
	int minx = 99999, miny = 99999, maxx = -1, maxy = -1;
	for (size_t i = 0; i < 3; i++)
	{
		minx = minx < texcoords[i].x ? minx : texcoords[i].x;
		miny = miny < texcoords[i].y ? miny : texcoords[i].y;
		maxx = maxx > texcoords[i].x ? maxx : texcoords[i].x;
		maxy = maxy > texcoords[i].y ? maxy : texcoords[i].y;
	}
	minx = (minx - 10) < 0 ? 0 : (minx - 10);
	miny = (miny - 10) < 0 ? 0 : (miny - 10);
	maxx = (maxx + 10) > (imgWidth - 1) ? (imgWidth - 1) : (maxx + 10);
	maxy = (maxy + 10) > (imgHeight - 1) ? (imgHeight - 1) : (maxy + 10);

	Rect area(minx, miny, maxx- minx, maxy- miny);
	cv::Mat textureMap_cut = textureMap(area);

	cv::namedWindow("textureMap_cut", 0);//创建窗口
	cvResizeWindow("textureMap_cut", 40, 40); //创建一个固定值大小的窗口
	cv::imshow("textureMap_cut", textureMap_cut);
	cv::waitKey();		//此函数等待按键，按键盘任意键就返回
}