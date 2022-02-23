#include "panoptic.h"
#include <json/json.h>
#include <iostream>
#include <io.h>
#include <fstream>
#include <sstream>
#include <memory> 
#include <time.h>
#include <string>
#include <iostream>
#include <fstream>

////读取全景分割标注文件
//panopticlabel::PanopticLabel panopticlabel::ReadPanopLabelFile(const string& fileName)
//{
//	panopticlabel::PanopticLabel panopticLabel;
//
//	std::ifstream ifs(fileName);
//	Json::CharReaderBuilder readerbuilder;
//	//Json::Reader reader;
//	Json::Value root;
//	JSONCPP_STRING errs;
//	bool ok = Json::parseFromStream(readerbuilder, ifs, &root, &errs);
//	if (ok)
//	{
//		Json::Value& infoNode = root["info"];
//		panopticLabel.info.contributor = infoNode["contributor"].asString();
//		panopticLabel.info.date_created = infoNode["date_created"].asString();
//		panopticLabel.info.description = infoNode["description"].asString();
//		panopticLabel.info.url = infoNode["url"].asString();
//		panopticLabel.info.version = infoNode["version"].asString();
//		panopticLabel.info.year = infoNode["year"].asInt();
//
//		Json::Value& licensesNode = root["licenses"];
//		int num_license = licensesNode.size();
//		panopticLabel.licenses.resize(num_license);
//		for (int i = 0; i < num_license; i++)
//		{
//			panopticLabel.licenses[i].id = licensesNode[i]["id"].asInt();
//			panopticLabel.licenses[i].name = licensesNode[i]["name"].asString();
//			panopticLabel.licenses[i].url = licensesNode[i]["url"].asString();
//		}
//
//		Json::Value& imagesNode = root["images"];
//		int num_image = imagesNode.size();
//		panopticLabel.images.resize(num_image);
//		for (int i = 0; i < num_image; i++)
//		{
//			panopticLabel.images[i].coco_url = imagesNode[i]["coco_url"].asString();
//			panopticLabel.images[i].date_captured = imagesNode[i]["date_captured"].asString();
//			panopticLabel.images[i].file_name = imagesNode[i]["file_name"].asString();
//			panopticLabel.images[i].flickr_url = imagesNode[i]["flickr_url"].asString();
//			panopticLabel.images[i].height = imagesNode[i]["height"].asInt();
//			panopticLabel.images[i].id = imagesNode[i]["id"].asInt();
//			panopticLabel.images[i].license = imagesNode[i]["license"].asInt();
//			panopticLabel.images[i].width = imagesNode[i]["width"].asInt();
//		}
//
//		Json::Value& annotationsNode = root["annotations"];
//		int num_annotation = annotationsNode.size();
//		panopticLabel.annotations.resize(num_annotation);
//		for (int i = 0; i < num_annotation; i++)
//		{
//			panopticLabel.annotations[i].file_name = annotationsNode[i]["file_name"].asString();
//			panopticLabel.annotations[i].image_id = annotationsNode[i]["image_id"].asInt();
//			Json::Value& segments_infoNode = annotationsNode[i]["segments_info"];
//			int num_segment = segments_infoNode.size();
//			panopticLabel.annotations[i].segments_info.resize(num_segment);
//			for (int j = 0; j < num_segment; j++)
//			{
//				panopticLabel.annotations[i].segments_info[j].area = segments_infoNode[j]["area"].asInt();
//				panopticLabel.annotations[i].segments_info[j].category_id = segments_infoNode[j]["category_id"].asInt();
//				panopticLabel.annotations[i].segments_info[j].id = segments_infoNode[j]["id"].asInt();
//				panopticLabel.annotations[i].segments_info[j].iscrowd = segments_infoNode[j]["id"].asInt();
//				for (int k = 0; k < 4; k++)
//				{
//					panopticLabel.annotations[i].segments_info[j].bbox[k] = segments_infoNode[j]["bbox"][k].asInt();
//				}
//			}
//		}
//
//		Json::Value& categoriesNode = root["categories"];
//		int num_category = categoriesNode.size();
//		panopticLabel.categories.resize(num_category);
//		for (int i = 0; i < num_category; i++)
//		{
//			panopticLabel.categories[i].id = categoriesNode[i]["id"].asInt();
//			panopticLabel.categories[i].isthing = categoriesNode[i]["isthing"].asInt();
//			panopticLabel.categories[i].name = categoriesNode[i]["name"].asString();
//			panopticLabel.categories[i].supercategory = categoriesNode[i]["supercategory"].asString();
//		}
//	}
//	ifs.close();
//
//	return panopticLabel;
//}


panopticlabel::PanopticLabel::PanopticLabel()
{
	this->info.contributor = "XYH";
	this->info.date_created = time(NULL);
	this->info.description = "";
	this->info.url = "";
	this->info.version = "test";
	this->info.year = 2020;
	LoadCategories();
}

