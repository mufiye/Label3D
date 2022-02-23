
// Label3DView.cpp : CLabel3DView 类的实现
//

#include "stdafx.h"
// SHARED_HANDLERS 可以在实现预览、缩略图和搜索筛选器句柄的
// ATL 项目中进行定义，并允许与该项目共享文档代码。
#ifndef SHARED_HANDLERS
#include "Label3D.h"
#endif

#include "Label3DDoc.h"
#include "Label3DView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CLabel3DView

IMPLEMENT_DYNCREATE(CLabel3DView, CView)

BEGIN_MESSAGE_MAP(CLabel3DView, CView)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_LBUTTONDOWN()
	ON_WM_MOUSEMOVE()
	ON_WM_MOUSEWHEEL()
	ON_WM_SIZE()
	ON_WM_RBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_RBUTTONUP()
END_MESSAGE_MAP()

// CLabel3DView 构造/析构

CLabel3DView::CLabel3DView()
{
	// TODO: 在此处添加构造代码
	m_hRC = NULL;
	m_pDC = NULL;
	m_width = 0;
	m_height = 0;

	//m_pivotPoint = easy3d::vec3(0, 0, 0);
}

CLabel3DView::~CLabel3DView()
{
}

BOOL CLabel3DView::PreCreateWindow(CREATESTRUCT& cs)
{
	// TODO: 在此处通过修改
	//  CREATESTRUCT cs 来修改窗口类或样式

	return CView::PreCreateWindow(cs);
}

BOOL CLabel3DView::OnEraseBkgnd(CDC* pDC)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	return TRUE;

	//return CView::OnEraseBkgnd(pDC);
}

// CLabel3DView 绘制

void CLabel3DView::OnDraw(CDC* /*pDC*/)
{
	//CLabel3DDoc* pDoc = GetDocument();
	ASSERT_VALID(pDoc);
	if (!pDoc)
		return;

	// TODO: 在此处为本机数据添加绘制代码
	pDoc->modelEditor->RenderScene();
}


// CLabel3DView 诊断

#ifdef _DEBUG
void CLabel3DView::AssertValid() const
{
	CView::AssertValid();
}

void CLabel3DView::Dump(CDumpContext& dc) const
{
	CView::Dump(dc);
}

CLabel3DDoc* CLabel3DView::GetDocument() const // 非调试版本是内联的
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CLabel3DDoc)));
	return (CLabel3DDoc*)m_pDocument;
}
#endif //_DEBUG

//创建控制台，用于输出实验数据
bool CLabel3DView::OpenConsole()
{
	// 保证函数只执行一次
	static bool Runed = false;
	if (Runed)
		return false;
	Runed = true;

	if (!AllocConsole() || !freopen("CONOUT$", "w", stdout))
		AfxMessageBox(_T("InitConsoleWindow Failed!")); //分配控制台在重定向输出流至控制台

	std::cout << "打开控制台成功" << std::endl << std::endl;

	return true;
}

BOOL CLabel3DView::SetupPixelFormat()
{
	static  PIXELFORMATDESCRIPTOR pfd =
	{
		sizeof(PIXELFORMATDESCRIPTOR),//pfd结构的大小
		1, //版本号
		PFD_DRAW_TO_WINDOW |//支持在窗口中绘图
		PFD_SUPPORT_OPENGL |//支持OpenGL
		PFD_DOUBLEBUFFER,//双缓存模式
		PFD_TYPE_RGBA,//RGBA颜色模式
		24, //24位颜色深度
		0, 0, 0, 0, 0, 0,//忽略颜色位
		0, //没有非透明度缓存
		0, //忽略移位位
		0, //无累计缓存
		0, 0, 0, 0,  //忽略累计位
		32, //32位深度缓存
		0, //无模板缓存
		0, //无辅助缓存
		PFD_MAIN_PLANE,//主层
		0, //保留
		0, 0, 0  //忽略层，可见性和损毁掩模
	};
	int pixelFormat;
	//为设备描述表得到最匹配的像素格式
	if ((pixelFormat = ChoosePixelFormat(m_pDC->GetSafeHdc(), &pfd)) == 0)
	{
		MessageBox(L"ChoosePixelFormat failed");
		return FALSE;
	}
	//设置最匹配的像素格式为当前的像素格式
	if (SetPixelFormat(m_pDC->GetSafeHdc(), pixelFormat, &pfd) == FALSE)
	{
		MessageBox(L"SetPixelFormat failed");
		return FALSE;
	}
	return TRUE;
}

