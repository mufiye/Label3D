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
	//����������ϵ�� ����world_posת��ΪͼƬ����ϵ������image_pos  ��Ƭ��Ϣphoto_info, �������camera_parameters
	Vector2d World2ImageProjection(const Vector3d &worldPos, const SPhotoInfo &photoInfo, const SCameraParam &cameraParams);

	/*
	������ά��������㵽��������ֵ(����ContextCapture�������������):
	type=1��ʾ��ͶӰ��ʽ��������������ת��������꣬Ȼ����㵽ԭ��ľ���
	type=2��ʾֱ�Ӽ��㶥�㵽������ĵ�ľ���
	*/
	double DisFromWorldPosToCameraAerial(const Vector3d &worldPos, const SPhotoInfo &photoInfo, const SCameraParam &cameraParams, int type = 1);

	/*
	������ά��������㵽��������ֵ(����OpenGL�������):
	type=1��ʾ��ͶӰ��ʽ��������������ת��������꣬Ȼ����㵽ԭ��ľ���
	type=2��ʾֱ�Ӽ��㶥�㵽������ĵ�ľ���(��δʵ��)
	����ֵ Vector3d[0],Vector3d[1] ��ʾͼ������x,y; Vector3d[2]��ʾ���
	*/
	Vector3d DisFromWorldPosToCameraGL(const Vector3d &worldPos, const SGlCameraParam &cameraParam, int type = 1);

	//��ȡ�ָ���ģ�ͷ�ͶӰ����ά�������ͼƬ_Aerial
	Mat GetBackProjectedImgAerial(const MeshGraph *meshGraph, const SPhotoInfo &photo_info, const SCameraParam &camera, const vector<int> &faceSegResult);
	//��ȡ�ָ���ģ�ͷ�ͶӰ����ά�������ͼƬ_OpenGL
	Mat GetBackProjectedImgOpenGL(const MeshGraph *meshGraph, const SGlCameraParam &cameParam, const vector<int> &faceSegResult);
}