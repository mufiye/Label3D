#include "glsl.h"

CGLSL::CGLSL() :
	model(nullptr),
	colorProgram(nullptr),
	lineProgram(nullptr),
	textureProgram(nullptr),
	textureColorProgram(nullptr),
	pickProgram(nullptr),
	//texture(nullptr),
	camera_(nullptr),
	fbo_(nullptr),
	vertices_color(nullptr),
	vertices_texture(nullptr),
	vertices_line(nullptr),
	vertices_picker(nullptr),
	vertices_textureColor(nullptr),
	v_indices(nullptr),
	numVertices(0),
	scr_width(0),
	scr_height(0),
	selectFaceIndex(-1),
	numTextures(5)
{
	for (int i = 0; i < 5; i++)
	{
		textures[i] = (GLuint)i;
	}
	camera_ = new easy3d::Camera;
	camera_->frame()->setZoomsOnPivotPoint(true);
}

CGLSL::~CGLSL()
{
	delete[] vertices_color;
	vertices_color = nullptr;
	delete[] vertices_texture;
	vertices_texture = nullptr;
	delete[] vertices_line;
	vertices_line = nullptr;
	delete[] vertices_picker;
	vertices_picker = nullptr;
	delete[] vertices_textureColor;
	vertices_textureColor = nullptr;
	delete[] v_indices;
	v_indices = nullptr;
	delete[] textures;

	model = nullptr;

	if (colorProgram)
	{
		colorProgram->release();
		delete colorProgram;
	}
	colorProgram = nullptr;

	if (lineProgram)
	{
		lineProgram->release();
		delete lineProgram;
	}
	lineProgram = nullptr;

	if (textureProgram)
	{
		textureProgram->release();
		delete textureProgram;
	}
	textureProgram = nullptr;

	if (pickProgram)
	{
		pickProgram->release();
		delete pickProgram;
	}
	pickProgram = nullptr;

	if (textureColorProgram)
	{
		textureColorProgram->release();
		delete textureColorProgram;
	}
	textureColorProgram = nullptr;

	if (camera_)
	{
		delete camera_;
	}
	camera_ = nullptr;

	if (fbo_)
	{
		delete fbo_;
	}
	fbo_ = nullptr;
}

void CGLSL::Init(GLMmodel* model)
{
	this->model = model;
	glewInit();

	// create and setup the camera
	camera_ = new easy3d::Camera;
	//camera_->frame()->setZoomsOnPivotPoint(false);
	camera_->frame()->setZoomsOnPivotPoint(true);
	camera_->setType(easy3d::Camera::PERSPECTIVE);
	camera_->setUpVector(easy3d::vec3(0, 0, 1)); // Z pointing up
	camera_->setViewDirection(easy3d::vec3(-1, 0, 0)); // X pointing out
	camera_->showEntireScene();

	colorProgram = easy3d::ShaderManager::get_program("F:/StudyDemo/Label3D/Label3D/src/shader/surface/surface_color");
	colorProgram->bind();

	textureProgram = easy3d::ShaderManager::get_program("F:/StudyDemo/Label3D/Label3D/src/shader/surface/surface_texture");
	textureProgram->bind();

	int numTriangles = model->numtriangles;
	//int numTriangles = 12;
	numVertices = numTriangles * 3;

	//初始化顶点
	vertices_color = new GLfloat[numVertices * 9];
	vertices_line = new GLfloat[numVertices * 9];
	for (size_t i = 0; i < numTriangles; i++)
	{
		for (size_t k = 0; k < 3; k++)
		{
			int vIndex = model->triangles[i].vindices[k];
			//坐标
			vertices_color[9 * (3 * i + k) + 0] = model->vertices[3 * model->triangles[i].vindices[k] + 0];
			vertices_color[9 * (3 * i + k) + 1] = model->vertices[3 * model->triangles[i].vindices[k] + 1];
			vertices_color[9 * (3 * i + k) + 2] = model->vertices[3 * model->triangles[i].vindices[k] + 2];
			//颜色
			vertices_color[9 * (3 * i + k) + 3] = setting_.default_color.x;
			vertices_color[9 * (3 * i + k) + 4] = setting_.default_color.y;
			vertices_color[9 * (3 * i + k) + 5] = setting_.default_color.z;
			//面片法向量
			vertices_color[9 * (3 * i + k) + 6] = model->facetnorms[3 * model->triangles[i].findex + 0];
			vertices_color[9 * (3 * i + k) + 7] = model->facetnorms[3 * model->triangles[i].findex + 1];
			vertices_color[9 * (3 * i + k) + 8] = model->facetnorms[3 * model->triangles[i].findex + 2];

			//坐标
			vertices_line[9 * (3 * i + k) + 0] = model->vertices[3 * model->triangles[i].vindices[k] + 0];
			vertices_line[9 * (3 * i + k) + 1] = model->vertices[3 * model->triangles[i].vindices[k] + 1];
			vertices_line[9 * (3 * i + k) + 2] = model->vertices[3 * model->triangles[i].vindices[k] + 2];
			//颜色
			vertices_line[9 * (3 * i + k) + 3] = 0.f;
			vertices_line[9 * (3 * i + k) + 4] = 0.f;
			vertices_line[9 * (3 * i + k) + 5] = 0.f;
			//面片法向量
			vertices_line[9 * (3 * i + k) + 6] = model->facetnorms[3 * model->triangles[i].findex + 0];
			vertices_line[9 * (3 * i + k) + 7] = model->facetnorms[3 * model->triangles[i].findex + 1];
			vertices_line[9 * (3 * i + k) + 8] = model->facetnorms[3 * model->triangles[i].findex + 2];
		}
	}

	// first, configure the cube's VAO (and VBO)
	glGenVertexArrays(1, &vao_color);
	glGenBuffers(1, &vbo_color);

	glBindBuffer(GL_ARRAY_BUFFER, vbo_color);
	int size_v = sizeof(vertices_color);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 9 * numVertices, vertices_color, GL_STATIC_DRAW);

	glBindVertexArray(vao_color);

	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// normal attribute
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

