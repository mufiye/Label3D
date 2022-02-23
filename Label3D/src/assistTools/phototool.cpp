#include "phototool.h"
using namespace phototool;

//读取场景照片对应的OpenGL相机参数Json文件
SGlCameraParam phototool::RSPOpenGLCParamJson(const string& fileName)
{
	SGlCameraParam camaParam;

	ifstream ifs(fileName);
	Json::CharReaderBuilder readerbuilder;
	//Json::Reader reader;
	Json::Value root;
	JSONCPP_STRING errs;
	bool ok = Json::parseFromStream(readerbuilder, ifs, &root, &errs);
	if (ok)
	{
		camaParam.picName = root["photoName"].asString();
		camaParam.imageWidth = root["imageWidth"].asInt();
		camaParam.imageHeight = root["imageHeight"].asInt();
		for (int i = 0; i < 16; i++)
		{
			camaParam.modelMatrix[i] = root["modelMat"][i].asDouble();
		}
		for (int i = 0; i < 16; i++)
		{
			camaParam.projMatrix[i] = root["projMat"][i].asDouble();
		}
		for (int i = 0; i < 4; i++)
		{
			camaParam.viewport[i] = root["viewMat"][i].asInt();
		}
	}

	return camaParam;
};

//读取场景照片对应的OpenGL相机参数Json文件
vector<SGlCameraParam> phototool::RSPOpenGLCParamInfosJson(const string& fileName)
{
	vector<SGlCameraParam> photoInfos;

	ifstream ifs(fileName);
	Json::CharReaderBuilder readerbuilder;
	//Json::Reader reader;
	Json::Value root;
	JSONCPP_STRING errs;
	bool ok = Json::parseFromStream(readerbuilder, ifs, &root, &errs);
	if (ok)
	{
		int photoCount = root["photos"].size();
		for (int pi = 0; pi < photoCount; pi++)
		{
			SGlCameraParam pInfo;
			pInfo.id = root["photos"][pi]["photoId"].asInt();
			pInfo.picName = root["photos"][pi]["photoName"].asString();
			pInfo.imageWidth = root["photos"][pi]["imageWidth"].asInt();
			pInfo.imageHeight = root["photos"][pi]["imageHeight"].asInt();

			for (int i = 0; i < 16; i++)
			{
				pInfo.modelMatrix[i] = root["photos"][pi]["modelMat"][i].asDouble();
			}
			for (int i = 0; i < 16; i++)
			{
				pInfo.projMatrix[i] = root["photos"][pi]["projMat"][i].asDouble();
			}
			for (int i = 0; i < 4; i++)
			{
				pInfo.viewport[i] = root["photos"][pi]["viewMat"][i].asInt();
			}

			photoInfos.push_back(pInfo);
		}
	}

	return photoInfos;
}

//将场景照片对应的OpenGL相机参数保存为Json文件
void phototool::WSPOpenGLCParamJson(const string &fileName, const SGlCameraParam& cameParam)
{
	//根节点
	Json::Value root;
	root["photoName"] = cameParam.picName;
	root["imageWidth"] = cameParam.imageWidth;
	root["imageHeight"] = cameParam.imageHeight;
	for (size_t i = 0; i < 16; i++)
	{
		root["modelMat"].append(cameParam.modelMatrix[i]);
	}
	for (size_t i = 0; i < 16; i++)
	{
		root["projMat"].append(cameParam.projMatrix[i]);
	}
	for (size_t i = 0; i < 4; i++)
	{
		root["viewMat"].append(cameParam.viewport[i]);
	}


	Json::StreamWriterBuilder  builder;
	builder["commentStyle"] = "None";
	builder["indentation"] = "	";
	std::unique_ptr<Json::StreamWriter> writer(builder.newStreamWriter());

	std::ofstream ofs;
	ofs.open(fileName, std::ios::app);
	writer->write(root, &ofs);
	ofs.close();
}

