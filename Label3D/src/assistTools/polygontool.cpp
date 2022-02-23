#include "polygontool.h"

//多边形求交集
bool polygontool::PolygonClip(const vector<Vector2d> &poly1, const vector<Vector2d> &poly2, std::vector<Vector2d> &interPoly)
{
	if (poly1.size() < 3 || poly2.size() < 3)
	{
		return false;
	}

	double x, y;
	//计算多边形交点
	for (unsigned int i = 0; i < poly1.size(); i++)
	{
		int poly1_next_idx = (i + 1) % poly1.size();
		for (unsigned int j = 0; j < poly2.size(); j++)
		{
			int poly2_next_idx = (j + 1) % poly2.size();
			if (GetCrossPoint(poly1[i], poly1[poly1_next_idx],
				poly2[j], poly2[poly2_next_idx],
				x, y))
			{
				interPoly.push_back(Vector2d(x, y));
			}
		}
	}
	//cout << "交点在边界坐标：" << interPoly << endl;

	//计算多边形内部点
	for (unsigned int i = 0; i < poly1.size(); i++)
	{
		if (IsPointInpolygon(poly2, poly1[i]))
		{
			interPoly.push_back(poly1[i]);
		}
	}
	//cout << "交点在边界以及矩形1内坐标：" << interPoly << endl;
	for (unsigned int i = 0; i < poly2.size(); i++)
	{
		if (IsPointInpolygon(poly1, poly2[i]))
		{
			interPoly.push_back(poly2[i]);
		}
	}
	//cout << "交点在边界以及矩形1内以及转化后矩形2内的坐标：" << interPoly << endl;

	if (interPoly.size() <= 0)
		return false;

	//点集排序 
	ClockwiseSortPoints(interPoly);
	//cout << "返回主程序点集排序：" << interPoly << endl;
	return true;
}

// 求线段交点
bool polygontool::GetCrossPoint(const Vector2d &p1, const Vector2d &p2, const Vector2d &q1, const Vector2d &q2, double &x, double &y)
{
	if (IsRectCross(p1, p2, q1, q2))
	{
		if (IsLineSegmentCross(p1, p2, q1, q2))
		{
			//cout << "相交线段1坐标1"<<p1 << endl;
			//cout << "相交线段1坐标2" << p2 << endl;
			//cout << "相交线段2坐标1" << q1 << endl;
			//cout << "相交线段2坐标2" << q2 << endl;
			//求交点
			double tmpLeft, tmpRight;
			tmpLeft = (q2[0] - q1[0]) * (p1[1] - p2[1]) - (p2[0] - p1[0]) * (q1[1] - q2[1]);
			tmpRight = (p1[1] - q1[1]) * (p2[0] - p1[0]) * (q2[0] - q1[0]) + q1[0] * (q2[1] - q1[1]) * (p2[0] - p1[0]) - p1[0] * (p2[1] - p1[1]) * (q2[0] - q1[0]);
			x = (double)(tmpRight / tmpLeft);//int
			//cout << "线段相交坐标x：" << x << endl;

			tmpLeft = (p1[0] - p2[0]) * (q2[1] - q1[1]) - (p2[1] - p1[1]) * (q1[0] - q2[0]);
			tmpRight = p2[1] * (p1[0] - p2[0]) * (q2[1] - q1[1]) + (q2[0] - p2[0]) * (q2[1] - q1[1]) * (p1[1] - p2[1]) - q2[1] * (q1[0] - q2[0]) * (p2[1] - p1[1]);
			y = (double)((double)tmpRight*1.0 / (double)tmpLeft);//int
			//cout << "线段相交坐标y：" << y << endl;

			return true;
		}
	}
	return false;
}

//判断点是否在多边形内部
bool polygontool::IsPointInpolygon(std::vector<Vector2d> poly, Vector2d pt)
{
	int i, j;
	bool c = false;
	for (i = 0, j = poly.size() - 1; i < poly.size(); j = i++)
	{
		if ((((poly[i][1] <= pt[1]) && (pt[1] < poly[j][1])) ||
			((poly[j][1] <= pt[1]) && (pt[1] < poly[i][1])))
			&& (pt[0] < (poly[j][0] - poly[i][0]) * (pt[1] - poly[i][1]) / (poly[j][1] - poly[i][1]) + poly[i][0]))
		{
			c = !c;
		}
	}
	return c;
}

