#include "glcamera.h"

CGLCamera::CGLCamera()
{
	this->m_pos = Vector3d(0, 0, 3);
	this->m_target = Vector3d(0, 0, 0);
	this->m_up = Vector3d(0, 1, 0);
	this->m_viewAngle = 60.0;

	//dot:点积, cross:叉积,a叉积b求得c向量为垂直于a,b的新向量
	n = Vector3d(m_pos.x() - m_target.x(), m_pos.y() - m_target.y(), m_pos.z() - m_target.z());
	u = Vector3d(m_up.cross(n).x(), m_up.cross(n).y(), m_up.cross(n).z());
	v = Vector3d(n.cross(u).x(), n.cross(u).y(), n.cross(u).z());

	n.normalize();
	u.normalize();
	v.normalize();

	setModelViewMatrix();
}

CGLCamera::CGLCamera(const Vector3d &pos, const Vector3d &target, const Vector3d &up) :
	m_pos(pos), m_target(target), m_up(up)
{
	this->m_viewAngle = 60.0;
	//dot:点积, cross:叉积,a叉积b求得c向量为垂直于a,b的新向量
	n = Vector3d(pos.x() - target.x(), pos.y() - target.y(), pos.z() - target.z());
	u = Vector3d(up.cross(n).x(), up.cross(n).y(), up.cross(n).z());
	v = Vector3d(n.cross(u).x(), n.cross(u).y(), n.cross(u).z());

	n.normalize();
	u.normalize();
	v.normalize();

	setModelViewMatrix();
}

void CGLCamera::setModelViewMatrix()
{
	double m[16];
	m[0] = u.x(); m[4] = u.y(); m[8] = u.z(); m[12] = -m_pos.dot(u);
	m[1] = v.x(); m[5] = v.y(); m[9] = v.z(); m[13] = -m_pos.dot(v);
	m[2] = n.x(); m[6] = n.y(); m[10] = n.z(); m[14] = -m_pos.dot(n);
	m[3] = 0;  m[7] = 0;  m[11] = 0;  m[15] = 1.0;
	glMatrixMode(GL_MODELVIEW);
	glLoadMatrixd(m);     //用M矩阵替换原视点矩阵
}

void CGLCamera::setShape(float viewAngle, float aspect, float Near, float Far)
{
	this->m_viewAngle = viewAngle;
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(viewAngle, aspect, Near, Far);
}

//滑动
void CGLCamera::slide(float du, float dv, float dn)
{
	//相当于是相机位置和相机看向的目标点在u,v,n方向上一起移动du,dv,dn个单位
	m_pos(0) += du*u.x() + dv*v.x() + dn*n.x();
	m_pos(1) += du*u.y() + dv*v.y() + dn*n.y();
	m_pos(2) += du*u.z() + dv*v.z() + dn*n.z();

	m_target(0) += du*u.x() + dv*v.x() + dn*n.x();
	m_target(1) += du*u.y() + dv*v.y() + dn*n.y();
	m_target(2) += du*u.z() + dv*v.z() + dn*n.z();

	setModelViewMatrix();
}

//绕n轴旋转
void CGLCamera::roll(float angle)
{
	float cs = cos(angle*m_pi / 180);
	float sn = sin(angle*m_pi / 180);
	Vector3d t(u);
	Vector3d s(v);
	u.x() = cs*t.x() - sn*s.x();
	u.y() = cs*t.y() - sn*s.y();
	u.z() = cs*t.z() - sn*s.z();

	v.x() = sn*t.x() + cs*s.x();
	v.y() = sn*t.y() + cs*s.y();
	v.z() = sn*t.z() + cs*s.z();

	setModelViewMatrix();          //每次计算完坐标轴变化后调用此函数更新视点矩阵  
}

//绕v轴旋转
void CGLCamera::yaw(float angle)
{
	float cs = cos(angle*m_pi / 180);
	float sn = sin(angle*m_pi / 180);
	Vector3d t(n);
	Vector3d s(u);

	n.x() = cs*t.x() - sn*s.x();
	n.y() = cs*t.y() - sn*s.y();
	n.z() = cs*t.z() - sn*s.z();

	u.x() = sn*t.x() + cs*s.x();
	u.y() = sn*t.y() + cs*s.y();
	u.z() = sn*t.z() + cs*s.z();

	setModelViewMatrix();
}

//绕u轴旋转
void CGLCamera::pitch(float angle)
{
	float cs = cos(angle*m_pi / 180);
	float sn = sin(angle*m_pi / 180);
	Vector3d t(v);
	Vector3d s(n);

	v.x() = cs*t.x() - sn*s.x();
	v.y() = cs*t.y() - sn*s.y();
	v.z() = cs*t.z() - sn*s.z();

	n.x() = sn*t.x() + cs*s.x();
	n.y() = sn*t.y() + cs*s.y();
	n.z() = sn*t.z() + cs*s.z();

	setModelViewMatrix();
}

//获取相机位置到目标点距离
float CGLCamera::getDist()
{
	float dist = pow(m_pos.x() - m_target.x(), 2) + pow(m_pos.y()- m_target.y(), 2) + pow(m_pos.z()- m_target.z(), 2);
	return pow(dist, 0.5);
}

void CGLCamera::setEyePos(const Vector3d& pos)
{
	this->m_pos = pos;
	updateCamera();
}

void CGLCamera::setEyeTarget(const Vector3d& target)
{
	this->m_target = target;
	updateCamera();
}
void CGLCamera::setEyeUp(const Vector3d& up)
{
	this->m_up = up;
	updateCamera();
}

