#pragma once
#include <GL/glew.h>
#include "glm.h"
//#include <easy3d/viewer/manipulated_camera_frame.h>
//#include <easy3d\viewer\camera.h>
//#include <easy3d\viewer\shader_program.h>
//#include <easy3d\viewer\shader_manager.h>
#include "easy3d/viewer/manipulated_camera_frame.h"
#include "easy3d\viewer\camera.h"
#include "easy3d\viewer\shader_program.h"
#include "easy3d\viewer\shader_manager.h"

using namespace std;
using namespace easy3d;

class CFacePicker
{
public:
	CFacePicker();
	~CFacePicker();

	//void Init(GLMmodel* model);

	//int pick_face_gpu(int x, int y, const Camera* camera);
	//void screen_to_opengl(int x, int y, int &gl_x, int &gl_y, int width, int height, int screenWidth, int screenHeight);


private:
	// the program handle
	GLuint VBO, VAO;
	unsigned int pickerProgram;

	int numVertices;
	GLfloat* vertices;

	easy3d::ShaderProgram *program;
	easy3d::Camera* camera_;
	int scr_width, scr_height;
};

CFacePicker::CFacePicker()
{
}

CFacePicker::~CFacePicker()
{
}
