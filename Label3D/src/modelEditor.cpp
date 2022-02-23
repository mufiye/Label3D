#include "modelEditor.h"
#include "graphCut\graphCut.h"
#include <opencv2/core/core.hpp>  
#include <opencv2/highgui/highgui.hpp>  
#include <opencv2/imgproc/imgproc.hpp>

ModelEditor::ModelEditor()
{
	Init();
}

ModelEditor::ModelEditor(const string& fileName)
{
	Init();
	LoadModel(fileName);
}

ModelEditor::~ModelEditor()
{
	Release();
}

//��ʼ��
void ModelEditor::Init()
{
	model = nullptr;
	model_origin = nullptr;
	this->glCamera = new CGLCamera();

	drawType = DrawColor;
	SetMeshColor(Vector3f(166 / 256.0, 166 / 256.0, 166 / 256.0));

	glRenderParam.lightPos0 = Vector4f(100, 100.0f, 100.0f, 1.f);
	glRenderParam.lightPos1 = Vector4f(-100.0f, -100.0f, 100.0f, 1.f);
	glRenderParam.ambiLightStrength = 0.4;
	glRenderParam.diffLightStrength = 0.4;
	glRenderParam.specLightStrength = 0.5;
	glRenderParam.ambiMetriStrength = 0.6;
	glRenderParam.diffMetriStrength = 0.4;
	glRenderParam.specMetriStrength = 0.5;
	glRenderParam.useLight0 = true;
	glRenderParam.useLight1 = false;

	annotation = nullptr;
	labelInfo = nullptr;
	meshGraph = nullptr;
	myGlsl = nullptr;

	selInsIndex = -2;
	selIns = Vector2i(-2, -2);
	ifEdited = false;
	editState = Edit_Render;
	pickType = Pick_Point;
	pivotPointType = PP_ModelCenter;
	ifDrawLine = false;

	m_width = 0;
	m_width = 0;

	paintPixel = 10;

	changeLabeledFace = false;

	if (myGlsl)
	{
		delete myGlsl;
		myGlsl = nullptr;
	}

	LoadCategories();
	//InitGLSL();
	//myGlsl = new CGLSL();
	//UpdateGLSLSize(m_width, m_height);
	//myGlsl->ResetCamera();
}

void ModelEditor::InitGLSL()
{
	if (myGlsl)
	{
		delete myGlsl;
		myGlsl = NULL;
	}
	myGlsl = new CGLSL();
	UpdateGLSLSize(m_width, m_height);
	myGlsl->ResetCamera();
}

//���ر�ע���
void ModelEditor::LoadCategories()
{
	string categoryFile = "data/label3d_categories.json";
	if (this->allCategory.size() > 0)
	{
		this->allCategory.clear();
	}

	std::ifstream ifs(categoryFile);
	Json::CharReaderBuilder readerbuilder;
	//Json::Reader reader;
	Json::Value root;
	JSONCPP_STRING errs;
	bool ok = Json::parseFromStream(readerbuilder, ifs, &root, &errs);
	if (ok)
	{
		Json::Value& categoryNode = root["categories"];
		int num_category = categoryNode.size();
		for (int i = 0; i < num_category; i++)
		{
			LabelInfo::Category category;
			category.categoryId = categoryNode[i]["id"].asInt();
			category.isthing = categoryNode[i]["isthing"].asInt();
			category.categoryName = categoryNode[i]["name"].asString();
			category.supercategory = categoryNode[i]["supercategory"].asString();
			category.cateColor = Eigen::Vector3f(categoryNode[i]["color"][0].asFloat(),
				categoryNode[i]["color"][1].asFloat(),
				categoryNode[i]["color"][2].asFloat());
			this->allCategory.push_back(category);
		}
	}
	ifs.close();
}

//�ͷ�����Global��Դ
void ModelEditor::Release()
{
	if (glCamera)
		delete glCamera;
	glCamera = NULL;

	if (model)
		glmDelete(model);
	model = NULL;
}

//��ʼ��OpenGL����
void ModelEditor::InitializeOpenGL()
{
	//glClearDepth(1.0f);
	//glEnable(GL_LIGHTING);		//���ù���
	GLfloat ambiLightStrength = glRenderParam.ambiLightStrength;
	GLfloat diffLightStrength = glRenderParam.diffLightStrength;
	GLfloat specLightStrength = glRenderParam.specLightStrength;

	GLfloat  ambientLight[] = { ambiLightStrength, ambiLightStrength, ambiLightStrength, 1.0f };	//������ɷ�
	GLfloat  diffuseLight[] = { diffLightStrength, diffLightStrength, diffLightStrength, 1.0f };	//ɢ���ɷ�
	GLfloat  specular[] = { specLightStrength, specLightStrength, specLightStrength, 1.0f };		//�����ɷ�

																									//���ú����ù���0
	GLfloat	 lightPos0[] = { glRenderParam.lightPos0[0],
		glRenderParam.lightPos0[1],
		glRenderParam.lightPos0[2],
		glRenderParam.lightPos0[3] };  //��Դ0λ��
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);		//���û�����
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);		//����ɢ���
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular);		//���þ����
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos0);	//���ù�Դλ��
	if (glRenderParam.useLight0)
	{
		glEnable(GL_LIGHT0);
	}

	//���ú����ù���1
	GLfloat	 lightPos1[] = { glRenderParam.lightPos1[0],
		glRenderParam.lightPos1[1],
		glRenderParam.lightPos1[2],
		glRenderParam.lightPos1[3] };  //��Դ1λ��
	glLightfv(GL_LIGHT1, GL_AMBIENT, ambientLight);		//���û�����
	glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuseLight);		//����ɢ���
	glLightfv(GL_LIGHT1, GL_SPECULAR, specular);		//���þ����
	glLightfv(GL_LIGHT1, GL_POSITION, lightPos1);	//���ù�Դλ��
	if (glRenderParam.useLight1)
	{
		glEnable(GL_LIGHT1);
	}

	GLUquadricObj *quadratic;	// Storage For Our Quadratic Objects
								////����������ͼ
	glEnable(GL_TEXTURE_2D);
	glShadeModel(GL_SMOOTH);        //����ƽ����ɫ
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //�����Ȼ������ɫ����  
	glEnable(GL_DEPTH_TEST);	//���ر���ɾ��
								//glFrontFace(GL_CCW);
								//glEnable(GL_CULL_FACE);	//�����޳�

	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, TRUE);//˫����գ�GL_TRUE������GL_FALSE�ر�
	glDisable(GL_CULL_FACE);

	quadratic = gluNewQuadric();							// Create A Pointer To The Quadric Object (Return 0 If No Memory)
	gluQuadricNormals(quadratic, GLU_SMOOTH);			// Create Smooth Normals 
	gluQuadricTexture(quadratic, GL_TRUE);				// Create Texture Coords 
	glTexGeni(GL_S, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);			// ����s����������Զ�����
	glTexGeni(GL_T, GL_TEXTURE_GEN_MODE, GL_SPHERE_MAP);			// ����t����������Զ�����

	glEnable(GL_LINE_SMOOTH);//�����߿����,��Ե�ή����alphaֵ
	glEnable(GL_BLEND);//���û��
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	/*��ͼ����������Ⱦ�ٶ�֮�����Ȩ���ϵ*/
	glHint(GL_LINE_SMOOTH, GL_NICEST);//û��ƫ��

	GLfloat ambiMateriStrength = glRenderParam.ambiMetriStrength;
	GLfloat diffMateriStrength = glRenderParam.diffMetriStrength;
	GLfloat specMateriStrength = glRenderParam.specMetriStrength;
	GLfloat  ambientf[] = { ambiMateriStrength, ambiMateriStrength, ambiMateriStrength, 1.0f };		//Ϊ������ӵĻ�����������
	GLfloat  diffusef[] = { diffMateriStrength, diffMateriStrength, diffMateriStrength, 1.0f };		//Ϊ������ӵ�����������
	GLfloat  specref[] = { specMateriStrength, specMateriStrength, specMateriStrength, 1.0f };		//Ϊ������ӵľ��淴������

	glEnable(GL_COLOR_MATERIAL);	//������ɫ׷��
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);	//��������ͱ����������,��glColorֵ��Ӧ
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specref);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambientf);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffusef);
	glMateriali(GL_FRONT_AND_BACK, GL_SHININESS, 128);

	//glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);	//���������������,��glColorֵ��Ӧ
	//glMaterialfv(GL_FRONT, GL_SPECULAR, specref);
	//glMaterialfv(GL_FRONT, GL_AMBIENT, ambientf);
	//glMaterialfv(GL_FRONT, GL_DIFFUSE, diffusef);
	//glMateriali(GL_FRONT, GL_SHININESS, 128);

	//glColorMaterial(GL_BACK, GL_AMBIENT_AND_DIFFUSE);	//���ñ����������,��glColorֵ��Ӧ
	//glMaterialfv(GL_BACK, GL_SPECULAR, specref);	//���ñ����������,��glColorֵ��Ӧ
	//glMaterialfv(GL_BACK, GL_AMBIENT, ambientf);
	//glMaterialfv(GL_BACK, GL_DIFFUSE, diffusef);
	//glMateriali(GL_BACK, GL_SHININESS, 128);

	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);	//���ñ�����ɫ	

	glLoadIdentity();
}

//��Ⱦ����
void ModelEditor::RenderScene()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	if (myGlsl)
	{
		if (this->drawType == DrawTexture && model->numtextures > 0)
		{
			myGlsl->Display_texture(myGlsl->camera_);
		}
		else if (this->drawType == DrawTextureColor && model->numtextures > 0)
		{
			myGlsl->Display_textureColor(myGlsl->camera_);
		}
		else
		{
			myGlsl->Display_color(myGlsl->camera_);
		}

		if (ifDrawLine)
		{
			myGlsl->Display_line(myGlsl->camera_);
		}

		////���ư�Χ��
		//if (selInsIndex == -1)	//��������ʵ����Χ��
		//{
		//	for (size_t i = 1; i < annotation->numInstance; i++)
		//	{
		//		DrawSingleBBox(annotation->insBBox[i], annotation->insColors[i]);
		//	}
		//}
		//else if (selInsIndex > 0)	//���Ƶ���������Χ��
		//{
		//	DrawSingleBBox(annotation->insBBox[selInsIndex], annotation->insColors[selInsIndex]);
		//}

		if (selIns[0] >= 0 && selIns[1] >= 0)
		{
			for (size_t ci = 0; ci < labelInfo->categories.size(); ci++)
			{
				if (labelInfo->categories[ci].categoryId == selIns[0])
				{
					int insSIze = labelInfo->categories[ci].instances.size();
					for (size_t ii = 0; ii < insSIze; ii++)
					{
						if (labelInfo->categories[ci].instances[ii].instanceId == selIns[1])
						{
							DrawSingleBBox(labelInfo->categories[ci].instances[ii].insBBox, labelInfo->categories[ci].instances[ii].insColor);
						}
					}
				}
			}
		}
	}

	// ����������  
	SwapBuffers(wglGetCurrentDC());
}

//����ģ��
void ModelEditor::Draw()
{
	//���Ƶ���objģ��
	if (drawType == DrawColor)
	{
		if (model != nullptr)
		{
			glPushMatrix();
			{
				glColor3f(meshColor.x(), meshColor.y(), meshColor.z());
				//GLM_SMOOTH���ö��㷨������Ⱦ;  GLM_FLAT�����淨������Ⱦ
				//glEnable(GL_TEXTURE_2D);//������������
				//glmDraw(model, GLM_FLAT);
				glmDraw(model, GLM_FLAT | GLM_TEXTURE);	//GLM_FLAT | GLM_TEXTURE
			}
			glPopMatrix();

			if (ifDrawLine)
			{
				DrawLine(model);
			}
		}
	}
	//���ƽ���ʵ��
	else if (drawType == DrawType::DrawColor)
	{
		glPushMatrix();
		{
			if (model->numtriangles != annotation->numFaces)
			{
				std::cout << "ģ���ļ����ע�ļ���ƥ��!" << endl;
				drawType = DrawColor;
				return;
			}

			DrawInstance(model, GLM_FLAT, annotation->insFaces, annotation->insColors);

			if (ifDrawLine)
			{
				DrawLine(model);
			}
		}
		glPopMatrix();
	}

	//���ư�Χ��
	if (selInsIndex == -1)	//��������ʵ����Χ��
	{
		for (size_t i = 1; i < annotation->numInstance; i++)
		{
			DrawSingleBBox(annotation->insBBox[i], annotation->insColors[i]);
		}
	}
	else if (selInsIndex > 0)	//���Ƶ���������Χ��
	{
		DrawSingleBBox(annotation->insBBox[selInsIndex], annotation->insColors[selInsIndex]);
	}

	DrawClickPoints();
}

//�����߿�
void ModelEditor::DrawLine(GLMmodel* model)
{
	glPushMatrix();
	{
		glColor3f(0.f, 0.f, 0.f);
		glLineWidth(2.0);
		glmDraw(model, GLM_LINE);
	}
	glPopMatrix();
}

//����ͬ��ɫ����ÿ������ʵ��
void ModelEditor::DrawInstance(const GLMmodel* model, GLuint mode, const vector<vector<int>> &fIndicsPerInstance, const vector<Vector3f> &colors)
{
	GLuint i, j;
	GLuint blenditer, newmaterial, newtexture;
	GLuint blendmodel = 0;
	GLMgroup* group;
	GLMtriangle* triangle;
	GLuint material, map_diffuse;
	GLMmaterial* materialp;

	assert(model);
	assert(model->vertices);

	/* do a bit of warning */
	if (mode & GLM_FLAT && !model->facetnorms) {
		mode &= ~GLM_FLAT;
	}
	if (mode & GLM_SMOOTH && !model->normals) {
		mode &= ~GLM_SMOOTH;
	}
	if (mode & GLM_TEXTURE && !model->texcoords) {
		mode &= ~GLM_TEXTURE;
	}
	if (mode & GLM_FLAT && mode & GLM_SMOOTH) {
		mode &= ~GLM_FLAT;
	}
	if (mode & GLM_COLOR && !model->materials) {
		mode &= ~GLM_COLOR;
	}
	if (mode & GLM_MATERIAL && !model->materials) {
		mode &= ~GLM_MATERIAL;
	}
	if (mode & GLM_COLOR && mode & GLM_MATERIAL) {
		mode &= ~GLM_COLOR;
	}
	if (mode & GLM_COLOR)
		glEnable(GL_COLOR_MATERIAL);
	else if (mode & GLM_MATERIAL)
		glDisable(GL_COLOR_MATERIAL);
	if (mode & GLM_TEXTURE) {
		glEnable(GL_TEXTURE_2D);
		glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	}
	else {
		glDisable(GL_TEXTURE_2D);
	}
#ifdef GLM_2_SIDED
	if (mode & GLM_2_SIDED)
		glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
	else
		//glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_FALSE);
		glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);	//˫�����
