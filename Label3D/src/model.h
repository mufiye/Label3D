#pragma once
#include <Eigen\Dense>
#include <vector>
#include <iostream>
#include <opencv2/core/core.hpp>  
#include <opencv2/highgui/highgui.hpp>  
#include <Eigen\Dense>
//#include "global.h"
#include "assistTools\modeltool.h"
#include <math.h>

extern "C" {
#include "openglTools\glm.h"
};

using namespace std;
using namespace cv;
using namespace Eigen;

struct SVertex
{
	Vector3d pos;	//顶点坐标
	Vector3d normal;	//顶点法向量
	Vector3d texcoord;	//纹理坐标 {texcoord[0], texcoord[1]}表示纹理坐标{u, v}， texcoord[2]记录第几张纹理
	vector<int> trianglesInclude;	//包含此顶点的三角形列表索引
};

struct STriangle
{
	Vector3i vIndex;	//三个顶点索引
	Vector3d normal;	//三角形法向量
	Vector3d midPos;	//三角形中心点坐标
	Vector3i tIndex;	//三个顶点的纹理坐标索引
	bool ifVertical45;	//是否竖直
	bool ifVertical60;	//是否竖直
	bool ifHorizontal;	//是否水平
	double area;		//面积
	int textureId;	//三角形做对应的纹理Id
};

//struct SImgMask
//{
//	int imgaeId;
//	int imgMaskId;
//	float score;
//};
//
//struct SFace
//{
//	//最终分类后的maskId
//	int global_maskId;
//
//	//平均得分
//	float score;
//
//	//此面片属于的照片以及其对应的maskId列表
//	//vector<MImgMask> belongIngMasks;
//
//	//此面片属于的照片maskId对应的全局MaskId列表
//	vector<int> belGlobalMaskIds;
//};

//距离三角面片最近的线段信息
struct SNearLine
{
	double distance;	//面片到线段的距离
	int lineIndex;	//线段索引
	double normalAngle; //面片法向量与线段向量夹角
};

/*
	BoxOutEdge:将包围框长宽的各扩大固定长度
	BoxOutDiagonal:将包围框按对角线扩大固定长度
*/
enum BoxDir { BoxUp, BoxDown, BoxLeft, BoxRight, BoxFront, BoxBack, BoxOutEdge, BoxOutDiagonal};

class CBoundingBox
{
public:
	CBoundingBox();

public:
	inline void setBox(const vector<Vector3d> boundingBox) { this->box = boundingBox; };
	bool InitBox(const vector<Vector3d>& vertices);	//初始化包围框
	bool InitBox(const vector<SVertex>& vertices);	//初始化包围框
	void CalcDirs();	//计算包围框方向
	void ExpandBox(BoxDir dir, float length);	//扩大包围框
	void RotateBoxAngle(double angle);	//按角度旋转包围框
	void RotateBoxRadian(double radian);	//按弧度旋转包围框
	void CalcPos();	//计算包围框位置
	vector<Vector2d> GetBoxXOY();	//获取二维平面XOY上的包围框

public:
	int buildIndex;	//建筑实例索引
	vector<Vector3d> box;	//包围框的顶点列表,包围框的8个三维坐标点，上面4个，下面4个
	Vector3d dirUD;	//上下方向
	Vector3d dirLR;	//左右方向
	Vector3d dirFB;	//前后方向
	vector<Vector3d> midMinMaxPoint; //中心点、xyz最小值, xyz最大值
};

class CModel
{
public:
	CModel();
	CModel(GLMmodel* glmModel);

public:
	//添加顶点
	int AddVertex(SVertex &ver)
	{
		int index = vertices.size();
		vertices.push_back(ver);
		return index;
	};

	//添加三角面片
	int AddFace(STriangle& tri)
	{
		int index = triangles.size();
		triangles.push_back(tri);
		return index;
	};

	//获取顶点节点总数
	int GetVertexNodeCount()
	{
		return (int)vertices.size();
	};

	//获取面片节点总数
	int GetFaceNodeCount()
	{
		return (int)triangles.size();
	};

	//计算包围框
	bool CalcBoundingBox();

	//获取模型边界n圈面片的索引
	vector<int> GetBoundaryFaceIndices() const;

	//获取模型分割区域的边界n圈面片的索引
	vector<int> GetBoundaryFaceIndices_SegRegion(const vector<int> &segment) const;

public:
	int numVertices;	//顶点数
	int numTriangles;	//三角面片数
	int numtexcoords;	//纹理坐标数
	int numTextures;	//纹理数目
	vector<SVertex> vertices;	//顶点列表
	vector<STriangle> triangles;	//三角面片列表
	vector<Vector2d> texcoords;	//纹理坐标
	vector<Vector3d> midMinMaxPoint;	//中心点、xyz最小值, xyz最大值
	CBoundingBox boundingBox;	//xoy平面方向包围框，并按minZ,maxZ将平面包围框拉高到三维
};

