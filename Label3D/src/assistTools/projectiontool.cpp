#include "projectiontool.h"

const double eps = 1e-6;

//判断三个顶点是否至少有一个在图像范围内
bool JudgeInImgRange(Vector2d &pos1, Vector2d &pos2, Vector2d &pos3, const int &width, const int &height)
{
	return ((pos1[0] >= 0 && pos1[0] < width && pos1[1] >= 0 && pos1[1] < height) || (pos2[0] >= 0 && pos2[0] < width && pos2[1] >= 0 && pos2[1] < height) || (pos3[0] >= 0 && pos3[0] < width && pos3[1] >= 0 && pos3[1] < height));
}

//根据cameraOrientation获取初始方向矩阵
Matrix3d OriMatrix(const string& orientation)
{
	Matrix3d oriMatrix;
	if (orientation == "XRightYDown")
	{
		oriMatrix << 1, 0, 0, 0, 1, 0, 0, 0, 1;
	}
	else if (orientation == "XLeftYDown")
	{
		oriMatrix << -1, 0, 0, 0, 1, 0, 0, 0, -1;
	}
	else if (orientation == "XLeftYUp")
	{
		oriMatrix << -1, 0, 0, 0, -1, 0, 0, 0, 1;
	}
	else if (orientation == "XRightYUp")
	{
		oriMatrix << 1, 0, 0, 0, -1, 0, 0, 0, -1;
	}
	else if (orientation == "XDownYRight")
	{
		oriMatrix << 0, -1, 0, 1, 0, 0, 0, 0, 1;
	}
	else if (orientation == "XDownYLeft")
	{
		oriMatrix << 0, -1, 0, 1, 0, 0, 0, 0, 1;
	}
	else if (orientation == "XUpYLeft")
	{
		oriMatrix << 0, -1, 0, -1, 0, 0, 0, 0, -1;
	}
	else if (orientation == "XUpYRight")
	{
		oriMatrix << 0, 1, 0, -1, 0, 0, 0, 0, 1;
	}
	else
	{
		oriMatrix << 1, 0, 0, 0, 1, 0, 0, 0, 1;
	}
	return oriMatrix;
}

//相机畸变方程
Vector2d Distortion(Vector2d vector, SCameraParam cameraParams)
{
	Vector2d out;
	double K1 = cameraParams.distortion_K1;
	double K2 = cameraParams.distortion_K2;
	double K3 = cameraParams.distortion_K3;
	double P1 = cameraParams.distortion_P1;
	double P2 = cameraParams.distortion_P2;
	double u = vector[0];
	double v = vector[1];
	double r2 = u*u + v*v;
	double out_1 = (1 + K1*r2 + K2*pow(r2, 2) + K3*pow(r2, 3))*u + 2 * P2*u*v + P1*(r2 + 2 * pow(u, 2));
	double out_2 = (1 + K1*r2 + K2*pow(r2, 2) + K3*pow(r2, 3))*v + 2 * P1*u*v + P2*(r2 + 2 * pow(v, 2));

	out << out_1, out_2;
	return out;
}

// 计算权重 CalculateWeight
bool CalcWeight(double i, double j, const Vector2d &v1, const Vector2d &v2, const Vector2d &v3, const Vector3d &k, double &a, double &b, double &c)
{
	// 计算权重
	a = ((v2[1] - v3[1]) * i + (v3[0] - v2[0]) * j + v2[0] * v3[1] - v3[0] * v2[1]) * k[1];
	b = ((v3[1] - v1[1]) * i + (v1[0] - v3[0]) * j + v3[0] * v1[1] - v1[0] * v3[1]) * k[2];
	// 由于重心a+b+c=1 所以在这里计算c=1-a-b来优化算法
	// c=((v1[1]-v2[1])*i+(v2[0]-v1[0])*j+v1[0]*v2[1]-v2[0]*v1[1])*k[0]; 
	c = 1 - a - b;
	// 判断是否在三角形内 连同边上的点进行处理
	if (a >= 0 && b >= 0 && c >= 0) {
		return true;
	}
	else {
		return false;
	}
}

