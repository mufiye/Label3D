#include "myopengltool.h"

#define BITMAP_ID 0x4D42        // the universal bitmap ID
int bmpIndex = 0;
char outFileName[256] = { 0 };
bool myopengltool::WriteBMP(const char filename[], unsigned char* data, unsigned int w, unsigned int h)
{
	std::ofstream out_file;

	////** ���data */
	if (!data)
	{
		std::cerr << "data corrupted! " << std::endl;
		out_file.close();
		return false;
	}

	////** ����λͼ�ļ���Ϣ��λͼ�ļ�ͷ�ṹ */
	BITMAPFILEHEADER header;
	BITMAPINFOHEADER bitmapInfoHeader;

	//unsigned char textureColors = 0;/**< ���ڽ�ͼ����ɫ��BGR�任��RGB */

	///** ���ļ�,�������� */
	out_file.open(filename, std::ios::out | std::ios::binary);
	if (!out_file)
	{
		std::cerr << "Unable to open file " << filename << std::endl;
		return false;
	}

	////** ���BITMAPFILEHEADER */
	header.bfType = BITMAP_ID;
	header.bfSize = w*h * 3 + sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
	header.bfReserved1 = 0;
	header.bfReserved2 = 0;
	header.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
	////** д��λͼ�ļ�ͷ��Ϣ */
	out_file.write((char*)&header, sizeof(BITMAPFILEHEADER));

	////** ���BITMAPINFOHEADER */
	bitmapInfoHeader.biSize = sizeof(BITMAPINFOHEADER);
	bitmapInfoHeader.biWidth = w;
	bitmapInfoHeader.biHeight = h;
	bitmapInfoHeader.biPlanes = 1;
	bitmapInfoHeader.biBitCount = 24;
	bitmapInfoHeader.biCompression = BI_RGB; // BI_RLE4 BI_RLE8
	bitmapInfoHeader.biSizeImage = w * h * 3; // ��ѹ������ΪBI_RGB��Ҳ��������Ϊ0
	bitmapInfoHeader.biXPelsPerMeter = 0;
	bitmapInfoHeader.biYPelsPerMeter = 0;
	bitmapInfoHeader.biClrUsed = 0;
	bitmapInfoHeader.biClrImportant = 0;
	////** д��λͼ�ļ���Ϣ */
	out_file.write((char*)&bitmapInfoHeader, sizeof(BITMAPINFOHEADER));

	////** ��ָ���Ƶ����ݿ�ʼλ�� */
	out_file.seekp(header.bfOffBits, std::ios::beg);

	////** д��ͼ������ */
	out_file.write((char*)data, bitmapInfoHeader.biSizeImage);

	out_file.close();
	return true;
}