class MeshGraph:public CModel
{
public:
	MeshGraph();

	/*根据GLMmodel构建MeshGraph
	glmmodel: GLMmodel模型
	ifCalcFaceInfo：是否计算面片信息（包括面片中心点，水平竖直性，面积等）
	ifCalcAdjGraph：是否创建模型三角面片邻接图
	*/
	void CreateGraph(const GLMmodel* glmmodel, bool ifCalcFaceInfo = false, bool ifCalcAdjGraph = false);
	//计算三角面片中心点坐标
	void CalcFaceCenter();
	//计算三角面片竖直水平性质
	void CalcFaceVerHor();
	//计算三角面片面积
	void CalcFaceArea();
	//构建顶点图,即计算每个顶点的邻接顶点
	void CalcAdjVerticesPerVertex();
	//构建面片图,即计算每个面片的邻接面片
	void CalcAdjFacesPerFace();
	//构建面片图,即计算每个面片的邻接面片（以相同边为相邻条件）
	void CalcAdjFacesPerFace_Edge();

	//构建图的边，一条边E[a, b]表示索引为a, b的两个面片具有公共边
	void CalcGraphEdges_e();

	//获取索引index顶点的邻接节点列表
	vector<int>& GetVertexNeighbourList(int index)
	{
		return adjVerticsPerVertex[index];
	};
	//获取索引index面片的邻接节点列表
	vector<int>& GetFaceNeighbourList(int index)
	{
		return adjFacesPerFace[index];
	};
	//
	//获取索引index面片的邻接面片列表（相同边）
	vector<int>& GetFaceNeighbourByEdgeList(int index)
	{
		return adjFacesPerFace_Edge[index];
	};
	//计算每个三角面片到建筑范围多边形的距离
	void CalcDist2BuildRegion(const vector<Vector3d> &region);
	//获取每个三角面片到建筑范围多边形的距离
	vector<SNearLine> GetDist2BuildRegion(const vector<Vector3d> &region) const;
	//根据分割结果计算“需要参与图割的模型面片”到屋顶多边形的最短距离，roofSegResult中元素值为1表示此面片为屋顶
	vector<SNearLine> GetDist2BuildRegion(const vector<Vector3d> &region, const vector<int>& roofSegResult) const;

	//计算三角形到屋顶中心点连线与屋顶范围的相交线段
	void CalcIntersLine2RoofCenter(const vector<Vector3d> &region);

	//根据模型屋顶分割结果获取建筑的竖直柱体多边形范围region
	vector<Vector3d> GetBuildRegionByRoof(const vector<int> &roofSegResult) const;

	//根据“面片索引数组”获取包围框(OpenCV算法取矩形包围框)
	vector<Vector3d> GetBBoxByFIndices(const vector<int> &faceIndices) const;
	//根据“面片索引数组”获取包围框(OpenCV算法取矩形包围框)
	CBoundingBox GetCBBoxByFIndices(const vector<int> &faceIndices) const;
	//根据矩形包围框获取包围框范围内所有面片索引数组
	vector<int> GetFIndicesInBBox(const CBoundingBox bBox, const vector<int> &segResult, int buildIndex) const;
	//根据矩形包围框获取包围框范围内所有面片索引数组(可以取到其他建筑)
	vector<int> GetFIndicesInBBox(const CBoundingBox bBox) const;

public:
	//每个顶点的相邻顶点数组
	vector<vector<int>> adjVerticsPerVertex;
	//每个面片的相邻面片数组（以相同顶点为相邻条件）
	vector<vector<int>> adjFacesPerFace;
	//每个面片的相邻面片数组（以相同边为相邻条件）
	vector<vector<int>> adjFacesPerFace_Edge;
	//图的边数组(两个具有公共边的面片之间为一条图的边)，边[a, b]表示索引为a, b的两个面片具有公共边
	vector<Vector2i> graphEdgesSameEdge;

	//建筑物多边形范围
	vector<Vector3d> buildRegion;
	//距离三角面片最近的线段信息
	vector<SNearLine> nearestLine;

	////每个三角面片到建筑范围多边形的距离，在多边形内距离为负，多边形外距离为正
	//vector<double> dist2BuildRegion;
	////每个三角面片距离它最近的buildRegion的线段索引
	//vector<int> nearLineIndex;
};

