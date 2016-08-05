#include <iostream>
#include <vector>
#include <cmath>//abs()
#include <memory.h>
#include <windows.h>//BOOL etc.

#include "..//shared//ExtPatternRecognize.h"

#ifdef _DEBUG
#pragma  comment(lib, "../Debug/hawkvis_D.lib")
#else
#pragma  comment(lib, "../Release/hawkvis.lib")
#endif

using namespace std;

typedef enum GradeSymbol {
	A = 4,
	B = 3,
	C = 2,
	D = 1,
	E = 0
}GradeSymbol;

typedef struct Grade {
	GradeSymbol SC;
	double SC_Score;
	GradeSymbol MOD;
	double MOD_Score;
	GradeSymbol AN;
	double AN_Score;
	GradeSymbol GN;
	double GN_Score;

	Grade() {
		SC = E;		SC_Score = 0;
		MOD = E;	MOD_Score = 0;
		AN = E;		AN_Score = 0;
		GN = E;		GN_Score = 0;
	}

}Grade;

//计算SC值
bool Func_SC(unsigned char* pBuffer, const long width, const long height, double &SC, GradeSymbol &Grade) {
	if (pBuffer == NULL)
		return false;
	if (width == 0 || height == 0)
		return false;
	unsigned long long tab[256];
	memset(tab, 0, 256 * sizeof(unsigned long long));
	for (long i = 0; i < width*height; ++i) {
		++tab[(unsigned int)pBuffer[i]];
	}

	unsigned long long limit = 0.1*width*height;
	unsigned long long count = 0;
	unsigned long long sum = 0;
	for (int i = 0; i < 256; ++i) {
		if (count < limit) {
			if (tab[i] > 0) {
				if (count + tab[i] > limit) {
					unsigned long long temp = limit - count;
					count += temp;
					sum += i*temp;
				}
				else {
					count += tab[i];
					sum += i*tab[i];
				}
			}
			else
				continue;
		}
		else
			break;
	}
	double avglower = (double)sum / count;

	count = 0;
	sum = 0;
	for (int i = 255; i >= 0; --i) {
		if (count < limit) {
			if (tab[i] > 0) {
				if (count + tab[i] > limit) {
					unsigned long long temp = limit - count;
					count += temp;
					sum += i*temp;
				}
				else {
					count += tab[i];
					sum += i*tab[i];
				}
			}
			else
				continue;
		}
		else
			break;
	}
	double avggreater = (double)sum / count;

	SC = avggreater - avglower;

	double score;
	score = 1.0 * SC / avggreater;

	if (score >= 0.7) {
		Grade = A;
	}
	else if (score >= 0.55) {
		Grade = B;
	}
	else if (score >= 0.4) {
		Grade = C;
	}
	else if (score >= 0.2) {
		Grade = D;
	}
	else {
		Grade = E;
	}

	return true;
}

//计算SC值
bool Func_SC(unsigned char** pBuffer, const long width, const long height, double &SC, GradeSymbol &Grade) {
	if (pBuffer == NULL)
		return false;
	if (width == 0 || height == 0)
		return false;
	unsigned long long tab[256];
	memset(tab, 0, 256 * sizeof(unsigned long long));

	for (long i = 0; i < height; ++i) {
		for (long j = 0; j < width; ++j) {
			++tab[(unsigned int)pBuffer[i][j]];
		}
	}

	unsigned long long limit = 0.1*width*height;
	unsigned long long count = 0;
	unsigned long long sum = 0;

	for (int i = 0; i < 256; ++i) {
		if (count < limit) {
			if (tab[i] > 0) {
				if (count + tab[i] > limit) {
					unsigned long long temp = limit - count;
					count += temp;
					sum += i*temp;
				}
				else {
					count += tab[i];
					sum += i*tab[i];
				}
			}
			else
				continue;
		}
		else
			break;
	}
	double avglower = (double)sum / count;

	count = 0;
	sum = 0;
	for (int i = 255; i >= 0; --i) {
		if (count < limit) {
			if (tab[i] > 0) {
				if (count + tab[i] > limit) {
					unsigned long long temp = limit - count;
					count += temp;
					sum += i*temp;
				}
				else {
					count += tab[i];
					sum += i*tab[i];
				}
			}
			else
				continue;
		}
		else
			break;
	}
	double avggreater = (double)sum / count;

	SC = avggreater - avglower;

	double score;
	score = 1.0 * (avggreater - avglower) / avggreater;

	if (score >= 0.7) {
		Grade = A;
	}
	else if (score >= 0.55) {
		Grade = B;
	}
	else if (score >= 0.4) {
		Grade = C;
	}
	else if (score >= 0.2) {
		Grade = D;
	}
	else {
		Grade = E;
	}

	return true;
}