#if 0
	// positions of the point lights
	glm::vec3 pointLightPositions[] = {
		glm::vec3(0.7f,  0.2f,  2.0f),
		glm::vec3(2.3f, -3.3f, -4.0f),
		glm::vec3(-4.0f,  2.0f, -12.0f),
		glm::vec3(0.0f,  0.0f, -3.0f)
	};

	colorShader->use();
	colorShader->setInt("material.diffuse", 0);
	colorShader->setInt("material.specular", 1);

	colorShader->setVec3("dirLight.direction", -0.2f, -1.0f, -0.3f);
	colorShader->setVec3("dirLight.ambient", 0.05f, 0.05f, 0.05f);
	colorShader->setVec3("dirLight.diffuse", 0.4f, 0.4f, 0.4f);
	colorShader->setVec3("dirLight.specular", 0.5f, 0.5f, 0.5f);
	// point light 1
	colorShader->setVec3("pointLights[0].position", pointLightPositions[0]);
	colorShader->setVec3("pointLights[0].ambient", 0.05f, 0.05f, 0.05f);
	colorShader->setVec3("pointLights[0].diffuse", 0.8f, 0.8f, 0.8f);
	colorShader->setVec3("pointLights[0].specular", 1.0f, 1.0f, 1.0f);
	colorShader->setFloat("pointLights[0].constant", 1.0f);
	colorShader->setFloat("pointLights[0].linear", 0.09);
	colorShader->setFloat("pointLights[0].quadratic", 0.032);
	//point light 2
	colorShader->setVec3("pointLights[1].position", pointLightPositions[1]);
	colorShader->setVec3("pointLights[1].ambient", 0.05f, 0.05f, 0.05f);
	colorShader->setVec3("pointLights[1].diffuse", 0.8f, 0.8f, 0.8f);
	colorShader->setVec3("pointLights[1].specular", 1.0f, 1.0f, 1.0f);
	colorShader->setFloat("pointLights[1].constant", 1.0f);
	colorShader->setFloat("pointLights[1].linear", 0.09);
	colorShader->setFloat("pointLights[1].quadratic", 0.032);
	// point light 3
	colorShader->setVec3("pointLights[2].position", pointLightPositions[2]);
	colorShader->setVec3("pointLights[2].ambient", 0.05f, 0.05f, 0.05f);
	colorShader->setVec3("pointLights[2].diffuse", 0.8f, 0.8f, 0.8f);
	colorShader->setVec3("pointLights[2].specular", 1.0f, 1.0f, 1.0f);
	colorShader->setFloat("pointLights[2].constant", 1.0f);
	colorShader->setFloat("pointLights[2].linear", 0.09);
	colorShader->setFloat("pointLights[2].quadratic", 0.032);
	// point light 4
	colorShader->setVec3("pointLights[3].position", pointLightPositions[3]);
	colorShader->setVec3("pointLights[3].ambient", 0.05f, 0.05f, 0.05f);
	colorShader->setVec3("pointLights[3].diffuse", 0.8f, 0.8f, 0.8f);
	colorShader->setVec3("pointLights[3].specular", 1.0f, 1.0f, 1.0f);
	colorShader->setFloat("pointLights[3].constant", 1.0f);
	colorShader->setFloat("pointLights[3].linear", 0.09);
	colorShader->setFloat("pointLights[3].quadratic", 0.032);


	//// position attribute
	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	//glEnableVertexAttribArray(0);
	//// normal attribute
	//glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	//glEnableVertexAttribArray(1);

	/*glClearColor(0.1f, 0.1f, 0.1f, 1.0f);*/
	glClearColor(1.f, 1.f, 1.f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// be sure to activate shader when setting uniforms/drawing objects
	colorShader->use();
	colorShader->setVec3("light.position", glm::vec3(1.2f, 1.0f, 2.0f));
	colorShader->setVec3("viewPos", camera->Position);

	// light properties
	glm::vec3 lightColor;
	lightColor.x = sin(2.0f);
	lightColor.y = sin(0.7f);
	lightColor.z = sin(1.3f);
	glm::vec3 diffuseColor = lightColor   * glm::vec3(0.5f); // decrease the influence
	glm::vec3 ambientColor = diffuseColor * glm::vec3(0.2f); // low influence
	colorShader->setVec3("light.ambient", ambientColor);
	colorShader->setVec3("light.diffuse", diffuseColor);
	colorShader->setVec3("light.specular", 1.0f, 1.0f, 1.0f);

	// material properties
	colorShader->setVec3("material.ambient", 1.0f, 0.5f, 0.31f);
	colorShader->setVec3("material.diffuse", 1.0f, 0.5f, 0.31f);
	colorShader->setVec3("material.specular", 0.5f, 0.5f, 0.5f); // specular lighting doesn't have full effect on this object's material
	colorShader->setFloat("material.shininess", 32.0f);

	// view/projection transformations
	glm::mat4 projection = glm::perspective(glm::radians(camera->Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
	glm::mat4 view = camera->GetViewMatrix();
	colorShader->setMat4("projection", projection);
	colorShader->setMat4("view", view);

	// world transformation
	glm::mat4 modelMatrix = glm::mat4(1.0f);
	colorShader->setMat4("model", modelMatrix);
#endif // 0
}

void CGLSL::Init_Easy3d(GLMmodel* model)
{
	this->model = model;
	glewInit();
	// create and setup the camera
	if (camera_)
	{
		delete camera_;
		camera_ = NULL;
	}
	camera_ = new easy3d::Camera;
	camera_->frame()->setZoomsOnPivotPoint(true);
	camera_->setType(easy3d::Camera::PERSPECTIVE);
	camera_->setUpVector(easy3d::vec3(0, 0, 1)); // Z pointing up
	camera_->setViewDirection(easy3d::vec3(-1, 0, 0)); // X pointing out
	camera_->showEntireScene();
	camera_->setScreenWidthAndHeight(scr_width, scr_height);
	glViewport(0, 0, scr_width, scr_height);

	//colorProgram = easy3d::ShaderManager::get_program("surface/surface_color");
	if (colorProgram == nullptr) {
		std::vector<ShaderProgram::Attribute> attributes;
		attributes.emplace_back(ShaderProgram::Attribute(ShaderProgram::POSITION, "vtx_position"));
		attributes.emplace_back(ShaderProgram::Attribute(ShaderProgram::COLOR, "vtx_color"));
		attributes.emplace_back(ShaderProgram::Attribute(ShaderProgram::NORMAL, "vtx_normal"));
		colorProgram = ShaderManager::create_program_from_files("surface/surface_color", attributes);
	}
	if (!colorProgram)
		return;

	//lineProgram = easy3d::ShaderManager::get_program("lines/lines_plain_color");
	if (lineProgram == nullptr) {
		std::vector<ShaderProgram::Attribute> attributes;
		attributes.emplace_back(ShaderProgram::Attribute(ShaderProgram::POSITION, "vtx_position"));
		attributes.emplace_back(ShaderProgram::Attribute(ShaderProgram::COLOR, "vtx_color"));
		lineProgram = ShaderManager::create_program_from_files("lines/lines_plain_color", attributes);
	}
	if (!lineProgram)
		return;

	const mat4 &MVP = camera_->modelViewProjectionMatrix();
	// camera position is defined in world coordinate system.
	const vec3 &wCamPos = camera_->position();
	// it can also be computed as follows:
	//const vec3& wCamPos = invMV * vec4(0, 0, 0, 1);
	const mat4 &MV = camera_->modelViewMatrix();
	const vec4 &wLightPos = inverse(MV) * setting_.light_position;

	colorProgram->bind();
	colorProgram->set_uniform("MVP", MVP);
	colorProgram->set_uniform("lighting", true);
	colorProgram->set_uniform("wLightPos", wLightPos);
	colorProgram->set_uniform("wCamPos", wCamPos);
	colorProgram->set_uniform("two_sides_lighting", setting_.light_two_sides);
	colorProgram->set_uniform("distinct_back_color", true);
	colorProgram->set_uniform("backside_color", setting_.background_color);
	colorProgram->set_uniform("smooth_shading", false);
	colorProgram->set_uniform("ssaoEnabled", false);
	colorProgram->set_uniform("per_vertex_color", true);
	colorProgram->set_uniform("default_color", setting_.default_color);
	colorProgram->set_block_uniform("Material", "ambient", easy3d::vec3(setting_.material_ambient));
	colorProgram->set_block_uniform("Material", "specular", easy3d::vec3(setting_.material_specular));
	colorProgram->set_block_uniform("Material", "shininess", &setting_.material_shininess);
	colorProgram->set_uniform("highlight", false);
	colorProgram->set_uniform("hightlight_id_min", -1);
	colorProgram->set_uniform("hightlight_id_max", -1);

	lineProgram->bind();
	lineProgram->set_uniform("MVP", MVP)
		->set_uniform("per_vertex_color", 0);

	int numTriangles = model->numtriangles;
	//int numTriangles = 12;
	numVertices = numTriangles * 3;
	//初始化顶点
	vertices_color = new GLfloat[numVertices * 9];
	for (size_t i = 0; i < numTriangles; i++)
	{
		for (size_t k = 0; k < 3; k++)
		{
			int vIndex = model->triangles[i].vindices[k];
			//坐标
			vertices_color[9 * (3 * i + k) + 0] = model->vertices[3 * model->triangles[i].vindices[k] + 0];
			vertices_color[9 * (3 * i + k) + 1] = model->vertices[3 * model->triangles[i].vindices[k] + 1];
			vertices_color[9 * (3 * i + k) + 2] = model->vertices[3 * model->triangles[i].vindices[k] + 2];
			//颜色
			vertices_color[9 * (3 * i + k) + 3] = setting_.default_color.x;
			vertices_color[9 * (3 * i + k) + 4] = setting_.default_color.y;
			vertices_color[9 * (3 * i + k) + 5] = setting_.default_color.z;
			//面片法向量
			vertices_color[9 * (3 * i + k) + 6] = model->facetnorms[3 * model->triangles[i].findex + 0];
			vertices_color[9 * (3 * i + k) + 7] = model->facetnorms[3 * model->triangles[i].findex + 1];
			vertices_color[9 * (3 * i + k) + 8] = model->facetnorms[3 * model->triangles[i].findex + 2];
		}
	}

	vertices_line = new GLfloat[numVertices * 12];
	for (size_t ti = 0; ti < numTriangles; ti++)
	{
		vertices_line[ti * 36 + 0] = model->vertices[3 * model->triangles[ti].vindices[0] + 0];
		vertices_line[ti * 36 + 1] = model->vertices[3 * model->triangles[ti].vindices[0] + 1];
		vertices_line[ti * 36 + 2] = model->vertices[3 * model->triangles[ti].vindices[0] + 2];
		vertices_line[ti * 36 + 3] = 0.f;
		vertices_line[ti * 36 + 4] = 0.f;
		vertices_line[ti * 36 + 5] = 0.f;

		vertices_line[ti * 36 + 6] = model->vertices[3 * model->triangles[ti].vindices[1] + 0];
		vertices_line[ti * 36 + 7] = model->vertices[3 * model->triangles[ti].vindices[1] + 1];
		vertices_line[ti * 36 + 8] = model->vertices[3 * model->triangles[ti].vindices[1] + 2];
		vertices_line[ti * 36 + 9] = 0.f;
		vertices_line[ti * 36 + 10] = 0.f;
		vertices_line[ti * 36 + 11] = 0.f;

		vertices_line[ti * 36 + 12] = model->vertices[3 * model->triangles[ti].vindices[1] + 0];
		vertices_line[ti * 36 + 13] = model->vertices[3 * model->triangles[ti].vindices[1] + 1];
		vertices_line[ti * 36 + 14] = model->vertices[3 * model->triangles[ti].vindices[1] + 2];
		vertices_line[ti * 36 + 15] = 0.f;
		vertices_line[ti * 36 + 16] = 0.f;
		vertices_line[ti * 36 + 17] = 0.f;

		vertices_line[ti * 36 + 18] = model->vertices[3 * model->triangles[ti].vindices[2] + 0];
		vertices_line[ti * 36 + 19] = model->vertices[3 * model->triangles[ti].vindices[2] + 1];
		vertices_line[ti * 36 + 20] = model->vertices[3 * model->triangles[ti].vindices[2] + 2];
		vertices_line[ti * 36 + 21] = 0.f;
		vertices_line[ti * 36 + 22] = 0.f;
		vertices_line[ti * 36 + 23] = 0.f;

		vertices_line[ti * 36 + 24] = model->vertices[3 * model->triangles[ti].vindices[2] + 0];
		vertices_line[ti * 36 + 25] = model->vertices[3 * model->triangles[ti].vindices[2] + 1];
		vertices_line[ti * 36 + 26] = model->vertices[3 * model->triangles[ti].vindices[2] + 2];
		vertices_line[ti * 36 + 27] = 0.f;
		vertices_line[ti * 36 + 28] = 0.f;
		vertices_line[ti * 36 + 29] = 0.f;

		vertices_line[ti * 36 + 30] = model->vertices[3 * model->triangles[ti].vindices[0] + 0];
		vertices_line[ti * 36 + 31] = model->vertices[3 * model->triangles[ti].vindices[0] + 1];
		vertices_line[ti * 36 + 32] = model->vertices[3 * model->triangles[ti].vindices[0] + 2];
		vertices_line[ti * 36 + 33] = 0.f;
		vertices_line[ti * 36 + 34] = 0.f;
		vertices_line[ti * 36 + 35] = 0.f;
	}

	vertices_picker = new GLfloat[numVertices * 3];
	for (size_t i = 0; i < numTriangles; i++)
	{
		for (size_t k = 0; k < 3; k++)
		{
			int vIndex = model->triangles[i].vindices[k];
			//坐标
			vertices_picker[3 * (3 * i + k) + 0] = model->vertices[3 * model->triangles[i].vindices[k] + 0];
			vertices_picker[3 * (3 * i + k) + 1] = model->vertices[3 * model->triangles[i].vindices[k] + 1];
			vertices_picker[3 * (3 * i + k) + 2] = model->vertices[3 * model->triangles[i].vindices[k] + 2];
		}
	}

	v_indices = new unsigned int[numVertices];
	for (size_t i = 0; i < numVertices; i++)
	{
		v_indices[i] = i;
	}

	// first, configure the cube's VAO (and VBO)
	glGenVertexArrays(1, &vao_color);
	glGenVertexArrays(1, &vao_line);
	glGenVertexArrays(1, &vao_picker);

	glGenBuffers(1, &vbo_color);
	glGenBuffers(1, &vbo_line);
	glGenBuffers(1, &vbo_picker);

	glGenBuffers(1, &ebo);

	// 1. 绑定顶点数组对象
	glBindVertexArray(vao_color);
	// 2. 把我们的顶点数组复制到一个顶点缓冲中，供OpenGL使用
	glBindBuffer(GL_ARRAY_BUFFER, vbo_color);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 9 * numVertices, vertices_color, GL_STATIC_DRAW);

	// 3. 复制我们的索引数组到一个索引缓冲中，供OpenGL使用()
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * numVertices, v_indices, GL_STATIC_DRAW);

	//4. 设定顶点属性指针
	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// normal attribute
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	// 1. 绑定顶点数组对象
	glBindVertexArray(vao_line);
	// 2. 把我们的顶点数组复制到一个顶点缓冲中，供OpenGL使用
	glBindBuffer(GL_ARRAY_BUFFER, vbo_line);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 12 * numVertices, vertices_line, GL_STATIC_DRAW);

	// 3. 复制我们的索引数组到一个索引缓冲中，供OpenGL使用()
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_line);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * numVertices, v_indices, GL_STATIC_DRAW);

	//4. 设定顶点属性指针
	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// 1. 绑定顶点数组对象
	glBindVertexArray(vao_picker);

	// 2. 把我们的顶点数组复制到一个顶点缓冲中，供OpenGL使用
	glBindBuffer(GL_ARRAY_BUFFER, vbo_picker);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * numVertices, vertices_picker, GL_STATIC_DRAW);

	// 3. 复制我们的索引数组到一个索引缓冲中，供OpenGL使用
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * numVertices, v_indices, GL_STATIC_DRAW);

	// 4. 设定顶点属性指针
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	InitPicker();

	numTextures = model->numtextures;
	if (numTextures > 0)
	{
		InitTextureShader(model);
		InitTextureColorShader(model);
	}
}

