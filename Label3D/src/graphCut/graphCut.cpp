#include "graphCut.h"

/*
��ʼ��������Ƭ
"Ground": ȡ��͵�����1.5�׷�Χ����ˮƽ����Ƭ��Ϊ����
"RoofHeight": ȡ�����ݶ���ߵ�roofHeight����Ƭ��Ϊ����
"BoundingBox": ����Χ��Χȡ������Ƭ
"Boundary": ȡ�߽�һȦ��Ƭ��Ϊ����
"CellRegion": ����������ݶ�����η�Χ��ȡ��Χ����Ƭ��Ϊ��������δ�޸ĺã�
*/
vector<int> graphCut::GetInitBackFace_I(const string initType,const MeshGraph *meshGraph, const vector<int> &originalSegResult, double roofHeight)
{
	vector<int> initNodes(originalSegResult);
	assert(initNodes.size() == meshGraph->triangles.size());
	int fSize = meshGraph->triangles.size();
	int vSize = meshGraph->vertices.size();
	if (initType == "Ground")
	{
		vector<bool> ifVInRegion(vSize);
		for (size_t vi = 0; vi < vSize; vi++)
		{
			ifVInRegion[vi] = false;
		}
		for (size_t fi = 0; fi < fSize; fi++)
		{
			if (originalSegResult[fi] >= 0)
			{
				ifVInRegion[meshGraph->triangles[fi].vIndex[0]] = true;
				ifVInRegion[meshGraph->triangles[fi].vIndex[1]] = true;
				ifVInRegion[meshGraph->triangles[fi].vIndex[2]] = true;
			}
		}
		vector<Vector3d> insBoxVertices;	//��ǰ���ָ����Χ��Χ�����ж���
		for (size_t vi = 0; vi < vSize; vi++)
		{
			if (ifVInRegion[vi])
			{
				insBoxVertices.push_back(meshGraph->vertices[vi].pos);
			}
		}
		vector<Vector3d> miMinMax = modeltool::GetMidMinMaxPointForVertices(insBoxVertices);
		double midZ = miMinMax[0][2]; //�������ĵ�
		double minZ = miMinMax[1][2]; //������͵�
		double maxZ = miMinMax[2][2]; //������ߵ�
		double modelHeight = maxZ - minZ;

		for (size_t fi = 0; fi < fSize; fi++)
		{
			//ѡ��ˮƽ�Ҹ߶ȵ͵���Ƭ��Ϊ���棬��������Ƭ
			if (meshGraph->triangles[fi].ifHorizontal && meshGraph->triangles[fi].midPos[2] < minZ + 1.5 && (initNodes[fi] == 0))
			{
				initNodes[fi] = 2;
			}
		}
		return initNodes;
	}
	else if (initType == "RoofHeight")
	{
		for (size_t fi = 0; fi < fSize; fi++)
		{
			if (originalSegResult[fi] == 0)
			{
				double h1 = meshGraph->vertices[meshGraph->triangles[fi].vIndex[0]].pos[2];
				double h2 = meshGraph->vertices[meshGraph->triangles[fi].vIndex[1]].pos[2];
				double h3 = meshGraph->vertices[meshGraph->triangles[fi].vIndex[2]].pos[2];
				//ѡ��ˮƽ�Ҹ߶ȵ͵���Ƭ��Ϊ���棬��������Ƭ
				if (h1 > roofHeight || h2 > roofHeight || h3 > roofHeight)
				{
					initNodes[fi] = 2;
				}
			}
		}
		return initNodes;
	}
	else if (initType == "BoundingBox")
	{
		float expandSize = 1;
		vector<bool> ifRoofVertex(vSize);
		vector<Vector3d> roofVertices;
		for (size_t vi = 0; vi < vSize; vi++)
		{
			ifRoofVertex[vi] = false;
		}
		for (size_t fi = 0; fi < fSize; fi++)
		{
			if (initNodes[fi] == 1)	//������ݶ�
			{
				ifRoofVertex[meshGraph->triangles[fi].vIndex[0]] = true;
				ifRoofVertex[meshGraph->triangles[fi].vIndex[1]] = true;
				ifRoofVertex[meshGraph->triangles[fi].vIndex[2]] = true;
			}
		}
		for (size_t vi = 0; vi < vSize; vi++)
		{
			if (ifRoofVertex[vi])
			{
				roofVertices.push_back(meshGraph->vertices[vi].pos);
			}
		}

		CBoundingBox boundingBox;
		boundingBox.InitBox(roofVertices);
		boundingBox.ExpandBox(BoxOutEdge, expandSize);	//�����Χ��
		vector<Eigen::Vector2d> polygon = boundingBox.GetBoxXOY();
		for (size_t fi = 0; fi < fSize; fi++)
		{
			if (initNodes[fi] == -1)
			{
				continue;
			}
			//��Χ���������Ƭ��Ϊ������Ƭ
			Vector2d p0(meshGraph->vertices[meshGraph->triangles[fi].vIndex[0]].pos[0], meshGraph->vertices[meshGraph->triangles[fi].vIndex[0]].pos[1]);
			Vector2d p1(meshGraph->vertices[meshGraph->triangles[fi].vIndex[1]].pos[0], meshGraph->vertices[meshGraph->triangles[fi].vIndex[1]].pos[1]);
			Vector2d p2(meshGraph->vertices[meshGraph->triangles[fi].vIndex[2]].pos[0], meshGraph->vertices[meshGraph->triangles[fi].vIndex[2]].pos[1]);
			if (modeltool::IfPointInPolygon(p0, polygon) && modeltool::IfPointInPolygon(p1, polygon) && modeltool::IfPointInPolygon(p2, polygon))
			{

			}
			else
			{
				initNodes[fi] = 2;
			}
		}
		return initNodes;
	}
	else if (initType == "Boundary")
	{
		vector<int> boundary = meshGraph->GetBoundaryFaceIndices_SegRegion(initNodes);
		for (size_t i = 0; i < boundary.size(); i++)
		{
			initNodes[boundary[i]] = 2;
		}
		return initNodes;
	}
	else if (initType == "CellRegion")	//��δ�޸ĺ�
	{
		double minX = meshGraph->midMinMaxPoint[1][0];
		double maxX = meshGraph->midMinMaxPoint[2][0];
		double minY = meshGraph->midMinMaxPoint[1][1];
		double maxY = meshGraph->midMinMaxPoint[2][1];
		int widthX = ceil(maxX - minX);
		int lengthY = ceil(maxY - minY);
		bool** cellInRoofRegion = new bool*[widthX];	//��¼�˷����Ƿ������ݶ���Χ
		for (size_t xi = 0; xi < widthX; xi++)
		{
			cellInRoofRegion[xi] = new bool[lengthY];
		}
		for (size_t xi = 0; xi < widthX; xi++)
		{
			for (size_t yi = 0; yi < lengthY; yi++)
			{
				cellInRoofRegion[xi][yi] = false;
			}
		}
		//���������ݶ���Ƭ������cellInRoofRegion
		for (size_t fi = 0; fi < fSize; fi++)
		{
			if (originalSegResult[fi] == 0)
			{
				//��Χ���������Ƭ��Ϊ������Ƭ
				Vector3d p0(meshGraph->vertices[meshGraph->triangles[fi].vIndex[0]].pos[0], meshGraph->vertices[meshGraph->triangles[fi].vIndex[0]].pos[1], 0);
				Vector3d p1(meshGraph->vertices[meshGraph->triangles[fi].vIndex[1]].pos[0], meshGraph->vertices[meshGraph->triangles[fi].vIndex[1]].pos[1], 0);
				Vector3d p2(meshGraph->vertices[meshGraph->triangles[fi].vIndex[2]].pos[0], meshGraph->vertices[meshGraph->triangles[fi].vIndex[2]].pos[1], 0);
				vector<Vector3d> vertices;
				vertices.push_back(p0);
				vertices.push_back(p1);
				vertices.push_back(p2);
				vector<Vector3d> midMinMaxPos = modeltool::GetMidMinMaxPointForVertices(vertices);
				cellInRoofRegion[(int)floor(midMinMaxPos[1][0] - minX)][(int)floor(midMinMaxPos[1][1] - minY)] = true;	//[minX, minY]
				cellInRoofRegion[(int)floor(midMinMaxPos[1][0] - minX)][(int)floor(midMinMaxPos[2][1] - minY)] = true;	//[minX, maxY]
				cellInRoofRegion[(int)floor(midMinMaxPos[2][0] - minX)][(int)floor(midMinMaxPos[1][1] - minY)] = true;	//[maxX, minY]
				cellInRoofRegion[(int)floor(midMinMaxPos[2][0] - minX)][(int)floor(midMinMaxPos[2][1] - minY)] = true;	//[maxX, maxY]
			}
		}

		//�������ݶ�������Cell��������һȦ
		bool** cellInRoofRegion_expend = new bool*[widthX];	//��¼�˷����Ƿ������ݶ���Χ
		for (size_t xi = 0; xi < widthX; xi++)
		{
			cellInRoofRegion_expend[xi] = new bool[lengthY];
		}
		for (size_t xi = 0; xi < widthX; xi++)
		{
			for (size_t yi = 0; yi < lengthY; yi++)
			{
				cellInRoofRegion_expend[xi][yi] = cellInRoofRegion[xi][yi];
			}
		}
		//�������ݶ�������Cell��������һȦ
		//for (size_t xi = 0; xi < widthX; xi++)
		//{
		//	for (size_t yi = 0; yi < lengthY; yi++)
		//	{
		//		if (cellInRoofRegion[xi][yi])
		//		{
		//			//����Cell[xi][yi]�İ����������ģ�ͷ�Χ�ڣ��򽫸�������Ϊtrue
		//			if ((xi - 1 >= 0 && xi - 1 < widthX) && (yi - 1 >= 0 && yi - 1 < lengthY))
		//				cellInRoofRegion_expend[xi - 1][yi - 1] = true;
		//			if ((xi - 1 >= 0 && xi - 1 < widthX) && (yi >= 0 && yi < lengthY))
		//				cellInRoofRegion_expend[xi - 1][yi] = true;
		//			if ((xi - 1 >= 0 && xi - 1 < widthX) && (yi + 1 >= 0 && yi + 1 < lengthY))
		//				cellInRoofRegion_expend[xi - 1][yi + 1] = true;
		//			if ((xi >= 0 && xi < widthX) && (yi - 1 >= 0 && yi - 1 < lengthY))
		//				cellInRoofRegion_expend[xi][yi - 1] = true;
		//			if ((xi >= 0 && xi < widthX) && (yi + 1 >= 0 && yi + 1 < lengthY))
		//				cellInRoofRegion_expend[xi][yi + 1] = true;
		//			if ((xi + 1 >= 0 && xi + 1 < widthX) && (yi - 1 >= 0 && yi - 1 < lengthY))
		//				cellInRoofRegion_expend[xi + 1][yi - 1] = true;
		//			if ((xi + 1 >= 0 && xi + 1 < widthX) && (yi >= 0 && yi < lengthY))
		//				cellInRoofRegion_expend[xi + 1][yi] = true;
		//			if ((xi + 1 >= 0 && xi + 1 < widthX) && (yi + 1 >= 0 && yi + 1 < lengthY))
		//				cellInRoofRegion_expend[xi + 1][yi + 1] = true;
		//		}
		//	}
		//}
		//����������Ƭ��������Ƭ�Ƿ��ʼ��Ϊ����
		for (size_t fi = 0; fi < fSize; fi++)
		{
			if (originalSegResult[fi] != 0)
			{
				//��Χ���������Ƭ��Ϊ������Ƭ
				Vector3d p0(meshGraph->vertices[meshGraph->triangles[fi].vIndex[0]].pos[0], meshGraph->vertices[meshGraph->triangles[fi].vIndex[0]].pos[1], 0);
				Vector3d p1(meshGraph->vertices[meshGraph->triangles[fi].vIndex[1]].pos[0], meshGraph->vertices[meshGraph->triangles[fi].vIndex[1]].pos[1], 0);
				Vector3d p2(meshGraph->vertices[meshGraph->triangles[fi].vIndex[2]].pos[0], meshGraph->vertices[meshGraph->triangles[fi].vIndex[2]].pos[1], 0);
				vector<Vector3d> vertices;
				vertices.push_back(p0);
				vertices.push_back(p1);
				vertices.push_back(p2);
				vector<Vector3d> midMinMaxPos = modeltool::GetMidMinMaxPointForVertices(vertices);
				if (!cellInRoofRegion_expend[(int)floor(midMinMaxPos[1][0] - minX)][(int)floor(midMinMaxPos[1][1] - minY)]
					&& !cellInRoofRegion_expend[(int)floor(midMinMaxPos[1][0] - minX)][(int)floor(midMinMaxPos[2][1] - minY)]
					&& !cellInRoofRegion_expend[(int)floor(midMinMaxPos[2][0] - minX)][(int)floor(midMinMaxPos[1][1] - minY)]
					&& !cellInRoofRegion_expend[(int)floor(midMinMaxPos[2][0] - minX)][(int)floor(midMinMaxPos[2][1] - minY)])
				{
					initNodes[fi] = 1;
				}
			}
		}

		for (size_t xi = 0; xi < widthX; xi++)
		{
			delete[]cellInRoofRegion_expend[xi];
		}
		delete[]cellInRoofRegion_expend;

		for (size_t xi = 0; xi < widthX; xi++)
		{
			delete[]cellInRoofRegion[xi];
		}
		delete[]cellInRoofRegion;
		return initNodes;
	}
	else
	{
		return initNodes;
	}
}

