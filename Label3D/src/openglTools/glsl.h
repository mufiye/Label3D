#pragma once
#include "shader_m.h"
//#include <GLFW/glfw3.h>
#include <string>
#include <Eigen\Dense>
#include <vector>
#include <queue>
#include <iostream>
#include "glm.h"
//#include <gl/glut.h>
//#include <glm/glm.hpp>
//#include <glm/gtc/matrix_transform.hpp>
//#include <glm/gtc/type_ptr.hpp>
//#include <learnopengl/camera.h>
//#include <easy3d/core/types.h>
//#include <easy3d/viewer/frame.h>
#include <easy3d/viewer/manipulated_camera_frame.h>
#include <easy3d\viewer\camera.h>
#include <easy3d\viewer\shader_program.h>
#include <easy3d\viewer\shader_manager.h>
#include <easy3d/viewer/framebuffer_object.h>
#include <easy3d/viewer/texture.h>
#include "../annotation.h"
#include <algorithm> 
//#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

using namespace std;
using namespace easy3d;

class CGLSL
{
public:
	CGLSL();
	~CGLSL();

public:

	struct Setting
	{
		// ui
		easy3d::vec4 background_color = easy3d::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		//easy3d::vec4 background_color = easy3d::vec4(0.8f, 0.8f, 0.8f, 1.0f);
		easy3d::vec4 default_color = easy3d::vec4(0.67f, 0.67f, 0.67f, 1.0f);

		// lighting
		easy3d::vec4 light_position = easy3d::vec4(0.27f, 0.27f, 0.92f, 0.0f);
		bool light_distinct_back_color = false;
		easy3d::vec4 light_back_color = easy3d::vec4(0.8f, 0.4f, 0.4f, 1.0f);
		bool light_two_sides = false;

		// material
		easy3d::vec4 material_ambient = easy3d::vec4(0.05f, 0.05f, 0.05f, 1.0f);
		easy3d::vec4 material_specular = easy3d::vec4(0.4f, 0.4f, 0.4f, 1.0f);
		float material_shininess = 64.0f;

		// effect
		int effect_ssao_algorithm = 0; // disabled
		float effect_ssao_radius = 2.0f;
		float effect_ssao_intensity = 1.5f;
		float effect_ssao_bias = 0.1f;
		float effect_ssao_sharpness = 40.0f;
		bool effect_edl_enabled = false;
		bool effect_transparency_enabled = false;
		bool effect_shadow_enabled = false;
		float shadow_light_distance = 50.0f;
		float shadow_softness = 0.5f;
		float shadow_darkness = 0.6f;

		// point cloud
		bool point_cloud_use_color_property = true;
		bool point_cloud_show_points = true;
		easy3d::vec4 point_cloud_points_color = easy3d::vec4(85 / 255.0f, 170 / 255.0f, 1.0f, 1.0f);
		float point_cloud_point_size = 2.0f;
		bool point_cloud_impostors = false;

		// surface mesh - surface
		bool surface_mesh_phong_shading = false;
		bool surface_mesh_show_faces = true;
		easy3d::vec4 surface_mesh_faces_color = easy3d::vec4(0.8f, 0.88f, 1.0f, 1.0f);
		bool surface_mesh_use_color_property = true;
		float surface_mesh_opacity = 0.6f;

		// surface mesh - vertices
		bool surface_mesh_show_vertices = false;
		easy3d::vec4 surface_mesh_vertices_color = easy3d::vec4(0.0f, 1.0f, 0.0f, 1.0f);
		bool surface_mesh_vertices_imposters = true;
		float surface_mesh_vertices_point_size = 5.0f;

		// surface mesh - edges
		bool surface_mesh_show_edges = false;
		easy3d::vec4 surface_mesh_edges_color = easy3d::vec4(0.0f, 0.0f, 0.0f, 1.0f);
		bool surface_mesh_edges_imposters = false;
		float surface_mesh_edges_line_width = 1.0f;

		// surface mesh - borders
		bool surface_mesh_show_borders = false;
		easy3d::vec4 surface_mesh_borders_color = easy3d::vec4(1.0f, 0.0f, 0.0f, 1.0f);
		bool surface_mesh_borders_imposters = true;
		float surface_mesh_borders_line_width = 2.0f;
	};

	typedef struct
	{
		GLfloat x;
		GLfloat y;
		GLfloat z;
	} POINT_S;

public:
	void Init(GLMmodel* model);
	void Init_Easy3d(GLMmodel* model);
	void InitPicker();
	void InitTextureShader(const GLMmodel* model);
	void InitTextureColorShader(const GLMmodel* model);

	void Display(Camera* camera);
	void Display_color(Camera* camera);
	void Display_line(Camera* camera);
	void Display_texture(Camera* camera);
	void Display_textureColor(Camera* camera);
	void Display_picker(Camera* camera);

	void setup_framebuffer(int width, int height);
	void screen_to_opengl(int x, int y, int &gl_x, int &gl_y, int width, int height, int screenWidth, int screenHeight);
	int PickFace(int x, int y);
	//按半径区域选择面片
	vector<int> PickFaces_Radius(int x, int y, int radius);
	//按矩形区域选择面片
	vector<int> PickFaces_Rect(const easy3d::ivec2& p1, const easy3d::ivec2& p2);
	vector<int> PickFaces_Rect2(const easy3d::ivec2& p1, const easy3d::ivec2& p2);
	void SetFaceHighlight(int faceIndex);

	//相机复位
	void ResetCamera();

public:
	//根据标注信息更新顶点数据缓存
	void UpdateVBuffer(const LabelInfo* labelInfo);
	////设置单个顶点颜色
	//void SetVColor(int faceIndex, const Vector3f &color, bool ifBackGround = false);
	//设置顶点数组颜色
	void SetVColor(const vector<int>& faces, const Vector3f &color, bool ifBackGround = false);
	//设置顶点数组颜色
	void SetVColor(const vector<vector<int>>& faces, const Vector3f &color, bool ifBackGround = false);

public:
	GLMmodel* model;
	GLuint vertexShaderID;
	GLuint fragmentShaderID;

	GLuint vao_color, vao_line, vao_texture, vao_picker, vao_textureColor;
	GLuint vbo_color, vbo_line, vbo_texture, vbo_picker, vbo_textureColor;
	//GLuint ibo_color, ibo_line, ibo_texture, ibo_picker;
	GLuint ebo;

	int numVertices;
	GLfloat* vertices_color;
	GLfloat* vertices_texture;
	GLfloat* vertices_line;
	GLfloat* vertices_picker;
	GLfloat* vertices_textureColor;
	unsigned int *v_indices;

	easy3d::ShaderProgram *colorProgram;
	easy3d::ShaderProgram *lineProgram;
	easy3d::ShaderProgram *textureProgram;
	easy3d::ShaderProgram *pickProgram;
	easy3d::ShaderProgram *textureColorProgram;
	//Shader* colorShader;
	//easy3d::Texture *texture;
	easy3d::Camera* camera_;
	Setting setting_;
	int scr_width, scr_height;

	FramebufferObject* fbo_;

	int selectFaceIndex;

	int numTextures;
	GLuint textures[5];//纹理也是使用ID引用的
};



