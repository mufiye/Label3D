#include "modeltool.h"

//容器vector中元素的去重
void modeltool::unique_element_in_vector(vector<int> &v)
{
	vector<int>::iterator vector_iterator;
	sort(v.begin(), v.end());
	vector_iterator = unique(v.begin(), v.end());
	if (vector_iterator != v.end()) {
		v.erase(vector_iterator, v.end());
	}
}

//两个vector求交集
vector<int> modeltool::vectors_intersection(const vector<int> &v1, const vector<int> &v2)
{
	vector<int> v;
	vector<int> v1_copy(v1);
	vector<int> v2_copy(v2);

	sort(v1_copy.begin(), v1_copy.end());
	sort(v2_copy.begin(), v2_copy.end());
	set_intersection(v1_copy.begin(), v1_copy.end(), v2_copy.begin(), v2_copy.end(), back_inserter(v));//求交集 
	return v;
}

//两个vector求并集
vector<int> modeltool::vectors_set_union(const vector<int> &v1, const vector<int> &v2)
{
	vector<int> v;
	vector<int> v1_copy(v1);
	vector<int> v2_copy(v2);
	sort(v1_copy.begin(), v1_copy.end());
	sort(v2_copy.begin(), v2_copy.end());
	set_union(v1_copy.begin(), v1_copy.end(), v2_copy.begin(), v2_copy.end(), back_inserter(v));//求交集 
	return v;
}

//将Vector3d数组转化为vector2d数组返回
vector<Vector2d> modeltool::ParseV3dToV2d(const vector<Vector3d>& vec3d)
{
	int vSize = vec3d.size();
	vector<Vector2d> vec2d(vSize);
	for (size_t i = 0; i < vSize; i++)
	{
		vec2d[i] = Vector2d(vec3d[i][0], vec3d[i][1]);
	}

	return vec2d;
}

//计算两点之间距离
double getDistance(const Vector3d v1, const Vector3d v2)
{
	return sqrt((v1[0] - v2[0])*(v1[0] - v2[0]) + (v1[1] - v2[1])*(v1[1] - v2[1]) + (v1[2] - v2[2])*(v1[2] - v2[2]));
}

//计算三角形面积
double modeltool::CalcTriArea(const Vector3d v1, const Vector3d v2, const Vector3d v3)
{
	//先求边长，在求面积
	double length1 = getDistance(v1, v2);
	double length2 = getDistance(v2, v3);
	double length3 = getDistance(v1, v3);
	double  p = (length1 + length2 + length3) / 2;
	double area = sqrt(p*(p - length1)*(p - length2)*(p - length3));
	return area;
}

//获取顶点列表的中心点、xyz最小值, xyz最大值
vector<Vector3d> modeltool::GetMidMinMaxPointForVertices(const vector<Vector3d> &vertics)
{
	vector<Vector3d> result;

	Vector3d midPoint, minPoint, maxPoint;
	int vSize = vertics.size();
	assert(vertics.size() > 0);
	maxPoint = vertics[0];
	minPoint = vertics[0];
	for (size_t i = 0; i < vSize; i++)
	{
		minPoint[0] = minPoint[0] <= vertics[i][0] ? minPoint[0] : vertics[i][0];
		minPoint[1] = minPoint[1] <= vertics[i][1] ? minPoint[1] : vertics[i][1];
		minPoint[2] = minPoint[2] <= vertics[i][2] ? minPoint[2] : vertics[i][2];

		maxPoint[0] = maxPoint[0] >= vertics[i][0] ? maxPoint[0] : vertics[i][0];
		maxPoint[1] = maxPoint[1] >= vertics[i][1] ? maxPoint[1] : vertics[i][1];
		maxPoint[2] = maxPoint[2] >= vertics[i][2] ? maxPoint[2] : vertics[i][2];
	}
	midPoint[0] = (minPoint[0] + maxPoint[0]) / 2.0;
	midPoint[1] = (minPoint[1] + maxPoint[1]) / 2.0;
	midPoint[2] = (minPoint[2] + maxPoint[2]) / 2.0;

	result.push_back(midPoint);
	result.push_back(minPoint);
	result.push_back(maxPoint);

	return result;
}