BOOL CLabel3DView::InitializeOpenGL()
{
	PIXELFORMATDESCRIPTOR pfd;
	int n;
	m_pDC = new CClientDC(this);
	ASSERT(m_pDC != NULL);
	//设置当前的绘图像素格式
	if (!SetupPixelFormat())
	{
		return FALSE;
	}
	n = ::GetPixelFormat(m_pDC->GetSafeHdc());
	::DescribePixelFormat(m_pDC->GetSafeHdc(), n, sizeof(pfd), &pfd);
	//创建绘图描述表
	m_hRC = wglCreateContext(m_pDC->GetSafeHdc());
	if (m_hRC == NULL)
	{
		return FALSE;
	}
	//使绘图描述表为当前调用线程的当前绘图描述表
	if (wglMakeCurrent(m_pDC->GetSafeHdc(), m_hRC) == FALSE)
	{
		return FALSE;
	}

	pDoc->modelEditor->InitializeOpenGL();

	return TRUE;
}

// CLabel3DView 消息处理程序

int CLabel3DView::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CView::OnCreate(lpCreateStruct) == -1)
		return -1;

	// TODO:  在此添加您专用的创建代码
	this->pDoc = GetDocument();

	pDoc->modelEditor = new ModelEditor();
	OpenConsole();	//打开控制台

	if (InitializeOpenGL())
	{
		const GLubyte* renderer = glGetString(GL_RENDERER);
		const GLubyte* vendor = glGetString(GL_VENDOR);
		const GLubyte* version = glGetString(GL_VERSION);
		const GLubyte* glslVersion = glGetString(GL_SHADING_LANGUAGE_VERSION);

		GLint major, minor;
		glGetIntegerv(GL_MAJOR_VERSION, &major);
		glGetIntegerv(GL_MINOR_VERSION, &minor);

		printf("GL Vendor : %s\n", vendor);
		printf("GL Renderer : %s\n", renderer);
		printf("GL Version(string) : %s\n", version);
		printf("GL Version(integer) : %d.%d\n", major, minor);
		printf("GLSL Version : %s\n", glslVersion);

		pDoc->modelEditor->glCamera->setModelViewMatrix();
		return 0;
	}
	return 0;
}

void CLabel3DView::OnDestroy()
{
	CView::OnDestroy();

	// TODO: 在此处添加消息处理程序代码
	m_hRC = ::wglGetCurrentContext();
	if (::wglMakeCurrent(0, 0) == FALSE)
	{
		MessageBox(L"Could not make RC non-current");
	}
	if (m_hRC)
	{
		if (::wglDeleteContext(m_hRC) == FALSE)
		{
			MessageBox(L"Could not delete RC");
		}
	}
	if (m_pDC)
		delete m_pDC;
	m_pDC = NULL;
}

void CLabel3DView::RotateX(float angle)
{
	float d = pDoc->modelEditor->glCamera->getDist();
	float slide_d = -2 * d*sin(angle*3.14159265 / 360);
	pDoc->modelEditor->glCamera->yaw(angle / 2);
	pDoc->modelEditor->glCamera->slide(slide_d, 0, 0);
	pDoc->modelEditor->glCamera->yaw(angle);
	pDoc->modelEditor->glCamera->yaw(-angle / 2);
}

void CLabel3DView::RotateY(float angle)
{
	float d = pDoc->modelEditor->glCamera->getDist();
	float slide_d = 2 * d*sin(angle*3.14159265 / 360);
	pDoc->modelEditor->glCamera->pitch(angle / 2);
	pDoc->modelEditor->glCamera->slide(0, slide_d, 0);
	pDoc->modelEditor->glCamera->pitch(angle);
	pDoc->modelEditor->glCamera->pitch(-angle / 2);
}

void CLabel3DView::OnSize(UINT nType, int cx, int cy)
{
	CView::OnSize(nType, cx, cy);

	// TODO: 在此处添加消息处理程序代码
	glViewport(0, 0, cx, cy);	// 设置视口  

	m_width = cx;
	m_height = cy;

	pDoc->modelEditor->UpdateGLSLSize(m_width, m_height);
	pDoc->modelEditor->glCamera->setShape(60.0, (GLfloat)cx / (GLfloat)cy, 0.01, 100000.0);

	if (pDoc->modelEditor->myGlsl)
	{
		pDoc->modelEditor->myGlsl->camera_->setScreenWidthAndHeight(m_width, m_height);
		//glViewport(0, 0, m_width, m_height);
		pDoc->modelEditor->myGlsl->scr_width = m_width;
		pDoc->modelEditor->myGlsl->scr_height = m_height;
		pDoc->modelEditor->RenderScene();
	}
	//else
	//{
	//	pDoc->modelEditor->InitGLSL();
	//}
}

