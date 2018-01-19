#pragma once
#include"Util.h"
#include"..\..\..\..\VTK\example\VirtualOperation\bin\vtkPixelManager.h"
class PointerPickerInteractorStyle :public vtkInteractorStyleTrackballCamera
{
public:
	vtkSmartPointer<vtkImageData> img;
	vtkPixelManager *manager;
public:
	static PointerPickerInteractorStyle* New();
	vtkTypeMacro(PointerPickerInteractorStyle, vtkInteractorStyleTrackballCamera);
	bool getIndex(double *picked, int *index);//获得有效序列号
	void  choosePoint(vtkSmartPointer<vtkImageData> &img, double picked[3]);
	virtual void OnRightButtonDown();//右键事件
};

