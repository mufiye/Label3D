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
	//��Ƭ����
	struct SPhotoInfo
	{
		int id;
		string picName;

		int width;	//��Ƭ���ؿ��
		int height;	//��Ƭ���ظ߶�

		int picNumber;
		bool istowardsX;
		bool isTilt;	//�Ƿ���б
		string photoType;	//��Ƭ���ͣ�0="A",1="B"
		double latitude;	//����
		double longitude;	//γ��
		double altitude;	//����
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

		Vector4d rotate;	//�����ת�����ڻ������

		vector<SMask> masks;
	};

	//�������
	struct SCameraParam
	{
		//�������(Perspective:͸��ͶӰ���; Fisheye:�������)
		string cameraModelType;

		//���������Ƭ�ߴ磨��λ:���أ�
		int imageDimensions_Width;
		int imageDimensions_Height;

		//���ԭ��
		double principalPoint_X;
		double principalPoint_Y;

		//���׽���
		double focalLength;
		//���ؽ���
		double focalLength_pixel;

		//����������
		double distortion_K1;
		double distortion_K2;
		double distortion_K3;
		double distortion_P1;
		double distortion_P2;

		double sensorSize;
		//�����ʼ����,���ڶ������O
		string cameraOrientation;

		//�ݺ��
		double aspectRatio;

		//ƫ��,��б
		double skew;
	};

	//��Ƭ��,������������Ͷ�Ӧ��Ƭ��Ϣ
	struct SPhotoGroup
	{
		vector<SPhotoInfo> photoInfos;
		SCameraParam cameraParam;
	};

	//��ȡ������Ƭ��Ӧ��OpenGL�������Json�ļ�
	SGlCameraParam RSPOpenGLCParamJson(const string& fileName);
	//��ȡ������Ƭ��Ӧ��OpenGL�������Json�ļ�
	vector<SGlCameraParam> RSPOpenGLCParamInfosJson(const string& fileName);
	//��������Ƭ��Ӧ��OpenGL�����������ΪJson�ļ�
	void WSPOpenGLCParamJson(const string &fileName, const SGlCameraParam& cameParam);

	//��ȡXML�ļ���photoGroup��Ϣ��������������Ͷ�Ӧ����Ƭ��Ϣ
	vector<SPhotoGroup> RPhotoGroupFromXML(const string &fileName);
	//��ȡmask�ļ�����Ƭ�б�(ifReadMask:�Ƿ��ȡmask��Ϣ)
	vector<SPhotoInfo> RImagesFromMaskFile(const string &fileName, bool ifReadMask = false);
	//����Ƭ�Լ���Ƭ������mask��Ϣд��json�ļ�
	void WImgMasksToJson(const vector<SPhotoInfo> &imgList, const string &fileName);

	//����mask�б��ͼƬ����maskedimg; transparency͸����
	void CreateImgMask(cv::Mat& image, vector<SMask> &masks, int w, int h, const vector<Eigen::Vector3d> mcolors, float transparency = 0.8);

	//����OpenGL�������
	bool SaveGLCameraParams(const vector<SGlCameraParam> &glCameraParams, const string& fileName);
}