#include "myopengltool.h"

#define BITMAP_ID 0x4D42        // the universal bitmap ID
int bmpIndex = 0;
char outFileName[256] = { 0 };
bool myopengltool::WriteBMP(const char filename[], unsigned char* data, unsigned int w, unsigned int h)
{
	std::ofstream out_file;

	////** 检查data */
	if (!data)
	{
		std::cerr << "data corrupted! " << std::endl;
		out_file.close();
		return false;
	}

	////** 创建位图文件信息和位图文件头结构 */
	BITMAPFILEHEADER header;
	BITMAPINFOHEADER bitmapInfoHeader;

	//unsigned char textureColors = 0;/**< 用于将图像颜色从BGR变换到RGB */

	///** 打开文件,并检查错误 */
	out_file.open(filename, std::ios::out | std::ios::binary);
	if (!out_file)
	{
		std::cerr << "Unable to open file " << filename << std::endl;
		return false;
	}

	////** 填充BITMAPFILEHEADER */
	header.bfType = BITMAP_ID;
	header.bfSize = w*h * 3 + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
	header.bfReserved1 = 0;
	header.bfReserved2 = 0;
	header.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
	////** 写入位图文件头信息 */
	out_file.write((char*)&header, sizeof(BITMAPFILEHEADER));

	////** 填充BITMAPINFOHEADER */
	bitmapInfoHeader.biSize = sizeof(BITMAPINFOHEADER);
	bitmapInfoHeader.biWidth = w;
	bitmapInfoHeader.biHeight = h;
	bitmapInfoHeader.biPlanes = 1;
	bitmapInfoHeader.biBitCount = 24;
	bitmapInfoHeader.biCompression = BI_RGB; // BI_RLE4 BI_RLE8
	bitmapInfoHeader.biSizeImage = w * h * 3; // 当压缩类型为BI_RGB是也可以设置为0
	bitmapInfoHeader.biXPelsPerMeter = 0;
	bitmapInfoHeader.biYPelsPerMeter = 0;
	bitmapInfoHeader.biClrUsed = 0;
	bitmapInfoHeader.biClrImportant = 0;
	////** 写入位图文件信息 */
	out_file.write((char*)&bitmapInfoHeader, sizeof(BITMAPINFOHEADER));

	////** 将指针移到数据开始位置 */
	out_file.seekp(header.bfOffBits, std::ios::beg);

	////** 写入图像数据 */
	out_file.write((char*)data, bitmapInfoHeader.biSizeImage);

	out_file.close();
	return true;
}

bool myopengltool::WriteJPG_CV(const char *filename, unsigned char* pixelData, int w, int h)
{
	cv::Mat jpgImage(h, w, CV_8UC3, pixelData);

	//上下翻转，不知道为什么直接保存图片上下颠倒
	cv::Mat result;
	cv:: flip(jpgImage, result, 0);                  //正数表示水平方向，0表示垂直方向，负数表示水平和垂直
	//cv::createAlphaMat(mat);

	vector<int> compression_params;
	//compression_params.push_back(CV_IMWRITE_JPEG_QUALITY);  //选择jpeg
	//compression_params.push_back(100); //在这个填入你要的图片质量

	compression_params.push_back(CV_IMWRITE_PNG_COMPRESSION);
	compression_params.push_back(9);

	// show image
	try {
		cv::imwrite(filename, result, compression_params);
		//cv::imshow("Created JPG", jpgImage);
		//fprintf(stdout, "Finish creating image with alpha channel.\n");
		//cv::waitKey(0);
	}
	catch (std::runtime_error& ex) {
		fprintf(stderr, "Error when converting to PNG: %s\n", ex.what());
		return false;
	}
	return true;
}