#endif

															/* perhaps this loop should be unrolled into material, color, flat,
															smooth, etc. loops?  since most cpu's have good branch prediction
															schemes (and these branches will always go one way), probably
															wouldn't gain too much?  */

															/* CHEESY BLENDING (AKA: NO SORTING)
															If model has blending, use two pass approach, alpha items last */
	for (blenditer = 0; blenditer<2; blenditer++) {
		int blending = 0;
		newmaterial = 0;
		material = -1;
		materialp = NULL;
		newtexture = 0;
		map_diffuse = -1;	/* default material */
		group = model->groups;
		if (group) {
			if (mode & (GLM_MATERIAL | GLM_COLOR | GLM_TEXTURE)) {
				material = group->material;
				materialp = &model->materials[material];
				blending = (materialp->diffuse[3] < 1.0);
				blendmodel |= blending;
				newmaterial = 1;
				if (materialp->map_diffuse != map_diffuse) {
					newtexture = 1;
					map_diffuse = materialp->map_diffuse;
				}
			}

			if (mode & (GLM_FLAT | GLM_SMOOTH | GLM_TEXTURE | GLM_COLOR | GLM_MATERIAL))
				glBegin(GL_TRIANGLES);

			size_t classCount = fIndicsPerInstance.size();
			for (size_t ci = 0; ci < classCount; ci++)
			{
				const vector<int> &faceIndics = fIndicsPerInstance[ci];
				size_t faceCount = faceIndics.size();
				Vector3f insColor = colors[ci];

				glColor3f(insColor.x(), insColor.y(), insColor.z());
				for (size_t fi = 0; fi < faceCount; fi++)
					//for (i = 0; i < group->numtriangles; i++)
				{
					int faceIndex = fIndicsPerInstance[ci][fi];
					triangle = &model->triangles[(group->triangles[faceIndex])];

#ifdef MATERIAL_BY_FACE
					if (mode & (GLM_MATERIAL | GLM_COLOR | GLM_TEXTURE)) {
						/* if the triangle has a different material than the last drawn triangle */
						if (triangle->material && triangle->material != material) {
							material = triangle->material;
							materialp = &model->materials[material];
							blending = (materialp->diffuse[3] < 1.0);
							blendmodel |= blending;
							newmaterial = 1;
							if (materialp->map_diffuse != map_diffuse) {
								newtexture = 1;
								map_diffuse = materialp->map_diffuse;
							}
						}
					}
#endif

					/* render only if in the right blending pass */
					if (blending == blenditer) {
						if (newmaterial) {
							newmaterial = 0;
							if (mode & GLM_TEXTURE) {
								//    if(newtexture) {  //jz
								//	newtexture = 0;     //jz
								glEnd();
								if (map_diffuse == -1)
									glBindTexture(GL_TEXTURE_2D, 0);
								else
									glBindTexture(GL_TEXTURE_2D, model->textures[map_diffuse].id);
								glBegin(GL_TRIANGLES);
								//    }                 //jz
							}
							if (mode & GLM_MATERIAL) {
								glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, materialp->ambient);
								glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, materialp->diffuse);
								glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, materialp->specular);
								glMaterialf(GL_FRONT_AND_BACK, GL_SHININESS, materialp->shininess);
							}
							if (mode & GLM_COLOR) {
								glColor3fv(materialp->diffuse);
							}
						}

						if (mode & GLM_FLAT)
							glNormal3fv(&model->facetnorms[3 * triangle->findex]);

						if (mode == GLM_POINT)
							glBegin(GL_POINTS);
						else if (mode == GLM_LINE)
							glBegin(GL_LINE_LOOP);
						for (j = 0; j < 3; j++) {
							if (mode & GLM_SMOOTH && (triangle->nindices[j] != -1)) {
								assert(triangle->nindices[j] >= 1 && triangle->nindices[j] <= model->numnormals);
								glNormal3fv(&model->normals[3 * triangle->nindices[j]]);
							}
							if (mode & GLM_TEXTURE && (triangle->tindices[j] != -1) && map_diffuse != -1) {
								assert(map_diffuse >= 0 && map_diffuse < model->numtextures);
								assert(triangle->tindices[j] >= 1 && triangle->tindices[j] <= model->numtexcoords);
								glTexCoord2f(model->texcoords[2 * triangle->tindices[j]] * model->textures[map_diffuse].width, model->texcoords[2 * triangle->tindices[j] + 1] * model->textures[map_diffuse].height);
							}

							assert(triangle->vindices[j] >= 1 && triangle->vindices[j] <= model->numvertices);
							glVertex3fv(&model->vertices[3 * triangle->vindices[j]]);
						}
						if (mode == GLM_POINT)
							glEnd();
						else if (mode == GLM_LINE)
							glEnd();
					}

				}
			}
			if (mode & (GLM_FLAT | GLM_SMOOTH | GLM_TEXTURE | GLM_COLOR | GLM_MATERIAL))
				glEnd();

			group = group->next;
		}
		if (!blendmodel)
			break;			/* jump out of the for(blenditer) */
		assert(blendmodel);
		/* Prep for second pass with alpha items */
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE); /* Type Of Blending To Perform */
		glDepthMask(GL_FALSE);	/* Turn off depth mask */
	} /* for(blenditer) */
	if (blendmodel) {
		glDepthMask(GL_TRUE);	/* DISABLE Blending conditions */
		glDisable(GL_BLEND);
	}
}

//���Ƶ���ʵ����Χ��
void ModelEditor::DrawSingleBBox(const CBoundingBox &bBox, const Vector3f &color)
{
	if (bBox.box.size() < 4)
	{
		return;
	}

	glLineWidth(4.0);
	//glColor3f(color[0], color[1], color[2]);

	glColor3f(1.f, 0.f, 0.f);
	double minZ = bBox.midMinMaxPoint[1][2];
	double maxZ = bBox.midMinMaxPoint[2][2];

	glBegin(GL_LINE_LOOP);
	for (int i = 0; i < 4; i++)
	{
		glVertex3f(bBox.box[i][0], bBox.box[i][1], maxZ);
	}
	glEnd();
	glBegin(GL_LINE_LOOP);
	for (int i = 0; i < 4; i++)
	{
		glVertex3f(bBox.box[i][0], bBox.box[i][1], minZ);
	}
	glEnd();
	glBegin(GL_LINES);
	for (int i = 0; i < 4; i++)
	{
		glVertex3f(bBox.box[i][0], bBox.box[i][1], minZ);
		glVertex3f(bBox.box[i][0], bBox.box[i][1], maxZ);
	}
	glEnd();
}

void ModelEditor::DrawSingleBBox(const CBoundingBox &bBox, const Vector3f &color, const Camera* camera)
{
	//
}

//���Ƶ����
void ModelEditor::DrawClickPoints()
{
	glPushMatrix();
	{
		int pSize = polygonPoints_3D.size();
		if (pSize > 0)
		{
			for (size_t i = 0; i < pSize; i++)
			{
				glColor3f(1, 0, 0);
				GLUquadricObj *quadricObj = gluNewQuadric();
				glPushMatrix();
				glTranslatef(polygonPoints_3D[i][0], polygonPoints_3D[i][1], polygonPoints_3D[i][2]);
				gluSphere(quadricObj, 0.2, 10, 5);          //0.001   0.02
				glPopMatrix();
			}

			glLineWidth(4.0);
			glBegin(GL_LINE_STRIP);
			for (size_t i = 0; i < pSize; i++)
			{
				glVertex3f(polygonPoints_3D[i][0], polygonPoints_3D[i][1], polygonPoints_3D[i][2]);
			}
			glEnd();
		}
	}
	glPopMatrix();
}

//����ģ��
void ModelEditor::LoadModel(const string &modelFile)
{
	if (this->model != nullptr)
	{
		glmDelete(this->model);
	}
	if (this->model_origin != nullptr)
	{
		glmDelete(this->model_origin);
	}
	long t1 = time(NULL);
	std::cout << "���ڼ���ģ��(" << modelFile << "):......" << endl;
	this->model = glmReadOBJ((char*)modelFile.c_str());
	this->model_origin = glmReadOBJ((char*)modelFile.c_str());
	std::cout << "����ģ����ɣ�" << round(time(NULL) - t1) << " seconds" << endl;
	std::cout << "������Ƭ������......" << endl;

	glmUnitize(this->model);
	glmFacetNormals(this->model);		//������Ƭ������
										//glmVertexNormals(rmodel, 90.0);	//���㶥�㷨����

	glmComputPosition(this->model);

	Vector3f modelPosition = Vector3f(this->model->position[0], this->model->position[1], this->model->position[2]);
	std::cout << "ģ��λ��: " << "[" << modelPosition.x() << ", " << modelPosition.y() << ", " << modelPosition.z() << "]" << endl << endl;

	SetGLCameraWithModel();

	if (annotation)
	{
		delete annotation;
		annotation = nullptr;
	}

	if (labelInfo)
	{
		delete labelInfo;
		labelInfo = nullptr;
	}

	annotation = new Annotation();
	vector<int> faceSegResult(model->numtriangles);
	for (size_t i = 0; i < model->numtriangles; i++)
	{
		faceSegResult[i] = -1;
	}
	vector<vector<int>> instanceFaces(1);
	for (size_t i = 0; i < faceSegResult.size(); i++)
	{
		instanceFaces[faceSegResult[i] + 1].push_back(i);
	}
	annotation->numFaces = model->numtriangles;
	annotation->numInstance = 1;
	annotation->segInfo = faceSegResult;
	annotation->insFaces = instanceFaces;
	annotation->insColors.push_back(meshColor);
	drawType = DrawType::DrawColor;

	CalcInsBBox();

	if (meshGraph)
	{
		delete meshGraph;
		meshGraph = nullptr;
	}

	if (myGlsl)
	{
		delete myGlsl;
		myGlsl = nullptr;
	}
	myGlsl = new CGLSL();
	UpdateGLSLSize(m_width, m_height);

	labelInfo = new LabelInfo(model);
	for (size_t i = 0; i < this->allCategory.size(); i++)
	{
		labelInfo->categories.push_back(this->allCategory[i]);
	}

	std::cout << "����GLSL......" << endl;
	myGlsl->Init_Easy3d(model);
	std::cout << "����GLSL���" << endl;

	//InitializeOpenGL();
}

//���ݼ��ص�ģ����������ӽ�
void ModelEditor::SetGLCameraWithModel()
{
	if (model != NULL)
	{
		float height = model->widthLengthHeight[2];
		float viewAngle = glCamera->getViewAngle();
		//ȡ�������ı�Ϊ�ױ�
		float lengthOfBottomEdge = model->widthLengthHeight[0] > model->widthLengthHeight[1] ? model->widthLengthHeight[0] : model->widthLengthHeight[1];
		float eyeHeight = lengthOfBottomEdge / 2.0 / tanf((viewAngle / 2.0) * PI / 180);

		Vector3d eyePos(model->position[0], model->position[1], model->position[2] + height / 2.0 + eyeHeight);
		Vector3d target(model->position[0], model->position[1], model->position[2]);
		Vector3d up(0, 1, 0);

		if (true)
		{
			std::cout << "����objģ��������ã�" << endl;
			std::cout << "eyePos: " << "[" << eyePos.x() << ", " << eyePos.y() << ", " << eyePos.z() << "]" << endl;
			std::cout << "target: " << "[" << target.x() << ", " << target.y() << ", " << target.z() << "]" << endl;
			std::cout << "up: " << "[" << up.x() << ", " << up.y() << ", " << up.z() << "]" << endl << endl;
		}

		glCamera->setEyePosTarUp(eyePos, target, up);
	}
}

//����ѡ�е�ʵ������Χ����������ӽ�
void ModelEditor::SetGLCameraWithInsBox(int insIndex, ViewDir direction)
{
	if (model != NULL)
	{
		if (annotation != NULL)
		{
			if (insIndex > 0 && insIndex < annotation->numInstance)
			{
				CBoundingBox& bBox = annotation->insBBox[insIndex];

				if (bBox.midMinMaxPoint.size() > 0)
				{
					float width_x = bBox.midMinMaxPoint[2][0] - bBox.midMinMaxPoint[1][0];
					float width_y = bBox.midMinMaxPoint[2][1] - bBox.midMinMaxPoint[1][1];
					float width_z = bBox.midMinMaxPoint[2][2] - bBox.midMinMaxPoint[1][2];
					float pianyi = width_x / 2.0 + width_y / 2.0;

					float width_max = width_x;
					width_max = width_max < width_y ? width_y : width_max;

					Vector3d eyePos, target, up;
					target = bBox.midMinMaxPoint[0];
					up = Vector3d(0.f, 0.f, 1.f);
					if (direction == View_Down)
					{
						eyePos = bBox.midMinMaxPoint[0] + Vector3d(0, 0, width_max);
						up = Vector3d(0.f, 1.f, 0.f);
					}
					else if (direction == View_Left)
					{
						eyePos = bBox.midMinMaxPoint[0] + Vector3d(0, 0, width_z) + bBox.dirLR * pianyi;
					}
					else if (direction == View_Right)
					{
						eyePos = bBox.midMinMaxPoint[0] + Vector3d(0, 0, width_z) - bBox.dirLR * pianyi;
					}
					else if (direction == View_Front)
					{
						eyePos = bBox.midMinMaxPoint[0] + Vector3d(0, 0, width_z) + bBox.dirFB * pianyi;
					}
					else if (direction == View_Back)
					{
						eyePos = bBox.midMinMaxPoint[0] + Vector3d(0, 0, width_z) - bBox.dirFB * pianyi;
					}

					glCamera->setEyePosTarUp(eyePos, target, up);
				}
			}
		}
	}
}

