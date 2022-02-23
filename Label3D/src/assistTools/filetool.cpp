#include "filetool.h"

//int����ת��Ϊstring����
string filetool::ItoS(int nunm)
{
	stringstream str;
	str << nunm;
	return str.str();
}

//ȥ����β�ո�
string filetool::Trim(const string &s)
{
	string s1(s);
	if (s1.empty())
	{
		return s1;
	}
	s1.erase(0, s1.find_first_not_of(" "));
	s1.erase(s1.find_last_not_of(" ") + 1);
	return s1;
}

//��c�ַ��ָ��ַ���s,�ָ���������v��
void filetool::SplitString(const string& s, vector<string>& v, const string& c)
{
	string::size_type pos1, pos2;
	pos2 = s.find(c);
	pos1 = 0;

	while (string::npos != pos2)
	{
		v.push_back(s.substr(pos1, pos2 - pos1));
		pos1 = pos2 + c.size();
		pos2 = s.find(c, pos1);
	}
	if (pos1 != s.length())
		v.push_back(s.substr(pos1));
}

//��c�ַ��ָ��ַ���s,�ָ���������v��(ֻ������һ��c�ַ�����c�ַ��ָ��2��)
void filetool::SplitStringTo2(const string& s, vector<string>& v, const string& c)
{
	string::size_type pos1, pos2;
	pos2 = s.find(c);
	pos1 = 0;

	if (s.length() != 0 && string::npos != pos2)
	{
		v.push_back(s.substr(pos1, pos2 - pos1));
		pos1 = pos2 + c.size();
	}
	if (pos1 != s.length())
		v.push_back(s.substr(pos1));
}

//��ȡ�ļ���׺��
string filetool::GetFileSuffixName(const string& fileName)
{
	string suffixName = "";
	vector<string> splitedStrs;
	SplitStringTo2(fileName, splitedStrs, ".");
	if (splitedStrs.size() == 2)
	{
		suffixName = splitedStrs[1];
	}
	return suffixName;
}

//��ȡ�ļ���(��������׺)
string filetool::GetFileNameNoSuffix(const string& fileName)
{
	string fileNameNosuffix = "";
	vector<string> splitedStrs;
	SplitStringTo2(fileName, splitedStrs, ".");
	if (splitedStrs.size() == 2)
	{
		fileNameNosuffix = splitedStrs[0];
	}
	return fileNameNosuffix;
}


//ֻ��ȡĳ����·���µĵ�ǰ�ļ�����
void filetool::GetJustCurrentDir(const string& path, vector<string>& files)
{
	//�ļ���� 
	long  hFile = 0;
	//�ļ���Ϣ 
	struct _finddata_t fileinfo;
	string p;
	if ((hFile = _findfirst(p.assign(path).append("\\*").c_str(), &fileinfo)) != -1)
	{
		do
		{
			if ((fileinfo.attrib & _A_SUBDIR))
			{
				if (strcmp(fileinfo.name, ".") != 0 && strcmp(fileinfo.name, "..") != 0)
				{
					files.push_back(fileinfo.name);
					//files.push_back(p.assign(path).append("\\").append(fileinfo.name) );
				}

			}
		} while (_findnext(hFile, &fileinfo) == 0);
		_findclose(hFile);
	}
}

//��ȡ�����ļ����������ļ���
void filetool::GetJustCurrentFile(const string& path, vector<string>& files)
{
	//�ļ���� 
	long long  hFile = 0;
	//�ļ���Ϣ 
	struct _finddata_t fileinfo;
	string p;
	if ((hFile = _findfirst(p.assign(path).append("\\*").c_str(), &fileinfo)) != -1)
	{
		do
		{
			if ((fileinfo.attrib & _A_SUBDIR))
			{
				;
			}
			else
			{
				files.push_back(fileinfo.name);
				//files.push_back(p.assign(path).append("\\").append(fileinfo.name) ); 
			}
		} while (_findnext(hFile, &fileinfo) == 0);
		_findclose(hFile);
	}
}