//点集排序
void polygontool::ClockwiseSortPoints(std::vector<Vector2d> &vPoints)
{
	//计算重心
	Vector2d center;
	double x = 0, y = 0;

	for (int i = 0; i < vPoints.size(); i++)
	{
		x += vPoints[i][0];
		y += vPoints[i][1];
	}
	center[0] = x / vPoints.size();
	center[1] = y / vPoints.size();
	//cout << "重心横坐标为：" << center[0] << endl;
	//cout << "重心纵坐标为：" << center[1] << endl;

	//冒泡排序
	for (int i = 0; i < vPoints.size() - 1; i++)
	{
		for (int j = 0; j < vPoints.size() - i - 1; j++)
		{
			if (PointCmp(vPoints[j], vPoints[j + 1], center))
			{
				Vector2d tmp = vPoints[j];
				vPoints[j] = vPoints[j + 1];
				vPoints[j + 1] = tmp;
			}
		}
		//cout << "子程序点集排序：" << vPoints << endl;
	}

}

//跨立判断，对角线相交，必跨立
bool polygontool::IsRectCross(const Vector2d &p1, const Vector2d &p2, const Vector2d &q1, const Vector2d &q2)
{
	bool ret = min(p1[0], p2[0]) <= max(q1[0], q2[0]) &&
		min(q1[0], q2[0]) <= max(p1[0], p2[0]) &&
		min(p1[1], p2[1]) <= max(q1[1], q2[1]) &&
		min(q1[1], q2[1]) <= max(p1[1], p2[1]);
	return ret;
}

