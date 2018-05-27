#ifndef _MATERIALMODEL_H
#define _MATERIALMODEL_H

#include "common.h"

class MaterialModel{
private:
	//组件头
	struct Header{
		float amb[3];  //环境光颜色
		float diff[3]; //漫射光颜色
		unsigned long cnt; //点数
	};
	//顶点描述
	struct Point{
		float x, y, z, nx, ny, nz;
	};
	//组件描述
	struct Asset{
		Point* st;
		Point* ed;
	};

	Header* heads;
	Point* pnts;
	Asset* asts;
	unsigned long cnt;

public:
	MaterialModel();

	//初始化一个模型（二进制文件）
	//binFile: 模型文件名
	//返回值 : 是否成功
	bool Initialize(_In_ const char* binFile);

	//绘制这个模型
	void Render();

	~MaterialModel();
};

#endif