#pragma once
#include "Çì´õ.h"

#define GREEN_FOCUS


extern double dft_radius;
extern double dft_thresh;
extern double dft_thresh_min;
extern double dft_thresh_max;
extern double adap_th1;
extern double adap_th2;
extern double canny_th1;
extern double canny_th2;
extern double back_th;



//void bitwise_mask(const Mat src, const Mat mask, Mat &dst);
//void dft_background_remove(Mat src, Mat &dst, double radius_ratio, double thresh_ratio);
void find_defect(vector<Mat> &src, vector<Mat> &dst, Rect ROI);
Rect find_ROI(Mat obj_img, Mat src_img, Mat dst, Rect ROI);
void PrintFunc(string Fn, const Mat in, int Zoom);
void Panorama(vector<Mat> &src, Mat dst);
string intToString(int n);
double img_std(Mat src, int th);
double avg_img(Mat src);
double avg_img_th(Mat src, unsigned char th);