//Sobel算子运算
bool Func_Sobel(unsigned char** img, long** &gx, long** &gy, const long width, const long height) {
	if (img == NULL || gx == NULL || gy == NULL)
		return false;
	unsigned char* r1 = NULL;
	unsigned char* r2 = NULL;
	unsigned char* r3 = NULL;
	for (long i = 0; i < height; ++i) {
		if (i == 0) {
			r1 = img[i];
		}
		else {
			r1 = img[i - 1];
		}
		r2 = img[i];
		if (i == height - 1) {
			r3 = img[i];
		}
		else {
			r3 = img[i + 1];
		}
		if (r1 == NULL || r2 == NULL || r3 == NULL)
			return false;
		gx[i][0] = r1[0] * (-1) + r2[0] * (-2) + r3[0] * (-1) + r1[2] + r2[2] * 2 + r3[2];
		gy[i][0] = r1[0] * (-1) + r1[1] * (-2) + r1[2] * (-1) + r3[0] + r3[1] * 2 + r3[2];
		for (long j = 1; j < width - 1; ++j) {
			gx[i][j] = r1[j - 1] * (-1) + r2[j - 1] * (-2) + r3[j - 1] * (-1) + r1[j + 1] + r2[j + 1] * 2 + r3[j + 1];
			gy[i][j] = r1[j - 1] * (-1) + r1[j] * (-2) + r1[j + 1] * (-1) + r3[j - 1] + r3[j] * 2 + r3[j + 1];
		}
		gx[i][width - 1] = r1[width - 2] * (-1) + r2[width - 2] * (-2) + r3[width - 2] * (-1) + r1[width - 1] + r2[width - 1] * 2 + r3[width - 1];
		gy[i][width - 1] = r1[width - 2] * (-1) + r1[width - 1] * (-2) + r1[width - 1] * (-1) + r3[width - 2] + r3[width - 1] * 2 + r3[width - 1];
	}
	return true;
}

//根据Sobel运算得到的X、Y方向的梯度值计算每个像素的梯度
bool Func_Gradient(long** gx, long** gy, double** &G, const long width, const long height) {
	if (gx == NULL || gy == NULL || G == NULL)
		return false;
	for (long j = 0; j < height; ++j) {
		for (long i = 0; i < width; ++i) {
			long long x = gx[j][i];
			long long y = gy[j][i];
			G[j][i] = sqrt((x * x) + (y * y)) > 255 ? 255 : sqrt((x * x) + (y * y));
		}
	}
	return true;
}

//将运算得到的边缘梯度图的X、Y方向分别映射
bool Func_Mapping(double** G, long long* &mx, long long* &my, const long width, const long height) {
	if (G == NULL || mx == NULL || my == NULL)
		return false;
	for (long j = 0; j < height; ++j) {
		for (long i = 0; i < width; ++i) {
			mx[j] += G[j][i];
			my[i] += G[j][i];
		}
	}
	return true;
}

