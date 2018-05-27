#ifndef _MODEL_H
#define _MODEL_H

#include "common.h"

class Model{
private:
	struct NODE{
		float x, y, z;
		float tu, tv;
		float nx, ny, nz;
	};

	//顶点
	NODE* node;

	//顶点数
	int count;

	//材质
	unsigned int texture;

	//从文件中载入文本模型
	//filename: 文件名
	//返回值  : 是否成功
	bool LoadTxtModel(_In_ const char* filename);

	//从二进制文件中载入模型
	//filename: 文件名
	//返回值  : 是否成功
	bool LoadBinModel(_In_ const char* filename);

	//保存到二进制文件
	//filename: 文件名
	//返回值  : 是否成功
	bool SaveBinModel(_In_ const char* filename);

	//从文件中载入BMP材质
	//filename: 文件名
	//返回值  : 是否成功
	bool LoadGLTexture_Bak(_In_ const char* filename);

	//从文件中载入任意图片材质
	//filename: 文件名
	//返回值  : 是否成功
	bool LoadGLTexture(_In_ const char* filename);

public:
	Model();

	//通用方法：载入任意图片
	//filename: 文件名
	//texture : 材质号
	//返回值  : 是否成功
	_Success_(return) static bool LoadGLTexture(_In_ const char* filename, _Out_ unsigned int *texture);

	//初始化一个模型（文本文件）
	//txtFile: 模型文件名
	//bmpFile: 材质文件名
	//返回值 : 是否成功
	bool InitializeTxt(_In_ const char* txtFile, _In_ const char* bmpFile);

	//初始化一个模型（二进制文件）
	//binFile: 模型文件名
	//bmpFile: 材质文件名
	//返回值 : 是否成功
	bool Initialize(_In_ const char* binFile, _In_ const char* bmpFile);

	//绘制这个模型
	void Render();

	~Model();
};

#endif