//获取顶点列表的包围框(OpenCV算法取矩形包围框)
vector<Vector3d> modeltool::GetBoundingBox(const vector<Vector3d> &vertics)
{
	vector<Vector3d> bBox;

	vector<cv::Point2f> points;
	int vSize = vertics.size();
	for (size_t i = 0; i < vSize; i++)
	{
		cv::Point2f p;
		p.x = vertics[i][0];
		p.y = vertics[i][1];
		points.push_back(p);
	}

	//在生成的那些随机点中寻找最小包围矩形
	//rectPoint变量中得到了矩形的四个顶点坐标
	//std::cout << "开始计算包围框 points.size()： " << points.size() << endl;
	RotatedRect rectPoint = minAreaRect(points);
	//std::cout << "计算包围框完成 " << endl;
	//定义一个存储以上四个点的坐标的变量
	Point2f fourPoint2f[4];
	//将rectPoint变量中存储的坐标值放到 fourPoint的数组中
	rectPoint.points(fourPoint2f);
	vector<cv::Point2f> recPointVec;
	for (int i = 0; i < 4; i++)
	{
		recPointVec.push_back(fourPoint2f[i]);
	}

	for (size_t i = 0; i < recPointVec.size(); i++)
	{
		bBox.push_back(Vector3d(recPointVec[i].x, recPointVec[i].y, 0));
	}

	return bBox;
}

//根据单个建筑面片索引创建新的obj模型
GLMmodel* modeltool::CreateSingleBuildModel(const GLMmodel* holeModel, const vector<int> &buildFaces)
{
	/*取出面片包含的所有顶点*/
	int buildFaceCount = buildFaces.size();
	vector<bool> ifInVertices(holeModel->numvertices + 1);
	for (int i = 0; i < (holeModel->numvertices + 1); i++)
	{
		ifInVertices[i] = false;
	}
	for (int i = 0; i < buildFaceCount; i++)
	{
		ifInVertices[holeModel->triangles[buildFaces[i]].vindices[0]] = true;
		ifInVertices[holeModel->triangles[buildFaces[i]].vindices[1]] = true;
		ifInVertices[holeModel->triangles[buildFaces[i]].vindices[2]] = true;
	}
	vector<int> buildVertics;
	for (int i = 0; i < (holeModel->numvertices + 1); i++)
	{
		if (ifInVertices[i])
		{
			buildVertics.push_back(i);
		}
	}
	//建立原全部顶点索引到新的部分顶点索引的对应关系
	vector<int> vIndexMapping(holeModel->numvertices + 1);
	for (int i = 0; i < (holeModel->numvertices + 1); i++)
	{
		vIndexMapping[i] = -1;
	}
	int buildVertexCount = buildVertics.size();
	for (int i = 0; i < buildVertexCount; i++)
	{
		vIndexMapping[buildVertics[i]] = i + 1;
	}

	GLMmodel* reModel = new GLMmodel();
	reModel = (GLMmodel*)malloc(sizeof(GLMmodel));
	reModel->pathname = _strdup("default");
	reModel->mtllibname = NULL;
	reModel->numvertices = 0;
	reModel->numnormals = 0;
	reModel->normals = NULL;
	reModel->numtexcoords = 0;
	reModel->texcoords = NULL;
	reModel->numfacetnorms = 0;
	reModel->facetnorms = NULL;
	reModel->numtriangles = 0;
	reModel->triangles = NULL;
	reModel->nummaterials = 0;
	reModel->materials = NULL;
	reModel->numgroups = 0;
	reModel->groups = NULL;
	reModel->position[0] = 0.0;
	reModel->position[1] = 0.0;
	reModel->position[2] = 0.0;

	reModel->numvertices = buildVertics.size();
	reModel->numtriangles = buildFaces.size();

	reModel->vertices = (GLfloat*)malloc(sizeof(GLfloat) *
		3 * (reModel->numvertices + 1));
	reModel->triangles = (GLMtriangle*)malloc(sizeof(GLMtriangle)*
		reModel->numtriangles);

	GLMgroup* group = new GLMgroup();
	group->numtriangles = buildFaces.size();
	group->triangles = (GLuint*)malloc(sizeof(GLuint)* group->numtriangles);


	//从索引1开始，0为-最大
	for (int i = 1; i <= reModel->numvertices; i++)
	{
		reModel->vertices[i * 3] = holeModel->vertices[3 * buildVertics[i-1] + 0];
		reModel->vertices[i * 3 + 1] = holeModel->vertices[3 * buildVertics[i-1] + 1];
		reModel->vertices[i * 3 + 2] = holeModel->vertices[3 * buildVertics[i-1] + 2];
	}

	for (int i = 0; i < reModel->numtriangles; i++)
	{
		reModel->triangles[i].vindices[0] = vIndexMapping[holeModel->triangles[buildFaces[i]].vindices[0]];
		reModel->triangles[i].vindices[1] = vIndexMapping[holeModel->triangles[buildFaces[i]].vindices[1]];
		reModel->triangles[i].vindices[2] = vIndexMapping[holeModel->triangles[buildFaces[i]].vindices[2]];

		reModel->triangles[i].material = 0;
	}

	for (int i = 0; i < reModel->numtriangles; i++)
	{
		group->triangles[i] = i;
	}

	reModel->numgroups = 1;
	reModel->groups = group;
	return reModel;
}