//将映射后的一维信号进行滤波
bool Func_Gauss(long long* mx, long long* my, long long* &Amx, long long* &Amy, const long width, const long height, const int* model, const int M) {
	if (mx == NULL || my == NULL || Amx == NULL || Amy == NULL)
		return false;
	Amx[0] = (mx[0] * model[3] + mx[1] * model[4] + mx[2] * model[5] + mx[3] * model[6]) / M;
	Amx[1] = (mx[0] * model[2] + mx[1] * model[3] + mx[2] * model[4] + mx[3] * model[5] + mx[4] * model[6]) / M;
	Amx[2] = (mx[0] * model[1] + mx[1] * model[2] + mx[2] * model[3] + mx[3] * model[4] + mx[4] * model[5] + mx[5] * model[6]) / M;
	for (long y = 3; y < height; ++y) {
		Amx[y] = (mx[y - 3] * model[0] + mx[y - 2] * model[1] + mx[y - 1] * model[2] + mx[y] * model[3] + mx[y + 1] * model[4] + mx[y + 2] * model[5] + mx[y + 3] * model[6]) / M;
	}
	Amx[height - 3] = (mx[height - 6] * model[0] + mx[height - 5] * model[1] + mx[height - 4] * model[2] + mx[height - 3] * model[3] + mx[height - 2] * model[4] + mx[height - 1] * model[5]) / M;
	Amx[height - 2] = (mx[height - 5] * model[0] + mx[height - 4] * model[1] + mx[height - 3] * model[2] + mx[height - 2] * model[3] + mx[height - 1] * model[4]) / M;
	Amx[height - 1] = (mx[height - 4] * model[0] + mx[height - 3] * model[1] + mx[height - 2] * model[2] + mx[height - 1] * model[3]) / M;

	Amy[0] = (my[0] * model[3] + my[1] * model[4] + my[2] * model[5] + my[3] * model[6]) / M;
	Amy[1] = (my[0] * model[2] + my[1] * model[3] + my[2] * model[4] + my[3] * model[5] + my[4] * model[6]) / M;
	Amy[2] = (my[0] * model[1] + my[1] * model[2] + my[2] * model[3] + my[3] * model[4] + my[4] * model[5] + my[5] * model[6]) / M;
	for (long x = 3; x < width; ++x) {
		Amy[x] = (my[x - 3] * model[0] + my[x - 2] * model[1] + my[x - 1] * model[2] + my[x] * model[3] + my[x + 1] * model[4] + my[x + 2] * model[5] + my[x + 3] * model[6]) / M;
	}
	Amy[width - 3] = (my[width - 6] * model[0] + my[width - 5] * model[1] + my[width - 4] * model[2] + my[width - 3] * model[3] + my[width - 2] * model[4] + my[width - 1] * model[5]) / M;
	Amy[width - 2] = (my[width - 5] * model[0] + my[width - 4] * model[1] + my[width - 3] * model[2] + my[width - 2] * model[3] + my[width - 1] * model[4]) / M;
	Amy[width - 1] = (my[width - 4] * model[0] + my[width - 3] * model[1] + my[width - 2] * model[2] + my[width - 1] * model[3]) / M;
	return true;
}

//排除滤波后一维信号中的中间干扰点
bool Func_CompareHill(long long* Amx, long long* Amy, vector<long > &Xmaxima, vector<long > &Ymaxima, const long width, const long height) {
	if (Amx == NULL || Amy == NULL || Xmaxima.size() == 0 || Ymaxima.size() == 0)
		return false;

	long ypre = 0, xpre = 0;
	vector<pair<long, long> > Xdifference;
	for each(long y in Xmaxima) {
		long max = 0;
		long d1 = abs(Amx[y - 1] - Amx[y - 2]);
		if (d1 >= max) {
			max = d1;
		}
		long d2 = abs(Amx[y] - Amx[y - 1]);
		if (d2 >= max) {
			max = d2;
		}
		long d3 = abs(Amx[y + 1] - Amx[y]);
		if (d3 >= max) {
			max = d3;
		}
		long d4 = abs(Amx[y + 2] - Amx[y + 1]);
		if (d4 >= max) {
			max = d4;
		}
		pair<long, long> temp;
		temp.first = y;
		temp.second = max;
		Xdifference.push_back(temp);

		//cout << "Y LOCATION:" << y << "\tDISTANCE:" << y - ypre << "\tMAXVALLUE:" << max << endl;
		ypre = y;
	}

	long XMAX = 0;
	for each(auto pa in Xdifference) {
		if (pa.second > XMAX)
			XMAX = pa.second;
	}

	long Xthreshold = XMAX * 15 / 100;
	for each(auto pa in Xdifference) {
		if (pa.second < Xthreshold) {
			Xmaxima.erase(find(Xmaxima.begin(), Xmaxima.end(), pa.first));
		}
	}



	//cout << "------------------------" << endl;

	vector<pair<long, long> > Ydifference;
	for each(long x in Ymaxima) {
		long max = 0;
		long d1 = abs(Amy[x - 1] - Amy[x - 2]);
		if (d1 >= max) {
			max = d1;
		}
		long d2 = abs(Amy[x] - Amy[x - 1]);
		if (d2 >= max) {
			max = d2;
		}
		long d3 = abs(Amy[x + 1] - Amy[x]);
		if (d3 >= max) {
			max = d3;
		}
		long d4 = abs(Amy[x + 2] - Amy[x + 1]);
		if (d4 >= max) {
			max = d4;
		}
		pair<long, long> temp;
		temp.first = x;
		temp.second = max;
		Ydifference.push_back(temp);

		//cout << "X LOCATION:" << x << "\tDISTANCE:" << x - xpre << "\tMAXVALLUE:" << max << endl;
		xpre = x;
	}

	long YMAX = 0;
	for each(auto pa in Ydifference) {
		if (pa.second > YMAX)
			YMAX = pa.second;
	}

	long Ythreshold = YMAX * 15 / 100;
	for each(auto pa in Ydifference) {
		if (pa.second < Ythreshold) {
			Ymaxima.erase(find(Ymaxima.begin(), Ymaxima.end(), pa.first));
		}
	}

	return true;
}

