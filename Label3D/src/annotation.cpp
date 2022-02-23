#include "annotation.h"

inline double Random() { return rand() % 100 / (double)101; };

Annotation::Annotation() :numFaces(0), numInstance(0)
{

}

//ɾ��ʵ��
bool Annotation::DeleteInstance(int insIndex)
{
	if (insIndex >= numInstance || insIndex <= 0)
	{
		return false;
	}

	int insFaceSize = insFaces[insIndex].size();
	for (size_t i = 0; i < insFaceSize; i++)
	{
		int faceIndex = insFaces[insIndex][i];
		segInfo[faceIndex] = -1;
		insFaces[0].push_back(faceIndex);
	}
	insFaces[insIndex].clear();

	//insBBox[insIndex].clear();
}

//���±�ע��Ϣ
void Annotation::UpdateInsFaces()
{
	vector<int> segInfo_new(numFaces);	//��ע��Ϣ
	vector<vector<int>> insFaces_noEmpty;	//ÿ��ʵ����������Ƭ����,��0��Ϊ����
	vector<Vector3f> insColors_noEmpty;		//ʵ����ɫ
	vector <CBoundingBox> insBBox_noEmpty;	//ÿ��ʵ����Χ��

	for (size_t i = 0; i < numInstance; i++)
	{
		if (insFaces[i].size() > 0)
		{
			insFaces_noEmpty.push_back(insFaces[i]);
			insColors_noEmpty.push_back(insColors[i]);
			insBBox_noEmpty.push_back(insBBox[i]);
		}
	}

	numInstance = insFaces_noEmpty.size();
	for (size_t i = 0; i < numFaces; i++)
	{
		segInfo_new[i] = -1;
	}

	if (numInstance > 1)
	{
		for (size_t i = 1; i < numInstance; i++)
		{
			int numInsFace = insFaces_noEmpty[i].size();
			for (size_t j = 0; j < numInsFace; j++)
			{
				segInfo_new[insFaces_noEmpty[i][j]] = i - 1;
			}
		}
	}

	segInfo = segInfo_new;
	insFaces = insFaces_noEmpty;
	insColors = insColors_noEmpty;
	insBBox = insBBox_noEmpty;
}

