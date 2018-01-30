#include "PointerPickerInteractorStyle.h"
#include"common.h"
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
	double picked[3];//拾取的点的坐标
	this->Interactor->GetPicker()->GetPickPosition(picked);
	double * cameraPos = this->Interactor->GetRenderWindow()->GetRenderers()->GetFirstRenderer()->GetActiveCamera()->GetPosition();//相机的坐标

	double point1[3], point2[3];
	int sourcedims[3];//数据集规模
	for (int i = 0; i < 3; i++)
	{
		sourcedims[i] = (manager->getDimensions())[i];
		point1[i] = sourcedims[i] / 2;//数据集中心坐标
		point2[i] = cameraPos[i];
	}

	int zerojudge[3] = { 0,0,0 };//难以描述 判断三点一线用到的
	int vectorjudge;
	int verticalVectorCount = 0;
	double  precision = 0.05;//设置判断三点一线的精度
	double *positionX = new double[1000];//选中有条件点的坐标
	double *positionY = new double[1000];
	double *positionZ = new double[1000];


	int validVectorCount = -1;
	double vectors_1[3];//比中心到相机和遍历点与拾取点的向量是否平行
	double vectors_2[3];

	for (int i = 0; i < 3; i++)
	{
		vectors_1[i] = point1[i] - point2[i];
		if (vectors_1[i] <= 1 && vectors_1[i] >= -1)
		{
			zerojudge[i] = 1;
			verticalVectorCount += 1;
		}
	}
	double ratio[3];
	double *pos;
	////////////////////////////////////////////////////////////////////

	// 傅川溥 优化机关枪 八叉树思想
	int start[3] = {0,0,0};
	//遍历的起始，终止在sourcedims中，通过不断改变来优化

	for (int i = start[0]; i < sourcedims[0]; i++)//机关枪 = =  里面别管了
	{
		for (int j = start[1]; j < sourcedims[1]; j++)
		{
			for (int k = start[2]; k < sourcedims[2]; k++)
			{
				vectorjudge = 1;
				manager->GetPosition(i, j, k, pos);
				if (*(manager->GetScalarPointer(i, j, k)) >= 90)
				{
					for (int l = 0; l < 3; l++)
					{
						vectors_2[l] = pos[l] - picked[l];
						if (zerojudge[l] == 1)
						{
							if (vectors_2[l] >= 1 || vectors_2[l] <= -1)
							{
								vectorjudge = 0;
								break;
							}
						}
						else
						{
							ratio[l] = vectors_1[l] / vectors_2[l];
						}
					}
					if (!vectorjudge)
					{
						delete[] pos;
						continue;
					}
					if (verticalVectorCount == 0)
					{
						for (int l = 0; l < 3; l++)
						{
							if (ratio[l] - ratio[(l + 1) % 3] <= -1 * precision || ratio[l] - ratio[(l + 1) % 3] >= precision)
							{
								vectorjudge = 0;
								break;
							}
						}
					}
					else
					{
						if (validVectorCount == 1)
						{
							for (int l = 0; l < 3; l++)
							{
								if (zerojudge[l] == 1)
								{
									if (ratio[(l + 1) % 3] - ratio[(l + 2) % 3] <= -1 * precision || ratio[(l + 1) % 3] - ratio[(l + 2) % 3] >= precision)
									{
										vectorjudge = 0;
										break;
									}
								}
							}
						}
					}

					if (vectorjudge)
					{
						//表示搜索到了
						//int mid = (start[0] + sourcedims[0]) / 2;
						int mid = (start[0] + sourcedims[0]) / 2;
						if (i > mid)
						{
							i = mid + 1;
							start[0] = i;
						}
						else
						{
							sourcedims[0] = mid - 1;
						}

						mid = (start[1] + sourcedims[1]) / 2;
						if (j > mid)
						{
							j = mid + 1;
							start[1] = j;
						}
						else
						{
							sourcedims[1] = mid - 1;
						}

						mid = (start[2] + sourcedims[2]) / 2;
						if (k > mid)
						{
							k = mid + 1;
							start[2] = k;
						}
						else
						{
							sourcedims[2] = mid - 1;
						}

						validVectorCount++;
						positionX[validVectorCount] = pos[0];
						positionY[validVectorCount] = pos[1];
						positionZ[validVectorCount] = pos[2];
					}
				}
				delete[] pos;
			}
		}
	}

	////////////////////////////////////////////////////////////////////

	double  dis = 9999999999;
	double  point[3];
	double  temp;
	int choosecount = -1;
	for (int i = 0; i <= validVectorCount; i++)//找出与相机最近的点
	{
		point[0] = positionX[i];
		point[1] = positionY[i];
		point[2] = positionZ[i];
		temp = DistanceSquare(point2, point);
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
		actor->SetScale(50);
		actor->GetProperty()->SetColor(1.0, 0.0, 0.0);
		this->Interactor->GetRenderWindow()->GetRenderers()->GetFirstRenderer()->AddActor(actor);
		cout << "add ball" << endl;//在算出的点加一个球
	}

	delete[] positionX;
	delete[] positionY;
	delete[] positionZ;
	manager->Update();
	vtkInteractorStyleTrackballCamera::OnRightButtonDown();
}