bool myopengltool::Screenshot(const char* filename)
{
	GLenum lastBuffer;//无符号整数
	GLbyte* pBits = 0; // 图像数据
	unsigned long lImageSize;
	GLint iViewport[4]; // 视图大小

	glGetIntegerv(GL_VIEWPORT, iViewport);// 获取视图大小
	lImageSize = iViewport[2] * iViewport[3] * 3;  //窗口长宽相乘，计算像素数，再乘以通道数3

	pBits = (GLbyte*)new unsigned char[lImageSize];//分配足以存放窗口图像的空间
	if (!pBits)
		return false;

	// 从color buffer中读取数据
	glPixelStorei(GL_PACK_ALIGNMENT, 4);
	glPixelStorei(GL_PACK_ROW_LENGTH, 0);
	glPixelStorei(GL_PACK_SKIP_ROWS, 0);
	glPixelStorei(GL_PACK_SKIP_PIXELS, 0);

	//
	glGetIntegerv(GL_READ_BUFFER, (GLint*)&lastBuffer);
	glReadBuffer(GL_FRONT);
	//glReadBuffer(GL_BACK);
	glReadPixels(0, 0, iViewport[2], iViewport[3], GL_BGR_EXT, GL_UNSIGNED_BYTE, pBits);
	glReadBuffer(lastBuffer);

	bool haveRead = 0;
	//haveRead = writeBMP(filename, (unsigned char*)pBits, iViewport[2], iViewport[3]);
	haveRead = WriteJPG_CV(filename, (unsigned char*)pBits, iViewport[2], iViewport[3]);

	if (haveRead)
		return true;
	else
		return false;

}

//保存场景到图片
bool myopengltool::SaveAsImg(const string &fileName)
{
	if (!Screenshot(fileName.c_str()))
	{
		return false;
	}
	string infoJson;
	vector<string> strs1;
	filetool::SplitStringTo2(fileName, strs1, ".");
	infoJson = strs1[0] + ".json";
	SGlCameraParam camaParam;

	vector<string> strs2;
	filetool::SplitString(fileName, strs2, "\\");
	if (strs2.size() > 0)
	{
		camaParam.picName = strs2[strs2.size() - 1];
	}

	GLdouble modelMat[16];		//模型视图矩阵
	glGetDoublev(GL_MODELVIEW_MATRIX, modelMat);
	GLdouble projMat[16];			//投影矩阵
	glGetDoublev(GL_PROJECTION_MATRIX, projMat);
	GLint viewMat[4]; // 视图大小
	glGetIntegerv(GL_VIEWPORT, viewMat);// 获取视图大小

	camaParam.imageWidth = viewMat[2];
	camaParam.imageHeight = viewMat[3];
	for (size_t i = 0; i < 16; i++)
	{
		camaParam.modelMatrix[i] = modelMat[i];
	}
	for (size_t i = 0; i < 16; i++)
	{
		camaParam.projMatrix[i] = projMat[i];
	}
	for (size_t i = 0; i < 4; i++)
	{
		camaParam.viewport[i] = viewMat[i];
	}

	//phototool::WSPOpenGLCParamJson(infoJson, camaParam);
	return true;
}

//计算三角形法向量
Eigen::Vector3f myopengltool::CalTriNormal(Eigen::Vector3d ver1, Eigen::Vector3d ver2, Eigen::Vector3d ver3)
{
	double temp1[3], temp2[3], normal[3];
	double length = 0.0;

	temp1[0] = ver1[0] - ver2[0];
	temp1[1] = ver1[1] - ver2[1];
	temp1[2] = ver1[2] - ver2[2];
	temp2[0] = ver2[0] - ver3[0];
	temp2[1] = ver2[1] - ver3[1];
	temp2[2] = ver2[2] - ver3[2];

	//计算法线
	normal[0] = temp1[1] * temp2[2] - temp1[2] * temp2[1];
	normal[1] = -(temp1[0] * temp2[2] - temp1[2] * temp2[0]);
	normal[2] = temp1[0] * temp2[1] - temp1[1] * temp2[0];

	//法线单位化
	length = sqrt(normal[0] * normal[0] + normal[1] * normal[1] + normal[2] * normal[2]);
	if (length == 0.0f)
	{
		length = 1.0f;
	}

	normal[0] /= length;
	normal[1] /= length;
	normal[2] /= length;
	Eigen::Vector3f e_normal(normal[0], normal[1], normal[2]);
	return e_normal;
}