//得到滤波后的一维信号的波峰点位
bool Func_GetMaxima(long long* Amx, long long* Amy, vector<long > &Xmaxima, vector<long > &Ymaxima, const long width, const long height) {
	if (Amx == NULL || Amy == NULL)
		return false;
	for (long y = 2; y < height - 2; ++y) {
		if (Amx[y] >= Amx[y - 1] && Amx[y - 1] >= Amx[y - 2] && Amx[y] > Amx[y + 1] && Amx[y + 1] >= Amx[y + 2])
			Xmaxima.push_back(y);
	}
	//Xmaxima.push_back(height - 1);

	for (long x = 2; x < width - 2; ++x) {
		if (Amy[x] >= Amy[x - 1] && Amy[x - 1] >= Amy[x - 2] && Amy[x] > Amy[x + 1] && Amy[x + 1] >= Amy[x + 2])
			Ymaxima.push_back(x);
	}
	//Ymaxima.push_back(width - 1);
	return true;
}

//根据得到的波峰点位绘制网格（仅供调试输出中间结果）
bool _Func_DrawLine(unsigned char** img, vector<long > Xmaxima, vector<long > Ymaxima, const long width, const long height, const unsigned char color) {
	if (img == NULL || Xmaxima.size() == 0 || Ymaxima.size() == 0)
		return false;
	unsigned char** preview = new unsigned char*[height];
	for (unsigned long i = 0; i < height; ++i) {
		preview[i] = new unsigned char[width];
	}

	for (unsigned long i = 0; i < height; ++i) {
		memcpy(preview[i], img[i], width * sizeof(unsigned char));
	}

	for each(long y in Xmaxima) {
		for (long i = 0; i < width; ++i) {
			preview[y][i] = (unsigned char)color;
		}
	}

	for each(long x in Ymaxima) {
		for (long j = 0; j < height; ++j) {
			preview[j][x] = (unsigned char)color;
		}
	}
	CImg * pPreviewImg = create_image();
	pPreviewImg->InitArray8(preview, height, width);
	pPreviewImg->SaveToFile("..//results//dqt//DrawLine.bmp");

	for (unsigned long i = 0; i < height; ++i) {
		delete[] preview[i];
	}
	delete[] preview;
	preview = NULL;

	return true;
}

//求得每个信息点的像素值均值
bool Func_ModuleAvg(unsigned char** img, vector<long > Xmaxima, vector<long > Ymaxima, vector<vector<unsigned char > > &avr, const long width, const long height) {
	if (img == NULL || Xmaxima.size() == 0 || Ymaxima.size() == 0)
		return false;
	long Xpre = 0, Ypre = 0;
	long long sum = 0;
	long count = 0;
	vector<unsigned char > temp;
	for each(long j in Xmaxima) {
		for each(long i in Ymaxima) {
			for (long y = Ypre; y < j; ++y) {
				for (long x = Xpre; x < i; ++x) {
					++count;
					sum += img[y][x];
				}
			}
			Xpre = i + 1;
			if (count != 0) {
				temp.push_back((unsigned char)(sum / count));
				sum = 0;
				count = 0;
			}
			else {
				sum = 0;
				count = 0;
				continue;
			}
		}
		Ypre = j + 1;
		avr.push_back(temp);
		temp.clear();
	}
	return true;
}

//求全图最小MOD值
bool Func_Modulation(vector<vector<unsigned char > > R, const unsigned char GT, const unsigned char SC, double &mod, GradeSymbol &grade) {
	if (R.size() == 0 || SC == 0)
		return false;
	double min = 1;
	for (vector<vector<unsigned char > >::iterator p = R.begin() + 1; p != prev(R.end()); ++p) {
		for (vector<unsigned char >::iterator pp = p->begin() + 1; pp != prev(p->end()); ++pp) {
			double temp = 2.00 * (double)abs(*pp - GT) / (double)SC;
			if (temp <= min) {
				min = temp;
			}
		}
	}

	mod = min;
	if (min >= 0.5) {
		grade = A;
	}
	else if (min >= 0.4) {
		grade = B;
	}
	else if (min >= 0.3) {
		grade = C;
	}
	else if (min >= 0.2) {
		grade = D;
	}
	else {
		grade = E;
	}

	return true;
}

