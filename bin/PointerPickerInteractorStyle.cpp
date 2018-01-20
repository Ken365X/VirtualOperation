#include "PointerPickerInteractorStyle.h"
#include"..\..\..\..\VTK\example\VirtualOperation\bin\common.h"
bool PointerPickerInteractorStyle::getIndex(double *picked, int *index)
{
	int *temp = manager->getDimensions();
	for (int i = 0; i < 3; i++)
	{
		if (0 <= picked[i] && picked[i] < temp[i])//防止出现过大过小的可能
		{
			index[i] = picked[i];
		}
		else
		{
			return false;
		}
	}
	return true;
}
void PointerPickerInteractorStyle::choosePoint(vtkSmartPointer<vtkImageData> &img, double picked[3])
{
	int index[3];
	if (getIndex(picked, index))
	{
		unsigned char *pixel = manager->GetScalarPointer(index[0], index[1], index[2]);
		*pixel = 255;//把选中的像素点值变为255
	}
}
 
void PointerPickerInteractorStyle::OnRightButtonDown()
{
	this->Interactor->GetPicker()->Pick(this->Interactor->GetEventPosition()[0],
		this->Interactor->GetEventPosition()[1],
		0,  // always zero.
		this->Interactor->GetRenderWindow()->GetRenderers()->GetFirstRenderer());
	double picked[3];
	this->Interactor->GetPicker()->GetPickPosition(picked);
	std::cout << "Picked value: " << picked[0] << " " << picked[1] << " " << picked[2] << std::endl;//vtk选点
	double * cameraPos = this->Interactor->GetRenderWindow()->GetRenderers()->GetFirstRenderer()->GetActiveCamera()->GetPosition();//相机坐标
	double point1[3], point2[3];
	int sourcedims[3];//数据集规模
	double *positionX = new double[10000];
	double *positionY = new double[10000];
	double *positionZ = new double[10000];
	double vectors_1[3];//相机到数据集中心的向量
	double vectors_2[3];//vtk选点到遍历点的向量
	double *pos;

	for (int i = 0; i < 3; i++)
	{
		sourcedims[i] = (manager->getDimensions())[i];
		point1[i] = sourcedims[i] / 2;
		point2[i] = cameraPos[i];
		vectors_1[i] = point2[i] - point1[i];
	}
	int  validVectorCount = -1;
	for (int i = 0; i < sourcedims[0]; i++)//优化了一下机关枪，提了一个函数到common，基本解决玄学选点
	{
		for (int j = 0; j < sourcedims[1]; j++)
		{
			for (int k = 0; k < sourcedims[2]; k++)
			{
				if (*(manager->GetScalarPointer(i, j, k)) >= 90)
				{
					manager->GetPosition(i, j, k, pos);
					for (int l = 0; l < 3; l++)
					{
						vectors_2[l] = pos[l] - picked[l];
					}
					if (judgeisParallel(vectors_1, vectors_2))
					{
						validVectorCount++;
						positionX[validVectorCount] = pos[0];
						positionY[validVectorCount] = pos[1];
						positionZ[validVectorCount] = pos[2];
					}

					delete[] pos;
				}
			}
		}
	}
	double  dis = 9999999999;
	double  point[3];
	double  temp;
	int choosecount = -1;
	for (int i = 0; i <= validVectorCount; i++)
	{

		point[0] = positionX[i];
		point[1] = positionY[i];
		point[2] = positionZ[i];
		temp = DistanceSquare(point2, point);
		//cout << " temp: " << temp << endl;
		if (temp < dis)
		{
			dis = temp;
			choosecount = i;
		}
	}

	cout << "over:" << endl;
	if (choosecount != -1)
	{
		point[0] = positionX[choosecount];
		point[1] = positionY[choosecount];
		point[2] = positionZ[choosecount];
		cout << "choosepoint: " << point[0] << "  " << point[1] << "  " << point[2] << endl;
		vtkSmartPointer<vtkSphereSource> sphereSource =
			vtkSmartPointer<vtkSphereSource>::New();
		sphereSource->Update();

		vtkSmartPointer<vtkPolyDataMapper> mapper =
			vtkSmartPointer<vtkPolyDataMapper>::New();
		mapper->SetInputConnection(sphereSource->GetOutputPort());
		vtkSmartPointer<vtkActor> actor = vtkSmartPointer<vtkActor>::New();
		actor->SetMapper(mapper);
		actor->SetPosition(point);
		actor->SetScale(10);
		actor->GetProperty()->SetColor(1.0, 0.0, 0.0);
		this->Interactor->GetRenderWindow()->GetRenderers()->GetFirstRenderer()->AddActor(actor);//选中点处加红球
		cout << "add ball" << endl;
	}

	delete[] positionX;
	delete[] positionY;
	delete[] positionZ;


	manager->Update();
	vtkInteractorStyleTrackballCamera::OnRightButtonDown();
}
