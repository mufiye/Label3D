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
		//读取全景分割标注文件
		bool ReadPanopLabelFile(const string& fileName);

		//保存为文件
		bool WriteToFile(const string& fileName);

		//加载类别
		bool LoadCategories();

	};

	////读取全景分割标注文件
	//PanopticLabel ReadPanopLabelFile(const string& fileName);

	//提取json文件中category字段并保存为.json文件
	bool ExtractCategoryJson(const string& srcFile, const string& dstFile);

	Annotation ReadAnnotation(const string& fileName);
}