//����ѡ�е�ʵ������Χ����������ӽ�
void ModelEditor::SetCameraWithInsBox(int insIndex, ViewDir direction)
{
	if (model != nullptr)
	{
		if (annotation != nullptr)
		{
			if (insIndex > 0 && insIndex < annotation->numInstance)
			{
				CBoundingBox& bBox = annotation->insBBox[insIndex];

				if (bBox.midMinMaxPoint.size() > 0)
				{
					float width_x = bBox.midMinMaxPoint[2][0] - bBox.midMinMaxPoint[1][0];
					float width_y = bBox.midMinMaxPoint[2][1] - bBox.midMinMaxPoint[1][1];
					float width_z = bBox.midMinMaxPoint[2][2] - bBox.midMinMaxPoint[1][2];
					float pianyi = width_x / 2.0 + width_y / 2.0;

					float width_max = width_x;
					width_max = width_max < width_y ? width_y : width_max;

					Vector3d eyePos, target, up;
					target = bBox.midMinMaxPoint[0];
					eyePos = bBox.midMinMaxPoint[0] + Vector3d(0, 0, 3 * width_max);
					up = Vector3d(0.f, 1.f, 0.f);

					myGlsl->camera_->setUpVector(easy3d::vec3(up[0], up[1], up[2]));
					myGlsl->camera_->setPosition(easy3d::vec3(eyePos[0], eyePos[1], eyePos[2]));
					myGlsl->camera_->lookAt(easy3d::vec3(target[0], target[1], target[2]));
				}
			}
		}
	}
}

//����ѡ�е�LabelInfoʵ������Χ����������ӽ�
void ModelEditor::SetCameraWithInsBox(int cateId, int insId, ViewDir direction)
{
	if (model != nullptr)
	{
		if (labelInfo != nullptr)
		{
			for (size_t ci = 0; ci < labelInfo->categories.size(); ci++)
			{
				if (labelInfo->categories[ci].categoryId == cateId)
				{
					int insSIze = labelInfo->categories[ci].instances.size();
					for (size_t ii = 0; ii < insSIze; ii++)
					{
						if (labelInfo->categories[ci].instances[ii].instanceId == insId)
						{
							try
							{
								CBoundingBox& bBox = labelInfo->categories[ci].instances[ii].insBBox;

								if (bBox.midMinMaxPoint.size() > 0)
								{
									float width_x = bBox.midMinMaxPoint[2][0] - bBox.midMinMaxPoint[1][0];
									float width_y = bBox.midMinMaxPoint[2][1] - bBox.midMinMaxPoint[1][1];
									float width_z = bBox.midMinMaxPoint[2][2] - bBox.midMinMaxPoint[1][2];
									float pianyi = width_x / 2.0 + width_y / 2.0;

									float width_max = width_x;
									width_max = width_max < width_y ? width_y : width_max;

									Vector3d eyePos, target, up;
									target = bBox.midMinMaxPoint[0];
									eyePos = bBox.midMinMaxPoint[0] + Vector3d(0, 0, 3 * width_max);
									up = Vector3d(0.f, 1.f, 0.f);

									myGlsl->camera_->setUpVector(easy3d::vec3(up[0], up[1], up[2]));
									myGlsl->camera_->setPosition(easy3d::vec3(eyePos[0], eyePos[1], eyePos[2]));
									myGlsl->camera_->lookAt(easy3d::vec3(target[0], target[1], target[2]));
									myGlsl->camera_->setPivotPoint(easy3d::vec3(target[0], target[1], target[2]));
								}
							}
							catch (const std::exception&)
							{
								return;
							}
						}
					}
				}
			}
		}
	}
}

//ˢ��OpenGL��Ⱦ����
void ModelEditor::RefreshGLRenderParm()
{
	GLfloat ambiLightStrength = glRenderParam.ambiLightStrength;
	GLfloat diffLightStrength = glRenderParam.diffLightStrength;
	GLfloat specLightStrength = glRenderParam.specLightStrength;

	GLfloat  ambientLight[] = { ambiLightStrength, ambiLightStrength, ambiLightStrength, 1.0f };	//������ɷ�
	GLfloat  diffuseLight[] = { diffLightStrength, diffLightStrength, diffLightStrength, 1.0f };	//ɢ���ɷ�
	GLfloat  specular[] = { specLightStrength, specLightStrength, specLightStrength, 1.0f };		//�����ɷ�

																									//���ú����ù���0
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);		//���û�����
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);		//����ɢ���
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular);		//���þ����
	if (glRenderParam.useLight0)
	{
		glEnable(GL_LIGHT0);
	}
	else
	{
		glDisable(GL_LIGHT0);
	}

	//���ú����ù���1
	glLightfv(GL_LIGHT1, GL_AMBIENT, ambientLight);		//���û�����
	glLightfv(GL_LIGHT1, GL_DIFFUSE, diffuseLight);		//����ɢ���
	glLightfv(GL_LIGHT1, GL_SPECULAR, specular);		//���þ����
	if (glRenderParam.useLight1)
	{
		glEnable(GL_LIGHT1);
	}
	else
	{
		glDisable(GL_LIGHT1);
	}

	GLfloat ambiMateriStrength = glRenderParam.ambiMetriStrength;
	GLfloat diffMateriStrength = glRenderParam.diffMetriStrength;
	GLfloat specMateriStrength = glRenderParam.specMetriStrength;
	GLfloat  ambientf[] = { ambiMateriStrength, ambiMateriStrength, ambiMateriStrength, 1.0f };		//Ϊ������ӵĻ�����������
	GLfloat  diffusef[] = { diffMateriStrength, diffMateriStrength, diffMateriStrength, 1.0f };		//Ϊ������ӵ�����������
	GLfloat  specref[] = { specMateriStrength, specMateriStrength, specMateriStrength, 1.0f };		//Ϊ������ӵľ��淴������

	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, TRUE);
	glEnable(GL_COLOR_MATERIAL);	//������ɫ׷��
	glColorMaterial(GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE);	//��������ͱ����������,��glColorֵ��Ӧ
	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specref);
	glMaterialfv(GL_FRONT_AND_BACK, GL_AMBIENT, ambientf);
	glMaterialfv(GL_FRONT_AND_BACK, GL_DIFFUSE, diffusef);
	glMateriali(GL_FRONT_AND_BACK, GL_SHININESS, 128);

	//glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);	//���ò�������,��glColorֵ��Ӧ
	//glMaterialfv(GL_FRONT, GL_SPECULAR, specref);
	//glMaterialfv(GL_FRONT, GL_AMBIENT, ambientf);
	//glMaterialfv(GL_FRONT, GL_DIFFUSE, diffusef);
	//glMateriali(GL_FRONT, GL_SHININESS, 128);

	//glColorMaterial(GL_BACK, GL_AMBIENT_AND_DIFFUSE);	//���������������,��glColorֵ��Ӧ
	//glMaterialfv(GL_BACK, GL_SPECULAR, specref);	//���ñ����������,��glColorֵ��Ӧ
	//glMaterialfv(GL_BACK, GL_AMBIENT, ambientf);
	//glMaterialfv(GL_BACK, GL_DIFFUSE, diffusef);
	//glMateriali(GL_BACK, GL_SHININESS, 128);

	return;
}

#if 0
//���ر�ע
bool ModelEditor::LoadAnnocation(const string& fileName)
{
	if (model == NULL)
	{
		std::cout << "���ȼ���ģ�ͣ�" << endl;
		return false;
	}

	if (annotation != NULL)
	{
		delete annotation;
		annotation = NULL;
	}

	this->annotation = new Annotation();

	vector<int> faceSegResult = filetool::ReadTxtFileInt(fileName);
	int annFaceSize = faceSegResult.size();
	if (annFaceSize != model->numtriangles)
	{
		std::cout << "��ע�ļ���ģ�Ͳ�ƥ�䣡" << endl;
		return false;
	}

	int maxIndex = *std::max_element(faceSegResult.begin(), faceSegResult.end());
	int instanceCount = maxIndex + 2;
	vector<vector<int>> instanceFaces(instanceCount);
	for (size_t i = 0; i < faceSegResult.size(); i++)
	{
		instanceFaces[faceSegResult[i] + 1].push_back(i);
	}

	vector<Vector3f> userColor;	//�û�ָ����ɫ
								//userColor.push_back(Vector3f(166 / 256.0, 166 / 256.0, 166 / 256.0));	//��ɫ
	userColor.push_back(Vector3f(1.0, 0.0, 0.0));	//��ɫ
	userColor.push_back(Vector3f(0.0, 1.0, 0.0));	//��ɫ
	userColor.push_back(Vector3f(0.0, 0.0, 1.0));	//��ɫ
	userColor.push_back(Vector3f(77 / 256.0, 255 / 256.0, 239 / 256.0));	//��ɫ
	userColor.push_back(Vector3f(244 / 256.0, 74 / 256.0, 229 / 256.0));	//����
	userColor.push_back(Vector3f(15 / 256.0, 167 / 256.0, 255 / 256.0));	//��ɫ
	userColor.push_back(Vector3f(239 / 256.0, 255 / 256.0, 77 / 256.0));	//����

																			//Ϊÿ��ʵ�����������ɫ
	srand(38);		//srand(45) srand(38) 
	annotation->insColors.clear();
	annotation->insColors.push_back(meshColor);
	{
		for (int i = 0; i < (instanceCount - 1); i++)
		{
			Vector3f color;
			if (i < userColor.size())
			{
				color = userColor[i];
			}
			else
			{
				color[0] = (float)Random();
				color[1] = (float)Random();
				color[2] = (float)Random();
			}
			annotation->insColors.push_back(color);
		}
	}

	annotation->numFaces = annFaceSize;
	annotation->numInstance = instanceCount;
	annotation->segInfo = faceSegResult;
	annotation->insFaces = instanceFaces;
	drawType = DrawType::DrawColor;

	CalcInsBBox();

	UpGlsl_V_Color(annotation);
	return true;
}
#endif // 0

//���ر�ע
bool ModelEditor::LoadAnnocation(const string& fileName)
{
	if (model == nullptr)
	{
		std::cout << "���ȼ���ģ�ͣ�" << endl;
		return false;
	}

	if (annotation != nullptr)
	{
		delete annotation;
		annotation = nullptr;
	}
	this->annotation = new Annotation();

	if (labelInfo != nullptr)
	{
		delete labelInfo;
		labelInfo = new LabelInfo(this->model);
	}
	labelInfo->ReadFromFile(fileName);

	this->myGlsl->UpdateVBuffer(labelInfo);

	drawType = DrawType::DrawColor;
	return true;
}


//���㵥��ʵ����Χ��
void ModelEditor::CalcInsBBox(int insIndex)
{
	int insCount = annotation->numInstance;
	if (insCount <= 0)
	{
		std::cout << "���ȼ��طָ�����" << endl << endl;
		return;
	}

	vector<Vector3d> insFaces;
	int includeFSize = annotation->insFaces[insIndex].size();
	if (includeFSize >0)
	{
		for (size_t fi = 0; fi < includeFSize; fi++)
		{
			int faceIndex = annotation->insFaces[insIndex][fi];
			for (size_t k = 0; k < 3; k++)
			{
				double x = model->vertices[3 * model->triangles[faceIndex].vindices[k] + 0];
				double y = model->vertices[3 * model->triangles[faceIndex].vindices[k] + 1];
				double z = model->vertices[3 * model->triangles[faceIndex].vindices[k] + 2];
				insFaces.push_back(Vector3d(x, y, z));
			}
		}
		annotation->insBBox[insIndex].InitBox(insFaces);
	}
}

//����ʵ����Χ��
void ModelEditor::CalcInsBBox()
{
	int insCount = annotation->numInstance;
	if (insCount <= 0)
	{
		std::cout << "���ȼ��طָ�����" << endl << endl;
		return;
	}

	annotation->insBBox.clear();
	annotation->insBBox.resize(insCount);
	for (size_t i = 1; i < insCount; i++)
	{
		vector<Vector3d> insFaces;
		int includeFSize = annotation->insFaces[i].size();
		if (includeFSize >0)
		{
			for (size_t fi = 0; fi < includeFSize; fi++)
			{
				int faceIndex = annotation->insFaces[i][fi];
				for (size_t k = 0; k < 3; k++)
				{
					double x = model->vertices[3 * model->triangles[faceIndex].vindices[k] + 0];
					double y = model->vertices[3 * model->triangles[faceIndex].vindices[k] + 1];
					double z = model->vertices[3 * model->triangles[faceIndex].vindices[k] + 2];
					insFaces.push_back(Vector3d(x, y, z));
				}
			}
			annotation->insBBox[i].InitBox(insFaces);
		}
	}
}

//ѡ�񵥸�ʵ��
int ModelEditor::SeletcInstance(Vector3d cameraPos, Vector3d ray_dir)
{
	return 1;
}

bool IntersectTriangle(const Vector3d& orig, const Vector3d& dir, Vector3d& v0, Vector3d& v1, Vector3d& v2, double* t, double* u, double* v)
{
	Vector3d E1 = v1 - v0;	// E1
	Vector3d E2 = v2 - v0;	// E2
	Vector3d P = dir.cross(E2);	// P

	double det = E1.dot(P);	// determinant

							// keep det > 0, modify T accordingly
	Vector3d T;
	if (det > 0)
	{
		T = orig - v0;
	}
	else
	{
		T = v0 - orig;
		det = -det;
	}

	// If determinant is near zero, ray lies in plane of triangle
	//if (det < 0.0001f)
	if (det < 0.0000001f)
		return false;

	// Calculate u and make sure u <= 1
	*u = T.dot(P);
	if (*u < 0.0f || *u > det)
		return false;

	// Q
	Vector3d Q = T.cross(E1);

	// Calculate v and make sure u + v <= 1
	*v = dir.dot(Q);
	if (*v < 0.0f || *u + *v > det)
		return false;

	// Calculate t, scale parameters, ray intersects triangle
	*t = E2.dot(Q);

	double fInvDet = 1.0f / det;
	*t *= fInvDet;
	*u *= fInvDet;
	*v *= fInvDet;

	return true;
}

