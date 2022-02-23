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
	//将OpenGL渲染缓冲区数据保存为bmp图片
	bool WriteBMP(const char filename[], unsigned char* data, unsigned int w, unsigned int h);

	//将OpenGL渲染缓冲区数据保存为jpg图片
	bool WriteJPG_CV(const char *filename, unsigned char* pixelData, int w, int h);

	//将当前场景保存为图片
	bool Screenshot(const char* filename);

	//保存场景到图片、包括保存对应的相机参数
	bool SaveAsImg(const string &fileName);

	//计算三角形法向量
	Eigen::Vector3f CalTriNormal(Eigen::Vector3d ver1, Eigen::Vector3d ver2, Eigen::Vector3d ver3);

	//初始化OpenGL环境
	void InitializeOpenGL();

	//刷新OpenGL渲染设置
	void RefreshGLRenderParm();

	//按OpenGL相机参数将世界坐标系中的点投影到图像坐标系，返回图像坐标和深度值
	Vector3d GluProject(const Vector3d &worldPosition, const SGlCameraParam &cameraParam);

	//获取当前OpenGL相机参数
	SGlCameraParam GetGLCameraParam();
}