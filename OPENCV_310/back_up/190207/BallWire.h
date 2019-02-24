#pragma once
#include "Util.h"

//�ϴ� �̹��� ���� �޾Ƽ�
//parsing �ؼ� ��ġ��
struct KirKBallWireInfo
{
	//3.45�� Kirk �̹������� ������ ������
	int img_num, focus_num, x, y;
};
class BallWire {
public:
	BallWire();
	BallWire(vector <ImagePack> &in, ConfigureInfo Configure, vector<int> &focus, vector<CalibData> &calib);
	~BallWire();

	//MergeImage �ȿ� �ִ� ��� src �̹����� �̸��� �޾Ƽ� I10-1-RxCy.bmp ���� �߸��̹����� ã�� ���� �з��ؼ� ��Ƶд�

	Mat ParseImage(int img_num, int focus_num, int R, int C, img_type img_type = ORIGINAL);
	Mat ParseImage(int img_num, int focus_num, int index_num, img_type img_type = ORIGINAL);
	Mat ParseImage(int img_num, int focus_num, img_type img_type = ORIGINAL);
	void PrintFunc(string Fn, const Mat in, int Zoom);
	double avg_img(Mat in);
	void deskew_all();
	Mat deskew(Mat& img, Point pos, double skew);
	void MaxMin_Func(vector<Mat> In, Mat &MaxImg, Mat &MinImg);
	bool del_by_size(Mat src, Mat &dst, int size);
	Mat patternMatchingMethod(Mat src, Mat tmpl);

	//3.45�� ���� �Լ���
	bool readModelingResultSecond();
	bool LoadBallWireINI35();
	bool kirkBall35();
	bool periBallFind35();
	void processTemplete35(ModelingInfo tmp_input);
	bool ballWireResult35();

	//3�� ���� �Լ���
	bool readModelingResultFirst();
	bool LoadBallWireINI();
	bool kirkBallFind();
	bool periBallFind();
	bool goldBallFind();
	void processTemplete(ModelingInfo tmp_input, ModelingInfo modeling_list);
	bool ballWireResult(int ballwire_result);

	//Ȥ�� ��������??
	void classificationBall(ModelingInfo tmp_input, ModelingInfo modeling_list, int mode);
	double callBallSize(Mat src, Point2f *center, double *radius);
	Point center_of_gravity(Mat src, Point init, bool flag = TRUE);
	int pixel_cnt(Mat src);
	Point2f findBoundaryCenter(Mat in);

	Point2f sizeCalibration(Point2f in, int focus_num);
	double sizeCalibration(double in, int focus_num);
private:
	vector <ImagePack> IMG;
	vector <Mat> src;
	vector <ModelingInfo> peri_modeling_list;
	vector <ModelingInfo> kirk_modeling_list;
	vector <ModelingInfo> Kirk_ball;
	vector <ModelingInfo> kirk35_input_list;
	vector <ModelingInfo> peri35_input_list;
	vector <ModelingInfo> peri_input_list;
	vector <ModelingInfo> gold_input_list;
	vector <ModelingInfo> kirk_input_list; 
	vector <ModelingInfo> kirk_gold_input_list; 
	vector <ModelingInfo> Peri_result_img;

	vector <ModelingInfo> Kirk_List;
	vector <ModelingInfo> Chip_List;
	//��� ��Ī�� �������� ��� ������ ���� �Ҳ���
	vector <vector <ModelingInfo>> pre_matching_ball_info;
	vector <vector <ModelingInfo>> sec_matching_ball_info;
	//Ȯ��� �̹����� �ƴ� ��ü�� �� �Կ��ϴ� �̹���
	Mat BW_src;
	char ChkString[300], TextString[300];
	FILE *fp_log, *fp_result;
	string log_fname;
	ConfigureInfo LoadConfigure;
	double fiducial_angle;
	Point2f fiducial_center;

	//Z�� Calibration 
	vector <int> adj_focus;
	vector <int> set_focus;
	vector <CalibData> calib_data;
};


bool CompLocation(ModelingInfo a, ModelingInfo b);