//求AN值
bool Func_AN(vector<long > Xmaxima, vector<long > Ymaxima, double &score, GradeSymbol &grade) {
	if (Xmaxima.size() == 0 || Ymaxima.size() == 0)
		return false;
	double xavg = (*prev(Ymaxima.end()) - *Ymaxima.begin())*1.0 / (Ymaxima.size() - 1);
	double yavg = (*prev(Xmaxima.end()) - *Xmaxima.begin())*1.0 / (Xmaxima.size() - 1);

	score = abs(xavg - yavg) / ((xavg + yavg)*2.0);

	if (score > 0.12) {
		grade = E;
	}
	if (score <= 0.12) {
		grade = D;
	}
	if (score <= 0.1) {
		grade = C;
	}
	if (score <= 0.08) {
		grade = B;
	}
	if (score <= 0.06) {
		grade = A;
	}

	return true;
}

//求网格偏差GN
bool Func_GN(vector<long > Xmaxima, vector<long > Ymaxima, double &score, GradeSymbol &grade) {
	double xavg = (*prev(Ymaxima.end()) - *Ymaxima.begin())*1.0 / (Ymaxima.size() - 1);
	double yavg = (*prev(Xmaxima.end()) - *Xmaxima.begin())*1.0 / (Xmaxima.size() - 1);
	//int xsize = xavg + 0.5, ysize = yavg + 0.5;

	vector<double > Xdistance, Ydistance;
	for (int i = 0; i < (Ymaxima.size() - 1); ++i) {
		//Xideal.push_back(i*xsize);
		Xdistance.push_back(abs((Ymaxima[i] - Ymaxima[0]) - 1.0*i*xavg));
	}
	for (int i = 0; i < (Xmaxima.size() - 1); ++i) {
		//Yideal.push_back(i*ysize);
		Ydistance.push_back(abs((Xmaxima[i] - Xmaxima[0]) - 1.0*i*yavg));
	}

	double xmaxdistance = 0;
	for each(auto p in Xdistance) {
		if (p > xmaxdistance)
			xmaxdistance = p;
	}

	double ymaxdistance = 0;
	for each(auto p in Ydistance) {
		if (p > ymaxdistance)
			ymaxdistance = p;
	}
	
	score = sqrt((xmaxdistance*xmaxdistance) + (ymaxdistance*ymaxdistance)) / xavg/*sqrt((xavg*xavg) + (yavg*yavg))*/;

	if (score > 0.75) {
		grade = E;
	}
	if (score <= 0.75) {
		grade = D;
	}
	if (score <= 0.63) {
		grade = C;
	}
	if (score <= 0.50) {
		grade = B;
	}
	if (score <= 0.38) {
		grade = A;
	}

	return true;
}