//读取XML文件中photoGroup信息，包括相机参数和对应的照片信息
vector<SPhotoGroup> phototool::RPhotoGroupFromXML(const string &fileName)
{
	vector<SPhotoGroup> pGroups;
	// 读入XML文件
	TiXmlDocument doc(fileName.c_str());
	if (!doc.LoadFile())
		return pGroups;  // 如果无法读取文件，则返回

	TiXmlHandle hDoc(&doc);         // hDoc是&doc指向的对象
	TiXmlElement* pElem;            // 指向元素的指针
	pElem = hDoc.FirstChildElement().Element(); //指向根节点
	TiXmlHandle hRoot(pElem);       // hRoot是根节点


	TiXmlNode* pGroupNodeElem = hRoot.FirstChild("Block").FirstChild("Photogroups").FirstChild("Photogroup").Node(); //当前指向了第一个Photogroup节点
	for (pGroupNodeElem; pGroupNodeElem; pGroupNodeElem = pGroupNodeElem->NextSibling())
	{
		SPhotoGroup p_group;
		//读取相机参数camera_param;
		{
			SCameraParam c_param;
			// 读取x,y，它们放在network->nodes->node节点中
			TiXmlElement* nodeElem = pGroupNodeElem->ToElement(); //当前指向了photo节点

			TiXmlHandle node(nodeElem);  // nodeElem所指向的节点
			c_param.imageDimensions_Width = atoi(node.FirstChild("ImageDimensions").FirstChild("Width").Element()->GetText());
			c_param.imageDimensions_Height = atoi(node.FirstChild("ImageDimensions").FirstChild("Height").Element()->GetText());
			c_param.cameraModelType = node.FirstChild("CameraModelType").Element()->GetText();
			c_param.focalLength = atof(node.FirstChild("FocalLength").Element()->GetText());
			c_param.sensorSize = atof(node.FirstChild("SensorSize").Element()->GetText());
			c_param.cameraOrientation = node.FirstChild("CameraOrientation").Element()->GetText();
			c_param.principalPoint_X = atof(node.FirstChild("PrincipalPoint").FirstChild("x").Element()->GetText());
			c_param.principalPoint_Y = atof(node.FirstChild("PrincipalPoint").FirstChild("y").Element()->GetText());
			c_param.distortion_K1 = atof(node.FirstChild("Distortion").FirstChild("K1").Element()->GetText());
			c_param.distortion_K2 = atof(node.FirstChild("Distortion").FirstChild("K2").Element()->GetText());
			c_param.distortion_K3 = atof(node.FirstChild("Distortion").FirstChild("K3").Element()->GetText());
			c_param.distortion_P1 = atof(node.FirstChild("Distortion").FirstChild("P1").Element()->GetText());
			c_param.distortion_P2 = atof(node.FirstChild("Distortion").FirstChild("P2").Element()->GetText());
			c_param.aspectRatio = atof(node.FirstChild("AspectRatio").Element()->GetText());
			c_param.skew = atof(node.FirstChild("Skew").Element()->GetText());
			c_param.focalLength_pixel = c_param.imageDimensions_Width*c_param.focalLength / c_param.sensorSize;

			p_group.cameraParam = c_param;
		}
		//读取照片信息
		{
			vector<SPhotoInfo> photo_camera_infos;

			TiXmlElement* nodeElem = pGroupNodeElem->FirstChild("Photo")->ToElement();
			for (nodeElem; nodeElem; nodeElem = nodeElem->NextSiblingElement())
			{
				SPhotoInfo pic;

				// 挨个读取node节点的信息
				TiXmlHandle node(nodeElem);  // nodeElem所指向的节点
				TiXmlElement* id_Elem = node.FirstChild("Id").Element();
				pic.id = atoi(id_Elem->GetText());
				TiXmlElement*  imagepath_Elem = node.FirstChild("ImagePath").Element();
				string photo_name_text = imagepath_Elem->GetText();
				pic.picName = photo_name_text.substr(photo_name_text.find_last_of('/') + 1);//获取文件后缀
				pic.photoType = pic.picName.substr(0, 2);

				TiXmlElement*  rotation_Elem = node.FirstChild("Pose").FirstChild("Rotation").Element();
				TiXmlHandle rota_node(rotation_Elem);
				TiXmlElement*  pitch_Elem = rota_node.FirstChild("Pitch").Element();
				pic.camera_pitch = atof(pitch_Elem->GetText());
				TiXmlElement*  yaw_Elem = rota_node.FirstChild("Yaw").Element();
				pic.camera_yaw = atof(yaw_Elem->GetText());
				TiXmlElement*  roll_Elem = rota_node.FirstChild("Roll").Element();
				pic.camera_roll = atof(roll_Elem->GetText());

				string exif_YawPitchRoll = node.FirstChild("ExifData").FirstChild("YawPitchRoll").Element()->GetText();
				vector<string> split_str;
				SplitString(exif_YawPitchRoll, split_str, " ");
				pic.yaw = atof(split_str[0].c_str());
				pic.pitch = atof(split_str[1].c_str());
				pic.roll = atof(split_str[2].c_str());


				TiXmlElement*  center_Elem = node.FirstChild("Pose").FirstChild("Center").Element();
				TiXmlHandle center_node(center_Elem);
				TiXmlElement*  x_Elem = center_node.FirstChild("x").Element();
				pic.x = atof(x_Elem->GetText());
				TiXmlElement*  y_Elem = center_node.FirstChild("y").Element();
				pic.y = atof(y_Elem->GetText());
				TiXmlElement*  z_Elem = center_node.FirstChild("z").Element();
				pic.z = atof(z_Elem->GetText());

				int start = pic.picName.find_last_of("(") + 1;
				int end = pic.picName.find_last_of(")");
				string number = pic.picName.substr(start, end - start);
				pic.picNumber = atoi(number.c_str());
				pic.photoType = pic.picName.substr(0, 2);

				photo_camera_infos.push_back(pic);
			}

			p_group.photoInfos = photo_camera_infos;
		}
		pGroups.push_back(p_group);
	}
	return pGroups;
}

