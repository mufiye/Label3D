#include "jsontool.h"

void AddPolygons(const Json::Value& polygonsNode, SGeometry& geometry, int polygonsCount)
{
	vector<SPolygon> polygons(polygonsCount);	//多边形
	for (size_t pi = 0; pi < polygonsCount; pi++)
	{
		SPolygon &polygon = polygons[pi];
		const Json::Value& polygonNode = polygonsNode[Json::Value::ArrayIndex(pi)];
		polygon.polygonId = polygonNode["polygonId"].asInt();
		polygon.verticesCount = polygonNode["verticesCount"].asInt();
		if (polygon.verticesCount > 0)
		{
			const Json::Value& verticesNode = polygonNode["vertices"];
			for (size_t vi = 0; vi < polygon.verticesCount; vi++)
			{
				const Json::Value& vertexNode = verticesNode[Json::Value::ArrayIndex(vi)];
				polygon.vertices.push_back(Eigen::Vector3d(vertexNode[0].asDouble(), vertexNode[1].asDouble(), vertexNode[2].asDouble()));
			}
		}
		if (polygon.verticesCount >= 3)
		{
			polygon.faceNormal = myopengltool::CalTriNormal(polygon.vertices[0], polygon.vertices[1], polygon.vertices[2]);
		}
	}
	geometry.polygons = polygons;
}

void AddGeometries(const Json::Value& geometriesNode, SCityObject& object, int geometriesCount)
{
	vector<SGeometry> geometries(geometriesCount);	//几何形状
	for (size_t gi = 0; gi < geometriesCount; gi++)
	{
		SGeometry &geometry = geometries[gi];
		const Json::Value& geometryNode = geometriesNode[Json::Value::ArrayIndex(gi)];
		geometry.geometryId = geometryNode["geometryId"].asInt();
		geometry.polygonsCount = geometryNode["polygonsCount"].asInt();
		if (geometry.polygonsCount > 0)
		{
			const Json::Value& polygonsNode = geometryNode["polygons"];
			AddPolygons(polygonsNode, geometry, geometry.polygonsCount);
		}
	}
	object.geometries = geometries;
}

void AddChildObjects(const Json::Value& objectNode, SCityObject& fatherObject)
{
	//解析当前类对象属性
	fatherObject.objectId = objectNode["objectId"].asInt();
	fatherObject.objectType = objectNode["objectType"].asString();
	fatherObject.geometriesCount = objectNode["geometriesCount"].asInt();
	if (fatherObject.geometriesCount > 0)
	{
		const Json::Value& geometriesNode = objectNode["geometries"];
		AddGeometries(geometriesNode, fatherObject, fatherObject.geometriesCount);
	}

	//递归解析子类对象
	fatherObject.childCityObjectsCount = objectNode["childObjectsCount"].asInt();
	int childNodeSize = objectNode["objects"].size();
	if (fatherObject.childCityObjectsCount > 0)
	{
		if (fatherObject.childCityObjectsCount == childNodeSize)
		{
			vector<SCityObject> childCityObjects(childNodeSize);
			for (size_t ci = 0; ci < childNodeSize; ci++)
			{
				const Json::Value& childNode = objectNode["objects"][Json::Value::ArrayIndex(ci)];
				AddChildObjects(childNode, childCityObjects[ci]);
			}
			fatherObject.childCityObjects = childCityObjects;
		}
	}
}

//读取CityGML中建筑信息
vector<SCityObject> jsontool::RCityGMLInfo(const string& fileName)
{
	vector<SCityObject> cityObjects;
	ifstream ifs(fileName);
	Json::CharReaderBuilder readerbuilder;
	//Json::Reader reader;
	Json::Value root;
	JSONCPP_STRING errs;
	bool ok = Json::parseFromStream(readerbuilder, ifs, &root, &errs);
	if (ok)
	{
		int objectsCount = root["objects"].size();
		Json::Value& objectsRoot = root["objects"];
		cityObjects.resize(objectsCount);
		for (size_t oi = 0; oi < objectsCount; oi++)
		{
			Json::Value& objectNode = objectsRoot[Json::Value::ArrayIndex(oi)];
			AddChildObjects(objectNode, cityObjects[oi]);
		}
	}
	return cityObjects;
}

