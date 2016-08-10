#include <iostream>
#include <vector>
#include <cmath>//abs()
#include <memory.h>
#include <windows.h>//BOOL etc.

//filename
#include <ctime>
#include <sstream>
#include <iomanip>

//CImg
#include "..//shared//ExtPatternRecognize.h"
#ifdef _DEBUG
#pragma  comment(lib, "../Debug/hawkvis_D.lib")
#else
#pragma  comment(lib, "../Release/hawkvis.lib")
#endif

using namespace std;

//�����ȼ����ż����Ӧ����
typedef enum GradeSymbol {
	A = 4,
	B = 3,
	C = 2,
	D = 1,
	E = 0
}GradeSymbol;

//����ȼ����������Լ��ȼ�
typedef struct Grade {
	GradeSymbol SC;
	double SC_Score;
	GradeSymbol MOD;

}Grade;

#ifdef _DEBUG

//��ȡ��ǰ��ϵͳʱ���Դ����ļ����������������ʹ�ã�
string NowTimeToFileName(const string preStr, const string suffixalNameStr) {
	static string pastTime;
	time_t t = time(NULL);
	tm tm = *localtime(&t);
	string nowTime;
	stringstream os;
	os.clear();
	os << put_time(&tm, "%y%m%d_%H%M%S");
	nowTime = os.str();
	string fileName;
	static short k = 0;
	if (nowTime != pastTime && (!nowTime.empty())) {//��Ϊ��ȡʱ��ֻ��ȷ���룬���ǳ��������һ��֮�ڴ������ٸ��ļ�������Ҫ���ļ�����������
		fileName = preStr + " - " + nowTime + "_0000" + suffixalNameStr;
		k = 0;
	}
	else {
		char extra[5];
		sprintf_s(extra, sizeof(extra), "%04d", ++k);//
		fileName = preStr + " - " + nowTime + "_" + extra + suffixalNameStr;
	}
	pastTime = nowTime;
	return fileName;
}

//���ݵõ��Ĳ����λ�������񣨽�����������м�����
bool __Func_DrawLine(unsigned char** img, unsigned int left, unsigned int right, const long width, const long height, const unsigned char color) {
	if (img == NULL || right <= left || right - left < 2)
		return false;

	unsigned char** preview = new unsigned char*[height];
	for (unsigned long i = 0; i < height; ++i) {
		preview[i] = new unsigned char[width];
	}

	for (unsigned long i = 0; i < height; ++i) {
		memcpy(preview[i], img[i], width * sizeof(unsigned char));
	}

	for (long i = 0; i < width; ++i) {
		preview[left][i] = (unsigned char)color;
		preview[right][i] = (unsigned char)color;
	}

	CImg * pPreviewImg = create_image();
	pPreviewImg->InitArray8(preview, height, width);
	string filepath = NowTimeToFileName("..//results//bqt//DrawLine", ".bmp");
	pPreviewImg->SaveToFile(filepath.c_str());

	for (unsigned long i = 0; i < height; ++i) {
		delete[] preview[i];
	}
	delete[] preview;
	preview = NULL;

	return true;
}

#endif

//��ͼƬ��X����ӳ�䣬�ۼӻҶ�ֵ
bool Func_Mapping(unsigned char** img, vector<long long > &MX, const long width, const long height) {
	if (img == NULL)
		return false;
	if (width == 0 || height == 0)
		return true;

	for (long j = 0; j < height; ++j) {
		long long sum = 0;
		for (long i = 0; i < width; ++i) {
			sum += img[j][i];
		}
		MX.push_back(sum);
	}
	return true;
}

//��ӳ�����źŲ��
bool Func_MappedDifference(vector<long long > MX, vector<long long > &AMX, long long &MaxOfAMX) {
	if (MX.size() <= 1)
		return false;
	long long prev = *MX.begin();
	long long max = 0;
	for (vector<long long >::iterator p = MX.begin(); p != MX.end(); ++p) {
		if (p == MX.begin())
			continue;
		AMX.push_back(abs(*p - prev));
		if (abs(*p - prev) > max)
			max = abs(*p - prev);
		prev = *p;
	}
	MaxOfAMX = max;

	return true;
}

