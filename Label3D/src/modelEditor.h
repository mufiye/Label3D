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

//ģ�ͱ༭��
class ModelEditor
{
public:
	ModelEditor();
	ModelEditor(const string& fileName);
	~ModelEditor();

public:
	void Init();	//��ʼ��
	void InitGLSL();
	void LoadCategories();	//���ر�ע���
	void Release();	//�ͷ�����Global��Դ
	void InitializeOpenGL();	//��ʼ��OpenGL����
	void RenderScene();	 //��Ⱦ����
	void LoadModel(const string &modelFile);	//����ģ��
	bool LoadAnnocation(const string& fileName);	//���ر�ע�ļ�

	inline void SetStateRender() { this->editState = Edit_Render; };
	inline void SetStateEdit() { this->editState = Edit_Edit; };
	inline void SetStateAddMesh() { this->editState = Edit_AddMesh; };
	inline void SetStateDeleteMesh() { this->editState = Edit_DeleteMesh; };
	inline void SetStateAddNewInsRec() { this->editState = Edit_AddNewInsRec; };
	inline void SetStateDeleteMesh_Batch() { this->editState = Edit_DeleteMesh_Polygon; };
	inline void SetStateAddNewInsRec_Batch() { this->editState = Edit_AddMesh_Polygon; };
	inline void SetPickType(PickType p) { this->pickType = p; };

	//����ģ��
	void Draw();
	//�����߿�
	void DrawLine(GLMmodel* model);
	//��ʵ������
	void DrawInstance(const GLMmodel* model, GLuint mode, const vector<vector<int>> &fIndicsPerInstance, const vector<Vector3f> &colors);
	//���Ƶ���ʵ����Χ��
	void DrawSingleBBox(const CBoundingBox &bBox, const Vector3f &color);
	void DrawSingleBBox(const CBoundingBox &bBox, const Vector3f &color, const Camera* camera);

	void DrawClickPoints();	//���Ƶ����
	void SetGLCameraWithModel();	// ���ݼ��ص�ģ����������ӽ�
	void SetGLCameraWithInsBox(int insIndex, ViewDir direction);	//����ѡ�е�ʵ������Χ����������ӽ�
	void SetCameraWithInsBox(int insIndex, ViewDir direction);	//����ѡ�е�ʵ������Χ����������ӽ�
	void SetCameraWithInsBox(int cateId, int insId, ViewDir direction);	//����ѡ�е�LabelInfoʵ������Χ����������ӽ�
	inline void SetMeshColor(Vector3f color) { meshColor = color; };
	void RefreshGLRenderParm();	//����OpenGL��Ⱦ����

	void CalcInsBBox(int insIndex);	//���㵥��ʵ����Χ��
	void CalcInsBBox();	//��������ʵ����Χ��
	int SeletcInstance(Vector3d cameraPos, Vector3d ray_dir);	 //ѡ�񵥸�ʵ��
	int SelectFace_Ray(Vector3d cameraPos, Vector3d ray_dir);	//ѡ�񵥸���Ƭ����
	int SelectFace_Ray(Vector3d cameraPos, Vector3d ray_dir, Vector3d &jiaodian);	//ѡ�񵥸���Ƭ����
	int SelectFace_GPU(int x, int y);	//ѡ�񵥸���Ƭ����
	bool AddNewInsRec(const vector<Vector3d> &rec);	//�����ı�������µ�ʵ��
	bool MergeBackForSingleIns(int insIndex);	//����ʵ���ϲ�������Ƭ
	bool ClearNoiseForSingleIns(int insIndex);	//����ʵ��ȥ������
	bool GetRoofForSingleIns(int insIndex, int roofNum);	//����������ȡ�ݶ�
	double CalcInsArea(int insIndex);	//����ʵ�����

										//GLSL VAO,VBO����
	void DrawWithVbo();
	void UpdateGLSLSize(int m_width, int m_height);

