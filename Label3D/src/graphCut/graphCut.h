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
	��ʼ��������Ƭ
	"Ground": ȡ��͵�����1.5�׷�Χ����ˮƽ����Ƭ��Ϊ����
	"RoofHeight": ȡ�����ݶ���ߵ�roofHeight����Ƭ��Ϊ����
	"BoundingBox": ����Χ��Χȡ������Ƭ
	"Boundary": ȡ�߽�һȦ��Ƭ��Ϊ����
	"CellRegion": ����������ݶ�����η�Χ��ȡ��Χ����Ƭ��Ϊ��������δ�޸ĺã�
	*/
	vector<int> GetInitBackFace_I(const string initType, const MeshGraph *meshGraph, const vector<int> &originalSegResult, double roofHeight);


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
	buildRegion: ǰ������η�Χ������nodeWeightType=��Distance��ʱʹ��
	nearLines:ÿ����Ƭ��ǰ������߶ε���Ϣ������nodeWeightType=��Distance��ʱʹ��
	*/
	vector<int> GraphCut(const vector<int>& initNodes,const MeshGraph *meshGraph, const string& nodeWeightType,const vector<Vector3d> &buildRegion, const vector<SNearLine>& nearLines);


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
	vector<int> SceneSModelGraphCut(const MeshGraph *meshGraph, const vector<int> &init, const vector<Vector3d> &boundary, double roofHeight);
}