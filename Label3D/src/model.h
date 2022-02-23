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
	Vector3d pos;	//��������
	Vector3d normal;	//���㷨����
	Vector3d texcoord;	//�������� {texcoord[0], texcoord[1]}��ʾ��������{u, v}�� texcoord[2]��¼�ڼ�������
	vector<int> trianglesInclude;	//�����˶�����������б�����
};

struct STriangle
{
	Vector3i vIndex;	//������������
	Vector3d normal;	//�����η�����
	Vector3d midPos;	//���������ĵ�����
	Vector3i tIndex;	//���������������������
	bool ifVertical45;	//�Ƿ���ֱ
	bool ifVertical60;	//�Ƿ���ֱ
	bool ifHorizontal;	//�Ƿ�ˮƽ
	double area;		//���
	int textureId;	//����������Ӧ������Id
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
//	//���շ�����maskId
//	int global_maskId;
//
//	//ƽ���÷�
//	float score;
//
//	//����Ƭ���ڵ���Ƭ�Լ����Ӧ��maskId�б�
//	//vector<MImgMask> belongIngMasks;
//
//	//����Ƭ���ڵ���ƬmaskId��Ӧ��ȫ��MaskId�б�
//	vector<int> belGlobalMaskIds;
//};

//����������Ƭ������߶���Ϣ
struct SNearLine
{
	double distance;	//��Ƭ���߶εľ���
	int lineIndex;	//�߶�����
	double normalAngle; //��Ƭ���������߶������н�
};

/*
	BoxOutEdge:����Χ�򳤿�ĸ�����̶�����
	BoxOutDiagonal:����Χ�򰴶Խ�������̶�����
*/
enum BoxDir { BoxUp, BoxDown, BoxLeft, BoxRight, BoxFront, BoxBack, BoxOutEdge, BoxOutDiagonal};

class CBoundingBox
{
public:
	CBoundingBox();

public:
	inline void setBox(const vector<Vector3d> boundingBox) { this->box = boundingBox; };
	bool InitBox(const vector<Vector3d>& vertices);	//��ʼ����Χ��
	bool InitBox(const vector<SVertex>& vertices);	//��ʼ����Χ��
	void CalcDirs();	//�����Χ����
	void ExpandBox(BoxDir dir, float length);	//�����Χ��
	void RotateBoxAngle(double angle);	//���Ƕ���ת��Χ��
	void RotateBoxRadian(double radian);	//��������ת��Χ��
	void CalcPos();	//�����Χ��λ��
	vector<Vector2d> GetBoxXOY();	//��ȡ��άƽ��XOY�ϵİ�Χ��

public:
	int buildIndex;	//����ʵ������
	vector<Vector3d> box;	//��Χ��Ķ����б�,��Χ���8����ά����㣬����4��������4��
	Vector3d dirUD;	//���·���
	Vector3d dirLR;	//���ҷ���
	Vector3d dirFB;	//ǰ����
	vector<Vector3d> midMinMaxPoint; //���ĵ㡢xyz��Сֵ, xyz���ֵ
};

class CModel
{
public:
	CModel();
	CModel(GLMmodel* glmModel);

public:
	//��Ӷ���
	int AddVertex(SVertex &ver)
	{
		int index = vertices.size();
		vertices.push_back(ver);
		return index;
	};

	//���������Ƭ
	int AddFace(STriangle& tri)
	{
		int index = triangles.size();
		triangles.push_back(tri);
		return index;
	};

	//��ȡ����ڵ�����
	int GetVertexNodeCount()
	{
		return (int)vertices.size();
	};

	//��ȡ��Ƭ�ڵ�����
	int GetFaceNodeCount()
	{
		return (int)triangles.size();
	};

	//�����Χ��
	bool CalcBoundingBox();

	//��ȡģ�ͱ߽�nȦ��Ƭ������
	vector<int> GetBoundaryFaceIndices() const;

	//��ȡģ�ͷָ�����ı߽�nȦ��Ƭ������
	vector<int> GetBoundaryFaceIndices_SegRegion(const vector<int> &segment) const;

public:
	int numVertices;	//������
	int numTriangles;	//������Ƭ��
	int numtexcoords;	//����������
	int numTextures;	//������Ŀ
	vector<SVertex> vertices;	//�����б�
	vector<STriangle> triangles;	//������Ƭ�б�
	vector<Vector2d> texcoords;	//��������
	vector<Vector3d> midMinMaxPoint;	//���ĵ㡢xyz��Сֵ, xyz���ֵ
	CBoundingBox boundingBox;	//xoyƽ�淽���Χ�򣬲���minZ,maxZ��ƽ���Χ�����ߵ���ά
};

class MeshGraph:public CModel
{
public:
	MeshGraph();

