#include "graphCut.h"

/*
初始化背景面片
"Ground": 取最低点以上1.5米范围内且水平的面片最为背景
"RoofHeight": 取高于屋顶最高点roofHeight的面片最为背景
"BoundingBox": 按包围框范围取背景面片
"Boundary": 取边界一圈面片作为背景
"CellRegion": 按方格近似屋顶多边形范围，取范围外面片作为背景（尚未修改好）
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
		vector<Vector3d> insBoxVertices;	//当前待分割建筑包围框范围内所有顶点
		for (size_t vi = 0; vi < vSize; vi++)
		{
			if (ifVInRegion[vi])
			{
				insBoxVertices.push_back(meshGraph->vertices[vi].pos);
			}
		}
		vector<Vector3d> miMinMax = modeltool::GetMidMinMaxPointForVertices(insBoxVertices);
		double midZ = miMinMax[0][2]; //建筑中心点
		double minZ = miMinMax[1][2]; //建筑最低点
		double maxZ = miMinMax[2][2]; //建筑最高点
		double modelHeight = maxZ - minZ;

		for (size_t fi = 0; fi < fSize; fi++)
		{
			//选择水平且高度低的面片作为地面，即背景面片
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
				//选择水平且高度低的面片作为地面，即背景面片
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
			if (initNodes[fi] == 1)	//如果是屋顶
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
		boundingBox.ExpandBox(BoxOutEdge, expandSize);	//扩大包围框
		vector<Eigen::Vector2d> polygon = boundingBox.GetBoxXOY();
		for (size_t fi = 0; fi < fSize; fi++)
		{
			if (initNodes[fi] == -1)
			{
				continue;
			}
			//包围框以外的面片作为背景面片
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
	else if (initType == "CellRegion")	//尚未修改好
	{
		double minX = meshGraph->midMinMaxPoint[1][0];
		double maxX = meshGraph->midMinMaxPoint[2][0];
		double minY = meshGraph->midMinMaxPoint[1][1];
		double maxY = meshGraph->midMinMaxPoint[2][1];
		int widthX = ceil(maxX - minX);
		int lengthY = ceil(maxY - minY);
		bool** cellInRoofRegion = new bool*[widthX];	//记录此方格是否属于屋顶范围
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
		//遍历所有屋顶面片，计算cellInRoofRegion
		for (size_t fi = 0; fi < fSize; fi++)
		{
			if (originalSegResult[fi] == 0)
			{
				//包围框以外的面片作为背景面片
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

		//将属于屋顶的网格Cell区域扩大一圈
		bool** cellInRoofRegion_expend = new bool*[widthX];	//记录此方格是否属于屋顶范围
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
		//将属于屋顶的网格Cell区域扩大一圈
		//for (size_t xi = 0; xi < widthX; xi++)
		//{
		//	for (size_t yi = 0; yi < lengthY; yi++)
		//	{
		//		if (cellInRoofRegion[xi][yi])
		//		{
		//			//遍历Cell[xi][yi]的八领域，如果在模型范围内，则将该区域设为true
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
		//遍历所有面片，计算面片是否初始化为背景
		for (size_t fi = 0; fi < fSize; fi++)
		{
			if (originalSegResult[fi] != 0)
			{
				//包围框以外的面片作为背景面片
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

/*图割
initNodes: 初始化节点，-1为不在范围内面片，不参与图割；0为待分割面片；1为屋顶面片(前景)；2为背景面片
meshGraph：模型邻接图
nodeWeightType：待分割节点权值赋值方式
{
"Average"：节点与前景背景的权重均为0.5
"Height"： 以高度作为节点权重
"Normal"： 以面片与竖直方向夹角作为节点权重，考虑到墙面一般为竖直，故面片越竖直，权值越大
"HeightNormal": Height * Normal
"Distance": 以面片中心点到建筑范围的距离作为节点权值
}
*/
vector<int> graphCut::GraphCut(const vector<int>& initNodes, const MeshGraph *meshGraph, const string& nodeWeightType, const vector<Vector3d> &buildRegion,const vector<SNearLine>& nearLines)
{
	assert(initNodes.size() == meshGraph->triangles.size());
	int fSize = meshGraph->triangles.size();

	double midZ = meshGraph->midMinMaxPoint[0][2]; //建筑中心点
	double minZ = meshGraph->midMinMaxPoint[1][2]; //建筑最低点
	double maxZ = meshGraph->midMinMaxPoint[2][2]; //建筑最高点
	double modelHeight = maxZ - minZ;

	//int edgeNum = meshGraph->graphEdgesSameEdge.size();
	typedef Graph<float, float, float> GraphType;
	//GraphType *g = new GraphType(/*estimated # of nodes*/ fSize, /*estimated # of edges*/ edgeNum);	//图
	//估算图割的节点数目和边数目
	vector<int> nodeFaceIndex;	//节点编号对应的整个建筑的面片编号
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
		
	//第一步：添加节点
	//vector<int> nodeFaceIndex;	//节点编号对应的整个建筑的面片编号
	for (size_t nodeIndex = 0; nodeIndex < nodeNum; nodeIndex++)
	{
		g->add_node();
	}
	//std::cout << "完成GraphCut步骤_1" << endl;
	//建立meshGraph每个面片对应到nodeIndex的对应表，-1表示没有与该面片对应的node
	vector<int> fNodeIndexMap(fSize);
	for (size_t fi = 0; fi < fSize; fi++)
	{
		fNodeIndexMap[fi] = -1;
	}
	for (size_t ni = 0; ni < nodeNum; ni++)
	{
		fNodeIndexMap[nodeFaceIndex[ni]] = ni;
	}

	//第二步：为每个前景和背景节点添加与Source(S)和Sink(T)节点之间的权重
	//vector<float> weights_node(fSize);
	//vector<float> weights_edge(fSize);
	for (size_t fi = 0; fi < fSize; fi++)
	{
		if (initNodes[fi] == 1)	//屋顶面片-前景面片
		{
			int nodeIndex = fNodeIndexMap[fi];
			g->add_tweights(nodeIndex, 1000.0, 0.0);
		}
		else if (initNodes[fi] == 2) //地面面片-背景面片
		{
			int nodeIndex = fNodeIndexMap[fi];
			g->add_tweights(nodeIndex, 0.0, 1000.0);
		}
	}

	//第三步：为每个待分割节点添加与Source(S)和Sink(T)节点之间的权重
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
		//计算当前模型高度
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
				//归一化
				w_s = w_s / (w_s + w_t);
				w_t = 1 - w_s;
				g->add_tweights(nodeIndex, w_s, w_t);
			}
		}
	}
	else if (nodeWeightType == "Distance")
	{
		/*
		三角面片到建筑范围的距离d
		三角面片法向量n1（x0, y0, z0）
		三角面片到离其最短距离的线段的垂直向量n2（x1, y1, 0）,z1=0表示在XOY水平面
		计算n1,n2的夹角，夹角越大（最大90度），表示与墙面垂直，属于建筑的概率越小
		*/
		vector<Vector3d> region_loop(buildRegion);
		region_loop.push_back(buildRegion[0]);
		int regionLoop_size = region_loop.size();
		vector<Vector3d> lineVetor(regionLoop_size - 1);	//每条线段的向量

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

		//modeltool::MinMaxNormalize(dist2BuildRegion);	//归一化面片到建筑的距离
		modeltool::MinMaxNormalizeWith0(dist2BuildRegion);	//归一化面片到建筑的距离
		for (size_t fi = 0; fi < fSize; fi++)
		{
			if (initNodes[fi] == 0)	//如果为待分割面片
			{
				if (meshGraph->triangles[fi].ifVertical45)//竖直面片（45度），考虑与墙面的关系，即与建筑柱体范围的关系
				{
					int nodeIndex = fNodeIndexMap[fi];
					const Vector3d &faceNormal = meshGraph->triangles[fi].normal;	//面片法向量
					double  distance = dist2BuildRegion[fi];	//面片到建筑范围的距离
					float w_t_normal = 1 - nearLines[fi].normalAngle;
					
					//float w_s = (w_t_normal + distance) / 2.0;	//前景权值
					//float w_t = 1 - w_s;	//背景权值
					//g->add_tweights(nodeIndex, w_s, 0.5);

					//距离边界dis越大，w_t_normal发挥的作用因越小，距离边界dis越近，w_t_normal发挥的作用因越大
					float w_s = 0;
					if (distance >= 0.5)	//多边形内
					{
						w_s = distance;
					}
					else
					{
						//w_t_normal = w_t_normal * exp(-(1.0 / (distance * 2) - 1.0));
						//w_s = distance;	//只考虑距离
						//w_s = distance + w_t_normal;
						w_t_normal = w_t_normal * distance;
						w_s = distance + w_t_normal;
					}
					g->add_tweights(nodeIndex, distance, 0.5);	//GraphCutResult_NoTheta
					//g->add_tweights(nodeIndex, w_t_normal, 0.5);	//GraphCutResult_NoD
					//g->add_tweights(nodeIndex, w_s, 0.5);		//GraphCutResult
				}
				else  //水平面片，考虑与屋顶或底面或建筑高度的关系
				{
					int nodeIndex = fNodeIndexMap[fi];
					double  distance = dist2BuildRegion[fi];	//面片到建筑范围的距离
					g->add_tweights(nodeIndex, distance, 0.5);
					//g->add_tweights(nodeIndex, 0.5, 0.5);
				}
			}
		}
	}
	else if (nodeWeightType == "DistanceHeight")
	{
		//计算当前模型高度
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
		
		//计算三角面片到建筑范围的距离d
		vector<Vector3d> region_loop(buildRegion);
		region_loop.push_back(buildRegion[0]);
		int regionLoop_size = region_loop.size();
		vector<Vector3d> lineVetor(regionLoop_size - 1);	//每条线段的向量

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

		//modeltool::MinMaxNormalize(dist2BuildRegion);	//归一化面片到建筑的距离
		modeltool::MinMaxNormalizeWith0(dist2BuildRegion);	//归一化面片到建筑的距离
		for (size_t fi = 0; fi < fSize; fi++)
		{
			if (initNodes[fi] == 0)	//如果为待分割面片
			{
				double  distance = dist2BuildRegion[fi];	//面片到建筑范围的距离
				if (meshGraph->triangles[fi].ifVertical45)//竖直面片（45度），考虑与墙面的关系，即与建筑柱体范围的关系
				{
					int nodeIndex = fNodeIndexMap[fi];
					
					float w_height = (meshGraph->triangles[fi].midPos[2] - midHeight) / (heightRange / 2.0);

					//距离边界dis越大，w_height发挥的作用因越小，距离边界dis越近，w_height发挥的作用因越大
					float w_s = 0;
					if (distance >= 0.5)	//多边形内
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
				else  //水平面片，考虑与屋顶或底面或建筑高度的关系
				{
					int nodeIndex = fNodeIndexMap[fi];
					double  distance = dist2BuildRegion[fi];	//面片到建筑范围的距离
					g->add_tweights(nodeIndex, distance, 0.5);
				}
			}
		}
	}

	//第四步：添加图节点之间的边以及给边赋权重
	for (size_t ei = 0; ei < edgeNum; ei++)
	{
		//判断此边的两个面片是否都在initNodes当前建筑范围内
		int fIndex1 = graphEdgesSameEdge[ei][0];
		int fIndex2 = graphEdgesSameEdge[ei][1];
		if (initNodes[fIndex1] >= 0 && initNodes[fIndex2] >= 0)
		{
			//计算两个节点所指向的两个面片之间的相似度,即面片之间的夹角，作为权重
			const STriangle &t1 = meshGraph->triangles[fIndex1];
			const STriangle &t2 = meshGraph->triangles[fIndex2];
			float w = abs(t1.normal.dot(t2.normal));
			//获取fIndex1对应的nodeIndex1   和  fIndex2对应的nodeIndex2
			int nodeIndex1 = fNodeIndexMap[fIndex1];
			int nodeIndex2 = fNodeIndexMap[fIndex2];
			g->add_edge(nodeIndex1, nodeIndex2, w, w);
		}
	}

	//第五步：图割
	int flow = g->maxflow();
	vector<int> graphCutResult(nodeNum);	//图割结果
	for (size_t ni = 0; ni < nodeNum; ni++)
	{
		if (g->what_segment(ni) == GraphType::SOURCE)
			graphCutResult[ni] = 1;
		else
			graphCutResult[ni] = 2;
	}

	//第六步、将node图割结果映射到meshGraph所有面片
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

/*大场景中单个建筑GraphCut：
meshGraph：大场景邻接图
init：初始面片初始化：
	{-1表示不属于屋顶包围框(扩大后)范围；
	0表示待分割面片；
	1表示屋顶面片；
	2表示背景面片；}
boundary:屋顶多边形范围
roofHeight:屋顶最高点高度
返回值格式与init一致
*/
vector<int> graphCut::SceneSModelGraphCut(const MeshGraph *meshGraph, const vector<int> &init, const vector<Vector3d> &boundary, double roofHeight)
{
	//vector<int> cutResult;	
	int fSize = meshGraph->triangles.size();
	//第一步、取地面面片作为图割的背景
	vector<int> initNodes = GetInitBackFace_I("Ground", meshGraph, init, roofHeight);
	//第二步、取边界一圈面片作为背景
	vector<int> initNodes_Boundary = GetInitBackFace_I("Boundary", meshGraph, initNodes, roofHeight);	//CellRegion	Boundary	
	//第三步、取高度高于屋顶最高点的面片为背景
	vector<int> initNodes_Height = GetInitBackFace_I("RoofHeight", meshGraph, initNodes_Boundary, roofHeight);
	//取多种背景初始化结果的交集, 并与地面面片取并集
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
	////第二步、取边界一圈面片作为背景
	//vector<int> initNodes_Boundary = GetInitBackFace_I("Boundary", meshGraph, initNodes_Ground, roofHeight);
	////第三步、取高度高于屋顶最高点的面片为背景
	//vector<int> initNodes = GetInitBackFace_I("RoofHeight", meshGraph, initNodes_Boundary, roofHeight);


	//第三步、取边界region内的面片作为图割的前景
	//vector<Vector3d> boundary = meshGraph->GetBuildRegionByRoof(initNodes);//包围多边形
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

	//计算模型所有面片到包围多边形的最短距离
	//vector<SNearLine> nearLines = meshGraph->GetDist2BuildRegion(boundary);

	//根据分割结果计算“需要参与图割的模型面片”到屋顶多边形的最短距离
	vector<SNearLine> nearLines = meshGraph->GetDist2BuildRegion(boundary, initNodes);

	//第四步、给创建图割的图，给图节点赋权值并进行最小割算法
	//vector<int> graphCutResult = GraphCut(initNodes, meshGraph, "Distance", boundary, nearLines);

	//vector<int> graphCutResult_h = GraphCut(initNodes, meshGraph, "Height", boundary, nearLines);
	vector<int> graphCutResult = GraphCut(initNodes, meshGraph, "Distance", boundary, nearLines);
	//std::cout << "完成GraphCut步骤" << endl;

	return graphCutResult;
}