//读取全景分割标注文件
bool panopticlabel::PanopticLabel::ReadPanopLabelFile(const string& fileName)
{
	std::ifstream ifs(fileName);
	Json::CharReaderBuilder readerbuilder;
	//Json::Reader reader;
	Json::Value root;
	JSONCPP_STRING errs;
	bool ok = Json::parseFromStream(readerbuilder, ifs, &root, &errs);
	if (ok)
	{
		Json::Value& infoNode = root["info"];
		this->info.contributor = infoNode["contributor"].asString();
		this->info.date_created = infoNode["date_created"].asString();
		this->info.description = infoNode["description"].asString();
		this->info.url = infoNode["url"].asString();
		this->info.version = infoNode["version"].asString();
		this->info.year = infoNode["year"].asInt();

		Json::Value& licensesNode = root["licenses"];
		int num_license = licensesNode.size();
		this->licenses.resize(num_license);
		for (int i = 0; i < num_license; i++)
		{
			this->licenses[i].id = licensesNode[i]["id"].asInt();
			this->licenses[i].name = licensesNode[i]["name"].asString();
			this->licenses[i].url = licensesNode[i]["url"].asString();
		}

		Json::Value& imagesNode = root["images"];
		int num_image = imagesNode.size();
		this->images.resize(num_image);
		for (int i = 0; i < num_image; i++)
		{
			this->images[i].coco_url = imagesNode[i]["coco_url"].asString();
			this->images[i].date_captured = imagesNode[i]["date_captured"].asString();
			this->images[i].file_name = imagesNode[i]["file_name"].asString();
			this->images[i].flickr_url = imagesNode[i]["flickr_url"].asString();
			this->images[i].height = imagesNode[i]["height"].asInt();
			this->images[i].id = imagesNode[i]["id"].asInt();
			this->images[i].license = imagesNode[i]["license"].asInt();
			this->images[i].width = imagesNode[i]["width"].asInt();
		}

		Json::Value& annotationsNode = root["annotations"];
		int num_annotation = annotationsNode.size();
		this->annotations.resize(num_annotation);
		for (int i = 0; i < num_annotation; i++)
		{
			this->annotations[i].file_name = annotationsNode[i]["file_name"].asString();
			this->annotations[i].image_id = annotationsNode[i]["image_id"].asInt();
			Json::Value& segments_infoNode = annotationsNode[i]["segments_info"];
			int num_segment = segments_infoNode.size();
			this->annotations[i].segments_info.resize(num_segment);
			for (int j = 0; j < num_segment; j++)
			{
				this->annotations[i].segments_info[j].area = segments_infoNode[j]["area"].asInt();
				this->annotations[i].segments_info[j].category_id = segments_infoNode[j]["category_id"].asInt();
				this->annotations[i].segments_info[j].id = segments_infoNode[j]["id"].asInt();
				this->annotations[i].segments_info[j].iscrowd = segments_infoNode[j]["id"].asInt();
				for (int k = 0; k < 4; k++)
				{
					this->annotations[i].segments_info[j].bbox[k] = segments_infoNode[j]["bbox"][k].asInt();
				}
			}
		}

		Json::Value& categoriesNode = root["categories"];
		int num_category = categoriesNode.size();
		this->categories.resize(num_category);
		for (int i = 0; i < num_category; i++)
		{
			this->categories[i].id = categoriesNode[i]["id"].asInt();
			this->categories[i].isthing = categoriesNode[i]["isthing"].asInt();
			this->categories[i].name = categoriesNode[i]["name"].asString();
			this->categories[i].supercategory = categoriesNode[i]["supercategory"].asString();
		}
	}
	ifs.close();

	return true;
}

//保存为文件
bool panopticlabel::PanopticLabel::WriteToFile(const string& fileName)
{
	//根节点
	Json::Value root;

	int num_annotations = this->annotations.size();
	for (int i = 0; i < num_annotations; i++)
	{
		Json::Value annotationNode;
		int num_segments = this->annotations[i].segments_info.size();
		annotationNode["file_name"] = this->annotations[i].file_name;
		annotationNode["image_id"] = this->annotations[i].image_id;
		annotationNode["segments_info"] = Json::arrayValue;
		for (int j = 0; j < num_segments; j++)
		{
			Json::Value segInfoNode;
			segInfoNode["id"] = this->annotations[i].segments_info[j].id;
			segInfoNode["iscrowd"] = this->annotations[i].segments_info[j].iscrowd;
			segInfoNode["category_id"] = this->annotations[i].segments_info[j].category_id;
			segInfoNode["area"] = this->annotations[i].segments_info[j].area;
			segInfoNode["bbox"] = Json::arrayValue;
			for (int k = 0; k < 4; k++)
			{
				segInfoNode["bbox"].append(this->annotations[i].segments_info[j].bbox[k]);
			}
			annotationNode["segments_info"].append(segInfoNode);
		}
		root["annotations"].append(annotationNode);
	}

	int num_categories = this->categories.size();
	for (int i = 0; i < num_categories; i++)
	{
		Json::Value categoryNode;
		categoryNode["id"] = this->categories[i].id;
		categoryNode["isthing"] = this->categories[i].isthing;
		categoryNode["name"] = this->categories[i].name;
		categoryNode["supercategory"] = this->categories[i].supercategory;
		for (int k = 0; k < 3; k++)
		{
			categoryNode["color"].append(this->categories[i].color[k]);
		}

		root["categories"].append(categoryNode);
	}

	int num_images = this->images.size();
	for (int i = 0; i < num_images; i++)
	{
		Json::Value imageNode;
		imageNode["file_name"] = this->images[i].file_name;
		imageNode["height"] = this->images[i].height;
		imageNode["id"] = this->images[i].id;
		imageNode["width"] = this->images[i].width;
		root["images"].append(imageNode);
	}

	Json::StreamWriterBuilder  builder;
	builder["commentStyle"] = "None";
	builder["indentation"] = "	";
	std::unique_ptr<Json::StreamWriter> writer(builder.newStreamWriter());
	try
	{
		std::ofstream ofs;
		ofs.open(fileName);		//覆盖保存
		writer->write(root, &ofs);
		ofs.close();
		return true;
	}
	catch (const std::exception& e)
	{
		return false;
	}

	return true;
}