/*ͼ��
initNodes: ��ʼ���ڵ㣬-1Ϊ���ڷ�Χ����Ƭ��������ͼ�0Ϊ���ָ���Ƭ��1Ϊ�ݶ���Ƭ(ǰ��)��2Ϊ������Ƭ
meshGraph��ģ���ڽ�ͼ
nodeWeightType�����ָ�ڵ�Ȩֵ��ֵ��ʽ
{
"Average"���ڵ���ǰ��������Ȩ�ؾ�Ϊ0.5
"Height"�� �Ը߶���Ϊ�ڵ�Ȩ��
"Normal"�� ����Ƭ����ֱ����н���Ϊ�ڵ�Ȩ�أ����ǵ�ǽ��һ��Ϊ��ֱ������ƬԽ��ֱ��ȨֵԽ��
"HeightNormal": Height * Normal
"Distance": ����Ƭ���ĵ㵽������Χ�ľ�����Ϊ�ڵ�Ȩֵ
}
*/
vector<int> graphCut::GraphCut(const vector<int>& initNodes, const MeshGraph *meshGraph, const string& nodeWeightType, const vector<Vector3d> &buildRegion,const vector<SNearLine>& nearLines)
{
	assert(initNodes.size() == meshGraph->triangles.size());
	int fSize = meshGraph->triangles.size();

	double midZ = meshGraph->midMinMaxPoint[0][2]; //�������ĵ�
	double minZ = meshGraph->midMinMaxPoint[1][2]; //������͵�
	double maxZ = meshGraph->midMinMaxPoint[2][2]; //������ߵ�
	double modelHeight = maxZ - minZ;

	//int edgeNum = meshGraph->graphEdgesSameEdge.size();
	typedef Graph<float, float, float> GraphType;
	//GraphType *g = new GraphType(/*estimated # of nodes*/ fSize, /*estimated # of edges*/ edgeNum);	//ͼ
	//����ͼ��Ľڵ���Ŀ�ͱ���Ŀ
	vector<int> nodeFaceIndex;	//�ڵ��Ŷ�Ӧ��������������Ƭ���
	for (size_t fi = 0; fi < fSize; fi++)
	{
		if (initNodes[fi] >= 0)
		{
			nodeFaceIndex.push_back(fi);
		}
	}
	vector<Vector2i> graphEdgesSameEdge;
	int nodeNum = nodeFaceIndex.size();
	for (size_t i = 0; i < nodeNum; i++)
	{
		const vector<int> &adjFaceIndics_e = meshGraph->adjFacesPerFace_Edge[nodeFaceIndex[i]];
		for (size_t j = 0; j < adjFaceIndics_e.size(); j++)
		{
			if (initNodes[adjFaceIndics_e[j]] >= 0 && adjFaceIndics_e[j] > nodeFaceIndex[i])
			{
				graphEdgesSameEdge.push_back(Vector2i(nodeFaceIndex[i], adjFaceIndics_e[j]));
			}
		}
	}
	int edgeNum = graphEdgesSameEdge.size();
	GraphType *g = new GraphType(/*estimated # of nodes*/ nodeNum, /*estimated # of edges*/ edgeNum);
		
	//��һ������ӽڵ�
	//vector<int> nodeFaceIndex;	//�ڵ��Ŷ�Ӧ��������������Ƭ���
	for (size_t nodeIndex = 0; nodeIndex < nodeNum; nodeIndex++)
	{
		g->add_node();
	}
	//std::cout << "���GraphCut����_1" << endl;
	//����meshGraphÿ����Ƭ��Ӧ��nodeIndex�Ķ�Ӧ��-1��ʾû�������Ƭ��Ӧ��node
	vector<int> fNodeIndexMap(fSize);
	for (size_t fi = 0; fi < fSize; fi++)
	{
		fNodeIndexMap[fi] = -1;
	}
	for (size_t ni = 0; ni < nodeNum; ni++)
	{
		fNodeIndexMap[nodeFaceIndex[ni]] = ni;
	}

	//�ڶ�����Ϊÿ��ǰ���ͱ����ڵ������Source(S)��Sink(T)�ڵ�֮���Ȩ��
	//vector<float> weights_node(fSize);
	//vector<float> weights_edge(fSize);
	for (size_t fi = 0; fi < fSize; fi++)
	{
		if (initNodes[fi] == 1)	//�ݶ���Ƭ-ǰ����Ƭ
		{
			int nodeIndex = fNodeIndexMap[fi];
			g->add_tweights(nodeIndex, 1000.0, 0.0);
		}
		else if (initNodes[fi] == 2) //������Ƭ-������Ƭ
		{
			int nodeIndex = fNodeIndexMap[fi];
			g->add_tweights(nodeIndex, 0.0, 1000.0);
		}
	}

	//��������Ϊÿ�����ָ�ڵ������Source(S)��Sink(T)�ڵ�֮���Ȩ��
	if (nodeWeightType == "Average")
	{
		for (size_t fi = 0; fi < fSize; fi++)
		{
			if (initNodes[fi] == 0)
			{
				int nodeIndex = fNodeIndexMap[fi];
				g->add_tweights(nodeIndex, 0.5, 0.5);
			}
		}
	}
	else if (nodeWeightType == "Height")
	{
		//���㵱ǰģ�͸߶�
		vector<Vector3d> buildVertices;
		for (size_t fi = 0; fi < fSize; fi++)
		{
			if (initNodes[fi] >= 0)
			{
				buildVertices.push_back(meshGraph->vertices[meshGraph->triangles[fi].vIndex[0]].pos);
				buildVertices.push_back(meshGraph->vertices[meshGraph->triangles[fi].vIndex[1]].pos);
				buildVertices.push_back(meshGraph->vertices[meshGraph->triangles[fi].vIndex[2]].pos);
			}
		}
		vector<Vector3d> midMinMax_build = modeltool::GetMidMinMaxPointForVertices(buildVertices);
		double midHeight = midMinMax_build[0][2];
		double heightRange = midMinMax_build[2][2] - midMinMax_build[1][2];

		for (size_t fi = 0; fi < fSize; fi++)
		{
			if (initNodes[fi] == 0)
			{
				int nodeIndex = fNodeIndexMap[fi];
				float w_height = (meshGraph->triangles[fi].midPos[2] - midHeight) / heightRange;
				g->add_tweights(nodeIndex, 0.5 + w_height, 0.5 - w_height);
			}
		}
	}
	else if (nodeWeightType == "Normal")
	{
		for (size_t fi = 0; fi < fSize; fi++)
		{
			if (initNodes[fi] == 0)
			{
				int nodeIndex = fNodeIndexMap[fi];
				const Vector3d &faceNormal = meshGraph->triangles[fi].normal;
				float w_t_normal = abs(faceNormal.dot(Vector3d(0, 0, 1)));
				float w_s_normal = 1 - w_t_normal;
				//g->add_tweights(i, w_s_normal, w_t_normal);
				g->add_tweights(nodeIndex, w_s_normal*0.5, w_t_normal*0.5);
			}
		}
	}
	else if (nodeWeightType == "HeightNormal")
	{
		for (size_t fi = 0; fi < fSize; fi++)
		{
			if (initNodes[fi] == 0)
			{
				int nodeIndex = fNodeIndexMap[fi];
				float w_height = (meshGraph->triangles[fi].midPos[2] - midZ) / modelHeight;
				const Vector3d &faceNormal = meshGraph->triangles[fi].normal;
				float w_t_normal = abs(faceNormal.dot(Vector3d(0, 0, 1)));
				float w_s_normal = 1 - w_t_normal;
				float w_s_height = 0.5 + w_height;
				float w_t_height = 0.5 - w_height;
				float w_s = w_s_normal * w_s_height;
				float w_t = w_s_height * w_t_height;
				//��һ��
				w_s = w_s / (w_s + w_t);
				w_t = 1 - w_s;
				g->add_tweights(nodeIndex, w_s, w_t);
			}
		}
	}
	else if (nodeWeightType == "Distance")
	{
		/*
		������Ƭ��������Χ�ľ���d
		������Ƭ������n1��x0, y0, z0��
		������Ƭ��������̾�����߶εĴ�ֱ����n2��x1, y1, 0��,z1=0��ʾ��XOYˮƽ��
		����n1,n2�ļнǣ��н�Խ�����90�ȣ�����ʾ��ǽ�洹ֱ�����ڽ����ĸ���ԽС
		*/
		vector<Vector3d> region_loop(buildRegion);
		region_loop.push_back(buildRegion[0]);
		int regionLoop_size = region_loop.size();
		vector<Vector3d> lineVetor(regionLoop_size - 1);	//ÿ���߶ε�����

		vector<double> dist2BuildRegion;
		for (size_t fi = 0; fi < fSize; fi++)
		{
			if (initNodes[fi] >= 0)
			{
				dist2BuildRegion.push_back(nearLines[fi].distance);
			}
			else
			{
				dist2BuildRegion.push_back(0);
			}
		}

		//modeltool::MinMaxNormalize(dist2BuildRegion);	//��һ����Ƭ�������ľ���
		modeltool::MinMaxNormalizeWith0(dist2BuildRegion);	//��һ����Ƭ�������ľ���
		for (size_t fi = 0; fi < fSize; fi++)
		{
			if (initNodes[fi] == 0)	//���Ϊ���ָ���Ƭ
			{
				if (meshGraph->triangles[fi].ifVertical45)//��ֱ��Ƭ��45�ȣ���������ǽ��Ĺ�ϵ�����뽨�����巶Χ�Ĺ�ϵ
				{
					int nodeIndex = fNodeIndexMap[fi];
					const Vector3d &faceNormal = meshGraph->triangles[fi].normal;	//��Ƭ������
					double  distance = dist2BuildRegion[fi];	//��Ƭ��������Χ�ľ���
					float w_t_normal = 1 - nearLines[fi].normalAngle;
					
					//float w_s = (w_t_normal + distance) / 2.0;	//ǰ��Ȩֵ
					//float w_t = 1 - w_s;	//����Ȩֵ
					//g->add_tweights(nodeIndex, w_s, 0.5);

					//����߽�disԽ��w_t_normal���ӵ�������ԽС������߽�disԽ����w_t_normal���ӵ�������Խ��
					float w_s = 0;
					if (distance >= 0.5)	//�������
					{
						w_s = distance;
					}
					else
					{
						//w_t_normal = w_t_normal * exp(-(1.0 / (distance * 2) - 1.0));
						//w_s = distance;	//ֻ���Ǿ���
						//w_s = distance + w_t_normal;
						w_t_normal = w_t_normal * distance;
						w_s = distance + w_t_normal;
					}
					g->add_tweights(nodeIndex, distance, 0.5);	//GraphCutResult_NoTheta
					//g->add_tweights(nodeIndex, w_t_normal, 0.5);	//GraphCutResult_NoD
					//g->add_tweights(nodeIndex, w_s, 0.5);		//GraphCutResult
				}
				else  //ˮƽ��Ƭ���������ݶ����������߶ȵĹ�ϵ
				{
					int nodeIndex = fNodeIndexMap[fi];
					double  distance = dist2BuildRegion[fi];	//��Ƭ��������Χ�ľ���
					g->add_tweights(nodeIndex, distance, 0.5);
					//g->add_tweights(nodeIndex, 0.5, 0.5);
				}
			}
		}
	}
	else if (nodeWeightType == "DistanceHeight")
	{
		//���㵱ǰģ�͸߶�
		vector<Vector3d> buildVertices;
		for (size_t fi = 0; fi < fSize; fi++)
		{
			if (initNodes[fi] >= 0)
			{
				buildVertices.push_back(meshGraph->vertices[meshGraph->triangles[fi].vIndex[0]].pos);
				buildVertices.push_back(meshGraph->vertices[meshGraph->triangles[fi].vIndex[1]].pos);
				buildVertices.push_back(meshGraph->vertices[meshGraph->triangles[fi].vIndex[2]].pos);
			}
		}
		vector<Vector3d> midMinMax_build = modeltool::GetMidMinMaxPointForVertices(buildVertices);
		double midHeight = midMinMax_build[0][2];
		double heightRange = midMinMax_build[2][2] - midMinMax_build[1][2];
		
		//����������Ƭ��������Χ�ľ���d
		vector<Vector3d> region_loop(buildRegion);
		region_loop.push_back(buildRegion[0]);
		int regionLoop_size = region_loop.size();
		vector<Vector3d> lineVetor(regionLoop_size - 1);	//ÿ���߶ε�����

		vector<double> dist2BuildRegion;
		for (size_t fi = 0; fi < fSize; fi++)
		{
			if (initNodes[fi] >= 0)
			{
				dist2BuildRegion.push_back(nearLines[fi].distance);
			}
			else
			{
				dist2BuildRegion.push_back(0);
			}
		}

		//modeltool::MinMaxNormalize(dist2BuildRegion);	//��һ����Ƭ�������ľ���
		modeltool::MinMaxNormalizeWith0(dist2BuildRegion);	//��һ����Ƭ�������ľ���
		for (size_t fi = 0; fi < fSize; fi++)
		{
			if (initNodes[fi] == 0)	//���Ϊ���ָ���Ƭ
			{
				double  distance = dist2BuildRegion[fi];	//��Ƭ��������Χ�ľ���
				if (meshGraph->triangles[fi].ifVertical45)//��ֱ��Ƭ��45�ȣ���������ǽ��Ĺ�ϵ�����뽨�����巶Χ�Ĺ�ϵ
				{
					int nodeIndex = fNodeIndexMap[fi];
					
					float w_height = (meshGraph->triangles[fi].midPos[2] - midHeight) / (heightRange / 2.0);

					//����߽�disԽ��w_height���ӵ�������ԽС������߽�disԽ����w_height���ӵ�������Խ��
					float w_s = 0;
					if (distance >= 0.5)	//�������
					{
						w_s = distance;
					}
					else
					{
						//w_t_normal = w_t_normal * exp(-(1.0 / (distance * 2) - 1.0));
						w_height = w_height * distance;
						w_s = 0.5 + w_height;
					}
					g->add_tweights(nodeIndex, w_s, 0.5);
				}
				else  //ˮƽ��Ƭ���������ݶ����������߶ȵĹ�ϵ
				{
					int nodeIndex = fNodeIndexMap[fi];
					double  distance = dist2BuildRegion[fi];	//��Ƭ��������Χ�ľ���
					g->add_tweights(nodeIndex, distance, 0.5);
				}
			}
		}
	}

	//���Ĳ������ͼ�ڵ�֮��ı��Լ����߸�Ȩ��
	for (size_t ei = 0; ei < edgeNum; ei++)
	{
		//�жϴ˱ߵ�������Ƭ�Ƿ���initNodes��ǰ������Χ��
		int fIndex1 = graphEdgesSameEdge[ei][0];
		int fIndex2 = graphEdgesSameEdge[ei][1];
		if (initNodes[fIndex1] >= 0 && initNodes[fIndex2] >= 0)
		{
			//���������ڵ���ָ���������Ƭ֮������ƶ�,����Ƭ֮��ļнǣ���ΪȨ��
			const STriangle &t1 = meshGraph->triangles[fIndex1];
			const STriangle &t2 = meshGraph->triangles[fIndex2];
			float w = abs(t1.normal.dot(t2.normal));
			//��ȡfIndex1��Ӧ��nodeIndex1   ��  fIndex2��Ӧ��nodeIndex2
			int nodeIndex1 = fNodeIndexMap[fIndex1];
			int nodeIndex2 = fNodeIndexMap[fIndex2];
			g->add_edge(nodeIndex1, nodeIndex2, w, w);
		}
	}

	//���岽��ͼ��
	int flow = g->maxflow();
	vector<int> graphCutResult(nodeNum);	//ͼ����
	for (size_t ni = 0; ni < nodeNum; ni++)
	{
		if (g->what_segment(ni) == GraphType::SOURCE)
			graphCutResult[ni] = 1;
		else
			graphCutResult[ni] = 2;
	}

	//����������nodeͼ����ӳ�䵽meshGraph������Ƭ
	vector<int> meshGraphCutResult(initNodes);
	for (size_t ni = 0; ni < nodeNum; ni++)
	{
		int fIndex = nodeFaceIndex[ni];
		meshGraphCutResult[fIndex] = graphCutResult[ni];
	}

	delete g;
	g = NULL;
	return meshGraphCutResult;
}