// ��ȡ�������ཻ��������Ƭ����
int GetInterTriIndex(const Vector3d &cameraPos, const Vector3d &ray_dir, const MeshGraph* mesh)
{
	vector<int> triIndics;
	vector<double> distances;

	const vector<SVertex> &vertics = mesh->vertices;	//��������
	const vector<STriangle> &faces = mesh->triangles;	//������Ƭ����
	int numFaces = faces.size();

	double interPoint[3];
	for (size_t i = 0; i < numFaces; i++)
	{
		Vector3d v0(vertics[faces[i].vIndex[0]].pos[0], vertics[faces[i].vIndex[0]].pos[1], vertics[faces[i].vIndex[0]].pos[2]);
		Vector3d v1(vertics[faces[i].vIndex[1]].pos[0], vertics[faces[i].vIndex[1]].pos[1], vertics[faces[i].vIndex[1]].pos[2]);
		Vector3d v2(vertics[faces[i].vIndex[2]].pos[0], vertics[faces[i].vIndex[2]].pos[1], vertics[faces[i].vIndex[2]].pos[2]);
		bool ifInter = IntersectTriangle(cameraPos, ray_dir, v0, v1, v2, &interPoint[0], &interPoint[1], &interPoint[2]);

		if (ifInter)
		{
			triIndics.push_back(i);
			Vector3d interPos = (1 - interPoint[1] - interPoint[2])*v0 + interPoint[1] * v1 + interPoint[2] * v2;
			double distance = pow(cameraPos.x() - interPos[0], 2) + pow(cameraPos.y() - interPos[1], 2) + pow(cameraPos.z() - interPos[2], 2);
			distances.push_back(distance);
		}
	}

	int triIndex = -1;
	double distance = 999999999999;
	for (size_t i = 0; i < triIndics.size(); i++)
	{
		if (distances[i] < distance)
		{
			triIndex = triIndics[i];
			distance = distances[i];
		}
	}
	return triIndex;
}

// ��ȡ�������ཻ��������Ƭ����
int GetInterTriIndex(const Vector3d &cameraPos, const Vector3d &ray_dir, const MeshGraph* mesh, Vector3d &jiaodian)
{
	vector<int> triIndics;
	vector<double> distances;
	vector<Vector3d> poses;

	const vector<SVertex> &vertics = mesh->vertices;	//��������
	const vector<STriangle> &faces = mesh->triangles;	//������Ƭ����
	int numFaces = faces.size();

	Vector3d interPoint;
	for (size_t i = 0; i < numFaces; i++)
	{
		Vector3d v0(vertics[faces[i].vIndex[0]].pos[0], vertics[faces[i].vIndex[0]].pos[1], vertics[faces[i].vIndex[0]].pos[2]);
		Vector3d v1(vertics[faces[i].vIndex[1]].pos[0], vertics[faces[i].vIndex[1]].pos[1], vertics[faces[i].vIndex[1]].pos[2]);
		Vector3d v2(vertics[faces[i].vIndex[2]].pos[0], vertics[faces[i].vIndex[2]].pos[1], vertics[faces[i].vIndex[2]].pos[2]);
		bool ifInter = IntersectTriangle(cameraPos, ray_dir, v0, v1, v2, &interPoint[0], &interPoint[1], &interPoint[2]);

		if (ifInter)
		{
			triIndics.push_back(i);

			Vector3d interPos = (1 - interPoint[1] - interPoint[2])*v0 + interPoint[1] * v1 + interPoint[2] * v2;
			double distance = pow(cameraPos.x() - interPos[0], 2) + pow(cameraPos.y() - interPos[1], 2) + pow(cameraPos.z() - interPos[2], 2);
			distances.push_back(distance);
			poses.push_back(interPos);
		}
	}

	int triIndex = -1;
	double distance = 999999999999;
	for (size_t i = 0; i < triIndics.size(); i++)
	{
		if (distances[i] < distance)
		{
			triIndex = triIndics[i];
			distance = distances[i];
			jiaodian = poses[i];
		}
	}
	return triIndex;
}

//ѡ�񵥸���Ƭ����
int ModelEditor::SelectFace_Ray(Vector3d cameraPos, Vector3d ray_dir)
{
	if (!meshGraph)
	{
		meshGraph = new MeshGraph();
		meshGraph->CreateGraph(model);
	}

	int triIndex = GetInterTriIndex(cameraPos, ray_dir, meshGraph);
	return triIndex;
}

//ѡ�񵥸���Ƭ����
int ModelEditor::SelectFace_Ray(Vector3d cameraPos, Vector3d ray_dir, Vector3d &jiaodian)
{
	if (!meshGraph)
	{
		meshGraph = new MeshGraph();
		meshGraph->CreateGraph(model);
	}

	int triIndex = GetInterTriIndex(cameraPos, ray_dir, meshGraph, jiaodian);
	return triIndex;
}

//ѡ�񵥸���Ƭ����
int ModelEditor::SelectFace_GPU(int x, int y)
{
	return -1;
	//float mat[16];

	//glGetFloatv(GL_PROJECTION_MATRIX, mat);
	//mat4 proj(mat[0], mat[1], mat[2], mat[3],
	//	mat[4], mat[5], mat[6], mat[7], 
	//	mat[8], mat[9], mat[10], mat[11], 
	//	mat[12], mat[13], mat[14], mat[15]);
	//pickerView->camera()->set_projection_matrix(proj);

	//glGetFloatv(GL_MODELVIEW_MATRIX, mat);
	//mat4 mv(mat[0], mat[1], mat[2], mat[3],
	//	mat[4], mat[5], mat[6], mat[7],
	//	mat[8], mat[9], mat[10], mat[11],
	//	mat[12], mat[13], mat[14], mat[15]);;
	//pickerView->camera()->set_modelview_matrix(mv);

	//int faceIndex = pickerView->pickFaceIndex(x, y);

	//return faceIndex;
}

//�����ı�������µ�ʵ��
bool ModelEditor::AddNewInsRec(const vector<Vector3d> &rec)
{
	vector<int> facesInBBox;
	vector<Vector3d> insFaces;

	int fSize = model->numtriangles;
	for (size_t fi = 0; fi < fSize; fi++)
	{
		const Vector3d v0 = Vector3d(model->vertices[3 * model->triangles[fi].vindices[0] + 0],
			model->vertices[3 * model->triangles[fi].vindices[0] + 1],
			model->vertices[3 * model->triangles[fi].vindices[0] + 2]);
		const Vector3d v1 = Vector3d(model->vertices[3 * model->triangles[fi].vindices[1] + 0],
			model->vertices[3 * model->triangles[fi].vindices[1] + 1],
			model->vertices[3 * model->triangles[fi].vindices[1] + 2]);
		const Vector3d v2 = Vector3d(model->vertices[3 * model->triangles[fi].vindices[2] + 0],
			model->vertices[3 * model->triangles[fi].vindices[2] + 1],
			model->vertices[3 * model->triangles[fi].vindices[2] + 2]);

		if (modeltool::IfPointInPolygon(v0, rec)
			&& modeltool::IfPointInPolygon(v1, rec)
			&& modeltool::IfPointInPolygon(v2, rec))
		{
			int insIndex_old = annotation->segInfo[fi] + 1;
			vector<int>::iterator iter = find(annotation->insFaces[insIndex_old].begin(), annotation->insFaces[insIndex_old].end(), fi);
			if (iter != annotation->insFaces[insIndex_old].end())
			{
				annotation->insFaces[insIndex_old].erase(iter);
			}

			facesInBBox.push_back(fi);
			insFaces.push_back(v0);
			insFaces.push_back(v1);
			insFaces.push_back(v2);
		}
	}
	if (facesInBBox.size() <= 0)
	{
		return false;
	}

	annotation->insFaces.push_back(facesInBBox);
	Vector3f insColor;
	insColor[0] = (float)Random();
	insColor[1] = (float)Random();
	insColor[2] = (float)Random();
	annotation->insColors.push_back(insColor);
	CBoundingBox bbox;
	bbox.InitBox(insFaces);
	annotation->insBBox.push_back(bbox);

	for (size_t i = 0; i < facesInBBox.size(); i++)
	{
		annotation->segInfo[facesInBBox[i]] = annotation->numInstance - 1;
	}

	annotation->numInstance++;
	return true;
}

//����,startIndex��>=1
void SortFSegVec(vector<vector<int>> &fSegResult, int startIndex)
{
	int k = fSegResult.size();
	if (startIndex < 1 || k <= 1)
	{
		return;
	}
	bool flag = true;
	while (flag)
	{
		flag = false;
		for (int i = startIndex; i < k; i++)
		{
			//if (simiCountList[i - 1].simiMaskId.size() < simiCountList[i].simiMaskId.size()) 
			if (fSegResult[i - 1].size() < fSegResult[i].size())
			{
				swap(fSegResult[i - 1], fSegResult[i]);
				flag = true;
			}
		}
		k--;
	}
}

//����ʵ���ϲ�������Ƭ
bool ModelEditor::MergeBackForSingleIns(int insIndex)
{
	if (insIndex <= 0)
	{
		return false;
	}

	if (meshGraph == NULL)
	{
		meshGraph = new MeshGraph();
		meshGraph->CreateGraph(model, true, true);
	}

	vector<int> &segResult = annotation->segInfo;

	//�Ա�����Ƭ����ͨ�Ծ���
	long t1 = time(NULL);
	std::cout << "��ʼ�Ա�����Ƭ����......" << endl;
	vector<int> &backFaces = annotation->insFaces[0];
	vector<bool> isFVisited(annotation->numFaces);	//��¼ÿ����Ƭ�Ƿ񱻷��ʹ�
	for (int i = 0; i < annotation->numFaces; i++)
	{
		isFVisited[i] = false;
	}
	vector<vector<int>> fCluResult;	//��Ƭ������
	queue<int> fIndics;	//����������Ƭ����
	int backFaceCount = backFaces.size();
	for (int fi = 0; fi < backFaceCount; fi++)
	{
		if (!isFVisited[backFaces[fi]])
		{
			fIndics.push(backFaces[fi]);
			vector<int> facesThisClass;	//��һ���������Ƭ
			facesThisClass.push_back(backFaces[fi]);
			isFVisited[backFaces[fi]] = true;
			while (!fIndics.empty())
			{
				int faceIndex = fIndics.front();
				//ȡ���뵱ǰface�ڽӵ���Ƭ�б�
				//vector<int> &adjFaces = meshGraph->adjFacesPerFace[faceIndex];
				const vector<int> &adjFaces = meshGraph->adjFacesPerFace_Edge[faceIndex];
				for (int j = 0; j < adjFaces.size(); j++)
				{
					int fIndex = adjFaces[j];
					//�ж������Ƭ�Ƿ�Ϊ����,����δ�����ʹ�
					if (segResult[fIndex] == -1 && !isFVisited[fIndex])
					{
						fIndics.push(fIndex);
						facesThisClass.push_back(fIndex);
						isFVisited[fIndex] = true;
					}
				}
				fIndics.pop();
			}
			fCluResult.push_back(facesThisClass);
		}
	}

	int classCount = fCluResult.size();
	std::cout << "�������: " << round(time(NULL) - t1) << " seconds" << endl;
	std::cout << "��: " << classCount << " ����ͨ����" << endl;
	//�ж��Ƿ񱻽���ʵ����Ƭ��Χ
	SortFSegVec(fCluResult, 1);
	std::cout << "��ʼ�ϲ�����Χ������......" << endl;
	for (int i = 1; i < classCount; i++)
	{
		//std::cout << "�ϲ��� " << i << " �鱳����Ƭ��: " << fCluResult[i].size() << endl;
		vector<int> &patchFaces = fCluResult[i];	//��ͨ������Ƭ
		int adjBuildIndex = -1;
		//ȡ�뱻��Χ���ݶ������ڽ������ݶ���Ƭ����Ϊ���ݶ������
		vector<int> maxCountV;
		bool ifMerge = true;	//�Ƿ�ϲ�
		bool ifOnlyBack = true;	//�ڽ���Ƭ�Ƿ�ֻ�б���
		for (int j = 0; j < patchFaces.size(); j++)
		{
			int faceIndex = patchFaces[j];
			//ȡ���뵱ǰface�ڽӵ���Ƭ�б�
			const vector<int> &adjFaces = meshGraph->adjFacesPerFace_Edge[faceIndex];
			//vector<int> &adjFaces = meshGraph->adjFacesPerFace[faceIndex];
			for (int k = 0; k < adjFaces.size(); k++)
			{
				if (segResult[adjFaces[k]] >= 0)
				{
					ifOnlyBack = false;
					if (segResult[adjFaces[k]] != (insIndex - 1))
					{
						ifMerge = false;
						break;
					}
				}
			}
		}

		if (ifMerge && (!ifOnlyBack))
		{
			for (int j = 0; j < patchFaces.size(); j++)
			{
				//segResult[patchFaces[j]] = adjBuildIndex;
				annotation->AddMesh(patchFaces[j], insIndex);
			}
		}
	}
	std::cout << "�ϲ���ɣ�" << round(time(NULL) - t1) << " s" << endl << endl;
	return true;
}