//加载类别
bool panopticlabel::PanopticLabel::LoadCategories()
{
	string categoryFile = "data/label3d_categories.json";

	std::ifstream ifs(categoryFile);
	Json::CharReaderBuilder readerbuilder;
	Json::Value root;
	JSONCPP_STRING errs;
	bool ok = Json::parseFromStream(readerbuilder, ifs, &root, &errs);
	ifs.close();
	if (ok)
	{
		int num_categories = root["categories"].size();
		for (int i = 0; i < num_categories; i++)
		{
			Category cate;
			cate.id = root["categories"][i]["id"].asInt();
			cate.isthing = root["categories"][i]["isthing"].asInt();
			cate.name = root["categories"][i]["name"].asString();
			cate.supercategory = root["categories"][i]["supercategory"].asString();
			for (int k = 0; k < 3; k++)
			{
				cate.color[k] = root["categories"][i]["color"][k].asInt();
			}
			this->categories.push_back(cate);
		}

		return true;
	}
	return false;

}

//提取json文件中category字段并保存为.json文件
bool panopticlabel::ExtractCategoryJson(const string& srcFile, const string& dstFile)
{
	panopticlabel::PanopticLabel panopticLabel();

	std::ifstream ifs(srcFile);
	Json::CharReaderBuilder readerbuilder;
	//Json::Reader reader;
	Json::Value root;
	JSONCPP_STRING errs;
	bool ok = Json::parseFromStream(readerbuilder, ifs, &root, &errs);
	if (ok)
	{
		Json::Value& categoriesNode = root["categories"];

		Json::StreamWriterBuilder  builder;
		builder["commentStyle"] = "None";
		builder["indentation"] = "	";
		std::unique_ptr<Json::StreamWriter> writer(builder.newStreamWriter());

		try
		{
			Json::Value cateRoot;
			cateRoot["categories"] = categoriesNode;
			std::ofstream ofs;
			//ofs.open(fileName, std::ios::app);	//不覆盖，在文件结尾写入
			ofs.open(dstFile);		//覆盖保存
			writer->write(cateRoot, &ofs);
			ofs.close();
			return true;
		}
		catch (const std::exception& e)
		{
			ifs.close();
			return false;
		}
		return true;
	}

	ifs.close();
	return false;
}

panopticlabel::Annotation panopticlabel::ReadAnnotation(const string& fileName)
{
	panopticlabel::Annotation annotation;
	annotation.file_name = "";
	annotation.image_id = 0;

	std::ifstream ifs(fileName);
	Json::CharReaderBuilder readerbuilder;
	//Json::Reader reader;
	Json::Value root;
	JSONCPP_STRING errs;
	bool ok = Json::parseFromStream(readerbuilder, ifs, &root, &errs);
	if (ok)
	{
		Json::Value& annotationNode = root["annotations"];
		if (annotationNode.size() > 0)
		{
			Json::Value& segInfoNode = annotationNode[0]["segments_info"];
			int num_SegInfo = segInfoNode.size();
			for (int i = 0; i < num_SegInfo; i++)
			{
				panopticlabel::SegmentInfo segInfo;
				segInfo.area = segInfoNode[i]["area"].asInt();
				for (int j = 0; j < 4; j++)
				{
					segInfo.bbox[j] = segInfoNode[i]["bbox"][j].asInt();
				}
				segInfo.category_id = segInfoNode[i]["category_id"].asInt();
				segInfo.id = segInfoNode[i]["id"].asInt();
				segInfo.iscrowd = segInfoNode[i]["iscrowd"].asInt();

				annotation.segments_info.push_back(segInfo);
			}
		}
	}

	ifs.close();
	return annotation;
}