#include "appwindow.h"
#include "common.h"
#include <iostream>
#include <fstream>

using namespace std;

Quaternion AppWindow::sides[6] =
{
	Quaternion(Vector3f(0.0f, 1.0f, 0.0f), 0.0f),   // front
	Quaternion(Vector3f(0.0f, 1.0f, 0.0f), 180.0f), // back
	Quaternion(Vector3f(1.0f, 0.0f, 0.0f), 90.0f),  // top
	Quaternion(Vector3f(1.0f, 0.0f, 0.0f), -90.0f), // bottom
	Quaternion(Vector3f(0.0f, 1.0f, 0.0f), 90.0f),  // left
	Quaternion(Vector3f(0.0f, 1.0f, 0.0f), -90.0f), // right
};

float AppWindow::knots[8] = { 0,0,0,0,1,1,1,1 };

AppWindow::AppWindow() :
	range_max(50),
	pMesh(NULL)
{
	surfType = BEZIER;
	selPoint = 0;
	perspective = true;
	fillMode = true;
	dragScene = dragPoint = isAnim = false;

	ctrlPoints = new Vector3f[order * order];
	//LoadModel("plane.md");
	LoadModel("default.md");

	viewer.qRotation = Quaternion(Vector3f(1.0f, 0.0f, 0.0f), -150.0f);

	this->CreateParam("Bezier Surface", CW_USEDEFAULT, CW_USEDEFAULT, 800, 600);
}

bool AppWindow::LoadModel(LPCSTR filename)
{
	ifstream file(filename);
	if (!file) return false;

	for (int i = 0; i < 16; i++) {
		file >> ctrlPoints[i].x >> ctrlPoints[i].y >> ctrlPoints[i].z;
	}
	file.close();
	return true;
}

void AppWindow::ChangeFillMode(bool solid)
{
	if (pMesh) {
		gluNurbsProperty(pMesh, GLU_DISPLAY_MODE, (float)(solid ? GLU_FILL : GLU_OUTLINE_POLYGON));
	}
	void (__stdcall *func)(GLenum arg) = solid ? glEnable : glDisable;
	func(GL_DEPTH_TEST);
	func(GL_AUTO_NORMAL);
	func(GL_LIGHTING);
	func(GL_LIGHT0);
	func(GL_TEXTURE_2D);
	func(GL_MAP2_TEXTURE_COORD_2);
}

void AppWindow::OnCreate()
{
	glEnable(GL_MAP2_VERTEX_3);
	glEnable(GL_NORMALIZE);

	pMesh = gluNewNurbsRenderer();
	gluNurbsProperty(pMesh, GLU_SAMPLING_TOLERANCE, 40.0f);

	float ambient[4]  = { 0.4f, 0.4f, 0.4f, 1.0f };
	float diffuse[4]  = { 0.8f, 0.8f, 0.8f, 1.0f };
	float specular[4] = { 0.7f, 0.7f, 0.7f, 1.0f };

	glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
	glLightModeli(GL_LIGHT_MODEL_COLOR_CONTROL, GL_SEPARATE_SPECULAR_COLOR);
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular);

	glMaterialfv(GL_FRONT_AND_BACK, GL_SPECULAR, specular);
	glMateriali(GL_FRONT_AND_BACK, GL_SHININESS, 100);

	glMap2f(GL_MAP2_VERTEX_3, 0.0f, 1.0f, 3, order,
		0.0f, 1.0f, 3*order, order, ctrlPoints[0].data);

	float tex_points[4][2] = { {0,0}, {0,1}, {1,0}, {1,1} };
	glMap2f(GL_MAP2_TEXTURE_COORD_2, 0.0f, 1.0f, 2, 2,
		0.0f, 1.0f, 4, 2, &tex_points[0][0]);

	glMapGrid2f(range_max, 0.0f, 1.0f, range_max, 0.0f, 1.0f);

	tex.LoadFromTGA(L"tex.tga");
	tex.BuildMipmaps();
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);

    glClearColor(1.0f, 1.0f, 1.0f, 1.0f );
	ChangeFillMode(fillMode);
}