	//����GLSL����
private:
	void AddMesh(int faceIndex, int insIndex);
	void DeleteMesh(int faceIndex);
	//void FindPlantMesh(cv::Mat &textureImg, vector<int> &addedMesh, vector<int> &excludedMesh, vector<int> &insFaces,int faceIndex,const int& referedValue);
	cv::Vec3d FindMeanHSVinMesh(vector<string> &indexVector, vector<cv::Mat> & textureImgVector, int faceIndex);
	//�ж��Ƿ�����������
	double sign(double x, double y, Vector2d triVertex1, Vector2d triVertex2);
	bool pointInTriangle(double x, double y, vector<Vector2d> triVertexs);
	string getTexturePath(const string& modelPath, const string& textureName);//�õ�����ͼƬ
																			  //����GLSL����
public:
	//���ݱ�ע��Ϣ����Glsl��ɫ��������
	void UpGlsl_V_Color(const Annotation* annotation);
	void AddMesh(vector<int> faceIndices, int insIndex);
	void DeleteMesh(vector<int> faceIndices);
	//�������LookAtĿ��� PivotPoint
	bool SetCameraPivotPoint(PivotPointType ppt);

	//����labelInfo����GLSL��Ϣ
private:
	////��ӵ���ʵ����Ƭ
	//bool AddMesh_labelInfo(int faceIndex, int cateId, int insId);
	////ɾ������ʵ����Ƭ
	//bool DeleteMesh_labelInfo(int faceIndex);

	//����labelInfo����GLSL��Ϣ
public:
	//����ʵ��, cateId: ���Id;  ����������ʵ��Id
	int AddNewInstance(int cateId);
	//�������cateName���������; �������������Id
	int AddNewCategory(const string& cateName);
	//ɾ��ʵ��
	bool DeleteInstance(int cateId, int insId);
	//ɾ�����
	bool DeleteCategory(int cateId);

	//���ʵ����Ƭ����
	bool AddMesh_labelInfo(vector<int> faceIndices, int cateId, int insId);
	//ɾ��ʵ����Ƭ����
	bool DeleteMesh_labelInfo(vector<int> faceIndices);

	//����ʵ���ϲ�������Ƭ
	bool MergeBackForSingleIns(int cateId, int insId);
	//����ʵ��ȥ������
	bool ClearNoiseForSingleIns(int cateId, int insId);
	//��������ʵ��ͼ��
	bool GraphCutForSingleIns(int cateId, int insId);

	//��ȡֲ����Ƭ
	bool PickPlantFaces(int cateId, int insId);
	//��ȡ��·��Ƭ
	bool PickRoadFaces(int cateId, int insId);
public:
	CGLCamera *glCamera;	//OpenGL���
	SGLRenderParm glRenderParam;	//OpenGL��Ⱦ����
									//PickerViewer* pickerView;
	bool ifDrawLine;

public:
	GLMmodel *model_origin;	//����objģ��(ԭʼģ�ͣ�δ��һ��)
	GLMmodel *model;	//��������objģ��(���������һ����0-1֮��)
	MeshGraph* meshGraph;
	DrawType drawType;	//���Ʒ�ʽ
	EditState editState;	//�༭״̬
	PickType pickType; //ѡȡ��ʽ
	PivotPointType pivotPointType;	//�����ת���ŵ��˶����ĵ�����
	CGLSL* myGlsl;

	Vector3f meshColor;
	Annotation* annotation;

	LabelInfo* labelInfo;	//��ע��Ϣ
	vector<LabelInfo::Category> allCategory;	//ֻ��¼���������Ϣ����������Ƭ��Ϣ

	int selInsIndex;	//��ǰѡ�е�ʵ������(-2:δѡ�У�����ʾ��-1��δѡ�У���ʾ���а�Χ��0��ѡ�б���������ʾ��1��2��3....)
	Vector2i selIns;	//selIns[0]:��ǰѡ�е�ʵ�����selIns[1]:ʵ������(-2:δѡ�У�����ʾ��-1��δѡ�У���ʾ���а�Χ��0��ѡ�б���������ʾ��1��2��3....)
	bool ifEdited;	//�Ƿ�����˱༭

	vector<Vector3d> polygonPoints_3D;	//����ε�_3D
	vector<Vector2i> polygonPoints_2D;	//����ε�_2D

	int m_width;
	int m_height;

	int paintPixel;

	bool changeLabeledFace;	//�Ƿ��޸��Ѿ���ע����Ƭ

							//vector<Vector3f> colors;	//ȫ�̶ֹ���ɫ��
							//vector<int> annotation;	//��ע��Ϣ
							//vector<vector<int>> insFaces;	//ÿ��ʵ����������Ƭ����
};