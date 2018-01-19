#pragma once
bool judgePointOnTheLine(double *a, double *b, double *pos)   //�����Ƿ�����һ�ߣ���ʱû���ϣ�
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

double  DistanceSquare(double *a, double *b)//�����������ƽ��
{
	double dis = 0;
	for (int i = 0; i < 3; i++)
	{
		dis += (a[i] - b[i])*(a[i] - b[i]);
	}
	return dis;
}
