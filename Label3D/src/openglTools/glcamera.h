#ifndef CGLCamera_H
#define CGLCamera_H
#include <Eigen/Dense>
#include <GL/freeglut.h>
#include <glm/glm.hpp>
#include <iostream>

#define m_pi 3.14159265 
using namespace Eigen;
class CGLCamera
{
public:
	CGLCamera();
	CGLCamera(const Vector3d& pos, const Vector3d& target, const Vector3d& up);

	//inline void setViewAngle(float viewAngle) { this->m_viewAngle = viewAngle; };
	inline float getViewAngle() { return this->m_viewAngle; };

	void setEyePos(const Vector3d& pos);
	void setEyeTarget(const Vector3d& target);
	void setEyeUp(const Vector3d& up);
	void setEyePosTarUp(const Vector3d& pos, const Vector3d& target, const Vector3d& up);
	void updateCamera();

	inline Vector3d getEyePos() { return m_pos; };

	//设置模型视图矩阵
	void setModelViewMatrix();
	//设置摄像机的视角(viewAngle:视野角度, aspect:纵横比)
	void setShape(float viewAngle, float aspect, float Near, float far);
	//滑动
	void slide(float du, float dv, float dn);
	//roll，yaw，pitch相当于绕N，V，U轴的旋转
	void roll(float angle);
	void yaw(float angle);
	void pitch(float angle);
	float getDist();

private:
	Vector3d m_pos;
	Vector3d m_target;
	Vector3d m_up;
	Vector3d u, v, n;
	float m_viewAngle;
};

#if 0
class CCamera
{
public:
	CCamera();
	~CCamera();

	enum Type { PERSPECTIVE, ORTHOGRAPHIC };

public:
	glm::vec3 position() const;
	glm::vec3 upVector() const;
	glm::vec3 viewDirection() const;
	glm::vec3 rightVector() const;
	//glm::quat orientation() const;
	void setFromModelViewMatrix(const glm::mat4& mv);
	void set_from_calibration(float fx, float fy, float skew, float cx, float cy, const glm::vec3& rot, const glm::vec3& t);
	void set_from_projection_matrix(const glm::mat4& proj);
	void set_projection_matrix(const glm::mat4& proj);
	void set_modelview_matrix(const glm::mat4& mv);

public:
	void setPosition(const glm::vec3 &pos);
	void setOrientation(const glm::quat &q);
	void setOrientation(float theta, float phi);
	void setUpVector(const glm::vec3 &up, bool noMove = true);
	void setViewDirection(const glm::vec3 &direction);

	/*! @name Positioning tools */
public:
	void lookAt(const glm::vec3 &target);
	void showEntireScene();
	void fitSphere(const glm::vec3 &center, float radius);
	void fitBoundingBox(const glm::vec3 &min, const glm::vec3 &max);
	void fitScreenRegion(int xmin, int ymin, int xmax, int ymax);
	void centerScene();

public:
	//Returns the Camera::Type of the Camera.
	Type type() const { return type_; }
	//Use setFOVToFitScene() to adapt the fieldOfView() to a given scene. * /
	float fieldOfView() const { return fieldOfView_; }
	/*! Returns the horizontal field of view of the Camera (in radians).

	Value is set using setHorizontalFieldOfView() or setFieldOfView(). These
	values are always linked by: \code horizontalFieldOfView() = 2.0 * atan (
	tan(fieldOfView()/2.0) * aspectRatio() ). \endcode */
	float horizontalFieldOfView() const {
		return 2.0f * atan(tan(fieldOfView() / 2.0f) * aspectRatio());
	}

	float aspectRatio() const {
		return screenWidth_ / static_cast<float>(screenHeight_);
	}

	int screenWidth() const { return screenWidth_; }
	int screenHeight() const { return screenHeight_; }
	float pixelGLRatio(const glm::vec3 &position) const;
	float zNearCoefficient() const { return zNearCoef_; }

public:
	void setType(Type type);

	void setFieldOfView(float fov);
	void setHorizontalFieldOfView(float hfov) {
		setFieldOfView(2.0f * atan(tan(hfov / 2.0f) / aspectRatio()));
	}

	void setFOVToFitScene();

	void setAspectRatio(float aspect) {
		setScreenWidthAndHeight(int(100.0f * aspect), 100);
	}

	void setScreenWidthAndHeight(int width, int height);
	/*! Sets the zNearCoefficient() value. */
	void setZNearCoefficient(float coef) {
		zNearCoef_ = coef;
		projectionMatrixIsUpToDate_ = false;
	}
	/*! Sets the zClippingCoefficient() value. */
	void setZClippingCoefficient(float coef) {
		zClippingCoef_ = coef;
		projectionMatrixIsUpToDate_ = false;
	}

public:
	float sceneRadius() const { return sceneRadius_; }
	glm::vec3 sceneCenter() const { return sceneCenter_; }
	float distanceToSceneCenter() const;

public:
	void setSceneRadius(float radius);
	void setSceneCenter(const glm::vec3 &center);
	void setSceneBoundingBox(const glm::vec3 &min, const glm::vec3 &max);
	//@}

public:
	// The camera will move around the pivot point if set.
	void setPivotPoint(const glm::vec3 &point);
	glm::vec3 pivotPoint() const;

public:
	void computeProjectionMatrix();
	void computeModelViewMatrix();

	const glm::mat4& projectionMatrix() const;
	const glm::mat4& modelViewMatrix() const;
	glm::mat4 modelViewProjectionMatrix() const;

	/*! @name World to Camera coordinate systems conversions */
	//@{
public:
	glm::vec3 cameraCoordinatesOf(const glm::vec3 &src) const;
	glm::vec3 worldCoordinatesOf(const glm::vec3 &src) const;
	//@}



private:
	// Camera parameters
	int screenWidth_, screenHeight_; // size of the window, in pixels
	float fieldOfView_;              // in radians
	glm::vec3  sceneCenter_;
	float sceneRadius_; // OpenGL units
	float zNearCoef_;
	float zClippingCoef_;
	float orthoCoef_;
	Type type_;                 // PERSPECTIVE or ORTHOGRAPHIC
	glm::mat4 modelViewMatrix_;		// Buffered model view matrix.
	bool modelViewMatrixIsUpToDate_;
	glm::mat4 projectionMatrix_;		// Buffered projection matrix.
	bool projectionMatrixIsUpToDate_;
};
#endif // 0


#endif