//判断点是否在多边形内
bool modeltool::IfPointInPolygon(const Eigen::Vector2d &p, const vector<Eigen::Vector2d> &polygon)
{
	int   i, j = polygon.size() - 1;
	bool  oddNodes = false;

	for (i = 0; i<polygon.size(); i++) {
		if ((polygon[i].y()< p.y() && polygon[j].y() >= p.y()
			|| polygon[j].y()<p.y() && polygon[i].y() >= p.y())
			&& (polygon[i].x() <= p.x() || polygon[j].x() <= p.x())) {
			oddNodes ^= (polygon[i].x() + (p.y() - polygon[i].y()) / (polygon[j].y() - polygon[i].y())*(polygon[j].x() - polygon[i].x())<p.x());
		}
		j = i;
	}

	return oddNodes;
}

//判断点是否在多边形内
bool modeltool::IfPointInPolygon(const Eigen::Vector3d &p, const vector<Eigen::Vector3d> &polygon)
{
	int   i, j = polygon.size() - 1;
	bool  oddNodes = false;

	for (i = 0; i<polygon.size(); i++) {
		if ((polygon[i].y()< p.y() && polygon[j].y() >= p.y()
			|| polygon[j].y()<p.y() && polygon[i].y() >= p.y())
			&& (polygon[i].x() <= p.x() || polygon[j].x() <= p.x())) {
			oddNodes ^= (polygon[i].x() + (p.y() - polygon[i].y()) / (polygon[j].y() - polygon[i].y())*(polygon[j].x() - polygon[i].x())<p.x());
		}
		j = i;
	}

	return oddNodes;
}

//在XOY平面上按中心点扩大包围框范围
void modeltool::ExpandBoundingBox_XOY(vector<Eigen::Vector3d> &box, float expandSize)
{
	//计算包围框中心点
	float sumx = 0, sumy = 0;
	float midx = 0, midy = 0;
	for (int i = 0; i < box.size(); i++)
	{
		sumx += box[i].x();
		sumy += box[i].y();
	}
	midx = box.size() == 0 ? 0 : sumx / (float)box.size();
	midy = box.size() == 0 ? 0 : sumy / (float)box.size();
	for (int i = 0; i < box.size(); i++)
	{
		Vector2f direction(box[i].x() - midx, box[i].y() - midy);
		direction.normalize();
		if (expandSize >= 0)
		{
			box[i].x() += direction.x() * expandSize;
			box[i].y() += direction.y() * expandSize;
		}
		else
		{
			//判断包围框长宽的一半是否小于exPandBoxSize,若是,则缩小至原来一半
			float halfLength = sqrt(pow(box[0].x() - midx, 2) + pow(box[0].y() - midy, 2));
			float halfWight = sqrt(pow(box[2].x() - midx, 2) + pow(box[2].y() - midy, 2));
			float half = halfLength > halfWight ? halfWight / 2.0 : halfLength / 2.0;
			if (half > abs(expandSize))
			{
				box[i].x() += direction.x() * expandSize;
				box[i].y() += direction.y() * expandSize;
			}
			else
			{
				box[i].x() += direction.x() * (-half);
				box[i].y() += direction.y() * (-half);
			}
		}
	}
}

