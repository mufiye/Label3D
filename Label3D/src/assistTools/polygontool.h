#ifndef __RECT_UTILS__
#define __RECT_UTILS__
//#include <opencv2/opencv.hpp>
//#include <opencv2/features2d.hpp>
//using namespace std;
//using namespace cv;

#include <Eigen\Dense>
#include <vector>

using namespace std;
using namespace Eigen;

namespace polygontool
{
	bool PolygonClip(const vector<Vector2d> &poly1, const vector<Vector2d> &poly2, std::vector<Vector2d> &interPoly);//������󽻼�
	bool GetCrossPoint(const Vector2d &p1, const Vector2d &p2, const Vector2d &q1, const Vector2d &q2, double &x, double &y);//���߶ν���
	bool IsPointInpolygon(std::vector<Vector2d> poly, Vector2d pt);//�жϵ��Ƿ��ڶ�����ڲ�
	void ClockwiseSortPoints(std::vector<Vector2d> &vPoints);//�㼯����
	bool IsRectCross(const Vector2d &p1, const Vector2d &p2, const Vector2d &q1, const Vector2d &q2);//�����ų�ʵ�飬�ж������ζԽ����Ƿ��ཻ
	bool IsLineSegmentCross(const Vector2d &pFirst1, const Vector2d &pFirst2, const Vector2d &pSecond1, const Vector2d &pSecond2);//�����жϣ��Խ����ཻ���ؿ���
	bool PointCmp(const Vector2d &a, const Vector2d &b, const Vector2d &center);//����a���ڵ�b,����a�ڵ�b˳ʱ�뷽��,����true,���򷵻�false
	double determinant(double v1, double v2, double v3, double v4);  // ����ʽ  
	double mult(double x1, double y1, double x2, double y2, double x3, double y3);   //������ 
	double LinearIntegration(const Vector2d &p1, const Vector2d &p2);//�����������
	double ComputePolygonArea(vector<Vector2d> &ptVector);//�����������
}

#endif
