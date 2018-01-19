#pragma once
#include"Util.h"
class vtkPixelManager
{
public:
protected:
	double *positionX;//储存像素点的坐标
	double *positionY;
	double *positionZ;
	unsigned char *pixel;//像素点的像素值
	int dims[3];//像素点的个数
	vtkImageData *source;
public:
	vtkPixelManager();
	~vtkPixelManager();
	void initial();//初始化
	void Update();//更新
	void  deleteData();
	void SetInputData(vtkImageData *source);//连接管线
	int *getDimensions();
	unsigned char * GetScalarPointer(int x, int y, int z);//得到特定序号的像素值
	void GetPosition(int x, int y, int z, double *&pos);//得到特定序号的像素点位置
};