//��ʵ��������Ƭ
bool Annotation::AddMesh(int faceIndex, int insIndex)
{
	if (faceIndex >= numFaces || faceIndex<0 || insIndex>numInstance || insIndex < 0)
	{
		return false;
	}

	int insIndex_old = segInfo[faceIndex] + 1;
	int insIndex_new = insIndex;
	if (insIndex_old != insIndex_new)
	{
		segInfo[faceIndex] = insIndex_new - 1;

		//����ʵ����insFaces�������Ƭ����
		insFaces[insIndex_new].push_back(faceIndex);
		//ɾ��ԭʵ���и���Ƭ����
		vector<int>::iterator iter = find(insFaces[insIndex_old].begin(), insFaces[insIndex_old].end(), faceIndex);
		if (iter != insFaces[insIndex_old].end())
		{
			insFaces[insIndex_old].erase(iter);
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
}

//����µ�ʵ��
bool Annotation::AddNewInstance()
{
	vector<int> faces;
	insFaces.push_back(faces);
	Vector3f insColor;
	insColor[0] = (float)Random();
	insColor[1] = (float)Random();
	insColor[2] = (float)Random();
	insColors.push_back(insColor);
	CBoundingBox bbox;
	insBBox.push_back(bbox);

	numInstance++;

	return true;
}

//ɾ��ʵ����Ƭ
bool Annotation::DeleteMesh(int faceIndex)
{
	int insIndex_old = segInfo[faceIndex] + 1;

	if (faceIndex >= numFaces || faceIndex < 0 || insIndex_old>numInstance || insIndex_old < 0)
	{
		return false;
	}
	if (insIndex_old == 0)
	{
		return true;
	}
	else
	{
		//����ʵ����insFaces�������Ƭ����
		insFaces[0].push_back(faceIndex);
		segInfo[faceIndex] = -1;
		//ɾ��ԭʵ���и���Ƭ����
		vector<int>::iterator iter = find(insFaces[insIndex_old].begin(), insFaces[insIndex_old].end(), faceIndex);
		if (iter != insFaces[insIndex_old].end())
		{
			insFaces[insIndex_old].erase(iter);
			return true;
		}
		else
		{
			return false;
		}
	}
}

//�����ע��Ϣ���ļ�
bool Annotation::SaveToFile(const string& fileName, bool updateLabelInfo)
{
	if (updateLabelInfo)
	{
		UpdateInsFaces();
	}

	bool isOk = filetool::WriteTXTFileInt(segInfo, fileName);
	if (isOk)
	{
		return true;
	}
	else
	{
		return false;
	}
}

LabelInfo::LabelInfo(GLMmodel *m)
{
	this->model = m;

	segInfo.resize(model->numtriangles);
	for (size_t i = 0; i < model->numtriangles; i++)
	{
		segInfo[i] = Vector2i(-1, -1);
	}

	backGround.instanceId = -1;
	backGround.insColor = Vector3f(0.67f, 0.67f, 0.67f);
	vector<Vector3d> insFaces;
	for (size_t fi = 0; fi < model->numtriangles; fi++)
	{
		for (size_t k = 0; k < 3; k++)
		{
			double x = model->vertices[3 * model->triangles[fi].vindices[k] + 0];
			double y = model->vertices[3 * model->triangles[fi].vindices[k] + 1];
			double z = model->vertices[3 * model->triangles[fi].vindices[k] + 2];
			insFaces.push_back(Vector3d(x, y, z));
		}
	}
	backGround.insBBox.InitBox(insFaces);
	for (size_t fi = 0; fi < model->numtriangles; fi++)
	{
		backGround.insFaces.push_back(fi);
	}
}

//�������Id��ʵ��Id����ʵ��
LabelInfo::Instance& LabelInfo::SearchInstance(int cateId, int insId)
{
	Instance* nullIns = new Instance;
	nullIns->instanceId = -2;

	if (cateId == -1 && insId == -1)
	{
		return backGround;
	}

	for (size_t ci = 0; ci < categories.size(); ci++)
	{
		if (categories[ci].categoryId == cateId)
		{
			int insSize = categories[ci].instances.size();
			for (size_t ii = 0; ii < insSize; ii++)
			{
				if (categories[ci].instances[ii].instanceId == insId)
				{
					return categories[ci].instances[ii];
				}
			}
		}
	}
	return *nullIns;
}

//�������Id��ѯ������Ƿ�Ϊthing
bool LabelInfo::CheckCateIsthing(int cateId)
{
	if (cateId < 0)
	{
		return false;
	}
	for (size_t ci = 0; ci < categories.size(); ci++)
	{
		if (categories[ci].categoryId == cateId)
		{
			if (categories[ci].isthing == 1)
			{
				return true;
			}
			else
			{
				return false;
			}
		}
	}
	return false;
}

//ɸѡ[cateId,insId]����Ƭ
vector<int> LabelInfo::SelectFaces(const vector<int> &faces, int cateId, int insId)
{
	vector<int> faces_selected;
	int num_faces = faces.size();
	for (size_t i = 0; i < num_faces; i++)
	{
		int cateId_old = segInfo[faces[i]][0];
		int insId_old = segInfo[faces[i]][1];
		if (cateId_old == cateId && insId_old == insId)
		{
			faces_selected.push_back(faces[i]);
		}
	}
	return faces_selected;
}

//ɸѡ������[cateId, insId]��ͬ���ͬʵ������Ƭ
vector<int> LabelInfo::SelectFacesDifferent(const vector<int> &faces, int cateId, int insId)
{
	vector<int> faces_selected;
	int num_faces = faces.size();
	for (size_t i = 0; i < num_faces; i++)
	{
		int cateId_old = segInfo[faces[i]][0];
		int insId_old = segInfo[faces[i]][1];
		if (cateId_old != cateId || insId_old != insId)
		{
			faces_selected.push_back(faces[i]);
		}
	}
	return faces_selected;
}

//ɸѡ������Ƭ
vector<int> LabelInfo::SelectBackFaces(const vector<int> &faces)
{
	vector<int> faces_selected;
	int num_faces = faces.size();
	for (size_t i = 0; i < num_faces; i++)
	{
		int cateId_old = segInfo[faces[i]][0];
		int insId_old = segInfo[faces[i]][1];
		if (cateId_old == -1 || insId_old == -1)
		{
			faces_selected.push_back(faces[i]);
		}
	}
	return faces_selected;
}

//�����������ʵ����ɫ
void LabelInfo::SetInsColor()
{
	for (size_t ci = 0; ci < categories.size(); ci++)
	{
		int numInstace = categories[ci].instances.size();
		for (size_t ii = 0; ii < numInstace; ii++)
		{
			if (categories[ci].isthing == 1)
			{
				categories[ci].instances[ii].insColor = Vector3f((float)Random(), (float)Random(), (float)Random());
			}
			else
			{
				categories[ci].instances[ii].insColor = categories[ci].cateColor / 256.0;;
			}
		}
	}
}

//���㱳����Χ��
void LabelInfo::CalcBackGroundBBox()
{
	vector<Vector3d> backFaces;
	int backFaceSize = this->backGround.insFaces.size();
	for (size_t fi = 0; fi < backFaceSize; fi++)
	{
		int faceIndex = this->backGround.insFaces[fi];
		for (size_t k = 0; k < 3; k++)
		{
			double x = model->vertices[3 * model->triangles[faceIndex].vindices[k] + 0];
			double y = model->vertices[3 * model->triangles[faceIndex].vindices[k] + 1];
			double z = model->vertices[3 * model->triangles[faceIndex].vindices[k] + 2];
			backFaces.push_back(Vector3d(x, y, z));
		}
	}
	backGround.insBBox.InitBox(backFaces);
}

//���㵥��ʵ����Χ��
void LabelInfo::CalcSingleBBox(int cateId, int insId)
{
	vector<int> insFaces;
	for (size_t ci = 0; ci < categories.size(); ci++)
	{
		if (categories[ci].categoryId == cateId)
		{
			int numInstace = categories[ci].instances.size();
			for (size_t ii = 0; ii < numInstace; ii++)
			{
				if (categories[ci].instances[ii].instanceId == insId)
				{
					int insFaceSize = categories[ci].instances[ii].insFaces.size();
					if (insFaceSize <= 0)
					{
						continue;
					}
					vector<Vector3d> insFaces;
					for (size_t fi = 0; fi < insFaceSize; fi++)
					{
						int faceIndex = categories[ci].instances[ii].insFaces[fi];
						for (size_t k = 0; k < 3; k++)
						{
							double x = model->vertices[3 * model->triangles[faceIndex].vindices[k] + 0];
							double y = model->vertices[3 * model->triangles[faceIndex].vindices[k] + 1];
							double z = model->vertices[3 * model->triangles[faceIndex].vindices[k] + 2];
							insFaces.push_back(Vector3d(x, y, z));
						}
					}
					categories[ci].instances[ii].insBBox.InitBox(insFaces);
				}
			}
		}
	}
}

//��������ʵ����Χ��
void  LabelInfo::CalcBBox()
{
	CalcBackGroundBBox();

	for (size_t ci = 0; ci < categories.size(); ci++)
	{
		int numInstace = categories[ci].instances.size();
		for (size_t ii = 0; ii < numInstace; ii++)
		{
			int insFaceSize = categories[ci].instances[ii].insFaces.size();
			if (insFaceSize<=0)
			{
				continue;
			}
			vector<Vector3d> insFaces;
			for (size_t fi = 0; fi < insFaceSize; fi++)
			{
				int faceIndex = categories[ci].instances[ii].insFaces[fi];
				for (size_t k = 0; k < 3; k++)
				{
					double x = model->vertices[3 * model->triangles[faceIndex].vindices[k] + 0];
					double y = model->vertices[3 * model->triangles[faceIndex].vindices[k] + 1];
					double z = model->vertices[3 * model->triangles[faceIndex].vindices[k] + 2];
					insFaces.push_back(Vector3d(x, y, z));
				}
			}
			categories[ci].instances[ii].insBBox.InitBox(insFaces);
		}
	}
}

// ɾ����ʵ����������0����Ƭ��ʵ��
void LabelInfo::DeleteEmptyIns()
{
	for (size_t ci = 0; ci < categories.size(); ci++)
	{
		vector<Instance> instances;
		for (size_t ii = 0; ii < categories[ci].instances.size(); ii++)
		{
			if (categories[ci].instances[ii].insFaces.size() > 0)
			{
				instances.push_back(categories[ci].instances[ii]);
			}
		}
		for (size_t ii = 0; ii < instances.size(); ii++)
		{
			instances[ii].instanceId = ii;
		}

		categories[ci].instances = instances;
	}

	segInfo.clear();
	segInfo.resize(model->numtriangles);
	for (size_t i = 0; i < model->numtriangles; i++)
	{
		segInfo[i] = Vector2i(-1, -1);
	}

	for (size_t ci = 0; ci < categories.size(); ci++)
	{
		for (size_t ii = 0; ii < categories[ci].instances.size(); ii++)
		{
			for (size_t fi = 0; fi < categories[ci].instances[ii].insFaces.size(); fi++)
			{
				segInfo[categories[ci].instances[ii].insFaces[fi]][0] = categories[ci].categoryId;
				segInfo[categories[ci].instances[ii].insFaces[fi]][1] = categories[ci].instances[ii].instanceId;
			}
		}
	}
}

//ɾ������0��ʵ�������
void LabelInfo::DeleteEmptyCategory()
{
	vector<Category> cates_noEmpty;
	for (size_t ci = 0; ci < categories.size(); ci++)
	{
		if (categories[ci].instances.size() > 0)
		{
			cates_noEmpty.push_back(categories[ci]);
		}
	}
	this->categories = cates_noEmpty;
}

//�����Id��ʵ��Id���±��
void LabelInfo::RenumberCateInsIds()
{
	for (size_t ci = 0; ci < categories.size(); ci++)
	{
		categories[ci].categoryId = ci;

		int insSize = categories[ci].instances.size();
		for (size_t ii = 0; ii < insSize; ii++)
		{
			categories[ci].instances[ii].instanceId = ii;
		}
	}
}

//����categories����segInfo
void LabelInfo::UpdateSegInfoWithCate()
{
	int numFaces = segInfo.size();
	vector<Vector2i> newSegInfo(numFaces);
	for (size_t i = 0; i < numFaces; i++)
	{
		newSegInfo[i] = Vector2i(-1, -1);
	}

	for (size_t ci = 0; ci < categories.size(); ci++)
	{
		int insSize = categories[ci].instances.size();
		for (size_t ii = 0; ii < insSize; ii++)
		{
			int fSize = categories[ci].instances[ii].insFaces.size();
			for (size_t fi = 0; fi < fSize; fi++)
			{
				int faceIndex = categories[ci].instances[ii].insFaces[fi];
				newSegInfo[faceIndex][0] = categories[ci].categoryId;
				newSegInfo[faceIndex][1] = categories[ci].instances[ii].instanceId;
			}
		}
	}

}

bool LabelInfo::SaveAsFile(const string& fileName)
{
	DeleteEmptyIns();
	//DeleteEmptyCategory();
	RenumberCateInsIds();
	UpdateSegInfoWithCate();

	//��ʱ
	//string txtFile = "F:/StudyDemo/Label3D/Label3D/data/project5/buildings_gt.txt";
	//vector<int> faceSegResult = filetool::ReadTxtFileInt(txtFile);
	//int fSize = faceSegResult.size();
	//for (size_t fi = 0; fi < fSize; fi++)
	//{
	//	if (faceSegResult[fi] >=0 )
	//	{
	//		segInfo[fi][0] = 0;
	//		segInfo[fi][1] = faceSegResult[fi];
	//	}
	//}

	//���ڵ�
	Json::Value root;

	root["categories"] = Json::arrayValue;
	for (size_t ci = 0; ci < categories.size(); ci++)
	{
		Json::Value cate;
		cate["id"] = categories[ci].categoryId;
		cate["name"] = categories[ci].categoryName;
		cate["isthing"] = categories[ci].isthing;
		cate["numInstance"] = categories[ci].instances.size();
		cate["supercategory"] = categories[ci].supercategory;
		cate["color"].append((int)categories[ci].cateColor[0]);
		cate["color"].append((int)categories[ci].cateColor[1]);
		cate["color"].append((int)categories[ci].cateColor[2]);
		root["categories"].append(cate);
	}

	int numFaces = this->segInfo.size();
	root["numFaces"] = numFaces;
	root["segInfo"] = Json::arrayValue;
	for (int fi = 0; fi < numFaces; fi++)
	{
		Json::Value faceInfo;
		faceInfo.append(segInfo[fi][0]);
		faceInfo.append(segInfo[fi][1]);
		root["segInfo"].append(faceInfo);
	}

	Json::StreamWriterBuilder  builder;
	builder["commentStyle"] = "None";
	builder["indentation"] = "	";
	std::unique_ptr<Json::StreamWriter> writer(builder.newStreamWriter());

	try
	{
		std::ofstream ofs;
		//ofs.open(fileName, std::ios::app);	//�����ǣ����ļ���βд��
		ofs.open(fileName);		//���Ǳ���
		writer->write(root, &ofs);
	}
	catch (const std::exception& e)
	{
		return false;
	}
	return true;
}

bool LabelInfo::ReadFromFile(const string& fileName)
{
	ifstream ifs(fileName);
	Json::CharReaderBuilder readerbuilder;
	//Json::Reader reader;
	Json::Value root;
	JSONCPP_STRING errs;
	bool ok = Json::parseFromStream(readerbuilder, ifs, &root, &errs);
	if (ok)
	{
		int numFaces = root["numFaces"].asInt();
		if (numFaces != this->model->numtriangles)
		{
			std::cout << "��ע�ļ���ģ�Ͳ�ƥ�䣡" << endl;
			return false;
		}

		this->segInfo.clear();
		this->segInfo.resize(numFaces);
		for (int fi = 0; fi < numFaces; fi++)
		{
			segInfo[fi][0] = root["segInfo"][fi][0].asInt();
			segInfo[fi][1] = root["segInfo"][fi][1].asInt();
		}
		int categoryCount = root["categories"].size();
		this->categories.clear();
		this->categories.resize(categoryCount);
		for (int ci = 0; ci < categoryCount; ci++)
		{
			categories[ci].categoryId = root["categories"][ci]["id"].asInt();
			categories[ci].categoryName = root["categories"][ci]["name"].asString();
			categories[ci].isthing = root["categories"][ci]["isthing"].asInt();
			categories[ci].supercategory = root["categories"][ci]["supercategory"].asString();
			categories[ci].cateColor = Eigen::Vector3f(root["categories"][ci]["color"][0].asFloat(),
													root["categories"][ci]["color"][1].asFloat(),
													root["categories"][ci]["color"][2].asFloat());

			int numInstance = root["categories"][ci]["numInstance"].asInt();
			categories[ci].newInsId = numInstance;
			categories[ci].instances.resize(numInstance);
			for (size_t ii = 0; ii < numInstance; ii++)
			{
				categories[ci].instances[ii].instanceId = ii;
			}
		}

		//����segInfo��ʼ��categories.instance �� background
		this->backGround.insFaces.clear();
		for (int fi = 0; fi < numFaces; fi++)
		{
			if (segInfo[fi][0] == -1)
			{
				backGround.insFaces.push_back(fi);
			}
			else
			{
				categories[segInfo[fi][0]].instances[segInfo[fi][1]].insFaces.push_back(fi);
			}
		}

		SetBackGroundColor(Vector3f(0.67, 0.67, 0.67));
		SetInsColor();
		CalcBBox();
	}

	return true;
}

//����ʵ��, cateId: ���Id;  ����������InstanceId
int LabelInfo::AddNewInstance(int cateId)
{
	for (size_t ci = 0; ci < categories.size(); ci++)
	{
		if (categories[ci].categoryId == cateId)
		{
			if (categories[ci].isthing == 1)
			{
				Instance ins;
				ins.instanceId = categories[ci].newInsId;
				ins.insColor = Vector3f((float)Random(), (float)Random(), (float)Random());
				categories[ci].instances.push_back(ins);
				categories[ci].newInsId++;
				return ins.instanceId;
			}
			else if (categories[ci].isthing == 0)
			{
				//if (categories[ci].newInsId > 0)
				//{
				//	return -1;
				//}
				//else
				//{
					Instance ins;
					ins.instanceId = categories[ci].newInsId;
					ins.insColor = categories[ci].cateColor / 256.0;
					categories[ci].instances.push_back(ins);
					categories[ci].newInsId++;
					return ins.instanceId;
				//}
			}
		}
	}
	return -1;
}

//�������cateName���������; �������������Id
int LabelInfo::AddNewCategory(const string& cateName)
{
	Category cate;
	cate.categoryName = cateName;
	categories.push_back(cate);
	return cate.categoryId;
}

//ɾ��ʵ��
bool LabelInfo::DeleteInstance(int cateId, int insId)
{
	for (size_t ci = 0; ci < categories.size(); ci++)
	{
		if (cateId == categories[ci].categoryId)
		{
			vector<Instance>::iterator itor;
			//for (size_t ii = 0; ii < insSize; ii++)
			for (itor = categories[ci].instances.begin(); itor != categories[ci].instances.end(); itor++)
			{
				if (insId == itor->instanceId)
				{
					int fSize = itor->insFaces.size();
					//1����׼��ɾ����ʵ����Ƭ������ӵ�backGround;
					for (size_t fi = 0; fi < fSize; fi++)
					{
						backGround.insFaces.push_back(itor->insFaces[fi]);
					}
					CalcBackGroundBBox();
					//2������segInfo��׼��ɾ����ʵ������Ƭ
					for (size_t fi = 0; fi < fSize; fi++)
					{
						segInfo[itor->insFaces[fi]] = Vector2i(-1, -1);
					}
					//3��ɾ��ʵ��
					categories[ci].instances.erase(itor);

					return true;
				}
			}
		}
	}
	return false;
}

//ɾ�����
bool LabelInfo::DeleteCategory(int cateId)
{
	vector<Category>::iterator itor;
	for (itor = categories.begin(); itor != categories.end(); itor++)
	{
		if (itor->categoryId == cateId)
		{
			int insSize = itor->instances.size();
			//1.�������������ʵ����Ƭ������ӵ�backGround;
			//2.������segInfo��Ϣ
			for (size_t ii = 0; ii < insSize; ii++)
			{
				int fSize = itor->instances[ii].insFaces.size();
				for (size_t fi = 0; fi < fSize; fi++)
				{
					backGround.insFaces.push_back(itor->instances[ii].insFaces[fi]);
					segInfo[itor->instances[ii].insFaces[fi]] = Vector2i(-1, -1);
				}
			}

			//3.ɾ�������
			categories.erase(itor);
			return true;
		}
	}
	return false;
}

//��ӵ���ʵ����Ƭ
bool LabelInfo::AddMesh(int faceIndex, int cateId, int insId)
{
	Instance& instance_new = SearchInstance(cateId, insId);
	if (faceIndex >= model->numtriangles || faceIndex<0 || cateId<0 || insId < 0)
	{
		return false;
	}

	int cateId_old = segInfo[faceIndex][0];
	int insId_old = segInfo[faceIndex][1];
	int cateId_new = cateId;
	int insId_new = insId;
	if (cateId_old != cateId_new && insId_old != insId_new)
	{
		Instance& instance_old = SearchInstance(cateId_old, insId_old);
		if (instance_new.instanceId == -2 || instance_old.instanceId == -2)
		{
			return false;
		}

		segInfo[faceIndex] = Vector2i(cateId_new, insId_new);

		//����ʵ����insFaces�������Ƭ����
		instance_new.insFaces.push_back(faceIndex);
		//ɾ��ԭʵ���и���Ƭ����
		vector<int>::iterator iter = find(instance_old.insFaces.begin(), instance_old.insFaces.end(), faceIndex);
		if (iter != instance_old.insFaces.end())
		{
			instance_old.insFaces.erase(iter);
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		return false;
	}
}

//���ʵ����Ƭ����
bool LabelInfo::AddMesh(vector<int> faceIndices, int cateId, int insId)
{
	bool isOk = true;
	for (size_t fi = 0; fi < faceIndices.size(); fi++)
	{
		int faceIndex = faceIndices[fi];

		isOk = AddMesh(faceIndex, cateId, insId);
	}
	return isOk;
}

//ɾ������ʵ����Ƭ
bool LabelInfo::DeleteMesh(int faceIndex)
{
	int cateId_old = segInfo[faceIndex][0];
	int insId_old = segInfo[faceIndex][1];

	if (cateId_old == -1 && insId_old == -1)
	{
		return true;
	}
	if (faceIndex >= model->numtriangles || faceIndex < 0 || insId_old < 0)
	{
		return false;
	}
	else
	{
		Instance& instance_old = SearchInstance(cateId_old, insId_old);
		if (instance_old.instanceId == -2)
		{
			return false;
		}

		//����ʵ����insFaces�������Ƭ����
		backGround.insFaces.push_back(faceIndex);
		segInfo[faceIndex] = Vector2i(-1, -1);
		//ɾ��ԭʵ���и���Ƭ����
		vector<int>::iterator iter = find(instance_old.insFaces.begin(), instance_old.insFaces.end(), faceIndex);
		if (iter != instance_old.insFaces.end())
		{
			instance_old.insFaces.erase(iter);
			return true;
		}
		else
		{
			return false;
		}
	}
}

//ɾ������Ƭ����
bool LabelInfo::DeleteMesh(vector<int> faceIndices)
{
	bool isOk = true;
	for (size_t fi = 0; fi < faceIndices.size(); fi++)
	{
		int faceIndex = faceIndices[fi];

		isOk = DeleteMesh(faceIndex);
	}
	return isOk;
}