//��ȡtxt�ļ�������
vector<int> filetool::ReadTxtFileInt(const string& fileName)
{
	vector<int> data;

	ifstream fin(fileName);   //��ȡ�ļ�
	if (!fin.is_open())
	{
		cout << "δ�ɹ����ļ�" << endl;
		return data;
	}
	int a;
	string temp;
	while (getline(fin, temp))
	{
		a = atoi(temp.c_str());
		data.push_back(a);
	}
	fin.close();       //�ر��ļ�

	return data;
}

vector<float> filetool::ReadTxtFileFloat(const string& fileName)
{
	vector<float> data;

	ifstream fin(fileName);   //��ȡ�ļ�
	if (!fin.is_open())
	{
		cout << "δ�ɹ����ļ�" << endl;
		return data;
	}
	float a;
	string temp;
	while (getline(fin, temp))
	{
		a = atof(temp.c_str());
		data.push_back(a);
	}
	fin.close();       //�ر��ļ�

	return data;
}

//��ȡ���ո�ֿ���int����txt����
vector<int> filetool::ReadTxtFileInt_Space(const string& fileName)
{
	vector<int> data;

	ifstream fin(fileName);   //��ȡ�ļ�
	if (!fin.is_open())
	{
		cout << "δ�ɹ����ļ�" << endl;
		return data;
	}

	int temp_i;
	string temp;
	while (!fin.eof())
	{
		getline(fin, temp); //һ�ζ�ȡһ������
		stringstream stringin(temp); //ʹ�ô���ʵ�ֶ�string�������������
		while (stringin >> temp_i) {      //���ո�һ�ζ�ȡһ�����ݴ���feat_onePoint 
			data.push_back(temp_i); //�洢ÿ�а��ո�ֿ������� 
		}
	}
	fin.close();       //�ر��ļ�
	return data;
}

//д��txt�ļ�
bool filetool::WriteTXTFileInt(const vector<int>& data, const string& fileName)
{
	try
	{
		ofstream fout(fileName);     //
		for (int i = 0; i < data.size(); i++)
		{
			fout << data[i] << endl;  //��������ֵд���ļ�
		}
		fout.close();
		return true;
	}
	catch (const std::exception&)
	{
		return false;
	}
}

bool filetool::WriteTXTFileFloat(const vector<float>& data, const string& fileName)
{
	try
	{
		ofstream fout(fileName);     //
		for (int i = 0; i < data.size(); i++)
		{
			fout << data[i] << endl;  //��������ֵд���ļ�
		}
		fout.close();
		return true;
	}
	catch (const std::exception&)
	{
		return false;
	}
}

//��ȡ��������
vector<Vector3d> filetool::ReadPointsTxt(const string &fileName)
{
	vector<Vector3d> data;

	ifstream fin(fileName);   //��ȡ�ļ�
	if (!fin.is_open())
	{
		cout << "δ�ɹ����ļ�" << endl;
		return data;
	}
	double x, y, z;
	string temp;
	while (getline(fin, temp))
	{
		vector<string> ps;
		SplitString(temp, ps, " ");
		if (ps.size() < 3)
		{
			continue;
		}
		x = atof(ps[0].c_str());
		y = atof(ps[1].c_str());
		z = atof(ps[2].c_str());
		data.push_back(Vector3d(x, y, z));
	}
	fin.close();       //�ر��ļ�

	return data;
}

//����������д��txt�ļ�
bool filetool::WriteTXTFileVec3d(const vector<Vector3d>& data, const string& fileName)
{
	try
	{
		ofstream fout(fileName);
		fout.setf(ios::fixed, ios::floatfield);  // �趨Ϊ fixed ģʽ����С�����ʾ������
		fout.precision(2);  // ���þ��� 2
		for (int i = 0; i < data.size(); i++)
		{
			fout << data[i][0] << " " << data[i][1] << " " << data[i][2] << " " << endl;  //��������ֵд���ļ�
		}
		fout.close();
		return true;
	}
	catch (const std::exception&)
	{
		return false;
	}
}