//����ʵ��ȥ������
bool ModelEditor::ClearNoiseForSingleIns(int insIndex)
{
	if (insIndex <= 0)
		return false;

	if (meshGraph == NULL)
	{
		meshGraph = new MeshGraph();
		meshGraph->CreateGraph(model, true, true);
	}

	vector<int> &segResult = annotation->segInfo;

	//��ʵ����Ƭ����ͨ�Ծ���
	long t1 = time(NULL);
	std::cout << "��ʼ��ʵ����Ƭ����......" << endl;
	vector<int> &insFaces = annotation->insFaces[insIndex];
	vector<bool> isFVisited(annotation->numFaces);	//��¼ÿ����Ƭ�Ƿ񱻷��ʹ�
	for (int i = 0; i < annotation->numFaces; i++)
	{
		isFVisited[i] = false;
	}
	vector<vector<int>> fCluResult;	//��Ƭ������
	queue<int> fIndics;	//����������Ƭ����
	int insFaceCount = insFaces.size();
	for (int fi = 0; fi < insFaceCount; fi++)
	{
		if (!isFVisited[insFaces[fi]])
		{
			fIndics.push(insFaces[fi]);
			vector<int> facesThisClass;	//��һ���������Ƭ
			facesThisClass.push_back(insFaces[fi]);
			isFVisited[insFaces[fi]] = true;
			while (!fIndics.empty())
			{
				int faceIndex = fIndics.front();
				//ȡ���뵱ǰface�ڽӵ���Ƭ�б�
				//vector<int> &adjFaces = meshGraph->adjFacesPerFace[faceIndex];
				const vector<int> &adjFaces = meshGraph->adjFacesPerFace_Edge[faceIndex];
				for (int j = 0; j < adjFaces.size(); j++)
				{
					int fIndex = adjFaces[j];
					//�ж������Ƭ�Ƿ�Ϊ����,����δ�����ʹ�
					if (segResult[fIndex] == (insIndex - 1) && !isFVisited[fIndex])
					{
						fIndics.push(fIndex);
						facesThisClass.push_back(fIndex);
						isFVisited[fIndex] = true;
					}
				}
				fIndics.pop();
			}
			fCluResult.push_back(facesThisClass);
		}
	}

	int classCount = fCluResult.size();
	std::cout << "�������: " << round(time(NULL) - t1) << " seconds" << endl;
	std::cout << "��: " << classCount << " ����ͨ����" << endl;
	//�ж��Ƿ񱻽���ʵ����Ƭ��Χ
	SortFSegVec(fCluResult, 1);

	if (classCount > 0)
	{
		annotation->insFaces[insIndex] = fCluResult[0];
		for (size_t i = 1; i < classCount; i++)
		{
			for (size_t j = 0; j < fCluResult[i].size(); j++)
			{
				annotation->insFaces[0].push_back(fCluResult[i][j]);
				annotation->segInfo[fCluResult[i][j]] = -1;

			}
		}
		std::cout << "ȥ���������" << endl;
		return true;
	}
	else
	{
		std::cout << "ȥ���������" << endl;
		return false;
	}
}

//����������ȡ�ݶ�
bool ModelEditor::GetRoofForSingleIns(int insIndex, int roofNum)
{
	if (insIndex <= 0)
	{
		return false;
	}

	if (meshGraph == NULL)
	{
		meshGraph = new MeshGraph();
		meshGraph->CreateGraph(model, true, true);
	}

	vector<int> &segResult = annotation->segInfo;
	vector<int> &insFaces = annotation->insFaces[insIndex];
	//��һ����ȥ����ֱ��Ƭ
	vector<int> verticalFaces;	//��ֱ��Ƭ
	vector<int> horizontalFaces;	//ˮƽ��Ƭ
	size_t insFaceCount = insFaces.size();
	for (size_t i = 0; i < insFaceCount; i++)
	{
		if (meshGraph->triangles[insFaces[i]].ifVertical60)
		{
			verticalFaces.push_back(insFaces[i]);
		}
		else
		{
			horizontalFaces.push_back(insFaces[i]);
		}
	}

	int vFacesCount = verticalFaces.size();
	for (size_t i = 0; i < vFacesCount; i++)
	{
		segResult[verticalFaces[i]] = -1;
		annotation->insFaces[0].push_back(verticalFaces[i]);
	}
	insFaces = horizontalFaces;

	//�ڶ�����ȥ��
	long t1 = time(NULL);
	std::cout << "��ʼ��ʵ����Ƭ����......" << endl;
	vector<bool> isFVisited(annotation->numFaces);	//��¼ÿ����Ƭ�Ƿ񱻷��ʹ�
	for (int i = 0; i < annotation->numFaces; i++)
	{
		isFVisited[i] = false;
	}
	vector<vector<int>> fCluResult;	//��Ƭ������
	queue<int> fIndics;	//����������Ƭ����
	insFaceCount = insFaces.size();
	for (int fi = 0; fi < insFaceCount; fi++)
	{
		if (!isFVisited[insFaces[fi]])
		{
			fIndics.push(insFaces[fi]);
			vector<int> facesThisClass;	//��һ���������Ƭ
			facesThisClass.push_back(insFaces[fi]);
			isFVisited[insFaces[fi]] = true;
			while (!fIndics.empty())
			{
				int faceIndex = fIndics.front();
				//ȡ���뵱ǰface�ڽӵ���Ƭ�б�
				//vector<int> &adjFaces = meshGraph->adjFacesPerFace[faceIndex];
				const vector<int> &adjFaces = meshGraph->adjFacesPerFace_Edge[faceIndex];
				for (int j = 0; j < adjFaces.size(); j++)
				{
					int fIndex = adjFaces[j];
					//�ж������Ƭ�Ƿ�Ϊ����,����δ�����ʹ�
					if (segResult[fIndex] == (insIndex - 1) && !isFVisited[fIndex])
					{
						fIndics.push(fIndex);
						facesThisClass.push_back(fIndex);
						isFVisited[fIndex] = true;
					}
				}
				fIndics.pop();
			}
			fCluResult.push_back(facesThisClass);
		}
	}

	int classCount = fCluResult.size();
	std::cout << "�������: " << round(time(NULL) - t1) << " seconds" << endl;
	std::cout << "��: " << classCount << " ����ͨ����" << endl;
	SortFSegVec(fCluResult, 1);

	//ֻ������ͨ����������
	for (size_t i = roofNum; i < fCluResult.size(); i++)
	{

		int rFacesCount = fCluResult[i].size();
		for (size_t j = 0; j < rFacesCount; j++)
		{
			segResult[fCluResult[i][j]] = -1;
			annotation->insFaces[0].push_back(fCluResult[i][j]);
		}
	}

	roofNum = roofNum > classCount ? classCount : roofNum;
	vector<int> roofFaces;
	for (size_t i = 0; i < roofNum; i++)
	{
		int size_ci = fCluResult[i].size();
		for (size_t j = 0; j < size_ci; j++)
		{
			roofFaces.push_back(fCluResult[i][j]);
		}
	}
	insFaces = roofFaces;
	return true;
}

//����ʵ�����
double ModelEditor::CalcInsArea(int insIndex)
{
	if (insIndex <= 0)
	{
		return -1;
	}

	if (meshGraph == NULL)
	{
		meshGraph = new MeshGraph();
		meshGraph->CreateGraph(model, true, false);
	}

	vector<int> &insFaces = annotation->insFaces[insIndex];

	double area = 0;
	int insFaceCount = insFaces.size();
	for (size_t i = 0; i < insFaceCount; i++)
	{
		area += meshGraph->triangles[insFaces[i]].area;
	}

	return area;
}

//GLSL VAO,VBO����
void ModelEditor::DrawWithVbo()
{

}

void ModelEditor::UpdateGLSLSize(int m_width, int m_height)
{
	this->m_width = m_width;
	this->m_height = m_height;
	if (this->myGlsl)
	{
		myGlsl->scr_width = m_width;
		myGlsl->scr_height = m_height;
	}
}

//���ݱ�ע��Ϣ����Glsl��ɫ��������
void ModelEditor::UpGlsl_V_Color(const Annotation* annotation)
{
	for (size_t i = 0; i < annotation->numFaces; i++)
	{
		for (size_t k = 0; k < 3; k++)
		{
			//��ɫ
			myGlsl->vertices_color[9 * (3 * i + k) + 3] = annotation->insColors[annotation->segInfo[i] + 1][0];
			myGlsl->vertices_color[9 * (3 * i + k) + 4] = annotation->insColors[annotation->segInfo[i] + 1][1];
			myGlsl->vertices_color[9 * (3 * i + k) + 5] = annotation->insColors[annotation->segInfo[i] + 1][2];
			int insIndex = annotation->segInfo[i];
			if (insIndex >= 0)
			{
				myGlsl->vertices_textureColor[12 * (3 * i + k) + 6] = annotation->insColors[annotation->segInfo[i] + 1][0];
				myGlsl->vertices_textureColor[12 * (3 * i + k) + 7] = annotation->insColors[annotation->segInfo[i] + 1][1];
				myGlsl->vertices_textureColor[12 * (3 * i + k) + 8] = annotation->insColors[annotation->segInfo[i] + 1][2];
			}
		}
	}

	// 1. �󶨶����������
	glBindVertexArray(myGlsl->vao_color);
	// 2. �����ǵĶ������鸴�Ƶ�һ�����㻺���У���OpenGLʹ��
	glBindBuffer(GL_ARRAY_BUFFER, myGlsl->vbo_color);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 9 * myGlsl->numVertices, myGlsl->vertices_color, GL_STATIC_DRAW);

	// 1. �󶨶����������
	glBindVertexArray(myGlsl->vao_textureColor);
	// 2. �����ǵĶ������鸴�Ƶ�һ�����㻺���У���OpenGLʹ��
	glBindBuffer(GL_ARRAY_BUFFER, myGlsl->vbo_textureColor);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 12 * myGlsl->numVertices, myGlsl->vertices_textureColor, GL_STATIC_DRAW);
}

void ModelEditor::AddMesh(int faceIndex, int insIndex)
{
	annotation->AddMesh(faceIndex, insIndex);
	for (size_t k = 0; k < 3; k++)
	{
		myGlsl->vertices_color[9 * (3 * faceIndex + k) + 3] = annotation->insColors[insIndex][0];
		myGlsl->vertices_color[9 * (3 * faceIndex + k) + 4] = annotation->insColors[insIndex][1];
		myGlsl->vertices_color[9 * (3 * faceIndex + k) + 5] = annotation->insColors[insIndex][2];

		myGlsl->vertices_textureColor[12 * (3 * faceIndex + k) + 6] = annotation->insColors[insIndex][0];
		myGlsl->vertices_textureColor[12 * (3 * faceIndex + k) + 7] = annotation->insColors[insIndex][1];
		myGlsl->vertices_textureColor[12 * (3 * faceIndex + k) + 8] = annotation->insColors[insIndex][2];
	}

	// 1. �󶨶����������
	glBindVertexArray(myGlsl->vao_color);
	// 2. �����ǵĶ������鸴�Ƶ�һ�����㻺���У���OpenGLʹ��
	glBindBuffer(GL_ARRAY_BUFFER, myGlsl->vbo_color);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 9 * myGlsl->numVertices, myGlsl->vertices_color, GL_STATIC_DRAW);

	// 1. �󶨶����������
	glBindVertexArray(myGlsl->vao_textureColor);
	// 2. �����ǵĶ������鸴�Ƶ�һ�����㻺���У���OpenGLʹ��
	glBindBuffer(GL_ARRAY_BUFFER, myGlsl->vbo_textureColor);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 12 * myGlsl->numVertices, myGlsl->vertices_textureColor, GL_STATIC_DRAW);
}

void ModelEditor::AddMesh(vector<int> faceIndices, int insIndex)
{
	for (size_t fi = 0; fi < faceIndices.size(); fi++)
	{
		int faceIndex = faceIndices[fi];

		if (faceIndex != insIndex)
		{
			annotation->AddMesh(faceIndex, insIndex);
			for (size_t k = 0; k < 3; k++)
			{
				myGlsl->vertices_color[9 * (3 * faceIndex + k) + 3] = annotation->insColors[insIndex][0];
				myGlsl->vertices_color[9 * (3 * faceIndex + k) + 4] = annotation->insColors[insIndex][1];
				myGlsl->vertices_color[9 * (3 * faceIndex + k) + 5] = annotation->insColors[insIndex][2];

				myGlsl->vertices_textureColor[12 * (3 * faceIndex + k) + 6] = annotation->insColors[insIndex][0];
				myGlsl->vertices_textureColor[12 * (3 * faceIndex + k) + 7] = annotation->insColors[insIndex][1];
				myGlsl->vertices_textureColor[12 * (3 * faceIndex + k) + 8] = annotation->insColors[insIndex][2];
			}
		}
	}

	// 1. �󶨶����������
	glBindVertexArray(myGlsl->vao_color);
	// 2. �����ǵĶ������鸴�Ƶ�һ�����㻺���У���OpenGLʹ��
	glBindBuffer(GL_ARRAY_BUFFER, myGlsl->vbo_color);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 9 * myGlsl->numVertices, myGlsl->vertices_color, GL_STATIC_DRAW);

	// 1. �󶨶����������
	glBindVertexArray(myGlsl->vao_textureColor);
	// 2. �����ǵĶ������鸴�Ƶ�һ�����㻺���У���OpenGLʹ��
	glBindBuffer(GL_ARRAY_BUFFER, myGlsl->vbo_textureColor);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 12 * myGlsl->numVertices, myGlsl->vertices_textureColor, GL_STATIC_DRAW);
}

void ModelEditor::DeleteMesh(int faceIndex)
{
	annotation->DeleteMesh(faceIndex);
	for (size_t k = 0; k < 3; k++)
	{
		myGlsl->vertices_color[9 * (3 * faceIndex + k) + 3] = annotation->insColors[0][0];
		myGlsl->vertices_color[9 * (3 * faceIndex + k) + 4] = annotation->insColors[0][1];
		myGlsl->vertices_color[9 * (3 * faceIndex + k) + 5] = annotation->insColors[0][2];

		myGlsl->vertices_textureColor[12 * (3 * faceIndex + k) + 6] = 2.f;
		myGlsl->vertices_textureColor[12 * (3 * faceIndex + k) + 7] = 2.f;
		myGlsl->vertices_textureColor[12 * (3 * faceIndex + k) + 8] = 2.f;
	}

	// 1. �󶨶����������
	glBindVertexArray(myGlsl->vao_color);
	// 2. �����ǵĶ������鸴�Ƶ�һ�����㻺���У���OpenGLʹ��
	glBindBuffer(GL_ARRAY_BUFFER, myGlsl->vbo_color);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 9 * myGlsl->numVertices, myGlsl->vertices_color, GL_STATIC_DRAW);

	// 1. �󶨶����������
	glBindVertexArray(myGlsl->vao_textureColor);
	// 2. �����ǵĶ������鸴�Ƶ�һ�����㻺���У���OpenGLʹ��
	glBindBuffer(GL_ARRAY_BUFFER, myGlsl->vbo_textureColor);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 12 * myGlsl->numVertices, myGlsl->vertices_textureColor, GL_STATIC_DRAW);
}