//将模型位置坐标信息写入json文件
bool jsontool::WModelPositionInfo(const vector<SModelPosInfo>& modelPosInfos, const string& saveJsonFile)
{
	//根节点
	Json::Value root;
	int modelCount = modelPosInfos.size();
	root["modelCount"] = modelCount;

	for (int i = 0; i < modelCount; i++)
	{
		Json::Value modelNode;
		modelNode["id"] = modelPosInfos[i].id;
		modelNode["modelPath"] = modelPosInfos[i].modelPath;
		modelNode["modelName"] = modelPosInfos[i].modelName;

		Json::Value posNode;
		posNode.append(modelPosInfos[i].position[0]);
		posNode.append(modelPosInfos[i].position[1]);
		posNode.append(modelPosInfos[i].position[2]);
		modelNode["position"] = posNode;

		Json::Value minMaxXNode;
		minMaxXNode.append(modelPosInfos[i].minMaxX[0]);
		minMaxXNode.append(modelPosInfos[i].minMaxX[1]);
		modelNode["minMaxX"] = minMaxXNode;

		Json::Value minMaxYNode;
		minMaxYNode.append(modelPosInfos[i].minMaxY[0]);
		minMaxYNode.append(modelPosInfos[i].minMaxY[1]);
		modelNode["minMaxY"] = minMaxYNode;

		Json::Value minMaxZNode;
		minMaxZNode.append(modelPosInfos[i].minMaxZ[0]);
		minMaxZNode.append(modelPosInfos[i].minMaxZ[1]);
		modelNode["minMaxZ"] = minMaxZNode;

		modelNode["columnIndex"] = modelPosInfos[i].columnIndex;
		modelNode["rowIndex"] = modelPosInfos[i].rowIndex;

		root["modelPosInfos"].append(modelNode);
	}

	Json::StreamWriterBuilder  builder;
	builder["commentStyle"] = "None";
	builder["indentation"] = "	";
	std::unique_ptr<Json::StreamWriter> writer(builder.newStreamWriter());

	try
	{
		std::ofstream ofs;
		ofs.open(saveJsonFile, std::ios::app);
		writer->write(root, &ofs);
	}
	catch (const std::exception& e)
	{
		return false;
	}

	return true;
}

//读取模型位置坐标信息
vector<SModelPosInfo> jsontool::RModelPositionInfo(const string& fileName)
{
	vector<SModelPosInfo> modelPosInfos;

	ifstream ifs(fileName);
	Json::CharReaderBuilder readerbuilder;
	//Json::Reader reader;
	Json::Value root;
	JSONCPP_STRING errs;
	bool ok = Json::parseFromStream(readerbuilder, ifs, &root, &errs);
	if (ok)
	{
		int modelCount = root["modelCount"].asInt();
		Json::Value& modelPosInfosNode = root["modelPosInfos"];
		modelPosInfos.resize(modelCount);
		for (int i = 0; i < modelCount; i++)
		{
			SModelPosInfo &modelPosInfo = modelPosInfos[i];
			Json::Value& posInfoNode = modelPosInfosNode[i];
			modelPosInfo.id = posInfoNode["id"].asInt();
			modelPosInfo.modelPath = posInfoNode["modelPath"].asString();
			modelPosInfo.modelName = posInfoNode["modelName"].asString();
			modelPosInfo.position = Eigen::Vector3d(posInfoNode["position"][0].asDouble(), posInfoNode["position"][1].asDouble(), posInfoNode["position"][2].asDouble());
			modelPosInfo.minMaxX = Eigen::Vector2d(posInfoNode["minMaxX"][0].asDouble(), posInfoNode["minMaxX"][1].asDouble());
			modelPosInfo.minMaxY = Eigen::Vector2d(posInfoNode["minMaxY"][0].asDouble(), posInfoNode["minMaxY"][1].asDouble());
			modelPosInfo.minMaxZ = Eigen::Vector2d(posInfoNode["minMaxZ"][0].asDouble(), posInfoNode["minMaxZ"][1].asDouble());
			modelPosInfo.columnIndex = posInfoNode["columnIndex"].asInt();
			modelPosInfo.rowIndex = posInfoNode["rowIndex"].asInt();
		}
	}
	return modelPosInfos;
}