string GetTexturePath(const string& modelPath, const string& textureName)
{
	string str1(modelPath);
	string str2(textureName);
	//string str3;
	vector<string> strs;
	filetool::SplitString(str1, strs, "\\");
	string sub_str = "";
	if (strs.size()>0)
	{
		sub_str = strs[strs.size() - 1];
	}
	string::size_type pos = 0;
	while ((pos = str1.find(sub_str)) != string::npos)   //替换所有指定子串
	{
		str1.replace(pos, sub_str.length(), str2);
	}
	return str1;
}

void CGLSL::InitTextureShader(const GLMmodel* model)
{
	textureProgram = ShaderManager::get_program("surface/surface_texture");
	if (!textureProgram) {
		std::vector<ShaderProgram::Attribute> attributes;
		attributes.emplace_back(ShaderProgram::Attribute(ShaderProgram::POSITION, "vtx_position"));
		attributes.emplace_back(ShaderProgram::Attribute(ShaderProgram::TEXCOORD, "vtx_texcoord"));
		attributes.emplace_back(ShaderProgram::Attribute(ShaderProgram::NORMAL, "vtx_normal"));
		textureProgram = ShaderManager::create_program_from_files("surface/surface_texture", attributes);
	}
	if (!textureProgram)
		return;

	numTextures = numTextures > 5 ? 5 : numTextures;
	for (unsigned int i = 0; i < numTextures; i++)
	{
		textures[i] = i;
	}

	for (int i = 0; i < numTextures; i++)
	{
		// 加载并生成纹理
		//char *filename = model->pathname + model->textures[0].name;
		string textureName = GetTexturePath(string(model->pathname), string(model->textures[i].name));

		int width, height, nrChannels;
		unsigned char *data = stbi_load(textureName.c_str(), &width, &height, &nrChannels, 0);
		if (data)
		{
			GLenum internal_format, format;
			switch (nrChannels)
			{
			case 4: internal_format = GL_RGBA8; format = GL_RGBA; break;
			case 3: internal_format = GL_RGB8; format = GL_RGB; break;
			case 2: internal_format = GL_RG8; format = GL_RG; break;
			case 1: internal_format = GL_R8; format = GL_RED; break;
			default: throw std::runtime_error("invalid format");
			}

			glGenTextures(1, &textures[i]);//glGenTextures先输入要生成纹理的数量，然后把它们储存在第二个参数的`unsigned int`数组中
			glBindTexture(GL_TEXTURE_2D, textures[i]);
			// 为当前绑定的纹理对象设置环绕、过滤方式
			glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	//GL_LINEAR_MIPMAP_LINEAR
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

			glTexImage2D(GL_TEXTURE_2D, 0, internal_format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
			glGenerateMipmap(GL_TEXTURE_2D);	//自动生成多级渐远纹理
			glBindTexture(GL_TEXTURE_2D, 0);
		}
		stbi_image_free(data);
	}

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, textures[0]);
	glActiveTexture(GL_TEXTURE1);
	glBindTexture(GL_TEXTURE_2D, textures[1]);
	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_2D, textures[2]);
	glActiveTexture(GL_TEXTURE3);
	glBindTexture(GL_TEXTURE_2D, textures[3]);
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, textures[4]);

	//glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_2D, textures[0]);
	//glActiveTexture(GL_TEXTURE1);
	//glBindTexture(GL_TEXTURE_2D, textures[1]);
	//glActiveTexture(GL_TEXTURE2);
	//glBindTexture(GL_TEXTURE_2D, textures[2]);
	//glActiveTexture(GL_TEXTURE3);
	//glBindTexture(GL_TEXTURE_2D, textures[3]);
	//glActiveTexture(GL_TEXTURE4);
	//glBindTexture(GL_TEXTURE_2D, textures[4]);

	const mat4 &MVP = camera_->modelViewProjectionMatrix();
	// camera position is defined in world coordinate system.
	const vec3 &wCamPos = camera_->position();
	// it can also be computed as follows:
	//const vec3& wCamPos = invMV * vec4(0, 0, 0, 1);
	const mat4 &MV = camera_->modelViewMatrix();
	const vec4 &wLightPos = inverse(MV) * setting_.light_position;

	textureProgram->bind();
	textureProgram->set_uniform("MVP", MVP)
		->set_uniform("lighting", false)
		->set_uniform("wLightPos", wLightPos)
		->set_uniform("wCamPos", wCamPos)
		->set_uniform("two_sides_lighting", setting_.light_two_sides)
		->set_uniform("distinct_back_color", true)
		->set_uniform("backside_color", setting_.background_color)
		->set_uniform("smooth_shading", false)
		->set_block_uniform("Material", "ambient", easy3d::vec3(setting_.material_ambient))
		->set_block_uniform("Material", "specular", easy3d::vec3(setting_.material_specular))
		->set_block_uniform("Material", "shininess", &setting_.material_shininess)
		//->bind_texture("textureID", texture1, 0)
		->set_uniform("texture_repeat", 0)
		->set_uniform("fractional_repeat", 0)
		->set_uniform("highlight", false)
		->set_uniform("hightlight_id_min", -1)
		->set_uniform("hightlight_id_max", -1);

	//textureProgram->bind_texture("textureID", texture1, 0);
	//textureProgram->set_uniform("textureID", textures[0]);
	//textureProgram->set_uniform("textureID[0]", textures);

	textureProgram->set_uniform("textureID0", 0);
	textureProgram->set_uniform("textureID1", 1);
	textureProgram->set_uniform("textureID2", 2);
	textureProgram->set_uniform("textureID3", 3);
	textureProgram->set_uniform("textureID4", 4);

	int numTriangles = model->numtriangles;
	numTextures = model->numtextures;

