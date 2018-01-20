#pragma once
bool judgePointOnTheLine(double *a, double *b, double *pos)   //计算是否三点一线（暂时没用上）
{
	double vectors[3];
	for (int i = 0; i < 3; i++)
	{
		vectors[i] = b[i] - a[i];
	}
	double ratio[3];
	for (int i = 0; i < 3; i++)
	{
		ratio[i] = (pos[i] - a[i]) / vectors[i];
	}
	for (int i = 0; i < 3; i++)
	{
		if ((ratio[i] - ratio[(i + 1) % 3]) <= -0.05 || (ratio[i] - ratio[(i + 1) % 3]) >= 0.05)
		{
			return false;
		}
	}
	return true;
}

double  DistanceSquare(double *a, double *b)//计算两点距离平方
{
	double dis = 0;
	for (int i = 0; i < 3; i++)
	{
		dis += (a[i] - b[i])*(a[i] - b[i]);
	}
	return dis;
}


bool  judgeisParallel(double *vector_1, double *vector_2)//计算是否平行两个向量
{
	double  precision = 0.001;
	int zeroCount = 0;
	double ratio[3];
	for (int i = 0; i < 3; i++)
	{
		if (vector_1[i] < 1 && vector_1[i] > -1)
		{
			if (vector_2[i] < 1 && vector_2[i] > -1)
			{
				zeroCount++;
			}
			else
			{
				return false;
			}
		}
	}
	switch (zeroCount)
	{
	case 0:
		ratio[0] = vector_2[0] / vector_1[0];
		ratio[1] = vector_2[1] / vector_1[1];
		ratio[2] = vector_2[2] / vector_1[2];
		for (int i = 0; i < 2; i++)
		{
			if ((ratio[i] - ratio[(i + 1) % 3]) >= precision || (ratio[i] - ratio[(i + 1) % 3]) <= -1 * precision)
			{
				return false;
			}
		}
		return true;
	case 1:
		for (int i = 0; i < 3; i++)
		{
			if (vector_1[i] < 1 && vector_1[i] > -1)
			{
				ratio[0] = vector_2[(i + 1) % 3] / vector_1[(i + 1) % 3];
				ratio[1] = vector_2[(i + 2) % 3] / vector_1[(i + 2) % 3];
				if ((ratio[0] - ratio[1]) > -1 * precision && (ratio[0] - ratio[1]) < precision)
				{
					return true;
				}
				else
				{
					return false;
				}
			}
		}
	default:
		return true;
	}
}