//更新深度表
void UpdateDepthMap(Mat &deepMap, const Vector2d &imgPos1, const Vector2d &imgPos2, const Vector2d &imgPos3, const double &depth1, const double &depth2, const double &depth3, const int &width, const int &height, const string& type)
{
	Vector2d p1 = imgPos1 - imgPos3;
	Vector2d p2 = imgPos2 - imgPos3;
	Vector2d p3 = imgPos3 - imgPos3;

	Vector3d k;
	// 计算直线斜率
	k[0] = ((p1[1] - p2[1]) * p3[0] + (p2[0] - p1[0]) * p3[1] + p1[0] * p2[1] - p2[0] * p1[1]);
	k[1] = ((p2[1] - p3[1]) * p1[0] + (p3[0] - p2[0]) * p1[1] + p2[0] * p3[1] - p3[0] * p2[1]);
	k[2] = ((p3[1] - p1[1]) * p2[0] + (p1[0] - p3[0]) * p2[1] + p3[0] * p1[1] - p1[0] * p3[1]);

	// 不绘制计算结果k为0的三角形 (处理不符合条件的三角形)
	if (abs(k[0]) < eps)
		return;
	if (abs(k[1]) < eps)
		return;
	if (abs(k[2]) < eps)
		return;

	// 计算直线斜率的倒数
	k[0] = 1 / k[0];
	k[1] = 1 / k[1];
	k[2] = 1 / k[2];

	// 第一步找出XMAX  YMAX  XMIN  YMIN找出最小矩形
	double xMIN, xMAX, yMIN, yMAX;
	xMIN = (imgPos1[0] < imgPos2[0] ? imgPos1[0] : imgPos2[0]) < imgPos3[0] ? (imgPos1[0] < imgPos2[0] ? imgPos1[0] : imgPos2[0]) : imgPos3[0];
	xMAX = (imgPos1[0] > imgPos2[0] ? imgPos1[0] : imgPos2[0]) > imgPos3[0] ? (imgPos1[0] > imgPos2[0] ? imgPos1[0] : imgPos2[0]) : imgPos3[0];
	yMIN = (imgPos1[1] < imgPos2[1] ? imgPos1[1] : imgPos2[1]) < imgPos3[1] ? (imgPos1[1] < imgPos2[1] ? imgPos1[1] : imgPos2[1]) : imgPos3[1];
	yMAX = (imgPos1[1] > imgPos2[1] ? imgPos1[1] : imgPos2[1]) > imgPos3[1] ? (imgPos1[1] > imgPos2[1] ? imgPos1[1] : imgPos2[1]) : imgPos3[1];

	int xMIN_i = floorf(xMIN);
	int xMAX_i = ceilf(xMAX);
	int yMIN_i = floorf(yMIN);
	int yMAX_i = ceilf(yMAX);

	//// 存储顶点位置以及深度信息临时变量
	//double pTemp[3];
	// 三顶点权重a, b, c, 初始化为三者的平均值0.33
	double weight_a = 0.333333333f, weight_b = 0.333333333f, weight_c = 0.333333333f;

	// 循环变量
	int i, j;

	Vector2d minP(xMIN_i, yMIN_i);
	Vector2d p1_ori = imgPos1 - minP;
	Vector2d p2_ori = imgPos2 - minP;
	Vector2d p3_ori = imgPos3 - minP;

	for (i = xMIN_i-1 ; i <= xMAX_i+1; i++)
	{
		for (j = yMIN_i-1; j <= yMAX_i; j++)
		{
			if (i >= 0 && i<width && j >= 0 && j<height)
			{
				if (CalcWeight(i - xMIN_i, j - yMIN_i, p1_ori, p2_ori, p3_ori, k, weight_a, weight_b, weight_c))
				{
					// 三顶点对i,j处的深度贡献值进行融合
					double depth_ij = weight_a * depth1 + weight_b * depth2 + weight_c * depth3;
					//if (floor(depth_ij) < ((int*)deepMap.data)[j*width + i])
					//	((int*)deepMap.data)[j*width + i] = floor(depth_ij);

					if (type == "distance")
					{
						if (depth_ij <= ((double*)deepMap.data)[j*width + i])
							((double*)deepMap.data)[j*width + i] = depth_ij;
					}
					else if (type == "height")
					{
						if (depth_ij >= ((double*)deepMap.data)[j*width + i])
							((double*)deepMap.data)[j*width + i] = depth_ij;
					}
				}
			}
		}
	}
}

