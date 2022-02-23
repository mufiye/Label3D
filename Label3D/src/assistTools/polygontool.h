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
	bool PolygonClip(const vector<Vector2d> &poly1, const vector<Vector2d> &poly2, std::vector<Vector2d> &interPoly);//多边形求交集
	bool GetCrossPoint(const Vector2d &p1, const Vector2d &p2, const Vector2d &q1, const Vector2d &q2, double &x, double &y);//求线段交点
	bool IsPointInpolygon(std::vector<Vector2d> poly, Vector2d pt);//判断点是否在多边形内部
	void ClockwiseSortPoints(std::vector<Vector2d> &vPoints);//点集排序
	bool IsRectCross(const Vector2d &p1, const Vector2d &p2, const Vector2d &q1, const Vector2d &q2);//快速排斥实验，判断俩矩形对角线是否相交
	bool IsLineSegmentCross(const Vector2d &pFirst1, const Vector2d &pFirst2, const Vector2d &pSecond1, const Vector2d &pSecond2);//跨立判断，对角线相交，必跨立
	bool PointCmp(const Vector2d &a, const Vector2d &b, const Vector2d &center);//若点a大于点b,即点a在点b顺时针方向,返回true,否则返回false
	double determinant(double v1, double v2, double v3, double v4);  // 行列式  
	double mult(double x1, double y1, double x2, double y2, double x3, double y3);   //计算叉乘 
	double LinearIntegration(const Vector2d &p1, const Vector2d &p2);//定积分求面积
	double ComputePolygonArea(vector<Vector2d> &ptVector);//计算多边形面积
}

#endif
