#pragma once
#include <iostream>
#include <string>
#include <vector>
#include <Eigen\Dense>
#include <opencv2/core/core.hpp>  
#include <opencv2/highgui/highgui.hpp>  
#include <opencv2/opencv.hpp>
#include <algorithm>
#include <iterator>

extern "C" {
#include "../openglTools/glm.h"
};

using namespace std;
using namespace Eigen;
using namespace cv;

namespace modeltool
{
	//将Vector3d数组转化为vector2d数组返回
	vector<Vector2d> ParseV3dToV2d(const vector<Vector3d>& vec3d);

	//向量数据min-max归一化
	template<typename T>
	void GetMinMaxValue(const vector<T> &data, T &min, T &max)
	{
		int dataSize = data.size();
		if (dataSize <= 0)
		{
			return;
		}

		min = data[0];
		max = data[0];

		for (size_t i = 0; i < dataSize; i++)
		{
			min = min > data[i] ? data[i] : min;
			max = max < data[i] ? data[i] : max;
		}
	}

	//计算三角形面积
	double CalcTriArea(const Vector3d v1, const Vector3d v2, const Vector3d v3);

	//获取顶点列表的中心点、xyz最小值, xyz最大值
	vector<Vector3d> GetMidMinMaxPointForVertices(const vector<Vector3d> &vertics);

	//获取顶点列表的包围框(OpenCV算法取矩形包围框)
	vector<Vector3d> GetBoundingBox(const vector<Vector3d> &vertics);

	//根据单个建筑面片索引创建新的obj模型
	GLMmodel* CreateSingleBuildModel(const GLMmodel* holeModel, const vector<int> &buildFaces);

	////根据单个建筑面片索引创建新的obj模型,并返回新模型顶点索引与原模型顶点索引对应表vMap, 以及面片索引对应表fMap
	//GLMmodel* CreateSingleBuildModel(const GLMmodel* holeModel, const vector<int> &buildFaces, vector<Vector2i> &vMap, vector<Vector2i> &fMap);

	//判断点是否在多边形内
	bool IfPointInPolygon(const Eigen::Vector2d &p, const vector<Eigen::Vector2d> &polygon);

	//判断点是否在多边形内
	bool IfPointInPolygon(const Eigen::Vector3d &p, const vector<Eigen::Vector3d> &polygon);

	//在XOY平面上按中心点扩大包围框范围
	void ExpandBoundingBox_XOY(vector<Eigen::Vector3d> &box, float expandSize);

	//获取点云在XOY平面上的多边形边界(AlphaShapes算法)
	vector<Eigen::Vector2d> GetPointsBoundary(const vector<Vector2d> &vertics, double m_radius);

	//获取点云在XOY平面上的多边形边界(AlphaShapes算法)
	vector<Eigen::Vector3d> GetPointsBoundary(const vector<Vector3d> &vertics, double m_radius);

	//计算点pt到线段p q的距离
	double DistancePoint2LineSeg(const Vector3d &pt, const Vector3d &p, const Vector3d &q);

	//向量数据min-max归一化
	template<typename T>
	void MinMaxNormalize(vector<T> &data)
	{
		int dataSize = data.size();
		if (dataSize <= 0)
		{
			return;
		}

		T min = data[0];
		T max = data[0];

		for (size_t i = 0; i < dataSize; i++)
		{
			min = min > data[i] ? data[i] : min;
			max = max < data[i] ? data[i] : max;
		}

		if (min == max)
		{
			for (size_t i = 0; i < dataSize; i++)
			{
				data[i] = 1;
			}
		}
		else
		{
			T dataRange = max - min;
			for (size_t i = 0; i < dataSize; i++)
			{
				data[i] = (data[i] - min) / dataRange;
			}
		}
	}

	//向量数据min-max归一化,按0分段，大于0和小于0的数据分开归一化
	template<typename T>
	void MinMaxNormalizeWith0(vector<T> &data, bool ifUseAbs = false)
	{
		int dataSize = data.size();
		if (dataSize <= 0)
		{
			return;
		}

		vector<T> data_Large0;	//大于0的数据
		vector<T> data_Small0;	//小于0的数据

		for (size_t i = 0; i < dataSize; i++)
		{
			if (data[i]>0)
			{
				data_Large0.push_back(data[i]);
			}
			else
			{
				data_Small0.push_back(data[i]);
			}
		}

		T min_data_Large0 = -999999999;
		T max_data_Large0 = 999999999;
		T min_data_Small0 = -999999999;
		T max_data_Small0 = 999999999;
		GetMinMaxValue(data_Large0, min_data_Large0, max_data_Large0);
		GetMinMaxValue(data_Small0, min_data_Small0, max_data_Small0);

		T disRange_Large0 = 0;
		T disRange_Small0 = 0;
		if (ifUseAbs)
		{
			disRange_Large0 = abs(max_data_Large0);
			disRange_Small0 = abs(min_data_Small0);
		}
		else
		{
			disRange_Large0 = max_data_Large0 - min_data_Large0;
			disRange_Small0 = max_data_Small0 - min_data_Small0;
		}

		T disRange = disRange_Large0 >= disRange_Small0 ? disRange_Large0 : disRange_Small0;
		std::cout << "disRange: " << disRange << endl;


		if (disRange == 0)
		{
			for (size_t i = 0; i < dataSize; i++)
			{
				data[i] = 1;
			}
		}
		else
		{
			for (size_t i = 0; i < dataSize; i++)
			{
				if (data[i] >= 0)
				{
					T distance = (data[i] - min_data_Large0) / disRange;
					data[i] = 0.5 - distance / 2.0;
				}
				else
				{
					T distance = (data[i] - min_data_Small0) / disRange;
					data[i] = 0.5 + distance / 2.0;
				}
			}
		}
	}

	//清空元素数目小于阈值threshold的向量
	void ClearVecGreThanSize(vector<vector<int>> &vect, int threshold);
	//筛选元素数目大于阈值threshold的向量
	void SelVecGreThanSize(vector<vector<int>> &vect, int threshold);

	//容器vector中元素的去重
	void unique_element_in_vector(vector<int> &v);

	//两个vector求交集
	vector<int> vectors_intersection(const vector<int> &v1, const vector<int> &v2);

	//两个vector求并集
	vector<int> vectors_set_union(const vector<int> &v1, const vector<int> &v2);
}
