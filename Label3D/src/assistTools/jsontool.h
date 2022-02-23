#pragma once
#include "../openglTools/myopengltool.h"
#include <json/json.h>
#include <iostream>
#include <io.h>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <Eigen\Dense>   
#pragma warning (push) 
#pragma warning (disable: 4996) 
#include <memory> 
#include "../publicstruct.h"
#include "filetool.h"
//#include <opencv2/core/core.hpp>  
//#include <opencv2/highgui/highgui.hpp>  
//#include <direct.h>

using namespace std;
using namespace Eigen;

namespace jsontool
{
	//读取CityGML中建筑信息
	vector<SCityObject> RCityGMLInfo(const string& fileName);

	//将模型位置坐标信息写入json文件
	bool WModelPositionInfo(const vector<SModelPosInfo>& modelPosInfos, const string& saveJsonFile);

	//读取模型位置坐标信息
	vector<SModelPosInfo> RModelPositionInfo(const string& fileName);

}