//更新深度表
void UpdateDepthMap(Mat &deepMap, Mat &segMap, const Vector2d &imgPos1, const Vector2d &imgPos2, const Vector2d &imgPos3, const double &depth1, const double &depth2, const double &depth3, const int &segIns, const int &width, const int &height)
{
	Vector2d p1 = imgPos1 - imgPos3;
	Vector2d p2 = imgPos2 - imgPos3;
	Vector2d p3 = imgPos3 - imgPos3;

	Vector3d k;
	// 计算直线斜率
	k[0] = ((p1[1] - p2[1]) * p3[0] + (p2[0] - p1[0]) * p3[1] + p1[0] * p2[1] - p2[0] * p1[1]);
	k[1] = ((p2[1] - p3[1]) * p1[0] + (p3[0] - p2[0]) * p1[1] + p2[0] * p3[1] - p3[0] * p2[1]);
	k[2] = ((p3[1] - p1[1]) * p2[0] + (p1[0] - p3[0]) * p2[1] + p3[0] * p1[1] - p1[0] * p3[1]);

	// 不绘制计算结果k为0的三角形 (处理不符合条件的三角形)
	if (abs(k[0]) < eps)
		return;
	if (abs(k[1]) < eps)
		return;
	if (abs(k[2]) < eps)
		return;

	// 计算直线斜率的倒数
	k[0] = 1 / k[0];
	k[1] = 1 / k[1];
	k[2] = 1 / k[2];

	// 第一步找出XMAX  YMAX  XMIN  YMIN找出最小矩形
	double xMIN, xMAX, yMIN, yMAX;
	xMIN = (imgPos1[0] < imgPos2[0] ? imgPos1[0] : imgPos2[0]) < imgPos3[0] ? (imgPos1[0] < imgPos2[0] ? imgPos1[0] : imgPos2[0]) : imgPos3[0];
	xMAX = (imgPos1[0] > imgPos2[0] ? imgPos1[0] : imgPos2[0]) > imgPos3[0] ? (imgPos1[0] > imgPos2[0] ? imgPos1[0] : imgPos2[0]) : imgPos3[0];
	yMIN = (imgPos1[1] < imgPos2[1] ? imgPos1[1] : imgPos2[1]) < imgPos3[1] ? (imgPos1[1] < imgPos2[1] ? imgPos1[1] : imgPos2[1]) : imgPos3[1];
	yMAX = (imgPos1[1] > imgPos2[1] ? imgPos1[1] : imgPos2[1]) > imgPos3[1] ? (imgPos1[1] > imgPos2[1] ? imgPos1[1] : imgPos2[1]) : imgPos3[1];

	int xMIN_i = floorf(xMIN);
	int xMAX_i = ceilf(xMAX);
	int yMIN_i = floorf(yMIN);
	int yMAX_i = ceilf(yMAX);

	//// 存储顶点位置以及深度信息临时变量
	//double pTemp[3];
	// 三顶点权重a, b, c, 初始化为三者的平均值0.33
	double weight_a = 0.333333333f, weight_b = 0.333333333f, weight_c = 0.333333333f;

	// 循环变量
	int i, j;

	Vector2d minP(xMIN_i, yMIN_i);
	Vector2d p1_ori = imgPos1 - minP;
	Vector2d p2_ori = imgPos2 - minP;
	Vector2d p3_ori = imgPos3 - minP;

	for (i = xMIN_i-1; i <= xMAX_i+1; i++)
	{
		for (j = yMIN_i-1; j <= yMAX_i+1; j++)
		{
			if (i >= 0 && i<width && j >= 0 && j<height)
			{
				if (CalcWeight(i - xMIN_i, j - yMIN_i, p1_ori, p2_ori, p3_ori, k, weight_a, weight_b, weight_c))
				{
					// 三顶点对i,j处的深度贡献值进行融合
					double depth_ij = weight_a * depth1 + weight_b * depth2 + weight_c * depth3;
					if (depth_ij <= ((double*)deepMap.data)[j*width + i])
					{
						((double*)deepMap.data)[j*width + i] = depth_ij;
						((int*)segMap.data)[j*width + i] = segIns;
					}
				}
			}
		}
	}
}