//��ȡ��Ŀ�ļ�
SProjectInfo filetool::ReadPorjectInfo(const string& projectFile)
{
	SProjectInfo projectInfo;

	ifstream ifs(projectFile);
	Json::CharReaderBuilder readerbuilder;
	//Json::Reader reader;
	Json::Value root;
	JSONCPP_STRING errs;
	bool ok = Json::parseFromStream(readerbuilder, ifs, &root, &errs);
	if (ok)
	{
		projectInfo.projectName = root["projectName"].asString();
		projectInfo.projectRoot = root["projectRoot"].asString();
		projectInfo.modelFile = root["modelFile"].asString();
		projectInfo.projectType = root["projectType"].asString();
		projectInfo.hasTextureMaterial = root["hasTextureMaterial"].asBool();
		projectInfo.minMaskScore = root["minMaskScore"].asInt();
	}
	return projectInfo;
}

//��ģ������Ƭ�ϵ����ͼ����Ϊ�Ҷ�ͼ
void filetool::WModelImgDeepMapsToGray(cv::Mat &deepImage, string &fileName)
{
	vector<int> compression_params;
	compression_params.push_back(CV_IMWRITE_JPEG_QUALITY);  //ѡ��jpeg
	compression_params.push_back(100); //�����������Ҫ��ͼƬ����

	try {
		cv::imwrite(fileName, deepImage, compression_params);
		//cv::imshow("Created JPG", jpgImage);
		//fprintf(stdout, "Finish creating image with alpha channel.\n");
		//cv::waitKey(0);
	}
	catch (std::runtime_error& ex) {
		fprintf(stderr, "Error when converting to Gray: %s\n", ex.what());
		return;
	}
}

//��Mat���ݱ���Ϊtxt
void filetool::WMatToTxt(const cv::Mat &mat, const string &fileName, int effiBit)
{
	FILE* file;
	file = fopen(fileName.c_str(), "w");
	if (!file) {
		return;
	}

	int dataType = mat.type();
	if (dataType == CV_64F)	//CV_64F
	{
		string _format;
		string _formatEnd;
		if (effiBit < 0)
		{
			_formatEnd = "%f";
			_format = "%f\n";
		}
		else if (effiBit == 0)
		{
			_formatEnd = "%d";
			_format = "%d\n";
		}
		else
		{
			_formatEnd = "%." + ItoS(effiBit) + "f";
			_format = "%." + ItoS(effiBit) + "f\n";
		}

		for (unsigned int nrow = 0; nrow < mat.rows; nrow++)
		{
			for (unsigned int ncol = 0; ncol < mat.cols; ncol++)
			{
				double val = mat.at<double>(nrow, ncol);
				if (nrow == (mat.rows - 1) && ncol == (mat.cols - 1))
				{
					fprintf(file, _formatEnd.c_str(), val);
				}
				else
				{
					fprintf(file, _format.c_str(), val);
				}
			}
		}
	}
	else if (dataType == CV_8U)
	{
		for (unsigned int nrow = 0; nrow < mat.rows; nrow++)
		{
			for (unsigned int ncol = 0; ncol < mat.cols; ncol++)
			{
				char val = mat.at<char>(nrow, ncol);
				if (nrow == (mat.rows - 1) && ncol == (mat.cols - 1))
				{
					fprintf(file, "%d", val);
				}
				else
				{
					fprintf(file, "%d\n", val);
				}
			}
		}
	}

	fclose(file);
}

//��Mat���ݱ���ΪXML
void filetool::WMatToXML(const cv::Mat &mat, const string &fileName)
{
	cv::FileStorage cvfs(fileName, cv::FileStorage::WRITE);
	cvfs << "depthMap" << mat;
	//write(cvfs, "mat", deepImage);
	cvfs.release();
}

//��ȡ�������
cv::Mat filetool::RMatDataFromTxtDouble(string &fileName, int img_w, int img_h)
{
	cv::Mat matData(img_w, img_h, CV_64F);
	ifstream fin(fileName);   //��ȡ�ļ�
	if (!fin.is_open())
	{
		cout << "δ�ɹ����ļ�" << endl;
		fin.close();
		return matData;
	}
	double a;
	string temp;
	int index = 0;
	int maxIndex = img_w*img_h;
	while (getline(fin, temp))
	{
		a = atof(temp.c_str());
		if (index < maxIndex)
		{
			((double*)matData.data)[index] = a;
			index++;
		}
	}
	fin.close();       //�ر��ļ�
	return matData;
}

