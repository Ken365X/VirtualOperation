#pragma once
#include"Util.h"
class vtkPixelManager
{
public:
protected:
	double *positionX;//�������ص������
	double *positionY;
	double *positionZ;
	unsigned char *pixel;//���ص������ֵ
	int dims[3];//���ص�ĸ���
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