void CLabel3DView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO:  在此添加消息处理程序代码和/或调用默认值
	lastPos = point;

#if 1
	BOOL bCtrl = GetKeyState(VK_CONTROL) & 0x8000;
	BOOL bShift = GetAsyncKeyState(VK_LSHIFT) && 0x8000;

	if (bCtrl)	//Ctrl键按下
	{
		int faceIndex = pDoc->modelEditor->myGlsl->PickFace(point.x, point.y);
	
		if (this->pDoc->modelEditor->editState == Edit_Render)
		{
			int cateId = -1;
			int insId = -1;
			if (faceIndex < 0)
			{
				pDoc->modelEditor->selInsIndex = -2;
			}
			else if (this->pDoc->modelEditor->labelInfo)
			{
				cateId = this->pDoc->modelEditor->labelInfo->segInfo[faceIndex][0];
				insId = this->pDoc->modelEditor->labelInfo->segInfo[faceIndex][1];
			}
			std::cout << "选中第 " << faceIndex << " 个面片: 属于第 " << cateId << " 类的第 " << insId << " 个实例" << endl;
			int mapdiffuse = this->pDoc->modelEditor->model->materials[this->pDoc->modelEditor->model->triangles[faceIndex].material].map_diffuse;
			std::cout << "textureId: " << mapdiffuse << endl;
			if (cateId >= 0 && insId >= 0)
			{
				this->pDoc->SeletcTreeItem(cateId, insId);
				if (this->pDoc->modelEditor->ifEdited)
				{
					this->pDoc->modelEditor->labelInfo->CalcSingleBBox(this->pDoc->modelEditor->selIns[0], this->pDoc->modelEditor->selIns[1]);
					this->pDoc->modelEditor->ifEdited = false;
				}
				this->pDoc->modelEditor->selIns = Vector2i(cateId, insId);
				//this->pDoc->modelEditor->SetGLCameraWithInsBox(insIndex, View_Down);

				if (this->pDoc->modelEditor->labelInfo->CheckCateIsthing(cateId))
				{
					this->pDoc->modelEditor->SetCameraWithInsBox(cateId, insId, View_Down);
					this->pDoc->modelEditor->SetCameraPivotPoint(PP_InsCenter);
					this->pDoc->SetRadioPivotPointType(PP_InsCenter);
				}
			}
			else
			{
				this->pDoc->modelEditor->selIns = Vector2i(cateId, insId);
			}
			pDoc->modelEditor->RenderScene();
		}
		else if (this->pDoc->modelEditor->editState == Edit_AddNewInsRec ||
			this->pDoc->modelEditor->editState == Edit_DeleteMesh_Polygon ||
			this->pDoc->modelEditor->editState == Edit_AddMesh_Polygon)
		{
			if (faceIndex > 0)
			{
				//this->pDoc->modelEditor->polygonPoints_3D.push_back(worldPos);
				pDoc->modelEditor->RenderScene();
			}
		}
		else if (this->pDoc->modelEditor->editState == Edit_Edit)
		{
			vector<int> faceIndices;
			if (pDoc->modelEditor->pickType == Pick_Point)
			{
				pickedPoint.clear();
				pickedPoint.push_back(point);

				if (faceIndex >= 0)
				{
					faceIndices.push_back(faceIndex);
				}
			}
			else if (pDoc->modelEditor->pickType == Pick_Circle)
			{
				faceIndices = pDoc->modelEditor->myGlsl->PickFaces_Radius(point.x, point.y, pDoc->modelEditor->paintPixel);
			}

			if (this->pDoc->modelEditor->selIns[0] >= 0 && this->pDoc->modelEditor->selIns[1] >= 0)
			{
				this->pDoc->modelEditor->AddMesh_labelInfo(faceIndices, this->pDoc->modelEditor->selIns[0], this->pDoc->modelEditor->selIns[1]);
				this->pDoc->modelEditor->ifEdited = true;
				pDoc->modelEditor->RenderScene();
				if (pDoc->modelEditor->pickType == Pick_Circle)
					MbCircle(point.x, point.y, pDoc->modelEditor->paintPixel);
			}
		}
#if 0
		int id = pDoc->modelEditor->myGlsl->PickFace(point.x, point.y);

		pDoc->modelEditor->myGlsl->SetFaceHighlight(id);
		std::cout << "选中第 " << id << " 个面片" << endl;
		pDoc->modelEditor->RenderScene();
#endif // 0

	}
	else if (bShift)
	{
		int faceIndex = pDoc->modelEditor->myGlsl->PickFace(point.x, point.y);
		if (faceIndex >= 0)
		{
			if (this->pDoc->modelEditor->meshGraph == nullptr)
			{
				this->pDoc->modelEditor->meshGraph = new MeshGraph();
				this->pDoc->modelEditor->meshGraph->CreateGraph(this->pDoc->modelEditor->model_origin, false, false);
			}
			CMyTest test;
			test.TestReadFaceTexture(faceIndex, this->pDoc->modelEditor->model_origin, this->pDoc->modelEditor->meshGraph);

			pDoc->modelEditor->RenderScene();
		}
	}
	else
	{
		/*int faceIndex = pDoc->modelEditor->myGlsl->PickFace(point.x, point.y);
		if (faceIndex < 0)
		{
			m_pivotPoint = easy3d::vec3(0, 0, 0);
		}
		else
		{
			easy3d::vec3 faceMidPos(0, 0, 0);
			if (pDoc->modelEditor->model)
			{
				for (size_t j = 0; j < 3; j++)
				{
					for (size_t k = 0; k < 3; k++)
					{
						faceMidPos[j] = pDoc->modelEditor->model->vertices[3 * pDoc->modelEditor->model->triangles[faceIndex].vindices[k] + j];
					}
				}
			}
			for (size_t i = 0; i < 3; i++)
			{
				faceMidPos[i] = faceMidPos[i] / 3.0;
			}
			m_pivotPoint = faceMidPos;
		}*/
	}