#if 1
	vertices_texture = new GLfloat[numVertices * 9];
	for (size_t i = 0; i < numTriangles; i++)
	{
		int map_diffuse = model->materials[model->triangles[i].material].map_diffuse;
		map_diffuse = map_diffuse < 0 ? 0 : map_diffuse;
		for (size_t k = 0; k < 3; k++)
		{
			assert(map_diffuse >= 0 && map_diffuse < model->numtextures);
			assert(model->triangles[i].tindices[k] >= 1 && model->triangles[i].tindices[k] <= model->numtexcoords);
			//坐标
			vertices_texture[9 * (3 * i + k) + 0] = model->vertices[3 * model->triangles[i].vindices[k] + 0];
			vertices_texture[9 * (3 * i + k) + 1] = model->vertices[3 * model->triangles[i].vindices[k] + 1];
			vertices_texture[9 * (3 * i + k) + 2] = model->vertices[3 * model->triangles[i].vindices[k] + 2];
			//纹理坐标
			float t_u = model->texcoords[2 * model->triangles[i].tindices[k]] * model->textures[map_diffuse].width;
			float t_v = 1 - model->texcoords[2 * model->triangles[i].tindices[k] + 1] * model->textures[map_diffuse].height;
			vertices_texture[9 * (3 * i + k) + 3] = t_u;
			vertices_texture[9 * (3 * i + k) + 4] = t_v;
			vertices_texture[9 * (3 * i + k) + 5] = map_diffuse;
			//面片法向量
			vertices_texture[9 * (3 * i + k) + 6] = model->facetnorms[3 * model->triangles[i].findex + 0];
			vertices_texture[9 * (3 * i + k) + 7] = model->facetnorms[3 * model->triangles[i].findex + 1];
			vertices_texture[9 * (3 * i + k) + 8] = model->facetnorms[3 * model->triangles[i].findex + 2];
		}
	}
#endif // 多纹理绑定顶点数据

#if 0
	vertices_texture = new GLfloat[numVertices * 8];
	for (size_t i = 0; i < numTriangles; i++)
	{
		int map_diffuse = model->materials[model->triangles[i].material].map_diffuse;
		map_diffuse = map_diffuse < 0 ? 0 : map_diffuse;
		for (size_t k = 0; k < 3; k++)
		{
			//坐标
			vertices_texture[8 * (3 * i + k) + 0] = model->vertices[3 * model->triangles[i].vindices[k] + 0];
			vertices_texture[8 * (3 * i + k) + 1] = model->vertices[3 * model->triangles[i].vindices[k] + 1];
			vertices_texture[8 * (3 * i + k) + 2] = model->vertices[3 * model->triangles[i].vindices[k] + 2];
			//纹理坐标
			float tex_x = model->texcoords[2 * model->triangles[i].tindices[k]] * model->textures[map_diffuse].width;
			float tex_y = model->texcoords[2 * model->triangles[i].tindices[k] + 1] * model->textures[map_diffuse].height;
			vertices_texture[8 * (3 * i + k) + 3] = tex_x;
			vertices_texture[8 * (3 * i + k) + 4] = 1 - tex_y;
			//面片法向量
			vertices_texture[8 * (3 * i + k) + 5] = model->facetnorms[3 * model->triangles[i].findex + 0];
			vertices_texture[8 * (3 * i + k) + 6] = model->facetnorms[3 * model->triangles[i].findex + 1];
			vertices_texture[8 * (3 * i + k) + 7] = model->facetnorms[3 * model->triangles[i].findex + 2];
		}
	}
#endif // 单纹理绑定顶点数据

	glGenVertexArrays(1, &vao_texture);
	glGenBuffers(1, &vbo_texture);

#if 1
	// 1. 绑定顶点数组对象
	glBindVertexArray(vao_texture);
	// 2. 把我们的顶点数组复制到一个顶点缓冲中，供OpenGL使用
	glBindBuffer(GL_ARRAY_BUFFER, vbo_texture);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 9 * numVertices, vertices_texture, GL_STATIC_DRAW);

	// 3. 复制我们的索引数组到一个索引缓冲中，供OpenGL使用()
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * numVertices, v_indices, GL_STATIC_DRAW);

	//4. 设定顶点属性指针
	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// texture attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// normal attribute
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 9 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);
#endif // 多纹理设置顶点数组属性

#if 0
	// 1. 绑定顶点数组对象
	glBindVertexArray(vao_texture);
	// 2. 把我们的顶点数组复制到一个顶点缓冲中，供OpenGL使用
	glBindBuffer(GL_ARRAY_BUFFER, vbo_texture);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 8 * numVertices, vertices_texture, GL_STATIC_DRAW);

	// 3. 复制我们的索引数组到一个索引缓冲中，供OpenGL使用()
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * numVertices, v_indices, GL_STATIC_DRAW);

	//4. 设定顶点属性指针
	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// texture attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// normal attribute
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(5 * sizeof(float)));
	glEnableVertexAttribArray(2);
#endif // 单纹理设置顶点数组属性
}

void CGLSL::InitTextureColorShader(const GLMmodel* model)
{
	textureColorProgram = ShaderManager::get_program("surface/surface_texture_color");
	if (!textureColorProgram) {
		std::vector<ShaderProgram::Attribute> attributes;
		attributes.emplace_back(ShaderProgram::Attribute(ShaderProgram::POSITION, "vtx_position"));
		attributes.emplace_back(ShaderProgram::Attribute(ShaderProgram::TEXCOORD, "vtx_texcoord"));
		attributes.emplace_back(ShaderProgram::Attribute(ShaderProgram::NORMAL, "vtx_normal"));
		textureColorProgram = ShaderManager::create_program_from_files("surface/surface_texture_color", attributes);
	}
	if (!textureColorProgram)
		return;

	const mat4 &MVP = camera_->modelViewProjectionMatrix();
	// camera position is defined in world coordinate system.
	const vec3 &wCamPos = camera_->position();
	// it can also be computed as follows:
	//const vec3& wCamPos = invMV * vec4(0, 0, 0, 1);
	const mat4 &MV = camera_->modelViewMatrix();
	const vec4 &wLightPos = inverse(MV) * setting_.light_position;

	textureColorProgram->bind();
	textureColorProgram->set_uniform("MVP", MVP)
		->set_uniform("lighting", true)
		->set_uniform("wLightPos", wLightPos)
		->set_uniform("wCamPos", wCamPos)
		->set_uniform("two_sides_lighting", setting_.light_two_sides)
		->set_uniform("distinct_back_color", true)
		->set_uniform("backside_color", setting_.background_color)
		->set_uniform("smooth_shading", false)
		->set_block_uniform("Material", "ambient", easy3d::vec3(setting_.material_ambient))
		->set_block_uniform("Material", "specular", easy3d::vec3(setting_.material_specular))
		->set_block_uniform("Material", "shininess", &setting_.material_shininess)
		//->bind_texture("textureID", texture1, 0)
		->set_uniform("texture_repeat", 0)
		->set_uniform("fractional_repeat", 0)
		->set_uniform("highlight", false)
		->set_uniform("hightlight_id_min", -1)
		->set_uniform("hightlight_id_max", -1)
		->set_uniform("transparency", 0.3f);

	textureColorProgram->set_uniform("textureID0", 0);
	textureColorProgram->set_uniform("textureID1", 1);
	textureColorProgram->set_uniform("textureID2", 2);
	textureColorProgram->set_uniform("textureID3", 3);
	textureColorProgram->set_uniform("textureID4", 4);

	int numTriangles = model->numtriangles;
	numTextures = model->numtextures;

#if 1
	vertices_textureColor = new GLfloat[numVertices * 12];
	for (size_t i = 0; i < numTriangles; i++)
	{
		int map_diffuse = model->materials[model->triangles[i].material].map_diffuse;
		map_diffuse = map_diffuse < 0 ? 0 : map_diffuse;
		for (size_t k = 0; k < 3; k++)
		{
			assert(map_diffuse >= 0 && map_diffuse < model->numtextures);
			assert(model->triangles[i].tindices[k] >= 1 && model->triangles[i].tindices[k] <= model->numtexcoords);
			//坐标
			vertices_textureColor[12 * (3 * i + k) + 0] = model->vertices[3 * model->triangles[i].vindices[k] + 0];
			vertices_textureColor[12 * (3 * i + k) + 1] = model->vertices[3 * model->triangles[i].vindices[k] + 1];
			vertices_textureColor[12 * (3 * i + k) + 2] = model->vertices[3 * model->triangles[i].vindices[k] + 2];
			//纹理坐标
			vertices_textureColor[12 * (3 * i + k) + 3] = model->texcoords[2 * model->triangles[i].tindices[k]] * model->textures[map_diffuse].width;
			vertices_textureColor[12 * (3 * i + k) + 4] = 1 - model->texcoords[2 * model->triangles[i].tindices[k] + 1] * model->textures[map_diffuse].height;
			vertices_textureColor[12 * (3 * i + k) + 5] = map_diffuse;
			//颜色
			vertices_textureColor[12 * (3 * i + k) + 6] = 2.f;
			vertices_textureColor[12 * (3 * i + k) + 7] = 2.f;
			vertices_textureColor[12 * (3 * i + k) + 8] = 2.f;
			//面片法向量
			vertices_textureColor[12 * (3 * i + k) + 9] = model->facetnorms[3 * model->triangles[i].findex + 0];
			vertices_textureColor[12 * (3 * i + k) + 10] = model->facetnorms[3 * model->triangles[i].findex + 1];
			vertices_textureColor[12 * (3 * i + k) + 11] = model->facetnorms[3 * model->triangles[i].findex + 2];
		}
	}
#endif // 多纹理绑定顶点数据
	glGenVertexArrays(1, &vao_textureColor);
	glGenBuffers(1, &vbo_textureColor);

#if 1
	// 1. 绑定顶点数组对象
	glBindVertexArray(vao_textureColor);
	// 2. 把我们的顶点数组复制到一个顶点缓冲中，供OpenGL使用
	glBindBuffer(GL_ARRAY_BUFFER, vbo_textureColor);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 12 * numVertices, vertices_textureColor, GL_STATIC_DRAW);

	// 3. 复制我们的索引数组到一个索引缓冲中，供OpenGL使用()
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * numVertices, v_indices, GL_STATIC_DRAW);

	//4. 设定顶点属性指针
	// position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 12 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	// texture attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 12 * sizeof(float), (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);
	// color attribute
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 12 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);
	// normal attribute
	glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, 12 * sizeof(float), (void*)(9 * sizeof(float)));
	glEnableVertexAttribArray(3);
