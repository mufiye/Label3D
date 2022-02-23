#pragma once
#include "openglTools\glsl.h"
#include <string>
#include <vector>
#include <iostream>
#include "openglTools\glcamera.h"
#include "openglTools\glm.h"
#include "publicstruct.h"
#include <time.h>
#include "assistTools\filetool.h"
#include "model.h"
#include "annotation.h"

using namespace std;

#define PI 3.14159265358979323846 
inline double Random() { return rand() % 100 / (double)101; };

//enum DrawType { DrawNothing, DrawSigleModel, DrawBlockModel, DrawInstance };
enum DrawType { DrawColor, DrawTexture, DrawTextureColor };
enum RenderType { RenderPloygon, RenderTriangle, RenderLine };
enum EditState { Edit_Render, Edit_Edit, Edit_AddMesh, Edit_DeleteMesh, Edit_AddNewInsRec, Edit_AddMesh_Polygon, Edit_DeleteMesh_Polygon };
enum ViewDir { View_Up, View_Down, View_Left, View_Right, View_Front, View_Back };
enum PickType { Pick_Point, Pick_Circle, Pick_Rect };
enum PivotPointType { PP_ModelCenter, PP_InsCenter, PP_ScreenCenter };

//模型编辑器
class ModelEditor
{
public:
	ModelEditor();
	ModelEditor(const string& fileName);
	~ModelEditor();

public:
	void Init();	//初始化
	void InitGLSL();
	void LoadCategories();	//加载标注类别
	void Release();	//释放所有Global资源
	void InitializeOpenGL();	//初始化OpenGL环境
	void RenderScene();	 //渲染场景
	void LoadModel(const string &modelFile);	//加载模型
	bool LoadAnnocation(const string& fileName);	//加载标注文件

	inline void SetStateRender() { this->editState = Edit_Render; };
	inline void SetStateEdit() { this->editState = Edit_Edit; };
	inline void SetStateAddMesh() { this->editState = Edit_AddMesh; };
	inline void SetStateDeleteMesh() { this->editState = Edit_DeleteMesh; };
	inline void SetStateAddNewInsRec() { this->editState = Edit_AddNewInsRec; };
	inline void SetStateDeleteMesh_Batch() { this->editState = Edit_DeleteMesh_Polygon; };
	inline void SetStateAddNewInsRec_Batch() { this->editState = Edit_AddMesh_Polygon; };
	inline void SetPickType(PickType p) { this->pickType = p; };

	//绘制模型
	void Draw();
	//绘制线框
	void DrawLine(GLMmodel* model);
	//按实例绘制
	void DrawInstance(const GLMmodel* model, GLuint mode, const vector<vector<int>> &fIndicsPerInstance, const vector<Vector3f> &colors);
	//绘制单个实例包围框
	void DrawSingleBBox(const CBoundingBox &bBox, const Vector3f &color);
	void DrawSingleBBox(const CBoundingBox &bBox, const Vector3f &color, const Camera* camera);

	void DrawClickPoints();	//绘制点击点
	void SetGLCameraWithModel();	// 根据加载的模型设置相机视角
	void SetGLCameraWithInsBox(int insIndex, ViewDir direction);	//根据选中的实例（包围框）设置相机视角
	void SetCameraWithInsBox(int insIndex, ViewDir direction);	//根据选中的实例（包围框）设置相机视角
	void SetCameraWithInsBox(int cateId, int insId, ViewDir direction);	//根据选中的LabelInfo实例（包围框）设置相机视角
	inline void SetMeshColor(Vector3f color) { meshColor = color; };
	void RefreshGLRenderParm();	//更新OpenGL渲染参数

	void CalcInsBBox(int insIndex);	//计算单个实例包围盒
	void CalcInsBBox();	//计算所有实例包围盒
	int SeletcInstance(Vector3d cameraPos, Vector3d ray_dir);	 //选择单个实例
	int SelectFace_Ray(Vector3d cameraPos, Vector3d ray_dir);	//选择单个面片索引
	int SelectFace_Ray(Vector3d cameraPos, Vector3d ray_dir, Vector3d &jiaodian);	//选择单个面片索引
	int SelectFace_GPU(int x, int y);	//选择单个面片索引
	bool AddNewInsRec(const vector<Vector3d> &rec);	//根据四边形添加新的实例
	bool MergeBackForSingleIns(int insIndex);	//单个实例合并背景面片
	bool ClearNoiseForSingleIns(int insIndex);	//单个实例去除噪声
	bool GetRoofForSingleIns(int insIndex, int roofNum);	//单个建筑提取屋顶
	double CalcInsArea(int insIndex);	//计算实例面积

