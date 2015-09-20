#ifndef _APPWINDOW_H_
#define _APPWINDOW_H_

#include "glwindow.h"
#include "qslerp.h"
#include "texture2d.h"
#include "viewer3d.h"

class AppWindow : public GLWindow
{
public:
	AppWindow();
protected:
	void OnCreate();
	void OnDisplay();
	void OnSize(int w, int h);
	void OnMouseDown(MouseButton btn, int x, int y);
	void OnMouseUp(MouseButton btn, int x, int y);
	void OnMouseMove(UINT keys, int x, int y);
	void OnKeyDown(UINT keyCode);
	void OnMouseWheel(short delta, UINT flags, int x, int y);
	void OnTimer();
	void OnDestroy();
private:
	enum { BEZIER, NURBS } surfType;
	static const int order = 4;
	int range_max;

	int selPoint;
	bool perspective, fillMode;
	bool dragScene, dragPoint, isAnim;
	Vector3f last_pos;

	Vector3f *ctrlPoints;
	static Quaternion sides[6];

	GLUnurbs *pMesh;
	static float knots[8];

	Viewer3D viewer;
	QSlerp anim;
	Texture2D tex;

	bool LoadModel(LPCSTR filename);
	void ChangeFillMode(bool solid);
	void DrawPoints();
	
	Vector3f Projectf(const Vector3f &p, const Matrix44f &m, int viewport[4]);
	void FrustumToWindow(const Vector3f &origin, int winWidth, int winHeight,
		const Matrix44f &modelview, const Matrix44f &projection);
	bool PickPoint(int winX, int winY);
	Vector3f MapToWindow(const Vector3f &p, int x, int y);
};

#endif // _APPWINDOW_H_