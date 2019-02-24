#pragma once
#ifndef _UTIL_H_
#define _UTIL_H_


#include <opencv/cv.h>
#include <opencv/cxcore.h>
#include <opencv/highgui.h>
#include <opencv2/opencv.hpp>
#include <opencv2/stitching.hpp>
#include "opencv2/imgcodecs.hpp" 
#include <vector>
#include <iostream>
#include <sstream>
#include "stdio.h"
#include "time.h"
#include <cmath>
#include <windows.h>
#include <direct.h>
#include <string>
#include <thread>
#include <io.h>
#include <conio.h>
#include <fstream>
#include <omp.h>

using namespace cv;
using namespace std;

struct ModelingInfo
{
	//
	/*
	ini�� ������
	img_num : ����
	focus_num : ��Ŀ��
	Rx,Cy : 3.5�� ������ ������
	ROI : x,y,width,height
	method : ���� ���
	location : ���� ��ġ 0 �»�, 1 ���, 2 ����, 3 ����
	corners[4] = �����̼ǵ� ��ǥ.
	Angle = ȸ���� ����
	box_num = �ڽ�����
	ball_num = ������

	�𵨸� ����� ������
	img_num : ����
	focus_num : ��Ŀ��
	Rx, Cy : 3.5���
	x,y,width,height : ROI width, height�� 0�̸� Point
	method : Fiducial�̳�, Periscope�� kirk��
	location : ���� ��ġ 0 �»�, 1 ���, 2 ����, 3 ����
	corners[4] = �����̼ǵ� ��ǥ.
	Angle = ȸ���� ����
	box_num = �ڽ�����
	ball_num = ������
	*/
	int img_num, focus_num, Rx, Cy, method; // �̰� ����?
	Point2f corners[4];// �������� 4������ ������ �����
	Rect ROI; //�󸶸�ŭ ROI�� ������ �� ���ΰ�?
	double Angle; //ȸ�� ���
	int box_num; //TOF���� ���� �ڽ��̸� �ƴ� �̰͵� ID ��� �ᵵ ���ڳ�
	int ball_num; //�̰��� TOF���� ������
	double th1, th2; //�Ӱ�ġ
	double size; // ��� ����� �����ϴ���
	int thickness; //�β� �β�
	int pre_process; //� ���μ����� �����ΰ�?
	double pre_th1, pre_th2; // �������μ��� th
	int index_num; //�ƴ�? �̰ɷ� ID�ص� ���ڳ�
	int in_location,out_location; // in_location <- ��� �����̼����� �����Ұ��ΰ�? out_location <- ��������̼����� �����Ұ��ΰ�?
	Rect out_ROI; //��� ROI �����Ұ���
	vector<double>list_th1;
	vector<double>list_th2; //
	string input_name; //�Է����� �̸�?? �� �������� �������??
	string output_name; //������� �̸�
	string func_name; // ��ɷ� � action�� ���Ұ��ΰ� ������ ����
	int ID;//��ɷ� �� ��ü ������ ����
	Point2f center_pos; //��� ��ġ
	Point2f result_pos; //��� ��ġ
	vector<int> input_ID;
};

struct ImagePack {
	Mat Original;
	Mat Gray, R, G, B;
	string ImageName;
};

struct ConfigureInfo {
	string ProductName;
	string InputFileType, OutputFileType;
	string input_directory;
	int ResultDisplay;
	int IsSimulator;
	int job_code;
	int index_num;
};
struct CalibData {
	Mat transe_mat;
	double scale_x, scale_y, shift_x, shift_y;
	double pitch_x;
};

const Mat kernel(3, 3, CV_8UC1, Scalar(255));

#define	cVersion "1.00"
#define	nonMouse "Non MouseEvent"
#define useMouse "Mouse Event"

#define PRINT_DEBUG 

#ifdef PRINT_DEBUG
#else
#pragma comment(linker, "/SUBSYSTEM:windows /ENTRY:mainCRTStartup")
#endif

//Load ������

#define int2str(a) std::to_string((long long)a)
#define float2str(a) std::to_string((long double)a)
#define PosDistance(p1,p2) sqrt((double)((double)(p1.x - p2.x)*(p1.x - p2.x) + (double)(p1.y - p2.y)*(p1.y - p2.y)));
#define PosRadius(p1,p2) (double)atan2f( (p2.x - p1.x), (p2.y - p1.y) ) * 180.0f / 3.1415f ; // -1 ����
enum img_type {ORIGINAL, GRAY, RED,GREEN,BLUE};
enum location_type { NONE, FIDUCIAL_MODE = 1, PERISCOPE_MODE, KIRK_MODE, CHIP_MODE };
enum pre_process_type { BLUR_MODE = 1, MEDIANBLUR_MODE, DFT_MODE };
enum ball_wire_type { PERI_BALL, GOLD_BALL, KIRK_BALL };
enum find_mode { LINE_MODE,CIRCLE_MODE  };
enum method_type { HARD_MASKING_MODE, THRESHOLD_MODE, THRESHOLD_INV_MODE, ADAPTHRESHOLD_MODE, ADAPTHRESHOLD_INV_MODE, CANNY_MODE, OTSU_CANNY_MODE, TEMPLETE_MATCHING_MODE,PCA_MODE,OTSU_MODE, OTSU_INV_MODE};

enum DIRECT { TOP = 0, BOTTOM };
enum defect_code {
	DEFECT_MODELING_FIDUCIAL = (1 << 0),
	DEFECT_MODELING_PERISCOPE = (1 << 1),
	DEFECT_MODELING_CHIP = (1 << 2),
	DEFECT_MODELING_KIRK = (1 << 3),

	DEFECT_PLACEMENT_PERISCOPE = (1 << 4),
	DEFECT_PLACEMENT_KIRK = (1 << 5),
	DEFECT_ROTATION_PERISCOPE = (1 << 6),
	DEFECT_ROTATION_KIRK = (1 << 7),

	DEFECT_DEFECT_PERISCOPE = (1 << 8),
	DEFECT_DEFECT_CHIP = (1 << 9),
	DEFECT_DEFECT_KIRK = (1 << 10),

	DEFECT_BALL_PERISCOPE = (1 << 11),
	DEFECT_BALL_KIRK = (1 << 12),
	DEFECT_WIRE_PERISCOPE = (1 << 13),
	DEFECT_WIRE_KIRK = (1 << 14)
	//DEFECT_PLACEMENT = (1 << 15)
};

#endif // !_UTIL_H_
