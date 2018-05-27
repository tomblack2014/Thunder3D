#include "materialModel.h"
#include <Windows.h>
#include <gl\GL.h>
#include <gl\GLU.h>

MaterialModel::MaterialModel():
heads(NULL),
pnts(NULL),
asts(NULL),
cnt(0)
{
}

MaterialModel::~MaterialModel(){
	if(heads)
		delete[] heads;
	if(pnts)
		delete[] pnts;
	if(asts)
		delete[] asts;
}

bool MaterialModel::Initialize(_In_ const char* binFile){
	HANDLE hFile = CreateFileA(binFile, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
	DWORD dwSize;

	if(hFile == INVALID_HANDLE_VALUE){
		return false;
	}

	//读入组件数
	ReadFile(hFile, &cnt, sizeof(unsigned long), &dwSize, NULL);
	if(cnt == 0){
		return false;
	}

	//计算点数
	DWORD fileSize = GetFileSize(hFile, NULL);
	DWORD totalCount = (fileSize - 4 - sizeof(Header) * cnt) / sizeof(Point);

	//分配组件内存
	heads = new Header[cnt];
	asts = new Asset[cnt];
	pnts = new Point[totalCount];

	//读入组件
	for(unsigned long i = 0; i < cnt; i ++){
		//头
		ReadFile(hFile, &heads[i], sizeof(Header), &dwSize, NULL);
		if(i > 0){
			asts[i].st = asts[i - 1].ed;
		}else{
			asts[i].st = pnts;
		}
		asts[i].ed = asts[i].st + heads[i].cnt;
		//点
		ReadFile(hFile, asts[i].st, sizeof(Point)* heads[i].cnt, &dwSize, NULL);
	}

	//关闭文件
	CloseHandle(hFile);
	return true;
}

void MaterialModel::Render(){
	float tmp[4] = {0.3f, 0.3f, 0.3f, 1.0f};
	glLightfv(GL_LIGHT5, GL_AMBIENT, tmp);
	glEnable(GL_LIGHT5);

	glDisable(GL_TEXTURE_2D);
	glDisable(GL_COLOR_MATERIAL);
	glBegin(GL_TRIANGLES);
	for(unsigned long i = 0; i < cnt; i ++){
		for(int j = 0; j < 3; j ++)
			tmp[j] = heads[i].amb[j];
		glMaterialfv(GL_FRONT, GL_AMBIENT, tmp);
		for(int j = 0; j < 3; j ++)
			tmp[j] = heads[i].diff[j];
		glMaterialfv(GL_FRONT, GL_DIFFUSE, tmp);
		for(Point* ptr = asts[i].st; ptr != asts[i].ed; ptr ++){
			glNormal3f(ptr->nx, ptr->ny, ptr->nz);
			glVertex3f(ptr->x, ptr->y, ptr->z);
		}
	}
	glEnd();
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_COLOR_MATERIAL);

	glDisable(GL_LIGHT5);
}