#endif // 多纹理设置顶点数组属性
}

void CGLSL::Display(Camera* camera)
{
	//if (numTextures>0)
	//{
	//	Display_texture(camera);
	//}
	//else
	//{
	//	Display_color(camera);
	//	Display_line(camera);
	//}

	Display_color(camera);
	//Display_line(camera);
}

void CGLSL::Display_color(Camera* camera)
{
	if (!colorProgram)
	{
		colorProgram = ShaderManager::get_program("surface/surface_color");
		if (!colorProgram) {
			std::vector<ShaderProgram::Attribute> attributes;
			attributes.emplace_back(ShaderProgram::Attribute(ShaderProgram::POSITION, "vtx_position"));
			attributes.emplace_back(ShaderProgram::Attribute(ShaderProgram::COLOR, "vtx_color"));
			attributes.emplace_back(ShaderProgram::Attribute(ShaderProgram::NORMAL, "vtx_normal"));
			colorProgram = ShaderManager::create_program_from_files("surface/surface_color", attributes);
		}
	}

	const mat4 &MVP = camera_->modelViewProjectionMatrix();
	// camera position is defined in world coordinate system.
	const vec3 &wCamPos = camera_->position();
	// it can also be computed as follows:
	//const vec3& wCamPos = invMV * vec4(0, 0, 0, 1);
	const mat4 &MV = camera_->modelViewMatrix();
	const vec4 &wLightPos = inverse(MV) * setting_.light_position;

	colorProgram->bind();
	colorProgram->set_uniform("MVP", MVP);
	colorProgram->set_uniform("lighting", true);
	colorProgram->set_uniform("wLightPos", wLightPos);
	colorProgram->set_uniform("wCamPos", wCamPos);
	colorProgram->set_uniform("two_sides_lighting", setting_.light_two_sides);
	colorProgram->set_uniform("distinct_back_color", true);
	colorProgram->set_uniform("backside_color", setting_.background_color);
	colorProgram->set_uniform("smooth_shading", false);
	colorProgram->set_uniform("ssaoEnabled", false);
	colorProgram->set_uniform("per_vertex_color", true);
	colorProgram->set_uniform("default_color", setting_.default_color);
	colorProgram->set_block_uniform("Material", "ambient", easy3d::vec3(setting_.material_ambient));
	colorProgram->set_block_uniform("Material", "specular", easy3d::vec3(setting_.material_specular));
	colorProgram->set_block_uniform("Material", "shininess", &setting_.material_shininess);
	colorProgram->set_uniform("highlight", false);
	colorProgram->set_uniform("hightlight_id_min", -1);
	colorProgram->set_uniform("hightlight_id_max", -1);

	glClearColor(setting_.background_color[0], setting_.background_color[1], setting_.background_color[2], setting_.background_color[3]);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glBindVertexArray(vao_color);
	glDrawElements(GL_TRIANGLES, numVertices, GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);

	//colorProgram->release();
}

void CGLSL::Display_line(Camera* camera)
{
	if (!lineProgram)
	{
		lineProgram = easy3d::ShaderManager::get_program("lines/lines_plain_color");
		if (!lineProgram) {
			std::vector<ShaderProgram::Attribute> attributes;
			attributes.emplace_back(ShaderProgram::Attribute(ShaderProgram::POSITION, "vtx_position"));
			attributes.emplace_back(ShaderProgram::Attribute(ShaderProgram::COLOR, "vtx_color"));
			lineProgram = ShaderManager::create_program_from_files("lines/lines_plain_color", attributes);
		}
	}

	const mat4 &MVP = camera_->modelViewProjectionMatrix();
	// camera position is defined in world coordinate system.
	const vec3 &wCamPos = camera_->position();
	// it can also be computed as follows:
	//const vec3& wCamPos = invMV * vec4(0, 0, 0, 1);
	const mat4 &MV = camera_->modelViewMatrix();
	const vec4 &wLightPos = inverse(MV) * setting_.light_position;

	lineProgram->bind();
	lineProgram->set_uniform("MVP", MVP);
	glBindVertexArray(vao_line);
	glLineWidth(2.0f);
	glDrawArrays(GL_LINES, 0, numVertices * 2);
	glBindVertexArray(0);

	//lineProgram->release();
}