bool myopengltool::WriteJPG_CV(const char *filename, unsigned char* pixelData, int w, int h)
{
	cv::Mat jpgImage(h, w, CV_8UC3, pixelData);

	//���·�ת����֪��Ϊʲôֱ�ӱ���ͼƬ���µߵ�
	cv::Mat result;
	cv:: flip(jpgImage, result, 0);                  //������ʾˮƽ����0��ʾ��ֱ���򣬸�����ʾˮƽ�ʹ�ֱ
	//cv::createAlphaMat(mat);

	vector<int> compression_params;
	//compression_params.push_back(CV_IMWRITE_JPEG_QUALITY);  //ѡ��jpeg
	//compression_params.push_back(100); //�����������Ҫ��ͼƬ����

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
	GLenum lastBuffer;//�޷�������
	GLbyte* pBits = 0; // ͼ������
	unsigned long lImageSize;
	GLint iViewport[4]; // ��ͼ��С

	glGetIntegerv(GL_VIEWPORT, iViewport);// ��ȡ��ͼ��С
	lImageSize = iViewport[2] * iViewport[3] * 3;  //���ڳ�����ˣ��������������ٳ���ͨ����3

	pBits = (GLbyte*)new unsigned char[lImageSize];//�������Դ�Ŵ���ͼ��Ŀռ�
	if (!pBits)
		return false;

	// ��color buffer�ж�ȡ����
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

//���泡����ͼƬ
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

	GLdouble modelMat[16];		//ģ����ͼ����
	glGetDoublev(GL_MODELVIEW_MATRIX, modelMat);
	GLdouble projMat[16];			//ͶӰ����
	glGetDoublev(GL_PROJECTION_MATRIX, projMat);
	GLint viewMat[4]; // ��ͼ��С
	glGetIntegerv(GL_VIEWPORT, viewMat);// ��ȡ��ͼ��С

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

//���������η�����
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

	//���㷨��
	normal[0] = temp1[1] * temp2[2] - temp1[2] * temp2[1];
	normal[1] = -(temp1[0] * temp2[2] - temp1[2] * temp2[0]);
	normal[2] = temp1[0] * temp2[1] - temp1[1] * temp2[0];

	//���ߵ�λ��
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

//��ʼ��OpenGL����
void myopengltool::InitializeOpenGL()
{
	////glClearDepth(1.0f);
	glEnable(GL_LIGHTING);		//���ù���
	//GLfloat ambiLightStrength = CGlobal::glRenderParam.ambiLightStrength;
	//GLfloat diffLightStrength = CGlobal::glRenderParam.diffLightStrength;
	//GLfloat specLightStrength = CGlobal::glRenderParam.specLightStrength;

	//GLfloat  ambientLight[] = { ambiLightStrength, ambiLightStrength, ambiLightStrength, 1.0f };	//������ɷ�
	//GLfloat  diffuseLight[] = { diffLightStrength, diffLightStrength, diffLightStrength, 1.0f };	//ɢ���ɷ�
	//GLfloat  specular[] = { specLightStrength, specLightStrength, specLightStrength, 1.0f };		//�����ɷ�

	////���ú����ù���0
	//GLfloat	 lightPos0[] = { CGlobal::glRenderParam.lightPos0[0],
	//	CGlobal::glRenderParam.lightPos0[1],
	//	CGlobal::glRenderParam.lightPos0[2],
	//	CGlobal::glRenderParam.lightPos0[3] };  //��Դ0λ��
	//glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);		//���û�����
	//glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);		//����ɢ���
	//glLightfv(GL_LIGHT0, GL_SPECULAR, specular);		//���þ����
	//glLightfv(GL_LIGHT0, GL_POSITION, lightPos0);	//���ù�Դλ��
	//if (CGlobal::glRenderParam.useLight0)
	//{
	//	glEnable(GL_LIGHT0);
	//}

	////���ú����ù���1
	//GLfloat	 lightPos1[] = { CGlobal::glRenderParam.lightPos1[0],
	//	CGlobal::glRenderParam.lightPos1[1],
	//	CGlobal::glRenderParam.lightPos1[2],
	//	CGlobal::glRenderParam.lightPos1[3] };  //��Դ1λ��
	//glLightfv(GL_LIGHT1, GL_AMBIENT, ambientLight);		//���û�����
	//glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuseLight);		//����ɢ���
	//glLightfv(GL_LIGHT1, GL_SPECULAR, specular);		//���þ����
	//glLightfv(GL_LIGHT1, GL_POSITION, lightPos1);	//���ù�Դλ��
	//if (CGlobal::glRenderParam.useLight1)
	//{
	//	glEnable(GL_LIGHT1);
	//}

	//GLUquadricObj *quadratic;	// Storage For Our Quadratic Objects
	//							////����������ͼ
	//glEnable(GL_TEXTURE_2D);
	//glShadeModel(GL_SMOOTH);        //����ƽ����ɫ
	//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //�����Ȼ������ɫ����  
	//glEnable(GL_DEPTH_TEST);	//���ر���ɾ��
	////glFrontFace(GL_CCW);
	////glEnable(GL_CULL_FACE);	//�����޳�

	//glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, TRUE);//˫����գ�GL_TRUE������GL_FALSE�ر�
	//glDisable(GL_CULL_FACE);

	//quadratic = gluNewQuadric();							// Create A Pointer To The Quadric Object (Return 0 If No Memory)
	//gluQuadricNormals(quadratic, GLU_SMOOTH);			// Create Smooth Normals 
	//gluQuadricTexture(quadratic, GL_TRUE);				// Create Texture Coords 
	//glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);			// ����s����������Զ�����
	//glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);			// ����t����������Զ�����

	//glEnable(GL_LINE_SMOOTH);//�����߿����,��Ե�ή����alphaֵ
	//glEnable(GL_BLEND);//���û��
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	///*��ͼ����������Ⱦ�ٶ�֮�����Ȩ���ϵ*/
	//glHint(GL_LINE_SMOOTH, GL_NICEST);//û��ƫ��

	//GLfloat ambiMateriStrength = CGlobal::glRenderParam.ambiMetriStrength;
	//GLfloat diffMateriStrength = CGlobal::glRenderParam.diffMetriStrength;
	//GLfloat specMateriStrength = CGlobal::glRenderParam.specMetriStrength;
	//GLfloat  ambientf[] = { ambiMateriStrength, ambiMateriStrength, ambiMateriStrength, 1.0f };		//Ϊ������ӵĻ�����������
	//GLfloat  diffusef[] = { diffMateriStrength, diffMateriStrength, diffMateriStrength, 1.0f };		//Ϊ������ӵ�����������
	//GLfloat  specref[] = { specMateriStrength, specMateriStrength, specMateriStrength, 1.0f };		//Ϊ������ӵľ��淴������

	//glEnable(GL_COLOR_MATERIAL);	//������ɫ׷��
	//glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);	//��������ͱ����������,��glColorֵ��Ӧ
	//glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specref);
	//glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambientf);
	//glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffusef);
	//glMateriali(GL_FRONT_AND_BACK, GL_SHININESS, 128);

	////glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);	//���������������,��glColorֵ��Ӧ
	////glMaterialfv(GL_FRONT, GL_SPECULAR, specref);
	////glMaterialfv(GL_FRONT, GL_AMBIENT, ambientf);
	////glMaterialfv(GL_FRONT, GL_DIFFUSE, diffusef);
	////glMateriali(GL_FRONT, GL_SHININESS, 128);

	////glColorMaterial(GL_BACK, GL_AMBIENT_AND_DIFFUSE);	//���ñ����������,��glColorֵ��Ӧ
	////glMaterialfv(GL_BACK, GL_SPECULAR, specref);	//���ñ����������,��glColorֵ��Ӧ
	////glMaterialfv(GL_BACK, GL_AMBIENT, ambientf);
	////glMaterialfv(GL_BACK, GL_DIFFUSE, diffusef);
	////glMateriali(GL_BACK, GL_SHININESS, 128);

	//glClearColor(1.0f, 1.0f, 1.0f, 1.0f);	//���ñ�����ɫ	

	//glLoadIdentity();
}

//ˢ��OpenGL��Ⱦ����
void myopengltool::RefreshGLRenderParm()
{
	//GLfloat ambiLightStrength = CGlobal::glRenderParam.ambiLightStrength;
	//GLfloat diffLightStrength = CGlobal::glRenderParam.diffLightStrength;
	//GLfloat specLightStrength = CGlobal::glRenderParam.specLightStrength;

	//GLfloat  ambientLight[] = { ambiLightStrength, ambiLightStrength, ambiLightStrength, 1.0f };	//������ɷ�
	//GLfloat  diffuseLight[] = { diffLightStrength, diffLightStrength, diffLightStrength, 1.0f };	//ɢ���ɷ�
	//GLfloat  specular[] = { specLightStrength, specLightStrength, specLightStrength, 1.0f };		//�����ɷ�

	////���ú����ù���0
	//glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);		//���û�����
	//glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);		//����ɢ���
	//glLightfv(GL_LIGHT0, GL_SPECULAR, specular);		//���þ����
	//if (CGlobal::glRenderParam.useLight0)
	//{
	//	glEnable(GL_LIGHT0);
	//}
	//else
	//{
	//	glDisable(GL_LIGHT0);
	//}

	////���ú����ù���1
	//glLightfv(GL_LIGHT1, GL_AMBIENT, ambientLight);		//���û�����
	//glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuseLight);		//����ɢ���
	//glLightfv(GL_LIGHT1, GL_SPECULAR, specular);		//���þ����
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
	//GLfloat  ambientf[] = { ambiMateriStrength, ambiMateriStrength, ambiMateriStrength, 1.0f };		//Ϊ������ӵĻ�����������
	//GLfloat  diffusef[] = { diffMateriStrength, diffMateriStrength, diffMateriStrength, 1.0f };		//Ϊ������ӵ�����������
	//GLfloat  specref[] = { specMateriStrength, specMateriStrength, specMateriStrength, 1.0f };		//Ϊ������ӵľ��淴������

	//glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, TRUE);
	//glEnable(GL_COLOR_MATERIAL);	//������ɫ׷��
	//glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);	//��������ͱ����������,��glColorֵ��Ӧ
	//glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specref);
	//glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambientf);
	//glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffusef);
	//glMateriali(GL_FRONT_AND_BACK, GL_SHININESS, 128);

	////glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);	//���ò�������,��glColorֵ��Ӧ
	////glMaterialfv(GL_FRONT, GL_SPECULAR, specref);
	////glMaterialfv(GL_FRONT, GL_AMBIENT, ambientf);
	////glMaterialfv(GL_FRONT, GL_DIFFUSE, diffusef);
	////glMateriali(GL_FRONT, GL_SHININESS, 128);

	////glColorMaterial(GL_BACK, GL_AMBIENT_AND_DIFFUSE);	//���������������,��glColorֵ��Ӧ
	////glMaterialfv(GL_BACK, GL_SPECULAR, specref);	//���ñ����������,��glColorֵ��Ӧ
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

//��OpenGL�����������������ϵ�еĵ�ͶӰ��ͼ������ϵ������ͼ����������ֵ
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

	// ����ԭ��������λ�ڱ�׼������(1, 0, 0), (0, 1, 0), (0, 0, 1)�У�������Ҫ��ת������ǰ��ģ�;�����  
	TransformPoint(objModel, cameraParam.modelMatrix, objCoor);

	// Ȼ��ģ�;����еĶ���ת����ͶӰ������������ϵ�ľ�����  
	TransformPoint(objProj, cameraParam.projMatrix, objModel);

	// scale matrix  

	if (objProj[3] == 0.0)
		return imagePos;

	double viewZ = -objProj[2];

	objProj[0] /= objProj[3];
	objProj[1] /= objProj[3];
	objProj[2] /= objProj[3];

	// ����ͶӰ����ͶӰ��[-1, 1]֮�䣬������Ҫ��ת�����ͶӰ������õ�[0, 1]֮��  
	// �������һ��offset ������ת��Ϊ��Ļ����Ϳ����ˣ�viewport[4]���Լ򵥵���Ϊһ��offset���Σ�  

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

//��ȡ��ǰOpenGL�������
SGlCameraParam myopengltool::GetGLCameraParam()
{
	SGlCameraParam cameraParam;
	GLdouble modelMat[16];		//ģ����ͼ����
	glGetDoublev(GL_MODELVIEW_MATRIX, modelMat);
	GLdouble projMat[16];			//ͶӰ����
	glGetDoublev(GL_PROJECTION_MATRIX, projMat);
	GLint viewMat[4]; // ��ͼ��С
	glGetIntegerv(GL_VIEWPORT, viewMat);// ��ȡ��ͼ��С

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

