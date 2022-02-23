#include "facePicker.h"

#if 0
void CFacePicker::Init(GLMmodel* model)
{
	glewInit();
	// create and setup the camera
	camera_ = new easy3d::Camera;
	camera_->setType(easy3d::Camera::PERSPECTIVE);
	camera_->setUpVector(easy3d::vec3(0, 0, 1)); // Z pointing up
	camera_->setViewDirection(easy3d::vec3(-1, 0, 0)); // X pointing out
	camera_->showEntireScene();
	camera_->setScreenWidthAndHeight(scr_width, scr_height);
	glViewport(0, 0, scr_width, scr_height);

	program = easy3d::ShaderManager::get_program("surface/surface_color");
	if (!program) {
		std::vector<ShaderProgram::Attribute> attributes;
		attributes.emplace_back(ShaderProgram::Attribute(ShaderProgram::POSITION, "vtx_position"));
		attributes.emplace_back(ShaderProgram::Attribute(ShaderProgram::COLOR, "vtx_color"));
		attributes.emplace_back(ShaderProgram::Attribute(ShaderProgram::NORMAL, "vtx_normal"));
		program = ShaderManager::create_program_from_files("surface/surface_color", attributes);
	}

	if (!program)
		return;

	const mat4 &MVP = camera_->modelViewProjectionMatrix();
	// camera position is defined in world coordinate system.
	const vec3 &wCamPos = camera_->position();
	// it can also be computed as follows:
	//const vec3& wCamPos = invMV * vec4(0, 0, 0, 1);
	const mat4 &MV = camera_->modelViewMatrix();
	const vec4 &wLightPos = inverse(MV) * setting_.light_position;

	program->bind();
	program->set_uniform("MVP", MVP);
	program->set_uniform("lighting", true);
	program->set_uniform("wLightPos", wLightPos);
	program->set_uniform("wCamPos", wCamPos);
	program->set_uniform("two_sides_lighting", setting_.light_two_sides);
	program->set_uniform("distinct_back_color", true);
	program->set_uniform("backside_color", setting_.background_color);
	program->set_uniform("smooth_shading", false);
	program->set_uniform("ssaoEnabled", false);
	program->set_uniform("per_vertex_color", true);
	program->set_uniform("default_color", setting_.default_color);
	program->set_block_uniform("Material", "ambient", easy3d::vec3(setting_.material_ambient));
	program->set_block_uniform("Material", "specular", easy3d::vec3(setting_.material_specular));
	program->set_block_uniform("Material", "shininess", &setting_.material_shininess);
	program->set_uniform("highlight", false);
	program->set_uniform("hightlight_id_min", -1);
	program->set_uniform("hightlight_id_max", -1);

	int numTriangles = model->numtriangles;
	//int numTriangles = 12;
	numVertices = numTriangles * 3;
	//初始化顶点
	vertices = new GLfloat[numVertices * 9];
	for (size_t i = 0; i < numTriangles; i++)
	{
		for (size_t k = 0; k < 3; k++)
		{
			int vIndex = model->triangles[i].vindices[k];
			//坐标
			vertices[9 * (3 * i + k) + 0] = model->vertices[3 * model->triangles[i].vindices[k] + 0];
			vertices[9 * (3 * i + k) + 1] = model->vertices[3 * model->triangles[i].vindices[k] + 1];
			vertices[9 * (3 * i + k) + 2] = model->vertices[3 * model->triangles[i].vindices[k] + 2];
			//颜色
			vertices[9 * (3 * i + k) + 3] = 0.65f;
			vertices[9 * (3 * i + k) + 4] = 0.65f;
			vertices[9 * (3 * i + k) + 5] = 0.65f;
			//面片法向量
			vertices[9 * (3 * i + k) + 6] = model->facetnorms[3 * model->triangles[i].findex + 0];
			vertices[9 * (3 * i + k) + 7] = model->facetnorms[3 * model->triangles[i].findex + 1];
			vertices[9 * (3 * i + k) + 8] = model->facetnorms[3 * model->triangles[i].findex + 2];

			//坐标
			//vertices[6 * (3 * i + k) + 0] = model->vertices[3 * model->triangles[i].vindices[k] + 0];
			//vertices[6 * (3 * i + k) + 1] = model->vertices[3 * model->triangles[i].vindices[k] + 1];
			//vertices[6 * (3 * i + k) + 2] = model->vertices[3 * model->triangles[i].vindices[k] + 2];
			//////颜色
			////vertices[6 * (3 * i + k) + 3] = 1;
			////vertices[6 * (3 * i + k) + 4] = 0;
			////vertices[6 * (3 * i + k) + 5] = 0;
			////面片法向量
			//vertices[6 * (3 * i + k) + 3] = model->facetnorms[3 * model->triangles[i].findex + 0];
			//vertices[6 * (3 * i + k) + 4] = model->facetnorms[3 * model->triangles[i].findex + 1];
			//vertices[6 * (3 * i + k) + 5] = model->facetnorms[3 * model->triangles[i].findex + 2];
		}
	}

	// first, configure the cube's VAO (and VBO)
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	int size_v = sizeof(vertices);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 9 * numVertices, vertices, GL_STATIC_DRAW);

	glBindVertexArray(VAO);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// normal attribute
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);
}
#endif // 0




#if 0

int CFacePicker::pick_face_gpu(int x, int y, const Camera* camera)
{
	int viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	int width = viewport[2];
	int height = viewport[3];
	//setup_framebuffer(width, height);

	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glm::mat4 MVP = glm::mat4(1.0f);
	shader->use();
	shader->setMat4("MVP", MVP);

	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, numVertices);

	glDisableVertexAttribArray(0);

	int gl_x, gl_y;
	screen_to_opengl(x, y, gl_x, gl_y, width, height, 100, 100);

	unsigned char c[4];
	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	glReadPixels(gl_x, gl_y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, c);

	return 1;
}

void CFacePicker::screen_to_opengl(int x, int y, int &gl_x, int &gl_y, int width, int height, int screenWidth, int screenHeight)
{
	float dpi_scaling_x = width / static_cast<float>(screenWidth);
	float dpi_scaling_y = height / static_cast<float>(screenHeight);

	gl_x = static_cast<int>(dpi_scaling_x * x);
	gl_y = static_cast<int>(dpi_scaling_y * (screenHeight - 1 - y));
}
#endif // 0