void CGLSL::Display_texture(Camera* camera)
{
	if (!textureProgram)
	{
		textureProgram = ShaderManager::get_program("surface/surface_texture");
		if (!textureProgram) {
			std::vector<ShaderProgram::Attribute> attributes;
			attributes.emplace_back(ShaderProgram::Attribute(ShaderProgram::POSITION, "vtx_position"));
			attributes.emplace_back(ShaderProgram::Attribute(ShaderProgram::TEXCOORD, "vtx_texcoord"));
			attributes.emplace_back(ShaderProgram::Attribute(ShaderProgram::NORMAL, "vtx_normal"));
			textureProgram = ShaderManager::create_program_from_files("surface/surface_texture", attributes);
		}
	}

	const mat4 &MVP = camera_->modelViewProjectionMatrix();
	// camera position is defined in world coordinate system.
	const vec3 &wCamPos = camera_->position();
	// it can also be computed as follows:
	//const vec3& wCamPos = invMV * vec4(0, 0, 0, 1);
	const mat4 &MV = camera_->modelViewMatrix();
	const vec4 &wLightPos = inverse(MV) * setting_.light_position;

	textureProgram->bind();
	textureProgram->set_uniform("MVP", MVP)
		->set_uniform("lighting", false)
		->set_uniform("wLightPos", wLightPos)
		->set_uniform("wCamPos", wCamPos)
		->set_uniform("two_sides_lighting", setting_.light_two_sides)
		->set_uniform("distinct_back_color", true)
		->set_uniform("backside_color", setting_.background_color)
		->set_uniform("smooth_shading", false)
		->set_block_uniform("Material", "ambient", easy3d::vec3(setting_.material_ambient))
		->set_block_uniform("Material", "specular", easy3d::vec3(setting_.material_specular))
		->set_block_uniform("Material", "shininess", &setting_.material_shininess)
		//->bind_texture("textureID", texture1, 0)
		->set_uniform("texture_repeat", 0)
		->set_uniform("fractional_repeat", 0)
		->set_uniform("highlight", false)
		->set_uniform("hightlight_id_min", -1)
		->set_uniform("hightlight_id_max", -1);

	textureProgram->set_uniform("textureID0", 0);
	textureProgram->set_uniform("textureID1", 1);
	textureProgram->set_uniform("textureID2", 2);
	textureProgram->set_uniform("textureID3", 3);
	textureProgram->set_uniform("textureID4", 4);

	glClearColor(setting_.background_color[0], setting_.background_color[1], setting_.background_color[2], setting_.background_color[3]);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// 绑定材质
	//glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_2D, textures[0]);
	//glActiveTexture(GL_TEXTURE1);
	//glBindTexture(GL_TEXTURE_2D, textures[1]);
	//glActiveTexture(GL_TEXTURE2);
	//glBindTexture(GL_TEXTURE_2D, textures[2]);
	//glActiveTexture(GL_TEXTURE3);
	//glBindTexture(GL_TEXTURE_2D, textures[3]);
	//glActiveTexture(GL_TEXTURE4);
	//glBindTexture(GL_TEXTURE_2D, textures[4]);

	//glEnable(GL_TEXTURE_2D);
	//glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	//glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);	//双面光照
	glBindVertexArray(vao_texture);
	glDrawElements(GL_TRIANGLES, numVertices, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	//textureProgram->release_texture();
	//textureProgram->release();
}

void CGLSL::Display_textureColor(Camera* camera)
{
	if (!textureColorProgram)
	{
		textureColorProgram = ShaderManager::get_program("surface/surface_texture_color");
		if (!textureColorProgram) {
			std::vector<ShaderProgram::Attribute> attributes;
			attributes.emplace_back(ShaderProgram::Attribute(ShaderProgram::POSITION, "vtx_position"));
			attributes.emplace_back(ShaderProgram::Attribute(ShaderProgram::TEXCOORD, "vtx_texcoord"));
			attributes.emplace_back(ShaderProgram::Attribute(ShaderProgram::NORMAL, "vtx_normal"));
			textureColorProgram = ShaderManager::create_program_from_files("surface/surface_texture_color", attributes);
		}
	}

	const mat4 &MVP = camera_->modelViewProjectionMatrix();
	// camera position is defined in world coordinate system.
	const vec3 &wCamPos = camera_->position();
	// it can also be computed as follows:
	//const vec3& wCamPos = invMV * vec4(0, 0, 0, 1);
	const mat4 &MV = camera_->modelViewMatrix();
	const vec4 &wLightPos = inverse(MV) * setting_.light_position;

	textureColorProgram->bind();
	textureColorProgram->set_uniform("MVP", MVP)
		->set_uniform("lighting", true)
		->set_uniform("wLightPos", wLightPos)
		->set_uniform("wCamPos", wCamPos)
		->set_uniform("two_sides_lighting", setting_.light_two_sides)
		->set_uniform("distinct_back_color", true)
		->set_uniform("backside_color", setting_.background_color)
		->set_uniform("smooth_shading", false)
		->set_block_uniform("Material", "ambient", easy3d::vec3(setting_.material_ambient))
		->set_block_uniform("Material", "specular", easy3d::vec3(setting_.material_specular))
		->set_block_uniform("Material", "shininess", &setting_.material_shininess)
		//->bind_texture("textureID", texture1, 0)
		->set_uniform("texture_repeat", 0)
		->set_uniform("fractional_repeat", 0)
		->set_uniform("highlight", false)
		->set_uniform("hightlight_id_min", -1)
		->set_uniform("hightlight_id_max", -1)
		->set_uniform("transparency", 0.5f);;

	textureColorProgram->set_uniform("textureID0", 0);
	textureColorProgram->set_uniform("textureID1", 1);
	textureColorProgram->set_uniform("textureID2", 2);
	textureColorProgram->set_uniform("textureID3", 3);
	textureColorProgram->set_uniform("textureID4", 4);

	glClearColor(setting_.background_color[0], setting_.background_color[1], setting_.background_color[2], setting_.background_color[3]);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// 绑定材质
	//glActiveTexture(GL_TEXTURE0);
	//glBindTexture(GL_TEXTURE_2D, textures[0]);
	//glActiveTexture(GL_TEXTURE1);
	//glBindTexture(GL_TEXTURE_2D, textures[1]);
	//glActiveTexture(GL_TEXTURE2);
	//glBindTexture(GL_TEXTURE_2D, textures[2]);
	//glActiveTexture(GL_TEXTURE3);
	//glBindTexture(GL_TEXTURE_2D, textures[3]);
	//glActiveTexture(GL_TEXTURE4);
	//glBindTexture(GL_TEXTURE_2D, textures[4]);

	//glEnable(GL_TEXTURE_2D);
	//glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	//glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);	//双面光照
	glBindVertexArray(vao_textureColor);
	glDrawElements(GL_TRIANGLES, numVertices, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	//textureColorProgram->release_texture();
	//textureColorProgram->release();
}

void CGLSL::Display_picker(Camera* camera)
{
	if (!pickProgram)
	{
		pickProgram = ShaderManager::get_program("selection/selection_single_primitive");
		if (!pickProgram) {
			std::vector<ShaderProgram::Attribute> attributes;
			attributes.push_back(ShaderProgram::Attribute(ShaderProgram::POSITION, "vtx_position"));
			pickProgram = ShaderManager::create_program_from_files("selection/selection_single_primitive", attributes);
		}
		if (!pickProgram) {
			return;
		}
	}

	pickProgram->bind();
	pickProgram->set_uniform("MVP", camera_->modelViewProjectionMatrix());
	glClearColor(setting_.background_color[0], setting_.background_color[1], setting_.background_color[2], setting_.background_color[3]);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glPolygonMode(GL_FRONT, GL_FILL);
	glBindVertexArray(vao_picker);
	glDrawElements(GL_TRIANGLES, numVertices, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}

void CGLSL::InitPicker()
{
	//glewInit();
	pickProgram = ShaderManager::get_program("selection/selection_single_primitive");
	if (!pickProgram) {
		std::vector<ShaderProgram::Attribute> attributes;
		attributes.push_back(ShaderProgram::Attribute(ShaderProgram::POSITION, "vtx_position"));
		pickProgram = ShaderManager::create_program_from_files("selection/selection_single_primitive", attributes);
	}
	if (!pickProgram) {
		return;
	}

	//// first, configure the cube's VAO (and VBO)
	//glGenVertexArrays(1, &vao_picker);
	//glGenBuffers(1, &vbo_picker);
	//glGenBuffers(1, &ibo_picker);

	//// 1. 绑定顶点数组对象
	//glBindVertexArray(vao_picker);

	//// 2. 把我们的顶点数组复制到一个顶点缓冲中，供OpenGL使用
	//glBindBuffer(GL_ARRAY_BUFFER, vbo_picker);
	//glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 3 * numVertices, vertices_picker, GL_STATIC_DRAW);

	//// 3. 复制我们的索引数组到一个索引缓冲中，供OpenGL使用
	//glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo_picker);
	//glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * numVertices, v_indices, GL_STATIC_DRAW);

	//// 4. 设定顶点属性指针
	//glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	//glEnableVertexAttribArray(0);

}

void CGLSL::setup_framebuffer(int width, int height)
{
	// prepare a frame buffer object (fbo), I will do offscreen rendering to the new fbo
	if (!fbo_) {
		fbo_ = new FramebufferObject(width, height, 0);
		fbo_->add_color_buffer();
		fbo_->add_depth_buffer();

#if 0
		fbo_->print_attachments();
		fbo_->print_draw_buffers();
		fbo_->print_read_buffer();
#endif
	}
	fbo_->ensure_size(width, height);
}

void CGLSL::screen_to_opengl(int x, int y, int &gl_x, int &gl_y, int width, int height, int screenWidth, int screenHeight)
{
	float dpi_scaling_x = width / static_cast<float>(screenWidth);
	float dpi_scaling_y = height / static_cast<float>(screenHeight);

	gl_x = static_cast<int>(dpi_scaling_x * x);
	gl_y = static_cast<int>(dpi_scaling_y * (screenHeight - 1 - y));
}

int CGLSL::PickFace(int x, int y)
{
	if (!pickProgram || numVertices <= 0)
	{
		return -1;
	}

	int viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	int width = viewport[2];
	int height = viewport[3];

	setup_framebuffer(width, height);
	fbo_->bind();

	float color[4];
	glGetFloatv(GL_COLOR_CLEAR_VALUE, color);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	pickProgram->bind();
	//glEnableVertexAttribArray(0);
	pickProgram->set_uniform("MVP", camera_->modelViewProjectionMatrix());
	glBindVertexArray(vao_picker);
	//glDrawArrays(GL_TRIANGLES, 0, numVertices);
	glDrawElements(GL_TRIANGLES, numVertices, GL_UNSIGNED_INT, 0);
	assert(vao_picker != 0);
	glBindVertexArray(0);
	//glDisableVertexAttribArray(0);
	pickProgram->release();

	// --- Maybe this is not necessary ---------
	glFlush();
	glFinish();
	// -----------------------------------------

	int gl_x, gl_y;
	screen_to_opengl(x, y, gl_x, gl_y, width, height, camera_->screenWidth(), camera_->screenHeight());

	unsigned char c[4];
	//glPixelStorei(GL_PACK_ALIGNMENT, 1);
	//glReadPixels(gl_x, gl_y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, c);
	fbo_->read_color(c, gl_x, gl_y);
	fbo_->release();

	glClearColor(color[0], color[1], color[2], color[3]);

	int id = rgb::rgba(c[0], c[1], c[2], c[3]);

	int c0 = c[0];
	int c1 = c[1];
	int c2 = c[2];
	int c3 = c[3];
	//std::cout << "rgba:  [" << c0 << "， " << c1 << "， " << c2 << "， " << c3 << "]" << std::endl;

	int pickedID = c[0] + c[1] * 256 + c[2] * 256 * 256;

	if (pickedID>numVertices / 3)
	{
		pickedID = -1;
	}

	return pickedID;
}

//按半径区域选择面片
vector<int> CGLSL::PickFaces_Radius(int x, int y, int radius)
{
	vector<int> pickedIDs;

	if (!pickProgram || numVertices <= 0)
	{
		return pickedIDs;
	}

	int viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	int width = viewport[2];
	int height = viewport[3];

	setup_framebuffer(width, height);
	fbo_->bind();

	float color[4];
	glGetFloatv(GL_COLOR_CLEAR_VALUE, color);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	pickProgram->bind();
	//glEnableVertexAttribArray(0);
	pickProgram->set_uniform("MVP", camera_->modelViewProjectionMatrix());
	glBindVertexArray(vao_picker);
	//glDrawArrays(GL_TRIANGLES, 0, numVertices);
	glDrawElements(GL_TRIANGLES, numVertices, GL_UNSIGNED_INT, 0);
	assert(vao_picker != 0);
	glBindVertexArray(0);
	//glDisableVertexAttribArray(0);
	pickProgram->release();

	// --- Maybe this is not necessary ---------
	glFlush();
	glFinish();
	// -----------------------------------------

	vector<Vector2i> pixels;
	//取x，y半径radius区域内所有像素点
	for (int px = (x - radius); px <= (x + radius); px++)
	{
		for (int py = (y - radius); py <= (y + radius); py++)
		{
			if (px >= 0 && px < width&&py >= 0 && py < height)
			{
				float distance = sqrtf(pow(px - x, 2) + pow(py - y, 2));
				if (distance <= radius)
				{
					pixels.push_back(Vector2i(px, py));
				}
			}
		}
	}

	for (size_t pi = 0; pi < pixels.size(); pi++)
	{
		int gl_x, gl_y;
		screen_to_opengl(pixels[pi][0], pixels[pi][1], gl_x, gl_y, width, height, camera_->screenWidth(), camera_->screenHeight());

		unsigned char c[4];
		//glPixelStorei(GL_PACK_ALIGNMENT, 1);
		//glReadPixels(gl_x, gl_y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, c);
		fbo_->read_color(c, gl_x, gl_y);
		int id = rgb::rgba(c[0], c[1], c[2], c[3]);

		int c0 = c[0];
		int c1 = c[1];
		int c2 = c[2];
		int c3 = c[3];
		//std::cout << "rgba:  [" << c0 << "， " << c1 << "， " << c2 << "， " << c3 << "]" << std::endl;

		int pickedID = c[0] + c[1] * 256 + c[2] * 256 * 256;

		if (pickedID > numVertices / 3)
		{
			pickedID = -1;
		}

		if (pickedID >= 0)
		{
			pickedIDs.push_back(pickedID);
		}
	}
	fbo_->release();
	glClearColor(color[0], color[1], color[2], color[3]);

	std::sort(pickedIDs.begin(), pickedIDs.end());
	pickedIDs.erase(unique(pickedIDs.begin(), pickedIDs.end()), pickedIDs.end());
	return pickedIDs;
}

//按矩形区域选择面片
vector<int> CGLSL::PickFaces_Rect(const easy3d::ivec2& p1, const easy3d::ivec2& p2)
{
	vector<int> pickedIDs;

	if (!pickProgram || numVertices <= 0)
	{
		return pickedIDs;
	}

	easy3d::ivec2 lbPoint;
	easy3d::ivec2 rtPoint;
	if (p1.x > p2.x)
	{
		lbPoint.x = p2.x;
		rtPoint.x = p1.x;
	}
	else
	{
		rtPoint.x = p2.x;
		lbPoint.x = p1.x;
	}

	if (p1.y > p2.y)
	{
		lbPoint.y = p2.y;
		rtPoint.y = p1.y;
	}
	else
	{
		rtPoint.y = p2.y;
		lbPoint.y = p1.y;
	}

	int viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	int width = viewport[2];
	int height = viewport[3];

	setup_framebuffer(width, height);
	fbo_->bind();

	float color[4];
	glGetFloatv(GL_COLOR_CLEAR_VALUE, color);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	pickProgram->bind();
	//glEnableVertexAttribArray(0);
	pickProgram->set_uniform("MVP", camera_->modelViewProjectionMatrix());
	glBindVertexArray(vao_picker);
	//glDrawArrays(GL_TRIANGLES, 0, numVertices);
	glDrawElements(GL_TRIANGLES, numVertices, GL_UNSIGNED_INT, 0);
	assert(vao_picker != 0);
	glBindVertexArray(0);
	//glDisableVertexAttribArray(0);
	pickProgram->release();

	// --- Maybe this is not necessary ---------
	glFlush();
	glFinish();
	// -----------------------------------------

	vector<Vector2i> pixels;
	//取x，y半径radius区域内所有像素点
	for (int px = lbPoint.x; px <= rtPoint.x; px++)
	{
		for (int py = lbPoint.y; py <= rtPoint.y; py++)
		{
			if (px >= 0 && px < width&&py >= 0 && py < height)
			{
				pixels.push_back(Vector2i(px, py));
			}
		}
	}

	for (size_t pi = 0; pi < pixels.size(); pi++)
	{
		int gl_x, gl_y;
		screen_to_opengl(pixels[pi][0], pixels[pi][1], gl_x, gl_y, width, height, camera_->screenWidth(), camera_->screenHeight());

		unsigned char c[4];
		//glPixelStorei(GL_PACK_ALIGNMENT, 1);
		//glReadPixels(gl_x, gl_y, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, c);
		fbo_->read_color(c, gl_x, gl_y);
		int id = rgb::rgba(c[0], c[1], c[2], c[3]);

		int c0 = c[0];
		int c1 = c[1];
		int c2 = c[2];
		int c3 = c[3];
		//std::cout << "rgba:  [" << c0 << "， " << c1 << "， " << c2 << "， " << c3 << "]" << std::endl;

		int pickedID = c[0] + c[1] * 256 + c[2] * 256 * 256;

		if (pickedID > numVertices / 3)
		{
			pickedID = -1;
		}

		if (pickedID >= 0)
		{
			pickedIDs.push_back(pickedID);
		}
	}
	fbo_->release();
	glClearColor(color[0], color[1], color[2], color[3]);

	std::sort(pickedIDs.begin(), pickedIDs.end());
	pickedIDs.erase(unique(pickedIDs.begin(), pickedIDs.end()), pickedIDs.end());
	return pickedIDs;
}

vector<int> CGLSL::PickFaces_Rect2(const easy3d::ivec2& p1, const easy3d::ivec2& p2)
{
	vector<int> pickedIDs;

	if (!pickProgram || numVertices <= 0)
	{
		return pickedIDs;
	}

	easy3d::ivec2 lbPoint;
	easy3d::ivec2 rtPoint;
	if (p1.x > p2.x)
	{
		lbPoint.x = p2.x;
		rtPoint.x = p1.x;
	}
	else
	{
		rtPoint.x = p2.x;
		lbPoint.x = p1.x;
	}

	if (p1.y > p2.y)
	{
		lbPoint.y = p2.y;
		rtPoint.y = p1.y;
	}
	else
	{
		rtPoint.y = p2.y;
		lbPoint.y = p1.y;
	}

	int pixelWidth = rtPoint.x - lbPoint.x;
	int pixelHeight = rtPoint.y - lbPoint.y;
	if (pixelWidth == 0 || pixelHeight == 0)
	{
		return pickedIDs;
	}

	int viewport[4];
	glGetIntegerv(GL_VIEWPORT, viewport);
	int width = viewport[2];
	int height = viewport[3];

	setup_framebuffer(width, height);
	fbo_->bind();

	float color[4];
	glGetFloatv(GL_COLOR_CLEAR_VALUE, color);
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	pickProgram->bind();
	//glEnableVertexAttribArray(0);
	pickProgram->set_uniform("MVP", camera_->modelViewProjectionMatrix());
	glBindVertexArray(vao_picker);
	//glDrawArrays(GL_TRIANGLES, 0, numVertices);
	glDrawElements(GL_TRIANGLES, numVertices, GL_UNSIGNED_INT, 0);
	assert(vao_picker != 0);
	glBindVertexArray(0);
	//glDisableVertexAttribArray(0);
	pickProgram->release();

	// --- Maybe this is not necessary ---------
	glFlush();
	glFinish();
	// -----------------------------------------

	int gl_x1, gl_y1, gl_x2, gl_y2;
	screen_to_opengl(lbPoint.x, lbPoint.y, gl_x1, gl_y1, width, height, camera_->screenWidth(), camera_->screenHeight());
	screen_to_opengl(rtPoint.x, rtPoint.y, gl_x2, gl_y2, width, height, camera_->screenWidth(), camera_->screenHeight());

	int gl_rect_width = abs(gl_x1 - gl_x2);
	int gl_rect_height = abs(gl_y1 - gl_y2);
	gl_x1 = gl_x1 < gl_x2 ? gl_x1 : gl_x2;
	gl_y1 = gl_y1 < gl_y2 ? gl_y1 : gl_y2;

	GLubyte* pPixelData;
	GLint    PixelDataLength;
	PixelDataLength = gl_rect_width * gl_rect_height * 4;
	pPixelData = (GLubyte*)malloc(PixelDataLength);
	if (pPixelData == 0)
		exit(0);

	// 读取像素 
	glPixelStorei(GL_UNPACK_ALIGNMENT, 4);
	glReadPixels(gl_x1, gl_y1, gl_rect_width, gl_rect_height, GL_RGBA, GL_UNSIGNED_BYTE, pPixelData);

	for (int i = 0; i < gl_rect_width * gl_rect_height; i++)
	{
		int pickedID = pPixelData[i * 4 + 0] + pPixelData[i * 4 + 1] * 256 + pPixelData[i * 4 + 2] * 256 * 256;
		if (pickedID > numVertices / 3)
		{
			pickedID = -1;
		}

		if (pickedID >= 0)
		{
			pickedIDs.push_back(pickedID);
		}
	}

	free(pPixelData);

	fbo_->release();
	glClearColor(color[0], color[1], color[2], color[3]);

	std::sort(pickedIDs.begin(), pickedIDs.end());
	pickedIDs.erase(unique(pickedIDs.begin(), pickedIDs.end()), pickedIDs.end());

	return pickedIDs;
}

void CGLSL::SetFaceHighlight(int faceIndex)
{
	if (selectFaceIndex >= 0)
	{
		for (size_t k = 0; k < 3; k++)
		{
			vertices_color[9 * (3 * selectFaceIndex + k) + 3] = setting_.default_color.x;
			vertices_color[9 * (3 * selectFaceIndex + k) + 4] = setting_.default_color.y;
			vertices_color[9 * (3 * selectFaceIndex + k) + 5] = setting_.default_color.z;
		}
	}

	if (faceIndex < 0 || faceIndex >= (numVertices / 3) || numVertices == 0)
	{
		selectFaceIndex = -1;
		return;
	}

	//颜色
	for (size_t k = 0; k < 3; k++)
	{
		vertices_color[9 * (3 * faceIndex + k) + 3] = 1.f;
		vertices_color[9 * (3 * faceIndex + k) + 4] = 0.f;
		vertices_color[9 * (3 * faceIndex + k) + 5] = 0.f;
	}

	// 1. 绑定顶点数组对象
	glBindVertexArray(vao_color);
	// 2. 把我们的顶点数组复制到一个顶点缓冲中，供OpenGL使用
	glBindBuffer(GL_ARRAY_BUFFER, vbo_color);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 9 * numVertices, vertices_color, GL_STATIC_DRAW);

	selectFaceIndex = faceIndex;
}

//相机复位
void CGLSL::ResetCamera()
{
	camera_->setType(easy3d::Camera::PERSPECTIVE);
	camera_->setUpVector(easy3d::vec3(0, 0, 1)); // Z pointing up
	camera_->setViewDirection(easy3d::vec3(-1, 0, 0)); // X pointing out
	camera_->showEntireScene();
}

//根据标注信息更新顶点数据缓存
void CGLSL::UpdateVBuffer(const LabelInfo* labelInfo)
{
	for (size_t i = 0; i < model->numtriangles; i++)
	{
		for (size_t k = 0; k < 3; k++)
		{
			int categoryId = labelInfo->segInfo[i][0];
			if (categoryId < 0)
			{
				vertices_color[9 * (3 * i + k) + 3] = labelInfo->backGround.insColor[0];
				vertices_color[9 * (3 * i + k) + 4] = labelInfo->backGround.insColor[1];
				vertices_color[9 * (3 * i + k) + 5] = labelInfo->backGround.insColor[2];

				//vertices_textureColor[12 * (3 * i + k) + 6] = labelInfo->backGround.insColor[0];
				//vertices_textureColor[12 * (3 * i + k) + 7] = labelInfo->backGround.insColor[1];
				//vertices_textureColor[12 * (3 * i + k) + 8] = labelInfo->backGround.insColor[2];
			}
			else
			{
				int insId = labelInfo->segInfo[i][1];
				vertices_color[9 * (3 * i + k) + 3] = labelInfo->categories[categoryId].instances[insId].insColor[0];
				vertices_color[9 * (3 * i + k) + 4] = labelInfo->categories[categoryId].instances[insId].insColor[1];
				vertices_color[9 * (3 * i + k) + 5] = labelInfo->categories[categoryId].instances[insId].insColor[2];

				if (numTextures > 0)
				{
					vertices_textureColor[12 * (3 * i + k) + 6] = labelInfo->categories[categoryId].instances[insId].insColor[0];
					vertices_textureColor[12 * (3 * i + k) + 7] = labelInfo->categories[categoryId].instances[insId].insColor[1];
					vertices_textureColor[12 * (3 * i + k) + 8] = labelInfo->categories[categoryId].instances[insId].insColor[2];
				}
			}
		}
	}

	// 1. 绑定顶点数组对象
	glBindVertexArray(vao_color);
	// 2. 把我们的顶点数组复制到一个顶点缓冲中，供OpenGL使用
	glBindBuffer(GL_ARRAY_BUFFER, vbo_color);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 9 * numVertices, vertices_color, GL_STATIC_DRAW);

	if (numTextures > 0)
	{
		// 1. 绑定顶点数组对象
		glBindVertexArray(vao_textureColor);
		// 2. 把我们的顶点数组复制到一个顶点缓冲中，供OpenGL使用
		glBindBuffer(GL_ARRAY_BUFFER, vbo_textureColor);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 12 * numVertices, vertices_textureColor, GL_STATIC_DRAW);
	}
}