	/*����GLMmodel����MeshGraph
	glmmodel: GLMmodelģ��
	ifCalcFaceInfo���Ƿ������Ƭ��Ϣ��������Ƭ���ĵ㣬ˮƽ��ֱ�ԣ�����ȣ�
	ifCalcAdjGraph���Ƿ񴴽�ģ��������Ƭ�ڽ�ͼ
	*/
	void CreateGraph(const GLMmodel* glmmodel, bool ifCalcFaceInfo = false, bool ifCalcAdjGraph = false);
	//����������Ƭ���ĵ�����
	void CalcFaceCenter();
	//����������Ƭ��ֱˮƽ����
	void CalcFaceVerHor();
	//����������Ƭ���
	void CalcFaceArea();
	//��������ͼ,������ÿ��������ڽӶ���
	void CalcAdjVerticesPerVertex();
	//������Ƭͼ,������ÿ����Ƭ���ڽ���Ƭ
	void CalcAdjFacesPerFace();
	//������Ƭͼ,������ÿ����Ƭ���ڽ���Ƭ������ͬ��Ϊ����������
	void CalcAdjFacesPerFace_Edge();

	//����ͼ�ıߣ�һ����E[a, b]��ʾ����Ϊa, b��������Ƭ���й�����
	void CalcGraphEdges_e();

	//��ȡ����index������ڽӽڵ��б�
	vector<int>& GetVertexNeighbourList(int index)
	{
		return adjVerticsPerVertex[index];
	};
	//��ȡ����index��Ƭ���ڽӽڵ��б�
	vector<int>& GetFaceNeighbourList(int index)
	{
		return adjFacesPerFace[index];
	};
	//
	//��ȡ����index��Ƭ���ڽ���Ƭ�б���ͬ�ߣ�
	vector<int>& GetFaceNeighbourByEdgeList(int index)
	{
		return adjFacesPerFace_Edge[index];
	};
	//����ÿ��������Ƭ��������Χ����εľ���
	void CalcDist2BuildRegion(const vector<Vector3d> &region);
	//��ȡÿ��������Ƭ��������Χ����εľ���
	vector<SNearLine> GetDist2BuildRegion(const vector<Vector3d> &region) const;
	//���ݷָ������㡰��Ҫ����ͼ���ģ����Ƭ�����ݶ�����ε���̾��룬roofSegResult��Ԫ��ֵΪ1��ʾ����ƬΪ�ݶ�
	vector<SNearLine> GetDist2BuildRegion(const vector<Vector3d> &region, const vector<int>& roofSegResult) const;

	//���������ε��ݶ����ĵ��������ݶ���Χ���ཻ�߶�
	void CalcIntersLine2RoofCenter(const vector<Vector3d> &region);

	//����ģ���ݶ��ָ�����ȡ��������ֱ�������η�Χregion
	vector<Vector3d> GetBuildRegionByRoof(const vector<int> &roofSegResult) const;

	//���ݡ���Ƭ�������顱��ȡ��Χ��(OpenCV�㷨ȡ���ΰ�Χ��)
	vector<Vector3d> GetBBoxByFIndices(const vector<int> &faceIndices) const;
	//���ݡ���Ƭ�������顱��ȡ��Χ��(OpenCV�㷨ȡ���ΰ�Χ��)
	CBoundingBox GetCBBoxByFIndices(const vector<int> &faceIndices) const;
	//���ݾ��ΰ�Χ���ȡ��Χ��Χ��������Ƭ��������
	vector<int> GetFIndicesInBBox(const CBoundingBox bBox, const vector<int> &segResult, int buildIndex) const;
	//���ݾ��ΰ�Χ���ȡ��Χ��Χ��������Ƭ��������(����ȡ����������)
	vector<int> GetFIndicesInBBox(const CBoundingBox bBox) const;

public:
	//ÿ����������ڶ�������
	vector<vector<int>> adjVerticsPerVertex;
	//ÿ����Ƭ��������Ƭ���飨����ͬ����Ϊ����������
	vector<vector<int>> adjFacesPerFace;
	//ÿ����Ƭ��������Ƭ���飨����ͬ��Ϊ����������
	vector<vector<int>> adjFacesPerFace_Edge;
	//ͼ�ı�����(�������й����ߵ���Ƭ֮��Ϊһ��ͼ�ı�)����[a, b]��ʾ����Ϊa, b��������Ƭ���й�����
	vector<Vector2i> graphEdgesSameEdge;

	//���������η�Χ
	vector<Vector3d> buildRegion;
	//����������Ƭ������߶���Ϣ
	vector<SNearLine> nearestLine;

	////ÿ��������Ƭ��������Χ����εľ��룬�ڶ�����ھ���Ϊ��������������Ϊ��
	//vector<double> dist2BuildRegion;
	////ÿ��������Ƭ�����������buildRegion���߶�����
	//vector<int> nearLineIndex;
};

