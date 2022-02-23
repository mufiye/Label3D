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
	//��Vector3d����ת��Ϊvector2d���鷵��
	vector<Vector2d> ParseV3dToV2d(const vector<Vector3d>& vec3d);

	//��������min-max��һ��
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

	//�������������
	double CalcTriArea(const Vector3d v1, const Vector3d v2, const Vector3d v3);

	//��ȡ�����б�����ĵ㡢xyz��Сֵ, xyz���ֵ
	vector<Vector3d> GetMidMinMaxPointForVertices(const vector<Vector3d> &vertics);

	//��ȡ�����б�İ�Χ��(OpenCV�㷨ȡ���ΰ�Χ��)
	vector<Vector3d> GetBoundingBox(const vector<Vector3d> &vertics);

	//���ݵ���������Ƭ���������µ�objģ��
	GLMmodel* CreateSingleBuildModel(const GLMmodel* holeModel, const vector<int> &buildFaces);

	////���ݵ���������Ƭ���������µ�objģ��,��������ģ�Ͷ���������ԭģ�Ͷ���������Ӧ��vMap, �Լ���Ƭ������Ӧ��fMap
	//GLMmodel* CreateSingleBuildModel(const GLMmodel* holeModel, const vector<int> &buildFaces, vector<Vector2i> &vMap, vector<Vector2i> &fMap);

	//�жϵ��Ƿ��ڶ������
	bool IfPointInPolygon(const Eigen::Vector2d &p, const vector<Eigen::Vector2d> &polygon);

	//�жϵ��Ƿ��ڶ������
	bool IfPointInPolygon(const Eigen::Vector3d &p, const vector<Eigen::Vector3d> &polygon);

	//��XOYƽ���ϰ����ĵ������Χ��Χ
	void ExpandBoundingBox_XOY(vector<Eigen::Vector3d> &box, float expandSize);

	//��ȡ������XOYƽ���ϵĶ���α߽�(AlphaShapes�㷨)
	vector<Eigen::Vector2d> GetPointsBoundary(const vector<Vector2d> &vertics, double m_radius);

	//��ȡ������XOYƽ���ϵĶ���α߽�(AlphaShapes�㷨)
	vector<Eigen::Vector3d> GetPointsBoundary(const vector<Vector3d> &vertics, double m_radius);

	//�����pt���߶�p q�ľ���
	double DistancePoint2LineSeg(const Vector3d &pt, const Vector3d &p, const Vector3d &q);

	//��������min-max��һ��
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

	//��������min-max��һ��,��0�ֶΣ�����0��С��0�����ݷֿ���һ��
	template<typename T>
	void MinMaxNormalizeWith0(vector<T> &data, bool ifUseAbs = false)
	{
		int dataSize = data.size();
		if (dataSize <= 0)
		{
			return;
		}

		vector<T> data_Large0;	//����0������
		vector<T> data_Small0;	//С��0������

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

	//���Ԫ����ĿС����ֵthreshold������
	void ClearVecGreThanSize(vector<vector<int>> &vect, int threshold);
	//ɸѡԪ����Ŀ������ֵthreshold������
	void SelVecGreThanSize(vector<vector<int>> &vect, int threshold);

	//����vector��Ԫ�ص�ȥ��
	void unique_element_in_vector(vector<int> &v);

	//����vector�󽻼�
	vector<int> vectors_intersection(const vector<int> &v1, const vector<int> &v2);

	//����vector�󲢼�
	vector<int> vectors_set_union(const vector<int> &v1, const vector<int> &v2);
}
