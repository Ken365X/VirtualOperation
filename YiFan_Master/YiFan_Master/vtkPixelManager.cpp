#include "vtkPixelManager.h"
vtkPixelManager::vtkPixelManager()
{
	positionX = NULL;
	positionY = NULL;
	positionZ = NULL;
	pixel = NULL;
	source = NULL;
	dims = new int[3];
}


vtkPixelManager::~vtkPixelManager()
{
	deleteData();
}

void vtkPixelManager::deleteData()
{
	if (positionX)
	{
		delete[] positionX;
		delete[] positionY;
		delete[] positionZ;
	}
	if (pixel)
	{
		delete[] pixel;
	}
	delete[] dims;
}

void vtkPixelManager::SetInputData(vtkImageData *source)
{
	this->source = source;
}

int* vtkPixelManager::getDimensions()
{
	return dims;
}

unsigned char *    vtkPixelManager::GetScalarPointer(int x, int y, int z)
{
	return pixel + z + y*dims[2] + x*dims[1] * dims[2];
}

void vtkPixelManager::GetPosition(int x, int y, int z, double *&pos)
{
	pos = new double[3];
	pos[0] = positionX[z + y*dims[2] + x*dims[1] * dims[2]];
	pos[1] = positionY[z + y*dims[2] + x*dims[1] * dims[2]];
	pos[2] = positionZ[z + y*dims[2] + x*dims[1] * dims[2]];
}
void  vtkPixelManager::initial()
{
	deleteData();
	if (source)
	{
		source->GetDimensions(dims);
		int count = dims[0] * dims[1] * dims[2];
		positionX = new double[count];
		positionY = new double[count];
		positionZ = new double[count];
		pixel = new unsigned char[count];
		for (int i = 0; i < dims[0]; i++)
		{
			for (int j = 0; j < dims[1]; j++)
			{
				for (int k = 0; k < dims[2]; k++)
				{
					int tempNumber = k + i* dims[1] * dims[2] + j*dims[2];
					unsigned char *tempPixel = (unsigned char *)source->GetScalarPointer(i, j, k);
					pixel[tempNumber] = *tempPixel;
					positionX[tempNumber] = i;
					positionY[tempNumber] = j;
					positionZ[tempNumber] = k;
				}
			}
		}
	}
}
void  vtkPixelManager::Update()
{
	if (source)
	{
		for (int i = 0; i < dims[0]; i++)
		{
			for (int j = 0; j < dims[1]; j++)
			{
				for (int k = 0; k < dims[2]; k++)
				{
					unsigned char *tempPixel = (unsigned char *)source->GetScalarPointer(i, j, k);
					*tempPixel = 0;
				}
			}
		}//先清空一遍数据
		for (int i = 0; i < dims[0]; i++)
		{
			for (int j = 0; j < dims[1]; j++)
			{
				for (int k = 0; k < dims[2]; k++)
				{
					int tempNumber = k + i* dims[1] * dims[2] + j*dims[2];
					unsigned char *tempPixel = (unsigned char *)source->GetScalarPointer((int)positionX[tempNumber],
						(int)positionY[tempNumber], (int)positionZ[tempNumber]);
					*tempPixel = pixel[tempNumber];
				}
			}
		}//录入数据
	}
}