//初始化OpenGL环境
void myopengltool::InitializeOpenGL()
{
	////glClearDepth(1.0f);
	glEnable(GL_LIGHTING);		//启用光照
	//GLfloat ambiLightStrength = CGlobal::glRenderParam.ambiLightStrength;
	//GLfloat diffLightStrength = CGlobal::glRenderParam.diffLightStrength;
	//GLfloat specLightStrength = CGlobal::glRenderParam.specLightStrength;

	//GLfloat  ambientLight[] = { ambiLightStrength, ambiLightStrength, ambiLightStrength, 1.0f };	//环境光成分
	//GLfloat  diffuseLight[] = { diffLightStrength, diffLightStrength, diffLightStrength, 1.0f };	//散射光成分
	//GLfloat  specular[] = { specLightStrength, specLightStrength, specLightStrength, 1.0f };		//镜面光成分

	////设置和启用光照0
	//GLfloat	 lightPos0[] = { CGlobal::glRenderParam.lightPos0[0],
	//	CGlobal::glRenderParam.lightPos0[1],
	//	CGlobal::glRenderParam.lightPos0[2],
	//	CGlobal::glRenderParam.lightPos0[3] };  //光源0位置
	//glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);		//设置环境光
	//glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);		//设置散射光
	//glLightfv(GL_LIGHT0, GL_SPECULAR, specular);		//设置镜面光
	//glLightfv(GL_LIGHT0, GL_POSITION, lightPos0);	//设置光源位置
	//if (CGlobal::glRenderParam.useLight0)
	//{
	//	glEnable(GL_LIGHT0);
	//}

	////设置和启用光照1
	//GLfloat	 lightPos1[] = { CGlobal::glRenderParam.lightPos1[0],
	//	CGlobal::glRenderParam.lightPos1[1],
	//	CGlobal::glRenderParam.lightPos1[2],
	//	CGlobal::glRenderParam.lightPos1[3] };  //光源1位置
	//glLightfv(GL_LIGHT1, GL_AMBIENT, ambientLight);		//设置环境光
	//glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuseLight);		//设置散射光
	//glLightfv(GL_LIGHT1, GL_SPECULAR, specular);		//设置镜面光
	//glLightfv(GL_LIGHT1, GL_POSITION, lightPos1);	//设置光源位置
	//if (CGlobal::glRenderParam.useLight1)
	//{
	//	glEnable(GL_LIGHT1);
	//}

	//GLUquadricObj *quadratic;	// Storage For Our Quadratic Objects
	//							////启用纹理贴图
	//glEnable(GL_TEXTURE_2D);
	//glShadeModel(GL_SMOOTH);        //允许平滑着色
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //清空深度缓冲和颜色缓冲  
	//glEnable(GL_DEPTH_TEST);	//隐藏表面删除
	////glFrontFace(GL_CCW);
	////glEnable(GL_CULL_FACE);	//背面剔除

	//glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, TRUE);//双面光照：GL_TRUE开启，GL_FALSE关闭
	//glDisable(GL_CULL_FACE);

	//quadratic = gluNewQuadric();							// Create A Pointer To The Quadric Object (Return 0 If No Memory)
	//gluQuadricNormals(quadratic, GLU_SMOOTH);			// Create Smooth Normals 
	//gluQuadricTexture(quadratic, GL_TRUE);				// Create Texture Coords 
	//glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);			// 设置s方向的纹理自动生成
	//glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);			// 设置t方向的纹理自动生成

	//glEnable(GL_LINE_SMOOTH);//启用线抗锯齿,边缘会降低其alpha值
	//glEnable(GL_BLEND);//启用混合
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	///*对图像质量和渲染速度之间控制权衡关系*/
	//glHint(GL_LINE_SMOOTH, GL_NICEST);//没有偏向

	//GLfloat ambiMateriStrength = CGlobal::glRenderParam.ambiMetriStrength;
	//GLfloat diffMateriStrength = CGlobal::glRenderParam.diffMetriStrength;
	//GLfloat specMateriStrength = CGlobal::glRenderParam.specMetriStrength;
	//GLfloat  ambientf[] = { ambiMateriStrength, ambiMateriStrength, ambiMateriStrength, 1.0f };		//为材料添加的环境反射属性
	//GLfloat  diffusef[] = { diffMateriStrength, diffMateriStrength, diffMateriStrength, 1.0f };		//为材料添加的漫反射属性
	//GLfloat  specref[] = { specMateriStrength, specMateriStrength, specMateriStrength, 1.0f };		//为材料添加的镜面反射属性

	//glEnable(GL_COLOR_MATERIAL);	//启用颜色追踪
	//glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);	//设置正面和背面材料属性,与glColor值对应
	//glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specref);
	//glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambientf);
	//glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffusef);
	//glMateriali(GL_FRONT_AND_BACK, GL_SHININESS, 128);

	////glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);	//设置正面材料属性,与glColor值对应
	////glMaterialfv(GL_FRONT, GL_SPECULAR, specref);
	////glMaterialfv(GL_FRONT, GL_AMBIENT, ambientf);
	////glMaterialfv(GL_FRONT, GL_DIFFUSE, diffusef);
	////glMateriali(GL_FRONT, GL_SHININESS, 128);

	////glColorMaterial(GL_BACK, GL_AMBIENT_AND_DIFFUSE);	//设置背面材料属性,与glColor值对应
	////glMaterialfv(GL_BACK, GL_SPECULAR, specref);	//设置背面材料属性,与glColor值对应
	////glMaterialfv(GL_BACK, GL_AMBIENT, ambientf);
	////glMaterialfv(GL_BACK, GL_DIFFUSE, diffusef);
	////glMateriali(GL_BACK, GL_SHININESS, 128);

	//glClearColor(1.0f, 1.0f, 1.0f, 1.0f);	//设置背景颜色	

	//glLoadIdentity();
}

