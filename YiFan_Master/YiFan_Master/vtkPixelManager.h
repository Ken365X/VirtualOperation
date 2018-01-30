#pragma once
#include"head.h"
class vtkPixelManager
{
public:
	double *positionX;//�������ص������
	double *positionY;
	double *positionZ;
	unsigned char *pixel;//���ص������ֵ
	int * dims;//���ص�ĸ���
	vtkImageData *source;
public:
	vtkPixelManager();
	~vtkPixelManager();
	void initial();//��ʼ��
	void Update();//����
	void  deleteData();
	void SetInputData(vtkImageData *source);//���ӹ���
	int *getDimensions();
	unsigned char * GetScalarPointer(int x, int y, int z);//�õ��ض���ŵ�����ֵ
	void GetPosition(int x, int y, int z, double *&pos);//�õ��ض���ŵ����ص�λ��
};