void AppWindow::OnDisplay()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glPushMatrix();

	float lightPos[4] = { 100.0f, 100.0f, 100.0f, 1.0f };
	glLightfv(GL_LIGHT0, GL_POSITION, lightPos);

	viewer.ApplyTransform();
	glColor3f(0.0f, 0.0f, 1.0f);
	
	if  (surfType == NURBS)
	{
		/*float outsidePts[5][2] =
			{{0.0f, 0.0f}, {1.0f, 0.0f}, {1.0f, 1.0f}, {0.0f, 1.0f}, {0.0f, 0.0f}};
		float insidePts[4][2] =
			{{0.25f, 0.25f}, {0.5f, 0.5f}, {0.75f, 0.25f}, { 0.25f, 0.25f}};*/

		glFrontFace(GL_CCW);
		gluBeginSurface(pMesh);
			gluNurbsSurface(pMesh, 8, knots, 8, knots,
				sizeof(float)*3, 3,
				ctrlPoints[0].data, 4, 4,
				GL_MAP2_VERTEX_3);

			/*gluBeginTrim(pMesh);
			gluPwlCurve(pMesh, 5, &outsidePts[0][0], 2, GLU_MAP1_TRIM_2);
			gluEndTrim(pMesh);

			gluBeginTrim(pMesh);
			gluPwlCurve(pMesh, 4, &insidePts[0][0], 2, GLU_MAP1_TRIM_2);
			gluEndTrim(pMesh);*/

		gluEndSurface(pMesh);
	}
	else {
		glFrontFace(GL_CW);
		glEvalMesh2(fillMode ? GL_FILL : GL_LINE, 0, range_max, 0, range_max);
	}

	DrawPoints();
    glPopMatrix();
}

void AppWindow::OnSize(int w, int h)
{
    glViewport(0, 0, w, h);

    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();

	
	if (perspective) {
		viewer.SetPerspective(45.0f, 0.01f, 200.0f, Point3f(0.0f, 0.0f, -20.0f), w, h);
	} else {
		float ratio;
		if (w > h) ratio = (float)w / h;
		else ratio = (float)h / w;
		viewer.SetOrtho(-10.0f, 10.0f, -10.0f * ratio, 10.0f * ratio, -10.0f, 10.0f, w, h);
	}

    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
}

void AppWindow::OnMouseDown(MouseButton btn, int x, int y)
{
	if (btn == MouseButton::LBUTTON) {
		if (PickPoint(x, y)) {
			last_pos = MapToWindow(ctrlPoints[selPoint], x, y);

			dragScene = false;
			dragPoint = true;
			RedrawWindow();
		}
		else {
			dragScene = true;
			dragPoint = false;
			viewer.BeginRotate(x, y);
		}
	} else if (btn == MouseButton::RBUTTON) {
		viewer.BeginPan(x, y);
		dragScene = true;
		dragPoint = false;
	}
}

void AppWindow::OnMouseUp(MouseButton btn, int x, int y)
{
	dragScene = dragPoint = false;
}

void AppWindow::OnMouseMove(UINT flags, int x, int y)
{
	if (isAnim) return;
	if (dragPoint) {
		Vector3f p = MapToWindow(ctrlPoints[selPoint], x, y);
		ctrlPoints[selPoint] += p - last_pos;
		last_pos = p;

		glMap2f(GL_MAP2_VERTEX_3, 0.0f, 1.0f, 3, order,
			0.0f, 1.0f, 3*order, order, ctrlPoints[0].data);
		RedrawWindow();
	}
	else if (dragScene) {
		if (flags & KM_LBUTTON) viewer.Rotate(x, y);
		else if (flags & KM_RBUTTON) viewer.Pan(x, y);
		RedrawWindow();
	}
}

void AppWindow::OnKeyDown(UINT key)
{
	if (dragPoint || dragScene || isAnim) return;

	switch (key)
	{
	case 'P':
		perspective = !perspective;
		RECT rect;
		GetClientRect(m_hwnd, &rect);
		OnSize(rect.right - rect.left, rect.bottom - rect.top);
		RedrawWindow();
		break;
	case 'S':
		ChangeFillMode(fillMode ^= true);
		RedrawWindow();
		break;
	case 'Z':
		surfType = surfType == BEZIER ? NURBS : BEZIER;
		RedrawWindow();
		break;
	case VK_TAB:
		selPoint += GetAsyncKeyState(VK_SHIFT) ? -1 : 1;
		if (selPoint < 0) selPoint = 15;
		else if (selPoint > 15) selPoint = 0;
		RedrawWindow();
		break;
	default:
		if (key >= '1' && key <= '6') {
			isAnim = true;
			anim.Setup(viewer.qRotation, sides[key-1-'0'], 0.15f);
			SetTimer(m_hwnd, 1, 30, NULL);
		}
	}
}

