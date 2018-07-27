#ifndef _VEC4F_H
#define _VEC4F_H

#include "common.h"
#include <cmath>

struct Mat4f;

/**
* 通用SSE点、向量、四元数类
* 对于3D空间中的点，要求w=1.0
* 对于特定方向距离的向量，要求w=0.0
*/
struct Vec4f{
	union{
		float v[4];
		struct{
			float x;
			float y;
			float z;
			float w;
		};
	};

	//初始化一个空向量
	Vec4f();

	//初始化一个特定点或向量
	Vec4f(_In_ float x, _In_ float y, _In_ float z, _In_ float w = 0.0f);

	//向量的复制
	Vec4f(_In_ const Vec4f& rhs);

	//向量的内积
	float dot(_In_ const Vec4f &rhs) const;

	//向量加法
	Vec4f operator+(_In_ const Vec4f &rhs) const;

	//点或向量的减法
	Vec4f operator-(_In_ const Vec4f &rhs) const;

	//向量的反向
	Vec4f operator-() const;

	//向量的数乘
	Vec4f operator*(_In_ const float scalar) const;

	//向量的逆数乘
	Vec4f operator/(_In_ const float scalar) const;

	//单位化当前向量
	Vec4f normal();

	//向量的叉乘
	Vec4f cross(_In_ const Vec4f &rhs) const;

	//计算两个向量之间的向量
	float CalAng(_In_ const Vec4f &rhs);

	//向量的长度
	float length() const;

	//两点的距离
	float dist(_In_ const Vec4f& rhs) const;

	//向量的张量积
	Mat4f tensormul(_In_ const Vec4f& rhs) const;

	//旋转的四元数
	//axis: 旋转轴，返回单位化后的旋转轴
	//ang : 逆时针旋转角
	static Vec4f rotate(_Inout_ Vec4f& axis, _In_ const float ang);

	//平移向量插值
	//v0: 起点向量
	//v1: 终点向量
	//t : 时间值
	static Vec4f lerp(_In_ const Vec4f& v0, _In_ const Vec4f& v1, _In_ const float t);

	//四元数插值
	//v0: 起点四元数
	//v1: 终点四元数
	//t : 时间值
	static Vec4f slerp(_In_ const Vec4f& q0, _In_ const Vec4f& q1, _In_ const float t);

	//四元数转旋转矩阵
	Mat4f QuatToMat() const;

	//旋转矩阵转四元数
	static Vec4f QuatFromMat(_In_ const Mat4f& mat);
};

inline Vec4f::Vec4f(){
	v[0] = v[1] = v[2] = v[3] = 0.0f;
}

inline Vec4f::Vec4f(_In_ float x, _In_ float y, _In_ float z, _In_ float w){
	v[0] = x;
	v[1] = y;
	v[2] = z;
	v[3] = w;
}

inline Vec4f::Vec4f(_In_ const Vec4f& rhs){
	v[0] = rhs.v[0];
	v[1] = rhs.v[1];
	v[2] = rhs.v[2];
	v[3] = rhs.v[3];
}

inline float Vec4f::dot(_In_ const Vec4f &rhs) const{
	return v[0] * rhs.v[0] + v[1] * rhs.v[1] + v[2] * rhs.v[2] + v[3] * rhs.v[3];
}

inline Vec4f Vec4f::operator+(_In_ const Vec4f &rhs) const{
	return Vec4f(v[0] + rhs.v[0], v[1] + rhs.v[1], v[2] + rhs.v[2], v[3] + rhs.v[3]);
}

inline Vec4f Vec4f::operator-(_In_ const Vec4f &rhs) const{
	return Vec4f(v[0] - rhs.v[0], v[1] - rhs.v[1], v[2] - rhs.v[2], v[3] - rhs.v[3]);
}

inline Vec4f Vec4f::operator-() const{
	return Vec4f(-v[0], -v[1], -v[2], -v[3]);
}

inline Vec4f Vec4f::operator*(_In_ const float scalar) const{
	return Vec4f(v[0] * scalar, v[1] * scalar, v[2] * scalar, v[3] * scalar);
}

inline Vec4f Vec4f::operator/(_In_ const float scalar) const{
	return *this * (1.0f / scalar);
}

inline Vec4f Vec4f::normal(){
	*this = *this / this->length();
	return *this;
}

inline Vec4f Vec4f::cross(_In_ const Vec4f &rhs) const{
	return Vec4f(
		v[1] * rhs.v[2] - v[2] * rhs.v[1],
		v[2] * rhs.v[0] - v[0] * rhs.v[2],
		v[0] * rhs.v[1] - v[1] * rhs.v[0],
		0.0f);
}

inline float Vec4f::CalAng(_In_ const Vec4f &rhs)
{
	return acos(this->dot(rhs) / this->length() / rhs.length());
}

inline float Vec4f::length() const{
	return sqrt(this->dot(*this));
}

inline float Vec4f::dist(_In_ const Vec4f& rhs) const{
	return (*this - rhs).length();
}

#endif