bool Func(CImg* pImg, Grade &grade) {
	if (pImg == NULL)
		return false;

	long width;
	long height;

	width = pImg->GetWidthPixel();
	height = pImg->GetHeight();

	//new

	unsigned char** source = new unsigned char*[height];
	for (unsigned long i = 0; i < height; ++i) {
		source[i] = new unsigned char[width];
	}

	long** gx = new long*[height];
	for (unsigned long i = 0; i < height; ++i) {
		gx[i] = new long[width];
	}

	long** gy = new long*[height];
	for (unsigned long i = 0; i < height; ++i) {
		gy[i] = new long[width];
	}

	double** gradient = new double*[height];
	for (unsigned long i = 0; i < height; ++i) {
		gradient[i] = new double[width];
	}

	long long *mappedx = new long long[height];
	long long *mappedy = new long long[width];
	memset(mappedx, 0, sizeof(long long)*height);
	memset(mappedy, 0, sizeof(long long)*width);

	long long *aftermx = new long long[height];
	long long *aftermy = new long long[width];
	memset(aftermx, 0, sizeof(long long)*height);
	memset(aftermy, 0, sizeof(long long)*width);

	long long *remx = new long long[height];
	long long *remy = new long long[width];
	memset(remx, 0, sizeof(long long)*height);
	memset(remy, 0, sizeof(long long)*width);

	//trans
	for (long j = 0; j < height; ++j) {
		for (long i = 0; i < width; ++i) {
			BYTE* pBuff = pImg->GetPixelAddressRow(j);
			source[j][i] = pBuff[i];
		}
	}

	//action

	bool rt = true;
	rt = Func_SC(source, width, height, grade.SC_Score, grade.SC);
	if (rt == false)
		return false;
	//cout << SC << endl;
	rt = Func_Sobel(source, gx, gy, width, height);
	if (rt == false)
		return false;
	rt = Func_Gradient(gx, gy, gradient, width, height);
	if (rt == false)
		return false;
	rt = Func_Mapping(gradient, mappedx, mappedy, width, height);
	if (rt == false)
		return false;
	const int model[7] = { 5,10,45,50,45,10,5 };
	const int M = 100;
	rt = Func_Gauss(mappedx, mappedy, aftermx, aftermy, width, height, model, M);
	//const int model2[7] = { 0,0,10,20,10,0,0 };
	//const int M2 = 100;
	//rt = Func_Gauss(aftermx, aftermy, remx, remy, width, height, model2, M2);
	if (rt == false)
		return false;
	vector<long > Xmaxima, Ymaxima;
	rt = Func_GetMaxima(aftermx, aftermy, Xmaxima, Ymaxima, width, height);
	if (rt == false)
		return false;
	rt = Func_CompareHill(aftermx, aftermy, Xmaxima, Ymaxima, width, height);
	if (rt == false)
		return false;
	rt = _Func_DrawLine(source, Xmaxima, Ymaxima, width, height, 0xDD);
	if (rt == false)
		return false;
	vector<vector<unsigned char > > avr;
	rt = Func_ModuleAvg(source, Xmaxima, Ymaxima, avr, width, height);
	if (rt == false)
		return false;
	//for (vector<vector<unsigned char > >::iterator p = avr.begin(); p != avr.end(); ++p) {
	//	for (vector<unsigned char >::iterator pp = p->begin(); pp != p->end(); ++pp) {
	//		cout << (unsigned int)*pp << " ";
	//	}
	//	cout << endl;
	//}

	rt = Func_Modulation(avr, 127, grade.SC_Score, grade.MOD_Score, grade.MOD);
	if (rt == false)
		return false;

	rt = Func_AN(Xmaxima, Ymaxima, grade.AN_Score, grade.AN);
	if (rt == false)
		return false;

	rt = Func_GN(Xmaxima, Ymaxima, grade.GN_Score, grade.GN);
	if (rt == false)
		return false;

	//cout << "MOD:" << MOD << "\nGrade:" << Grade << endl;

	//delete
	delete mappedx;
	mappedx = NULL;
	delete mappedy;
	mappedy = NULL;
	delete aftermx;
	aftermx = NULL;
	delete aftermy;
	aftermy = NULL;


	for (unsigned long i = 0; i < height; ++i) {
		delete[] source[i];
	}
	delete[] source;
	source = NULL;

	for (unsigned long i = 0; i < height; ++i) {
		delete[] gx[i];
	}
	delete[] gx;
	gx = NULL;

	for (unsigned long i = 0; i < height; ++i) {
		delete[] gy[i];
	}
	delete[] gy;
	gy = NULL;

	for (unsigned long i = 0; i < height; ++i) {
		delete[] gradient[i];
	}
	delete[] gradient;
	gradient = NULL;


	return true;
}

int main() {
	CImg* pImg = create_image();
	//BOOL rt = pImg->AttachFromFile("..//imgs//2-1-0.bmp");
	BOOL rt = pImg->AttachFromFile("..//imgs//code-test-10.bmp");
	if (!rt)
		return -1;

	Grade Gr;

	bool rt1 = true;

	rt1 = Func(pImg, Gr);

	if (rt1) {
		cout << "\nSC_Score = " << Gr.SC_Score << "\tSC_Grade = " << Gr.SC << endl;
		cout << "MOD_Score = " << Gr.MOD_Score << "\tMOD_Grade = " << Gr.MOD << endl;
		cout << "AN_Score = " << Gr.AN_Score << "\tAN_Grade = " << Gr.AN << endl;
		cout << "GN_Score = " << Gr.GN_Score << "\tGN_Grade = " << Gr.GN << endl;
	}
	else
		return -2;

	getchar();
	return 0;
}