////设置单个顶点颜色
//void CGLSL::SetVColor(int faceIndex, const Vector3f &color, bool ifBackGround)
//{
//	for (size_t k = 0; k < 3; k++)
//	{
//		vertices_color[9 * (3 * faceIndex + k) + 3] = color[0];
//		vertices_color[9 * (3 * faceIndex + k) + 4] = color[1];
//		vertices_color[9 * (3 * faceIndex + k) + 5] = color[2];
//		if (numTextures > 0)
//		{
//			if (ifBackGround)
//			{
//				vertices_textureColor[12 * (3 * faceIndex + k) + 6] = 2.0;
//				vertices_textureColor[12 * (3 * faceIndex + k) + 7] = 2.0;
//				vertices_textureColor[12 * (3 * faceIndex + k) + 8] = 2.0;
//			}
//			else
//			{
//				vertices_textureColor[12 * (3 * faceIndex + k) + 6] = color[0];
//				vertices_textureColor[12 * (3 * faceIndex + k) + 7] = color[1];
//				vertices_textureColor[12 * (3 * faceIndex + k) + 8] = color[2];
//			}
//		}
//	}
//
//	// 1. 绑定顶点数组对象
//	glBindVertexArray(vao_color);
//	// 2. 把我们的顶点数组复制到一个顶点缓冲中，供OpenGL使用
//	glBindBuffer(GL_ARRAY_BUFFER, vbo_color);
//	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 9 * numVertices, vertices_color, GL_STATIC_DRAW);
//
//	if (numTextures > 0)
//	{
//		// 1. 绑定顶点数组对象
//		glBindVertexArray(vao_textureColor);
//		// 2. 把我们的顶点数组复制到一个顶点缓冲中，供OpenGL使用
//		glBindBuffer(GL_ARRAY_BUFFER, vbo_textureColor);
//		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 12 * numVertices, vertices_textureColor, GL_STATIC_DRAW);
//	}
//}