//将世界坐标系下 坐标world_pos转换为图片坐标系下坐标image_pos  (double focal_length 焦距, double pixel_rotio 比例, double skew 倾斜参数)
Vector2d projectiontool::World2ImageProjection(const Vector3d &worldPos, const SPhotoInfo &photoInfo, const SCameraParam &cameraParams)
{
	Vector3d cameraCenter(photoInfo.x, photoInfo.y, photoInfo.z);	//相机中心
	Vector2d principalPoint;		//图像主点
	principalPoint << cameraParams.principalPoint_X, cameraParams.principalPoint_Y;

	Matrix3d rotateMatrix;	//3*3旋转矩阵,代表相机的旋转
	double pitch = photoInfo.camera_pitch * M_PI / 180.0f;
	double yaw = photoInfo.camera_yaw * M_PI / 180.0f;
	double roll = photoInfo.camera_roll * M_PI / 180.0f;
	rotateMatrix << cos(roll)*cos(yaw) - sin(roll)*sin(pitch)*sin(yaw), -cos(roll)*sin(yaw) - cos(yaw)*sin(roll)*sin(pitch), cos(pitch)*sin(roll),
		cos(yaw)*sin(roll) + cos(roll)*sin(pitch)*sin(yaw), cos(roll)*cos(yaw)*sin(pitch) - sin(roll)*sin(yaw), -cos(roll)*cos(pitch),
		cos(pitch)*sin(yaw), cos(pitch)*cos(yaw), sin(pitch);

	Matrix2d focalMatrix;	//焦距（焦点）矩阵
	focalMatrix << cameraParams.focalLength_pixel, cameraParams.skew, 0.f, cameraParams.aspectRatio * cameraParams.focalLength_pixel;

	Matrix3d oriMatrix = OriMatrix(cameraParams.cameraOrientation); 	//3*3旋转矩阵,代表相机的旋转

	Vector3d orx_c = oriMatrix*rotateMatrix*(worldPos - cameraCenter);

	Vector2d orx_c_p;
	orx_c_p << orx_c[0] / orx_c[2], orx_c[1] / orx_c[2];

	Vector2d imagePos = focalMatrix * Distortion(orx_c_p, cameraParams) + principalPoint;

	//cout << "world_pos:" << endl << world_pos << endl << endl;
	//cout << "image_pos:" << endl << image_pos << endl << endl;

	return imagePos;
}

