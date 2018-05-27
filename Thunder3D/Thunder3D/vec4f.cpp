#include "mat4f.h"

Mat4f Vec4f::tensormul(_In_ const Vec4f& rhs) const{
	return Mat4f(
		v[0] * rhs.v[0], v[0] * rhs.v[1], v[0] * rhs.v[2], v[0] * rhs.v[3],
		v[1] * rhs.v[0], v[1] * rhs.v[1], v[1] * rhs.v[2], v[1] * rhs.v[3],
		v[2] * rhs.v[0], v[2] * rhs.v[1], v[2] * rhs.v[2], v[2] * rhs.v[3],
		v[3] * rhs.v[0], v[3] * rhs.v[1], v[3] * rhs.v[2], v[3] * rhs.v[3]
		);
}

Vec4f Vec4f::rotate(_Inout_ Vec4f& axis, _In_ const float ang){
	float c = cos(ang / 2.0f), s = sin(ang / 2.0f);
	axis.normal();
	return Vec4f(axis.x * s, axis.y * s, axis.z * s, c);
}

Vec4f Vec4f::lerp(_In_ const Vec4f& v0, _In_ const Vec4f& v1, _In_ const float t){
	return v0 + (v1 - v0) * t;
}

static float CounterWarp(float t, float fCos)
{
	const float ATTENUATION = 0.82279687f;
	const float WORST_CASE_SLOPE = 0.58549219f;

	float fFactor = 1.0f - ATTENUATION * fCos;
	fFactor *= fFactor;
	float fK = WORST_CASE_SLOPE * fFactor;

	return t * (fK * t * (2.0f * t - 3.0f) + 1.0f + fK);
}

Vec4f Vec4f::slerp(_In_ const Vec4f& q0, _In_ const Vec4f& q1, _In_ const float t){
	Vec4f tq1 = q1;
	float fCos = q0.dot(tq1);
	if(fCos < 0.0f){
		fCos = -fCos;
		tq1 = - q1;
	}
	float fTPrime;
	if(t <= 0.5f) {
		fTPrime = CounterWarp(t, fCos);
	} else {
		fTPrime = 1.0f - CounterWarp(1.0f - t, fCos);
	}
	Vec4f ret = lerp(q0, q1, fTPrime);
	ret.normal();
	return ret;
}

Mat4f Vec4f::QuatToMat() const{
	float tx = 2.0f * v[0];
	float ty = 2.0f * v[1];
	float tz = 2.0f * v[2];
	float twx = tx * v[3];
	float twy = ty * v[3];
	float twz = tz * v[3];
	float txx = tx * v[0];
	float txy = ty * v[0];
	float txz = tz * v[0];
	float tyy = ty * v[1];
	float tyz = tz * v[1];
	float tzz = tz * v[2];

	return Mat4f(
		1.0f - tyy - tzz, txy - twz, txz + twy, 0.0f,
		txy + twz, 1.0f - txx - tzz, tyz - twx, 0.0f,
		txz - twy, tyz + twx, 1.0f - txx - tyy, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f
		);
}

Vec4f Vec4f::QuatFromMat(_In_ const Mat4f& mat){
	float fTrace = mat.at(0, 0) + mat.at(1, 1) + mat.at(2, 2);
	float fRoot;
	Vec4f ret;

	if(fTrace > 0.0f)
	{
		// |w| > 1/2, may as well choose w > 1/2
		fRoot = sqrt(fTrace + 1.0f);  // 2w
		ret.v[3] = 0.5f * fRoot;
		fRoot = 0.5f / fRoot;  // 1/(4w)

		ret.v[0] = (mat.at(2, 1) - mat.at(1, 2)) * fRoot;
		ret.v[1] = (mat.at(0, 2) - mat.at(2, 0)) * fRoot;
		ret.v[2] = (mat.at(1, 0) - mat.at(0, 1)) * fRoot;
	} else
	{
		// |w| <= 1/2
		static unsigned int next[3] = {1, 2, 0};
		unsigned int i = 0;
		if(mat.at(1, 1) > mat.at(0, 0))
			i = 1;
		if(mat.at(2, 2) > mat.at(i, i))
			i = 2;
		unsigned int j = next[i];
		unsigned int k = next[j];

		fRoot = sqrt(mat.at(i, i) - mat.at(j, j) - mat.at(k, k) + 1.0f);
		float* quat[3] = {&ret.v[0], &ret.v[1], &ret.v[2]};
		*quat[i] = 0.5f * fRoot;
		fRoot = 0.5f / fRoot;
		ret.v[3] = (mat.at(k, j) - mat.at(j, k)) * fRoot;
		*quat[j] = (mat.at(j, i) + mat.at(i, j)) * fRoot;
		*quat[k] = (mat.at(k, i) + mat.at(i, k)) * fRoot;
	}

	return ret;
}