#endif // Ctrl（+Shift）+鼠标左键选中建筑实例
	
	CView::OnLButtonDown(nFlags, point);
}

void CLabel3DView::OnRButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	lastPos = point;

#if 1
	BOOL bCtrl = GetKeyState(VK_CONTROL) & 0x8000;
	BOOL bShift = GetAsyncKeyState(VK_LSHIFT) && 0x8000;

	if (!pDoc->modelEditor->myGlsl)
	{
		return;
	}

	if (bCtrl)	//Ctrl键按下
	{
		if (this->pDoc->modelEditor->editState == Edit_Edit)
		{
			vector<int> faceIndices;
			if (pDoc->modelEditor->pickType == Pick_Point)
			{
				pickedPoint.clear();
				pickedPoint.push_back(point);
				int faceIndex = pDoc->modelEditor->myGlsl->PickFace(point.x, point.y);
				if (faceIndex >= 0)
				{
					faceIndices.push_back(faceIndex);
				}
			}
			else if (pDoc->modelEditor->pickType == Pick_Circle)
			{
				faceIndices = pDoc->modelEditor->myGlsl->PickFaces_Radius(point.x, point.y, pDoc->modelEditor->paintPixel);
			}
			if (this->pDoc->modelEditor->selIns[0] >= 0 && this->pDoc->modelEditor->selIns[1] >= 0)
			{
				this->pDoc->modelEditor->DeleteMesh_labelInfo(faceIndices);
				this->pDoc->modelEditor->ifEdited = true;
				pDoc->modelEditor->RenderScene();
				if (pDoc->modelEditor->pickType == Pick_Circle)
					MbCircle(point.x, point.y, pDoc->modelEditor->paintPixel);
			}
		}
	}
#endif // Ctrl（+Shift）+鼠标左键选中建筑实例

	CView::OnRButtonDown(nFlags, point);
}

