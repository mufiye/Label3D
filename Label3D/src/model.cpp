#include "model.h"

CBoundingBox::CBoundingBox()
{

}

//初始化包围框
bool CBoundingBox::InitBox(const vector<Vector3d>& vertices)
{
	int verticesNum = vertices.size();
	vector<cv::Point2f> points;
	float minz = 999999999999;
	float maxz = -999999999999;
	for (size_t i = 0; i < verticesNum; i++)
	{
		cv::Point2f p;
		p.x = vertices[i][0];
		p.y = vertices[i][1];
		points.push_back(p);

		minz = vertices[i][2] < minz ? vertices[i][2] : minz;
		maxz = vertices[i][2]  > maxz ? vertices[i][2] : maxz;
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

	vector<Vector3d> boundingBox_3d;
	for (size_t i = 0; i < 4; i++)
	{
		boundingBox_3d.push_back(Vector3d(recPointVec[i].x, recPointVec[i].y, maxz));
	}
	for (size_t i = 0; i < 4; i++)
	{
		boundingBox_3d.push_back(Vector3d(recPointVec[i].x, recPointVec[i].y, minz));
	}

	setBox(boundingBox_3d);
	CalcDirs();
	CalcPos();

	return true;
}

//初始化包围框
bool CBoundingBox::InitBox(const vector<SVertex>& vertices)
{
	int verticesNum = vertices.size();
	vector<cv::Point2f> points;
	float minz = 999999999999;
	float maxz = -999999999999;
	for (size_t i = 0; i < verticesNum; i++)
	{
		cv::Point2f p;
		p.x = vertices[i].pos[0];
		p.y = vertices[i].pos[1];
		points.push_back(p);

		minz = vertices[i].pos[2] < minz ? vertices[i].pos[2] : minz;
		maxz = vertices[i].pos[2] > maxz ? vertices[i].pos[2] : maxz;
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

	vector<Vector3d> boundingBox_3d;
	for (size_t i = 0; i < 4; i++)
	{
		boundingBox_3d.push_back(Vector3d(recPointVec[i].x, recPointVec[i].y, maxz));
	}
	for (size_t i = 0; i < 4; i++)
	{
		boundingBox_3d.push_back(Vector3d(recPointVec[i].x, recPointVec[i].y, minz));
	}

	setBox(boundingBox_3d);
	CalcDirs();
	CalcPos();

	return true;
}

//计算包围盒方向
void CBoundingBox::CalcDirs()
{
	int pointSize = this->box.size();
	if (pointSize != 8)
	{
		return;
	}

	this->dirUD = Vector3d(0, 0, box[0][2] - box[4][2]);
	this->dirUD.normalize();

	this->dirLR = Vector3d(box[0][0] - box[1][0], box[0][1] - box[1][1], 0);
	this->dirLR.normalize();

	this->dirFB = Vector3d(box[1][0] - box[2][0], box[1][1] - box[2][1], 0);
	this->dirFB.normalize();
}

//计算包围框位置
void CBoundingBox::CalcPos()
{
	this->midMinMaxPoint = modeltool::GetMidMinMaxPointForVertices(this->box);
}

//获取二维平面XOY上的包围框
vector<Vector2d> CBoundingBox::GetBoxXOY()
{
	vector<Eigen::Vector2d> polygon;
	for (size_t i = 0; i < 4; i++)
	{
		polygon.push_back(Vector2d(box[i][0], box[i][1]));
	}
	return polygon;
}

//扩大包围框
void CBoundingBox::ExpandBox(BoxDir direction, float length)
{
	Vector3d dir(0, 0, 0);
	if (direction == BoxUp)
	{
		dir = dirUD;
		for (size_t i = 0; i < 4; i++)
		{
			box[i] += dir * length;
		}
	}
	else if (direction == BoxDown)
	{
		dir = dirUD;
		for (size_t i = 4; i < 7; i++)
		{
			box[i] += dir * length;
		}
	}
	else if (direction == BoxLeft)
	{
		dir = dirLR;
		box[0] += dir * length;
		box[3] += dir * length;
		box[4] += dir * length;
		box[7] += dir * length;
	}
	else if (direction == BoxRight)
	{
		dir = dirLR;
		box[1] += dir * length;
		box[2] += dir * length;
		box[5] += dir * length;
		box[6] += dir * length;
	}
	else if (direction == BoxFront)
	{
		dir = dirFB;
		box[2] += dir * length;
		box[3] += dir * length;
		box[6] += dir * length;
		box[7] += dir * length;
	}
	else if (direction == BoxBack)
	{
		dir = dirFB;
		box[0] += dir * length;
		box[1] += dir * length;
		box[4] += dir * length;
		box[5] += dir * length;
	}
	else if (direction == BoxOutEdge)
	{
		Vector2d minPos1((box[0][0] + box[1][0]) / 2.0, (box[0][1] + box[1][1]) / 2.0);
		Vector3d dir1(minPos1[0] - midMinMaxPoint[0][0], minPos1[1] - midMinMaxPoint[0][1], 0);
		dir1.normalize();
		box[0] += dir1*length;
		box[1] += dir1*length;
		box[2] -= dir1*length;
		box[3] -= dir1*length;
		box[4] += dir1*length;
		box[5] += dir1*length;
		box[6] -= dir1*length;
		box[7] -= dir1*length;

		Vector2d minPos2((box[1][0] + box[2][0]) / 2.0, (box[1][1] + box[2][1]) / 2.0);
		Vector3d dir2(minPos2[0] - midMinMaxPoint[0][0], minPos2[1] - midMinMaxPoint[0][1], 0);
		dir2.normalize();
		box[0] -= dir2*length;
		box[1] += dir2*length;
		box[2] += dir2*length;
		box[3] -= dir2*length;
		box[4] -= dir2*length;
		box[5] += dir2*length;
		box[6] += dir2*length;
		box[7] -= dir2*length;
	}
}

//按角度旋转包围框
void CBoundingBox::RotateBoxAngle(double angle)
{
	double radian = angle * M_PI / 180.0;
	RotateBoxRadian(radian);
}

//按弧度旋转包围框
void CBoundingBox::RotateBoxRadian(double radian)
{
	if (midMinMaxPoint.size() < 0)
	{
		CalcPos();
	}
	Vector2d midPos2d(midMinMaxPoint[0][0], midMinMaxPoint[0][1]);
	for (size_t i = 0; i < box.size(); i++)
	{
		box[i][0] = (box[i][0] - midPos2d[0])*cos(radian) - (box[i][1] - midPos2d[1])*sin(radian) + midPos2d[0];
		box[i][1] = (box[i][1] - midPos2d[1])*cos(radian) + (box[i][0] - midPos2d[0])*sin(radian) + midPos2d[1];
	}
}

CModel::CModel()
{
}

CModel::CModel(GLMmodel* glmModel)
{
	vector<Vector3d> verticsTemp;
	for (int i = 1; i <= glmModel->numvertices; i++)
	{
		SVertex mv;
		mv.pos[0] = glmModel->vertices[3 * i + 0];
		mv.pos[1] = glmModel->vertices[3 * i + 1];
		mv.pos[2] = glmModel->vertices[3 * i + 2];
		vertices.push_back(mv);
		verticsTemp.push_back(mv.pos);
	}
	this->midMinMaxPoint = modeltool::GetMidMinMaxPointForVertices(verticsTemp);

	for (int i = 0; i < glmModel->numtriangles; i++)
	{
		STriangle mt;

		mt.vIndex[0] = glmModel->triangles[i].vindices[0] - 1;
		mt.vIndex[1] = glmModel->triangles[i].vindices[1] - 1;
		mt.vIndex[2] = glmModel->triangles[i].vindices[2] - 1;
		mt.normal[0] = glmModel->facetnorms[3 * glmModel->triangles[i].findex + 0];
		mt.normal[1] = glmModel->facetnorms[3 * glmModel->triangles[i].findex + 1];
		mt.normal[2] = glmModel->facetnorms[3 * glmModel->triangles[i].findex + 2];
		triangles.push_back(mt);

		//复制glmModel顶点法向量,需要glmModel有顶点法向量,glmVertexNormals(glmModel, 90.0);
		//vertices[mt.vindex[0]].normal = Vec3f(glmModel->normals[3 * glmModel->triangles[i].nindices[0]], glmModel->normals[3 * glmModel->triangles[i].nindices[0] + 1], glmModel->normals[3 * glmModel->triangles[i].nindices[0] + 2]);
		//vertices[mt.vindex[1]].normal = Vec3f(glmModel->normals[3 * glmModel->triangles[i].nindices[1]], glmModel->normals[3 * glmModel->triangles[i].nindices[1] + 1], glmModel->normals[3 * glmModel->triangles[i].nindices[1] + 2]);
		//vertices[mt.vindex[2]].normal = Vec3f(glmModel->normals[3 * glmModel->triangles[i].nindices[2]], glmModel->normals[3 * glmModel->triangles[i].nindices[2] + 1], glmModel->normals[3 * glmModel->triangles[i].nindices[2] + 2]);

		vertices[mt.vIndex[0]].trianglesInclude.push_back(i);
		vertices[mt.vIndex[1]].trianglesInclude.push_back(i);
		vertices[mt.vIndex[2]].trianglesInclude.push_back(i);
	}
}

//计算包围框
bool CModel::CalcBoundingBox()
{
	return this->boundingBox.InitBox(this->vertices);
}

//获取模型边界n圈面片的索引
vector<int> CModel::GetBoundaryFaceIndices() const
{
	/*
		边界三角面片具有以下特点：三角形包含的三个顶点中至少有一个顶点处于边界;
		边界顶点一定位于边界边上;
		边界边只属于一个三角形
	*/

	//第一步，筛选边界边
	int vCount = this->vertices.size();
	int fCount = this->triangles.size();
	vector<vector<int>> edges(vCount);
	for (size_t fi = 0; fi < fCount; fi++)
	{
		int v0 = this->triangles[fi].vIndex[0];
		int v1 = this->triangles[fi].vIndex[1];
		int v2 = this->triangles[fi].vIndex[2];
		if (v0 > v1)
		{
			int temp = v1;
			v1 = v0;
			v0 = temp;
		}
		if (v0 > v2)
		{
			int temp = v2;
			v2 = v0;
			v0 = temp;
		}
		if (v1 > v2)
		{
			int temp = v2;
			v2 = v1;
			v1 = temp;
		}
		edges[v0].push_back(v1);
		edges[v0].push_back(v2);
		edges[v1].push_back(v2);
	}
	for (size_t i = 0; i < vCount; i++)
	{
		sort(edges[i].begin(), edges[i].end());
	}
	vector<Vector2i> boundaryEdges;
	for (size_t vi = 0; vi < vCount; vi++)
	{
		if (edges[vi].size() > 0)
		{
			for (size_t i = 0; i < edges[vi].size(); i++)
			{
				int cursor = i;
				bool ifSingle = true;
				for (size_t next = i + 1; next < edges[vi].size(); next++)
				{
					if (edges[vi][next] == edges[vi][i])
					{
						ifSingle = false;
						cursor = next;
					}
					else
					{
						break;
					}
				}
				if (ifSingle)
				{
					boundaryEdges.push_back(Vector2i(vi, edges[vi][i]));
				}
				i = cursor;
			}
		}
	}

	//第二步，筛选边界顶点
	vector<bool> ifBoundaryVertex(vCount);	
	for (size_t vi = 0; vi < vCount; vi++)
	{
		ifBoundaryVertex[vi] = false;
	}
	for (size_t i = 0; i < boundaryEdges.size(); i++)
	{
		ifBoundaryVertex[boundaryEdges[i][0]] = true;
		ifBoundaryVertex[boundaryEdges[i][1]] = true;
	}

	//第三步，遍历所有面片，包含边界顶点的三角面记为边界面片
	vector<int> ifBoundaryFace;
	for (size_t fi = 0; fi < fCount; fi++)
	{
		if (ifBoundaryVertex[this->triangles[fi].vIndex[0]] || ifBoundaryVertex[this->triangles[fi].vIndex[1]] || ifBoundaryVertex[this->triangles[fi].vIndex[2]])
		{
			ifBoundaryFace.push_back(fi);
		}
	}

	return ifBoundaryFace;
}

//获取模型分割区域的边界n圈面片的索引
vector<int> CModel::GetBoundaryFaceIndices_SegRegion(const vector<int> &segment) const
{
	/*
	边界三角面片具有以下特点：三角形包含的三个顶点中至少有一个顶点处于边界;
	边界顶点一定位于边界边上;
	边界边只属于一个三角形
	*/

	//第一步，筛选边界边
	int vCount = this->vertices.size();
	int fCount = this->triangles.size();
	vector<vector<int>> edges(vCount);
	for (size_t fi = 0; fi < fCount; fi++)
	{
		//排除不在分割区域内的面片参与计算
		if (segment[fi] < 0)
		{
			continue;
		}

		int v0 = this->triangles[fi].vIndex[0];
		int v1 = this->triangles[fi].vIndex[1];
		int v2 = this->triangles[fi].vIndex[2];
		if (v0 > v1)
		{
			int temp = v1;
			v1 = v0;
			v0 = temp;
		}
		if (v0 > v2)
		{
			int temp = v2;
			v2 = v0;
			v0 = temp;
		}
		if (v1 > v2)
		{
			int temp = v2;
			v2 = v1;
			v1 = temp;
		}
		edges[v0].push_back(v1);
		edges[v0].push_back(v2);
		edges[v1].push_back(v2);
	}
	for (size_t i = 0; i < vCount; i++)
	{
		sort(edges[i].begin(), edges[i].end());
	}
	vector<Vector2i> boundaryEdges;
	for (size_t vi = 0; vi < vCount; vi++)
	{
		if (edges[vi].size() > 0)
		{
			for (size_t i = 0; i < edges[vi].size(); i++)
			{
				int cursor = i;
				bool ifSingle = true;
				for (size_t next = i + 1; next < edges[vi].size(); next++)
				{
					if (edges[vi][next] == edges[vi][i])
					{
						ifSingle = false;
						cursor = next;
					}
					else
					{
						break;
					}
				}
				if (ifSingle)
				{
					boundaryEdges.push_back(Vector2i(vi, edges[vi][i]));
				}
				i = cursor;
			}
		}
	}

	//第二步，筛选边界顶点
	vector<bool> ifBoundaryVertex(vCount);
	for (size_t vi = 0; vi < vCount; vi++)
	{
		ifBoundaryVertex[vi] = false;
	}
	for (size_t i = 0; i < boundaryEdges.size(); i++)
	{
		ifBoundaryVertex[boundaryEdges[i][0]] = true;
		ifBoundaryVertex[boundaryEdges[i][1]] = true;
	}

	//第三步，遍历所有面片，包含边界顶点的三角面记为边界面片
	vector<int> ifBoundaryFace;
	for (size_t fi = 0; fi < fCount; fi++)
	{
		//排除不在分割区域内的面片参与计算
		if (segment[fi] < 0)
		{
			continue;
		}

		if (ifBoundaryVertex[this->triangles[fi].vIndex[0]] || ifBoundaryVertex[this->triangles[fi].vIndex[1]] || ifBoundaryVertex[this->triangles[fi].vIndex[2]])
		{
			ifBoundaryFace.push_back(fi);
		}
	}

	return ifBoundaryFace;
}

MeshGraph::MeshGraph()
{
	this->numTriangles = 0;
	this->numVertices = 0;
}

//计算三角面片中心点坐标
void MeshGraph::CalcFaceCenter()
{
	for (int i = 0; i < triangles.size(); i++)
	{
		triangles[i].midPos[0] = (vertices[triangles[i].vIndex[0]].pos[0] + vertices[triangles[i].vIndex[1]].pos[0] + vertices[triangles[i].vIndex[2]].pos[0]) / 3.0;
		triangles[i].midPos[1] = (vertices[triangles[i].vIndex[0]].pos[1] + vertices[triangles[i].vIndex[1]].pos[1] + vertices[triangles[i].vIndex[2]].pos[1]) / 3.0;
		triangles[i].midPos[2] = (vertices[triangles[i].vIndex[0]].pos[2] + vertices[triangles[i].vIndex[1]].pos[2] + vertices[triangles[i].vIndex[2]].pos[2]) / 3.0;
	}
}

//计算三角面片竖直水平性质
void MeshGraph::CalcFaceVerHor()
{
	int numtriangles = this->triangles.size();
	double threshold60 = abs(cos(60.f * M_PI / 180.0));
	double threshold45 = abs(cos(45.f * M_PI / 180.0));
	double threshold30 = abs(cos(30.f * M_PI / 180.0));
	Vector3d zAxis(0, 0, 1.f);
	for (int i = 0; i < numtriangles; i++)
	{
		Vector3d faceNormal(triangles[i].normal[0], triangles[i].normal[1], triangles[i].normal[2]);
		faceNormal.normalize();
		if (abs(zAxis.dot(faceNormal)) < threshold60)
			triangles[i].ifVertical60 = true;
		else
			triangles[i].ifVertical60 = false;

		if (abs(zAxis.dot(faceNormal)) < threshold45)
			triangles[i].ifVertical45 = true;
		else
			triangles[i].ifVertical45 = false;

		if (abs(zAxis.dot(faceNormal)) > threshold30)
			triangles[i].ifHorizontal = true;
		else
			triangles[i].ifHorizontal = false;
	}
}

//计算三角面片面积
void MeshGraph::CalcFaceArea()
{
	int numtriangles = this->triangles.size();
	for (int i = 0; i < numtriangles; i++)
	{
		Vector3d v1 = this->vertices[this->triangles[i].vIndex[0]].pos;
		Vector3d v2 = this->vertices[this->triangles[i].vIndex[1]].pos;
		Vector3d v3 = this->vertices[this->triangles[i].vIndex[2]].pos;
		this->triangles[i].area = modeltool::CalcTriArea(v1, v2, v3);
	}
}

//构建顶点图,即计算每个顶点的邻接顶点
void MeshGraph::CalcAdjVerticesPerVertex()
{
	adjVerticsPerVertex.resize(vertices.size());
	for (int i = 0; i < triangles.size(); i++)
	{
		STriangle &t = triangles[i];
		//遍历triangles的时候把face_index加入顶点的faceIncludeList列表
		vertices[t.vIndex[0]].trianglesInclude.push_back(i);
		vertices[t.vIndex[1]].trianglesInclude.push_back(i);
		vertices[t.vIndex[2]].trianglesInclude.push_back(i);

		//创建顶点邻接关系图
		vector<int> &p0list = adjVerticsPerVertex[t.vIndex[0]];
		vector<int> &p1list = adjVerticsPerVertex[t.vIndex[1]];
		vector<int> &p2list = adjVerticsPerVertex[t.vIndex[2]];
		if (std::find(p0list.begin(), p0list.end(), t.vIndex[1]) == p0list.end())
			p0list.push_back(t.vIndex[1]);
		if (std::find(p0list.begin(), p0list.end(), t.vIndex[2]) == p0list.end())
			p0list.push_back(t.vIndex[2]);
		if (std::find(p1list.begin(), p1list.end(), t.vIndex[0]) == p1list.end())
			p1list.push_back(t.vIndex[0]);
		if (std::find(p1list.begin(), p1list.end(), t.vIndex[2]) == p1list.end())
			p1list.push_back(t.vIndex[2]);
		if (std::find(p2list.begin(), p2list.end(), t.vIndex[0]) == p2list.end())
			p2list.push_back(t.vIndex[0]);
		if (std::find(p2list.begin(), p2list.end(), t.vIndex[1]) == p2list.end())
			p2list.push_back(t.vIndex[1]);
	}
}

//构建面片图,即计算每个面片的邻接面片
void MeshGraph::CalcAdjFacesPerFace()
{
	adjFacesPerFace.resize(triangles.size());
	for (int i = 0; i < vertices.size(); i++)
	{
		for (int j = 0; j < vertices[i].trianglesInclude.size(); j++)
		{
			int triIndex = vertices[i].trianglesInclude[j];
			vector<int> &flist = adjFacesPerFace[triIndex];
			//将trianglesInclude列表中除triIndex外的其他tri索引加入flist
			for (int k = 0; k < vertices[i].trianglesInclude.size(); k++)
			{
				if (vertices[i].trianglesInclude[k] != triIndex)
				{
					if (std::find(flist.begin(), flist.end(), vertices[i].trianglesInclude[k]) == flist.end())
						flist.push_back(vertices[i].trianglesInclude[k]);
				}
			}
		}
	}
}

//构建面片图,即计算每个面片的邻接面片（以相同边为相邻条件）
void MeshGraph::CalcAdjFacesPerFace_Edge()
{
	//adjFacesPerFace_Edge.resize(triangles.size());
	int faceCount = adjFacesPerFace.size();
	for (int i = 0; i < faceCount; i++)	//遍历所有三角面片
	{
		vector<int> &adjFaces = adjFacesPerFace[i];	//与当前三角面片有公共顶点的相邻三角面片列表
		vector<int> adjFaces_e;
		//筛选取所有相邻面片中不仅有公共顶点,而且有公共边的面片索引
		int fCount = 0;
		for (int j = 0; j < adjFaces.size(); j++)
		{
			int sameVertexCount = 0;	//相同顶点数目
			for (int k1 = 0; k1 < 3; k1++)
			{
				int f1_v = this->triangles[i].vIndex[k1];
				for (int k2 = 0; k2 < 3; k2++)
				{
					int f2_v = this->triangles[adjFaces[j]].vIndex[k2];
					if (f1_v == f2_v)
					{
						sameVertexCount++;
					}
				}
			}
			if (sameVertexCount >= 2)
			{
				adjFaces_e.push_back(adjFaces[j]);
				fCount++;
				if (fCount == 3)
				{
					break;
				}
			}
		}
		adjFacesPerFace_Edge.push_back(adjFaces_e);
	}
}

//构建图的边，一条边E[a, b]表示索引为a, b的两个面片具有公共边
void MeshGraph::CalcGraphEdges_e()
{
	int faceCount = adjFacesPerFace_Edge.size();
	for (size_t i = 0; i < faceCount; i++)
	{
		vector<int> &adjFaceIndics_e = adjFacesPerFace_Edge[i];
		for (size_t j = 0; j < adjFaceIndics_e.size(); j++)
		{
			if (adjFaceIndics_e[j] > i)
			{
				graphEdgesSameEdge.push_back(Vector2i(i, adjFaceIndics_e[j]));
			}
		}
	}
	//std::cout << "图的边集合共 " << graphEdgesSameEdge.size() << " 条" << endl;
}

//根据GLMmodel构建MeshGraph
void MeshGraph::CreateGraph(const GLMmodel* glmmodel, bool ifCalcFaceInfo, bool ifCalcAdjGraph)
{
	this->numVertices = glmmodel->numvertices;
	//初始化顶点数组
	vertices.resize(glmmodel->numvertices);
	vector<Vector3d> verticsTemp;
	for (int i = 1; i <= glmmodel->numvertices; i++)
	{
		SVertex &mv = vertices[i - 1];
		mv.pos[0] = glmmodel->vertices[3 * i + 0];
		mv.pos[1] = glmmodel->vertices[3 * i + 1];
		mv.pos[2] = glmmodel->vertices[3 * i + 2];
		verticsTemp.push_back(mv.pos);

		//minx = minx <= glmmodel->vertices[3 * i + 0] ? minx : glmmodel->vertices[3 * i + 0];
		//maxx = maxx >= glmmodel->vertices[3 * i + 0] ? maxx : glmmodel->vertices[3 * i + 0];
		//miny = miny <= glmmodel->vertices[3 * i + 1] ? miny : glmmodel->vertices[3 * i + 1];
		//maxy = maxy >= glmmodel->vertices[3 * i + 1] ? maxy : glmmodel->vertices[3 * i + 1];
	}

	//初始化顶点纹理坐标
	this->numTextures = glmmodel->numtextures;
	if (glmmodel->numtextures > 0)
	{
		this->numtexcoords = glmmodel->numtexcoords;
		this->texcoords.resize(numtexcoords);
		for (int i = 1; i <= numtexcoords; i++)
		{
			this->texcoords[i - 1] = Vector2d(glmmodel->texcoords[2 * i + 0], glmmodel->texcoords[2 * i + 1]);
		}
	}

	this->midMinMaxPoint = modeltool::GetMidMinMaxPointForVertices(verticsTemp);
	//std::cout << "初始化顶点数组完成！" << endl;
	float midx = midMinMaxPoint[0][0];
	float midy = midMinMaxPoint[0][1];

	float area = (midMinMaxPoint[2][0] - midMinMaxPoint[1][0])*(midMinMaxPoint[2][1] - midMinMaxPoint[1][1]);
	//std::cout << "模型面积：" << area << endl;

	//初始化三角面片数组
	this->numTriangles = glmmodel->numtriangles;
	triangles.resize(glmmodel->numtriangles);
	for (int i = 0; i < glmmodel->numtriangles; i++)
	{
		STriangle& mt = triangles[i];

		mt.vIndex[0] = glmmodel->triangles[i].vindices[0] - 1;
		mt.vIndex[1] = glmmodel->triangles[i].vindices[1] - 1;
		mt.vIndex[2] = glmmodel->triangles[i].vindices[2] - 1;
		mt.normal[0] = glmmodel->facetnorms[3 * glmmodel->triangles[i].findex + 0];
		mt.normal[1] = glmmodel->facetnorms[3 * glmmodel->triangles[i].findex + 1];
		mt.normal[2] = glmmodel->facetnorms[3 * glmmodel->triangles[i].findex + 2];
		mt.tIndex[0] = glmmodel->triangles[i].tindices[0] - 1;
		mt.tIndex[1] = glmmodel->triangles[i].tindices[1] - 1;
		mt.tIndex[2] = glmmodel->triangles[i].tindices[2] - 1;
		mt.textureId = glmmodel->materials[glmmodel->triangles[i].material].map_diffuse;
	}

	if (ifCalcFaceInfo)
	{
		//计算面片中心点
		CalcFaceCenter();
		//计算面片水平竖直性质
		CalcFaceVerHor();
		//计算面片面积
		CalcFaceArea();
	}
	std::cout << "计算面片信息完成！" << endl;
	//std::cout << "初始化三角面片数组完成！" << endl;

	//计算面片邻接图
	if (ifCalcAdjGraph)
	{
		CalcAdjVerticesPerVertex();
		std::cout << "计算顶点邻接图完成！" << endl;
		CalcAdjFacesPerFace();
		std::cout << "计算三角面片邻接图（公共顶点）完成！" << endl;
		CalcAdjFacesPerFace_Edge();
		std::cout << "计算三角面片邻接图（公共边）完成！" << endl;
		CalcGraphEdges_e();
		std::cout << "计算图的边集合（公共边）完成！" << endl;
	}
}

//计算每个三角面片到建筑范围多边形的距离
void MeshGraph::CalcDist2BuildRegion(const vector<Vector3d> &region)
{
	this->buildRegion = region;
	vector<Vector3d> region_loop(region);
	region_loop.push_back(region[0]);
	int regionLoop_size = region_loop.size();
	for (size_t i = 0; i < regionLoop_size; i++)
	{
		region_loop[i][2] = 0;	//使点位于XOY平面内
	}

	int numTriangle = this->triangles.size();
	this->nearestLine.clear();
	for (size_t ti = 0; ti < numTriangle; ti++)
	{
		SNearLine nearLine;
		Vector3d &faceMidPos = this->triangles[ti].midPos;	//面片中心点
		faceMidPos[2] = 0;	//使点位于XOY平面内

		//首先判断中心点是否在多边形内，如果在，则另距离为负；否则为正
		bool ifFaceInRegion = modeltool::IfPointInPolygon(faceMidPos, region);

		nearLine.distance = 9999999999;
		nearLine.normalAngle = 0;
		int lineIndex;	//记录取到最小距离时的线段索引
		for (size_t ri = 1; ri < regionLoop_size; ri++)
		{
			double distance = modeltool::DistancePoint2LineSeg(faceMidPos, region_loop[ri-1], region_loop[ri]);
			if (nearLine.distance > distance)
			{
				nearLine.distance = distance;
				nearLine.lineIndex = ri - 1;
				 
				Vector3d faceNormal = this->triangles[ti].normal;
				Vector3d lineVec = region_loop[ri - 1] - region_loop[ri];
				lineVec.normalize();
				double w_t_normal = abs(faceNormal.dot(lineVec));	//向量夹角cos值
				nearLine.normalAngle = w_t_normal;

			}
			else if (nearLine.distance == distance)	//说明面片中心点到线段的最短距离为中心点到线段端点的距离
			{
				Vector3d faceNormal = this->triangles[ti].normal;
				Vector3d lineVec = region_loop[ri - 1] - region_loop[ri];
				lineVec.normalize();
				double w_t_normal = abs(faceNormal.dot(lineVec));	//向量夹角cos值

				if (w_t_normal > nearLine.normalAngle)
				{
					nearLine.lineIndex = ri - 1;
					nearLine.normalAngle = w_t_normal;
				}
			}
		}
		nearLine.distance = ifFaceInRegion ? -nearLine.distance : nearLine.distance;
		this->nearestLine.push_back(nearLine);
	}
}

//获取每个三角面片到建筑范围多边形的距离
vector<SNearLine> MeshGraph::GetDist2BuildRegion(const vector<Vector3d> &region) const
{
	vector<SNearLine> nearLines;

	vector<Vector3d> region_loop(region);
	region_loop.push_back(region[0]);
	int regionLoop_size = region_loop.size();
	for (size_t i = 0; i < regionLoop_size; i++)
	{
		region_loop[i][2] = 0;	//使点位于XOY平面内
	}

	int numTriangle = this->triangles.size();
	nearLines.clear();
	for (size_t ti = 0; ti < numTriangle; ti++)
	{
		SNearLine nearLine;
		Vector3d faceMidPos = this->triangles[ti].midPos;	//面片中心点
		faceMidPos[2] = 0;	//使点位于XOY平面内

		//首先判断中心点是否在多边形内，如果在，则另距离为负；否则为正
		bool ifFaceInRegion = modeltool::IfPointInPolygon(faceMidPos, region);

		nearLine.distance = 9999999999;
		nearLine.normalAngle = 0;
		int lineIndex;	//记录取到最小距离时的线段索引
		for (size_t ri = 1; ri < regionLoop_size; ri++)
		{
			double distance = modeltool::DistancePoint2LineSeg(faceMidPos, region_loop[ri - 1], region_loop[ri]);
			if (nearLine.distance > distance)
			{
				nearLine.distance = distance;
				nearLine.lineIndex = ri - 1;

				Vector3d faceNormal = this->triangles[ti].normal;
				Vector3d lineVec = region_loop[ri - 1] - region_loop[ri];
				lineVec.normalize();
				double w_t_normal = abs(faceNormal.dot(lineVec));	//向量夹角cos值
				nearLine.normalAngle = w_t_normal;

			}
			else if (nearLine.distance == distance)	//说明面片中心点到线段的最短距离为中心点到线段端点的距离
			{
				Vector3d faceNormal = this->triangles[ti].normal;
				Vector3d lineVec = region_loop[ri - 1] - region_loop[ri];
				lineVec.normalize();
				double w_t_normal = abs(faceNormal.dot(lineVec));	//向量夹角cos值

				if (w_t_normal > nearLine.normalAngle)
				{
					nearLine.lineIndex = ri - 1;
					nearLine.normalAngle = w_t_normal;
				}
			}
		}
		nearLine.distance = ifFaceInRegion ? -nearLine.distance : nearLine.distance;
		nearLines.push_back(nearLine);
	}
	return nearLines;
}

//根据分割结果计算“需要参与图割的模型面片”到屋顶多边形的最短距离
vector<SNearLine> MeshGraph::GetDist2BuildRegion(const vector<Vector3d> &region, const vector<int>& roofSegResult) const
{
	vector<SNearLine> nearLines;
	vector<Vector3d> region_loop(region);
	region_loop.push_back(region[0]);
	int regionLoop_size = region_loop.size();
	for (size_t i = 0; i < regionLoop_size; i++)
	{
		region_loop[i][2] = 0;	//使点位于XOY平面内
	}

	int numTriangle = this->triangles.size();
	nearLines.clear();
	for (size_t ti = 0; ti < numTriangle; ti++)
	{
		SNearLine nearLine;
		if (roofSegResult[ti] == -1)
		{
			nearLine.lineIndex = -1;
			nearLine.distance = 999999999;
			nearLines.push_back(nearLine);
			continue;
		}

		Vector3d faceMidPos = this->triangles[ti].midPos;	//面片中心点
		faceMidPos[2] = 0;	//使点位于XOY平面内

		//首先判断中心点是否在多边形内，如果在，则另距离为负；否则为正
		bool ifFaceInRegion = modeltool::IfPointInPolygon(faceMidPos, region);

		nearLine.distance = 9999999999;
		nearLine.normalAngle = 0;
		int lineIndex;	//记录取到最小距离时的线段索引
		for (size_t ri = 1; ri < regionLoop_size; ri++)
		{
			double distance = modeltool::DistancePoint2LineSeg(faceMidPos, region_loop[ri - 1], region_loop[ri]);
			if (nearLine.distance > distance)
			{
				nearLine.distance = distance;
				nearLine.lineIndex = ri - 1;

				Vector3d faceNormal = this->triangles[ti].normal;
				Vector3d lineVec = region_loop[ri - 1] - region_loop[ri];
				lineVec.normalize();
				double w_t_normal = abs(faceNormal.dot(lineVec));	//向量夹角cos值
				nearLine.normalAngle = w_t_normal;

			}
			else if (nearLine.distance == distance)	//说明面片中心点到线段的最短距离为中心点到线段端点的距离
			{
				Vector3d faceNormal = this->triangles[ti].normal;
				Vector3d lineVec = region_loop[ri - 1] - region_loop[ri];
				lineVec.normalize();
				double w_t_normal = abs(faceNormal.dot(lineVec));	//向量夹角cos值

				if (w_t_normal > nearLine.normalAngle)
				{
					nearLine.lineIndex = ri - 1;
					nearLine.normalAngle = w_t_normal;
				}
			}
		}
		nearLine.distance = ifFaceInRegion ? -nearLine.distance : nearLine.distance;
		nearLines.push_back(nearLine);
	}
	return nearLines;
}

//计算三角形到屋顶中心点连线与屋顶范围的相交线段
void MeshGraph::CalcIntersLine2RoofCenter(const vector<Vector3d> &region)
{
	if (this->buildRegion.size() <= 0)
	{
		this->buildRegion = region;
	}

	Vector3d roofCenter(0, 0, 0);
}

//根据模型分割结果获取前景的竖直柱体多边形范围region： roofSegResult中1表示前景
vector<Vector3d> MeshGraph::GetBuildRegionByRoof(const vector<int> &segResult) const
{
	int vSize = vertices.size();
	int fSize = triangles.size();
	vector<bool> ifForeGroundV(vSize);	//是否是前景顶点
	for (size_t vi = 0; vi < vSize; vi++)
	{
		ifForeGroundV[vi] = false;
	}
	vector<Vector3d> segVertices;
	for (size_t fi = 0; fi < fSize; fi++)
	{
		if (segResult[fi] == 1)
		{
			ifForeGroundV[triangles[fi].vIndex[0]] = true;
			ifForeGroundV[triangles[fi].vIndex[1]] = true;
			ifForeGroundV[triangles[fi].vIndex[2]] = true;
		}
	}
	for (int vi = 0; vi < vSize; vi++)
	{
		if (ifForeGroundV[vi])
		{
			Vector3d v = vertices[vi].pos;
			segVertices.push_back(v);
		}
	}

	//计算三角形平均边长
	double edgeLengthCount = 0;
	double edgeLengthAvg = 0;
	double edgeLengthMax = 0;
	int edgeNum = 0;
	for (size_t fi = 0; fi < fSize; fi++)
	{
		if (segResult[fi] == 1)
		{
			Vector3d v0 = vertices[triangles[fi].vIndex[0]].pos;
			Vector3d v1 = vertices[triangles[fi].vIndex[1]].pos;
			Vector3d v2 = vertices[triangles[fi].vIndex[2]].pos;

			double l0 = sqrt(pow(v0[0] - v1[0], 2) + pow(v0[1] - v1[1], 2));
			double l1 = sqrt(pow(v0[0] - v2[0], 2) + pow(v0[1] - v2[1], 2));
			double l2 = sqrt(pow(v1[0] - v2[0], 2) + pow(v1[1] - v2[1], 2));

			edgeLengthMax = edgeLengthMax > l0 ? edgeLengthMax : l0;
			edgeLengthMax = edgeLengthMax > l1 ? edgeLengthMax : l1;
			edgeLengthMax = edgeLengthMax > l2 ? edgeLengthMax : l2;

			edgeLengthCount += l0;
			edgeLengthCount += l1;
			edgeLengthCount += l2;
			edgeNum += 3;
		}
	}
	edgeLengthMax = edgeLengthMax / 2.0;
	edgeLengthAvg = edgeLengthCount / edgeNum / 2.0;

	vector<Vector3d> boundary = modeltool::GetPointsBoundary(segVertices, edgeLengthMax);//包围多边形

	//去除相邻的重复的点
	int boundaryPointNum = boundary.size();
	vector<Vector3d> boundary_Distinct;
	if (boundaryPointNum > 0)
	{
		Vector3d pFront = boundary[0];
		boundary_Distinct.push_back(pFront);
		for (size_t i = 1; i < boundaryPointNum; i++)
		{
			if (boundary[i] == pFront)
			{
				continue;
			}
			else
			{
				pFront = boundary[i];
				boundary_Distinct.push_back(boundary[i]);
			}
		}
	}
	return boundary_Distinct;
}

//根据“面片索引数组”获取包围框(OpenCV算法取矩形包围框)
vector<Vector3d> MeshGraph::GetBBoxByFIndices(const vector<int> &faceIndices) const
{
	int vSize = this->vertices.size();
	int fSize = this->triangles.size();
	int insFSize = faceIndices.size();

	vector<bool> ifVInFaces(vSize);
	for (size_t vi = 0; vi < vSize; vi++)
	{
		ifVInFaces[vi] = false;
	}
	for (size_t fi = 0; fi < insFSize; fi++)
	{
		ifVInFaces[triangles[faceIndices[fi]].vIndex[0]] = true;
		ifVInFaces[triangles[faceIndices[fi]].vIndex[1]] = true;
		ifVInFaces[triangles[faceIndices[fi]].vIndex[2]] = true;
	}

	vector<Vector3d> vInFaces;
	for (size_t vi = 0; vi < vSize; vi++)
	{
		if (ifVInFaces[vi])
		{
			vInFaces.push_back(vertices[vi].pos);
		}
	}

	vector<Vector3d> boundingBox_f = modeltool::GetBoundingBox(vInFaces);
	return boundingBox_f;
}

//根据“面片索引数组”获取包围框(OpenCV算法取矩形包围框)
CBoundingBox MeshGraph::GetCBBoxByFIndices(const vector<int> &faceIndices) const
{
	int vSize = this->vertices.size();
	int fSize = this->triangles.size();
	int insFSize = faceIndices.size();

	vector<bool> ifVInFaces(vSize);
	for (size_t vi = 0; vi < vSize; vi++)
	{
		ifVInFaces[vi] = false;
	}
	for (size_t fi = 0; fi < insFSize; fi++)
	{
		ifVInFaces[triangles[faceIndices[fi]].vIndex[0]] = true;
		ifVInFaces[triangles[faceIndices[fi]].vIndex[1]] = true;
		ifVInFaces[triangles[faceIndices[fi]].vIndex[2]] = true;
	}

	vector<Vector3d> vInFaces;
	for (size_t vi = 0; vi < vSize; vi++)
	{
		if (ifVInFaces[vi])
		{
			vInFaces.push_back(vertices[vi].pos);
		}
	}
	CBoundingBox cbBox;
	cbBox.InitBox(vInFaces);
	return cbBox;
}

//根据矩形包围框获取包围框范围内所有面片索引数组(只取背景面片)
vector<int> MeshGraph::GetFIndicesInBBox(const CBoundingBox bBox, const vector<int> &segResult, int buildIndex) const
{
	vector<int> fIndicesInBBox;

	vector<Vector3d> bBox_4;
	if (bBox.box.size() < 4)
	{
		return fIndicesInBBox;
	}
	for (size_t i = 0; i < 4; i++)
	{
		bBox_4.push_back(bBox.box[i]);
	}

	int maxHeight = bBox_4[0][2];	//取最高点一下面片 

	int fSize = triangles.size();
	for (size_t fi = 0; fi < fSize; fi++)
	{
		if (segResult[fi] != -1 && segResult[fi] != buildIndex)
		{
			continue;
		}
		const Vector3d v0 = vertices[triangles[fi].vIndex[0]].pos;
		const Vector3d v1 = vertices[triangles[fi].vIndex[1]].pos;
		const Vector3d v2 = vertices[triangles[fi].vIndex[2]].pos;

		if (modeltool::IfPointInPolygon(v0, bBox_4)
			&& modeltool::IfPointInPolygon(v1, bBox_4)
			&& modeltool::IfPointInPolygon(v2, bBox_4)
			&& triangles[fi].midPos[2] <= maxHeight + 1)
		{
			fIndicesInBBox.push_back(fi);
		}
	}
	return fIndicesInBBox;
}

//根据矩形包围框获取包围框范围内所有面片索引数组(可以取到其他建筑)
vector<int> MeshGraph::GetFIndicesInBBox(const CBoundingBox bBox) const
{
	vector<int> fIndicesInBBox;

	vector<Vector3d> bBox_4;
	if (bBox.box.size() < 4)
	{
		return fIndicesInBBox;
	}
	for (size_t i = 0; i < 4; i++)
	{
		bBox_4.push_back(bBox.box[i]);
	}

	int fSize = triangles.size();
	for (size_t fi = 0; fi < fSize; fi++)
	{
		const Vector3d v0 = vertices[triangles[fi].vIndex[0]].pos;
		const Vector3d v1 = vertices[triangles[fi].vIndex[1]].pos;
		const Vector3d v2 = vertices[triangles[fi].vIndex[2]].pos;

		if (modeltool::IfPointInPolygon(v0, bBox_4)
			&& modeltool::IfPointInPolygon(v1, bBox_4)
			&& modeltool::IfPointInPolygon(v2, bBox_4))
		{
			fIndicesInBBox.push_back(fi);
		}
	}
	return fIndicesInBBox;
}