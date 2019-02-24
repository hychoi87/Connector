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
	ini로 받을때
	img_num : 조명
	focus_num : 포커스
	Rx,Cy : 3.5배 나눠서 찍을때
	ROI : x,y,width,height
	method : 검출 방법
	location : 검출 위치 0 좌상, 1 우상, 2 우하, 3 좌하
	corners[4] = 로테이션된 좌표.
	Angle = 회전된 각도
	box_num = 박스숫자
	ball_num = 볼갯수

	모델링 결과로 보낼때
	img_num : 조명
	focus_num : 포커스
	Rx, Cy : 3.5배용
	x,y,width,height : ROI width, height가 0이면 Point
	method : Fiducial이냐, Periscope냐 kirk냐
	location : 검출 위치 0 좌상, 1 우상, 2 우하, 3 좌하
	corners[4] = 로테이션된 좌표.
	Angle = 회전된 각도
	box_num = 박스숫자
	ball_num = 볼갯수
	*/
	int img_num, focus_num, Rx, Cy, method; // 이건 알지?
	Point2f corners[4];// 기형적인 4각형을 쓰려고 만든것
	Rect ROI; //얼마만큼 ROI를 가져다 쓸 것인가?
	double Angle; //회전 결과
	int box_num; //TOF에서 쓰던 박스이름 아닛 이것도 ID 대신 써도 되자녀
	int ball_num; //이것은 TOF에서 쓰던것
	double th1, th2; //임계치
	double size; // 결과 사이즈를 저장하던것
	int thickness; //두께 두께
	int pre_process; //어떤 프로세싱을 쓸것인가?
	double pre_th1, pre_th2; // 프리프로세싱 th
	int index_num; //아닛? 이걸로 ID해도 됬자녀
	int in_location,out_location; // in_location <- 어느 로케이션으로 결정할것인가? out_location <- 어느로케이션으로 저장할것인가?
	Rect out_ROI; //결과 ROI 저장할거임
	vector<double>list_th1;
	vector<double>list_th2; //
	string input_name; //입력파일 이름?? 뭔 생각으로 만든거지??
	string output_name; //결과파일 이름
	string func_name; // 요걸로 어떤 action을 취할것인가 결정할 예정
	int ID;//요걸로 각 객체 관리할 예정
	Point2f center_pos; //가운데 위치
	Point2f result_pos; //결과 위치
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

//Load 변수들

#define int2str(a) std::to_string((long long)a)
#define float2str(a) std::to_string((long double)a)
#define PosDistance(p1,p2) sqrt((double)((double)(p1.x - p2.x)*(p1.x - p2.x) + (double)(p1.y - p2.y)*(p1.y - p2.y)));
#define PosRadius(p1,p2) (double)atan2f( (p2.x - p1.x), (p2.y - p1.y) ) * 180.0f / 3.1415f ; // -1 보정
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
