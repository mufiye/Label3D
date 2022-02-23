#pragma once
#include "modeltool.h"
#include <vector>
#include "../model.h"
#include "phototool.h"
#include "math.h"
#include "../annotation.h"

using namespace modeltool;
using namespace phototool;

namespace projectiontool
{
	//将世界坐标系下 坐标world_pos转换为图片坐标系下坐标image_pos  照片信息photo_info, 相机参数camera_parameters
	Vector2d World2ImageProjection(const Vector3d &worldPos, const SPhotoInfo &photoInfo, const SCameraParam &cameraParams);

	/*
	计算三维世界坐标点到相机的深度值(根据ContextCapture导出的相机参数):
	type=1表示按投影公式将顶点世界坐标转到相机坐标，然后计算到原点的距离
	type=2表示直接计算顶点到相机中心点的距离
	*/
	double DisFromWorldPosToCameraAerial(const Vector3d &worldPos, const SPhotoInfo &photoInfo, const SCameraParam &cameraParams, int type = 1);

	/*
	计算三维世界坐标点到相机的深度值(根据OpenGL相机参数):
	type=1表示按投影公式将顶点世界坐标转到相机坐标，然后计算到原点的距离
	type=2表示直接计算顶点到相机中心点的距离(尚未实现)
	返回值 Vector3d[0],Vector3d[1] 表示图像坐标x,y; Vector3d[2]表示深度
	*/
	Vector3d DisFromWorldPosToCameraGL(const Vector3d &worldPos, const SGlCameraParam &cameraParam, int type = 1);

	//获取分割后的模型反投影回三维场景后的图片_Aerial
	Mat GetBackProjectedImgAerial(const MeshGraph *meshGraph, const SPhotoInfo &photo_info, const SCameraParam &camera, const vector<int> &faceSegResult);
	//获取分割后的模型反投影回三维场景后的图片_OpenGL
	Mat GetBackProjectedImgOpenGL(const MeshGraph *meshGraph, const SGlCameraParam &cameParam, const vector<int> &faceSegResult);
}