#pragma once
#include <string>
#include <vector>

using std::string;
using std::vector;

namespace panopticlabel
{
	struct Info
	{
		int year;
		string version;
		string description;
		string contributor;
		string url;
		string date_created;
	};

	struct License
	{
		int id;
		string name;
		string url;
	};

	struct Image
	{
		int id;
		int width;
		int height;
		string file_name;
		int license;
		string flickr_url;
		string coco_url;
		string date_captured;
	};

	struct Category
	{
		int id;
		string name;
		string supercategory;
		int isthing;	//1 or 0
		int color[3];
	};

	struct SegmentInfo
	{
		int id;
		int category_id;
		int area;
		int bbox[4];	//[x, y, width, height]
		int iscrowd;	//1 or 0
	};

	struct Annotation
	{
		int image_id;
		string file_name;
		vector<SegmentInfo> segments_info;
	};

	class PanopticLabel
	{
	public:
		PanopticLabel();
	public:
		Info info;
		vector<License> licenses;
		vector<Image> images;
		vector<Annotation> annotations;
		vector<Category> categories;

	public:
		//��ȡȫ���ָ��ע�ļ�
		bool ReadPanopLabelFile(const string& fileName);

		//����Ϊ�ļ�
		bool WriteToFile(const string& fileName);

		//�������
		bool LoadCategories();

	};

	////��ȡȫ���ָ��ע�ļ�
	//PanopticLabel ReadPanopLabelFile(const string& fileName);

	//��ȡjson�ļ���category�ֶβ�����Ϊ.json�ļ�
	bool ExtractCategoryJson(const string& srcFile, const string& dstFile);

	Annotation ReadAnnotation(const string& fileName);
}