/*
计算三维世界坐标点到相机的深度值(根据ContextCapture导出的相机参数):
type=1表示按投影公式将顶点世界坐标转到相机坐标，然后计算到原点的距离
type=2表示直接计算顶点到相机中心点的距离
*/
double projectiontool::DisFromWorldPosToCameraAerial(const Vector3d &worldPos, const SPhotoInfo &photoInfo, const SCameraParam &cameraParams, int type)
{
	double distance = 0;
	Vector3d cameraCenter(photoInfo.x, photoInfo.y, photoInfo.z);	//相机中心
	if (type == 1)
	{
		Vector2d principalPoint;	//图像主点
		principalPoint << cameraParams.principalPoint_X, cameraParams.principalPoint_Y;
		Matrix3d rotateMatrix;	//3*3旋转矩阵,代表相机的旋转
		double pitch = photoInfo.camera_pitch * M_PI / 180.0f;
		double yaw = photoInfo.camera_yaw * M_PI / 180.0f;
		double roll = photoInfo.camera_roll * M_PI / 180.0f;
		rotateMatrix << cos(roll)*cos(yaw) - sin(roll)*sin(pitch)*sin(yaw), -cos(roll)*sin(yaw) - cos(yaw)*sin(roll)*sin(pitch), cos(pitch)*sin(roll),
			cos(yaw)*sin(roll) + cos(roll)*sin(pitch)*sin(yaw), cos(roll)*cos(yaw)*sin(pitch) - sin(roll)*sin(yaw), -cos(roll)*cos(pitch),
			cos(pitch)*sin(yaw), cos(pitch)*cos(yaw), sin(pitch);
		Vector3d rx_c = rotateMatrix*(worldPos - cameraCenter);
		distance = pow(rx_c[0], 2) + pow(rx_c[1], 2) + pow(rx_c[2], 2);
	}
	else if (type == 2)
	{
		distance = pow(worldPos[0] - cameraCenter[0], 2) + pow(worldPos[1] - cameraCenter[1], 2) + pow(worldPos[2] - cameraCenter[2], 2);
	}
	return distance;
}

void m_transform_point(GLdouble out[4], const GLdouble m[16], const GLdouble in[4])
{
#define M(row,col) m[col*4+row]  
	out[0] = M(0, 0) * in[0] + M(0, 1) * in[1] + M(0, 2) * in[2] + M(0, 3) * in[3];
	out[1] = M(1, 0) * in[0] + M(1, 1) * in[1] + M(1, 2) * in[2] + M(1, 3) * in[3];
	out[2] = M(2, 0) * in[0] + M(2, 1) * in[1] + M(2, 2) * in[2] + M(2, 3) * in[3];
	out[3] = M(3, 0) * in[0] + M(3, 1) * in[1] + M(3, 2) * in[2] + M(3, 3) * in[3];
#undef M  
}

Vector3d projectiontool::DisFromWorldPosToCameraGL(const Vector3d &worldPos, const SGlCameraParam &cameraParam, int type)
{
	//const GLdouble modelMatrix[16] = cameraParam.modelMatrix;
	//const GLdouble projMatrix[16] = cameraParam.projMatrix;
	//const GLint viewport[4] = cameraParam.viewport;
	Vector3d imagePosAndDepth(0, 0, 0);
	GLdouble winx;
	GLdouble winy;
	GLdouble winz;

	//GLdouble zNear = 0.01;
	//GLdouble zFar = 1000.0;

	GLdouble zNear = 10;
	GLdouble zFar = 200.0;

	GLdouble objCoor[4];
	GLdouble objProj[4], objModel[4];

	// 4x4 matrix must be multi to a 4 dimension vector( it a 1 x 4 matrix)  
	// so we need to put the original vertex to a 4D vector  
	objCoor[0] = worldPos[0];
	objCoor[1] = worldPos[1];
	objCoor[2] = worldPos[2];
	objCoor[3] = 1.0;

	// 由于原来的向量位于标准基向量(1, 0, 0), (0, 1, 0), (0, 0, 1)中，所以需要先转换到当前的模型矩阵中  
	m_transform_point(objModel, cameraParam.modelMatrix, objCoor);
	// 然后将模型矩阵中的顶点转换到投影矩阵所在坐标系的矩阵中  
	m_transform_point(objProj, cameraParam.projMatrix, objModel);
	// scale matrix  
	if (objProj[3] == 0.0)
		return imagePosAndDepth;

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

	winx = cameraParam.viewport[0] + objProj[0] * cameraParam.viewport[2];
	winy = cameraParam.viewport[1] + objProj[1] * cameraParam.viewport[3];
	winy = cameraParam.viewport[3] - winy;
	double ndcZ = (zNear + zFar) / (zFar - zNear) + (2 * zFar*zNear) / ((zFar - zNear)*viewZ);
	winz = ndcZ;
	imagePosAndDepth[0] = winx;
	imagePosAndDepth[1] = winy;
	imagePosAndDepth[2] = winz;
	return imagePosAndDepth;
	//*winz = objProj[2];
	//*winz = objProj[2] * objProj[3];
}

