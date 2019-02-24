#pragma once
#include "Util.h"

#define FIDUCIAL_0 1
#define FIDUCIAL_1 2
#define FIDUCIAL_2 4
//parsing �ؼ� ��ġ��
bool CompPOSX(Point a, Point b);
bool CompPOSY(Point a, Point b);
bool CompPOS2fX(ModelingInfo a, ModelingInfo b);
void CallBackFunc(int event, int x, int y, int flags, void* userdata);
class Modeling {
public:
	Modeling();
	Modeling(vector <ImagePack> &in, ConfigureInfo Configure);
	~Modeling();

	//MergeImage �ȿ� �ִ� ��� src �̹����� �̸��� �޾Ƽ� I10-1-1-RxCy.bmp ���� �߸��̹����� ã�� ���� �з��ؼ� ��Ƶд�.
	Mat deskew(Mat& img, double skew);
	bool LoadModeingINI();
	bool FiducialModeling(bool PreAlign);
	bool PeriScopeModeling();
	bool ChipModeling();
	bool KirkModeling();
	bool ModelingResultFirst(int modeling_result);
	Mat patternMatchingMethod(Mat src, Mat tmpl);
	Mat ParseImage(int img_num, int focus_num, img_type img_type = ORIGINAL);
	Mat ParseImage(int img_num, int focus_num, int R, int C, img_type img_type = ORIGINAL);
	Mat ParseImage(int img_num, int focus_num, int index_num, img_type img_type = ORIGINAL);
	void PrintFunc(string Fn, const Mat in, int Zoom);
	double avg_img(Mat in);
	Point2f PosCenter(Point2f p1, Point2f p2);
	void deskew_all();
	bool calSkewFunc();
	bool MeasureDist();
	Point2d processInside(Mat src, ModelingInfo info);
	Point2d processOutside(Mat src, ModelingInfo info);
	void Thinning(Mat input, int row, int col);
	//������ �о ��ǥ �̵� ������ ����������
	bool readCellInfo();

private:
	vector <ImagePack> IMG;
	vector <Mat> src;
	vector <ModelingInfo> Chip_List;
	vector <ModelingInfo> Outside_List;
	vector <ModelingInfo> Inside_List;
	vector <ModelingInfo> SKEW_List; 
	vector <ModelingInfo> HOUGH_List; 

	ModelingInfo Inside_Bottom;
	ModelingInfo Inside_Top;
	ModelingInfo Outside_Bottom;
	ModelingInfo Outside_Top;
	//Ȯ��� �̹����� �ƴ� ��ü�� �� �Կ��ϴ� �̹���
	Mat Modeling_src;
	char ChkString[300] , TextString[300];
	string log_fname;
	FILE *fp_log, *fp_result, *fp_csv;
	ConfigureInfo LoadConfigure;
	//Z�� Ķ���극�̼ǿ�
	vector<int> adj_focus;
	vector<int> set_focus;
	vector <vector <ModelingInfo>> matching_info;
	clock_t loadClock;
	double clock_result;
	double main_rad;
	Point2f Center;
};