void AppWindow::OnMouseWheel(short delta, UINT flags, int x, int y)
{
	if (delta < 0) viewer.ZoomIn(0.9f);
	else viewer.ZoomOut(0.9f);
	this->RedrawWindow();
}

void AppWindow::OnTimer()
{
	if (!anim.IsComplete()) {
		viewer.qRotation = anim.Next();
		RedrawWindow();
	}
	else {
		isAnim = false;
		KillTimer(m_hwnd, 1);
	}
}

void AppWindow::DrawPoints()
{
	if (fillMode) {
		glDisable(GL_DEPTH_TEST);
		glDisable(GL_LIGHTING);
		glDisable(GL_TEXTURE_2D);
	}

    glPointSize(5.0f);
	glColor3f(0.0f, 0.0f, 0.0f);
    glBegin(GL_POINTS);
    for (int i = 0, n = order*order; i < n; i++) {
		if (i != selPoint)
			glVertex3fv(ctrlPoints[i].data);
	}
    glEnd();

	if (selPoint != -1)
	{
		RECT rect;
		GetClientRect(m_hwnd, &rect);
		int w = rect.right - rect.left;
		int h = rect.bottom - rect.top;

		Matrix44f projection;
		glGetFloatv(GL_PROJECTION_MATRIX, projection.data);

		glColor3f(1.0f, 0.0f, 0.0f);
		glBegin(GL_POINTS);
			glVertex3fv(ctrlPoints[selPoint].data);
		glEnd();

		glMatrixMode(GL_MODELVIEW);
		glPushMatrix();
		glMatrixMode(GL_PROJECTION);
		glPushMatrix();

		FrustumToWindow(ctrlPoints[selPoint], w, h, viewer.Modelview(), projection);

		glBegin(GL_LINE_STRIP);
		float a = (float)M_PI*0.2f;
		for (int i = 0; i <= 10; i++) {
			glVertex2f(10*cos(i*a), 10*sin(i*a));
		}
		glEnd();

		glMatrixMode(GL_PROJECTION);
		glPopMatrix();
		glMatrixMode(GL_MODELVIEW);
		glPopMatrix();
	}
	glPointSize(1.0f);

	if (fillMode) {
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_LIGHTING);
		glEnable(GL_TEXTURE_2D);
	}
}

Vector3f AppWindow::Projectf(const Vector3f &p, const Matrix44f &m, int viewport[4])
{
	Vector4f v = Vector4f(p) * m;
	v.Cartesian();

	return Vector3f(
		viewport[0] + (v.x + 1.0f) * 0.5f * viewport[2],
		0.5f * viewport[3] * (1.0f - v.y) - viewport[1],
		0.5f * v.z + 0.5f);
}

void AppWindow::FrustumToWindow(const Vector3f &origin, int winWidth, int winHeight,
	const Matrix44f &modelview, const Matrix44f &projection)
{
	int viewport[4] = { };
	glGetIntegerv(GL_VIEWPORT, viewport);
	Vector3f winCoords = Projectf(origin, projection * modelview, viewport);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, winWidth, winHeight, 0, -1.0f, 1.0f);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslatef(winCoords.x, winCoords.y, 0.0f);
}

bool AppWindow::PickPoint(int winX, int winY)
{
	Matrix44f m;
	glGetFloatv(GL_PROJECTION_MATRIX, m.data);
	m *= viewer.Modelview();

	int viewport[4] = { };
	glGetIntegerv(GL_VIEWPORT, viewport);

	for (int i = 0, n = order*order; i < n; i++)
	{
		Vector3f coords = Projectf(ctrlPoints[i], m, viewport);

		if (abs(coords.x - winX) <= 10 && abs(coords.y - winY) <= 10) {
			selPoint = i;
			return true;
		}
	}
	return false;
}

Vector3f AppWindow::MapToWindow(const Vector3f &p, int x, int y)
{
	Vector3d v(p);
	Matrix44d modelview(viewer.Modelview());
	Matrix44d projection;
	int viewport[4] = { };

	glGetDoublev(GL_PROJECTION_MATRIX, projection.data);
	glGetIntegerv(GL_VIEWPORT, viewport);

	gluProject(v.x, v.y, v.z, modelview.data, projection.data, viewport, &v.x, &v.y, &v.z);
	gluUnProject(x, viewport[3] - y, v.z, modelview.data, projection.data, viewport, &v.x, &v.y, &v.z);
	return Vector3f(v);
}

void AppWindow::OnDestroy()
{
	if (pMesh) gluDeleteNurbsRenderer(pMesh);
	delete [] ctrlPoints;
}