void CLabel3DView::OnLButtonUp(UINT nFlags, CPoint point)
{
	BOOL bCtrl = GetKeyState(VK_CONTROL) & 0x8000;
	if (bCtrl)	//Ctrl键按下
	{
		if (this->pDoc->modelEditor->editState == Edit_Edit)
		{
			if (pDoc->modelEditor->pickType == Pick_Rect)
			{
				if (this->pDoc->modelEditor->selIns[0] >= 0 && this->pDoc->modelEditor->selIns[1] >= 0)
				{
					easy3d::ivec2 p1(lastPos.x, lastPos.y);
					easy3d::ivec2 p2(point.x, point.y);
					vector<int> faceIndices = pDoc->modelEditor->myGlsl->PickFaces_Rect2(p1, p2);
					this->pDoc->modelEditor->AddMesh_labelInfo(faceIndices, this->pDoc->modelEditor->selIns[0], this->pDoc->modelEditor->selIns[1]);
					this->pDoc->modelEditor->ifEdited = true;
					//pDoc->modelEditor->RenderScene();
				}
			}
			else if (pDoc->modelEditor->pickType == Pick_Point)
			{
				pickedPoint.push_back(point);
				//添加所有扫过地方的面片
				vector<int> faceIndices = GetAllFacesInRoot(pickedPoint);
				this->pDoc->modelEditor->AddMesh_labelInfo(faceIndices, this->pDoc->modelEditor->selIns[0], this->pDoc->modelEditor->selIns[1]);
				this->pDoc->modelEditor->ifEdited = true;
				pickedPoint.clear();
			}
		}

		pDoc->modelEditor->RenderScene();
	}

	lastPos = point;
	//m_pivotPoint = easy3d::vec3(0, 0, 0);

	CView::OnLButtonUp(nFlags, point);
}

void CLabel3DView::OnRButtonUp(UINT nFlags, CPoint point)
{
	BOOL bCtrl = GetKeyState(VK_CONTROL) & 0x8000;
	if (bCtrl)	//Ctrl键按下
	{
		if (this->pDoc->modelEditor->editState == Edit_Edit)
		{
			if (pDoc->modelEditor->pickType == Pick_Rect)
			{
				if (this->pDoc->modelEditor->selIns[0] >= 0 && this->pDoc->modelEditor->selIns[1] >= 0)
				{
					easy3d::ivec2 p1(lastPos.x, lastPos.y);
					easy3d::ivec2 p2(point.x, point.y);
					vector<int> faceIndices = pDoc->modelEditor->myGlsl->PickFaces_Rect2(p1, p2);
					this->pDoc->modelEditor->DeleteMesh_labelInfo(faceIndices);
					this->pDoc->modelEditor->ifEdited = true;
					//pDoc->modelEditor->RenderScene();
				}
			}
		}
		else if (pDoc->modelEditor->pickType == Pick_Point)
		{
			pickedPoint.push_back(point);
			//删除所有扫过地方的面片
			vector<int> faceIndices = GetAllFacesInRoot(pickedPoint);
			this->pDoc->modelEditor->DeleteMesh_labelInfo(faceIndices);
			this->pDoc->modelEditor->ifEdited = true;
			pickedPoint.clear();

		}
		pDoc->modelEditor->RenderScene();
	}

	if (this->pDoc->modelEditor->pivotPointType == PP_ScreenCenter)
	{
		this->pDoc->modelEditor->SetCameraPivotPoint(PP_ScreenCenter);
	}

	lastPos = point;
	CView::OnRButtonUp(nFlags, point);
}