//设置顶点数组颜色
void CGLSL::SetVColor(const vector<int>& faces, const Vector3f &color, bool ifBackGround)
{
	int fSize = faces.size();
	for (size_t i = 0; i < fSize; i++)
	{
		int faceIndex = faces[i];
		for (size_t k = 0; k < 3; k++)
		{
			vertices_color[9 * (3 * faceIndex + k) + 3] = color[0];
			vertices_color[9 * (3 * faceIndex + k) + 4] = color[1];
			vertices_color[9 * (3 * faceIndex + k) + 5] = color[2];

			if (numTextures > 0)
			{
				if (ifBackGround)
				{
					vertices_textureColor[12 * (3 * faceIndex + k) + 6] = 2.0;
					vertices_textureColor[12 * (3 * faceIndex + k) + 7] = 2.0;
					vertices_textureColor[12 * (3 * faceIndex + k) + 8] = 2.0;
				}
				else
				{
					vertices_textureColor[12 * (3 * faceIndex + k) + 6] = color[0];
					vertices_textureColor[12 * (3 * faceIndex + k) + 7] = color[1];
					vertices_textureColor[12 * (3 * faceIndex + k) + 8] = color[2];
				}
			}
		}
	}

	// 1. 绑定顶点数组对象
	glBindVertexArray(vao_color);
	// 2. 把我们的顶点数组复制到一个顶点缓冲中，供OpenGL使用
	glBindBuffer(GL_ARRAY_BUFFER, vbo_color);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 9 * numVertices, vertices_color, GL_STATIC_DRAW);

	if (numTextures > 0)
	{
		// 1. 绑定顶点数组对象
		glBindVertexArray(vao_textureColor);
		// 2. 把我们的顶点数组复制到一个顶点缓冲中，供OpenGL使用
		glBindBuffer(GL_ARRAY_BUFFER, vbo_textureColor);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 12 * numVertices, vertices_textureColor, GL_STATIC_DRAW);
	}
}

//设置顶点数组颜色
void CGLSL::SetVColor(const vector<vector<int>>& faces, const Vector3f &color, bool ifBackGround)
{
	int cateSize = faces.size();
	for (size_t ci = 0; ci < cateSize; ci++)
	{
		int fSize = faces[ci].size();
		for (size_t i = 0; i < fSize; i++)
		{
			int faceIndex = faces[ci][i];
			for (size_t k = 0; k < 3; k++)
			{
				vertices_color[9 * (3 * faceIndex + k) + 3] = color[0];
				vertices_color[9 * (3 * faceIndex + k) + 4] = color[1];
				vertices_color[9 * (3 * faceIndex + k) + 5] = color[2];

				if (numTextures > 0)
				{
					if (ifBackGround)
					{
						vertices_textureColor[12 * (3 * faceIndex + k) + 6] = 2.0;
						vertices_textureColor[12 * (3 * faceIndex + k) + 7] = 2.0;
						vertices_textureColor[12 * (3 * faceIndex + k) + 8] = 2.0;
					}
					else
					{
						vertices_textureColor[12 * (3 * faceIndex + k) + 6] = color[0];
						vertices_textureColor[12 * (3 * faceIndex + k) + 7] = color[1];
						vertices_textureColor[12 * (3 * faceIndex + k) + 8] = color[2];
					}
				}
			}
		}
	}

	// 1. 绑定顶点数组对象
	glBindVertexArray(vao_color);
	// 2. 把我们的顶点数组复制到一个顶点缓冲中，供OpenGL使用
	glBindBuffer(GL_ARRAY_BUFFER, vbo_color);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 9 * numVertices, vertices_color, GL_STATIC_DRAW);

	if (numTextures > 0)
	{
		// 1. 绑定顶点数组对象
		glBindVertexArray(vao_textureColor);
		// 2. 把我们的顶点数组复制到一个顶点缓冲中，供OpenGL使用
		glBindBuffer(GL_ARRAY_BUFFER, vbo_textureColor);
		glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 12 * numVertices, vertices_textureColor, GL_STATIC_DRAW);
	}
}
