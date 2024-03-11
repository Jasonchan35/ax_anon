#pragma once

#include "axRay3.h"

template<class T>
class axCamera3 {
public:
	using Vec2  = axVec2<T>;
	using Vec3  = axVec3<T>;
	using Vec4  = axVec4<T>;
	using Mat4  = axMat4<T>;
	using Quat4 = axQuat4<T>;
	using Rect2 = axRect2<T>;
	using Ray3	= axRay3<T>;

	void pan	(T x, T y);
	void orbit	(T x, T y);
	void move	(T x, T y, T z);
	void move	(const Vec3& v) { move(v.x, v.y, v.z); }

	void dolly	(T z);

	void setPos(T x, T y, T z)	{ setPos(Vec3(x,y,z)); }
	void setAim(T x, T y, T z)	{ setAim(Vec3(x,y,z)); }
	void setUp (T x, T y, T z)	{ setUp (Vec3(x,y,z)); }

	void setPos(const Vec3& pos)	{ _pos = pos; }
	void setAim(const Vec3& aim)	{ _aim = aim; }
	void setUp (const Vec3& up)		{ _up  = up;  }

	const Vec3& pos() const { return _pos; }
	const Vec3& aim() const { return _aim; }
	const Vec3& up () const { return _up;  }

	void setViewport(const Rect2& v) { _viewport = v; }
	const Rect2& viewport() const { return _viewport; }

	Ray3	getRay(const Vec2& screenPos) const;

	Mat4	viewMatrix() const;
	Mat4	projMatrix() const;
	Mat4	viewProjMatrix() const { return projMatrix() * viewMatrix(); }

private:
	//axVec3f pos {244, 170, -193};
	//axVec3f aim {-58, 190, -91};
	//axVec3f _pos {500, 250, 500};
		
	float _fov = 50.0f;
	float _nearClip = 0.1f;
	float _farClip = 10000.0f;
	Rect2 _viewport;
	Vec3 _pos {150, 150, 200};
	Vec3 _aim {0,0,0};
	Vec3 _up  {0,1,0};
};

using axCamera3f = axCamera3<axFloat32>;
using axCamera3d = axCamera3<axFloat64>;