void CLabel3DView::OnMouseMove(UINT nFlags, CPoint point)
{
	//HCURSOR hCur = LoadCursorFromFile(_T("F:\StudyDemo\Label3D\Label3D/res/cur.cur"));
	//::SetCursor(hCur);

	float moveSpeed = 5.0;	//鼠标移动模型速度，值越小移动越快
	int dx = point.x - lastPos.x;
	int dy = point.y - lastPos.y;
	BOOL bCtrl = GetKeyState(VK_CONTROL) & 0x8000;
	BOOL bShift = GetAsyncKeyState(VK_LSHIFT) && 0x8000;

	if (!pDoc->modelEditor->myGlsl)
	{
		return;
	}

	if (bCtrl)	//Ctrl键按下
	{
		if (this->pDoc->modelEditor->editState == Edit_Edit)
		{
			if (pDoc->modelEditor->pickType == Pick_Rect)
			{
				if ((nFlags & MK_LBUTTON) || (nFlags & MK_RBUTTON))
				{
					pDoc->modelEditor->RenderScene();
					rectPoint(lastPos, point);
				}
				return;
			}

			vector<int> faceIndices;
			if (pDoc->modelEditor->pickType == Pick_Point)
			{
				if ((nFlags & MK_LBUTTON) || (nFlags & MK_RBUTTON))
				{
					pickedPoint.push_back(point);
					int faceIndex = pDoc->modelEditor->myGlsl->PickFace(point.x, point.y);
					if (faceIndex >= 0)
					{
						faceIndices.push_back(faceIndex);
					}
				}
			}
			else if (pDoc->modelEditor->pickType == Pick_Circle)
			{
				if ((nFlags & MK_LBUTTON) || (nFlags & MK_RBUTTON))
				{
					faceIndices = pDoc->modelEditor->myGlsl->PickFaces_Radius(point.x, point.y, pDoc->modelEditor->paintPixel);
				}
			}

			if (nFlags & MK_LBUTTON)
			{
				if (this->pDoc->modelEditor->selIns[0] >= 0 && this->pDoc->modelEditor->selIns[1] >= 0)
				{
					this->pDoc->modelEditor->AddMesh_labelInfo(faceIndices, this->pDoc->modelEditor->selIns[0], this->pDoc->modelEditor->selIns[1]);
					this->pDoc->modelEditor->ifEdited = true;
					pDoc->modelEditor->RenderScene();
					if (pDoc->modelEditor->pickType == Pick_Circle)
						MbCircle(point.x, point.y, pDoc->modelEditor->paintPixel);
				}
			}
			else if (nFlags & MK_RBUTTON)
			{
				if (this->pDoc->modelEditor->selIns[0] >= 0 && this->pDoc->modelEditor->selIns[1] >= 0)
				{
					this->pDoc->modelEditor->DeleteMesh_labelInfo(faceIndices);
					this->pDoc->modelEditor->ifEdited = true;
					pDoc->modelEditor->RenderScene();
					if (pDoc->modelEditor->pickType == Pick_Circle)
						MbCircle(point.x, point.y, pDoc->modelEditor->paintPixel);
				}
			}
		}
	}
	else if (bShift)
	{
		//if (nFlags & MK_LBUTTON)
		//{
		//	easy3d::vec3 pivotPoint(0, 0, 0);
		//	if (this->pDoc->modelEditor->selIns[0] >= 0 && this->pDoc->modelEditor->selIns[1] >= 0)
		//	{
		//		LabelInfo::Instance& ins = pDoc->modelEditor->labelInfo->SearchInstance(this->pDoc->modelEditor->selIns[0], this->pDoc->modelEditor->selIns[1]);
		//		if (ins.instanceId != -2 && ins.insBBox.midMinMaxPoint.size()>0)
		//		{
		//			//pivotPoint[0] = pDoc->modelEditor->annotation->insBBox[pDoc->modelEditor->selInsIndex].midMinMaxPoint[0][0];
		//			//pivotPoint[1] = pDoc->modelEditor->annotation->insBBox[pDoc->modelEditor->selInsIndex].midMinMaxPoint[0][1];
		//			//pivotPoint[2] = pDoc->modelEditor->annotation->insBBox[pDoc->modelEditor->selInsIndex].midMinMaxPoint[0][2];

		//			pivotPoint[0] = ins.insBBox.midMinMaxPoint[0][0];
		//			pivotPoint[1] = ins.insBBox.midMinMaxPoint[0][1];
		//			pivotPoint[2] = ins.insBBox.midMinMaxPoint[0][2];
		//		}
		//	}
		//	pDoc->modelEditor->myGlsl->camera_->setPivotPoint(pivotPoint);

		//	lastPos = point;
		//	pDoc->modelEditor->myGlsl->camera_->frame()->action_rotate(point.x, point.y, dx, dy, pDoc->modelEditor->myGlsl->camera_, false);
		//	pDoc->modelEditor->RenderScene();

		//	pDoc->modelEditor->myGlsl->camera_->setPivotPoint(easy3d::vec3(0, 0, 0));
		//}
	}
	else
	{
		if (nFlags & MK_LBUTTON)
		{
			lastPos = point;
			//pDoc->modelEditor->myGlsl->camera_->setPivotPoint(m_pivotPoint);
			pDoc->modelEditor->myGlsl->camera_->frame()->action_rotate(point.x, point.y, dx, dy, pDoc->modelEditor->myGlsl->camera_, false);
			pDoc->modelEditor->RenderScene();
		}
		else if (nFlags & MK_RBUTTON)
		{
			lastPos = point;
			pDoc->modelEditor->myGlsl->camera_->frame()->action_translate(point.x, point.y, dx, dy, pDoc->modelEditor->myGlsl->camera_, false);
			pDoc->modelEditor->RenderScene();
		}
		else if (nFlags & MK_MBUTTON)
		{
			if (dy != 0)
				pDoc->modelEditor->myGlsl->camera_->frame()->action_zoom_nolimit(dy > 0 ? 1 : -1, pDoc->modelEditor->myGlsl->camera_);
			pDoc->modelEditor->RenderScene();
		}
		lastPos = point;
	}
}