struct Edge
{
	Vector2d a;
	Vector2d b;
	Vector2d par;
};

//轮廓线闭合操作
void closeProfile(vector<Edge>& total, vector<Vector2d>& seqPs)
{
	int n = total.size();
	vector<bool> isVisited(n, false);
	int count = 0;
	count++;
	Vector2d base = total[0].b;
	isVisited[0] = true;
	seqPs.push_back(base);
	while (count < n)
	{
		double minDis = 100000;
		int minIndex = -1;
		for (int k = 0; k < n; k++)
		{

			if (isVisited[k] == false)
			{
				Edge e = total[k];
				Vector2d center = (e.a + e.b);
				center[0] /= 2.0;
				center[1] /= 2.0;
				Vector2d temp = base - center;
				double dis = sqrt(temp.dot(temp));
				if (dis < minDis)
				{
					minDis = dis;
					minIndex = k;
				}
			}
		}

		if (minIndex != -1)
		{
			Vector2d temp1 = base - total[minIndex].a;
			Vector2d temp2 = base - total[minIndex].b;

			double dis1 = sqrt(temp1.dot(temp1));
			double dis2 = sqrt(temp2.dot(temp2));
			if (dis1 < dis2)
			{
				seqPs.push_back(total[minIndex].a);
				base = total[minIndex].b;
			}
			else {
				seqPs.push_back(total[minIndex].b);
				base = total[minIndex].a;
			}
			seqPs.push_back(base);
			isVisited[minIndex] = true;
			count++;
		}
		else
		{
			//qDebug() << "没找到相距最小的边";
		}
	}
}

void alphaShapes(vector<Vector2d> m_points, vector<Edge> &m_edges, double m_radius)
{
	int n = m_points.size();
	for (int i = 0; i < n; i++)
	{
		for (int k = i + 1; k < n; k++)
		{
			//跳过距离大于直径的情况
			Vector2d temp = m_points[i] - m_points[k];
			double dis = sqrt(temp.dot(temp));
			if (dis > 2 * m_radius)
				continue;

			//两个圆心
			Vector2d c1, c2;

			//线段中点
			Vector2d center = 0.5*(m_points[i] + m_points[k]);

			// 方向向量 d = (x,y)
			Vector2d dir = m_points[i] - m_points[k];

			//垂直向量 n = (a,b)
			Vector2d normal;
			normal[1] = 5;    //随便给y赋一个值

			if (dir[0] != 0)
				normal[0] = -(normal[1]*dir[1]) / dir[0];
			else {     //如果方向平行于y轴
				normal[0] = 1;
				normal[1] = 0;
			}

			double nLen = sqrt(normal.dot(normal));
			normal[0] /= nLen;  //单位化
			normal[1] /= nLen;

			double dirLen2 = dir.dot(dir);
			double len = sqrt(m_radius*m_radius - (0.25*dirLen2));
			c1 = center + len*normal;
			c2 = center - len*normal;


			//b1,b2记录是否在圆C1,C2中找到其他点
			bool b1 = false, b2 = false;
			for (int m = 0; m < n; m++)
			{
				if (m == i || m == k)
					continue;

				Vector2d temp1 = m_points[m] - c1, temp2 = m_points[m] - c2;
				double dis2c1 = sqrt(temp1.dot(temp1));
				double dis2c2 = sqrt(temp2.dot(temp2));
				if (b1 != true && dis2c1 < m_radius)
					b1 = true;
				if (b2 != true && dis2c2 < m_radius)
					b2 = true;

				//如果都有内部点，不必再继续检查了
				if (b1 == true && b2 == true)
					break;
			}

			Edge edge;
			if ((b1 != true || b2 != true) && (m_points[i] != m_points[k]))
			{
				edge.a = m_points[i];
				edge.b = m_points[k];

				int j;
				for (j = 0; j < m_edges.size(); j++)
				{
					if (((m_edges[j].a == edge.a) && (m_edges[j].b == edge.b)) || ((m_edges[j].b == edge.a) && (m_edges[j].a == edge.b)))
						break;
				}
				if (j == m_edges.size())
					m_edges.push_back(edge);
			}
		}
	}
}