//获取分割后的模型反投影回三维场景后的图片
Mat projectiontool::GetBackProjectedImgAerial(const MeshGraph *meshGraph, const SPhotoInfo &photo_info, const SCameraParam &camera, const vector<int> &faceSegResult)
{
	int imgWidth = camera.imageDimensions_Width;
	int imgHeight = camera.imageDimensions_Height;

	Mat deepImage(imgHeight, imgWidth, CV_64F);
	double* pData = (double*)deepImage.data;
	for (int i = 0; i<imgHeight; i++)
	{
		for (int j = 0; j<imgWidth; j++)
		{
			*(pData) = 99999999999;
			pData++;
		}
	}

	Mat segImage(imgHeight, imgWidth, CV_32S);
	int* sData = (int*)segImage.data;
	//初始化为-1，表示所有像素都为背景
	for (int i = 0; i<imgHeight; i++)
	{
		for (int j = 0; j<imgWidth; j++)
		{
			*(sData) = -1;
			sData++;
		}
	}

	//计算模型每个顶点深度值(即顶点到相机中心点距离)
	const vector<SVertex> &vertics = meshGraph->vertices;
	int vSize = vertics.size();
	double* depthPerVertex = new double[vSize];
	for (int i = 0; i < vSize; i++)
	{
		Eigen::Vector3d vetPos(vertics[i].pos[0], vertics[i].pos[1], vertics[i].pos[2]);
		double distanceM2 = DisFromWorldPosToCameraAerial(vetPos, photo_info, camera);
		depthPerVertex[i] = sqrt(distanceM2);
	}

	//遍历所有三角网格,计算三角网格三个顶点在照片上的二维坐标,并对三个二维坐标点的三角范围内深度值进行线性插值
	const vector<STriangle> &faces = meshGraph->triangles;
	for (int i = 0; i < faces.size(); i++)
	{
		if (i == 400000 || i == 800000 || i == 1200000 || i == 1600000 || i == 2000000 || i == 2400000)
		{
			cout << "计算第 " << i << " 个面片" << endl;
		}

		Vector2d imgPosF1 = World2ImageProjection(Eigen::Vector3d(vertics[faces[i].vIndex[0]].pos[0], vertics[faces[i].vIndex[0]].pos[1], vertics[faces[i].vIndex[0]].pos[2]), photo_info, camera);
		Vector2d imgPosF2 = World2ImageProjection(Eigen::Vector3d(vertics[faces[i].vIndex[1]].pos[0], vertics[faces[i].vIndex[1]].pos[1], vertics[faces[i].vIndex[1]].pos[2]), photo_info, camera);
		Vector2d imgPosF3 = World2ImageProjection(Eigen::Vector3d(vertics[faces[i].vIndex[2]].pos[0], vertics[faces[i].vIndex[2]].pos[1], vertics[faces[i].vIndex[2]].pos[2]), photo_info, camera);

		//imgPosF1 = Vector2d(floor(imgPosF1[0]), floor(imgPosF1[1]));
		//imgPosF2 = Vector2d(floor(imgPosF2[0]), floor(imgPosF2[1]));
		//imgPosF3 = Vector2d(floor(imgPosF3[0]), floor(imgPosF3[1]));

		//三个点至少有一个点在图片范围内
		if (JudgeInImgRange(imgPosF1, imgPosF2, imgPosF3, camera.imageDimensions_Width, camera.imageDimensions_Height))
		{
			double depth1 = depthPerVertex[faces[i].vIndex[0]];
			double depth2 = depthPerVertex[faces[i].vIndex[1]];
			double depth3 = depthPerVertex[faces[i].vIndex[2]];
			int segIns = faceSegResult[i];

			//更新deepMap,根据imgPos1, imgPos2, imgPos3 以及depth1, depth2, depth3进行线性插值
			UpdateDepthMap(deepImage, segImage, imgPosF1, imgPosF2, imgPosF3, depth1, depth2, depth3, segIns, imgWidth, imgHeight);
		}
	}
	delete depthPerVertex;
	depthPerVertex = NULL;

	return segImage;
}