BOOL CLabel3DView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt)
{
	// TODO: 在此添加消息处理程序代码和/或调用默认值
	/*this->modelEditor->glCamera->slide(0, 0, zDelta / 10.0);*/
	//pDoc->modelEditor->glCamera->slide(0, 0, zDelta / 100.0);

	zDelta /= 100.0;

	BOOL bShift = GetAsyncKeyState(VK_LSHIFT) && 0x8000;

	if (!pDoc->modelEditor->myGlsl)
	{
		return false;
	}

	//easy3d::vec3 pivotPoint(0, 0, 0);
	//if (this->pDoc->modelEditor->selIns[0] >= 0 && this->pDoc->modelEditor->selIns[1] >= 0)
	//{
	//	LabelInfo::Instance& ins = pDoc->modelEditor->labelInfo->SearchInstance(this->pDoc->modelEditor->selIns[0], this->pDoc->modelEditor->selIns[1]);
	//	if (ins.instanceId != -2 && ins.insBBox.midMinMaxPoint.size()>0)
	//	{
	//		//pivotPoint[0] = pDoc->modelEditor->annotation->insBBox[pDoc->modelEditor->selInsIndex].midMinMaxPoint[0][0];
	//		//pivotPoint[1] = pDoc->modelEditor->annotation->insBBox[pDoc->modelEditor->selInsIndex].midMinMaxPoint[0][1];
	//		//pivotPoint[2] = pDoc->modelEditor->annotation->insBBox[pDoc->modelEditor->selInsIndex].midMinMaxPoint[0][2];

	//		pivotPoint[0] = ins.insBBox.midMinMaxPoint[0][0];
	//		pivotPoint[1] = ins.insBBox.midMinMaxPoint[0][1];
	//		pivotPoint[2] = ins.insBBox.midMinMaxPoint[0][2];
	//	}
	//}
	//pDoc->modelEditor->myGlsl->camera_->setPivotPoint(pivotPoint);
	//pDoc->modelEditor->myGlsl->camera_->frame()->setZoomsOnPivotPoint(true);

	pDoc->modelEditor->myGlsl->camera_->frame()->action_zoom_nolimit(zDelta, pDoc->modelEditor->myGlsl->camera_);
	pDoc->modelEditor->RenderScene();

	//pDoc->modelEditor->myGlsl->camera_->setPivotPoint(easy3d::vec3(0, 0, 0));

	return CView::OnMouseWheel(nFlags, zDelta, pt);
}

//用户自定义函数

//键盘事件
BOOL CLabel3DView::PreTranslateMessage(MSG* pMsg)
{
	if (pMsg->message == WM_KEYDOWN)
	{
		if (GetAsyncKeyState(VK_CONTROL) && pMsg->wParam == 'F')	//Ctrl+F同时按下
		{
			//if (this->pDoc->modelEditor->polygonPoints_3D.size() > 2)
			//{
			//	bool isOk = this->pDoc->modelEditor->AddNewInsRec(this->pDoc->modelEditor->polygonPoints_3D);
			//	if (isOk)
			//	{
			//		this->pDoc->AddNewListItem();
			//	}
			//	this->pDoc->modelEditor->polygonPoints_3D.clear();
			//	this->pDoc->modelEditor->editState = Edit_Render;
			//	this->pDoc->modelEditor->RenderScene();
			//}
		}
		else if (GetAsyncKeyState(VK_CONTROL) && pMsg->wParam == 'W')
		{
			this->pDoc->SetRadioSelect();
			this->pDoc->modelEditor->SetStateRender();
		}
		else if (GetAsyncKeyState(VK_CONTROL) && pMsg->wParam == 'E')
		{
			this->pDoc->SetRadioEdit();
			this->pDoc->modelEditor->SetStateEdit();
		}
		else if (GetAsyncKeyState(VK_CONTROL) && pMsg->wParam == 'Z')	//撤销操作
		{
		}
		else if (GetAsyncKeyState(VK_CONTROL) && pMsg->wParam == 'Y')	//取消撤销操作
		{
		}
	}

	return CView::PreTranslateMessage(pMsg);
}