//刷新OpenGL渲染设置
void myopengltool::RefreshGLRenderParm()
{
	//GLfloat ambiLightStrength = CGlobal::glRenderParam.ambiLightStrength;
	//GLfloat diffLightStrength = CGlobal::glRenderParam.diffLightStrength;
	//GLfloat specLightStrength = CGlobal::glRenderParam.specLightStrength;

	//GLfloat  ambientLight[] = { ambiLightStrength, ambiLightStrength, ambiLightStrength, 1.0f };	//环境光成分
	//GLfloat  diffuseLight[] = { diffLightStrength, diffLightStrength, diffLightStrength, 1.0f };	//散射光成分
	//GLfloat  specular[] = { specLightStrength, specLightStrength, specLightStrength, 1.0f };		//镜面光成分

	////设置和启用光照0
	//glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);		//设置环境光
	//glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);		//设置散射光
	//glLightfv(GL_LIGHT0, GL_SPECULAR, specular);		//设置镜面光
	//if (CGlobal::glRenderParam.useLight0)
	//{
	//	glEnable(GL_LIGHT0);
	//}
	//else
	//{
	//	glDisable(GL_LIGHT0);
	//}

	////设置和启用光照1
	//glLightfv(GL_LIGHT1, GL_AMBIENT, ambientLight);		//设置环境光
	//glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuseLight);		//设置散射光
	//glLightfv(GL_LIGHT1, GL_SPECULAR, specular);		//设置镜面光
	//if (CGlobal::glRenderParam.useLight1)
	//{
	//	glEnable(GL_LIGHT1);
	//}
	//else
	//{
	//	glDisable(GL_LIGHT1);
	//}

	//GLfloat ambiMateriStrength = CGlobal::glRenderParam.ambiMetriStrength;
	//GLfloat diffMateriStrength = CGlobal::glRenderParam.diffMetriStrength;
	//GLfloat specMateriStrength = CGlobal::glRenderParam.specMetriStrength;
	//GLfloat  ambientf[] = { ambiMateriStrength, ambiMateriStrength, ambiMateriStrength, 1.0f };		//为材料添加的环境反射属性
	//GLfloat  diffusef[] = { diffMateriStrength, diffMateriStrength, diffMateriStrength, 1.0f };		//为材料添加的漫反射属性
	//GLfloat  specref[] = { specMateriStrength, specMateriStrength, specMateriStrength, 1.0f };		//为材料添加的镜面反射属性

	//glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, TRUE);
	//glEnable(GL_COLOR_MATERIAL);	//启用颜色追踪
	//glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);	//设置正面和背面材料属性,与glColor值对应
	//glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specref);
	//glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambientf);
	//glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffusef);
	//glMateriali(GL_FRONT_AND_BACK, GL_SHININESS, 128);

	////glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);	//设置材料属性,与glColor值对应
	////glMaterialfv(GL_FRONT, GL_SPECULAR, specref);
	////glMaterialfv(GL_FRONT, GL_AMBIENT, ambientf);
	////glMaterialfv(GL_FRONT, GL_DIFFUSE, diffusef);
	////glMateriali(GL_FRONT, GL_SHININESS, 128);

	////glColorMaterial(GL_BACK, GL_AMBIENT_AND_DIFFUSE);	//设置正面材料属性,与glColor值对应
	////glMaterialfv(GL_BACK, GL_SPECULAR, specref);	//设置背面材料属性,与glColor值对应
	////glMaterialfv(GL_BACK, GL_AMBIENT, ambientf);
	////glMaterialfv(GL_BACK, GL_DIFFUSE, diffusef);
	////glMateriali(GL_BACK, GL_SHININESS, 128);

	//return;
}

