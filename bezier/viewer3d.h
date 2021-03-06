#ifndef _VIEWER3D_
#define _VIEWER3D_

#include "datatypes.h"
#include "quaternion.h"
#include <gl\GL.h>

class Viewer3D
{
public:
	Quaternion qRotation;
	Viewer3D();

	Matrix44f Modelview();
	void ApplyTransform();
	void ResetView();
	void BeginPan(int winX, int winY);
	void BeginRotate(int winX, int winY);
	void Pan(int winX, int winY);
	void Rotate(int winX, int winY);
	void ZoomIn(float scale);
	void ZoomOut(float scale);
	void SetScale(float scale);

	void SetPerspective(float fovy, float zNear, float zFar,
		Point3f center, int winWidth, int winHeight);

	void SetOrtho(float left, float right, float bottom, float top,
		float zNear, float zFar, int winWidth, int winHeight);

	void SetRotationSpeed(float factor) { view.s = 1.0f / factor; }
private:
	struct { float fw, fh, w, h, s; } view;
	float scale;
	Matrix44f rot, trs, matr, matr_inv;
	Vector3f from, to, last;
	bool changed;

	Vector3f pos(const Vector3f &p, int x, int y);
	void calcMatr();
};

#endif // _VIEWER3D_