//获取分割后的模型反投影回三维场景后的图片_OpenGL
Mat projectiontool::GetBackProjectedImgOpenGL(const MeshGraph *meshGraph, const SGlCameraParam &cameParam, const vector<int> &faceSegResult)
{
	int imgWidth = cameParam.imageWidth;
	int imgHeight = cameParam.imageHeight;

	Mat deepImage(imgHeight, imgWidth, CV_64F);
	double* pData = (double*)deepImage.data;
	for (int i = 0; i<imgHeight; i++)
	{
		for (int j = 0; j<imgWidth; j++)
		{
			*(pData) = 99999999;
			pData++;
		}
	}

	Mat segImage(imgHeight, imgWidth, CV_32S);
	int* sData = (int*)segImage.data;
	//初始化为-1，表示所有像素都为背景
	for (int i = 0; i<imgHeight; i++)
	{
		for (int j = 0; j<imgWidth; j++)
		{
			*(sData) = -1;
			sData++;
		}
	}

	//计算模型每个顶点深度值(即顶点到相机中心点距离)
	const vector<SVertex> &vertices = meshGraph->vertices;
	int vSize = vertices.size();
	double* depthPerVertex = new double[vSize];
	for (int i = 0; i < vSize; i++)
	{
		Eigen::Vector3d vetPos(vertices[i].pos[0], vertices[i].pos[1], vertices[i].pos[2]);
		double distance = DisFromWorldPosToCameraGL(vetPos, cameParam, 1)[2];
		depthPerVertex[i] = distance;
	}

	//遍历所有三角网格,计算三角网格三个顶点在照片上的二维坐标,并对三个二维坐标点的三角范围内深度值进行线性插值
	const vector<STriangle> &faces = meshGraph->triangles;
	for (int i = 0; i < faces.size(); i++)
	{
		if (i == 400000 || i == 800000 || i == 1200000 || i == 1600000 || i == 2000000 || i == 2400000)
		{
			cout << "计算第 " << i << " 个面片" << endl;
		}

		Vector3d imgPosF1 = DisFromWorldPosToCameraGL(vertices[faces[i].vIndex[0]].pos, cameParam, 1);
		Vector3d imgPosF2 = DisFromWorldPosToCameraGL(vertices[faces[i].vIndex[1]].pos, cameParam, 1);
		Vector3d imgPosF3 = DisFromWorldPosToCameraGL(vertices[faces[i].vIndex[2]].pos, cameParam, 1);

		Vector2d imgPosF1_2d = Vector2d(imgPosF1[0], imgPosF1[1]);
		Vector2d imgPosF2_2d = Vector2d(imgPosF2[0], imgPosF2[1]);
		Vector2d imgPosF3_2d = Vector2d(imgPosF3[0], imgPosF3[1]);

		//三个点至少有一个点在图片范围内
		if (JudgeInImgRange(imgPosF1_2d, imgPosF2_2d, imgPosF3_2d, imgWidth, imgHeight))
		{
			double depth1 = depthPerVertex[faces[i].vIndex[0]];
			double depth2 = depthPerVertex[faces[i].vIndex[1]];
			double depth3 = depthPerVertex[faces[i].vIndex[2]];
			int segIns = faceSegResult[i];

			//更新deepMap,根据imgPos1, imgPos2, imgPos3 以及depth1, depth2, depth3进行线性插值
			UpdateDepthMap(deepImage, segImage, imgPosF1_2d, imgPosF2_2d, imgPosF3_2d, depth1, depth2, depth3, segIns, imgWidth, imgHeight);
		}
	}
	delete depthPerVertex;
	depthPerVertex = NULL;

	return segImage;
}