////Ѱ����ƬԪ����
//void ModelEditor::FindPlantMesh(cv::Mat & textureImg, vector<int> &addedMesh,vector<int> &excludedMesh,
//	                            vector<int> &insFaces,int faceIndex, const int & referedValue)
//{
//	vector<int> &neighborFaceIndex = meshGraph->GetFaceNeighbourByEdgeList(faceIndex);
//	for (int i = 0; i < neighborFaceIndex.size(); i++)
//	{
//		vector<int>::iterator itInAddMesh = find(addedMesh.begin(),addedMesh.end(),neighborFaceIndex[i]);
//		vector<int>::iterator itInExcludedMesh = find(excludedMesh.begin(), excludedMesh.end(), neighborFaceIndex[i]);
//		vector<int>::iterator itIninsFaces = find(insFaces.begin(), insFaces.end(), neighborFaceIndex[i]);
//		if (itInAddMesh == addedMesh.end() && itInExcludedMesh == excludedMesh.end()
//			&& itIninsFaces == insFaces.end()) {
//			//δ����ע����δ���ų�
//			cv::Vec3b meanMeshRGBi = FindMeanRGBinMesh(textureImg, faceIndex);
//			cv::Vec3b meanMeshRGBj = FindMeanRGBinMesh(textureImg, neighborFaceIndex[i]);
//			double temp = sqrt(pow(meanMeshRGBi[0] - meanMeshRGBj[0], 2)
//				+ pow(meanMeshRGBi[1] - meanMeshRGBj[1], 2)
//				+ pow(meanMeshRGBi[2] - meanMeshRGBj[2], 2));
//			if (temp <= referedValue)
//			{
//				addedMesh.push_back(neighborFaceIndex[i]);
//				FindPlantMesh(textureImg, addedMesh, excludedMesh, insFaces, neighborFaceIndex[i], referedValue);
//			}
//			else
//				excludedMesh.push_back(neighborFaceIndex[i]);
//		}
//		
//	}
//}

//�õ����ֵ
double max(double a, double b, double c)
{
	double m = (a > b) ? a : b;
	m = (m > c) ? m : c;
	return m;
}
//�õ���Сֵ
double min(double a, double b, double c)
{
	double m = (a<b) ? a : b; //ȡab�е�С��
	m = (m<c) ? m : c; //��ab�е�С����c�ȣ���ȡС��
	return m; //������Сֵ
}

//�õ�һ��������Ƭ��ƽ��RGB��ֵ,ģ�¹�դ�����̣�version1.0
cv::Vec3d ModelEditor::FindMeanHSVinMesh(vector<string> &indexVector, vector<cv::Mat> & textureImgVector, int faceIndex)
{
	//����faceIndex��ȡ�������������������
	int mapdiffuse = model->materials[model->triangles[faceIndex].material].map_diffuse;
	string textureName = string(model->textures[mapdiffuse].name);
	cv::Mat textureImg;
	bool isIn = false;
	int index = 0;
	for (int i = 0; i<indexVector.size(); ++i)
	{
		if (indexVector[i] == textureName)
		{
			isIn = true;
			index = i;
			break;
		}
	}
	if (!isIn)
	{
		string texturePath = getTexturePath(string(model->pathname), textureName);
		std::cout << texturePath << endl;
		textureImg = cv::imread(texturePath);
		indexVector.push_back(textureName);
		textureImgVector.push_back(textureImg);
	}
	else
	{
		textureImg = textureImgVector[index];
	}

	Vector3i texcoordIndexs = meshGraph->triangles[faceIndex].tIndex;
	vector<Vector2d> triVertexs;
	vector<cv::Vec3i> meshRGB;
	vector<cv::Vec3d> meshHSV;
	cv::Vec3i meanMeshRGB;
	cv::Vec3d meanMeshHSV;
	int maxRows = textureImg.rows;
	int maxCols = textureImg.cols;
	//std::cout <<"maxRows:" <<maxRows<<endl;
	//std::cout <<"maxCols:" << maxCols << endl;
	for (int i = 0; i < 3; i++)
	{
		triVertexs.push_back(Vector2d(meshGraph->texcoords[texcoordIndexs[i]][0],
			meshGraph->texcoords[texcoordIndexs[i]][1]));
		//�任Ϊ�������꣬��opencv��mat�����У���������Ĺ�ϵ�����Ų�˳��һ��
		triVertexs[i][0] = triVertexs[i][0] * maxCols;
		triVertexs[i][1] = (1 - triVertexs[i][1]) * maxRows;
	}
	int boundBoxminX = int(triVertexs[0][0]);
	int boundBoxminY = int(triVertexs[0][1]);
	int boundBoxmaxX = int(triVertexs[0][0] + 1);
	int boundBoxmaxY = int(triVertexs[0][1] + 1);
	for (int i = 1; i<3; i++)
	{
		//��������εİ�Χ��
		if (triVertexs[i][0] < boundBoxminX)
			boundBoxminX = int(triVertexs[i][0]);
		else if (triVertexs[i][0] > boundBoxmaxX)
			boundBoxmaxX = int(triVertexs[i][0] + 1);

		if (triVertexs[i][1] < boundBoxminY)
			boundBoxminY = int(triVertexs[i][1]);
		else if (triVertexs[i][1] > boundBoxmaxY)
			boundBoxmaxY = int(triVertexs[i][1] + 1);
	}
	//�������õ����������еĵ�
	for (int i = boundBoxminX; i<boundBoxmaxX; i++)
	{
		for (int j = boundBoxminY; j <boundBoxmaxY; j++)
		{
			double x = i + 0.5;
			double y = j + 0.5;
			if (pointInTriangle(x, y, triVertexs))
			{
				//std::cout << "mat type:" << textureImg.type() << endl;
				//std::cout << "i:"<< i <<endl;
				//std::cout << "j:"<< j <<endl;
				//debug code��for unsolved problem 1��all black mesh in project3
				//cv::Vec3b��Ҫתint����Ȼ�����
				//std::cout <<"The block's B is " <<(int)textureImg.at<cv::Vec3b>(i, j)[0] 
				//	      << "The block's G is " << (int)textureImg.at<cv::Vec3b>(i, j)[1] 
				//	      << "The block's R is " << (int)textureImg.at<cv::Vec3b>(i, j)[2] 
				//	      << endl;
				//Vec3b demo = Vec3b((int)textureImg.at<cv::Vec3b>(i, j)[0],
				//	(int)textureImg.at<cv::Vec3b>(i, j)[1],
				//	(int)textureImg.at<cv::Vec3b>(i, j)[2]);
				//unsigned char demo1 = textureImg.at<cv::Vec3b>(i, j)[0];
				//unsigned char demo2 = textureImg.at<cv::Vec3b>(i, j)[1];
				//unsigned char demo3 = textureImg.at<cv::Vec3b>(i, j)[2];
				meshRGB.push_back(cv::Vec3i((int)textureImg.at<cv::Vec3b>(j, i)[0],
					(int)textureImg.at<cv::Vec3b>(j, i)[1],
					(int)textureImg.at<cv::Vec3b>(j, i)[2]));
			}
		}
	}

	if (meshRGB.size() == 0)//�����������̫�����޷������������ĵ�
	{
		for (int i = 0; i < 3; i++) {
			meshRGB.push_back(cv::Vec3i((int)textureImg.at<cv::Vec3b>((int)triVertexs[i][1], (int)triVertexs[i][0])[0],
				(int)textureImg.at<cv::Vec3b>((int)triVertexs[i][1], (int)triVertexs[i][0])[1],
				(int)textureImg.at<cv::Vec3b>((int)triVertexs[i][1], (int)triVertexs[i][0])[2]));
		}
	}
	//��ƽ��
	for (int i = 0; i < meshRGB.size(); i++)
	{
		meanMeshRGB[0] += meshRGB[i][0];
		meanMeshRGB[1] += meshRGB[i][1];
		meanMeshRGB[2] += meshRGB[i][2];
	}
	for (int i = 0; i < 3; i++)
		meanMeshRGB[i] = meanMeshRGB[i] / meshRGB.size();

	//�õ���Ƭ��HSV��Ϣ
	for (int i = 0; i < meshRGB.size(); i++)
	{
		double b = (double)meshRGB[i][0] / 255.0;
		double g = (double)meshRGB[i][1] / 255.0;
		double r = (double)meshRGB[i][2] / 255.0;
		double Cmax = max(b, g, r);
		double Cmin = min(b, g, r);
		double delta = Cmax - Cmin;
		double h, s, v;
		//get Hue
		if (delta == 0)
			h = 0;
		else if (Cmax == r && g >= b)
			h = 60 * ((g - b) / delta);
		else if (Cmax == r && g < b)
			h = 60 * ((g - b) / delta) + 360;
		else if (Cmax == g)
			h = 60 * ((b - r) / delta) + 120;
		else if (Cmax == b)
			h = 60 * ((r - g) / delta) + 240;
		//get saturation
		if (Cmax == 0)
			s = 0;
		else
			s = delta / Cmax;
		//get Value
		v = Cmax;

		//output for test
		//std::cout << "the Hue is: " << h << " the saturation is: "<<s<<" the value is:"<<v<<endl;
		meshHSV.push_back(Vec3d(h, s, v));
	}

	for (int i = 0; i < meshHSV.size(); i++)
	{
		meanMeshHSV[0] += meshHSV[i][0];
		meanMeshHSV[1] += meshHSV[i][1];
		meanMeshHSV[2] += meshHSV[i][2];
	}
	for (int i = 0; i < 3; i++)
		meanMeshHSV[i] = meanMeshHSV[i] / meshHSV.size();
	//std::cout << "-----------------------------------" << endl;
	//std::cout << "the mean hue is:" << meanMeshHSV[0] << endl;
	//std::cout << "the mean saturation is:" << meanMeshHSV[1] << endl;
	//std::cout << "the mean value is:" << meanMeshHSV[2] << endl;
	//ֻ��mean hue�����ж�
	//return meanMeshRGB;
	return meanMeshHSV;
}

//�жϵ��Ƿ����������У���������
double ModelEditor::sign(double x, double y, Vector2d triVertex1, Vector2d triVertex2)
{
	return (x - triVertex2[0])*(triVertex1[1] - triVertex2[1])
		- (triVertex1[0] - triVertex2[0])*(y - triVertex2[1]);
}
//�жϵ��Ƿ����������У��ж�
bool ModelEditor::pointInTriangle(double x, double y, vector<Vector2d> triVertexs)
{
	double d1, d2, d3;
	bool has_neg, has_pos;
	d1 = sign(x, y, triVertexs[0], triVertexs[1]);
	d2 = sign(x, y, triVertexs[1], triVertexs[2]);
	d3 = sign(x, y, triVertexs[2], triVertexs[0]);

	has_neg = (d1 < 0) || (d2 < 0) || (d3 < 0);
	has_pos = (d1 > 0) || (d2 > 0) || (d3 > 0);

	return !(has_neg && has_pos);
}
//�õ������ͼƬ�ļ���Ϣ
string ModelEditor::getTexturePath(const string& modelPath, const string& textureName)
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
	while ((pos = str1.find(sub_str)) != string::npos)   //�滻����ָ���Ӵ�
	{
		str1.replace(pos, sub_str.length(), str2);
	}
	return str1;
}

void ModelEditor::DeleteMesh(vector<int> faceIndices)
{
	for (size_t fi = 0; fi < faceIndices.size(); fi++)
	{
		int faceIndex = faceIndices[fi];
		if (faceIndex > 0)
		{
			annotation->DeleteMesh(faceIndex);
			for (size_t k = 0; k < 3; k++)
			{
				myGlsl->vertices_color[9 * (3 * faceIndex + k) + 3] = annotation->insColors[0][0];
				myGlsl->vertices_color[9 * (3 * faceIndex + k) + 4] = annotation->insColors[0][1];
				myGlsl->vertices_color[9 * (3 * faceIndex + k) + 5] = annotation->insColors[0][2];

				myGlsl->vertices_textureColor[12 * (3 * faceIndex + k) + 6] = 2.f;
				myGlsl->vertices_textureColor[12 * (3 * faceIndex + k) + 7] = 2.f;
				myGlsl->vertices_textureColor[12 * (3 * faceIndex + k) + 8] = 2.f;
			}
		}
	}

	// 1. �󶨶����������
	glBindVertexArray(myGlsl->vao_color);
	// 2. �����ǵĶ������鸴�Ƶ�һ�����㻺���У���OpenGLʹ��
	glBindBuffer(GL_ARRAY_BUFFER, myGlsl->vbo_color);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 9 * myGlsl->numVertices, myGlsl->vertices_color, GL_STATIC_DRAW);

	// 1. �󶨶����������
	glBindVertexArray(myGlsl->vao_textureColor);
	// 2. �����ǵĶ������鸴�Ƶ�һ�����㻺���У���OpenGLʹ��
	glBindBuffer(GL_ARRAY_BUFFER, myGlsl->vbo_textureColor);
	glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 12 * myGlsl->numVertices, myGlsl->vertices_textureColor, GL_STATIC_DRAW);
}


//����labelInfo����GLSL��Ϣ

//����ʵ��, cateId: ���Id;  ����������ʵ��Id
int ModelEditor::AddNewInstance(int cateId)
{
	int insId = this->labelInfo->AddNewInstance(cateId);
	this->selIns = Vector2i(cateId, insId);

	return insId;
}

//�������cateName���������; �������������Id
int ModelEditor::AddNewCategory(const string& cateName)
{
	int cateId = this->labelInfo->AddNewCategory(cateName);
	return cateId;
}

//ɾ��ʵ��
bool ModelEditor::DeleteInstance(int cateId, int insId)
{
	if (cateId < 0 || insId < 0)
	{
		return false;
	}

	//����GLSL����
	bool isOk = false;
	for (size_t ci = 0; ci < labelInfo->categories.size(); ci++)
	{
		if (cateId == labelInfo->categories[ci].categoryId)
		{
			int insSize = labelInfo->categories[ci].instances.size();
			for (size_t ii = 0; ii < insSize; ii++)
			{
				if (insId == labelInfo->categories[ci].instances[ii].instanceId)
				{
					vector<int>& insFaces = labelInfo->categories[ci].instances[ii].insFaces;
					Vector3f &color = labelInfo->backGround.insColor;
					myGlsl->SetVColor(insFaces, color, true);
					isOk = true;
					break;
				}
			}
		}
		if (isOk)
		{
			break;
		}
	}

	return labelInfo->DeleteInstance(cateId, insId);
}

