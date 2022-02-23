#pragma once

#include <Eigen/Dense>
#include <string>
#include "tinystr.h"
#include "tinyxml.h"
#include "filetool.h"
#include "masktool.h"
#include <opencv2/core/core.hpp>  
#include <opencv2/highgui/highgui.hpp>  
#include "../publicstruct.h"

using namespace std;
using namespace filetool;
using namespace masktool;

namespace phototool
{
	//照片参数
	struct SPhotoInfo
	{
		int id;
		string picName;

		int width;	//照片像素宽度
		int height;	//照片像素高度

		int picNumber;
		bool istowardsX;
		bool isTilt;	//是否倾斜
		string photoType;	//照片类型：0="A",1="B"
		double latitude;	//经度
		double longitude;	//纬度
		double altitude;	//海拔
		double speed_x;
		double speed_y;
		double speed_z;
		double x;
		double y;
		double z;
		double pitch;
		double yaw;
		double roll;
		double camera_pitch;
		double camera_yaw;
		double camera_roll;

		Vector4d rotate;	//相机旋转，用于绘制相机

		vector<SMask> masks;
	};

	//相机参数
	struct SCameraParam
	{
		//相机类型(Perspective:透视投影相机; Fisheye:鱼眼相机)
		string cameraModelType;

		//相机拍摄照片尺寸（单位:像素）
		int imageDimensions_Width;
		int imageDimensions_Height;

		//相机原点
		double principalPoint_X;
		double principalPoint_Y;

		//毫米焦距
		double focalLength;
		//像素焦距
		double focalLength_pixel;

		//相机畸变参数
		double distortion_K1;
		double distortion_K2;
		double distortion_K3;
		double distortion_P1;
		double distortion_P2;

		double sensorSize;
		//相机初始朝向,用于定义矩阵O
		string cameraOrientation;

		//纵横比
		double aspectRatio;

		//偏离,歪斜
		double skew;
	};

	//照片组,包括相机参数和对应照片信息
	struct SPhotoGroup
	{
		vector<SPhotoInfo> photoInfos;
		SCameraParam cameraParam;
	};

	//读取场景照片对应的OpenGL相机参数Json文件
	SGlCameraParam RSPOpenGLCParamJson(const string& fileName);
	//读取场景照片对应的OpenGL相机参数Json文件
	vector<SGlCameraParam> RSPOpenGLCParamInfosJson(const string& fileName);
	//将场景照片对应的OpenGL相机参数保存为Json文件
	void WSPOpenGLCParamJson(const string &fileName, const SGlCameraParam& cameParam);

	//读取XML文件中photoGroup信息，包括相机参数和对应的照片信息
	vector<SPhotoGroup> RPhotoGroupFromXML(const string &fileName);
	//读取mask文件中照片列表(ifReadMask:是否读取mask信息)
	vector<SPhotoInfo> RImagesFromMaskFile(const string &fileName, bool ifReadMask = false);
	//将照片以及照片包含的mask信息写入json文件
	void WImgMasksToJson(const vector<SPhotoInfo> &imgList, const string &fileName);

	//根据mask列表和图片生成maskedimg; transparency透明度
	void CreateImgMask(cv::Mat& image, vector<SMask> &masks, int w, int h, const vector<Eigen::Vector3d> mcolors, float transparency = 0.8);

	//保存OpenGL相机参数
	bool SaveGLCameraParams(const vector<SGlCameraParam> &glCameraParams, const string& fileName);
}