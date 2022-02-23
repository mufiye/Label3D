#pragma once
#include <vector>
#include <string>
#include <Eigen/Dense>
#include <GL/freeglut.h>

using namespace std;
using namespace Eigen;

//CityGLM文件读取城市物体对象中的几何信息中的多边形信息
struct SPolygon
{
	int polygonId;
	int verticesCount;	//顶点数目
	vector<Eigen::Vector3d> vertices;	//顶点列表
	Eigen::Vector3f faceNormal;	//面片法向量
};

//CityGLM文件读取城市物体对象中的几何信息
struct SGeometry
{
	int geometryId;
	int polygonsCount;	//多边形数目
	vector<SPolygon> polygons;	//多边形列表
};

//CityGLM文件读取城市物体对象
struct SCityObject
{
	int objectId;
	string objectType;	//对象类型：Building、 RoofSurface、 WallSurface、 GroundSurface、 BuildingPart
	int geometriesCount;	//几何形状数目
	vector<SGeometry> geometries;	//几何形状
	int childCityObjectsCount;
	vector<SCityObject> childCityObjects;	//子对象
};

//模型位置信息
struct SModelPosInfo
{
	int id;
	string modelPath;
	string modelName;
	Eigen::Vector3d position;
	Eigen::Vector2d minMaxX;
	Eigen::Vector2d minMaxY;
	Eigen::Vector2d minMaxZ;

	int columnIndex;	//列序号，从左往右列号0-7, xCell
	int rowIndex;	//行序号，从下往上行号0-7, yCell
};

////OpenGL相机参数
//struct SGlCameraParam
//{
//	int photoId;
//	string photoName;
//	double modelMat[16];	//模型视图矩阵
//	double projMat[16];	//投影矩阵
//	int viewMat[4];	//视图大小
//	int imageWidth;	
//	int imageHeight;
//};

//OpenGL渲染参数
struct SGLRenderParm 
{
	Vector4f lightPos0;	//0号光源位置
	Vector4f lightPos1;	//1号光源位置
	bool useLight0;	//是否启用光源0
	bool useLight1;	//是否启用光源1

	//光照属性
	float ambiLightStrength;	//环境光强度
	float diffLightStrength;	//散射光强度
	float specLightStrength;	//镜面光强度
	//材料属性
	float ambiMetriStrength;	//环境光反射强度
	float diffMetriStrength;	//散射光反射强度
	float specMetriStrength;	//镜面光反射强度
};

//CityGML文件中的建筑实例
struct SCityGMLBuilding
{
	int id;
	vector<SPolygon> roofs;	//屋顶
	vector<SPolygon> walls;	//墙面
	vector<SPolygon> grounds;	//地面

	Vector3d roofPosition;	//建筑物位置，按屋顶中心点计算
	vector<Vector3d> roofBBox; //建筑物屋顶包围框
};

//实例分割项目信息
struct SProjectInfo
{	
	string projectName;	//项目名称
	string projectRoot;	//项目路径
	string modelFile;	//模型文件
	string projectType;	//项目类型：航拍项目-"Aerial"; OpenGL渲染项目-"OpenGL"
	bool hasTextureMaterial;	//是否具有纹理
	int minMaskScore;	//Mask得分最小值
};

//OpenGL相机参数
struct SGlCameraParam
{
	int id;
	string picName;
	double modelMatrix[16];	//模型视图矩阵
	double projMatrix[16];	//投影矩阵
	int viewport[4];	//视图大小
	int imageWidth;
	int imageHeight;
};
