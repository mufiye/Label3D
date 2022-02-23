#include "model.h"

CBoundingBox::CBoundingBox()
{

}

//��ʼ����Χ��
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

	//�����ɵ���Щ�������Ѱ����С��Χ����
	//rectPoint�����еõ��˾��ε��ĸ���������
	//std::cout << "��ʼ�����Χ�� points.size()�� " << points.size() << endl;
	RotatedRect rectPoint = minAreaRect(points);
	//std::cout << "�����Χ����� " << endl;
	//����һ���洢�����ĸ��������ı���
	Point2f fourPoint2f[4];
	//��rectPoint�����д洢������ֵ�ŵ� fourPoint��������
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

//��ʼ����Χ��
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

	//�����ɵ���Щ�������Ѱ����С��Χ����
	//rectPoint�����еõ��˾��ε��ĸ���������
	//std::cout << "��ʼ�����Χ�� points.size()�� " << points.size() << endl;
	RotatedRect rectPoint = minAreaRect(points);
	//std::cout << "�����Χ����� " << endl;
	//����һ���洢�����ĸ��������ı���
	Point2f fourPoint2f[4];
	//��rectPoint�����д洢������ֵ�ŵ� fourPoint��������
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

//�����Χ�з���
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

//�����Χ��λ��
void CBoundingBox::CalcPos()
{
	this->midMinMaxPoint = modeltool::GetMidMinMaxPointForVertices(this->box);
}

//��ȡ��άƽ��XOY�ϵİ�Χ��
vector<Vector2d> CBoundingBox::GetBoxXOY()
{
	vector<Eigen::Vector2d> polygon;
	for (size_t i = 0; i < 4; i++)
	{
		polygon.push_back(Vector2d(box[i][0], box[i][1]));
	}
	return polygon;
}

//�����Χ��
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

//���Ƕ���ת��Χ��
void CBoundingBox::RotateBoxAngle(double angle)
{
	double radian = angle * M_PI / 180.0;
	RotateBoxRadian(radian);
}

//��������ת��Χ��
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

		//����glmModel���㷨����,��ҪglmModel�ж��㷨����,glmVertexNormals(glmModel, 90.0);
		//vertices[mt.vindex[0]].normal = Vec3f(glmModel->normals[3 * glmModel->triangles[i].nindices[0]], glmModel->normals[3 * glmModel->triangles[i].nindices[0] + 1], glmModel->normals[3 * glmModel->triangles[i].nindices[0] + 2]);
		//vertices[mt.vindex[1]].normal = Vec3f(glmModel->normals[3 * glmModel->triangles[i].nindices[1]], glmModel->normals[3 * glmModel->triangles[i].nindices[1] + 1], glmModel->normals[3 * glmModel->triangles[i].nindices[1] + 2]);
		//vertices[mt.vindex[2]].normal = Vec3f(glmModel->normals[3 * glmModel->triangles[i].nindices[2]], glmModel->normals[3 * glmModel->triangles[i].nindices[2] + 1], glmModel->normals[3 * glmModel->triangles[i].nindices[2] + 2]);

		vertices[mt.vIndex[0]].trianglesInclude.push_back(i);
		vertices[mt.vIndex[1]].trianglesInclude.push_back(i);
		vertices[mt.vIndex[2]].trianglesInclude.push_back(i);
	}
}

//�����Χ��
bool CModel::CalcBoundingBox()
{
	return this->boundingBox.InitBox(this->vertices);
}