void CLabel3DView::getMaxXY()
{
	CRect rect;
	GetClientRect(&rect);
	maxX = rect.right;
	maxY = rect.bottom;
}

void CLabel3DView::circlePoint(double x, double y, int ori_x, int ori_y)
{
	ASSERT(m_pDC != NULL);

	COLORREF color = RGB(225, 0, 0);

	m_pDC->SetPixel(ROUND(x) + ori_x, ROUND(y) + ori_y, color); //(x, y)
	m_pDC->SetPixel(ROUND(y) + ori_x, ROUND(x) + ori_y, color); //(y, x)
	m_pDC->SetPixel(ROUND(y) + ori_x, -ROUND(x) + ori_y, color); //(y, -x)
	m_pDC->SetPixel(ROUND(x) + ori_x, -ROUND(y) + ori_y, color); //(x, -y)
	m_pDC->SetPixel(-ROUND(x) + ori_x, -ROUND(y) + ori_y, color); //(-x, -y)
	m_pDC->SetPixel(-ROUND(y) + ori_x, -ROUND(x) + ori_y, color); //(-y, -x)
	m_pDC->SetPixel(-ROUND(y) + ori_x, ROUND(x) + ori_y, color); //(-y, x)
	m_pDC->SetPixel(-ROUND(x) + ori_x, ROUND(y) + ori_y, color); //(-x, y)
}

void CLabel3DView::rectPoint(const CPoint& p1, const CPoint& p2)
{
	ASSERT(m_pDC != NULL);
	COLORREF color = RGB(225, 0, 0);
	CPoint lbPoint;
	CPoint rtPoint;
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

	for (int x = lbPoint.x; x < rtPoint.x; x++)
	{
		m_pDC->SetPixel(x, p1.y, color);
		m_pDC->SetPixel(x, p2.y, color);
	}

	for (int y = lbPoint.y; y < rtPoint.y; y++)
	{
		m_pDC->SetPixel(p1.x, y, color);
		m_pDC->SetPixel(p2.x, y, color);
	}
}

void CLabel3DView::MbCircle(int ori_x, int ori_y, double r)
{
	getMaxXY();

	double x, y, d;
	d = 1.25 - r;x = 0;y = r;

	for (x = 0; x < y; x++)
	{
		circlePoint(x, y, ori_x, ori_y);
		if (d < 0)
		{
			d = d + 2 * x + 3;
		}
		else
		{
			d = d + 2 * (x - y) + 5;
			y--;
		}
	}
}

//两点之间采样
vector<CPoint> samplingPoints(const CPoint& p1, const CPoint& p2, int pixelSetp)
{
	vector<CPoint> points;

	Eigen::Vector2d direction(p2.x - p1.x, p2.y - p1.y);
	direction.normalize();
	double length = sqrt(pow(p2.x - p1.x, 2) + pow(p2.y - p1.y, 2));
	for (double i = 0; i < length; i = i + pixelSetp)
	{
		CPoint p;
		p.x = floor(p1.x + direction.x() * i);
		p.y = floor(p1.y + direction.y() * i);
		if (p == p1 || p == p2)
		{
			continue;
		}
		points.push_back(p);
	}
	return points;
}

//获取路径上所有经过的面片索引
vector<int> CLabel3DView::GetAllFacesInRoot(const vector<CPoint>& points)
{
	vector<int> faces;

	int pSize = points.size();
	if (pSize < 2)
	{
		return faces;
	}

	vector<CPoint> allPoints = points;
	for (size_t i = 1; i < pSize; i++)
	{
		vector<CPoint> p_bet2p = samplingPoints(points[i-1], points[i], 5);
		int p_bet2p_size = p_bet2p.size();
		for (size_t j = 0; j < p_bet2p_size; j++)
		{
			allPoints.push_back(p_bet2p[j]);
		}
	}

	int allPoints_size = allPoints.size();
	for (size_t i = 0; i < allPoints_size; i++)
	{
		int faceIndices = pDoc->modelEditor->myGlsl->PickFace(allPoints[i].x, allPoints[i].y);
		if (faceIndices >= 0)
		{
			faces.push_back(faceIndices);
		}
	}

	sort(faces.begin(), faces.end());
	faces.erase(unique(faces.begin(), faces.end()), faces.end());

	return faces;
}