cv::Mat filetool::RMatDataFromTxtDouble2(string &fileName, int img_w, int img_h)
{
	cv::Mat matData(img_w, img_h, CV_64F);
	FILE *fp;
	fp = fopen(fileName.c_str(), "r");
	if (!fp)
	{
		cout << "δ�ɹ����ļ�" << endl;
		fclose(fp);
		return matData;
	}
	int index = 0;
	double a;
	int ch, i = 0, n = 0;
	char str[50];
	int maxIndex = img_w*img_h;
	while ((ch = fgetc(fp)) != EOF) 
	{
		if (ch == '\t' || ch == '\n') 
		{
			if (i > 0)
			{
				str[i++] = '\0';
				a = atof(str);
				if (index < maxIndex)
				{
					((double*)matData.data)[index] = a;
					index++;
				}
				i = 0;
				++n;
			}
		}
		else str[i++] = ch;
	}
	fclose(fp);
	return matData;
}

//�����������б����Ϊ�ַ���
string filetool::EncodePIndex(const vector<int> &points)
{
	string s = "";
	int pSize = points.size();
	for (int i = 0; i < pSize; i++)
	{
		stringstream ss;
		ss << points[i];
		string s1 = ss.str();

		s = s + s1 + " ";
	}

	return s;
}

//���ַ�������Ϊ���������б�
vector<int> filetool::DecodePIndexString(string &pString)
{
	vector<int> pList;
	vector<string> sList;

	SplitString(Trim(pString), sList, " ");
	for (int i = 0; i < sList.size(); i++)
	{
		pList.push_back(atoi(sList[i].c_str()));
	}

	return pList;
}

//�����������������ÿո�ֿ����ַ���
string EncodeFloatList(int count, const float* f)
{
	string s = "";
	for (int i = 0; i < count; i++)
	{
		stringstream ss;
		ss << f[i];
		string s1 = ss.str();

		s = s + s1 + " ";
	}

	return s;
}

//���ַ�������Ϊ����������
void DecodeFloatStr(const string &str, float* simiVector, int count)
{
	simiVector = new float[count];
	vector<string> sList;

	filetool::SplitString(filetool::Trim(str), sList, " ");
	for (int i = 0; i < sList.size(); i++)
	{
		simiVector[i] = (atoi(sList[i].c_str()));
	}
}

//����mask֮�����ƶȾ���
bool filetool::WMaskSimiMatrix(int maskCount, float** matrix,const string &fileName)
{
	//���ڵ�
	Json::Value root;
	root["MaskCount"] = maskCount;

	for (int i = 0; i < maskCount; i++)
	{
		Json::Value matrixRoot;
		string fstr = EncodeFloatList(maskCount, matrix[i]);
		matrixRoot = fstr;

		root["SimiMatrix"].append(matrixRoot);
	}

	Json::StreamWriterBuilder  builder;
	builder["commentStyle"] = "None";
	builder["indentation"] = "	";
	std::unique_ptr<Json::StreamWriter> writer(builder.newStreamWriter());

	std::ofstream ofs;
	ofs.open(fileName, std::ios::app);
	writer->write(root, &ofs);
	ofs.close();
	return true;
}

//��ȡmask���ƶȾ���
float** filetool::RMaskSimiMatrix(const string &fileName, int &maskCount)
{
	float **simiMatrix = new float*[maskCount];
	ifstream ifs(fileName);
	Json::CharReaderBuilder readerbuilder;
	//Json::Reader reader;
	Json::Value root;
	JSONCPP_STRING errs;
	bool ok = Json::parseFromStream(readerbuilder, ifs, &root, &errs);
	if (ok)
	{
		maskCount = root["MaskCount"].asInt();

		simiMatrix = new float*[maskCount];
		for (int i = 0; i < maskCount; i++)
		{
			simiMatrix[i] = new float[maskCount];
		}

		Json::Value matrixRoot = root["SimiMatrix"];
		for (int i = 0; i < maskCount; i++)
		{
			string simiStr = matrixRoot[i].asString();
			stringstream input(simiStr);
			//���������result�У�������simiMatrix�� 
			float result = 0;
			int j = 0;
			while (input >> result)
			{
				simiMatrix[i][j] = result;
				j++;
			}

			//DecodeFloatStr(simiStr, simiMatrix[i], maskCount);
		}
	}
	return simiMatrix;
}