//��ȡģ�ͱ߽�nȦ��Ƭ������
vector<int> CModel::GetBoundaryFaceIndices() const
{
	/*
		�߽�������Ƭ���������ص㣺�����ΰ���������������������һ�����㴦�ڱ߽�;
		�߽綥��һ��λ�ڱ߽����;
		�߽��ֻ����һ��������
	*/

	//��һ����ɸѡ�߽��
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

	//�ڶ�����ɸѡ�߽綥��
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

	//������������������Ƭ�������߽綥����������Ϊ�߽���Ƭ
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

//��ȡģ�ͷָ�����ı߽�nȦ��Ƭ������
vector<int> CModel::GetBoundaryFaceIndices_SegRegion(const vector<int> &segment) const
{
	/*
	�߽�������Ƭ���������ص㣺�����ΰ���������������������һ�����㴦�ڱ߽�;
	�߽綥��һ��λ�ڱ߽����;
	�߽��ֻ����һ��������
	*/

	//��һ����ɸѡ�߽��
	int vCount = this->vertices.size();
	int fCount = this->triangles.size();
	vector<vector<int>> edges(vCount);
	for (size_t fi = 0; fi < fCount; fi++)
	{
		//�ų����ڷָ������ڵ���Ƭ�������
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

	//�ڶ�����ɸѡ�߽綥��
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

	//������������������Ƭ�������߽綥����������Ϊ�߽���Ƭ
	vector<int> ifBoundaryFace;
	for (size_t fi = 0; fi < fCount; fi++)
	{
		//�ų����ڷָ������ڵ���Ƭ�������
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

//����������Ƭ���ĵ�����
void MeshGraph::CalcFaceCenter()
{
	for (int i = 0; i < triangles.size(); i++)
	{
		triangles[i].midPos[0] = (vertices[triangles[i].vIndex[0]].pos[0] + vertices[triangles[i].vIndex[1]].pos[0] + vertices[triangles[i].vIndex[2]].pos[0]) / 3.0;
		triangles[i].midPos[1] = (vertices[triangles[i].vIndex[0]].pos[1] + vertices[triangles[i].vIndex[1]].pos[1] + vertices[triangles[i].vIndex[2]].pos[1]) / 3.0;
		triangles[i].midPos[2] = (vertices[triangles[i].vIndex[0]].pos[2] + vertices[triangles[i].vIndex[1]].pos[2] + vertices[triangles[i].vIndex[2]].pos[2]) / 3.0;
	}
}

//����������Ƭ��ֱˮƽ����
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

//����������Ƭ���
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

//��������ͼ,������ÿ��������ڽӶ���
void MeshGraph::CalcAdjVerticesPerVertex()
{
	adjVerticsPerVertex.resize(vertices.size());
	for (int i = 0; i < triangles.size(); i++)
	{
		STriangle &t = triangles[i];
		//����triangles��ʱ���face_index���붥���faceIncludeList�б�
		vertices[t.vIndex[0]].trianglesInclude.push_back(i);
		vertices[t.vIndex[1]].trianglesInclude.push_back(i);
		vertices[t.vIndex[2]].trianglesInclude.push_back(i);

		//���������ڽӹ�ϵͼ
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

//������Ƭͼ,������ÿ����Ƭ���ڽ���Ƭ
void MeshGraph::CalcAdjFacesPerFace()
{
	adjFacesPerFace.resize(triangles.size());
	for (int i = 0; i < vertices.size(); i++)
	{
		for (int j = 0; j < vertices[i].trianglesInclude.size(); j++)
		{
			int triIndex = vertices[i].trianglesInclude[j];
			vector<int> &flist = adjFacesPerFace[triIndex];
			//��trianglesInclude�б��г�triIndex�������tri��������flist
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

//������Ƭͼ,������ÿ����Ƭ���ڽ���Ƭ������ͬ��Ϊ����������
void MeshGraph::CalcAdjFacesPerFace_Edge()
{
	//adjFacesPerFace_Edge.resize(triangles.size());
	int faceCount = adjFacesPerFace.size();
	for (int i = 0; i < faceCount; i++)	//��������������Ƭ
	{
		vector<int> &adjFaces = adjFacesPerFace[i];	//�뵱ǰ������Ƭ�й������������������Ƭ�б�
		vector<int> adjFaces_e;
		//ɸѡȡ����������Ƭ�в����й�������,�����й����ߵ���Ƭ����
		int fCount = 0;
		for (int j = 0; j < adjFaces.size(); j++)
		{
			int sameVertexCount = 0;	//��ͬ������Ŀ
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

//����ͼ�ıߣ�һ����E[a, b]��ʾ����Ϊa, b��������Ƭ���й�����
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
	//std::cout << "ͼ�ı߼��Ϲ� " << graphEdgesSameEdge.size() << " ��" << endl;
}

//����GLMmodel����MeshGraph
void MeshGraph::CreateGraph(const GLMmodel* glmmodel, bool ifCalcFaceInfo, bool ifCalcAdjGraph)
{
	this->numVertices = glmmodel->numvertices;
	//��ʼ����������
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

	//��ʼ��������������
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
	//std::cout << "��ʼ������������ɣ�" << endl;
	float midx = midMinMaxPoint[0][0];
	float midy = midMinMaxPoint[0][1];

	float area = (midMinMaxPoint[2][0] - midMinMaxPoint[1][0])*(midMinMaxPoint[2][1] - midMinMaxPoint[1][1]);
	//std::cout << "ģ�������" << area << endl;

	//��ʼ��������Ƭ����
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
		//������Ƭ���ĵ�
		CalcFaceCenter();
		//������Ƭˮƽ��ֱ����
		CalcFaceVerHor();
		//������Ƭ���
		CalcFaceArea();
	}
	std::cout << "������Ƭ��Ϣ��ɣ�" << endl;
	//std::cout << "��ʼ��������Ƭ������ɣ�" << endl;

	//������Ƭ�ڽ�ͼ
	if (ifCalcAdjGraph)
	{
		CalcAdjVerticesPerVertex();
		std::cout << "���㶥���ڽ�ͼ��ɣ�" << endl;
		CalcAdjFacesPerFace();
		std::cout << "����������Ƭ�ڽ�ͼ���������㣩��ɣ�" << endl;
		CalcAdjFacesPerFace_Edge();
		std::cout << "����������Ƭ�ڽ�ͼ�������ߣ���ɣ�" << endl;
		CalcGraphEdges_e();
		std::cout << "����ͼ�ı߼��ϣ������ߣ���ɣ�" << endl;
	}
}

//����ÿ��������Ƭ��������Χ����εľ���
void MeshGraph::CalcDist2BuildRegion(const vector<Vector3d> &region)
{
	this->buildRegion = region;
	vector<Vector3d> region_loop(region);
	region_loop.push_back(region[0]);
	int regionLoop_size = region_loop.size();
	for (size_t i = 0; i < regionLoop_size; i++)
	{
		region_loop[i][2] = 0;	//ʹ��λ��XOYƽ����
	}

	int numTriangle = this->triangles.size();
	this->nearestLine.clear();
	for (size_t ti = 0; ti < numTriangle; ti++)
	{
		SNearLine nearLine;
		Vector3d &faceMidPos = this->triangles[ti].midPos;	//��Ƭ���ĵ�
		faceMidPos[2] = 0;	//ʹ��λ��XOYƽ����

		//�����ж����ĵ��Ƿ��ڶ�����ڣ�����ڣ��������Ϊ��������Ϊ��
		bool ifFaceInRegion = modeltool::IfPointInPolygon(faceMidPos, region);

		nearLine.distance = 9999999999;
		nearLine.normalAngle = 0;
		int lineIndex;	//��¼ȡ����С����ʱ���߶�����
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
				double w_t_normal = abs(faceNormal.dot(lineVec));	//�����н�cosֵ
				nearLine.normalAngle = w_t_normal;

			}
			else if (nearLine.distance == distance)	//˵����Ƭ���ĵ㵽�߶ε���̾���Ϊ���ĵ㵽�߶ζ˵�ľ���
			{
				Vector3d faceNormal = this->triangles[ti].normal;
				Vector3d lineVec = region_loop[ri - 1] - region_loop[ri];
				lineVec.normalize();
				double w_t_normal = abs(faceNormal.dot(lineVec));	//�����н�cosֵ

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

//��ȡÿ��������Ƭ��������Χ����εľ���
vector<SNearLine> MeshGraph::GetDist2BuildRegion(const vector<Vector3d> &region) const
{
	vector<SNearLine> nearLines;

	vector<Vector3d> region_loop(region);
	region_loop.push_back(region[0]);
	int regionLoop_size = region_loop.size();
	for (size_t i = 0; i < regionLoop_size; i++)
	{
		region_loop[i][2] = 0;	//ʹ��λ��XOYƽ����
	}

	int numTriangle = this->triangles.size();
	nearLines.clear();
	for (size_t ti = 0; ti < numTriangle; ti++)
	{
		SNearLine nearLine;
		Vector3d faceMidPos = this->triangles[ti].midPos;	//��Ƭ���ĵ�
		faceMidPos[2] = 0;	//ʹ��λ��XOYƽ����

		//�����ж����ĵ��Ƿ��ڶ�����ڣ�����ڣ��������Ϊ��������Ϊ��
		bool ifFaceInRegion = modeltool::IfPointInPolygon(faceMidPos, region);

		nearLine.distance = 9999999999;
		nearLine.normalAngle = 0;
		int lineIndex;	//��¼ȡ����С����ʱ���߶�����
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
				double w_t_normal = abs(faceNormal.dot(lineVec));	//�����н�cosֵ
				nearLine.normalAngle = w_t_normal;

			}
			else if (nearLine.distance == distance)	//˵����Ƭ���ĵ㵽�߶ε���̾���Ϊ���ĵ㵽�߶ζ˵�ľ���
			{
				Vector3d faceNormal = this->triangles[ti].normal;
				Vector3d lineVec = region_loop[ri - 1] - region_loop[ri];
				lineVec.normalize();
				double w_t_normal = abs(faceNormal.dot(lineVec));	//�����н�cosֵ

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

//���ݷָ������㡰��Ҫ����ͼ���ģ����Ƭ�����ݶ�����ε���̾���
vector<SNearLine> MeshGraph::GetDist2BuildRegion(const vector<Vector3d> &region, const vector<int>& roofSegResult) const
{
	vector<SNearLine> nearLines;
	vector<Vector3d> region_loop(region);
	region_loop.push_back(region[0]);
	int regionLoop_size = region_loop.size();
	for (size_t i = 0; i < regionLoop_size; i++)
	{
		region_loop[i][2] = 0;	//ʹ��λ��XOYƽ����
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

		Vector3d faceMidPos = this->triangles[ti].midPos;	//��Ƭ���ĵ�
		faceMidPos[2] = 0;	//ʹ��λ��XOYƽ����

		//�����ж����ĵ��Ƿ��ڶ�����ڣ�����ڣ��������Ϊ��������Ϊ��
		bool ifFaceInRegion = modeltool::IfPointInPolygon(faceMidPos, region);

		nearLine.distance = 9999999999;
		nearLine.normalAngle = 0;
		int lineIndex;	//��¼ȡ����С����ʱ���߶�����
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
				double w_t_normal = abs(faceNormal.dot(lineVec));	//�����н�cosֵ
				nearLine.normalAngle = w_t_normal;

			}
			else if (nearLine.distance == distance)	//˵����Ƭ���ĵ㵽�߶ε���̾���Ϊ���ĵ㵽�߶ζ˵�ľ���
			{
				Vector3d faceNormal = this->triangles[ti].normal;
				Vector3d lineVec = region_loop[ri - 1] - region_loop[ri];
				lineVec.normalize();
				double w_t_normal = abs(faceNormal.dot(lineVec));	//�����н�cosֵ

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

//���������ε��ݶ����ĵ��������ݶ���Χ���ཻ�߶�
void MeshGraph::CalcIntersLine2RoofCenter(const vector<Vector3d> &region)
{
	if (this->buildRegion.size() <= 0)
	{
		this->buildRegion = region;
	}

	Vector3d roofCenter(0, 0, 0);
}

//����ģ�ͷָ�����ȡǰ������ֱ�������η�Χregion�� roofSegResult��1��ʾǰ��
vector<Vector3d> MeshGraph::GetBuildRegionByRoof(const vector<int> &segResult) const
{
	int vSize = vertices.size();
	int fSize = triangles.size();
	vector<bool> ifForeGroundV(vSize);	//�Ƿ���ǰ������
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

	//����������ƽ���߳�
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

	vector<Vector3d> boundary = modeltool::GetPointsBoundary(segVertices, edgeLengthMax);//��Χ�����

	//ȥ�����ڵ��ظ��ĵ�
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

//���ݡ���Ƭ�������顱��ȡ��Χ��(OpenCV�㷨ȡ���ΰ�Χ��)
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

//���ݡ���Ƭ�������顱��ȡ��Χ��(OpenCV�㷨ȡ���ΰ�Χ��)
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

//���ݾ��ΰ�Χ���ȡ��Χ��Χ��������Ƭ��������(ֻȡ������Ƭ)
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

	int maxHeight = bBox_4[0][2];	//ȡ��ߵ�һ����Ƭ 

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

//���ݾ��ΰ�Χ���ȡ��Χ��Χ��������Ƭ��������(����ȡ����������)
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