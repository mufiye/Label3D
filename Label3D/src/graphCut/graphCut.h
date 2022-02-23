#pragma once
#include <Eigen\Dense>
#include <iostream>
#include <string>
#include <vector>
#include "graph.h"
#include "../model.h"

namespace graphCut
{
	/*
	初始化背景面片
	"Ground": 取最低点以上1.5米范围内且水平的面片最为背景
	"RoofHeight": 取高于屋顶最高点roofHeight的面片最为背景
	"BoundingBox": 按包围框范围取背景面片
	"Boundary": 取边界一圈面片作为背景
	"CellRegion": 按方格近似屋顶多边形范围，取范围外面片作为背景（尚未修改好）
	*/
	vector<int> GetInitBackFace_I(const string initType, const MeshGraph *meshGraph, const vector<int> &originalSegResult, double roofHeight);


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
	buildRegion: 前景多边形范围，仅当nodeWeightType=“Distance”时使用
	nearLines:每个面片到前景最短线段的信息，仅当nodeWeightType=“Distance”时使用
	*/
	vector<int> GraphCut(const vector<int>& initNodes,const MeshGraph *meshGraph, const string& nodeWeightType,const vector<Vector3d> &buildRegion, const vector<SNearLine>& nearLines);


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
	vector<int> SceneSModelGraphCut(const MeshGraph *meshGraph, const vector<int> &init, const vector<Vector3d> &boundary, double roofHeight);
}