//读取mask文件中照片列表(ifReadMask:是否读取mask信息)
vector<SPhotoInfo> phototool::RImagesFromMaskFile(const string &fileName, bool ifReadMask)
{
	vector<SPhotoInfo> imageList;
	ifstream ifs(fileName);
	Json::CharReaderBuilder readerbuilder;
	//Json::Reader reader;
	Json::Value root;
	JSONCPP_STRING errs;
	bool ok = Json::parseFromStream(readerbuilder, ifs, &root, &errs);
	if (ok)
	{
		//读取照片信息
		int imageSize = root["images"].size();
		imageList.resize(imageSize);
		for (int i = 0; i < imageSize; i++)
		{
			int id = root["images"][i]["id"].asInt();
			imageList[id - 1].id = id;
			imageList[id - 1].picName = root["images"][i]["file_name"].asString();
			imageList[id - 1].width = root["images"][i]["width"].asInt();
			imageList[id - 1].height = root["images"][i]["height"].asInt();
		}

		//读取照片Mask信息
		if (ifReadMask)
		{
			vector<int> imgMaskIndexNow(imageSize);	//记录当前图像Mask索引
			for (size_t i = 0; i < imageSize; i++)
			{
				imgMaskIndexNow[i] = 0;
			}

			Json::Value result_root = root["result"];
			int maskCount = result_root.size();
			for (int i = 0; i < maskCount; i++)
			{
				SMask sr;
				sr.allMaskId = i;
				sr.imageId = result_root[i]["image_id"].asInt();
				sr.imgMaskId = imgMaskIndexNow[sr.imageId - 1];
				imgMaskIndexNow[sr.imageId - 1]++;
				sr.imageName = imageList[sr.imageId - 1].picName;
				sr.categoryId = result_root[i]["category_id"].asInt();
				sr.score = result_root[i]["score"].asFloat();
				sr.h = result_root[i]["segmentation"]["size"][0].asInt();
				sr.w = result_root[i]["segmentation"]["size"][1].asInt();
				sr.segmentations = result_root[i]["segmentation"]["counts"].asString();
				sr.segmentations = result_root[i]["segmentation"]["counts"].asString();

				imageList[sr.imageId - 1].masks.push_back(sr);
			}
		}
	}

	return imageList;
}

//将照片以及照片包含的mask信息写入json文件
void phototool::WImgMasksToJson(const vector<SPhotoInfo> &imgList, const string &fileName)
{
	//根节点
	Json::Value root;
	Json::Value categoryNode;
	Json::Value category1;
	category1["id"] = 1;
	category1["name"] = "roof";
	category1["supercategory"] = "building";
	categoryNode.append(category1);
	root["categories"] = categoryNode;

	Json::Value imageNode;
	int imgCount = imgList.size();
	for (size_t i = 0; i < imgCount; i++)
	{
		Json::Value image;
		image["id"] = imgList[i].id;
		image["file_name"] = imgList[i].picName;
		image["height"] = imgList[i].height;
		image["width"] = imgList[i].width;
		imageNode.append(image);
	}
	root["images"] = imageNode;

	Json::Value maskNode;
	for (size_t i = 0; i < imgCount; i++)
	{
		int imgMaskCount = imgList[i].masks.size();
		for (size_t j = 0; j < imgMaskCount; j++)
		{
			Json::Value mask;
			mask["category_id"] = 1;
			mask["image_id"] = imgList[i].id;
			mask["score"] = imgList[i].masks[j].score;

			Json::Value seg;
			seg["counts"] = imgList[i].masks[j].segmentations;
			Json::Value size;
			size.append(imgList[i].height);
			size.append(imgList[i].width);
			seg["size"] = size;
			//Json::Value bbox;
			//bbox.append(imgList[i].masks[j].bBox[0]);
			//bbox.append(imgList[i].masks[j].bBox[1]);
			//bbox.append(imgList[i].masks[j].bBox[2]);
			//bbox.append(imgList[i].masks[j].bBox[3]);
			//seg["bbox"] = bbox;
			mask["segmentation"] = seg;
			maskNode.append(mask);
		}
	}
	root["result"] = maskNode;

	Json::StreamWriterBuilder  builder;
	builder["commentStyle"] = "None";
	builder["indentation"] = "	";
	std::unique_ptr<Json::StreamWriter> writer(builder.newStreamWriter());
	std::ofstream ofs;
	ofs.open(fileName, std::ios::app);
	writer->write(root, &ofs);
	ofs.close();
}