//线段相交判断
bool polygontool::IsLineSegmentCross(const Vector2d &pFirst1, const Vector2d &pFirst2, const Vector2d &pSecond1, const Vector2d &pSecond2)
{
	//方法一
	/*long line1, line2;
	line1 = pFirst1[0] * (pSecond1[1] - pFirst2[1]) +
	pFirst2[0] * (pFirst1[1] - pSecond1[1]) +
	pSecond1[0] * (pFirst2[1] - pFirst1[1]);
	line2 = pFirst1[0] * (pSecond2[1] - pFirst2[1]) +
	pFirst2[0] * (pFirst1[1] - pSecond2[1]) +
	pSecond2[0] * (pFirst2[1] - pFirst1[1]);
	if (((line1 ^ line2) >= 0) && !(line1 == 0 && line2 == 0))
	return false;

	line1 = pSecond1[0] * (pFirst1[1] - pSecond2[1]) +
	pSecond2[0] * (pSecond1[1] - pFirst1[1]) +
	pFirst1[0] * (pSecond2[1] - pSecond1[1]);
	line2 = pSecond1[0] * (pFirst2[1] - pSecond2[1]) +
	pSecond2[0] * (pSecond1[1] - pFirst2[1]) +
	pFirst2[0] * (pSecond2[1] - pSecond1[1]);
	if (((line1 ^ line2) >= 0) && !(line1 == 0 && line2 == 0))
	return false;
	return true;*/

	//方法二
	/*double fC = (pSecond1[1] - pFirst1[1]) * (pFirst1[0] - pFirst2[0]) - (pSecond1[0] - pFirst1[0]) * (pFirst1[1] - pFirst2[1]);
	double fD = (pSecond2[1] - pFirst1[1]) * (pFirst1[0] - pFirst2[0]) - (pSecond2[0] - pFirst1[0]) * (pFirst1[1] - pFirst2[1]);

	if (fC * fD > 0)
	return true;
	return false;
	double fE = (pFirst1[1] - pSecond1[1]) * (pSecond1[0] - pSecond2[0]) - (pFirst1[0] - pSecond1[0]) * (pSecond1[1] - pSecond2[1]);
	double fF = (pFirst2[1] - pSecond1[1]) * (pSecond1[0] - pSecond2[0]) - (pFirst2[0] - pSecond1[0]) * (pSecond1[1] - pSecond2[1]);
	if (fE * fF > 0)
	return true;
	return false;*/

	//方法三
	//double delta = determinant(pFirst2[0] - pFirst1[0], pSecond1[0] - pSecond2[0], pFirst2[1] - pFirst1[1], pSecond1[1] - pSecond2[1]);
	//if (delta <= (1e-6) && delta >= -(1e-6))  // delta=0，表示两线段重合或平行  
	//{
	//	return false;
	//}
	//double namenda = determinant(pSecond1[0] - pFirst1[0], pSecond1[0] - pSecond2[0], pSecond1[1] - pFirst1[1], pSecond1[1] - pSecond2[1]) / delta;
	//if (namenda>1 || namenda<0)
	//{
	//	return false;
	//}
	//double miu = determinant(pFirst2[0] - pFirst1[0], pSecond1[0] - pFirst1[0], pFirst2[1] - pFirst1[1], pSecond1[1] - pFirst1[1]) / delta;
	//if (miu>1 || miu<0)
	//{
	//	return false;
	//}
	//return true;

	//方法四
	if (max(pFirst1[0], pFirst2[0]) < min(pSecond1[0], pSecond2[0]))  return false;
	if (max(pFirst1[1], pFirst2[1]) < min(pSecond1[1], pSecond2[1]))  return false;
	if (max(pSecond1[0], pSecond2[0]) < min(pFirst1[0], pFirst2[0]))  return false;
	if (max(pSecond1[1], pSecond2[1]) < min(pFirst1[1], pFirst2[1]))  return false;
	if (mult(pSecond1[0], pSecond1[1], pFirst2[0], pFirst2[1], pFirst1[0], pFirst1[1])*mult(pFirst2[0], pFirst2[1], pSecond2[0], pSecond2[1], pFirst1[0], pFirst1[1])<0)    return false;
	if (mult(pFirst1[0], pFirst1[1], pSecond2[0], pSecond2[1], pSecond1[0], pSecond1[1])*mult(pSecond2[0], pSecond2[1], pFirst2[0], pFirst2[1], pSecond1[0], pSecond1[1])<0)    return false;
	return true;

}
//判断线段相交方法三中求行列式
double polygontool::determinant(double v1, double v2, double v3, double v4)  // 行列式  
{
	return (v1*v3 - v2*v4);
}
//判断线段相交方法四中计算叉乘
double polygontool::mult(double x1, double y1, double x2, double y2, double x3, double y3)   //计算叉乘 
{
	return (x1 - x3)*(y2 - y3) - (x2 - x3)*(y1 - y3);
}

//若点a大于点b,即点a在点b顺时针方向,返回true,否则返回false
bool polygontool::PointCmp(const Vector2d &a, const Vector2d &b, const Vector2d &center)
{
	if (a[0] >= 0 && b[0] < 0)
		return true;
	if (a[0] == 0 && b[0] == 0)
		return a[1] > b[1];
	//向量OA和向量OB的叉积
	double det = (a[0] - center[0]) * (b[1] - center[1]) - (b[0] - center[0]) * (a[1] - center[1]);//int
	if (det < 0)
		return true;
	if (det > 0)
		return false;
	//向量OA和向量OB共线，以距离判断大小
	double d1 = (a[0] - center[0]) * (a[0] - center[0]) + (a[1] - center[1]) * (a[1] - center[1]);//int
	double d2 = (b[0] - center[0]) * (b[0] - center[1]) + (b[1] - center[1]) * (b[1] - center[1]);//int
	return d1 > d2;
}

//定积分计算公式
double polygontool::LinearIntegration(const Vector2d &p1, const Vector2d &p2) {
	return 0.5 * (p2[0] - p1[0]) * (p2[1] + p1[1]);
}

double polygontool::ComputePolygonArea(vector<Vector2d> &ptVector)
{
	if (ptVector.size() == NULL) return 0.0;
	double area = 0.0000000000000;
	for (int i = 0; i < ptVector.size() - 1; i++) {
		area += LinearIntegration(ptVector[i], ptVector[i + 1]);
	}
	area += LinearIntegration(ptVector[ptVector.size() - 1], ptVector[0]);
	return area >= 0.0 ? area : -area;
}