//获取点云在XOY平面上的多边形边界(AlphaShapes算法)
vector<Eigen::Vector2d> modeltool::GetPointsBoundary(const vector<Vector2d> &vertics, double m_radius)
{
	vector<Edge> allEdges;
	vector<Vector2d> seqPs;
	alphaShapes(vertics, allEdges, m_radius);
	closeProfile(allEdges, seqPs);

	return seqPs;
}

//获取点云在XOY平面上的多边形边界(AlphaShapes算法)
vector<Eigen::Vector3d> modeltool::GetPointsBoundary(const vector<Vector3d> &vertics, double m_radius)
{
	vector<Eigen::Vector2d> vertics_2d;
	int verticeNum = vertics.size();
	for (size_t i = 0; i < verticeNum; i++)
	{
		vertics_2d.push_back(Vector2d(vertics[i][0], vertics[i][1]));
	}
	vector<Eigen::Vector2d> boundary_2d = GetPointsBoundary(vertics_2d, m_radius);
	
	vector<Eigen::Vector3d> boundary_3d;
	int boundaryNum = boundary_2d.size();
	for (size_t i = 0; i < boundaryNum; i++)
	{
		boundary_3d.push_back(Vector3d(boundary_2d[i][0], boundary_2d[i][1], 0));
	}
	return boundary_3d;
}

//计算点pt到线段[p,q]的距离
double modeltool::DistancePoint2LineSeg(const Vector3d &pt, const Vector3d &p, const Vector3d &q)
{
	double pqx = q[0] - p[0];
	double pqy = q[1] - p[1];
	double pqz = q[2] - p[2];
	double dx = pt[0] - p[0];
	double dy = pt[1] - p[1];
	double dz = pt[2] - p[2];
	double d = pqx*pqx + pqy*pqy + pqz*pqz;	// qp线段长度的平方
	double t = pqx*dx + pqy*dy + pqz*dz;	// p pt向量 点积 pq 向量（p相当于A点，q相当于B点，pt相当于P点）
	if (d > 0)	// 除数不能为0; 如果为零 t应该也为零。下面计算结果仍然成立。                   
		t /= d;		// 此时t 相当于 上述推导中的 r。
	if (t < 0)
		t = 0;	// 当t（r）< 0时，最短距离即为 pt点 和 p点（A点和P点）之间的距离。
	else if (t > 1)
		t = 1;	// 当t（r）> 1时，最短距离即为 pt点 和 q点（B点和P点）之间的距离。
	
				// t = 0，计算 pt点 和 p点的距离; t = 1, 计算 pt点 和 q点 的距离; 否则计算 pt点 和 投影点 的距离。
	dx = p[0] + t*pqx - pt[0];
	dy = p[1] + t*pqy - pt[1];
	dz = p[2] + t*pqz - pt[2];
	double d2 = dx*dx + dy*dy + dz*dz;

	return sqrt(d2);
}

//筛选元素数目大于阈值threshold的向量
void modeltool::SelVecGreThanSize(vector<vector<int>> &vect, int threshold)
{
	int vectSize = vect.size();
	vector<vector<int>> resultVec;
	for (int i = 0; i < vectSize; i++)
	{
		if (vect[i].size() >= threshold)
		{
			resultVec.push_back(vect[i]);
		}
	}
	vect = resultVec;
}

//清空元素数目小于阈值threshold的向量
void modeltool::ClearVecGreThanSize(vector<vector<int>> &vect, int threshold)
{
	int vectSize = vect.size();
	for (int i = 0; i < vectSize; i++)
	{
		if (vect[i].size() <= threshold)
		{
			vect[i].clear();
		}
	}
}