//ɾ�����
bool ModelEditor::DeleteCategory(int cateId)
{
	if (cateId < 0)
	{
		return false;
	}

	//����GLSL����
	vector<vector<int>> faces;
	for (size_t ci = 0; ci < labelInfo->categories.size(); ci++)
	{
		if (cateId == labelInfo->categories[ci].categoryId)
		{
			int insSize = labelInfo->categories[ci].instances.size();
			for (size_t ii = 0; ii < insSize; ii++)
			{
				faces.push_back(labelInfo->categories[ci].instances[ii].insFaces);
			}
		}
	}
	Vector3f &color = labelInfo->backGround.insColor;
	myGlsl->SetVColor(faces, color, true);

	return labelInfo->DeleteCategory(cateId);
}

////��ӵ���ʵ����Ƭ
//bool ModelEditor::AddMesh_labelInfo(int faceIndex, int cateId, int insId)
//{
//	LabelInfo::Instance &ins = labelInfo->SearchInstance(cateId, insId);
//	if (ins.instanceId == -2)
//	{
//		return false;
//	}
//
//	bool isOk = labelInfo->AddMesh(faceIndex, cateId, insId, this->changeLabeledFace);
//	if (isOk)
//	{
//		Vector3f insColor = ins.insColor;
//		myGlsl->SetVColor(faceIndex, insColor);
//		return true;
//	}
//	else
//	{
//		return false;
//	}
//}

//���ʵ����Ƭ����
bool ModelEditor::AddMesh_labelInfo(vector<int> faceIndices, int cateId, int insId)
{
	LabelInfo::Instance &ins = labelInfo->SearchInstance(cateId, insId);
	if (ins.instanceId == -2)
	{
		return false;
	}

	vector<int> faceIndices_select;
	if (!this->changeLabeledFace)
	{
		faceIndices_select = labelInfo->SelectBackFaces(faceIndices);
	}
	else
	{
		faceIndices_select = faceIndices;
	}

	bool isOk = labelInfo->AddMesh(faceIndices_select, cateId, insId);

	//if (isOk)
	//{
	//	Vector3f insColor = ins.insColor;
	//	myGlsl->SetVColor(faceIndices, insColor);
	//	return true;
	//}
	//else
	//{
	//	return false;
	//}
	Vector3f insColor = ins.insColor;
	myGlsl->SetVColor(faceIndices_select, insColor);
	return true;
}

////ɾ������ʵ����Ƭ
//bool ModelEditor::DeleteMesh_labelInfo(int faceIndex)
//{
//	bool isOk = labelInfo->DeleteMesh(faceIndex, this->selIns[0], this->selIns[1], this->changeLabeledFace);
//	if (isOk)
//	{
//		Vector3f insColor = labelInfo->backGround.insColor;
//		myGlsl->SetVColor(faceIndex, insColor, true);
//		return true;
//	}
//	else
//	{
//		return false;
//	}
//}

//ɾ��ʵ����Ƭ����
bool ModelEditor::DeleteMesh_labelInfo(vector<int> faceIndices)
{
	vector<int> faceIndices_select;
	if (!this->changeLabeledFace)
	{
		faceIndices_select = labelInfo->SelectFaces(faceIndices, this->selIns[0], this->selIns[1]);
	}
	else
	{
		faceIndices_select = faceIndices;
	}

	bool isOk = labelInfo->DeleteMesh(faceIndices_select);
	//if (isOk)
	//{
	//	Vector3f insColor = labelInfo->backGround.insColor;
	//	myGlsl->SetVColor(faceIndices, insColor, true);
	//	return true;
	//}
	//else
	//{
	//	return false;
	//}
	Vector3f insColor = labelInfo->backGround.insColor;
	myGlsl->SetVColor(faceIndices_select, insColor, true);
	return true;
}

//����ʵ���ϲ�������Ƭ
bool ModelEditor::MergeBackForSingleIns(int cateId, int insId)
{
	if (cateId < 0 || insId < 0)
	{
		return false;
	}

	LabelInfo::Instance &ins = labelInfo->SearchInstance(cateId, insId);
	if (ins.instanceId == -2)
	{
		return false;
	}

	if (meshGraph == nullptr)
	{
		meshGraph = new MeshGraph();
		meshGraph->CreateGraph(model, true, true);
	}

	const vector<Vector2i> &segResult = labelInfo->segInfo;

	//�Ա�����Ƭ����ͨ�Ծ���
	long t1 = time(NULL);
	int numFaces = segResult.size();
	std::cout << "��ʼ�Ա�����Ƭ����......" << endl;
	vector<int> &backFaces = labelInfo->backGround.insFaces;
	vector<bool> isFVisited(numFaces);	//��¼ÿ����Ƭ�Ƿ񱻷��ʹ�
	for (int i = 0; i < numFaces; i++)
	{
		isFVisited[i] = false;
	}
	vector<vector<int>> fCluResult;	//��Ƭ������
	queue<int> fIndics;	//����������Ƭ����
	int backFaceCount = backFaces.size();
	for (int fi = 0; fi < backFaceCount; fi++)
	{
		if (!isFVisited[backFaces[fi]])
		{
			fIndics.push(backFaces[fi]);
			vector<int> facesThisClass;	//��һ���������Ƭ
			facesThisClass.push_back(backFaces[fi]);
			isFVisited[backFaces[fi]] = true;
			while (!fIndics.empty())
			{
				int faceIndex = fIndics.front();
				//ȡ���뵱ǰface�ڽӵ���Ƭ�б�
				//vector<int> &adjFaces = meshGraph->adjFacesPerFace[faceIndex];
				const vector<int> &adjFaces = meshGraph->adjFacesPerFace_Edge[faceIndex];
				for (int j = 0; j < adjFaces.size(); j++)
				{
					int fIndex = adjFaces[j];
					//�ж������Ƭ�Ƿ�Ϊ����,����δ�����ʹ�
					if (segResult[fIndex] == Vector2i(-1, -1) && !isFVisited[fIndex])
					{
						fIndics.push(fIndex);
						facesThisClass.push_back(fIndex);
						isFVisited[fIndex] = true;
					}
				}
				fIndics.pop();
			}
			fCluResult.push_back(facesThisClass);
		}
	}

	int classCount = fCluResult.size();
	std::cout << "�������: " << round(time(NULL) - t1) << " seconds" << endl;
	std::cout << "��: " << classCount << " ����ͨ����" << endl;
	//�ж��Ƿ񱻽���ʵ����Ƭ��Χ
	SortFSegVec(fCluResult, 1);
	std::cout << "��ʼ�ϲ�����Χ������......" << endl;
	for (int i = 1; i < classCount; i++)
	{
		//std::cout << "�ϲ��� " << i << " �鱳����Ƭ��: " << fCluResult[i].size() << endl;
		vector<int> &patchFaces = fCluResult[i];	//��ͨ������Ƭ
		int adjBuildIndex = -1;
		//ȡ�뱻��Χ���ݶ������ڽ������ݶ���Ƭ����Ϊ���ݶ������
		vector<int> maxCountV;
		bool ifMerge = true;	//�Ƿ�ϲ�
		bool ifOnlyBack = true;	//�ڽ���Ƭ�Ƿ�ֻ�б���
		for (int j = 0; j < patchFaces.size(); j++)
		{
			int faceIndex = patchFaces[j];
			//ȡ���뵱ǰface�ڽӵ���Ƭ�б�
			const vector<int> &adjFaces = meshGraph->adjFacesPerFace_Edge[faceIndex];
			//vector<int> &adjFaces = meshGraph->adjFacesPerFace[faceIndex];
			for (int k = 0; k < adjFaces.size(); k++)
			{
				if (segResult[adjFaces[k]][0] >= 0 || segResult[adjFaces[k]][1] >= 0)
				{
					ifOnlyBack = false;
					if (segResult[adjFaces[k]][0] != cateId || segResult[adjFaces[k]][1] != insId)
					{
						ifMerge = false;
						break;
					}
				}
			}
		}

		if (ifMerge && (!ifOnlyBack))
		{
			labelInfo->AddMesh(patchFaces, cateId, insId);
			//����glsl
			Vector3f insColor = ins.insColor;
			myGlsl->SetVColor(patchFaces, insColor);
		}
	}
	std::cout << "�ϲ���ɣ�" << round(time(NULL) - t1) << " s" << endl;
	return true;
}

//����ʵ��ȥ������
bool ModelEditor::ClearNoiseForSingleIns(int cateId, int insId)
{
	if (cateId < 0 || insId < 0)
	{
		return false;
	}

	LabelInfo::Instance &ins = labelInfo->SearchInstance(cateId, insId);
	if (ins.instanceId == -2)
	{
		return false;
	}

	if (meshGraph == nullptr)
	{
		meshGraph = new MeshGraph();
		meshGraph->CreateGraph(model, true, true);
	}

	const vector<Vector2i> &segResult = labelInfo->segInfo;

	//��ʵ����Ƭ����ͨ�Ծ���
	long t1 = time(NULL);
	std::cout << "��ʼ��ʵ����Ƭ����......" << endl;
	int numFaces = segResult.size();
	vector<int> &insFaces = ins.insFaces;
	vector<bool> isFVisited(numFaces);	//��¼ÿ����Ƭ�Ƿ񱻷��ʹ�
	for (int i = 0; i < numFaces; i++)
	{
		isFVisited[i] = false;
	}
	vector<vector<int>> fCluResult;	//��Ƭ������
	queue<int> fIndics;	//����������Ƭ����
	int insFaceCount = insFaces.size();
	for (int fi = 0; fi < insFaceCount; fi++)
	{
		if (!isFVisited[insFaces[fi]])
		{
			fIndics.push(insFaces[fi]);
			vector<int> facesThisClass;	//��һ���������Ƭ
			facesThisClass.push_back(insFaces[fi]);
			isFVisited[insFaces[fi]] = true;
			while (!fIndics.empty())
			{
				int faceIndex = fIndics.front();
				//ȡ���뵱ǰface�ڽӵ���Ƭ�б�
				//vector<int> &adjFaces = meshGraph->adjFacesPerFace[faceIndex];
				const vector<int> &adjFaces = meshGraph->adjFacesPerFace_Edge[faceIndex];
				for (int j = 0; j < adjFaces.size(); j++)
				{
					int fIndex = adjFaces[j];
					//�ж������Ƭ�Ƿ�Ϊ����,����δ�����ʹ�
					if (segResult[fIndex] == Vector2i(cateId, insId) && !isFVisited[fIndex])
					{
						fIndics.push(fIndex);
						facesThisClass.push_back(fIndex);
						isFVisited[fIndex] = true;
					}
				}
				fIndics.pop();
			}
			fCluResult.push_back(facesThisClass);
		}
	}

	int classCount = fCluResult.size();
	std::cout << "�������: " << round(time(NULL) - t1) << " seconds" << endl;
	std::cout << "��: " << classCount << " ����ͨ����" << endl;
	//�ж��Ƿ񱻽���ʵ����Ƭ��Χ
	SortFSegVec(fCluResult, 1);

	Vector3f backColor = labelInfo->backGround.insColor;
	if (classCount > 0)
	{
		for (size_t i = 1; i < classCount; i++)
		{
			labelInfo->DeleteMesh(fCluResult[i]);
			//����glsl
			myGlsl->SetVColor(fCluResult[i], backColor);
		}
		return true;
	}
	else
	{
		return false;
	}
}

//����GraphCut�����ʼ��������͵���ʵ����segResultΪmeshGraph������Ƭ��ʼ�������-1Ϊ���������ڣ�0Ϊ���ָ1λǰ���ݶ���2Ϊ����ȱ�����
vector<int> ExpandSingInsWithGraphInit(const MeshGraph *meshGraph, const vector<int> &insFaces, vector<int> &segResult, double expandAngle)
{
	vector<int> expandFaces;
	int fSize = meshGraph->triangles.size();
	vector<bool> isFVisited(meshGraph->triangles.size());		//��¼ÿ����Ƭ�Ƿ񱻷���
	for (int i = 0; i < fSize; i++)
	{
		isFVisited[i] = false;
	}
	vector<int> insExpandResult;	//��¼��ǰʵ�����ͽ��
	queue<int> fIndics;	//����������Ƭ����
	for (int i = 0; i < insFaces.size(); i++)
	{
		if (!isFVisited[insFaces[i]])
		{
			insExpandResult.push_back(insFaces[i]);
			fIndics.push(insFaces[i]);
			isFVisited[insFaces[i]] = true;
			while (!fIndics.empty())
			{
				int faceIndex = fIndics.front();
				Vector3d normal1 = meshGraph->triangles[faceIndex].normal;
				normal1.normalize();
				//ȡ���뵱ǰface�ڽӵ���Ƭ�б�
				//vector<int> &adjFaces = meshGraph->adjFacesPerFace[faceIndex]; 
				const vector<int> &adjFaces = meshGraph->adjFacesPerFace_Edge[faceIndex];

				for (int j = 0; j < adjFaces.size(); j++)
				{
					int adjFIndex = adjFaces[j];
					//�ж������Ƭ�Ƿ�δ�����ʹ�
					if (!isFVisited[adjFIndex])
					{
						/*
						�ж������Ƭ: -1:���ڷ�Χ��; 0�����ָ���Ƭ; 1��ǰ���ݶ�; 2������������ȣ�
						-1���Ǵ���Ƭ�ѱ���
						0�����ݴ�������Ƭ����������Ƭ֮��Ƕ��ж��Ƿ��Ϊһ�࣬������ִ��1,���޸�facesSegResult��Ӧֵ ����ʲô������
						1������Ƭ�����ʵ������Ƭ���� ���� �����������Ƭ����
						2���Ǵ���Ƭ�ѱ���
						*/
						if (segResult[adjFIndex] == -1)
						{
							isFVisited[adjFIndex] = true;
						}
						else if (segResult[adjFIndex] == 0)
						{
							bool ifParallel = false;	//�Ƿ�ƽ��,���ж�����������Ƭ֮��Ƕ�
							Vector3d normal2 = meshGraph->triangles[adjFIndex].normal;
							normal2.normalize();
							if (abs(normal1.dot(normal2)) > expandAngle)
							{
								ifParallel = true;
							}

							//if ((!meshGraph->triangles[adjFIndex].ifVertical) && ifParallel)
							if ((meshGraph->triangles[adjFIndex].ifHorizontal) && ifParallel)
							{
								//std::cout << "�б�����Ƭ������" << endl;
								insExpandResult.push_back(adjFIndex);
								fIndics.push(adjFIndex);
								segResult[adjFIndex] = 1;
								isFVisited[adjFIndex] = true;
							}
						}
						else if (segResult[adjFIndex] == 1)
						{
							insExpandResult.push_back(adjFIndex);
							fIndics.push(adjFIndex);
							isFVisited[adjFIndex] = true;
						}
						else
						{
							isFVisited[adjFIndex] = true;
						}
					}
				}
				fIndics.pop();
			}
		}
	}
	return insExpandResult;
}