//��С��Ĩƽ
bool Func_SmoothDifference(vector<long long > &AMX, const long long MaxOfAMX, const float M) {
	if (AMX.size() <= 1 || MaxOfAMX == 0 || M == 0)
		return false;
	for (vector<long long>::iterator p = AMX.begin(); p < AMX.end(); ++p) {
		if (*p < (MaxOfAMX*M)) {
			*p = -1;
		}
	}
	if (*AMX.begin() < 0)
		*AMX.begin() = 1;
	//if (*(AMX.end() - 1) < 0)
	//	*(AMX.end() - 1) = 1;
	return true;
}

//�ҵ���Ȼ���ڵĲ���
bool Func_FindSurvivingPeak(vector<long long > &AMX, vector<unsigned int > &loc) {
	if (AMX.size() <= 1)
		return false;
	if (*AMX.begin() > *(AMX.begin() + 1))
		loc.push_back(0);
	for (vector<long long>::iterator p = AMX.begin() + 1; p < AMX.end() - 1; ++p) {
		if (*p > 0 && *p > *(p - 1) && *p > *(p + 1)) {
			loc.push_back((unsigned int)(p - AMX.begin()));
			//prev = p;
		}
	}
	loc.push_back((unsigned int)((AMX.end() - 1) - AMX.begin()));
	return true;
}

//�ҵ�������Զ�������嶥
bool Func_FindFarmost(vector<unsigned int > loc, unsigned int &left, unsigned int &right) {
	if (loc.size() <= 1)
		return false;
	unsigned int distance = 0;
	for (vector<unsigned int >::iterator p = loc.begin() + 1; p < loc.end(); ++p) {
		if (*p - *(p - 1) > distance) {
			left = *(p - 1) + 5;
			right = *p - 5;
			distance = *p - *(p - 1);
		}
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

	//trans
	for (long j = 0; j < height; ++j) {
		for (long i = 0; i < width; ++i) {
			BYTE* pBuff = pImg->GetPixelAddressRow(j);
			source[j][i] = pBuff[i];
		}
	}


	//action
	vector<long long > MX;//��ͼ��Ҷ�ֵ����ӳ���ۼ�
	Func_Mapping(source, MX, width, height);
	//for each (auto var in MX) {
	//	cout << var << " ";
	//}
	vector<long long > AMX;//���ۼӵĻҶ�ֵ���в������
	long long MaxOfAMX = 0;//AMX�е����ֵ
	Func_MappedDifference(MX, AMX, MaxOfAMX);
	//for each (auto var in AMX) {
	//	cout << var << " ";
	//}
	Func_SmoothDifference(AMX, MaxOfAMX, 0.35);
	cout << endl;
	//for each (auto var in AMX) {
	//	cout << var << " ";
	//}
	vector<unsigned int> loc;//�����λ��
	Func_FindSurvivingPeak(AMX, loc);
	cout << endl;
	//for each (auto var in loc) {
	//	cout << var << " ";
	//}
	unsigned int left = 0, right = height - 1;
	Func_FindFarmost(loc, left, right);
	//cout << "LEFT:" << left << " RIGHT:" << right << endl;
	__Func_DrawLine(source, left, right, width, height, 0xEE);



	//delete
	for (unsigned long i = 0; i < height; ++i) {
		delete[] source[i];
	}
	delete[] source;
	source = NULL;

	return true;
}

//-----�������-----
int main() {
	CImg* pImg = create_image();
	BOOL rt = pImg->AttachFromFile("..//imgs//barcodes//barcode-test-40.bmp");
	if (!rt)
		return -1;

	Grade grade;
	Func(pImg, grade);

	getchar();
	return 0;
}