#pragma once
#include <vector>
#include <string>
#include <Eigen/Dense>
#include <GL/freeglut.h>

using namespace std;
using namespace Eigen;

//CityGLM�ļ���ȡ������������еļ�����Ϣ�еĶ������Ϣ
struct SPolygon
{
	int polygonId;
	int verticesCount;	//������Ŀ
	vector<Eigen::Vector3d> vertices;	//�����б�
	Eigen::Vector3f faceNormal;	//��Ƭ������
};

//CityGLM�ļ���ȡ������������еļ�����Ϣ
struct SGeometry
{
	int geometryId;
	int polygonsCount;	//�������Ŀ
	vector<SPolygon> polygons;	//������б�
};

//CityGLM�ļ���ȡ�����������
struct SCityObject
{
	int objectId;
	string objectType;	//�������ͣ�Building�� RoofSurface�� WallSurface�� GroundSurface�� BuildingPart
	int geometriesCount;	//������״��Ŀ
	vector<SGeometry> geometries;	//������״
	int childCityObjectsCount;
	vector<SCityObject> childCityObjects;	//�Ӷ���
};

//ģ��λ����Ϣ
struct SModelPosInfo
{
	int id;
	string modelPath;
	string modelName;
	Eigen::Vector3d position;
	Eigen::Vector2d minMaxX;
	Eigen::Vector2d minMaxY;
	Eigen::Vector2d minMaxZ;

	int columnIndex;	//����ţ����������к�0-7, xCell
	int rowIndex;	//����ţ����������к�0-7, yCell
};

////OpenGL�������
//struct SGlCameraParam
//{
//	int photoId;
//	string photoName;
//	double modelMat[16];	//ģ����ͼ����
//	double projMat[16];	//ͶӰ����
//	int viewMat[4];	//��ͼ��С
//	int imageWidth;	
//	int imageHeight;
//};

//OpenGL��Ⱦ����
struct SGLRenderParm 
{
	Vector4f lightPos0;	//0�Ź�Դλ��
	Vector4f lightPos1;	//1�Ź�Դλ��
	bool useLight0;	//�Ƿ����ù�Դ0
	bool useLight1;	//�Ƿ����ù�Դ1

	//��������
	float ambiLightStrength;	//������ǿ��
	float diffLightStrength;	//ɢ���ǿ��
	float specLightStrength;	//�����ǿ��
	//��������
	float ambiMetriStrength;	//�����ⷴ��ǿ��
	float diffMetriStrength;	//ɢ��ⷴ��ǿ��
	float specMetriStrength;	//����ⷴ��ǿ��
};

//CityGML�ļ��еĽ���ʵ��
struct SCityGMLBuilding
{
	int id;
	vector<SPolygon> roofs;	//�ݶ�
	vector<SPolygon> walls;	//ǽ��
	vector<SPolygon> grounds;	//����

	Vector3d roofPosition;	//������λ�ã����ݶ����ĵ����
	vector<Vector3d> roofBBox; //�������ݶ���Χ��
};

//ʵ���ָ���Ŀ��Ϣ
struct SProjectInfo
{	
	string projectName;	//��Ŀ����
	string projectRoot;	//��Ŀ·��
	string modelFile;	//ģ���ļ�
	string projectType;	//��Ŀ���ͣ�������Ŀ-"Aerial"; OpenGL��Ⱦ��Ŀ-"OpenGL"
	bool hasTextureMaterial;	//�Ƿ��������
	int minMaskScore;	//Mask�÷���Сֵ
};

//OpenGL�������
struct SGlCameraParam
{
	int id;
	string picName;
	double modelMatrix[16];	//ģ����ͼ����
	double projMatrix[16];	//ͶӰ����
	int viewport[4];	//��ͼ��С
	int imageWidth;
	int imageHeight;
};
