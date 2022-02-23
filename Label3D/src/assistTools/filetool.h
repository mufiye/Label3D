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
	//int类型转换为string类型
	string ItoS(int nunm);
	//去掉首尾空格
	string Trim(const string &s);
	//按c字符分割字符串s,分割结果保存在v中
	void SplitString(const string& s, vector<string>& v, const string& c);
	//按c字符分割字符串s,分割结果保存在v中(只搜索第一个c字符，按c字符分割成2段)
	void SplitStringTo2(const string& s, vector<string>& v, const string& c);
	//获取文件后缀名
	string GetFileSuffixName(const string& fileName);
	//获取文件名(不包含后缀)
	string GetFileNameNoSuffix(const string& fileName);
	//只读取某给定路径下的当前文件夹名
	void GetJustCurrentDir(const string& path, vector<string>& files);
	//读取给定文件夹下所有文件名
	void GetJustCurrentFile(const string& path, vector<string>& files);

	//将顶点索引列表编码为字符串
	string EncodePIndex(const vector<int> &points);
	//将字符串解码为顶点索引列表
	vector<int> DecodePIndexString(string &pString);

	//读取txt文件中数据
	vector<int> ReadTxtFileInt(const string& fileName);
	vector<float> ReadTxtFileFloat(const string& fileName);

	//读取按空格分开的int类型txt数据
	vector<int> ReadTxtFileInt_Space(const string& fileName);

	//写入txt文件
	bool WriteTXTFileInt(const vector<int>& data, const string& fileName);
	bool WriteTXTFileFloat(const vector<float>& data, const string& fileName);

	//读取点云数据
	vector<Vector3d> ReadPointsTxt(const string &fileName);
	//将点云数据写入txt文件
	bool WriteTXTFileVec3d(const vector<Vector3d>& data, const string& fileName);

	//读取项目文件
	SProjectInfo ReadPorjectInfo(const string& projectFile);

	//将模型在照片上的深度图保存为灰度图
	void WModelImgDeepMapsToGray(cv::Mat &deepImage, string &fileName);

	//将Mat数据保存为txt
	void WMatToTxt(const cv::Mat &mat,const string &fileName, int effiBit = 2);
	//将Mat数据保存为XML
	void WMatToXML(const cv::Mat &mat, const string &fileName);
	//读取深度数据
	cv::Mat RMatDataFromTxtDouble(string &fileName, int img_w, int img_h);
	cv::Mat RMatDataFromTxtDouble2(string &fileName, int img_w, int img_h);

	//保存mask之间相似度矩阵
	bool WMaskSimiMatrix(int maskCount, float** matrix, const string &fileName);
	//读取mask相似度矩阵
	float** RMaskSimiMatrix(const string &fileName, int &maskCount);
}