										//GLSL VAO,VBO绘制
	void DrawWithVbo();
	void UpdateGLSLSize(int m_width, int m_height);

	//更新GLSL数据
private:
	void AddMesh(int faceIndex, int insIndex);
	void DeleteMesh(int faceIndex);
	//void FindPlantMesh(cv::Mat &textureImg, vector<int> &addedMesh, vector<int> &excludedMesh, vector<int> &insFaces,int faceIndex,const int& referedValue);
	cv::Vec3d FindMeanHSVinMesh(vector<string> &indexVector, vector<cv::Mat> & textureImgVector, int faceIndex);
	//判断是否在三角形中
	double sign(double x, double y, Vector2d triVertex1, Vector2d triVertex2);
	bool pointInTriangle(double x, double y, vector<Vector2d> triVertexs);
	string getTexturePath(const string& modelPath, const string& textureName);//得到纹理图片
																			  //更新GLSL数据
public:
	//根据标注信息设置Glsl颜色顶点数据
	void UpGlsl_V_Color(const Annotation* annotation);
	void AddMesh(vector<int> faceIndices, int insIndex);
	void DeleteMesh(vector<int> faceIndices);
	//设置相机LookAt目标点 PivotPoint
	bool SetCameraPivotPoint(PivotPointType ppt);

	//根据labelInfo更新GLSL信息
private:
	////添加单个实例面片
	//bool AddMesh_labelInfo(int faceIndex, int cateId, int insId);
	////删除单个实例面片
	//bool DeleteMesh_labelInfo(int faceIndex);

	//根据labelInfo更新GLSL信息
public:
	//新增实例, cateId: 类别Id;  返回新增的实例Id
	int AddNewInstance(int cateId);
	//新增类别，cateName：类别名称; 返回新增的类别Id
	int AddNewCategory(const string& cateName);
	//删除实例
	bool DeleteInstance(int cateId, int insId);
	//删除类别
	bool DeleteCategory(int cateId);

	//添加实例面片数组
	bool AddMesh_labelInfo(vector<int> faceIndices, int cateId, int insId);
	//删除实例面片数组
	bool DeleteMesh_labelInfo(vector<int> faceIndices);

	//单个实例合并背景面片
	bool MergeBackForSingleIns(int cateId, int insId);
	//单个实例去除噪声
	bool ClearNoiseForSingleIns(int cateId, int insId);
	//单个建筑实例图割
	bool GraphCutForSingleIns(int cateId, int insId);

	//提取植物面片
	bool PickPlantFaces(int cateId, int insId);
	//提取道路面片
	bool PickRoadFaces(int cateId, int insId);
public:
	CGLCamera *glCamera;	//OpenGL相机
	SGLRenderParm glRenderParam;	//OpenGL渲染参数
									//PickerViewer* pickerView;
	bool ifDrawLine;

public:
	GLMmodel *model_origin;	//场景obj模型(原始模型，未归一化)
	GLMmodel *model;	//单个场景obj模型(顶点坐标归一化到0-1之间)
	MeshGraph* meshGraph;
	DrawType drawType;	//绘制方式
	EditState editState;	//编辑状态
	PickType pickType; //选取方式
	PivotPointType pivotPointType;	//相机旋转缩放等运动中心点类型
	CGLSL* myGlsl;

	Vector3f meshColor;
	Annotation* annotation;

	LabelInfo* labelInfo;	//标注信息
	vector<LabelInfo::Category> allCategory;	//只记录所有类别信息，不包括面片信息

	int selInsIndex;	//当前选中的实例索引(-2:未选中，不显示，-1：未选中，显示所有包围框，0：选中背景，不显示，1，2，3....)
	Vector2i selIns;	//selIns[0]:当前选中的实例类别；selIns[1]:实例索引(-2:未选中，不显示，-1：未选中，显示所有包围框，0：选中背景，不显示，1，2，3....)
	bool ifEdited;	//是否进行了编辑

	vector<Vector3d> polygonPoints_3D;	//多边形点_3D
	vector<Vector2i> polygonPoints_2D;	//多边形点_2D

	int m_width;
	int m_height;

	int paintPixel;

	bool changeLabeledFace;	//是否修改已经标注的面片

							//vector<Vector3f> colors;	//全局固定颜色表
							//vector<int> annotation;	//标注信息
							//vector<vector<int>> insFaces;	//每个实例包含的面片索引
};