//��������ʵ��ͼ��
bool ModelEditor::GraphCutForSingleIns(int cateId, int insId)
{
	float boxExpandSize = 4.0;
	double expandAngle = cos(30.0*PI / 180.0);

	const vector<Vector2i> &segInfo = labelInfo->segInfo;
	LabelInfo::Instance &ins = labelInfo->SearchInstance(cateId, insId);
	if (ins.instanceId == -2)
	{
		return false;
	}
	long t1 = time(NULL);
	if (meshGraph == nullptr)
	{
		meshGraph = new MeshGraph();
		meshGraph->CreateGraph(model_origin, true, true);
	}
	std::cout << "Create model graph used: " << std::round(time(NULL) - t1) << " seconds" << endl;
	int allFSize = meshGraph->triangles.size();	//�󳡾�������Ƭ��Ŀ

	const vector<int> &insFaces = ins.insFaces;
	int insFCount = insFaces.size();
	if (insFCount <= 0)
	{
		return true;
	}

	//�����ݶ��߶�
	double roofHeight = 0;
	vector<double> vHeights(3 * insFCount);
	for (size_t fi = 0; fi < insFCount; fi++)
	{
		vHeights[3 * fi + 0] = meshGraph->vertices[meshGraph->triangles[insFaces[fi]].vIndex[0]].pos[2];
		vHeights[3 * fi + 1] = meshGraph->vertices[meshGraph->triangles[insFaces[fi]].vIndex[1]].pos[2];
		vHeights[3 * fi + 2] = meshGraph->vertices[meshGraph->triangles[insFaces[fi]].vIndex[2]].pos[2];
	}
	roofHeight = *(max_element(vHeights.begin(), vHeights.end()));

	//��������ȡ�ݶ���Χ������4�׺�ķ�Χ����Ƭ����ͼ��
	CBoundingBox rectBox = meshGraph->GetCBBoxByFIndices(insFaces);
	rectBox.ExpandBox(BoxOutEdge, boxExpandSize);
	//vector<int> roofFaces_expand = meshGraph->GetFIndicesInBBox(rectBox, roofSegResult);
	vector<int> insFaces_expand = meshGraph->GetFIndicesInBBox(rectBox);

	/*��¼�󳡾���Ƭ��Ե�ǰ�ݶ���ʼ�����ࣺ
	- 1��ʾ�������ݶ���Χ��(�����)��Χ��
	0��ʾ���ָ���Ƭ��
	1��ʾ�ݶ���Ƭ��
	2��ʾ������Ƭ��
	*/
	int insFaces_expand_size = insFaces_expand.size();
	vector<int> faceInit(allFSize);
	for (size_t fi = 0; fi < allFSize; fi++)
	{
		faceInit[fi] = -1;
	}
	for (size_t fi = 0; fi < insFaces_expand_size; fi++)
	{
		faceInit[insFaces_expand[fi]] = 0;
	}
	for (size_t fi = 0; fi < insFCount; fi++)
	{
		faceInit[insFaces[fi]] = 1;
	}

	//���Ĳ�����ȡ������ƬΪ����
	vector<int> initNodes_Ground = graphCut::GetInitBackFace_I("Ground", meshGraph, faceInit, roofHeight);

	//���岽�������ݶ���Ƭ
	vector<int> insFace_Expand = ExpandSingInsWithGraphInit(meshGraph, insFaces, initNodes_Ground, expandAngle);
	std::cout << "����ǰ��Ƭ���� " << insFaces.size() << endl;
	std::cout << "���ͺ���Ƭ���� " << insFace_Expand.size() << endl;

	//���������������ͺ��ݶ������
	std::cout << "�����ݶ�����η�Χ." << endl;
	vector<int> boundaryNodes(allFSize);
	for (size_t fi = 0; fi < allFSize; fi++)
	{
		boundaryNodes[fi] = -1;
	}
	int roofFCount_expand = insFace_Expand.size();
	for (size_t rfi = 0; rfi < roofFCount_expand; rfi++)
	{
		boundaryNodes[insFace_Expand[rfi]] = 1;
	}
	vector<Vector3d> insBoundary = meshGraph->GetBuildRegionByRoof(boundaryNodes);//��Χ�����
	for (size_t rfi = 0; rfi < roofFCount_expand; rfi++)
	{
		initNodes_Ground[insFace_Expand[rfi]] = 1;
	}

	//���߲��������ݶ�����η�Χ�Դ��ָ���Ƭ���и�Ȩ�ز�ͼ��
	vector<int> graphCutResult = graphCut::SceneSModelGraphCut(meshGraph, initNodes_Ground, insBoundary, roofHeight);
	std::cout << "ͼ��������" << endl;

	vector<int> addFace_gf;	//��¼ͼ�����������Ƭ
							//��GraphCut���ӳ���ȫ����Ƭ�ָ���roofSegResult
	for (size_t afi = 0; afi < allFSize; afi++)
	{
		if (graphCutResult[afi] == 1) //���ͼ����Ϊǰ����������
		{
			//�������Ƭ�ָ�ǰΪ�������򽫸ñ�����Ƭ�Ĺ�����������(ri-1)���뵽face_RoofIndexList
			if (segInfo[afi][0] == -1)
			{
				addFace_gf.push_back(afi);
			}
		}
	}
	std::cout << "�ָ���ɣ�" << round(time(NULL) - t1) << " s" << endl << endl;

	//����ͼ��������LabelInfo
	return AddMesh_labelInfo(addFace_gf, cateId, insId);
}

//�������LookAtĿ��� PivotPoint
bool ModelEditor::SetCameraPivotPoint(PivotPointType ppt)
{
	pivotPointType = ppt;
	if (this->myGlsl == nullptr)
	{
		return false;
	}
	if (this->myGlsl->camera_ == nullptr)
	{
		return false;
	}
	if (this->model == nullptr)
	{
		this->myGlsl->camera_->setPivotPoint(easy3d::vec3(0, 0, 0));
		this->myGlsl->camera_->frame()->setZoomsOnPivotPoint(true);
		return false;
	}

	//���������ģ������
	if (ppt == PP_ModelCenter)
	{
		this->myGlsl->camera_->setPivotPoint(easy3d::vec3(0, 0, 0));
		this->myGlsl->camera_->frame()->setZoomsOnPivotPoint(true);
		return true;
	}
	//��������Ƶ�ǰѡ��ʵ��������
	else if (ppt == PP_InsCenter)
	{
		if (this->selIns[0] >= 0 && this->selIns[1] >= 0)
		{
			easy3d::vec3 pivotPoint(0, 0, 0);
			LabelInfo::Instance& ins = labelInfo->SearchInstance(selIns[0], selIns[1]);
			if (ins.instanceId != -2 && ins.insBBox.midMinMaxPoint.size()>0)
			{
				pivotPoint[0] = ins.insBBox.midMinMaxPoint[0][0];
				pivotPoint[1] = ins.insBBox.midMinMaxPoint[0][1];
				pivotPoint[2] = ins.insBBox.midMinMaxPoint[0][2];
				this->myGlsl->camera_->setPivotPoint(pivotPoint);
				this->myGlsl->camera_->frame()->setZoomsOnPivotPoint(true);
				return true;
			}
			else
			{
				this->myGlsl->camera_->setPivotPoint(easy3d::vec3(0, 0, 0));
				this->myGlsl->camera_->frame()->setZoomsOnPivotPoint(true);
				return false;
			}
		}
		else
		{
			this->myGlsl->camera_->setPivotPoint(easy3d::vec3(0, 0, 0));
			this->myGlsl->camera_->frame()->setZoomsOnPivotPoint(true);
			return false;
		}
	}
	else if (ppt == PP_ScreenCenter)
	{
		Vector2i point(0, 0);
		point[0] = myGlsl->scr_width / 2;
		point[1] = myGlsl->scr_height / 2;
		int faceIndex = myGlsl->PickFace(point[0], point[1]);
		if (faceIndex < 0)
		{
			this->myGlsl->camera_->setPivotPoint(easy3d::vec3(0, 0, 0));
			this->myGlsl->camera_->frame()->setZoomsOnPivotPoint(true);
			return false;
		}

		easy3d::vec3 faceMidPos(0, 0, 0);
		for (size_t j = 0; j < 3; j++)
		{
			for (size_t k = 0; k < 3; k++)
			{
				faceMidPos[j] = model->vertices[3 * model->triangles[faceIndex].vindices[k] + j];
			}
		}
		for (size_t i = 0; i < 3; i++)
		{
			faceMidPos[i] = faceMidPos[i] / 3.0;
		}
		this->myGlsl->camera_->setPivotPoint(faceMidPos);
		this->myGlsl->camera_->frame()->setZoomsOnPivotPoint(false);
		return true;
	}
}

bool compareMesh(Vec2d a, Vec2d b) { return (a[1]<b[1]); }
//��ȡֲ����Ƭ
bool ModelEditor::PickPlantFaces(int cateId, int insId)
{
	//debug:vec3b�����������⣬����ģ�Ͷ�ȡ����ͼƬ���⣬�ж�������ͼƬ�����
	const vector<Vector2i> &segInfo = labelInfo->segInfo;	//��¼ģ��ÿ����Ƭ�ķָ���Ϣ�� segInfo[0]��¼����Ƭ�������segInfo[1]��¼����Ƭ��������ʵ�����
	LabelInfo::Instance &ins = labelInfo->SearchInstance(cateId, insId);
	if (ins.instanceId == -2)
	{
		return false;
	}
	long t1 = time(NULL);
	if (meshGraph == nullptr)
	{
		meshGraph = new MeshGraph();
		meshGraph->CreateGraph(model_origin, true, true);
	}
	std::cout << "Create model graph used: " << std::round(time(NULL) - t1) << " seconds" << endl;

	const vector<int> &insFaces = ins.insFaces;	//��ʵ����������Ƭ����
	int insFCount = insFaces.size();
	//��ȡǰ����֤�Ƿ���һ����������Ƭ�Ѿ������

	if (insFCount <= 10)
	{
		std::cout << "the refered mesh is not enough" << endl;
		return true;
	}
	else
	{
		vector<int> excludedMesh;//��¼������Ҫ���mesh�±�
		vector<int> testedMesh;//��¼�������mesh�±������ھ��±�
		vector<int> addedMesh;//��¼�������mesh�±������ھ��±�
		double referedValue = 0.0;//hue�ο�ֵ

		vector<cv::Mat> textureImgVector;
		vector<string> indexVector;

		//����ԭʼ�ı�עmesh�õ���һ����������Ƭ
		for (int i = 0; i < insFCount; i++) {
			vector<int> neighborFaceIndex = meshGraph->GetFaceNeighbourByEdgeList(insFaces[i]);
			addedMesh.push_back(insFaces[i]);
			for (int j = 0; j < neighborFaceIndex.size(); j++)
			{
				testedMesh.push_back(neighborFaceIndex[j]);
			}
		}
		int index = 0;
		while (testedMesh.size() > 0) {
			cv::Vec3d meanHSV = FindMeanHSVinMesh(indexVector, textureImgVector, testedMesh[index]);
			if (meanHSV[0] >= 70 && meanHSV[0] <= 154)
			{
				addedMesh.push_back(testedMesh[index]);
				//�����ھ�
				vector<int> neighborFaceIndex = meshGraph->GetFaceNeighbourByEdgeList(testedMesh[index]);
				for (int j = 0; j < neighborFaceIndex.size(); j++)
				{
					testedMesh.push_back(neighborFaceIndex[j]);
				}
				//�ų�testedMesh���Ѿ�Ҫ�������
				for (vector<int>::iterator it = testedMesh.begin(); it != testedMesh.end(); ++it)
				{
					for (int j = 0; j < addedMesh.size(); j++) {
						//std::cout << "in 1" << endl;
						if ((*it) == addedMesh[j])
						{
							//std::cout << "get out added" << endl;
							it = testedMesh.erase(it);
							--it;//�������һ��
							break;
						}
					}
				}
			}
			else
			{
				excludedMesh.push_back(testedMesh[index]);
				//�ų�testedMesh�в����ϵ�
				for (vector<int>::iterator it = testedMesh.begin(); it != testedMesh.end(); ++it)
				{
					for (int j = 0; j < excludedMesh.size(); j++) {
						if ((*it) == excludedMesh[j])
						{
							//std::cout << "get out excluded" << endl;
							it = testedMesh.erase(it);

							--it;//�������һ��
							break;
						}
					}
				}
			}
			std::cout << addedMesh.size() << endl;
		}
		addedMesh.erase(addedMesh.begin(), addedMesh.begin() + insFCount);
		std::cout << "final addedMesh:" << addedMesh.size() << endl;
		return AddMesh_labelInfo(addedMesh, cateId, insId);
	}

}

//��ȡ��·��Ƭ
bool ModelEditor::PickRoadFaces(int cateId, int insId)
{
	const vector<Vector2i> &segInfo = labelInfo->segInfo;	//��¼ģ��ÿ����Ƭ�ķָ���Ϣ�� segInfo[0]��¼����Ƭ�������segInfo[1]��¼����Ƭ��������ʵ�����
	LabelInfo::Instance &ins = labelInfo->SearchInstance(cateId, insId);
	if (ins.instanceId == -2)
	{
		return false;
	}
	long t1 = time(NULL);
	if (meshGraph == nullptr)
	{
		meshGraph = new MeshGraph();
		meshGraph->CreateGraph(model_origin, true, true);
	}
	std::cout << "Create model graph used: " << std::round(time(NULL) - t1) << " seconds" << endl;

	const vector<int> &insFaces = ins.insFaces;	//��ʵ����������Ƭ����
	int insFCount = insFaces.size();
	if (insFCount <= 0)
	{
		return true;
	}
}