void TransformPoint(GLdouble out[4], const GLdouble m[16], const GLdouble in[4])
{
#define M(row,col) m[col*4+row]  
	out[0] = M(0, 0) * in[0] + M(0, 1) * in[1] + M(0, 2) * in[2] + M(0, 3) * in[3];
	out[1] = M(1, 0) * in[0] + M(1, 1) * in[1] + M(1, 2) * in[2] + M(1, 3) * in[3];
	out[2] = M(2, 0) * in[0] + M(2, 1) * in[1] + M(2, 2) * in[2] + M(2, 3) * in[3];
	out[3] = M(3, 0) * in[0] + M(3, 1) * in[1] + M(3, 2) * in[2] + M(3, 3) * in[3];
#undef M  
}

//按OpenGL相机参数将世界坐标系中的点投影到图像坐标系，返回图像坐标和深度值
Vector3d myopengltool::GluProject(const Vector3d &worldPosition, const SGlCameraParam &cameraParam)
{
	Vector3d imagePos(-1, -1, -1);

	//GLdouble model[16] = ;
	//GLdouble proj[16];
	//GLint viewport[4];

	//GLdouble zNear = 0.01;
	//GLdouble zFar = 1000.0;

	GLdouble zNear = 10;
	GLdouble zFar = 200.0;

	GLdouble objCoor[4];
	GLdouble objProj[4], objModel[4];


	// 4x4 matrix must be multi to a 4 dimension vector( it a 1 x 4 matrix)  
	// so we need to put the original vertex to a 4D vector  
	objCoor[0] = worldPosition[0];
	objCoor[1] = worldPosition[1];
	objCoor[2] = worldPosition[2];
	objCoor[3] = 1.0;

	// 由于原来的向量位于标准基向量(1, 0, 0), (0, 1, 0), (0, 0, 1)中，所以需要先转换到当前的模型矩阵中  
	TransformPoint(objModel, cameraParam.modelMatrix, objCoor);

	// 然后将模型矩阵中的顶点转换到投影矩阵所在坐标系的矩阵中  
	TransformPoint(objProj, cameraParam.projMatrix, objModel);

	// scale matrix  

	if (objProj[3] == 0.0)
		return imagePos;

	double viewZ = -objProj[2];

	objProj[0] /= objProj[3];
	objProj[1] /= objProj[3];
	objProj[2] /= objProj[3];

	// 由于投影矩阵投影在[-1, 1]之间，所以需要将转换后的投影坐标放置到[0, 1]之间  
	// 最后再在一个offset 矩形中转换为屏幕坐标就可以了（viewport[4]可以简单的认为一个offset矩形）  

#define SCALE_FROM_0_TO_1(_pt)  (((_pt) + 1)/2)  
	objProj[0] = SCALE_FROM_0_TO_1(objProj[0]);
	objProj[1] = SCALE_FROM_0_TO_1(objProj[1]);
	objProj[2] = SCALE_FROM_0_TO_1(objProj[2]);
#undef SCALE_FROM_0_TO_1  

	imagePos[0] = cameraParam.viewport[0] + objProj[0] * cameraParam.viewport[2];
	imagePos[1] = cameraParam.viewport[1] + objProj[1] * cameraParam.viewport[3];

	imagePos[1] = cameraParam.viewport[3] - imagePos[1];

	double ndcZ = (zNear + zFar) / (zFar - zNear) + (2 * zFar*zNear) / ((zFar - zNear)*viewZ);
	imagePos[2] = ndcZ;

	//*winz = objProj[2];
	//*winz = objProj[2] * objProj[3];
	return imagePos;
}

//获取当前OpenGL相机参数
SGlCameraParam myopengltool::GetGLCameraParam()
{
	SGlCameraParam cameraParam;
	GLdouble modelMat[16];		//模型视图矩阵
	glGetDoublev(GL_MODELVIEW_MATRIX, modelMat);
	GLdouble projMat[16];			//投影矩阵
	glGetDoublev(GL_PROJECTION_MATRIX, projMat);
	GLint viewMat[4]; // 视图大小
	glGetIntegerv(GL_VIEWPORT, viewMat);// 获取视图大小

	cameraParam.imageWidth = viewMat[2];
	cameraParam.imageHeight = viewMat[3];
	for (size_t i = 0; i < 16; i++)
	{
		cameraParam.modelMatrix[i] = modelMat[i];
	}
	for (size_t i = 0; i < 16; i++)
	{
		cameraParam.projMatrix[i] = projMat[i];
	}
	for (size_t i = 0; i < 4; i++)
	{
		cameraParam.viewport[i] = viewMat[i];
	}

	return cameraParam;
}

