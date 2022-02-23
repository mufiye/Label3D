#pragma once
#include <string>
#include <Eigen\Dense>
#include <vector>
#include <queue>
#include <iostream>
#include <opencv2/core/core.hpp>  
#include <opencv2/highgui/highgui.hpp>  
#include <opencv2/opencv.hpp>
#include "../assistTools/filetool.h"
#include "glm.h"

using namespace std;

namespace myopengltool
{
	//��OpenGL��Ⱦ���������ݱ���ΪbmpͼƬ
	bool WriteBMP(const char filename[], unsigned char* data, unsigned int w, unsigned int h);

	//��OpenGL��Ⱦ���������ݱ���ΪjpgͼƬ
	bool WriteJPG_CV(const char *filename, unsigned char* pixelData, int w, int h);

	//����ǰ��������ΪͼƬ
	bool Screenshot(const char* filename);

	//���泡����ͼƬ�����������Ӧ���������
	bool SaveAsImg(const string &fileName);

	//���������η�����
	Eigen::Vector3f CalTriNormal(Eigen::Vector3d ver1, Eigen::Vector3d ver2, Eigen::Vector3d ver3);

	//��ʼ��OpenGL����
	void InitializeOpenGL();

	//ˢ��OpenGL��Ⱦ����
	void RefreshGLRenderParm();

	//��OpenGL�����������������ϵ�еĵ�ͶӰ��ͼ������ϵ������ͼ����������ֵ
	Vector3d GluProject(const Vector3d &worldPosition, const SGlCameraParam &cameraParam);

	//��ȡ��ǰOpenGL�������
	SGlCameraParam GetGLCameraParam();
}