/*�󳡾��е�������GraphCut��
meshGraph���󳡾��ڽ�ͼ
init����ʼ��Ƭ��ʼ����
	{-1��ʾ�������ݶ���Χ��(�����)��Χ��
	0��ʾ���ָ���Ƭ��
	1��ʾ�ݶ���Ƭ��
	2��ʾ������Ƭ��}
boundary:�ݶ�����η�Χ
roofHeight:�ݶ���ߵ�߶�
����ֵ��ʽ��initһ��
*/
vector<int> graphCut::SceneSModelGraphCut(const MeshGraph *meshGraph, const vector<int> &init, const vector<Vector3d> &boundary, double roofHeight)
{
	//vector<int> cutResult;	
	int fSize = meshGraph->triangles.size();
	//��һ����ȡ������Ƭ��Ϊͼ��ı���
	vector<int> initNodes = GetInitBackFace_I("Ground", meshGraph, init, roofHeight);
	//�ڶ�����ȡ�߽�һȦ��Ƭ��Ϊ����
	vector<int> initNodes_Boundary = GetInitBackFace_I("Boundary", meshGraph, initNodes, roofHeight);	//CellRegion	Boundary	
	//��������ȡ�߶ȸ����ݶ���ߵ����ƬΪ����
	vector<int> initNodes_Height = GetInitBackFace_I("RoofHeight", meshGraph, initNodes_Boundary, roofHeight);
	//ȡ���ֱ�����ʼ������Ľ���, ���������Ƭȡ����
	int nodeNum = initNodes.size();
	for (size_t i = 0; i < nodeNum; i++)
	{
		//if (initNodes_Boundary[i] == 2)
		if (initNodes_Height[i] == 2)
		{
			initNodes[i] = 2;
		}
	}

	//vector<int> initNodes_Ground = GetInitBackFace_I("Ground", meshGraph, init, roofHeight);
	////�ڶ�����ȡ�߽�һȦ��Ƭ��Ϊ����
	//vector<int> initNodes_Boundary = GetInitBackFace_I("Boundary", meshGraph, initNodes_Ground, roofHeight);
	////��������ȡ�߶ȸ����ݶ���ߵ����ƬΪ����
	//vector<int> initNodes = GetInitBackFace_I("RoofHeight", meshGraph, initNodes_Boundary, roofHeight);


	//��������ȡ�߽�region�ڵ���Ƭ��Ϊͼ���ǰ��
	//vector<Vector3d> boundary = meshGraph->GetBuildRegionByRoof(initNodes);//��Χ�����
	for (size_t fi = 0; fi < fSize; fi++)
	{
		const Vector3d &v0 = meshGraph->vertices[meshGraph->triangles[fi].vIndex[0]].pos;
		const Vector3d &v1 = meshGraph->vertices[meshGraph->triangles[fi].vIndex[1]].pos;
		const Vector3d &v2 = meshGraph->vertices[meshGraph->triangles[fi].vIndex[2]].pos;

		if (modeltool::IfPointInPolygon(v0, boundary) &&
			modeltool::IfPointInPolygon(v1, boundary) &&
			modeltool::IfPointInPolygon(v2, boundary) &&
			initNodes[fi] == 0)
		{
			initNodes[fi] = 1;
		}
	}

	//return initNodes;
	//vector<int> init_expand(fSize);
	//for (size_t fi = 0; fi < fSize; fi++)
	//{
	//	init_expand[fi] = -1;
	//	if (initNodes[fi] == 1)
	//	{
	//		init_expand[fi] = 0;
	//	}
	//}
	//string expand_File = "res/wangcun4/graphCut5/init_expand.txt";
	//filetool::WriteTXTFileInt(init_expand, expand_File);

	//����ģ��������Ƭ����Χ����ε���̾���
	//vector<SNearLine> nearLines = meshGraph->GetDist2BuildRegion(boundary);

	//���ݷָ������㡰��Ҫ����ͼ���ģ����Ƭ�����ݶ�����ε���̾���
	vector<SNearLine> nearLines = meshGraph->GetDist2BuildRegion(boundary, initNodes);

	//���Ĳ���������ͼ���ͼ����ͼ�ڵ㸳Ȩֵ��������С���㷨
	//vector<int> graphCutResult = GraphCut(initNodes, meshGraph, "Distance", boundary, nearLines);

	//vector<int> graphCutResult_h = GraphCut(initNodes, meshGraph, "Height", boundary, nearLines);
	vector<int> graphCutResult = GraphCut(initNodes, meshGraph, "Distance", boundary, nearLines);
	//std::cout << "���GraphCut����" << endl;

	return graphCutResult;
}