void CGLCamera::setEyePosTarUp(const Vector3d& pos, const Vector3d& target, const Vector3d& up)
{
	this->m_pos = pos;
	this->m_target = target;
	this->m_up = up;
	updateCamera();
}

void CGLCamera::updateCamera()
{
	//dot:点积, cross:叉积,a叉积b求得c向量为垂直于a,b的新向量
	n = Vector3d(this->m_pos.x() - this->m_target.x(), this->m_pos.y() - this->m_target.y(), this->m_pos.z() - this->m_target.z());
	u = Vector3d(this->m_up.cross(n).x(), this->m_up.cross(n).y(), this->m_up.cross(n).z());
	v = Vector3d(n.cross(u).x(), n.cross(u).y(), n.cross(u).z());

	n.normalize();
	u.normalize();
	v.normalize();

	setModelViewMatrix();
}
#if 0

CCamera::CCamera()
	: fieldOfView_(static_cast<float>(M_PI / 4.0))
	, modelViewMatrixIsUpToDate_(false)
	, projectionMatrixIsUpToDate_(false)
{
	// Requires fieldOfView() to define focusDistance()
	setSceneRadius(1.0f);

	// Initial value (only scaled after this)
	orthoCoef_ = tan(fieldOfView() / 2.0f);

	// Also defines the pivotPoint(), which changes orthoCoef_. Requires a
	// frame().
	setSceneCenter(glm::vec3(0.0f, 0.0f, 0.0f));

	// Requires fieldOfView() when called with ORTHOGRAPHIC. Attention to
	// projectionMatrix_ below.
	setType(PERSPECTIVE);
	setViewDirection(glm::vec3(0.0, 1.0, 0.0));

	// #CONNECTION# initFromDOMElement default values
	setZNearCoefficient(0.005f);
	setZClippingCoefficient(sqrt(3.0f));

	// Dummy values
	setScreenWidthAndHeight(600, 400);

	modelViewMatrix_ = glm::mat4(1.0f);
	projectionMatrix_ = glm::mat4(0.f); // computeProjectionMatrix() is lazy and assumes 0.0 everywhere


	// position and orient the camera
	showEntireScene();
}

CCamera::~CCamera()
{
}

glm::vec3 CCamera::position() const
{
	return glm::vec3();
}

glm::vec3 CCamera::upVector() const
{
	return glm::vec3();
}

glm::vec3 CCamera::viewDirection() const
{
	return glm::vec3();
}

glm::vec3 CCamera::rightVector() const
{
	return glm::vec3();
}

//glm::quat CCamera::orientation() const
//{
//	return glm::quat();
//}

void CCamera::setFromModelViewMatrix(const glm::mat4 & mv)
{
}

void CCamera::set_from_calibration(float fx, float fy, float skew, float cx, float cy, const glm::vec3 & rot, const glm::vec3 & t)
{
}

void CCamera::set_from_projection_matrix(const glm::mat4 & proj)
{
}

void CCamera::set_projection_matrix(const glm::mat4 & proj)
{
}

void CCamera::set_modelview_matrix(const glm::mat4 & mv)
{
}

void CCamera::setPosition(const glm::vec3 & pos)
{
}

void CCamera::setOrientation(const glm::quat & q)
{
}

void CCamera::setOrientation(float theta, float phi)
{
}

void CCamera::setUpVector(const glm::vec3 & up, bool noMove)
{
}

void CCamera::setViewDirection(const glm::vec3 & direction)
{
}

void CCamera::lookAt(const glm::vec3 & target)
{
}

void CCamera::showEntireScene()
{
}

void CCamera::fitSphere(const glm::vec3 & center, float radius)
{
}

void CCamera::fitBoundingBox(const glm::vec3 & min, const glm::vec3 & max)
{
}

void CCamera::fitScreenRegion(int xmin, int ymin, int xmax, int ymax)
{
}

void CCamera::centerScene()
{
}

float CCamera::pixelGLRatio(const glm::vec3 & position) const
{
	return 0.0f;
}

void CCamera::setType(Type type)
{
}

void CCamera::setFieldOfView(float fov)
{
}

void CCamera::setFOVToFitScene()
{
}

void CCamera::setScreenWidthAndHeight(int width, int height)
{
}

float CCamera::distanceToSceneCenter() const
{
	return 0.0f;
}

void CCamera::setSceneRadius(float radius)
{
}

void CCamera::setSceneCenter(const glm::vec3 & center)
{
}

void CCamera::setSceneBoundingBox(const glm::vec3 & min, const glm::vec3 & max)
{
}

void CCamera::setPivotPoint(const glm::vec3 & point)
{
}

glm::vec3 CCamera::pivotPoint() const
{
	return glm::vec3();
}

void CCamera::computeProjectionMatrix()
{
}

void CCamera::computeModelViewMatrix()
{
}

const glm::mat4 & CCamera::projectionMatrix() const
{
	return glm::mat4();
}

const glm::mat4 & CCamera::modelViewMatrix() const
{
	return glm::mat4();
}

glm::mat4 CCamera::modelViewProjectionMatrix() const
{
	return glm::mat4();
}

glm::vec3 CCamera::cameraCoordinatesOf(const glm::vec3 & src) const
{
	return glm::vec3();
}

glm::vec3 CCamera::worldCoordinatesOf(const glm::vec3 & src) const
{
	return glm::vec3();
}

#endif // 0