//根据mask列表和图片生成maskedImg; transparency透明度
void phototool::CreateImgMask(cv::Mat& image, vector<SMask> &masks, int w, int h, const vector<Eigen::Vector3d> mColors, float transparency)
{
	int iChannels = image.channels();
	int iRows = image.rows;
	// cols(width) of the image
	int cols = image.cols * iRows;
	int iCols = image.cols * iChannels;

	// check if the image data is stored continuous
	if (image.isContinuous())
	{
		iCols *= iRows;
		iRows = 1;
	}

	//srand(time(0));
	double color[3];

	double* bimsk[3];
	bimsk[0] = (double*)malloc(sizeof(double)*w*h);
	bimsk[1] = (double*)malloc(sizeof(double)*w*h);
	bimsk[2] = (double*)malloc(sizeof(double)*w*h);
	//for (int mIndex = 0; mIndex < maskList.size(); mIndex++)
	for (int mIndex = 0; mIndex < masks.size(); mIndex++)
	{
		byte* mask = DecodeMask(masks[mIndex].segmentations, w, h);
		color[0] = mColors[mIndex][0];
		color[1] = mColors[mIndex][1];
		color[2] = mColors[mIndex][2];
		for (int i = 0; i < 3; i++)
		{
			for (int j = 0; j < w*h; j++)
			{
				bimsk[i][j] = mask[j] * color[i] * 255;
			}
		}

		uchar* p;
		for (int i = 0; i < iRows; i++)
		{
			// get the pointer to the ith row
			p = image.ptr<uchar>(i);
			// operates on each pixel
			for (int j = 0; j < cols; j++)
			{
				// assigns new value
				p[3 * j] = (uchar)(p[3 * j] + bimsk[0][j] * transparency - (double)p[3 * j] * (double)mask[j] * transparency);
				p[3 * j + 1] = (uchar)(p[3 * j + 1] + bimsk[1][j] * transparency - (double)p[3 * j + 1] * (double)mask[j] * transparency);
				p[3 * j + 2] = (uchar)(p[3 * j + 2] + bimsk[2][j] * transparency - (double)p[3 * j + 2] * (double)mask[j] * transparency);
			}
		}
		free(mask);
	}

	free(bimsk[0]);
	free(bimsk[1]);
	free(bimsk[2]);
}

//保存OpenGL相机参数
bool phototool::SaveGLCameraParams(const vector<SGlCameraParam> &glCameraParams, const string& fileName)
{
	int photoCount = glCameraParams.size();

	//根节点
	Json::Value root;
	root["photoCount"] = photoCount;
	for (size_t pi = 0; pi < photoCount; pi++)
	{
		const SGlCameraParam& cameParam = glCameraParams[pi];

		Json::Value photoNode;
		photoNode["photoId"] = pi;
		photoNode["photoName"] = cameParam.picName;
		photoNode["imageWidth"] = cameParam.imageWidth;
		photoNode["imageHeight"] = cameParam.imageHeight;
		for (size_t i = 0; i < 16; i++)
		{
			photoNode["modelMat"].append(cameParam.modelMatrix[i]);
		}
		for (size_t i = 0; i < 16; i++)
		{
			photoNode["projMat"].append(cameParam.projMatrix[i]);
		}
		for (size_t i = 0; i < 4; i++)
		{
			photoNode["viewMat"].append(cameParam.viewport[i]);
		}
		root["photos"].append(photoNode);
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
