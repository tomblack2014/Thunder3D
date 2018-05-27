#include "mat4f.h"

Mat4f Mat4f::translate(_In_ const Vec4f& pos){
	return Mat4f(
		1, 0, 0, pos.x,
		0, 1, 0, pos.y,
		0, 0, 1, pos.z,
		0, 0, 0, 1
		);
}

Mat4f Mat4f::lookat(_In_ const Vec4f& eye, _Inout_ Vec4f& at, _Inout_ Vec4f& up)
{
	Vec4f f = at - eye;
	f.normal();
	at = eye + f;

	up.normal();
	Vec4f right = f.cross(up);
	right.normal();
	up = right.cross(f);

	Mat4f rot(
		right.x, right.y, right.z, 0,
		up.x, up.y, up.z, 0,
		- f.x, - f.y, - f.z, 0,
		0, 0, 0, 1
		);

	return rot * translate(- eye);
}

Mat4f Mat4f::rotate_sse(_Inout_ Vec4f& axis, _In_ const float ang){
	float c = cos(ang), s = sin(ang);
	axis.normal();
	Mat4f mat1 = axis.tensormul(axis * (1 - c));
	Vec4f v2 = axis * s;
	Mat4f mat2(
		c, -v2.z, v2.y, 0,
		v2.z, c, -v2.x, 0,
		-v2.y, v2.x, c, 0,
		0, 0, 0, 1
		);
	return mat2 + mat1;
}

Mat4f Mat4f::rotate(_Inout_ Vec4f& axis, _In_ const float ang){
	float c = cos(ang), s = sin(ang);
	axis.normal();
	float x = axis.x, y = axis.y, z = axis.z;
	return Mat4f(
		c + x * x * (1 - c), - z * s + x * y * (1 - c), y * s + x * z * (1 - c), 0,
		z * s + x * y * (1 - c), c + y * y * (1 - c), - x * s + x * z * (1 - c), 0,
		- y * s + x * z * (1 - c), x * s + x * z * (1 - c), c + z * z * (1 - c), 0,
		0, 0, 0, 1
		);
}

Vec4f Mat4f::FindEyePos() const{
	Mat4f inv = this->transpose();
	inv.m[3] = inv.m[7] = inv.m[11] = inv.m[12] = inv.m[13] = inv.m[14] = 0.0f;
	Vec4f ret = inv * c[3];
	ret.v[0] = - ret.v[0];
	ret.v[1] = - ret.v[1];
	ret.v[2] = - ret.v[2];
	return ret;
}
