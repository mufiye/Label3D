#pragma once
#include <iostream>
#include <io.h>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#pragma warning (push) 
#pragma warning (disable: 4996) 
#include <json/json.h>
#include <memory> 
#include <Eigen\Dense>   
#include <direct.h>
#include  "../publicstruct.h"
#include <opencv2/core/core.hpp>  
#include <opencv2/highgui/highgui.hpp>  
#include "../assistTools/masktool.h"

using namespace std;
using namespace Eigen;
//using namespace cv;

namespace filetool
{
	//int����ת��Ϊstring����
	string ItoS(int nunm);
	//ȥ����β�ո�
	string Trim(const string &s);
	//��c�ַ��ָ��ַ���s,�ָ���������v��
	void SplitString(const string& s, vector<string>& v, const string& c);
	//��c�ַ��ָ��ַ���s,�ָ���������v��(ֻ������һ��c�ַ�����c�ַ��ָ��2��)
	void SplitStringTo2(const string& s, vector<string>& v, const string& c);
	//��ȡ�ļ���׺��
	string GetFileSuffixName(const string& fileName);
	//��ȡ�ļ���(��������׺)
	string GetFileNameNoSuffix(const string& fileName);
	//ֻ��ȡĳ����·���µĵ�ǰ�ļ�����
	void GetJustCurrentDir(const string& path, vector<string>& files);
	//��ȡ�����ļ����������ļ���
	void GetJustCurrentFile(const string& path, vector<string>& files);

	//�����������б����Ϊ�ַ���
	string EncodePIndex(const vector<int> &points);
	//���ַ�������Ϊ���������б�
	vector<int> DecodePIndexString(string &pString);

	//��ȡtxt�ļ�������
	vector<int> ReadTxtFileInt(const string& fileName);
	vector<float> ReadTxtFileFloat(const string& fileName);

	//��ȡ���ո�ֿ���int����txt����
	vector<int> ReadTxtFileInt_Space(const string& fileName);

	//д��txt�ļ�
	bool WriteTXTFileInt(const vector<int>& data, const string& fileName);
	bool WriteTXTFileFloat(const vector<float>& data, const string& fileName);

	//��ȡ��������
	vector<Vector3d> ReadPointsTxt(const string &fileName);
	//����������д��txt�ļ�
	bool WriteTXTFileVec3d(const vector<Vector3d>& data, const string& fileName);

	//��ȡ��Ŀ�ļ�
	SProjectInfo ReadPorjectInfo(const string& projectFile);

	//��ģ������Ƭ�ϵ����ͼ����Ϊ�Ҷ�ͼ
	void WModelImgDeepMapsToGray(cv::Mat &deepImage, string &fileName);

	//��Mat���ݱ���Ϊtxt
	void WMatToTxt(const cv::Mat &mat,const string &fileName, int effiBit = 2);
	//��Mat���ݱ���ΪXML
	void WMatToXML(const cv::Mat &mat, const string &fileName);
	//��ȡ�������
	cv::Mat RMatDataFromTxtDouble(string &fileName, int img_w, int img_h);
	cv::Mat RMatDataFromTxtDouble2(string &fileName, int img_w, int img_h);

	//����mask֮�����ƶȾ���
	bool WMaskSimiMatrix(int maskCount, float** matrix, const string &fileName);
	//��ȡmask���ƶȾ���
	float** RMaskSimiMatrix(const string &fileName, int &maskCount);
}