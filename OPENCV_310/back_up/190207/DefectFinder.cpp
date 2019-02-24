
#include "헤더.h"

double dft_radius;
double dft_thresh;
double dft_thresh_min;
double dft_thresh_max;
double back_th;

double black_adap_th1;
double black_adap_th2;
double black_canny_th1;
double black_canny_th2;

double black_invisible_adap_th1;
double black_invisible_adap_th2;
double black_invisible_canny_th1;
double black_invisible_canny_th2;

double invisible_adap_th1;
double invisible_adap_th2;
double invisible_canny_th1;
double invisible_canny_th2;

double white_invisible_adap_th1;
double white_invisible_adap_th2;
double white_invisible_canny_th1;
double white_invisible_canny_th2;

double white_adap_th1;
double white_adap_th2;
double white_canny_th1;
double white_canny_th2;

double ledoff_adap_th1;
double ledoff_adap_th2;
double ledoff_canny_th1;
double ledoff_canny_th2;

double adap_th1;
double adap_th2;
double canny_th1;
double canny_th2;

double log_DV;
double log_EV;
double log_SV;
double log_RV;
double log_BF;

const Mat kernel(3, 3, CV_8UC1, Scalar(255));
enum th_num { INVISIBLE_TH = 73, WHITE_TH = 90, };
enum defect_type { OLED_DEFECT, NORMAL_DEFECT, SCRATCH_DEFECT, LINEAR_DEFECT, BLACK_WHITE_DEFECT };
enum color { BLACK = 1, WHITE = 2, INVISIBLE = 9 };
enum BACKLIGHTONOFF { OFF = 0, ON = 1 };

static void getThreshold(int, void*)
{

}

void MaxMin_Func(vector<Mat> In, Mat &MaxImg, Mat &MinImg) {

	int row;
	int col;
	int size;
	int x, y, top;

	size = In.size();
	row = In[0].rows;
	col = In[0].cols;
	MaxImg = Mat::zeros(row, col, CV_8U);
	MinImg = Mat::zeros(row, col, CV_8U);
	unsigned char *MINST, *MAXST;
	//#pragma omp parallel for
	for (y = 0; y < row; y++) {
		for (x = 0; x < col; x++) {
			vector<unsigned char> sortVal;
			for (int i = 0; i < size; i++) {
				sortVal.push_back(In[i].at<uchar>(y, x));
			}
			std::sort(sortVal.begin(), sortVal.end());
			MinImg.at<uchar>(y, x) = sortVal[0];
			MaxImg.at<uchar>(y, x) = sortVal[sortVal.size() - 1];
		}
	}

}

int pixel_cnt(Mat in, int th = 0)
{
	Mat src = in.clone();
	int w = src.size().width;

	int h = src.size().height;



	unsigned char* data = (unsigned char*)src.data;



	double avg = 0.0;

	int num = 0;

	for (int y = 0; y < h - 1; y++)
	{
		for (int x = 0; x < w - 1; x++)
		{
			if (th < (int)data[y *  src.cols + x]) {
				//avg = (avg*(num-1) + (double)data[y *  src.cols + x]) /num;
				num++;
			}
		}
	}
	return num;
}

double avg_img(Mat in)
{
	Mat src = in.clone();

	int w = src.size().width;

	int h = src.size().height;



	//unsigned char* data = (unsigned char*)src.data;
	//
	//
	//
	double avg = 0.0;
	//
	int num = 1;
	//
	//for (int y = 0; y < h - 1; y++)
	//{
	//	for (int x = 0; x < w - 1; x++)
	//	{
	//		avg = (avg*(num - 1) + (double)data[y *  src.cols + x]) / num;
	//		num++;
	//	}
	//}

	if (src.depth() == CV_32F) {
		float* data = (float*)src.data;



		for (int y = 0; y < h - 0; y++)
		{
			for (int x = 0; x < w - 0; x++)
			{
				avg = (avg*(num - 1) + (double)data[y *  src.cols + x]) / (double)num;
				num++;
			}
		}
	}
	else if (src.depth() == CV_8U)
	{
		unsigned char* data = (unsigned char*)src.data;



		for (int y = 0; y < h - 0; y++)
		{
			for (int x = 0; x < w - 0; x++)
			{
				avg = (avg*(num - 1) + (double)data[y *  src.cols + x]) / (double)num;
				num++;
			}
		}
	}
	else if (src.depth() == CV_64F)
	{
		double* data = (double*)src.data;



		for (int y = 0; y < h - 0; y++)
		{
			for (int x = 0; x < w - 0; x++)
			{
				avg = (avg*(num - 1) + (double)data[y *  src.cols + x]) / (double)num;
				num++;
			}
		}
	}


	return avg;
}

double avg_img_th(Mat in, unsigned char th)

{

	Mat src = in.clone();

	int w = src.size().width;

	int h = src.size().height;



	unsigned char* data = (unsigned char*)src.data;



	double avg = 0.0;

	int num = 1;

	for (int y = 0; y < h - 1; y++)

	{

		for (int x = 0; x < w - 1; x++)

		{
			if (th < data[y *  src.cols + x]) {
				avg = (avg*(num - 1) + (double)data[y *  src.cols + x]) / num;
				num++;
			}
		}

	}



	return avg;

}
void PrintFunc(string Fn, const Mat in, int Zoom)
{
#ifdef PRINT_DEBUG
	Mat src, src1, src2;
	src = in.clone();
	resize(src, src1, Size(src.cols / Zoom, src.rows / Zoom));

	namedWindow(Fn, WINDOW_AUTOSIZE);
	imshow(Fn, src1);
#endif
}

string intToString(int n)
{
	stringstream s;
	s << n;
	return s.str();
}

string doubleToString(double n)
{
	stringstream s;
	s << n;
	return s.str();
}
void quickSort(int arr[], int left, int right) {
	int i = left, j = right;
	int pivot = arr[(left + right) / 2];
	int temp;
	do {
		while (arr[i] < pivot)
			i++;
		while (arr[j] > pivot)
			j--;
		if (i <= j) {
			temp = arr[i];
			arr[i] = arr[j];
			arr[j] = temp;
			i++;
			j--;
		}
	} while (i <= j);

	/* recursion */
	if (left < j)
		quickSort(arr, left, j);

	if (i < right)
		quickSort(arr, i, right);
}

void bitwise_mask(const Mat src, const Mat mask, Mat &dst)
{
	int w = src.size().width;

	int h = src.size().height;

	dst = src.clone();

	if (src.depth() == CV_32F) {
		float* mask_data = (float*)mask.data;
		float* data = (float*)src.data;



		for (int y = 0; y < h - 0; y++)
		{
			for (int x = 0; x < w - 0; x++)
			{
				if (mask_data[y *  mask.cols + x] == 0)
					dst.at<float>(y, x) = -0.0;
				else
					;
			}
		}
	}
	else if (src.depth() == CV_8U)
	{
		unsigned char* mask_data = (unsigned char*)mask.data;
		unsigned char* data = (unsigned char*)src.data;



		for (int y = 0; y < h - 0; y++)
		{
			for (int x = 0; x < w - 0; x++)
			{
				if (mask_data[y *  mask.cols + x] == 0)
					dst.at<unsigned char>(y, x) = 0;
				else
					;
			}
		}
	}
	else if (src.depth() == CV_64F)
	{
		double* mask_data = (double*)mask.data;
		double* data = (double*)src.data;



		for (int y = 0; y < h - 0; y++)
		{
			for (int x = 0; x < w - 0; x++)
			{
				if (mask_data[y *  mask.cols + x] == 0)
					dst.at<double>(y, x) = 0;
				else
					;
			}
		}
	}

}
#define USCHA unsigned char


void LowPassFilter(Mat src, Mat &dst, double cutoff)
{
	int		u, v;
	double	FactorU, FactorV;
	float	ftmp, Sum;
	double	D;
	double	H;
	double	centerU, centerV;
	cutoff = 0;
	int th = 0;
	double SensorPatternRemovalTH = 0;
	IplImage *srcM = &IplImage(src);
	dst = Mat::zeros(dst.size(), CV_32F);
	IplImage *dstM = &IplImage(dst);
	int cH_SIZE = srcM->width;
	int cV_SIZE = srcM->height;
	vector<USCHA>	PatternMaskingX(cH_SIZE);
	vector<USCHA>	PatternMaskingY(cV_SIZE);


	IplImage *mmm = cvCreateImage(cvSize(srcM->width, srcM->height), IPL_DEPTH_8U, 1);
	cvCvtScale(srcM, mmm, 1);

	for (v = 0; v < srcM->height; v++) {
		if (v < srcM->height / 10) {
			for (Sum = u = 0; u < srcM->width; u++) if ((USCHA)mmm->imageData[v*mmm->width + u] > th) Sum += 1;

			if (Sum >= SensorPatternRemovalTH * 2) PatternMaskingY[v] = 1;
			else                                     PatternMaskingY[v] = 0;
		}
		else {
			for (Sum = u = 0; u < srcM->width; u++) {
				//if((int)mmm->imageData[v*mmm->width + u] != 0)
				//cout << (int)mmm->imageData[v*mmm->width + u] << endl;
				if ((USCHA)mmm->imageData[v*mmm->width + u] > th) Sum += 1;
			}
			if (Sum >= SensorPatternRemovalTH) PatternMaskingY[v] = 1;
			else                                   PatternMaskingY[v] = 0;
		}
	}
	for (u = 0; u < srcM->width; u++) {
		if (u < srcM->width / 10) {
			for (Sum = v = 0; v < srcM->height; v++) if ((USCHA)mmm->imageData[v*mmm->width + u] > th) Sum += 1;
			if (Sum >= SensorPatternRemovalTH * 2) PatternMaskingX[u] = 1;
			else								     PatternMaskingX[u] = 0;
		}
		else {
			for (Sum = v = 0; v < srcM->height; v++) if ((USCHA)mmm->imageData[v*mmm->width + u] > th) Sum += 1;
			if (Sum >= SensorPatternRemovalTH) PatternMaskingX[u] = 1;
			else								   PatternMaskingX[u] = 0;
		}
	}

	//	fprintf( fp_log, "\n>>>> Average = %d\n\n", (USCHA) mmm->imageData[0] );
	//	fprintf( fp_log, "\n>>>> Average = %d %d %d %d\n\n", (USCHA) srcM->imageData[0], (USCHA) srcM->imageData[0], (USCHA) srcM->imageData[1], (USCHA) srcM->imageData[2], (USCHA) srcM->imageData[3] );

	centerU = srcM->width / 2;
	centerV = srcM->height / 2;

	FactorU = centerU < centerV ? 1 : centerV / centerU;
	FactorV = centerV < centerU ? 1 : centerU / centerV;

	for (v = 0; v < srcM->height; v++) {
		for (u = 0; u < srcM->width; u++) {
			D = sqrt((u)*(u)*FactorU*FactorU + (v)*(v)*FactorV*FactorV);

			//			if( D >= cutoff && ( (USCHA) mmm->imageData[v*mmm->width+u] <= SensorPatternRemovalTH[1] && (v > 2 && u > 2) ) ) H = 1.0;
			//			if( D <= 20 || D >= cutoff || (USCHA) mmm->imageData[v*mmm->width+u] <= SensorPatternRemovalTH[1] ) H = 1.0;
			//			if( D <= 300 || D >= cutoff ) H = 1.0;
			//			else                                                                                                             H = 0.0;
			H = 1;
			//if (D <= cutoff) H = 0;

			if (PatternMaskingY[v]) H = 0;
			if (PatternMaskingX[u]) H = 0;
			//if(H != 0)
			//	cout << v<<", "<<u<<endl;
			/*
			if( u < 6 ) H = 0;
			//			if( u >= 503 && u < 519 ) H = 0;	// 510
			////		if( u >= 918 && u < 927 ) H = 0;
			//			if( u >= 1013 && u < 1024 ) H = 0;

			if( v < 4 ) H = 0;
			//			if( v >= 185 && v < 194 ) H = 0;	// 190
			//			if( v >= 376 && v < 384 ) H = 0;
			////		if( v >= 568 && v < 570 ) H = 0;
			////		if( v >= 687 && v < 688 ) H = 0;
			*/

			if (u == 0 && v == 0) {
				dstM->imageData[v*dstM->width * 4 + 4 * u + 0] = H * srcM->imageData[v*srcM->width * 4 + 4 * u + 0];
				dstM->imageData[v*dstM->width * 4 + 4 * u + 1] = H * srcM->imageData[v*srcM->width * 4 + 4 * u + 1];
				dstM->imageData[v*dstM->width * 4 + 4 * u + 2] = H * srcM->imageData[v*srcM->width * 4 + 4 * u + 2];
				dstM->imageData[v*dstM->width * 4 + 4 * u + 3] = H * srcM->imageData[v*srcM->width * 4 + 4 * u + 3];
			}
			else {
				//cout << (int)srcM->imageData[v*srcM->width * 4 + 4 * u + 0] << endl;
				dstM->imageData[v*dstM->width * 4 + 4 * u + 0] = H * srcM->imageData[v*srcM->width * 4 + 4 * u + 0];
				dstM->imageData[v*dstM->width * 4 + 4 * u + 1] = H * srcM->imageData[v*srcM->width * 4 + 4 * u + 1];
				dstM->imageData[v*dstM->width * 4 + 4 * u + 2] = H * srcM->imageData[v*srcM->width * 4 + 4 * u + 2];
				dstM->imageData[v*dstM->width * 4 + 4 * u + 3] = H * srcM->imageData[v*srcM->width * 4 + 4 * u + 3];
			}
		}
	}
	cvNamedWindow("fsrcMuck");
	cvShowImage("fsrcMuck", srcM);
	cvNamedWindow("fuck");
	cvShowImage("fuck", dstM);
	dst = Mat::zeros(src.size(), CV_32F);
	dst = cvarrToMat(dstM);
	PrintFunc("tst", dst, 2);
	cv::waitKey();

	cvDestroyAllWindows();
}


void dct_background_remove(Mat src, Mat &dst)
{
	Mat fimg, dimg, dct_8uc, masking_map, masking_result;

	src.convertTo(fimg, CV_32F, 1.0 / 255);

	int m = getOptimalDFTSize(src.rows);
	int n = getOptimalDFTSize(src.cols); // on the border add zero values
	resize(fimg, dimg, Size(m, n));


	dct(dimg, dimg);
	masking_map = Mat::zeros(dimg.size(), CV_32F);
	masking_result = dimg.clone();

	PrintFunc("dct", dimg, 1);
	//여기서 지지고 복고
	//마스킹 맵 만들어서 특정 값 이상이면 1* data[x,y]; 아니면 0 *data[x,y];
	//LowPassFilter(dimg, masking_result, 10);
	//다해서 여기서 전환해주자
	masking_map += 1;
	rectangle(masking_map, Rect(-20, -20, 70, 70), 0, 20);
	rectangle(masking_map, Rect(-30, -30, 130, 130), 0, 20);
	rectangle(masking_map, Rect(-30, -30, 300, 300), 0, 20);
	//rectangle(masking_map, Rect(0, 0, 150, 150), 1, -1);

	bitwise_mask(dimg, masking_map, masking_result);
	dct(masking_result, dimg, DCT_INVERSE);
	dimg.convertTo(dst, CV_8U, 255);

	//PrintFunc("dct_8uc", dct_8uc, 1);
	PrintFunc("masking_result", masking_result, 1);
	PrintFunc("dst", dst, 3);
	PrintFunc("src", src, 3);
	waitKey();
}

void dft_background_remove_second(Mat src, Mat &dst, double radius_ratio, double thresh_ratio)
{
	//src : ÀÔ·Â ÀÌ¹ÌÁö
	//dst : °á°ú Ãâ·Â ÀÌ¹ÌÁö
	//radius_ratio : moire Á¦°Å ½Ã »ç¿ëÇÏ´Â ¿øº» ³²°Ü³õ´Â Á¤µµ
	//thresh_ratio : moire Á¦°Å ½Ã ÆÐÅÏÁ¦°Å Á¤µµ
	//ºÐÇÒÇØ¼­ 
	Mat padded, dft_img, idft_img;
	Mat del_dft, result, inv_img;
	Mat tmp;
	//expand input image to optimal size
	int m = getOptimalDFTSize(src.rows);
	int n = getOptimalDFTSize(src.cols); // on the border add zero values
	resize(src, dft_img, Size(m, n));

	//cout<<"dft_img size"<<dft_img.size()<<endl;

	copyMakeBorder(dft_img, padded, 0, m - dft_img.rows, 0, n - dft_img.cols, BORDER_CONSTANT, Scalar::all(0));
	//cout<<"padded size"<<padded.size()<<endl;
	Mat planes[] = { Mat_<float>(padded), Mat::zeros(padded.size(), CV_32F) };
	Mat tplanes[] = { Mat_<float>(padded), Mat::zeros(padded.size(), CV_32F) };
	Mat complexI, tcomplexI;
	merge(planes, 2, complexI);         // Add to the expanded another plane with zeros

	dft(complexI, complexI);            // this way the result may fit in the source matrix
	tcomplexI = complexI.clone();
	// compute the magnitude and switch to logarithmic scale
	// => log(1 + sqrt(Re(DFT(I))^2 + Im(DFT(I))^2))
	split(complexI, planes);                   // planes[0] = Re(DFT(I), planes[1] = Im(DFT(I))
	magnitude(planes[0], planes[1], planes[0]);// planes[0] = magnitude
	Mat magI = planes[0];

	magI += Scalar::all(1);                    // switch to logarithmic scale
	log(magI, magI);

	// crop the spectrum, if it has an odd number of rows or columns
	magI = magI(Rect(0, 0, magI.cols & -2, magI.rows & -2));

	// rearrange the quadrants of Fourier image  so that the origin is at the image center
	int cx = magI.cols / 2;
	int cy = magI.rows / 2;


	Mat q0(magI, Rect(0, 0, cx, cy));   // Top-Left - Create a ROI per quadrant
	Mat q1(magI, Rect(cx, 0, cx, cy));  // Top-Right
	Mat q2(magI, Rect(0, cy, cx, cy));  // Bottom-Left
	Mat q3(magI, Rect(cx, cy, cx, cy)); // Bottom-Right

										// swap quadrants (Top-Left with Bottom-Right)
	q0.copyTo(tmp);
	q3.copyTo(q0);
	tmp.copyTo(q3);

	// swap quadrant (Top-Right with Bottom-Left)
	q1.copyTo(tmp);
	q2.copyTo(q1);
	tmp.copyTo(q2);

	normalize(magI, magI, 0, 1, CV_MINMAX); // Transform the matrix with float values into a

											//¿ä±â±îÁö°¡ magI FFT ÀÌ¹ÌÁö
											//¿©±â¼­ ºÎÅÍ ÁöÁö°í ºº°í ÇØ¾ßÇÔ	

											//GaussianBlur(magI,del_dft,Size(7,7),2);
	cv::threshold(magI, del_dft, thresh_ratio, 1, CV_THRESH_BINARY_INV);

	int radius = MIN(del_dft.rows / 2, del_dft.cols / 2) * radius_ratio;

	circle(del_dft, Point(del_dft.cols / 2, del_dft.rows / 2), radius, 1, -1);

	//PrintFunc("del_dft ", del_dft, 3);
	erode(del_dft, del_dft, kernel, Point(-1, -1), 2);
	//PrintFunc("del_dft erode", del_dft, 3);

	bitwise_mask(magI, del_dft, result);

	//PrintFunc("Input Image result", result, 2);    // Show the result

	Mat qq0(del_dft, Rect(0, 0, cx, cy));   // Top-Left - Create a ROI per quadrant
	Mat qq1(del_dft, Rect(cx, 0, cx, cy));  // Top-Right
	Mat qq2(del_dft, Rect(0, cy, cx, cy));  // Bottom-Left
	Mat qq3(del_dft, Rect(cx, cy, cx, cy)); // Bottom-Right


	PrintFunc("del_dft rot", del_dft, 3);
	// swap quadrants (Top-Left with Bottom-Right)
	qq0.copyTo(tmp);
	qq3.copyTo(qq0);
	tmp.copyTo(qq3);

	// swap quadrant (Top-Right with Bottom-Left)
	qq1.copyTo(tmp);
	qq2.copyTo(qq1);
	tmp.copyTo(qq2);

	split(tcomplexI, tplanes);


	bitwise_mask(tplanes[0], del_dft, tplanes[0]);
	bitwise_mask(tplanes[1], del_dft, tplanes[1]);

	merge(tplanes, 2, tcomplexI);
	//¿ªDFT°¡ µÇ¾î¾ß ÇÏ´Âµ¥ Áö±Ý ¿ª DFT°¡ Àß ¾ÈµÈ´ç ÀÌ°Å Á¦´ë·Î °è»êÇØ¼­ DFT ÀÌ¹ÌÁö °Çµé°í ´Ù½Ã ¿ª°è»êÇÏ´Â ¹æÇâÀ¸·Î °¡¾ß ÇÑ´Ù.
	//³í¹®¿¡¼±
	//Mat Z = (magl - mean(magl))/std(magI);
	//Mat H; gaussianBlur(Z,H);
	//calculating the idft
	cv::dft(tcomplexI, idft_img, cv::DFT_INVERSE | cv::DFT_REAL_OUTPUT);

	resize(idft_img, dst, src.size());

	//cout<<"dst size"<<dst.size()<<endl;
	normalize(dst, dst, 0, 1, CV_MINMAX);

	PrintFunc("Input Image", src, 3);    // Show the result
	PrintFunc("spectrum magnitude", magI, 3);
	PrintFunc("spectrum thresh", result, 3);
	PrintFunc("inverse ", dst, 3);
	waitKey();
}

void dft_background_remove(Mat src, Mat &dst, double radius_ratio, double thresh_ratio)
{
	//src : 입력 이미지
	//dst : 결과 출력 이미지
	//radius_ratio : moire 제거 시 사용하는 원본 남겨놓는 정도
	//thresh_ratio : moire 제거 시 패턴제거 정도
	//분할해서 
	Mat padded, dft_img, idft_img;
	Mat del_dft, result, inv_img;
	Mat tmp;
	//expand input image to optimal size
	int m = getOptimalDFTSize(src.rows);
	int n = getOptimalDFTSize(src.cols); // on the border add zero values
	resize(src, dft_img, Size(m, n));

	//cout<<"dft_img size"<<dft_img.size()<<endl;

	copyMakeBorder(dft_img, padded, 0, m - dft_img.rows, 0, n - dft_img.cols, BORDER_CONSTANT, Scalar::all(0));
	//cout<<"padded size"<<padded.size()<<endl;
	Mat planes[] = { Mat_<float>(padded), Mat::zeros(padded.size(), CV_32F) };
	Mat tplanes[] = { Mat_<float>(padded), Mat::zeros(padded.size(), CV_32F) };
	Mat complexI, tcomplexI;
	merge(planes, 2, complexI);         // Add to the expanded another plane with zeros

	dft(complexI, complexI);            // this way the result may fit in the source matrix
	tcomplexI = complexI.clone();
	// compute the magnitude and switch to logarithmic scale
	// => log(1 + sqrt(Re(DFT(I))^2 + Im(DFT(I))^2))
	split(complexI, planes);                   // planes[0] = Re(DFT(I), planes[1] = Im(DFT(I))
	magnitude(planes[0], planes[1], planes[0]);// planes[0] = magnitude
	Mat magI = planes[0];

	magI += Scalar::all(1);                    // switch to logarithmic scale
	log(magI, magI);

	// crop the spectrum, if it has an odd number of rows or columns
	magI = magI(Rect(0, 0, magI.cols & -2, magI.rows & -2));

	// rearrange the quadrants of Fourier image  so that the origin is at the image center
	int cx = magI.cols / 2;
	int cy = magI.rows / 2;


	Mat q0(magI, Rect(0, 0, cx, cy));   // Top-Left - Create a ROI per quadrant
	Mat q1(magI, Rect(cx, 0, cx, cy));  // Top-Right
	Mat q2(magI, Rect(0, cy, cx, cy));  // Bottom-Left
	Mat q3(magI, Rect(cx, cy, cx, cy)); // Bottom-Right

										// swap quadrants (Top-Left with Bottom-Right)
	q0.copyTo(tmp);
	q3.copyTo(q0);
	tmp.copyTo(q3);

	// swap quadrant (Top-Right with Bottom-Left)
	q1.copyTo(tmp);
	q2.copyTo(q1);
	tmp.copyTo(q2);

	Mat Zmgl = magI.clone();

	//double avg_val, std_val;
	//Scalar a;
	//a = mean(Zmgl);
	//std_val = img_std(Zmgl);

	normalize(magI, magI, 0, 1, CV_MINMAX); // Transform the matrix with float values into a

											//요기까지가 magI FFT 이미지
											//여기서 부터 지지고 볶고 해야함	

											//GaussianBlur(magI,del_dft,Size(7,7),2);
	cv::threshold(magI, del_dft, thresh_ratio, 1, CV_THRESH_BINARY_INV);

	int radius = MIN(del_dft.rows / 2, del_dft.cols / 2) * radius_ratio;

	circle(del_dft, Point(del_dft.cols / 2, del_dft.rows / 2), radius, 1, -1);

	//PrintFunc("del_dft ", del_dft, 3);
	erode(del_dft, del_dft, kernel, Point(-1, -1), 2);
	//PrintFunc("del_dft erode", del_dft, 3);

	bitwise_mask(magI, del_dft, result);

	//PrintFunc("Input Image result", result, 2);    // Show the result

	Mat qq0(del_dft, Rect(0, 0, cx, cy));   // Top-Left - Create a ROI per quadrant
	Mat qq1(del_dft, Rect(cx, 0, cx, cy));  // Top-Right
	Mat qq2(del_dft, Rect(0, cy, cx, cy));  // Bottom-Left
	Mat qq3(del_dft, Rect(cx, cy, cx, cy)); // Bottom-Right

											// swap quadrants (Top-Left with Bottom-Right)
	qq0.copyTo(tmp);
	qq3.copyTo(qq0);
	tmp.copyTo(qq3);

	// swap quadrant (Top-Right with Bottom-Left)
	qq1.copyTo(tmp);
	qq2.copyTo(qq1);
	tmp.copyTo(qq2);

	split(tcomplexI, tplanes);


	bitwise_mask(tplanes[0], del_dft, tplanes[0]);
	bitwise_mask(tplanes[1], del_dft, tplanes[1]);

	merge(tplanes, 2, tcomplexI);
	//역DFT가 되어야 하는데 지금 역 DFT가 잘 안된당 이거 제대로 계산해서 DFT 이미지 건들고 다시 역계산하는 방향으로 가야 한다.
	//논문에선
	//Mat Z = (magl - mean(magl))/std(magI);
	//Mat H; gaussianBlur(Z,H);
	//calculating the idft
	cv::dft(tcomplexI, idft_img, cv::DFT_INVERSE | cv::DFT_REAL_OUTPUT);

	resize(idft_img, dst, src.size());

	//cout<<"dst size"<<dst.size()<<endl;
	normalize(dst, dst, 0, 1, CV_MINMAX);

	//PrintFunc("Input Image"       , src   ,3);    // Show the result
	//PrintFunc("spectrum magnitude", magI,3);
	//PrintFunc("spectrum thresh", result,3);
	//PrintFunc("inverse ", dst,3);
	//waitKey();
}

void Sharpen(const Mat& myImage, Mat& Result)
{
	CV_Assert(myImage.depth() == CV_8U);  // accept only uchar images

	Result.create(myImage.size(), myImage.type());
	const int nChannels = myImage.channels();

	for (int j = 1; j < myImage.rows - 1; ++j)
	{
		const uchar* previous = myImage.ptr<uchar>(j - 1);
		const uchar* current = myImage.ptr<uchar>(j);
		const uchar* next = myImage.ptr<uchar>(j + 1);

		uchar* output = Result.ptr<uchar>(j);

		for (int i = nChannels; i < nChannels * (myImage.cols - 1); ++i)
		{
			*output++ = saturate_cast<uchar>(5 * current[i]
				- current[i - nChannels] - current[i + nChannels] - previous[i] - next[i]);
		}
	}

	Result.row(0).setTo(Scalar(0));
	Result.row(Result.rows - 1).setTo(Scalar(0));
	Result.col(0).setTo(Scalar(0));
	Result.col(Result.cols - 1).setTo(Scalar(0));
}

int delete_by_size(const Mat& src, Mat& dst, int minsize, int maxsize, const Mat& base, bool maskON, int thickness = -1)
{
	//contour 사이즈 제외하고 출력하기
	//size 이상만 출력한다
	//그러고 나서 그 이상만 되는 blob 들을 base와 masking 시켜서 
	//gray level 만 남겨놓고 0을 제외한 gray만 평균값을 구해서 그 이하만 255만 출력하게 만듬

	//결과 만들어지긴 했는데 별로 좋은거 같진 않음

	CV_Assert(src.depth() == CV_8U);  // accept only uchar images

	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;
	Mat tt = src.clone();

	//PrintFunc("center_zone",center_zone,2);
	//waitKey();
	findContours(tt, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE, Point(0, 0));

	int cmin = minsize;  // ÃÖ¼Ò ¿Ü°û¼± ±æÀÌ
	int cmax = maxsize; // ÃÖ´ë ¿Ü°û¼± ±æÀÌ
	std::vector<std::vector<cv::Point>>::const_iterator itc = contours.begin();
	int ditan = 0;
	while (itc != contours.end()) {
		if (itc->size() < cmin || itc->size() > cmax)
			itc = contours.erase(itc);
		else {
			ditan += itc->size();
			++itc;
		}
	}
	//cout << "contour size : " << ditan << endl;
	Mat drawing = Mat::zeros(src.size(), CV_8UC1);
	for (int i = 0; i < contours.size(); i++)
	{

		drawContours(drawing, contours, i, Scalar(255), thickness, 8, hierarchy, 0, Point());
	}

	hierarchy.clear();
	contours.clear();
	dst = drawing.clone();

	if (maskON) {
		Mat tmp;
		bitwise_mask(base, dst, tmp);

		unsigned char th = (unsigned char)avg_img_th(tmp, 2);
		int w = src.size().width;

		int h = src.size().height;
		unsigned char* base_data = (unsigned char*)base.data;
		unsigned char* tmp_data = (unsigned char*)tmp.data;



		for (int y = 0; y < h - 0; y++)
		{
			for (int x = 0; x < w - 0; x++)
			{
				if (tmp_data[y *  base.cols + x] != 0) {
					if (tmp_data[y *  base.cols + x] <= th + 20)
						dst.at<unsigned char>(y, x) = 255;
					else
						dst.at<unsigned char>(y, x) = 0;
				}
			}
		}
		return th;
	}
	return ditan;

}
int delete_by_size_avg(const Mat& src, Mat& dst, int size, const Mat& base, bool maskON)
{
	//contour 사이즈 제외하고 출력하기
	//size 이상만 출력한다
	//그러고 나서 그 이상만 되는 blob 들을 base와 masking 시켜서 
	//gray level 만 남겨놓고 0을 제외한 gray만 평균값을 구해서 그 이하만 255만 출력하게 만듬

	//결과 만들어지긴 했는데 별로 좋은거 같진 않음

	CV_Assert(src.depth() == CV_8U);  // accept only uchar images

	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;
	vector<Rect> defect_list;
	Mat tt = src.clone();

	//PrintFunc("center_zone",center_zone,2);
	//waitKey();
	findContours(tt, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE, Point(0, 0));
	double avg = 0.0;

	int num = 1;
	int cmin = 0;  // 최소 외곽선 길이
	int cmax = size; // 최대 외곽선 길이
	std::vector<std::vector<cv::Point>>::const_iterator itc = contours.begin();
	for (int i = 0; i < contours.size(); i++)
	{
		avg = (avg*(num - 1) + (double)contours[i].size()) / num;
		num++;
	}


	while (itc != contours.end()) {
		if (itc->size() < avg || itc->size() > size)
			itc = contours.erase(itc);
		else
			++itc;
	}

	Mat drawing = Mat::zeros(src.size(), CV_8UC1);



	for (int i = 0; i < contours.size(); i++)
	{

		drawContours(drawing, contours, i, Scalar(255), -1, 8, hierarchy, 0, Point());
	}

	dst = drawing.clone();

	if (maskON) {
		Mat tmp;
		bitwise_mask(base, dst, tmp);

		unsigned char th = (unsigned char)avg_img_th(tmp, 2);
		int w = src.size().width;

		int h = src.size().height;
		unsigned char* base_data = (unsigned char*)base.data;
		unsigned char* tmp_data = (unsigned char*)tmp.data;



		for (int y = 0; y < h - 0; y++)
		{
			for (int x = 0; x < w - 0; x++)
			{
				if (tmp_data[y *  base.cols + x] != 0) {
					if (tmp_data[y *  base.cols + x] <= th + 10)
						dst.at<unsigned char>(y, x) = 255;
					else
						dst.at<unsigned char>(y, x) = 0;
				}
			}
		}
		return th;
	}
	return 0;
}

void mutiple_self(const Mat src, Mat& dst)
{
	int w = src.size().width;

	int h = src.size().height;



	if (src.depth() == CV_32F) {
		float* data = (float*)src.data;
		dst = Mat::zeros(src.size(), CV_32FC1);

		for (int y = 0; y < h - 0; y++)
		{
			for (int x = 0; x < w - 0; x++)
			{

				dst.at<float>(y, x) = data[y *  src.cols + x] * data[y *  src.cols + x];
				//cout<<dst.at<float>(y,x)<<endl;
			}
		}
	}
	else if (src.depth() == CV_8U)
	{
		unsigned char* data = (unsigned char*)src.data;
		dst = Mat::zeros(src.size(), CV_8UC1);

		for (int y = 0; y < h - 0; y++)
		{
			for (int x = 0; x < w - 0; x++)
			{

				dst.at<unsigned char>(y, x) = data[y *  src.cols + x] * data[y *  src.cols + x];

			}
		}
	}
	else if (src.depth() == CV_64F)
	{
		double* data = (double*)src.data;
		dst = Mat::zeros(src.size(), CV_64FC1);

		for (int y = 0; y < h - 0; y++)
		{
			for (int x = 0; x < w - 0; x++)
			{

				dst.at<double>(y, x) = data[y *  src.cols + x] * data[y *  src.cols + x];

			}
		}
	}

}

void gamma_func(const Mat src, Mat& dst, double gamma_)
{
	Mat lookUpTable(1, 256, CV_8U);
	uchar* p = lookUpTable.ptr();
	for (int i = 0; i < 256; ++i)
		p[i] = saturate_cast<uchar>(pow(i / 255.0, gamma_) * 255.0);
	Mat res = src.clone();
	LUT(src, lookUpTable, dst);
}

double median(cv::Mat channel)
{
	double m = (channel.rows*channel.cols) / 2;
	int bin = 0;
	double med = -1.0;

	int histSize = 256;
	float range[] = { 0, 256 };
	const float* histRange = { range };
	bool uniform = true;
	bool accumulate = false;
	cv::Mat hist;
	cv::calcHist(&channel, 1, 0, cv::Mat(), hist, 1, &histSize, &histRange, uniform, accumulate);

	for (int i = 0; i < histSize && med < 0.0; ++i)
	{
		bin += cvRound(hist.at< float >(i));
		if (bin > m && med < 0.0)
			med = i;
	}

	return med;
}

void auto_canny(Mat in, Mat& dst, double sigma)
{
	//원래는 중간값? 을 구해야 하는데 일단 대충 평균값 구하자
	Mat src = in.clone();
	//blur(src, src, Size(3, 3));
	double v = median(src);

	double lower = MAX((1.0 - sigma) * v, 0);
	double upper = lower * 3;

	//cout << "sigma : " << sigma << endl;
	//cout << "median : " << v << endl;
	//cout << "avg : " << avg_img(src) << endl;
	//cout << "lower : " << lower << endl;
	//cout << "upper : " << upper << endl;

	Canny(src, dst, lower, upper);
}

void ring_canny(const Mat& src, Mat& dst, int size, double min_th, double max_th)
{
	//canny를 중심에는 쎄게 때리고 점점 흐려지게 할 요령임
	//그냥 size 별로 등분 때리고 원 모양으로 양궁 과녁처럼 할 예정

	CV_Assert(src.depth() == CV_8U);  // accept only uchar images
	CV_Assert(size > 1); // accep only over one
						 //CV_Assert(min_th < max_th); // accep only over one
	vector<Mat> Canny_img;
	Mat canny_image;
	int radius = MAX(src.rows / 2, src.cols / 2);
	double th = (max_th - min_th) / (size);

	dst = Mat::zeros(src.size(), CV_8UC1);

	//0부터 중앙
	for (size_t i = 0; i < size + 1; i++)
	{
		Mat drawing = Mat::zeros(src.size(), CV_8UC1);
		circle(drawing, Point(drawing.cols / 2, drawing.rows / 2), radius*(i + 1) / size, 255, -1);
		circle(drawing, Point(drawing.cols / 2, drawing.rows / 2), radius*(i) / size, 0, -1);

		double th1 = min_th + th*i;
		//cout << th1 << endl;
		//Canny(src, canny_image, th1, th1 * 3);
		auto_canny(src, canny_image, th1);

		cv::bitwise_and(canny_image, drawing, canny_image);
		dst += canny_image;
		//Canny_img.push_back(canny_image);
	}
	//cout << endl;

}


Rect extractMainLabel(Mat input, Mat &dst)
{
	Mat img_gray, img_color, img_binary;



	threshold(input, img_binary, 127, 255, THRESH_BINARY);
	img_color = Mat::zeros(input.size(), CV_8UC3);

	Mat img_labels, stats, centroids;
	int numOfLables = connectedComponentsWithStats(img_binary, img_labels,
		stats, centroids, 8, CV_32S);




	//라벨링 된 이미지에 각각 직사각형으로 둘러싸기 
	int max_area = 0;
	int max_idx = 1;
	int left, top, height, width;
	for (int j = 1; j < numOfLables; j++) {
		int area = stats.at<int>(j, CC_STAT_AREA);
		if (area > max_area)
		{
			max_area = area;
			max_idx = j;
			left = stats.at<int>(j, CC_STAT_LEFT);
			top = stats.at<int>(j, CC_STAT_TOP);
			height = stats.at<int>(j, CC_STAT_HEIGHT);
			width = stats.at<int>(j, CC_STAT_WIDTH);
		}
	}


	//라벨링된 이미지중 특정 라벨을 컬러로 표현해주기  
	for (int y = 0; y < img_labels.rows; ++y) {

		int *label = img_labels.ptr<int>(y);
		Vec3b* pixel = img_color.ptr<Vec3b>(y);


		for (int x = 0; x < img_labels.cols; ++x) {


			if (label[x] == max_idx) {
				pixel[x][2] = 255;
				pixel[x][1] = 255;
				pixel[x][0] = 255;
			}
		}
	}
	cv::cvtColor(img_color, img_gray, CV_BGR2GRAY);
	threshold(img_gray, dst, 127, 255, THRESH_BINARY);
	return Rect(left, top, width, height);
	/*
	int row;
	int col;
	int x, y;
	unsigned int top;
	short l_no = 0, r, c, m, n;

	row = input.rows;
	col = input.cols;
	dst = Mat::zeros(row, col, CV_8U);

	unsigned char *m_ResultImg;
	m_ResultImg = (unsigned char*)malloc(sizeof(unsigned char)*row*col);

	unsigned char *arr;
	arr = (unsigned char*)malloc(sizeof(unsigned char)*row*col);

	// 스택으로 사용할 메모리 할당
	short *stack_x;
	short *stack_y;

	stack_x = (short *)malloc(sizeof(short)*row*col);
	stack_y = (short *)malloc(sizeof(short)*row*col);

	// 속도개선을 위하여 배열에 Mat Pixel 값을 복사
	for (y = 0; y < row; y++) {
	for (x = 0; x < col; x++) {
	*(arr + (col*y) + x) = input.at<uchar>(y, x);
	}
	}

	// 결과 이미지 배열 초기화
	for (y = 0; y<row; y++) {
	for (x = 0; x<col; x++) *(m_ResultImg + (col*y) + x) = 0;
	}

	for (y = 1; y<row - 1; y++) {
	for (x = 1; x<col - 1; x++) {
	// 이미 방문한 점이거나 픽셀값이 255가 아니면처리하지 않음
	if (*(arr + (col*y) + x) != 255 || *(m_ResultImg + (col*y) + x) != 0) continue;

	r = y;
	c = x;
	top = 0;
	l_no++;

	while (1) {
	for (m = r - 1; m <= r + 1; m++) {
	if (m<0 || m>row - 1) continue;

	for (n = c - 1; n <= c + 1; n++) {
	if (n<0 || n>col - 1) continue;
	if (*(arr + (col*m) + n) != 255 || *(m_ResultImg + (col*m) + n) != 0) continue;

	// l_no: Labeling Number(Label Key)
	*(m_ResultImg + (col*m) + n) = (unsigned char)l_no;
	if (Push(stack_x, stack_y, m, n, &top) == -1) continue;

	}
	}
	if (Pop(stack_x, stack_y, &r, &c, &top) == -1) break;
	}
	}
	}




	//**************** 수정부분 시작
	int max_ln = 0;            // 가장 많은 픽셀을 갖고 있는 Labeling number
	int* max_px;
	max_px = new int[(int)l_no + 1];

	for (int i = 0; i <= l_no; i++) {
	max_px[i] = 0;
	}

	// Labeling Pixel 저장
	for (y = 0; y<row; y++) {
	for (x = 0; x<col; x++) {
	if (*(m_ResultImg + (col*y) + x) == 0) continue;
	max_px[*(m_ResultImg + (col*y) + x)]++;
	}
	}

	l_no++;
	// 가장 큰 Label Number 저장
	int cntNumber = 0;
	for (int i = 1; i <= l_no - 1; i++) {
	if (max_px[max_ln] < max_px[i])
	max_ln = i;
	}

	// 가장 큰 영역을 남기고 제거(가장 큰 영역은 흰색으로)
	for (y = 0; y<row; y++) {
	for (x = 0; x<col; x++) {
	if (*(m_ResultImg + (col*y) + x) == 0) continue;
	if (*(m_ResultImg + (col*y) + x) == max_ln) *(m_ResultImg + (col*y) + x) = 255;
	else
	*(m_ResultImg + (col*y) + x) = 0;
	}
	}
	delete[] max_px;
	//**************** 수정부분 종료



	for (y = 0; y < row; y++) {
	for (x = 0; x < col; x++) {
	dst.at<uchar>(y, x) = *(m_ResultImg + (col*y) + x);
	}
	}

	free(arr);
	free(m_ResultImg);
	free(stack_x);
	free(stack_y);

	return l_no - 1;
	*/
}

void flood_extract(Mat src1, Mat src2, Mat& dst)
{
	Mat draw = src1 + src2;
	Mat tmp = draw.clone();
	bitwise_and(src1, src2, tmp);
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;
	vector<Rect> defect_list;
	Mat tt = tmp.clone();

	//PrintFunc("center_zone",center_zone,2);
	//waitKey();
	findContours(tt, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE, Point(0, 0));
	vector<Rect> BoundRect;
	for (size_t i = 0; i < contours.size(); i++) {
		Point maxloc;
		maxloc.x = contours[i][0].x;
		maxloc.y = contours[i][0].y;
		floodFill(draw, maxloc, 0); //mark drawn blob    

	}

	//while (true)
	//{
	//	double minval, maxval, threshold = 0.5*255.0;
	//	Point minloc, maxloc;
	//	minMaxLoc(tmp, &minval, &maxval, &minloc, &maxloc);
	//	//cout << "maxval : " << maxval << endl;
	//	if (maxval >= threshold)
	//	{
	//		floodFill(tmp, maxloc, 0); //mark drawn blob     
	//		floodFill(draw, maxloc, 0); //mark drawn blob    
	//	}
	//	else
	//		break;
	//}

	bitwise_xor(draw, src1 + src2, dst);
}
double PosDistance(Point2f p1, Point2f p2)
{
	double distance;

	// 피타고라스의 정리
	// pow(x,2) x의 2승,  sqrt() 제곱근
	distance = sqrt((double)((double)(p1.x - p2.x)*(p1.x - p2.x) + (double)(p1.y - p2.y)*(p1.y - p2.y)));

	return distance;
}
double compare_feature(Mat img1, Mat img2, Rect ROI)
{

	Mat LA_img1, img_32;
	//blur(img1, img1, Size(3, 3));
	//Sharpen(img1,img1);
	int w_10 = (int)ROI.width / 10;
	int h_10 = (int)ROI.height / 10;
	//ROI.x -= w_10;
	//ROI.y -= h_10;
	//ROI.width  += w_10 * 2;
	//ROI.height += h_10 * 2;
	img1(ROI).convertTo(LA_img1, CV_32FC1, 1.0 / 255.0);
	resize(LA_img1, LA_img1, Size(), 0.5, 0.5, CV_INTER_AREA);
	//blur(LA_img1,LA_img1,Size(3,3));
	//PrintFunc("convertTo",LA_img1,1);
	Mat kernel_x = (Mat_<float>(1, 3) << -1, 2, -1);
	Mat kernel_y = (Mat_<float>(3, 1) << -1, 2, -1);
	Mat kern = (Mat_<float>(3, 3) << -1, -1, -1,
		-1, 9, -1,
		-1, -1, -1);
	//Mat kernel_mean = Mat::ones(3, 3, CV_32F);
	//kernel_mean = kernel_mean / (kernel_mean.cols * kernel_mean.rows);
	Mat Lx, Ly, Mx, My;
	filter2D(LA_img1, Lx, CV_32F, kern, Point(-1, -1), 0.0, BORDER_REFLECT_101);
	//filter2D(LA_img1, Ly, CV_32F, kernel_y, Point(-1, -1), 0.0, BORDER_REFLECT_101);
	//filter2D(abs(Lx) + abs(Ly), dst, CV_32F, kernel_mean);
	//mutiple_self(Lx,Mx);
	//mutiple_self(Ly,My);
	Mat dst = Mx + My;
	Lx.convertTo(img_32, CV_8UC1, 255.0);
	threshold(img_32, img_32, 0, 255, THRESH_BINARY | THRESH_OTSU);
	//cout<<"LA_img1+ : "<<avg_img(img_32)<<endl;
	medianBlur(img_32, img_32, 9);

	//PrintFunc("LA_img1", LA_img1, 1);
	//PrintFunc("dst2", img_32, 1);
	////PrintFunc("img2", img2, 1);
	//
	//waitKey();
	return avg_img(img_32);

}
double compare_feature_second(Mat img1, Mat img2, Rect ROI)
{

	Mat LA_img1, canny_img, bitwise_image, res_32f;
	Mat LA_img2;
	LA_img1 = img1(ROI).clone();
	LA_img2 = img2(ROI).clone();
	auto_canny(LA_img1, canny_img, 0.6);
	PrintFunc("LA_img1", LA_img1, 1);
	PrintFunc("LA_img2", LA_img2, 1);
	PrintFunc("canny_img", canny_img, 1);

	//제일 의미있고 제일 선명한 선을 가진 이미지를 찾아야한다.

	waitKey();
	return avg_img(bitwise_image);

}

struct focus_info {
	int idx;
	int pixel_number;
	double pixel_feature_number;
	Mat DFT, Image;
};

bool cmp_pixel_cnt(focus_info a, focus_info b) {
	return (a.pixel_number > b.pixel_number);
}
bool cmp_pixel_feature(focus_info a, focus_info b) {
	return (a.pixel_feature_number > b.pixel_feature_number);
}



Point best_focus_color(vector<Mat> &image, vector<Mat> &black, vector<Mat> &white, double dft_thresh_min, double dft_thresh_max, double dft_radius)
{
	//각 포커스 별로 DFT를 수행한 후에 auto_canny를 사용해서 각각의 canny이미지를 만든다.
	//모든 canny 이미지들을 합치게 되면 불량이라 할수 특징들이 가장 많이 겹쳐지게 되고
	//가장 큰 blob을 가져와서 가장큰 blob과 비슷한 <- 현재는 pixel 수, 유사도 계산 해보자
	// canny이미지를 가져와 그걸 가장 포커스가 잘 맞는다고 가정한다.
	// 그리고 가장큰 blob의 밝기값을 가져와서 이것이 black인지 white인지 판단한다.
	//dft_min, max는 외부에서 조절 가능하게 만들고
	// 입력은 각기 다른 포커스 이미지 vector
	// 출력은 binary로 best focus 이미지 원본 출력 또는 vector 출력
	// return은 포커스 위치를 출력해줄거임

	black.clear();
	white.clear();
	Mat img_gray, inverseTransform, U8char_img, thresh, canny_img, otsu, back_del;


	double dft_thresh = 0.58;

	double factor_value = (dft_thresh_max - dft_thresh_min) / (image.size() - 1);

	Mat sum = Mat::zeros(image[0].size(), CV_8UC1);
	Mat egde_sum = Mat::zeros(image[0].size(), CV_8UC1);
	Mat gray_egde_sum = Mat::zeros(image[0].size(), CV_8UC1);
	Mat otsu_sum = Mat::zeros(image[0].size(), CV_8UC1);
	//otsu_sum += 255;
	vector <Mat> canny_list;
	vector <Mat> DFT_list;
	for (size_t i = 0; i < image.size(); i++)
	{
		cvtColor(image[i], img_gray, CV_BGR2GRAY);
		//PrintFunc("img_gray",img_gray,2);
		//waitKey();
		dft_thresh = (factor_value*i) + dft_thresh_min;

		dft_background_remove(img_gray, inverseTransform, dft_radius, dft_thresh);
		inverseTransform.convertTo(U8char_img, CV_8UC1, 255.0);
		//PrintFunc("U8char_img",U8char_img,3);

		gamma_func(U8char_img, U8char_img, 1.2);
		//auto_canny(U8char_img, thresh, 0.28);
		ring_canny(U8char_img, thresh, 10, 0.5, 0.1);
		sum += thresh;

		//threshold(U8char_img, canny_img, 0, 255, THRESH_BINARY_INV | THRESH_OTSU);
		cv::adaptiveThreshold(U8char_img, canny_img, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY_INV, 351, 20);

		//PrintFunc("U8char_img", U8char_img, 2);
		//PrintFunc("thresh", canny_img, 2);
		//flood_extract(otsu_sum, canny_img, otsu_sum);
		otsu_sum += canny_img;
		DFT_list.push_back(U8char_img.clone());
		canny_list.push_back(thresh.clone());

		auto_canny(img_gray, thresh, 0.67);
		//
		//adaptiveThreshold(img_gray,thresh,255,ADAPTIVE_THRESH_MEAN_C,THRESH_BINARY,51,10);
		gray_egde_sum += thresh;
		//waitKey();
	}


	////////////////////////////////////////////////////////////////////
	//배경제거 - 나중에 홍차장님께 depth map 받아서 처리하는 방법도 생각해보자
	cvtColor(image[0], img_gray, CV_BGR2GRAY);
	threshold(img_gray, otsu, 25, 255, THRESH_BINARY_INV);//ADAPTIVE_THRESH_GAUSSIAN_C
	erode(otsu, otsu, kernel, Point(-1, -1), 15);
	medianBlur(otsu, otsu, 15);
	dilate(otsu, otsu, kernel, Point(-1, -1), 55);
	rectangle(otsu, Point(0, 0), Point(otsu.cols, otsu.rows), 255, MIN((int)adap_th1*1.2, 254));
	Mat tmp = otsu.clone();
	floodFill(tmp, Point(1, 1), 0);
	bitwise_xor(otsu, tmp, back_del);

	otsu_sum -= back_del;
	PrintFunc("otsu_sum", otsu_sum, 2);

	sum = otsu_sum.clone();

	Mat resu, roi;
	rectangle(sum, Point(0, 0), Point(sum.size().width, sum.size().height), 0, 10);
	egde_sum = sum.clone();

	delete_by_size(sum, resu, 30, 35000, sum, false);
	//erode(resu, resu, kernel, Point(-1, -1), 2);
	PrintFunc("resu", resu, 2);
	dilate(resu, sum, kernel);
	rectangle(sum, Point(0, 0), Point(sum.size().width, sum.size().height), 0, 200);
	Mat drawing = Mat::zeros(image[0].size(), CV_8UC1);
	int radius = MAX(drawing.rows / 2, drawing.cols / 2);
	circle(drawing, Point(drawing.cols / 2, drawing.rows / 2), radius, 255, -1);
	bitwise_and(sum, drawing, sum);
	PrintFunc("sum", egde_sum, 2);
	//waitKey();
	extractMainLabel(sum, resu);

	//
	//erode(resu,resu,kernel);
	roi = resu.clone();
	//flood_extract(sum,resu,roi);
	PrintFunc("roi", roi, 2);
	//waitKey();
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;
	vector<Rect> defect_list;
	Mat tt = resu.clone();

	//PrintFunc("center_zone",center_zone,2);
	//waitKey();
	findContours(tt, contours, hierarchy, CV_RETR_EXTERNAL, CV_CHAIN_APPROX_NONE, Point(0, 0));
	vector<Rect> BoundRect;
	for (size_t i = 0; i < contours.size(); i++) {

		Rect BR = boundingRect(Mat(contours[i]));

		BoundRect.push_back(BR);

	}


	double maxSize = 0;
	double minSize = 3000;
	int maxIdx = 0;
	int minIdx = 0;


	vector<focus_info> info_list;
	for (size_t i = 0; i < canny_list.size(); i++)
	{
		canny_list[i];
		Mat test;
		bitwise_and(canny_list[i], roi, test);
		//PrintFunc(intToString(i)+"test",test,2);
		//PrintFunc(intToString(i)+"dst",DFT_list[i],2);
		//PrintFunc(intToString(i)+"canny_list",canny_list[i],2);
		cout << i << endl;
		cout << "pixel_cnt : " << pixel_cnt(test) << endl;
		double f = compare_feature(DFT_list[i], test, BoundRect[0]);
		double ff = compare_feature_second(DFT_list[i], roi, BoundRect[0]);
		//cout<<"pixel_cnt : "<<t<<endl;
		cout << "compare_feature : " << f << endl;
		if (maxSize < f) {
			maxSize = f;
			maxIdx = i;
		}
		if (minSize > f) {
			minSize = f;
			minIdx = i;
		}
		focus_info temp_info;
		temp_info.idx = i;
		temp_info.pixel_number = pixel_cnt(test);
		temp_info.pixel_feature_number = f;
		temp_info.DFT = DFT_list[i].clone();
		temp_info.Image = image[i].clone();
		info_list.push_back(temp_info);
		//waitKey();
	}


	sort(info_list.begin(), info_list.end(), cmp_pixel_feature);
	Point return_idx;
	return_idx.y = info_list[info_list.size() - 1].idx;
	//cout << "return_idx.y : " << return_idx.y << endl;
	white.push_back(info_list[info_list.size() - 1].DFT.clone());
	white.push_back(info_list[info_list.size() - 1].Image.clone());
	white.push_back(egde_sum);

	sort(info_list.begin(), info_list.end(), cmp_pixel_cnt);

	//info_list.erase(info_list.begin() + 4, info_list.end());
	sort(info_list.begin(), info_list.end(), cmp_pixel_feature);

	black.push_back(info_list[0].DFT.clone());
	black.push_back(info_list[0].Image.clone());
	black.push_back(egde_sum);

	//0인것도 지워보자
	std::vector<focus_info>::const_iterator itc = info_list.begin();
	while (itc != info_list.end()) {
		if (itc->pixel_number == 0)
			itc = info_list.erase(itc);
		else
			++itc;
	}

	//혹시나 다 지워지면 파괴되니깐
	if (info_list.size() != 0)
	{
		black.clear();
		black.push_back(info_list[0].DFT.clone());
		black.push_back(info_list[0].Image.clone());
		black.push_back(egde_sum);
	}

	//LED에 딱 달라붙은 검정 이물을 찾기 위해서하는거	
	//PrintFunc("gray_egde_sum", gray_egde_sum, 2);

	dilate(gray_egde_sum, gray_egde_sum, kernel, Point(-1, -1), 5);
	erode(gray_egde_sum, gray_egde_sum, kernel, Point(-1, -1), 7);
	medianBlur(gray_egde_sum, gray_egde_sum, 5);
	threshold(gray_egde_sum, gray_egde_sum, 120, 255, THRESH_BINARY_INV);
	flood_extract(gray_egde_sum, resu, roi);

	black.push_back(roi);
	return_idx.x = info_list[0].idx;
	//cout << "return_idx.x : " << return_idx.x << endl;

	return return_idx;
}

void std_img(vector<Mat> src, Mat& dst, float th)
{

	/*
	//이루틴이랑 한세트
	//dft 변화량 테스트 해보기
	//dft th값을 60 ~ 80 까지 변화해서 표준편차 구하고 그 차이값에 따라 1,0으로 구분해보자
	vector<Mat> image;
	for(size_t i = 0; i<10; i++)
	{
	Mat dft_img;
	Mat IT;
	double tasda = (double)(60+(i*4))/100;
	//cout<<tasda<<endl;
	dft_background_remove(img_gray,IT,dft_radius ,tasda );
	IT.convertTo(dft_img, CV_8UC1, 255.0);
	image.push_back(dft_img.clone());
	//PrintFunc("dft_img",dft_img,2);
	//waitKey();
	}
	Mat dft_img;
	std_img(image,dft_img,170);
	PrintFunc("dft_img",dft_img,2);
	*/
	//AIF와 비슷하게 변화량을 보려는 방법
	int row;
	int col;
	int x, y, top;

	row = src[0].rows;
	col = src[0].cols;
	dst = Mat::zeros(row, col, CV_8U);

	vector <float*> test(src.size());

	for (size_t i = 0; i < src.size(); i++)
	{
		//PrintFunc(intToString(i),src[i],4);
		test[i] = (float*)malloc(sizeof(float)*row*col);
	}
	//	waitKey();
	for (y = 0; y < row; y++) {
		for (x = 0; x < col; x++) {
			for (size_t i = 0; i < src.size(); i++)
			{
				*(test[i] + (col*y) + x) = (float)src[i].at<uchar>(y, x);
			}

		}
	}
	for (y = 0; y < row; y++) {
		for (x = 0; x < col; x++) {
			float avg_val = 0;
			float sum_val = 0;
			float var_val = 0;
			float std_val = 0;
			for (size_t i = 0; i < src.size(); i++)
			{
				avg_val += (float)*(test[i] + (col*y) + x);
				//cout<<"*(test[i]+(col*y)+x) : "<<*(test[i]+(col*y)+x)<<endl;
			}
			//평균을 구하고
			//cout<<"sum_val : "<<avg_val<<endl;
			avg_val /= (float)src.size();
			//cout<<"avg_val : "<<avg_val<<endl;
			for (size_t i = 0; i < src.size(); i++)
			{
				sum_val += pow(*(test[i] + (col*y) + x) - avg_val, 2);
			}
			//cout<<"sum_val : "<<sum_val<<endl;
			var_val = sum_val / (float)src.size();
			std_val = sqrt(var_val);
			//cout<<"abs(avg_val - std_val) : "<<abs(avg_val - std_val)<<endl;
			dst.at<uchar>(y, x) = (unsigned char)abs(avg_val - std_val);

		}
	}

	for (size_t i = 0; i < src.size(); i++)
	{
		free(test[i]);
	}


}

void PID_canny(Mat src, Mat& dst, double init, int dst_point)
{
	Mat tmp = Mat::zeros(src.size(), CV_8UC1);
	Mat canny_img;
	auto_canny(src, canny_img, init);

	double current_point = delete_by_size(canny_img, tmp, 50, 15000, tmp, false, 1);

	double ratio = init;

	double KP = 10;
	double KI = 0.0000001;
	double KD = 20.02;
	int iteration_time = 1;
	double error_prior = 0;
	double integral = 0;
	double bias = -100;
	while (1)
	{
		cout << "current_point : " << current_point << endl;
		double error = dst_point - current_point;
		cout << "error : " << error << endl;
		integral = integral + (error*iteration_time);
		double derivative = (double)(error - error_prior) / iteration_time;
		double output = KP*error + KI*integral + KD*derivative + bias;
		//output += current_point;
		cout << "output : " << output << endl;

		double input = (double)(ratio * output) / current_point;
		ratio = input;
		cout << "ratio : " << ratio << endl;
		error_prior = error;

		ratio = MAX(ratio, 0.0001);
		ratio = MIN(ratio, 0.8);
		auto_canny(src, canny_img, ratio);

		current_point = delete_by_size(canny_img, tmp, 50, 15000, tmp, false, 1);

		iteration_time++;
		PrintFunc("canny_img", canny_img, 2);
		cout << endl;
		waitKey();
		if (iteration_time > 15)
			break;
	}

}


int lowThreshold, highThreshold;
void TrackBar(Mat src, Mat &dst)
{
	namedWindow("result", WINDOW_AUTOSIZE);

	//트랙바를 생성하고 디폴트값으로 50지정  
	createTrackbar("min threahold", "result", &lowThreshold, 200, getThreshold);
	setTrackbarPos("min threahold", "result", 62);

	//트랙바를 생성하고 디폴트값으로 150지정  
	createTrackbar("max threahold", "result", &highThreshold, 100, getThreshold);
	setTrackbarPos("max threahold", "result", 2);

	Mat result, before, img_gray, inverseTransform, U8char_img, Max_img, Min_img;
	//int th1 = lowThreshold * 2 + 3;
	//int th2 = highThreshold - 100;
	//result = src.clone();
	//adaptiveThreshold(src, before, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY_INV, th1, th2);//ADAPTIVE_THRESH_GAUSSIAN_C
	//vector<Mat> BGR(3);
	//split(src, BGR);
	//MaxMin_Func(BGR, Max_img, Min_img);
	if (src.channels() == 3)
		cvtColor(src, img_gray, CV_BGR2GRAY);
	else
		img_gray = src.clone();
	while (1)
	{
		//트랙바 이동시 바뀐값을 실시간으로 계속 가져옴  
		lowThreshold = getTrackbarPos("min threahold", "result");
		highThreshold = getTrackbarPos("max threahold", "result");


		//blur(src, img_edge, Size(3, 3));

		//Canny함수에 바뀐값을 적용  
		//gamma_func(src, result, (double)lowThreshold/100 );
		//auto_canny(result, result, (double)highThreshold / 100);
		double th1 = (double)lowThreshold * 2 + 3;
		double th2 = (double)highThreshold;
		th1 = (double)lowThreshold / 100;

		cout << "th1 : " << th1 << endl;
		//cout << "th2 : " << th2 << endl;
		//adaptiveThreshold(img_gray, result, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY_INV, th1, th2);//ADAPTIVE_THRESH_GAUSSIAN_C
		auto_canny(img_gray, result, th1);
		PrintFunc("result", result, 2);

		int key = waitKey(1);
		if (key == 'q') break;
		if (key == 'a')
		{
			flood_extract(dst, result, result);
			PrintFunc("dst", dst, 2);
			PrintFunc("median", result, 2);
		}
	}
	dst = result.clone();
}
int good_canny(Mat src, Mat &dst, int set_point, int min_th = 50, int max_th = 15000, int start = 50, int end = 90, int iter = 3, int back_th = 25, int new_ver = 0)
{
	vector<Mat> LUT_canny;
	vector<int> current_val;
	vector<double> canny_val;
	int current, error;
	int min_error, min_idx;
	min_error = set_point;

	////////////////////////////////////////////////////////////////////
	//배경제거 - 나중에 홍차장님께 depth map 받아서 처리하는 방법도 생각해보자
	//cvtColor(image[0], img_gray, CV_BGR2GRAY);
	Mat otsu;
	threshold(src, otsu, back_th, 255, THRESH_BINARY_INV);//ADAPTIVE_THRESH_GAUSSIAN_C
	erode(otsu, otsu, kernel, Point(-1, -1), 15);
	medianBlur(otsu, otsu, 15);
	dilate(otsu, otsu, kernel, Point(-1, -1), 55);
	rectangle(otsu, Point(0, 0), Point(otsu.cols, otsu.rows), 255, MIN((int)adap_th1*1.2, 254));
	Mat tmp = otsu.clone();
	floodFill(tmp, Point(1, 1), 0);
	bitwise_xor(otsu, tmp, otsu);

	PrintFunc("canny otsu", otsu, 2);

	blur(src, src, Size(5, 5));


	for (int i = start; i < end; i += iter)
	{
		Mat tmp;
		//상위에 좀더 많은 탐색을 위해서 감마그래프의 값을 가져다 쓴다 
		double gamma_val = pow((double)i / 100, 0.3);
		auto_canny(src, tmp, gamma_val);
		tmp -= otsu;
		//PrintFunc("gg", tmp, 3);
		//waitKey();
		//dilate(tmp, tmp, kernel);
		current = delete_by_size(tmp, tmp, min_th, max_th, tmp, false, 1);
		error = abs(set_point - current);
		//if()
		cout << gamma_val << " : " << current << endl;
		// * 1 -> 1.5 :  O:\SDOPTICS\WDMS\Images\Measure\I2-S2-0405-002
		//cout << "set_point : "<<set_point * 1.5 << endl;
		if (current < 100)
			continue;
		LUT_canny.push_back(tmp);
		current_val.push_back(current);
		canny_val.push_back(gamma_val);
		if (current > set_point * 1.5)
			break;
		if (min_error > error)
		{
			//cout << gamma_val << " : " << current <<endl;

			min_error = error;
			dst = tmp.clone();
		}
		//PrintFunc("gg", tmp, 3);
		//waitKey();
	}
	//PrintFunc("good_canny", dst, 2);
	//for (int i = 0; i < LUT_canny.size(); i++)
	//{
	//	cout << canny_val[i] << " : " << current_val[i] << endl;
	//	PrintFunc("canny",LUT_canny[i],2);
	//	int a = waitKey();
	//	if (a == 'a') {
	//		i -= 2;
	//		if (i < 0)
	//			i = 0;
	//	}
	//	if (a == 'q')
	//		break;
	//}

	float avg_val = 0;
	float sum_val = 0;
	float var_val = 0;
	float std_val = 0;
	float cnt = 0;
	int small_cnt = 0;
	int big_cnt = 0;
	for (size_t i = 0; i < current_val.size(); i++)
	{
		if (500 < current_val[i] && current_val[i] < set_point) {
			avg_val += current_val[i];
			cnt++;
		}
		if (500 > current_val[i])
			small_cnt++;
		if (set_point < current_val[i])
			big_cnt++;
		//cout<<"*(test[i]+(col*y)+x) : "<<*(test[i]+(col*y)+x)<<endl;
	}

	//평균을 구하고
	//cout<<"sum_val : "<<avg_val<<endl;
	//너무 크거나 너무 작아서
	if (LUT_canny.size() == 0) {
		dst = Mat::zeros(src.size(), CV_8UC1);
		return 0;
	}
	if (avg_val == 0)
	{
		if (small_cnt > big_cnt)
			dst = LUT_canny[LUT_canny.size() - 1].clone();
		else
			dst = LUT_canny[0].clone();
	}

	avg_val /= (float)cnt;
	//cout<<"avg_val : "<<avg_val<<endl;
	for (size_t i = 0; i < cnt; i++)
	{
		sum_val += pow(current_val[i] - avg_val, 2);
	}
	//cout<<"sum_val : "<<sum_val<<endl;
	var_val = sum_val / cnt;
	std_val = sqrt(var_val);

	//cout << " avg_val : " << avg_val << endl;
	//cout << " sum_val : " << sum_val << endl;
	//cout << " var_val : " << var_val << endl;
	//cout << " std_val : " << std_val << endl << endl;

	//평균값이 설정한 값보다 작으면 다시 계산하자
	//set_point = 평균 + 표준편차 / 2;
	if (avg_val < set_point) {
		if (new_ver == 1)
			set_point = current_val[current_val.size() / 2];
		//cout << "set_point : " << set_point << endl;

		min_error = set_point;
		for (size_t i = 0; i < current_val.size(); i++)
		{
			current = current_val[i];
			error = abs(set_point - current);
			//cout << error << " : " << current << endl;
			if (min_error > error)
			{
				cout << i << " : " << current << endl;

				min_error = error;
				dst = LUT_canny[i].clone();
			}
		}
	}

	return set_point;
}

double img_std(Mat in, int th = 0)
{

	Mat src = in.clone();

	int w = src.size().width;

	int h = src.size().height;



	unsigned char* data = (unsigned char*)src.data;


	float aa[10] = { 0,1,2,3,4,5,6,7,8,9 };
	float avg_val = 0;
	float sum_val = 0;
	float var_val = 0;
	float std_val = 0;
	float num = 1;
	vector <float> data_val;
	for (int y = 0; y < h - 1; y++)
	{
		for (int x = 0; x < w - 1; x++)
		{
			int val = (int)data[y *  src.cols + x];
			if (val >= th) {
				avg_val = (float)(avg_val*(num - 1) + (float)val) / num;
				num++;
				data_val.push_back(val);
			}
		}
	}
	//for (int i = 0; i < 10; i++)
	//{
	//	if (aa[i] >= th) {
	//			avg_val = (float)(avg_val*(num-1) + (float)aa[i]) /num;
	//			num++;
	//			data_val.push_back(aa[i]);
	//			//cout << "avg_val : " << avg_val << endl;
	//		}
	//}

	//avg_val /= (float)num;
	//cout<<"avg_val : "<<avg_val<<endl;
	for (size_t i = 0; i < data_val.size(); i++)
	{
		sum_val += pow((float)data_val[i] - avg_val, 2);
	}
	//cout<<"sum_val : "<<sum_val<<endl;
	var_val = sum_val / num;
	std_val = sqrt(var_val);
	return (double)std_val;
}

double img_histo(Mat src, bool show_histo = false)
{
	//히스토그램 만들어서 차이값을 출력해보자
	struct histo_info {
		int val;
		int cnt = 0;
	};

	vector <histo_info> histo_list(256);

	for (int i = 0; i < histo_list.size(); i++)
	{
		histo_list[i].val = i;
		histo_list[i].cnt = 0;
	}

	int w = src.size().width;

	int h = src.size().height;

	unsigned char* data = (unsigned char*)src.data;



	double avg = 0.0;

	int num = 1;

	for (int y = 0; y < h - 1; y++)
	{

		for (int x = 0; x < w - 1; x++) {

			histo_list[data[y *  src.cols + x]].cnt++;

		}

	}
	///*
	int histSize = 256;

	/// Set the ranges ( for B,G,R) )
	float range[] = { 0, 256 };
	const float* histRange = { range };

	bool uniform = true; bool accumulate = false;

	Mat b_hist, g_hist, r_hist;

	/// Compute the histograms:
	calcHist(&src, 1, 0, Mat(), b_hist, 1, &histSize, &histRange, uniform, accumulate);

	// Draw the histograms for B, G and R
	int hist_w = 512; int hist_h = 400;
	int bin_w = cvRound((double)hist_w / histSize);

	Mat histImage(hist_h, hist_w, CV_8UC1, Scalar(0));

	/// Normalize the result to [ 0, histImage.rows ]
	//cout << b_hist << endl;
	normalize(b_hist, b_hist, 0, histImage.rows, NORM_MINMAX, -1, Mat());
	double normal_avg = 0;
	/// Draw for each channel
	for (int i = 1; i < histSize; i++)
	{

		line(histImage, Point(bin_w*(i - 1), hist_h - cvRound(b_hist.at<float>(i - 1))),
			Point(bin_w*(i), hist_h - cvRound(b_hist.at<float>(i))),
			Scalar(255), 2, 8, 0);
		normal_avg += (double)b_hist.at<float>(i - 1);
	}
	normal_avg /= (double)histSize;
	/// Display
	if (show_histo) {
		namedWindow("calcHist Demo", CV_WINDOW_AUTOSIZE);
		imshow("calcHist Demo", histImage);
	}
	//*/


	std::vector<histo_info>::const_iterator itc = histo_list.begin();
	int ditan = 0;
	while (itc != histo_list.end()) {
		if (itc->cnt < 1)
			itc = histo_list.erase(itc);
		else {
			++itc;
		}
	}
	vector <int> diff;

	for (int i = 1; i < histo_list.size(); i++)
	{
		diff.push_back(histo_list[i].val - histo_list[i - 1].val);
	}
	double avg_val = 0;
	int max_val = 0;
	for (int i = 0; i < diff.size(); i++)
	{
		//cout << i << " : " << diff[i] << endl;
		avg_val = (double)(avg_val*i + diff[i]) / (i + 1);
		if (max_val < diff[i])
			max_val = diff[i];
	}
	//cout << endl << endl;
	return normal_avg;
}
void good_threshold(Mat img_binary, Mat U8_img, Mat &dst, double th = 20)
{
	Mat gray_map;
	dst = img_binary.clone();




	//bitwise_and(img_binary, U8_img, gray_map);
	//
	//Mat img_labels, stats, centroids;
	//int numOfLables = connectedComponentsWithStats(img_binary, img_labels,
	//	stats, centroids, 8, CV_32S);
	//
	//
	//
	////라벨링 된 이미지에 각각 직사각형으로 둘러싸기 
	//int max_area = 0;
	//int max_idx = 1;
	//float avg_val = 0;
	//float num = 1;
	//vector<int> del_blob;
	//vector<float> blob;
	//
	//for (int j = 1; j < numOfLables; j++) {
	//	int LEFT = stats.at<int>(j, CC_STAT_LEFT);
	//	int TOP = stats.at<int>(j, CC_STAT_TOP);
	//	int WIDTH = stats.at<int>(j, CC_STAT_WIDTH);
	//	int HEIGHT = stats.at<int>(j, CC_STAT_HEIGHT);
	//	Mat tt = U8_img(Rect(LEFT, TOP, WIDTH, HEIGHT)).clone();
	//	double std_val = img_std(tt);
	//	img_histo(tt,true);
	//	
	//	//cout << j << "'s blob std_img : " << std_val << endl ;
	//	avg_val = (float)(avg_val*(num - 1) + (float)std_val) / num;
	//	num++;
	//	//cout << "std_avg : " << avg_val << endl << endl;
	//	//blob.push_back((float)std_val);
	//	//Mat otsu;
	//	//
	//	//threshold(tt, otsu, 0, 255, THRESH_BINARY_INV | THRESH_OTSU);
	//	//PrintFunc("otsu", otsu, 1);
	//	//
	//	//threshold(tt, otsu, std_val, 255, THRESH_BINARY_INV);
	//	//PrintFunc("std_val", otsu, 1);
	//	//
	//	//waitKey();
	//}
	//cout << "good_th : " << avg_val << endl << endl;
	//
	//for (int j = 0; j < blob.size(); j++) {
	//	if (blob[j] < avg_val)
	//		del_blob.push_back(j + 1);
	//}
	//
	//unsigned char* data = (unsigned char*)dst.data;
	//for (int y = 0; y<img_labels.rows; ++y) {
	//
	//	int *label = img_labels.ptr<int>(y);
	//	for (int x = 0; x < img_labels.cols; ++x) {
	//
	//		for (size_t i = 0; i<del_blob.size(); i++) {
	//			if (label[x] == del_blob[i]) {
	//				data[y *  dst.cols + x] = 0;
	//			}
	//		}
	//	}
	//}
	//PrintFunc("dst",dst,2);
	//waitKey();

}


struct best_focus_info {
	int idx;
	double nor_avg;
	double avg_val;
	double std_val;
	double adder;
	double edge_val;
	double modi_std_val;
	double laplasion_val;
	double laplasion_ori_val;
	double laplasion_back_val;
};
bool nor_val_min(best_focus_info a, best_focus_info b) {
	return (a.nor_avg < b.nor_avg);
}
bool cmp_nor_val(best_focus_info a, best_focus_info b) {
	return (a.nor_avg > b.nor_avg);
}
bool cmp_avg_val(best_focus_info a, best_focus_info b) {
	return (a.avg_val > b.avg_val);
}
bool cmp_std_val(best_focus_info a, best_focus_info b) {
	return (a.std_val > b.std_val);
}
bool cmp_std_val_min(best_focus_info a, best_focus_info b) {
	return (a.std_val < b.std_val);
}
bool cmp_modi_std_val(best_focus_info a, best_focus_info b) {
	return (a.modi_std_val > b.modi_std_val);
}
bool cmp_lapla_val(best_focus_info a, best_focus_info b) {
	return (a.laplasion_val > b.laplasion_val);
}
bool cmp_adder_min(best_focus_info a, best_focus_info b) {
	return (a.adder < b.adder);
}
bool cmp_adder(best_focus_info a, best_focus_info b) {
	return (a.adder > b.adder);
}
bool cmp_edge_val(best_focus_info a, best_focus_info b) {
	return (a.edge_val > b.edge_val);
}
bool cmp_edge_val_min(best_focus_info a, best_focus_info b) {
	return (a.edge_val < b.edge_val);
}
bool cmp_lapla_back_val(best_focus_info a, best_focus_info b) {
	return ((a.laplasion_back_val) >(b.laplasion_back_val));
}

bool cmp_lapla_back_val_min(best_focus_info a, best_focus_info b) {
	return (a.laplasion_back_val < b.laplasion_back_val);
}
bool cmp_lapla_ori_val(best_focus_info a, best_focus_info b) {
	return (a.laplasion_ori_val > b.laplasion_ori_val);
}

Rect find_best_focus(vector<Mat> &src, Mat &dst, Mat &and_img, double dft_thresh_min, double dft_thresh_max, double dft_radius, string FolderPath, int *best_focus_idx, double *defect_val, double *defect_std_val, double *defect_ratio, double *canny_edge_ratio, int *invi_three)
{
	dst = src[0].clone();



	double dft_thresh = 0.581;

	double factor_value = (dft_thresh_max - dft_thresh_min) / (src.size());

	//Mat and_img;// = Mat::zeros(src[0].size(), CV_8UC1);

	Mat egde_sum = Mat::zeros(src[0].size(), CV_8UC1);
	Mat gray_egde_sum = Mat::zeros(src[0].size(), CV_8UC1);
	Mat img_gray, inverseTransform, U8char_img, thresh, tmp;
	//otsu_sum += 255;
	vector <Mat> canny_list(src.size());
	//vector <Mat> DFT_list;
	int zoom = 2;
	//°¡ÀåÅ« ÀÌ¹° Ã£¾Æº¸±â±á±á±×±â¤Ñ±â¤Ñ±â¤Ñ
#ifdef DEBUG_MODE
	string debug_img_path = FolderPath + "\\_img_and.bmp";
	and_img = imread(debug_img_path);

#endif
	if (and_img.empty())
	{

		and_img = Mat::zeros(src[0].size(), CV_8UC1);
		vector<Mat> edge_list(src.size());
		int start_num = src.size() / 4;
#pragma omp parallel for
		for (int i = start_num; i < src.size(); i++)
		{
			cout << "defect part processing..... " << i + 1 << " / " << src.size() << endl;
			Mat img_gray, inverseTransform, U8char_img, tmp;
			cvtColor(src[i], img_gray, CV_BGR2GRAY);

			dft_thresh = (factor_value*i) + dft_thresh_min;

			resize(img_gray, img_gray, Size(img_gray.cols / zoom, img_gray.rows / zoom));

			dft_background_remove(img_gray, inverseTransform, dft_radius, dft_thresh);
			//cout << dft_radius << endl;
			//cout << dft_thresh << endl;
			inverseTransform.convertTo(U8char_img, CV_8UC1, 255.0);
			gamma_func(U8char_img, tmp, 0.6);
			//PrintFunc("U8char_img", tmp, 2);
			//DFT_list.push_back(U8char_img.clone());
			//PrintFunc("gamma_func", U8char_img, 4);
			blur(tmp, tmp, Size(11, 11));
			int dd = good_canny(tmp, tmp, 5000, 0, 15000, 35, 85, 5, i);
			//PrintFunc("good_canny", tmp, 2);
			//cout << dd << endl;
			//auto_canny(tmp, tmp, 0.7);
			//PrintFunc("auto_canny", tmp, 3);
			//threshold(tmp, tmp, 0, 255, THRESH_BINARY_INV | THRESH_OTSU);
			resize(tmp, tmp, src[0].size());


			Mat cir_tmp = Mat::zeros(tmp.size(), CV_8UC1);
			circle(cir_tmp, Point(tmp.cols / 2, tmp.rows / 2), tmp.size().width / 2 * 1.00, 255, -1);
			bitwise_and(cir_tmp, tmp, tmp);

			canny_list[i] = (tmp.clone());
			cout << "tmp : " << tmp.size() << endl;
			//OLDE Æ÷Ä¿½º¸¦ À§ÇÑ ºÎºÐ
			//cout << avg_img(U8char_img) << endl;
			//waitKey();

		}

		cout << "Multi done!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!" << endl;
		and_img = Mat::zeros(src[0].size(), CV_8UC1);
		for (size_t i = start_num; i < canny_list.size(); i++)
		{
			cout << "and_img : " << and_img.size() << endl;
			cout << "canny_list : " << canny_list[i].size() << endl;
			//PrintFunc("and_img", and_img, 4);
			//PrintFunc(" canny_list[i]", canny_list[i], 4);
			//waitKey();
			and_img += canny_list[i];
		}

#ifdef DEBUG_MODE
		string debug_img_path = FolderPath + "\\_img_and.bmp";
		imwrite(debug_img_path, and_img.clone()); // ¿µ»ó ÀúÀå
#endif
	}
	else
	{
		cvtColor(and_img, and_img, CV_BGR2GRAY);
	}
	//proposal_img = and_img.clone();
	//PrintFunc("otsu_sum", otsu_sum, 2);
	PrintFunc("and_img", and_img, 3);
	medianBlur(and_img, and_img, 3);
	dilate(and_img, and_img, kernel, Point(-1, -1), 5);
	PrintFunc("and_img", and_img, 3);
	Rect Blob_ROI = extractMainLabel(and_img, dst);
	Rect ROI_ori = Blob_ROI;
	Rect ROI = Blob_ROI;
	Mat ex_val_img = dst(Blob_ROI).clone();

	delete_by_size(ex_val_img, ex_val_img, 0, 100000, ex_val_img, false, -1);

	PrintFunc("ex_val_img", ex_val_img, 3);
	//ÀÏÁ¤ÇÑ »óÀ§ÀÇ µ¥ÀÌÅÍ¸¸ °¡Á®°¡°Ú´Ù.
	int scan_size = (double)(src.size() * 2) / 3 - 1;
	//8¹æÇâÀ¸·Î ÁÂÇ¥ ¹Ù²Ù¸é¼­ ¤¡¤¡
	//ÇÑ¹ø¸¸ ÇÏ´Â°Ô ¾Æ´Ï¶ó ROI »ìÂ¦ ¹Ù²ã°¡¸é¼­ ÇØº¸ÀÚ
	vector <best_focus_info> repeat_info;
	vector <Mat> lapla_img;
	//0.25 -> 0.5 = O:\SDOPTICS\WDMS\Images\Data\9\S2-13\A-S2-13-002
	double resize_ratio = 0.25;
	double roi_ratio = avg_img(ex_val_img);
	cout << roi_ratio << endl;
	for (int i = 0; i < src.size(); i++)
	{
		Mat input_gray, blur_img, otsu_img;
		cvtColor(src[i], input_gray, CV_BGR2GRAY);
		int blur_kernel = (int)((double)(MIN(25, src.size() / 2) - i) / 1) * 2 + 1;

		cout << blur_kernel << endl;
		//O:\SDOPTICS\WDMS\Images\Data\10\S1-0014\F-S1-0014-002 : 9 -> 11
		blur_kernel = MIN(blur_kernel, 11);

		blur_kernel = MAX(blur_kernel, 1);
		blur(input_gray, input_gray, Size(blur_kernel, blur_kernel));
		if (roi_ratio < 60)
			blur(input_gray, input_gray, Size(3, 3));
		cv::resize(input_gray, input_gray, Size(), resize_ratio, resize_ratio, CV_INTER_AREA);

		Mat sobel, sobel_ori;
		Mat sobelX;
		Mat sobelY;

		Laplacian(input_gray, sobelX, CV_16S, 3, 1, 0);
		convertScaleAbs(sobelX, sobel);
		lapla_img.push_back(sobel);
	}

	for (int x_offset = 1 - 1; x_offset < 2 - 1; x_offset++)
	{
		for (int y_offset = 1 - 1; y_offset < 2 - 1; y_offset++)
		{
			for (int ex_offset = 0; ex_offset < 2; ex_offset++) {

				vector<best_focus_info> img_info(src.size());
				ROI = Blob_ROI;
				//ÀÏ´Ü X,Y¸¸ ¹Ù²ãº¸ÀÚ

				ROI_ori = ROI;

				//cout << "ROI : " << ROI << endl;
				//waitKey();


				Mat back_ground;
				if (ROI.width == 0 || ROI.height == 0)
				{
					ROI.width = src[0].size().width;
					ROI.height = src[0].size().height;
				}

				double roi_ratio = avg_img(ex_val_img);
				double ex_val = 50; //10 <- 0 : C-G-F2-001	  ROI_RATIO = 110
									//						//10 <- 0 : E-S1-0004-001 ROI_RATIO = 125
									//if (roi_ratio < 120)
									//	ex_val = 1;
									//if (roi_ratio > 180)
									//	ex_val = 200;		//50 <- 150 : H-S1-972-002
				double ROI_SIZE;
				if (0 < roi_ratio && roi_ratio < 50)
					ROI_SIZE = 10;
				else if (50 < roi_ratio && roi_ratio < 100)
					ROI_SIZE = 10;
				else if (101 < roi_ratio && roi_ratio < 140)
					ROI_SIZE = 10;
				else if (141 < roi_ratio && roi_ratio < 180)
				{
					if (abs(ex_val_img.cols - ex_val_img.rows) < 100)
						ROI_SIZE = 10;
					else
						ROI_SIZE = 10;
				}

				else {
					//O:\SDOPTICS\WDMS\Images\Data\12\S1-0012\E-S1-0012-002 : 10 -> 200 -> 80
					//O:\SDOPTICS\WDMS\Images\Measure\I-S2-0025-202 : 200 -> 80
					if (abs(ex_val_img.cols - ex_val_img.rows) < 100)
						ROI_SIZE = 80;
					else
						ROI_SIZE = 10;
				}
				//ROI_SIZE = 10;

				ex_val = ROI_SIZE + (40 * ex_offset);

				ROI_ori.x -= ex_val * 1;
				ROI_ori.x = MAX(ROI_ori.x, 1);
				ROI_ori.y -= ex_val * 1;
				ROI_ori.y = MAX(ROI_ori.y, 1);
				ROI_ori.width += ex_val * 2;
				if ((ROI_ori.width + ROI_ori.x) > dst.size().width)
					ROI_ori.width = abs(dst.size().width - ROI_ori.x);
				ROI_ori.height += ex_val * 2;
				if ((ROI_ori.height + ROI_ori.y) > dst.size().height)
					ROI_ori.height = (dst.size().height - ROI_ori.y);

				//cout << ROI_ori << endl;

				cout << "small : " << ex_val << endl;
				ex_val += 150;//(50 * (1+(double)ex_offset/ (double)5));		//150 <- 350 : H-S1-972-002
				cout << "big : " << ex_val << endl;
				ROI.x -= ex_val * 1;
				ROI.x = MAX(ROI.x, 1);
				ROI.y -= ex_val * 1;
				ROI.y = MAX(ROI.y, 1);
				ROI.width += ex_val * 2;
				if ((ROI.width + ROI.x) > dst.size().width)
					ROI.width = abs(dst.size().width - ROI.x);
				ROI.height += ex_val * 2;
				if ((ROI.height + ROI.y) > dst.size().height)
					ROI.height = (dst.size().height - ROI.y);


				ROI_ori.x *= (double)resize_ratio;
				ROI_ori.y *= (double)resize_ratio;
				ROI_ori.width *= (double)resize_ratio;
				ROI_ori.height *= (double)resize_ratio;


				ROI.x *= (double)resize_ratio;
				ROI.y *= (double)resize_ratio;
				ROI.width *= (double)resize_ratio;
				ROI.height *= (double)resize_ratio;
				//cout << ROI_ori << endl;
				//	cout << ROI << endl;
				double avg_avg_val = 0;
				for (int i = 0; i < src.size(); i++)
				{

					Mat ori, input_gray;
					//È«½ÂÇ¥ Â÷Àå´Ô ¹æ½Ä Ãß°¡¿ë
					//1/4·Î ³ª´²¼­ ÇÏÀÌÇÁ¸®Äö½Ã ÀÚ¸£°í
					//¶óÇÃ¶ó½Ã¾ÈÀÇ Æò±Õ°ªÀ» ³»¼­ º¯°îÁ¡ Ã£±â±â±â
					//cout << "otsu_img ÀÌ¹ÌÁö : " << avg_img(otsu_img) << endl;

					//input_gray = DFT_list[i](ROI).clone();
					//gray·Î Å×½ºÆ®
					//cout << "lapla_img[i] size" << lapla_img[i].size() << endl;
					input_gray = lapla_img[i](ROI).clone();

					//PrintFunc("input1", input_gray, 1);

					img_info[i].laplasion_val = avg_img(input_gray);
					img_info[i].avg_val = img_std(input_gray);

					input_gray = lapla_img[i](ROI_ori).clone();

					img_info[i].laplasion_ori_val = avg_img(input_gray);
					img_info[i].nor_avg = img_std(input_gray);

					img_info[i].laplasion_back_val = (img_info[i].laplasion_val - img_info[i].laplasion_ori_val);
					img_info[i].std_val = img_info[i].avg_val - img_info[i].nor_avg;

					//PrintFunc("input2", input_gray, 1);

					img_info[i].idx = i;
					cout << i + 1 << "lap : " << img_info[i].laplasion_back_val << ", std : " << img_info[i].std_val << endl;
					//waitKey();
				}

				int avg_factor = 1;

				double min_val = img_info[0].laplasion_val;
				double max_val = img_info[img_info.size() - 1].laplasion_val;
				//Á¤±ÔÈ­ ¸Ç~
				//sort(img_info.begin(), img_info.end(), cmp_lapla_val);
				if (roi_ratio < 40) {
					max_val = img_info.front().laplasion_val;
					min_val = 0; img_info.back().laplasion_val;
					for (int i = 0; i < img_info.size(); i++)
					{
						img_info[i].laplasion_val -= min_val;
						img_info[i].laplasion_val /= (max_val - min_val);
					}

					//sort(img_info.begin(), img_info.end(), cmp_lapla_ori_val);
					max_val = img_info.front().laplasion_ori_val;
					min_val = 0; img_info.back().laplasion_ori_val;
					for (int i = 0; i < img_info.size(); i++)
					{
						img_info[i].laplasion_ori_val -= min_val;
						img_info[i].laplasion_ori_val /= (max_val - min_val);
						img_info[i].laplasion_back_val = (img_info[i].laplasion_val - img_info[i].laplasion_ori_val);
					}

					//Á¤±ÔÈ­ ¸Ç~
					//sort(img_info.begin(), img_info.end(), cmp_avg_val);
					max_val = img_info.front().avg_val;
					min_val = 0; img_info.back().avg_val;
					for (int i = 0; i < img_info.size(); i++)
					{
						img_info[i].avg_val -= min_val;
						img_info[i].avg_val /= (max_val - min_val);
					}

					//sort(img_info.begin(), img_info.end(), cmp_nor_val);
					max_val = img_info.front().nor_avg;
					min_val = 0; img_info.back().nor_avg;
					for (int i = 0; i < img_info.size(); i++)
					{
						img_info[i].nor_avg -= min_val;
						img_info[i].nor_avg /= (max_val - min_val);
						img_info[i].std_val = (img_info[i].avg_val - img_info[i].nor_avg);
						//cout << i + 1 << "lap : " << img_info[i].laplasion_back_val << ", std : " << img_info[i].std_val << endl;
					}
				}
				int start_idx = 0;

				//ÇöÀç±îÁö´Â diff °ªÀÌ ³ôÀº°ÍÀÌ º£½ºÆ® Æ÷Ä¿½ºÀÓ
				//¸¸¾à¿¡ diff °ªÀÌ ¶È°°Àº ¾ÆÀÌµéÀÌ ÀÖ´Ù¸é?!
				//std_valÀ» ºñ±³ÇØ¼­ ´õÅ«°É·Î °áÁ¤ÇÏÀÚ
				//¾ÈµÇ´Â ½Ã·á°¡ ÀÖÀ½.... 180306
				//±×·¡¼­ Æò±Õ°ªÀ» absÇÏ°í std¸¦ ±¸ÇßÀ»¶§ °¡Àå °ªÀÌ Å« °ÍÀÌ º£Æ÷¿¡ ¸¶Áð°Å °°´Ù.
				vector <best_focus_info> edge_tmp, sobel_tmp;

				//STD_VALÀÌ ÀüºÎ À½¼ö¸é ¿ªÀ¸·Î ¼ÒÆÃÇØÁÖ´Â°Ô ÁÁÀº°Å °°À½
				bool inverse_std_flag = true;//À½¼ö
				for (int i = 0; i < img_info.size(); i++)
				{
					if (img_info[i].laplasion_back_val < 0)
						inverse_std_flag = false;//¾ç¼ö°¡ÀÖ´Ù
				}
				img_info.erase(img_info.begin(), img_info.begin() + start_idx);


				//E:\img\samsung_note_8\Ver_G\G-S1-0010-002
				//STD_VALÀÌ ÀüºÎ À½¼ö¸é ¿ªÀ¸·Î ¼ÒÆÃÇØÁÖ´Â°Ô ÁÁÀº°Å °°À½
				if (inverse_std_flag == false)
					sort(img_info.begin(), img_info.end(), cmp_lapla_back_val_min);
				else {
					//전부 양수일때
					//
					for (int i = 0; i < img_info.size(); i++)
					{
						img_info[i].std_val *= -1;
					}
					sort(img_info.begin(), img_info.end(), cmp_lapla_back_val);
				}

				//img_info.erase(img_info.begin() + scan_size, img_info.end());
				//sort(img_info.begin(), img_info.end(), cmp_lapla_back_val_min);
				sort(img_info.begin(), img_info.end(), cmp_std_val_min);


				int best_idx = img_info[0].idx;
				//º¸´Ï±ñ ÃÖ¼Ò °ªÇÏ°í ÃÖ´ë°ª Áß¿¡  std°¡ Å«°Ô º£Æ÷ÀÎµí
				if (img_info[0].std_val > img_info[scan_size - 1].std_val)
					best_idx = img_info[0].idx;
				cout << "best_idx : " << best_idx + 1 << endl;
				repeat_info.push_back(img_info[0]);

			}
		}
	}

	//#ifdef PRINT_DEBUG
	//·Î±× ÀúÀå¿ë
	char fname[300];


	time_t     now = time(0); //ÇöÀç ½Ã°£À» time_t Å¸ÀÔÀ¸·Î ÀúÀå
	struct tm  tstruct;
	char       buf[80];
	tstruct = *localtime(&now);
	strftime(buf, sizeof(buf), "%Y-%m-%d", &tstruct); // YYYY-MM-DD.HH:mm:ss ÇüÅÂÀÇ ½ºÆ®¸µ
	cout << FolderPath.c_str() << endl;
	cout << buf << endl;
	FILE	*fp_in, *fp_log;
	sprintf(buf, "BestFocusLOG");
	sprintf(fname, "%s/%s.csv", FolderPath.c_str(), buf);
	cout << "fname : " << buf << endl;
	fopen_s(&fp_log, fname, "w");
	if (!fp_log) {
		//waitKey();
		exit(-1);
	}
	fprintf(fp_log, "####################################################\n");
	fprintf(fp_log, "#######   Company:    SD Optics Inc.          ######\n");
	fprintf(fp_log, "#######   File Name:  BestFocusLOG.csv		   ######\n");
	fprintf(fp_log, "#######   Version:    3.00                    ######\n");
	fprintf(fp_log, "####################################################\n\n\n");

	fprintf(fp_log, "idx,sobel_val,avg_val,adder,modi_std_val,laplasion_val,laplasion_ori_val,laplasion_back_val,std_val,edge_val\n");
	for (int i = 0; i < repeat_info.size(); i++) {
		fprintf(fp_log, "%d, ", repeat_info[i].idx + 1);
		fprintf(fp_log, "%.05f, ", repeat_info[i].nor_avg);
		fprintf(fp_log, "%.05f, ", repeat_info[i].avg_val);
		fprintf(fp_log, "%.05f, ", repeat_info[i].adder);
		fprintf(fp_log, "%.05f, ", repeat_info[i].modi_std_val);
		fprintf(fp_log, "%.05f, ", repeat_info[i].laplasion_val);
		fprintf(fp_log, "%.05f, ", repeat_info[i].laplasion_ori_val);
		fprintf(fp_log, "%.05f, ", repeat_info[i].laplasion_back_val);
		fprintf(fp_log, "%.05f, ", repeat_info[i].std_val);
		fprintf(fp_log, "%.05f ", repeat_info[i].edge_val);
		fprintf(fp_log, "\n");
	}

	fprintf(fp_log, "\n");


	//#endif

	int best_idx = 0;
	int avg_idx = 0;
	int min_val_idx = 0;
	double max_edge_std_val = 0;
	int std_idx = 0;
	double max_std_val = 0;
	int Max_cnt_idx = 0;
	int max_val = 0;
	if (1) //idxÀÇ Æò±Õ ¹æ¹ý
	{
		for (int i = 0; i < repeat_info.size(); i++) {
			avg_idx += repeat_info[i].idx;
		}
		avg_idx /= (double)repeat_info.size();

		PrintFunc(" avg idx", src[avg_idx], 3);
		cout << "avg_idx : " << avg_idx + 1 << endl;
	}
	if (1)//edge_val ÃÖ¼ÒÀÇ ¹æ¹ý
	{
		sort(repeat_info.begin(), repeat_info.end(), cmp_lapla_back_val_min);
		min_val_idx = repeat_info[0].idx;
		PrintFunc(" lapla_min sort", src[min_val_idx], 3);
		cout << "lapla_min_idx : " << min_val_idx + 1 << endl;
		fprintf(fp_log, "lapla_min_idx : %d \n", min_val_idx + 1);
		max_edge_std_val = repeat_info.back().std_val;
	}
	if (1)//std_val ÃÖ´ëÀÇ ¹æ¹ý
	{
		sort(repeat_info.begin(), repeat_info.end(), cmp_std_val_min);
		max_std_val = repeat_info[0].std_val;
		std_idx = repeat_info[0].idx;
		PrintFunc("std val sort", src[std_idx], 3);
		cout << "std idx : " << std_idx + 1 << endl;
		fprintf(fp_log, "std_idx : %d \n", std_idx + 1);
	}

	if (1) //idx sorting ¹æ¹ý
	{
		vector<int> cnt_idx(src.size());
		vector<double> std_idx_val;
		std_idx_val.assign(src.size(), 0);
		for (int i = 0; i < repeat_info.size(); i++) {
			cnt_idx[repeat_info[i].idx]++;
			//°¹¼ö°¡ °°À»¶§¸¦ ´ëºñÇØ¼­ laplasion°ªÀÌ ÀÛÀº°É·Î ÇÏ°Í´Ù.
			if (std_idx_val[repeat_info[i].idx] > repeat_info[i].laplasion_back_val)
			{
				std_idx_val[repeat_info[i].idx] = repeat_info[i].laplasion_back_val;
			}
		}

		for (int i = 0; i < cnt_idx.size(); i++) {
			if (max_val <= cnt_idx[i])
			{
				if (max_val == cnt_idx[i])
				{

					//fprintf(fp_log, "max_val == cnt_idx\n");
					if (std_idx_val[Max_cnt_idx] < std_idx_val[i])
					{
						Max_cnt_idx = i;
						max_val = cnt_idx[i];
					}

				}
				else {
					Max_cnt_idx = i;
					max_val = cnt_idx[i];
				}
			}
		}

		PrintFunc(" max idx", src[Max_cnt_idx], 3);
		cout << "max_idx : " << Max_cnt_idx + 1 << endl;
		fprintf(fp_log, "max_cnt_idx : %d \n", Max_cnt_idx + 1);
	}
	if (1) //stdÀÇ ÃÖ´ë¿¡¼­ edge ÃÖ¼Ò
	{
		//scan_size;
		//sort(repeat_info.begin(), repeat_info.end(), cmp_std_val);
		//repeat_info.erase(repeat_info.begin() + scan_size, repeat_info.end());
		//sort(repeat_info.begin(), repeat_info.end(), cmp_lapla_back_val_min);
		//best_idx = repeat_info[0].idx;
		//PrintFunc("std sort edge_min", src[best_idx], 3);
		//cout << "sort edge : " << best_idx + 1 << endl;
	}
	//waitKey();


	//´ëºÎºÐÀ¸·Î ¼ö·ÅÇÏ´Â °ªÀº Max_cnt_idx·Î ÇØÁÖ°í
	//´ëºÎºÐÀÌ ¾Æ´Ï¶ó¸é ÃÖ¼Ò°ªÀ¸·Î º£½ºÆ® Æ÷Ä¿½º·Î º¸ÀÚ
	if (max_val >= repeat_info.size() / 2)
		best_idx = std_idx;
	else
		best_idx = std_idx;// min_val_idx;

	if (max_edge_std_val == max_std_val) {
		//Æ¯ÀÌ ÄÉÀÌ½º ÆíÂ÷°¡ Å«µ¥ edge_val ±îÁö Å©¸é ÀÌ°Ô º£½ºÆ®Æ÷Ä¿½º´õ¶ó
		//E-S1-0012-002
		cout << "max_edge_std_val == max_std_val" << endl;
		//sort(repeat_info.begin(), repeat_info.end(), cmp_std_val);
		best_idx = std_idx;
		fprintf(fp_log, "max_edge_std_val == max_std_val\n");
	}

	fprintf(fp_log, "scan_size : %d \n", scan_size);
	fprintf(fp_log, "%d 's img select\n", best_idx + 1);
	fclose(fp_log);

	//if (img_info[0].edge_val > img_info[img_info.size() - 1].edge_val)
	//	best_idx = img_info[0].idx;
	//else
	//	best_idx = img_info[img_info.size() - 1].idx;
	//

	//medianBlur(otsu_sum, otsu_sum,5);
	Mat kern = Mat::ones(15, 15, CV_32F);
	Mat OLDE_img, temp;
	kern /= 255;
	//PrintFunc("OLDE_img", otsu_sum, 2);
	//waitKey();
	//PrintFunc("max_idx", src[max_idx], 2);

	cout << best_idx + 1 << "'s img select" << endl;
	//ºÒ·® À¯Çü ±¸ºÐÀ» À§ÇÑ Part
	ROI = Blob_ROI;
	int ex_val = 150; //0.5
	ROI.x -= ex_val;
	ROI.x = MAX(ROI.x, 1);
	ROI.y -= ex_val;
	ROI.y = MAX(ROI.y, 1);
	ROI.width += ex_val * 2;
	if ((ROI.width + ROI.x) > dst.size().width)
		ROI.width = abs(dst.size().width - ROI.x);
	ROI.height += ex_val * 2;
	if ((ROI.height + ROI.y) > dst.size().height)
		ROI.height = (dst.size().height - ROI.y);



	cvtColor(src[best_idx](ROI).clone(), img_gray, CV_BGR2GRAY);
	Mat tttt, back_ground, edd;
	erode(dst(ROI).clone(), tttt, kernel, Point(-1, -1), 5);
	delete_by_size(tttt, edd, 0, 999999, tttt, false, -1);

	//white
	threshold(img_gray, back_ground, 0, 255, THRESH_BINARY | THRESH_OTSU);

	//black
	threshold(img_gray, tttt, 0, 255, THRESH_BINARY_INV | THRESH_OTSU);

	bitwise_and(back_ground, edd, back_ground);
	bitwise_and(tttt, edd, tttt);

	Point maxloc;
	maxloc.x = ROI.width / 2;
	maxloc.y = ROI.height / 2;
	Mat tmp_white = back_ground.clone();
	Mat tmp_black = tttt.clone();
	PrintFunc("tmp_white", back_ground, 3);
	PrintFunc("tmp_black", tttt, 3);
	bitwise_and(tmp_white, img_gray, tmp_white);
	bitwise_and(tmp_black, img_gray, tmp_black);
	PrintFunc("white", tmp_white, 3);
	PrintFunc("black", tmp_black, 3);


	*invi_three = 0;
	bool black_flag = false;
	double tmp_white_avg = avg_img_th(tmp_white, 0);
	double tmp_black_avg = avg_img_th(tmp_black, 0);
	double tmp_gray_avg = avg_img_th(img_gray - edd, 0);
	//cout << "avg_img_th(tmp_white,1) : " << avg_img_th(tmp_white, 0) << endl;
	//cout << "avg_img_th(tmp_black,1) : " << avg_img_th(tmp_black, 0) << endl;
	//cout << "avg_img_th(img_gray,1) : " << avg_img_th(img_gray - edd, 0) << endl;
	//cout << "avg_img(back_ground) - avg_img(tttt) : " << abs(avg_img(back_ground)) << ", " << (avg_img(tttt)) << endl;


	if (tmp_white_avg  > 200) {
		back_ground = back_ground.clone();
		black_flag = false;
		if (avg_img(back_ground) < 1)
		{
			back_ground = tttt.clone();
			black_flag = true;
		}

		*invi_three = 1;
	}
	else {
		//Gray 평균 밝기보다 차이가 많이 나는 걸로 하양 검정 본다
		if (abs(tmp_white_avg - tmp_gray_avg) > abs(tmp_black_avg - tmp_gray_avg))
		{
			if (avg_img(back_ground) < 1)
			{
				back_ground = tttt.clone();
				black_flag = true;
			}
			else {
				//인비져블이나 삼진화가 이게 더 이득인듯
				*invi_three = 1;
				back_ground = back_ground.clone() + tttt.clone();
				black_flag = false;
			}
		}
		else
		{
			back_ground = tttt.clone();
			black_flag = true;
		}

		//만약 검정의 픽셀이 1보다 작으면 걍 하양으로
		//if (avg_img(tttt) < 1)
		//{
		//	back_ground = back_ground.clone();
		//	black_flag = false;
		//}
	}
	//만약 하양의 픽셀이 1보다 작으면 걍 블랙으로


	PrintFunc("back_ground", back_ground, 3);
	//waitKey();


	//bitwise_and(back_ground, img_gray, img_gray);






	*best_focus_idx = best_idx;



	//±â¿ÕÀÌ¸é Á¤È®ÇÒ¼ö·Ï ÁÁÀÚ³à
	//if (*defect_val > 85)
	//{
	//	cvtColor(src[best_idx](ROI).clone(), img_gray, CV_BGR2GRAY);
	//	img_gray = abs(255 - img_gray);
	//	bitwise_and(back_ground, img_gray, img_gray);
	//}

	Mat Inv_back, canny_line;
	threshold(back_ground, Inv_back, 0, 255, THRESH_BINARY_INV | THRESH_OTSU);

	PrintFunc("img_gray-canny_line", img_gray, 1);
	auto_canny(img_gray, canny_line, 0.9);
	PrintFunc("auto_canny", canny_line, 1);
	bitwise_and(canny_line, back_ground, canny_line);
	PrintFunc("canny_line", canny_line, 1);

	img_gray -= Inv_back;
	//PrintFunc("Inv_backInv_back", Inv_back, 3);

	//auto_canny(back_ground, canny_line, 1);
	//dilate(canny_line, canny_line, kernel,Point(-1,-1),5);
	//PrintFunc(" canny_line", canny_line, 1);




	//È­ÀÌÆ®ÀÎ°æ¿ì¿¡ ÀâÀ½¶§¹®¿¡ ´Ù½Ã ÇØ¾ß ÇÏ¸®µµ
	Mat white_tmp, black_tmp, color_tmp;
	//PrintFunc("black_tmp", black_tmp, 3);
	threshold(img_gray, white_tmp, 200, 255, THRESH_BINARY);
	threshold(img_gray, black_tmp, 50, 255, THRESH_BINARY_INV);
	bitwise_and(white_tmp, back_ground, white_tmp);
	bitwise_and(black_tmp, back_ground, black_tmp);
	//PrintFunc("white_tmp", white_tmp, 3);
	//PrintFunc("black_tmp", black_tmp, 3);
	//PrintFunc("img_grayimg_grayimg_grayimg_grayimg_grayimg_grayimg_gray", img_gray, 3);

	//cout << "avg_white : " << avg_img(white_tmp) << endl;
	//cout << "avg_black : " << avg_img(black_tmp) << endl;
	if (!black_flag)
	{

		cout << "white bigger " << endl;
		img_gray = abs(255 - img_gray.clone());
	}
	else
		black_flag = true;
	adaptiveThreshold(img_gray, tttt, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY_INV, 371, 5);
	if (black_flag)
	{
		cout << "black bigger " << endl;
		tttt += white_tmp;
	}

	PrintFunc(" avg_img_th(tttt", tttt, 1);
	//erode(tttt, back_ground, kernel, Point(-1, -1), 10);
	//
	cvtColor(src[best_idx](ROI).clone(), img_gray, CV_BGR2GRAY);

	//bitwise_and(tttt, Inv_back, Inv_back);
	//
	//delete_by_size(tttt, canny_line, 0, 155555, tttt, false, -1);
	//auto_canny(canny_line, canny_line, 1);
	//dilate(canny_line, canny_line, kernel, Point(-1, -1), 3);
	//delete_by_size(canny_line, canny_line, 0, 155555, tttt, false, -1);
	//erode(canny_line, canny_line, kernel, Point(-1, -1), 3);
	//auto_canny(canny_line, canny_line, 1);
	//dilate(canny_line, canny_line, kernel, Point(-1, -1), 3);
	//
	//Inv_back -= canny_line;




	//PrintFunc("canny_edge_ratio", canny_line, 1);
	//PrintFunc("tttt", tttt, 1);
	tttt = back_ground.clone();
	//PrintFunc("tttt", tttt, 1);
	*canny_edge_ratio = (double)pixel_cnt(canny_line) / (pixel_cnt(back_ground) + 1);
	if (*canny_edge_ratio > 0.1) {
		tttt = back_ground.clone();
	}

	Mat roi_i = tttt(Rect(ex_val, ex_val, Blob_ROI.width, Blob_ROI.height)).clone();
	*defect_ratio = avg_img(roi_i);
	cout << "dst.ROI : " << *defect_ratio << endl;

	if (*canny_edge_ratio > 0.1) {
		erode(tttt, tttt, kernel, Point(-1, -1), 2);
	}
	bitwise_and(tttt, img_gray, img_gray);

	//PrintFunc("dv sv", img_gray, 1);

	*defect_val = avg_img_th(img_gray, 1);
	*defect_std_val = img_std(img_gray, 1);

	cout << "defect_std_val : " << *defect_std_val << endl;
	if (best_idx < 4) {

		cout << "OLED??" << endl;
		cout << ROI << endl;
		if (*defect_ratio > 1) {
			if (ROI.width < 400 && ROI.height < 400)
				*defect_val = 1;
		}
	}
	dst = src[best_idx].clone();
	//waitKey();
	return ROI;
}

void black_defect_process(Mat src, Mat canny, Mat &dst, double adap_th1, double adap_th2)
{
	Mat U8char_img = src.clone();
	Mat test, thresh_image, defect, fat_th_img, thin_th_img;
	////////////////////////////////////////////////////////////////////
	//ºÒ·®À» Ã£¾Æº¸ÀÚ
	//adap_th2 = 20;
	test = canny.clone();
	dilate(test, test, kernel, Point(-1, -1), 3);
	if (adap_th2 < 4)
		adap_th2 = 5;
	if ((int)adap_th1 % 2 == 0)
		adap_th1++;
	Mat fe = test.clone();
	for (int i = adap_th2; i > 1; i -= 1) {
		Mat blur_img;
		blur(U8char_img, blur_img, Size(5, 5));
		adaptiveThreshold(blur_img, thresh_image, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY_INV, adap_th1, 1 + i);//ADAPTIVE_THRESH_GAUSSIAN_C
		medianBlur(thresh_image, thresh_image, 5);

		bitwise_and(thresh_image, test, test);
		//erode(fe, fe, kernel);
		flood_extract(thresh_image, fe, fe);
		//PrintFunc("fe", fe,3);
		//PrintFunc("test", test, 3);
		//PrintFunc("thresh_image", thresh_image, 3);
		//waitKey();
		dilate(test, test, kernel, Point(-1, -1), 3);
	}

	erode(test, test, kernel, Point(-1, -1), 3);
	bitwise_and(thresh_image, test, thresh_image);
	defect = thresh_image.clone();
	//	debug_thresh = defect.clone();
	fat_th_img = defect.clone();
	PrintFunc("fat_th_img", fat_th_img, 2);
	//PrintFunc("flood_extract", fe, 2);
	dst = fat_th_img.clone();

	//
	//Mat before, result;
	//for (int i = 7; i > 2; i--)
	//{
	//	int th1 = 8 * 2 + 3;
	//	int th2 = i;
	//	adaptiveThreshold(U8char_img, thresh_image, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY_INV, th1, th2);//ADAPTIVE_THRESH_GAUSSIAN_C
	//	medianBlur(thresh_image, thresh_image, 9);
	//	if (before.empty())
	//		before = thresh_image.clone();
	//	flood_extract(thresh_image, before, before);
	//	//PrintFunc("fe", fe,3);
	//}
	//thin_th_img = before.clone();
	////before¿¡¼­ ÀâÀ½À» Á¦°ÅÇÏ´Â ¹æ¹ýÀ» »ý°¢À» ÇØº¸ÀÚ
	////·¹ÀÌºí¸µÀ» ÀÌ¿ëÇØ¼­
	////good_threshold(before, U8char_img, before);
	//Mat fat_tmp, thin_tmp;
	//bitwise_and(fat_th_img, thin_th_img, fat_tmp);
	//medianBlur(fat_tmp, fat_tmp, 7);
	//thin_tmp = fat_tmp.clone();
	//flood_extract(fat_th_img, fat_tmp, fat_th_img);
	//flood_extract(thin_th_img, thin_tmp, thin_th_img);
	//
	//fat_tmp = fat_th_img + thin_th_img;
	//
	//PrintFunc("test_good_thresh", fat_th_img + thin_th_img, 2);
	////waitKey();
	//dst = fat_tmp.clone();
}
void defect_process(Mat src, Mat &dst, double adap_th1, double adap_th2)
{
	Mat U8char_img = src.clone();
	Mat test, thresh_image, defect, fat_th_img, thin_th_img;
	////////////////////////////////////////////////////////////////////
	//ºÒ·®À» Ã£¾Æº¸ÀÚ
	//adap_th2 = 20;
	adaptiveThreshold(U8char_img, test, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY_INV, adap_th1, adap_th2);//ADAPTIVE_THRESH_GAUSSIAN_C
	medianBlur(test, test, 5);
	//test = base.clone();
	Mat fe = test.clone();
	dilate(test, test, kernel, Point(-1, -1), 5);
	if (adap_th2 < 4)
		adap_th2 = 5;
	for (int i = adap_th2; i > 3; i -= 2) {
		Mat blur_img;
		blur(U8char_img, blur_img, Size(5, 5));
		adaptiveThreshold(blur_img, thresh_image, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY_INV, adap_th1, 1 + i);//ADAPTIVE_THRESH_GAUSSIAN_C
		medianBlur(thresh_image, thresh_image, 5);

		bitwise_and(thresh_image, test, test);
		//erode(fe, fe, kernel);
		flood_extract(thresh_image, fe, fe);
		//PrintFunc("fe", fe, 2);
		//PrintFunc("test", test, 2);
		//PrintFunc("thresh_image", thresh_image, 2);
		//waitKey();
		dilate(test, test, kernel, Point(-1, -1), 3);
	}

	erode(test, test, kernel, Point(-1, -1), 3);
	bitwise_and(thresh_image, test, thresh_image);
	defect = thresh_image.clone();
	//	debug_thresh = defect.clone();
	fat_th_img = defect.clone();
	PrintFunc("fat_th_img", fat_th_img, 2);
	//PrintFunc("flood_extract", fe, 2);

	Mat before, result;
	for (int i = 7; i > 2; i--)
	{
		int th1 = 8 * 2 + 3;
		int th2 = i;
		adaptiveThreshold(U8char_img, thresh_image, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY_INV, th1, th2);//ADAPTIVE_THRESH_GAUSSIAN_C
		medianBlur(thresh_image, thresh_image, 9);
		if (before.empty())
			before = thresh_image.clone();
		flood_extract(thresh_image, before, before);

	}
	thin_th_img = before.clone();
	//before¿¡¼­ ÀâÀ½À» Á¦°ÅÇÏ´Â ¹æ¹ýÀ» »ý°¢À» ÇØº¸ÀÚ
	//·¹ÀÌºí¸µÀ» ÀÌ¿ëÇØ¼­
	//good_threshold(before, U8char_img, before);
	Mat fat_tmp, thin_tmp;
	bitwise_and(fat_th_img, thin_th_img, fat_tmp);
	medianBlur(fat_tmp, fat_tmp, 7);
	thin_tmp = fat_tmp.clone();
	flood_extract(fat_th_img, fat_tmp, fat_th_img);
	flood_extract(thin_th_img, thin_tmp, thin_th_img);

	fat_tmp = fat_th_img + thin_th_img;

	PrintFunc("test_good_thresh", fat_th_img + thin_th_img, 2);
	//waitKey();
	dst = fat_tmp.clone();
}
void WHITE_BLACK_defect_process(Mat src, Mat &dst, double adap_th1, double adap_th2)
{
	//will make 3-th image
	Mat result, white_result, black_result;
	//white image
	adaptiveThreshold(src, white_result, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY_INV, adap_th1, adap_th2);//ADAPTIVE_THRESH_GAUSSIAN_C
	PrintFunc("white", white_result, 3);

	adaptiveThreshold(src, black_result, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, adap_th1, -adap_th2);//ADAPTIVE_THRESH_GAUSSIAN_C
	PrintFunc("black", black_result, 3);
	result = white_result + black_result;
	PrintFunc("3th", result, 3);
}
void make_big_data(string FolderPath, Mat ROI_UCHAR, Mat ROI_SRC, double defect_val, double canny_edge_ratio, double defect_std_val, double defect_ratio, int focus_idx, string DC, double bestfocus_avg, double bestfocus_std, int defect_color)
{
	char fname[300], ChkString[300], TextString[300];
	struct tm  tstruct;
	char       buf[80];
	FILE	*fp_in, *fp_data;
	fopen_s(&fp_in, "make_data.ini", "r");
	if (fp_in) {

		fscanf(fp_in, "%s", ChkString);
		if (!strcmp(ChkString, "[BEGIN]")) {
			while (1) {
				if (fscanf(fp_in, "%s", ChkString) == EOF) break;
				if (!strcmp(ChkString, "[END]")) break;
				if (!strcmp(ChkString, "##")) {
					while (1) if (getc(fp_in) == '\n') break;
				}
				string temp;
				if (!strcmp(ChkString, "DATAFOLDER")) { fscanf(fp_in, "%s\n", TextString); }
				if (!strcmp(ChkString, "CONTINUENUMBER")) { fscanf(fp_in, "%s\n", buf); }
			}
		}
		fclose(fp_in);
	}
	else
	{
		return;
	}

	int intValue = atoi(buf);

	sprintf(fname, "%s/%s.csv", TextString, "MakeDATA");
	cout << "CONTINUENUMBER : " << buf << endl;
	fopen_s(&fp_data, fname, "a");
	if (!fp_data) {
		return;
	}
	//fprintf(fp_data, "img_no,TID,defect_val,tmp_avg,defect_std_val,defect_ratio,class,test_sobel,bestFocuslayer\n");
	string TID;
	int index2 = FolderPath.substr(0, FolderPath.size() - 2).rfind("\\");  // °á°ú :9, ³¡¿¡¼­ ºÎÅÍ °Ë»öÇØ¼­ 2ÀÇ À§Ä¡¸¦ Ã£´Â´Ù.

	TID = FolderPath.substr(index2 + 1);
	//if (location == FolderPath.size())
	//	FolderPath.pop_back();
	//location = FolderPath.find_last_of("\\", 0);
	cout << TID << endl;

	Mat gray_IMG, auto_canny_img;
	cvtColor(ROI_SRC, gray_IMG, CV_BGR2GRAY);
	double best_avg, best_std;
	best_avg = avg_img(gray_IMG);
	best_std = img_std(gray_IMG);
	fprintf(fp_data, "%d,", intValue);
	fprintf(fp_data, "%s,", TID.c_str());
	fprintf(fp_data, "%.05f,", defect_val);
	fprintf(fp_data, "%.05f,", canny_edge_ratio);
	fprintf(fp_data, "%.05f,", defect_std_val);
	fprintf(fp_data, "%.05f,", defect_ratio);
	fprintf(fp_data, "%.05f,", bestfocus_avg);
	fprintf(fp_data, "%.05f,", bestfocus_std);
	fprintf(fp_data, "%.05f,", best_avg);
	fprintf(fp_data, "%.05f,", best_std);
	fprintf(fp_data, "%s,", DC.c_str());
	fprintf(fp_data, "%d,", focus_idx + 1);
	fprintf(fp_data, "ver003,,");

	//DNN 테스트용
	int class_num[10] = { 0 };
	if (DC.compare("BLACK") == 0)
	{
		class_num[0] = 1;
	}
	else if (DC.compare("BLACK + INVISIBLE") == 0)
	{
		class_num[1] = 1;
	}
	else if (DC.compare("INVISIBLE") == 0)
	{
		class_num[2] = 1;
	}
	else if (DC.compare("WHITE + INVISIBLE") == 0)
	{
		class_num[3] = 1;
	}
	else if (DC.compare("WHITE") == 0)
	{
		class_num[4] = 1;
	}
	else if (DC.compare("OLED OFF") == 0)
	{
		class_num[5] = 1;
	}
	else if (DC.compare("BUBBLE") == 0)
	{
		class_num[6] = 1;
	}
	else if (DC.compare("BLACK + WHITE") == 0)
	{
		class_num[7] = 1;
	}
	for (int i = 0; i<10; i++)
		fprintf(fp_data, "%d,", class_num[i]);

	fprintf(fp_data, "\n");

	fclose(fp_data);
	sprintf(fname, "%s/%s.bmp", TextString, buf);
	imwrite(fname, ROI_UCHAR); // ¿µ»ó ÀúÀå
	fopen_s(&fp_in, "make_data.ini", "w");
	fprintf(fp_in, "[BEGIN]\n");
	fprintf(fp_in, "DATAFOLDER	%s\n", TextString);
	intValue++;
	intToString(intValue);
	fprintf(fp_in, "CONTINUENUMBER	%s\n", intToString(intValue).c_str());
	fprintf(fp_in, "[END]\n");
	fclose(fp_in);

}
int good_canny_second(Mat src, Mat and_img, Mat &dst, int set_point, int min_th = 50, int max_th = 15000, int start = 50, int end = 90, int iter = 3, int back_th = 25, int new_ver = 0)
{
	//기존과 동일 다만 Dele_by_size 없애고 모든 사이즈에서 median에 가까운 사이즈만 남겨 놓는다.
	//contour 모든 정보 저장
	//

	destroyAllWindows();
	vector<Mat> LUT_canny;
	vector<double> diff_val;
	vector<double> diff2_val;
	////////////////////////////////////////////////////////////////////
	//¹è°æÁ¦°Å - ³ªÁß¿¡ È«Â÷Àå´Ô²² depth map ¹Þ¾Æ¼­ Ã³¸®ÇÏ´Â ¹æ¹ýµµ »ý°¢ÇØº¸ÀÚ
	//cvtColor(image[0], img_gray, CV_BGR2GRAY);
	delete_by_size(and_img, and_img, 0, 100000, and_img, false, -1);
	PrintFunc("and_img", and_img, 3);
	end = MIN(100, end);

	double dif_avg = 0;
	double pre_dif_avg = 0;
	double pre_avg = 0;
	double min_val = 1000;
	int min_idx = 3;
	for (int i = start; i < end; i += iter)
	{
		Mat tmp;
		//»óÀ§¿¡ Á»´õ ¸¹Àº Å½»öÀ» À§ÇØ¼­ °¨¸¶±×·¡ÇÁÀÇ °ªÀ» °¡Á®´Ù ¾´´Ù 
		double gamma_val = (double)i / 100;//pow((double)i / 100, 0.3);
		auto_canny(src, tmp, gamma_val);
		//tmp -= otsu;

		vector<vector<Point> > contours;
		vector<Vec4i> hierarchy;
		Mat tt = tmp.clone();

		//PrintFunc("center_zone",center_zone,2);
		//waitKey();
		bitwise_and(and_img, tmp, tt);

		double avg_size;// = pixel_cnt(tt);
		double dif_dif_avg;
		avg_size = pixel_cnt(tmp - tt);
		dif_avg = pre_avg - avg_size;
		dif_dif_avg = dif_avg - pre_dif_avg;
		cout << i << "'s" << endl;;
		cout << "avg_size:" << avg_size << endl;
		cout << "dif_avg : " << dif_avg << endl;
		cout << "dif_dif_avg : " << dif_dif_avg << endl << endl;
		//fprintf(fp_log, "%.05f, ", avg_size);
		//fprintf(fp_log, "%.05f, ", dif_avg);
		//fprintf(fp_log, "%.05f, ", pre_dif_avg - dif_avg);
		//fprintf(fp_log, "\n");

		//PrintFunc("src", src, 3);
		//PrintFunc("tmp-tt", tmp - tt, 3);
		//PrintFunc("tmp", tmp, 3);
		//PrintFunc("tt", tt, 3);
		//waitKey();
		if (avg_size > 10000) {
			cout << "pass~~~" << endl;
			continue;
		}
		diff_val.push_back(dif_avg);
		diff2_val.push_back(dif_dif_avg);
		pre_avg = avg_size;
		pre_dif_avg = dif_avg;
		LUT_canny.push_back(tt);

		if (dif_dif_avg == 0)
			continue;
		if ((min_val > dif_dif_avg) && (dif_dif_avg > -1000))
		{
			min_val = dif_dif_avg;
			min_idx = i;
			dst = tt.clone();
		}

	}

	cout << min_idx*iter + start << endl;
	cout << min_val << endl;
	min_idx -= 1;
	//PrintFunc("gg", LUT_canny[min_idx], 3);
	//fclose(fp_log);

	//waitKey();
}

void find_defect(vector<Mat> &src, vector<Mat> &dst, string FolderPath, int *best_focus_idx, string *defect_class, Mat idx_mat)
{
#ifdef PRINT_DEBUG

	FILE	*fp_ini;
	char ChkString[500];
	cout << "19881-0182039812309183" << endl;
	fopen_s(&fp_ini, "Simul.ini", "r");
	if (fp_ini) {

		//fscanf(fp_ini, "%s", ChkString);
		//if (!strcmp(ChkString, "[ModelingBEGIN]")) 
		{
			while (1) {
				if (fscanf(fp_ini, "%s", ChkString) == EOF) break;
				//if (!strcmp(ChkString, "[ModelingEND]")) break;
				if (!strcmp(ChkString, "##")) {
					while (1) if (getc(fp_ini) == '\n') break;
				}
				string temp;
				if (!strcmp(ChkString, "dft_radius")) { fscanf(fp_ini, " = %lf\n", &dft_radius); }
				if (!strcmp(ChkString, "dft_thresh")) { fscanf(fp_ini, " = %lf\n", &dft_thresh); }
				if (!strcmp(ChkString, "dft_thresh_min")) { fscanf(fp_ini, " = %lf\n", &dft_thresh_min); }
				if (!strcmp(ChkString, "dft_thresh_max")) { fscanf(fp_ini, " = %lf\n", &dft_thresh_max); }
				if (!strcmp(ChkString, "adap_th1")) { fscanf(fp_ini, " = %lf\n", &adap_th1); }
				if (!strcmp(ChkString, "adap_th2")) { fscanf(fp_ini, " = %lf\n", &adap_th2); }
				if (!strcmp(ChkString, "canny_th1")) { fscanf(fp_ini, " = %lf\n", &canny_th1); }
				if (!strcmp(ChkString, "canny_th2")) { fscanf(fp_ini, " = %lf\n", &canny_th2); }
				if (!strcmp(ChkString, "back_th")) { fscanf(fp_ini, " = %lf\n", &back_th); }

				if (!strcmp(ChkString, "black_adap_th1")) { fscanf(fp_ini, " = %lf\n", &black_adap_th1); }
				if (!strcmp(ChkString, "black_adap_th2")) { fscanf(fp_ini, " = %lf\n", &black_adap_th2); }
				if (!strcmp(ChkString, "black_canny_th1")) { fscanf(fp_ini, " = %lf\n", &black_canny_th1); }
				if (!strcmp(ChkString, "black_canny_th2")) { fscanf(fp_ini, " = %lf\n", &black_canny_th2); }

				if (!strcmp(ChkString, "black_invisible_adap_th1")) { fscanf(fp_ini, " = %lf\n", &black_invisible_adap_th1); }
				if (!strcmp(ChkString, "black_invisible_adap_th2")) { fscanf(fp_ini, " = %lf\n", &black_invisible_adap_th2); }
				if (!strcmp(ChkString, "black_invisible_canny_th1")) { fscanf(fp_ini, " = %lf\n", &black_invisible_canny_th1); }
				if (!strcmp(ChkString, "black_invisible_canny_th2")) { fscanf(fp_ini, " = %lf\n", &black_invisible_canny_th2); }

				if (!strcmp(ChkString, "invisible_adap_th1")) { fscanf(fp_ini, " = %lf\n", &invisible_adap_th1); }
				if (!strcmp(ChkString, "invisible_adap_th2")) { fscanf(fp_ini, " = %lf\n", &invisible_adap_th2); }
				if (!strcmp(ChkString, "invisible_canny_th1")) { fscanf(fp_ini, " = %lf\n", &invisible_canny_th1); }
				if (!strcmp(ChkString, "invisible_canny_th2")) { fscanf(fp_ini, " = %lf\n", &invisible_canny_th2); }

				if (!strcmp(ChkString, "white_invisible_adap_th1")) { fscanf(fp_ini, " = %lf\n", &white_invisible_adap_th1); }
				if (!strcmp(ChkString, "white_invisible_adap_th2")) { fscanf(fp_ini, " = %lf\n", &white_invisible_adap_th2); }
				if (!strcmp(ChkString, "white_invisible_canny_th1")) { fscanf(fp_ini, " = %lf\n", &white_invisible_canny_th1); }
				if (!strcmp(ChkString, "white_invisible_canny_th2")) { fscanf(fp_ini, " = %lf\n", &white_invisible_canny_th2); }

				if (!strcmp(ChkString, "white_adap_th1")) { fscanf(fp_ini, " = %lf\n", &white_adap_th1); }
				if (!strcmp(ChkString, "white_adap_th2")) { fscanf(fp_ini, " = %lf\n", &white_adap_th2); }
				if (!strcmp(ChkString, "white_canny_th1")) { fscanf(fp_ini, " = %lf\n", &white_canny_th1); }
				if (!strcmp(ChkString, "white_canny_th2")) { fscanf(fp_ini, " = %lf\n", &white_canny_th2); }

				if (!strcmp(ChkString, "ledoff_adap_th1")) { fscanf(fp_ini, " = %lf\n", &ledoff_adap_th1); }
				if (!strcmp(ChkString, "ledoff_adap_th2")) { fscanf(fp_ini, " = %lf\n", &ledoff_adap_th2); }
				if (!strcmp(ChkString, "ledoff_canny_th1")) { fscanf(fp_ini, " = %lf\n", &ledoff_canny_th1); }
				if (!strcmp(ChkString, "ledoff_canny_th2")) { fscanf(fp_ini, " = %lf\n", &ledoff_canny_th2); }

			}
		}
		fclose(fp_ini);
	}

#endif


	vector <string> defect_name(4);


	defect_name[OLED_DEFECT] = "OLED_DEFECT";
	defect_name[NORMAL_DEFECT] = "NORMAL_DEFECT";
	defect_name[SCRATCH_DEFECT] = "SCRATCH_DEFECT";
	defect_name[LINEAR_DEFECT] = "LINEAR_DEFECT";
	vector<Mat> input;
	vector<Mat> white_img;
	Mat img_gray, inverseTransform, U8char_img, thresh_image, defect, thresh, canny_img;
	Mat bilateral, otsu, best_focus_src;
	Mat ttt, canny_image;
	Mat test;
	Mat back_del;
	Mat res, tmp;


	int defect_mode = NORMAL_DEFECT;// ±âº»ÀûÀ¸·Î ³ë¸»·Î º¸ÀÚ
	int defect_color = BLACK;// ±âº»ÀûÀ¸·Î ³ë¸»·Î º¸ÀÚ
	int good_canny_start = 50;
	int good_canny_end = 90;
	Mat cmp_img, gray_img;
	cvtColor(src[0], gray_img, CV_BGR2GRAY);
	Mat before_img = gray_img.clone();
	if (src.size() > 15) {
		for (int i = 1; i < src.size(); )
		{

			cvtColor(src[i], gray_img, CV_BGR2GRAY);
			cmp_img = before_img - gray_img.clone();
			//cout << avg_img(cmp_img) << endl;
			if (avg_img(cmp_img) <= 0.3)
				src.erase(src.begin() + i);
			else
				i++;

			before_img = gray_img.clone();
		}
	}
	clock_t loadClock = clock();
	double defect_val = 0;
	double defect_ratio = 0;
	double canny_edge_ratio = 0;
	double defect_std_val = 0;
	double bestfocus_std = 0;
	double bestfocus_avg = 0;
	bool back_light_onoff = OFF;
	bool black_white_mode = OFF;
	bool isBubble = OFF;
	vector<Mat> BGR(3);
	vector <Mat> green_src;

	Mat and_img;
	int focus_idx;
	int invi_three = 0;
	Rect defect_ROI = find_best_focus(src, best_focus_src, and_img, dft_thresh_min, dft_thresh_max, 0.00, FolderPath, &focus_idx, &defect_val, &defect_std_val, &defect_ratio, &canny_edge_ratio, &invi_three);
	invi_three = 0;
	double result = (double)(clock() - loadClock) / CLOCKS_PER_SEC;
	cout << "find_best_focus Elapsed Time is " << result << " Seconds" << endl;


	loadClock = clock();



	//ÇÏ¾çÀÌ ÀÖ´ÂÁö ¾ø´ÂÁö ÆÇ´ÜÇØ¾ß ÇÔ
	//MIN RGB ÇØ¼­ ÇÏ¾ç°Í¸¸ µµµå¶óÁö°Ô ÇØº¸ÀÚ
	//best_focus_src = src[0].clone();

	Mat Min_MAP = best_focus_src.clone();
	Mat Min_img, Max_img, gray_IMG, auto_canny_img;
	bestfocus_avg = avg_img(best_focus_src);
	bestfocus_std = img_std(best_focus_src);
	//auto_canny(gray_IMG, auto_canny_img, 0.5);
	cout << "avg : best_focus_src : " << bestfocus_avg << endl;
	cout << "std : best_focus_src : " << bestfocus_std << endl;

	cvtColor(best_focus_src(defect_ROI), gray_IMG, CV_BGR2GRAY);
	double avg1, std1, avg3, std3;
	avg1 = avg_img(gray_IMG);
	std1 = img_std(gray_IMG);
	avg3 = bestfocus_avg;
	std3 = bestfocus_std;


	PrintFunc("best_focus_src", best_focus_src, 3);
	blur(Min_MAP, Min_MAP, Size(5, 5));

	split(Min_MAP, BGR);
	//MaxMin_Func(BGR, Max_img, Min_img);

	//PrintFunc("Max_img", Max_img, 3);
	//PrintFunc("Min_img", Min_img, 3);
	//bool isBlack = true; // ºí·¢ÀÌ¸é T ¾Æ´Ï¸é F


	//¿ä°Ç º£½ºÆ® Æ÷Ä¿½º ¾Ë°í¸®ÁòÀ¸·Î ÇØ°áÇØº¸ÀÚ

	cv::cvtColor(Min_MAP, img_gray, CV_BGR2GRAY);
	Mat resize_img;
	loadClock = clock();
	Mat test_dct;

	//아 전무님 패턴제거가 아니라 노이즈 제거같은데..... 
	//dft_background_remove_second(img_gray, test_dct, dft_radius, dft_thresh);
	//waitKey();
	Mat sum = Mat::zeros(img_gray.size(), CV_8UC1);
	Mat egde_sum = Mat::zeros(img_gray.size(), CV_8UC1);
	Mat gray_egde_sum = Mat::zeros(img_gray.size(), CV_8UC1);
	Mat otsu_sum = Mat::zeros(img_gray.size(), CV_8UC1);
	//otsu_sum += 255;
	vector <Mat> canny_list;
	vector <Mat> DFT_list;

	//TrackBar(best_focus_src, U8char_img);
	if (focus_idx < 3)
		dft_thresh -= 0.05;
	//cout << "defect_mode : " << defect_mode << endl;

	//RGB ³ª´²¼­ ÇÏÀÚÀÚÀÚÀÚ
	//RGBº°·Î DFT ÇØ¼­ ³ªÁß¿¡ ÇÕÄ¡´Â ¹æ½Ä ¤¡¤¡ 
	cout << dft_radius << endl;
	cout << dft_thresh << endl;
	waitKey();
	dft_background_remove(BGR[0], inverseTransform, dft_radius, dft_thresh);
	inverseTransform.convertTo(BGR[0], CV_8UC1, 255.0);
	dft_background_remove(BGR[1], inverseTransform, dft_radius, dft_thresh);
	inverseTransform.convertTo(BGR[1], CV_8UC1, 255.0);
	dft_background_remove(BGR[2], inverseTransform, dft_radius, dft_thresh);
	inverseTransform.convertTo(BGR[2], CV_8UC1, 255.0);

	BGR[0] = abs(BGR[0] - 255) / 3;
	BGR[1] = abs(BGR[1] - 255) / 3;
	BGR[2] = abs(BGR[2] - 255) / 3;

	U8char_img = BGR[0] + BGR[1] + BGR[2];

	U8char_img = abs(U8char_img - 255);

	PrintFunc("U8char_imgU8char_img", U8char_img, 3);


	result = (double)(clock() - loadClock) / CLOCKS_PER_SEC;
	cout << "dft_background_remove Elapsed Time is " << result << " Seconds" << endl;
	loadClock = clock();
	if (back_light_onoff)
		back_th = 0;
	//back_del;
	//Mat otsu;
	threshold(img_gray, otsu, back_th, 255, THRESH_BINARY_INV);//ADAPTIVE_THRESH_GAUSSIAN_C
	erode(otsu, otsu, kernel, Point(-1, -1), 15);
	medianBlur(otsu, otsu, 15);
	dilate(otsu, otsu, kernel, Point(-1, -1), 55);
	//rectangle(otsu, Point(0, 0), Point(otsu.cols, otsu.rows), 255, MIN((int)adap_th1*1.2, 254));
	rectangle(otsu, Point(0, 0), Point(otsu.cols, otsu.rows), 255, 250);
	tmp = otsu.clone();
	floodFill(tmp, Point(1, 1), 0);
	bitwise_xor(otsu, tmp, back_del);
	rectangle(back_del, Point(0, 0), Point(otsu.cols, otsu.rows), 0, 250);

	//¸¸¾à back_delÀÌ rectangle ÇÑ°Å¶û ´Ù¸¦°Ô ¾ø´Ù¸é otsuÇØ¼­ ´Ù½Ã ÇØº¸±á±á±á±á±á

	if (!idx_mat.empty())
	{
		Mat test_mat;
		//Canny(idx_mat, test_mat, 125, 125*3);
		idx_mat.convertTo(test_mat, CV_8UC1);
		//equalizeHist(test_mat, test_mat);
		auto_canny(test_mat, test_mat, 0.97);
		//PrintFunc("test_mat", test_mat, 2);
		//dilate(test_mat, test_mat, kernel, Point(-1, -1), 3);
		back_del += test_mat.clone();

		//blur(test_mat, test_mat, Size(25, 25));
		//Mat test_blur = U8char_img * test_mat;


		string debug_img_path = FolderPath + "\\_indexmap.bmp";
		imwrite(debug_img_path, idx_mat.clone()); // ¿µ»ó ÀúÀå
												  //waitKey();

	}
	else
	{
		//AIF, console 전용
		string debug_img_path = FolderPath + "\\_indexmap.bmp";
		idx_mat = imread(debug_img_path, CV_LOAD_IMAGE_ANYDEPTH);

		if (!idx_mat.empty())
		{
			Mat test_mat;
			//Canny(idx_mat, test_mat, 125, 125*3);
			idx_mat.convertTo(test_mat, CV_8UC1);
			//equalizeHist(test_mat, test_mat);
			auto_canny(test_mat, test_mat, 0.97);
			//PrintFunc("test_mat", test_mat, 2);
			//dilate(test_mat, test_mat, kernel,Point(-1,-1),30);
			//blur(test_mat, test_mat, Size(25, 25));
			//Mat test_blur;
			//blur(U8char_img, test_blur, Size(5, 5));
			//bitwise_and(U8char_img, test_mat, test_blur);
			//PrintFunc("test_blur", test_blur, 2);

			//U8char_img -= test_mat;
			//U8char_img += test_blur;
			//PrintFunc("test_U8char_img", U8char_img,2);
			back_del += test_mat.clone();
			//waitKey();
		}
	}


	PrintFunc("back_del", back_del, 2);
	//ÀÌ¹°¿¡ ´ëÇØ¼­¸¸ std¸¦ ±¸ÇØº¸´Â°ÅÁÕ ÈæÁ¡Àº Áß°£Áß°£ »öÀÌ ¹Ù²î´Âµ¥ À×Å©Æ¦Àº µü ¶³¾îÁö°Ô ´Þ¶óÁö´Ï²²
	//Mat std_test = U8char_img(defect_ROI).clone();
	//Mat sobel, sobel_ori;
	//Mat sobelX;
	//Mat sobelY;
	//blur(std_test, std_test, Size(5, 5));
	//
	//Laplacian(std_test, sobelX, CV_16S, 3, 1, 0);
	//convertScaleAbs(sobelX, sobel);
	////PrintFunc("Laplacian", sobel, 1);
	//double lap_avg = avg_img(sobel);
	//double lap_std = img_std(sobel);
	//cout << "Laplacian avg : " << avg_img(sobel) << endl;
	//cout << "Laplacian std : " << img_std(sobel) << endl;

	//°ËÁ¤, ¹é»ö, Åõ¸íÀ» ±¸ºÐÇÏ±â À§ÇÑ ºÎºÐ

	//cout << avg_img(U8char_img) << endl;
	//cout << avg_img(U8char_img) << endl;
	//cout << avg_img(U8char_img) << endl;
	//cout << avg_img(U8char_img) << endl;
	//gamma_func(U8char_img, tmp, 5.0);
	//auto_canny(tmp, tmp, 1);
	//dilate(tmp, tmp, kernel, Point(-1, -1), 1);
	//threshold(tmp, tmp, 0, 255, THRESH_BINARY_INV | THRESH_OTSU);
	//tmp -= back_del;


	//Mat cir_tmp = Mat::zeros(tmp.size(), CV_8UC1);
	//circle(cir_tmp, Point(tmp.cols / 2, tmp.rows / 2), tmp.size().width / 2, 255, -1);
	//bitwise_and(cir_tmp, tmp, tmp);

	//PrintFunc("tmp", tmp, 3);

	//º¯¼öÈ­ ½ÃÄÑ¾ß ÇÒµí
	cout << "defect_val : " << defect_val << endl;
	cout << "defect_std_val : " << defect_std_val << endl;
	cout << "canny_edge_ratio : " << canny_edge_ratio << endl;

	defect_color = INVISIBLE;

	/*
	//E:\img\samsung_note_8\180826\180815\MakeDATA.xlsx
	//dv ~ 47;
	black_white_mode = OFF;
	if (defect_val < 47) {
	defect_color = BLACK;
	if (defect_ratio < 110)
	{
	defect_color = BLACK;
	}
	else
	{
	if (defect_std_val < 10)
	{
	if (canny_edge_ratio < 0.16)
	{
	defect_color = BLACK + INVISIBLE;

	}
	else
	{
	defect_color = BLACK;
	}
	}
	else
	{
	defect_color = BLACK;
	}
	}
	}
	else if (defect_val < 70)
	{
	defect_color = BLACK + INVISIBLE;
	if (defect_std_val < 9)
	{
	if (bestfocus_std < 10)
	{
	defect_color = WHITE + INVISIBLE;
	}
	else
	{
	defect_color = BLACK + INVISIBLE;
	}
	}
	else
	{
	if (defect_val < 65)
	{
	if (canny_edge_ratio < 0.10)
	{
	if (defect_ratio < 100)
	{
	defect_color = WHITE + INVISIBLE;

	}
	else
	{
	defect_color = WHITE + INVISIBLE;
	black_white_mode = ON;
	}
	}
	else
	{
	if (defect_std_val < 15)
	{
	if (defect_val < 52)
	{
	defect_color = WHITE + INVISIBLE;
	}
	else
	{
	if (bestfocus_std < 15.5)
	{
	defect_color = WHITE + INVISIBLE;
	black_white_mode = ON;
	}
	else
	{
	defect_color = INVISIBLE;
	}
	}
	}
	else
	{
	if (bestfocus_std < 25)
	{
	if (canny_edge_ratio < 0.20)
	{
	if (bestfocus_std < 9)
	{
	defect_color = WHITE + INVISIBLE;
	black_white_mode = ON;
	}
	else
	{
	defect_color = WHITE + INVISIBLE;
	}
	}
	else
	{
	if (bestfocus_std < 13)
	{
	defect_color = WHITE + INVISIBLE;
	}
	else
	{
	if (defect_val < 62.8)
	{
	defect_color = WHITE + INVISIBLE;
	black_white_mode = ON;
	}
	else
	{
	defect_color = WHITE + INVISIBLE;
	}
	}
	}
	}
	else
	{
	if (defect_std_val < 30)
	{
	defect_color = INVISIBLE;
	}
	else
	{
	defect_color = WHITE + INVISIBLE;
	}
	}
	}
	}
	}
	else
	{
	if (defect_std_val < 25.5)
	{
	defect_color = WHITE + INVISIBLE;
	}
	else if (defect_std_val < 38.5)
	{
	defect_color = WHITE + INVISIBLE;
	black_white_mode = ON;
	}
	else
	{
	defect_color = WHITE + INVISIBLE;
	}
	}
	}
	}
	else if (defect_val < 106)
	{
	if (canny_edge_ratio < 0.225)
	{
	if (bestfocus_std < 22.5)
	{
	if (defect_std_val < 40)
	{
	defect_color = WHITE + INVISIBLE;
	}
	else
	{
	defect_color = WHITE + INVISIBLE;
	black_white_mode = ON;
	}
	}
	else
	{
	defect_color = INVISIBLE;
	}
	}
	else
	{
	if (bestfocus_std < 10)
	{
	defect_color = WHITE + INVISIBLE;
	}
	else
	{
	if (defect_std_val < 35)
	{
	if (defect_ratio < 105)
	{
	defect_color = WHITE + INVISIBLE;
	}
	else if (defect_ratio < 115)
	{
	if (bestfocus_std < 21)
	{
	defect_color = WHITE + INVISIBLE;
	}
	else
	defect_color = INVISIBLE;

	}
	else
	{
	defect_color = WHITE + INVISIBLE;
	}
	}
	else
	{
	defect_color = BLACK + INVISIBLE;
	}
	}
	}
	}
	else if (defect_val < 120)
	{
	if (defect_std_val < 43)
	{
	defect_color = WHITE + INVISIBLE;
	}
	else if (defect_std_val < 62.4)
	{
	if (bestfocus_std < 10.4)
	{
	defect_color = WHITE + INVISIBLE;
	}
	else
	{
	defect_color = WHITE + INVISIBLE;
	black_white_mode = ON;
	}
	}
	else
	{
	defect_color = WHITE + INVISIBLE;
	}
	}
	else if (defect_val < 201)
	{
	if (defect_std_val < 16.5)
	{
	defect_color = BLACK;
	}
	else
	{
	if (defect_ratio < 7)
	{
	defect_color = WHITE + INVISIBLE;
	black_white_mode = ON;
	}
	else
	{
	if (defect_std_val < 26)
	{
	if (defect_ratio < 30)
	{
	defect_color = WHITE + INVISIBLE;
	}
	else
	{
	defect_color = WHITE + INVISIBLE;
	black_white_mode = ON;
	}
	}
	else
	{
	defect_color = WHITE + INVISIBLE;
	}
	}
	}
	}
	else if (defect_val < 203)
	{
	defect_color = WHITE + INVISIBLE;
	black_white_mode = ON;
	}
	else
	{
	if (defect_val < 214)
	{
	defect_color = WHITE + INVISIBLE;
	}
	else
	{
	if (defect_std_val < 6.5)
	{
	defect_color = WHITE;
	}
	else
	{
	if (defect_ratio < 15)
	{
	if (defect_val < 225)
	{
	defect_color = WHITE + INVISIBLE;
	black_white_mode = ON;
	}
	else if (defect_val < 243)
	{
	//BUBBLE
	isBubble = ON;
	defect_color = WHITE + INVISIBLE;
	black_white_mode = ON;
	}
	else
	{
	defect_color = WHITE + INVISIBLE;
	black_white_mode = ON;
	}
	}
	else if (defect_ratio < 53.5)
	{
	if (bestfocus_std < 26)
	{
	if (defect_std_val < 14.5)
	{
	defect_color = WHITE + INVISIBLE;
	black_white_mode = ON;
	}
	else
	{//BUBBLE
	isBubble = ON;
	defect_color = WHITE + INVISIBLE;
	black_white_mode = ON;
	}
	}
	else if (bestfocus_std < 32.4)
	{
	defect_color = WHITE;
	}
	else
	{
	if (canny_edge_ratio < 0.30)
	{
	defect_color = WHITE + INVISIBLE;
	black_white_mode = OFF;
	}
	else
	{
	if (canny_edge_ratio < 18.5)
	{
	defect_color = WHITE + INVISIBLE;
	black_white_mode = OFF;
	}
	else
	{
	//BUBBLE
	isBubble = ON;
	defect_color = WHITE + INVISIBLE;
	black_white_mode = ON;
	}
	}
	}
	}
	else
	{
	if (defect_std_val < 14.5)
	{
	defect_color = WHITE + INVISIBLE;
	black_white_mode = ON;
	}
	else
	{
	defect_color = WHITE + INVISIBLE;
	}
	}

	}
	}
	}

	*/

	//E:\hychoi\project\edge_display\결과파일정리_180907
	//OLED부터하자
	if (avg1 < 40)
	{
		if (avg3 < 26)
		{
			back_light_onoff = ON;
		}
		else
		{
			if (defect_val < 200)
				defect_color = BLACK;
			else
				back_light_onoff = ON;
		}
	}
	else {
		if (defect_val < 40)
		{
			defect_color = BLACK;
		}
		else if (defect_val < 46)
		{
			if (defect_std_val < 12)
			{
				defect_color = BLACK + INVISIBLE;
			}
			else
			{
				defect_color = BLACK;
			}
		}
		else if (defect_val < 70)
		{
			if (canny_edge_ratio < 0.2)
			{
				if (std1 < 40)
				{
					defect_color = WHITE + INVISIBLE;
				}
				else
				{
					defect_color = BLACK;
				}
			}
			else
			{
				if (std1 < 30)
				{
					if (avg3 < 29)
					{
						//BUBBLE
						isBubble = ON;
						defect_color = WHITE + INVISIBLE;
						black_white_mode = ON;
					}
					else if (avg3 < 36)
					{
						//BW
						defect_color = WHITE + INVISIBLE;
						black_white_mode = ON;
					}
					else if (avg3 < 40)
					{
						if (std1 < 10.5)
						{
							if (defect_val < 58)
								defect_color = BLACK + INVISIBLE;
							else
								defect_color = WHITE + INVISIBLE;
						}
						else
						{
							if (defect_std_val < 15)
								defect_color = BLACK + INVISIBLE;
							else
							{//BW
								defect_color = WHITE + INVISIBLE;
								black_white_mode = ON;
							}
						}
					}
					else // 40 ~
					{
						if (std1 < 12)
						{
							defect_color = INVISIBLE;
						}
						else
						{
							if (avg1 < 59)
							{
								if (defect_std_val < 17) {
									defect_color = BLACK;
								}
								else
								{//BW
									defect_color = WHITE + INVISIBLE;
									black_white_mode = ON;
								}
							}
							else if (avg1 < 62)
							{
								defect_color = WHITE + INVISIBLE;
							}
							else
							{
								defect_color = BLACK;
							}
						}
					}
				}
				else // std1 30 ~
				{
					defect_color = BLACK;
				}
			}
		}
		else if (defect_val < 101)
		{
			if (std1 < 38)
			{
				if (canny_edge_ratio < 0.12)
				{
					defect_color = WHITE + INVISIBLE;
				}
				else
				{
					if (defect_std_val < 37)
					{
						defect_color = WHITE + INVISIBLE;
					}
					else
					{//BW
						defect_color = WHITE + INVISIBLE;
						black_white_mode = ON;
					}
				}
			}
			else
			{

				defect_color = WHITE + INVISIBLE;
			}
		}
		else if (defect_val < 200)
		{
			defect_color = WHITE + INVISIBLE;
		}
		else if (defect_val < 230)
		{
			if (avg1 < 61)
			{//BUBBLE
				isBubble = ON;
				defect_color = WHITE + INVISIBLE;
				black_white_mode = ON;
			}
			else
			{
				if (std1 < 25)
				{
					//BW
					defect_color = WHITE + INVISIBLE;
					black_white_mode = ON;
				}
				else if (std1 < 31)
				{
					defect_color = WHITE + INVISIBLE;
				}
				else if (std1 < 35)
				{//BW
					defect_color = WHITE + INVISIBLE;
					black_white_mode = ON;
				}
				else
				{
					if (defect_std_val < 22.5)
					{//BUBBLE
						isBubble = ON;
						defect_color = WHITE + INVISIBLE;
						black_white_mode = ON;
					}
					else
					{
						defect_color = WHITE + INVISIBLE;
					}
				}
			}
		}
		else
		{
			if (defect_std_val < 14)
			{//BW
				defect_color = WHITE + INVISIBLE;
				black_white_mode = ON;
			}
			else
			{
				if (avg1 < 60)
				{//BUBBLE
					isBubble = ON;
					defect_color = WHITE + INVISIBLE;
					black_white_mode = ON;
				}
				else
				{
					if (avg3 < 20)
					{//BW
						defect_color = WHITE + INVISIBLE;
						black_white_mode = ON;
					}
					else
					{
						if (defect_std_val < 20)
						{
							if (std3 < 22)
							{//BW
								defect_color = WHITE + INVISIBLE;
								black_white_mode = ON;
							}
							else
							{
								defect_color = WHITE;
							}
						}
						else
						{//BUBBLE
							isBubble = ON;
							defect_color = WHITE + INVISIBLE;
							black_white_mode = ON;
						}
					}
				}
			}
		}
	}
	if (back_light_onoff)
		defect_color = BLACK + WHITE + INVISIBLE;
	//여깁니다 강제로 바꾸는곳이
	//defect_color = BLACK;

	//TrackBar(U8char_img.clone(), otsu_sum);
	result = (double)(clock() - loadClock) / CLOCKS_PER_SEC;
	cout << "defect type Elapsed Time is " << result << " Seconds" << endl;
	loadClock = clock();

	Mat debug_canny;//= canny_img.clone();
	Mat debug_thresh;// = canny_img.clone();
	Mat debug_before;// = canny_img.clone();
	medianBlur(U8char_img, tmp, 3);
	//blur(tmp, tmp, Size(5, 5));

	//°ËÁ¤
	if (defect_color == BLACK) {
		cout << "BLACK" << endl;
		//blur(tmp, tmp, Size(5, 5));
		adap_th1 = black_adap_th1;
		adap_th2 = black_adap_th2;
		canny_th1 = black_canny_th1;
		canny_th2 = 5;
		good_canny_start = black_canny_th2;
		good_canny_end = 95;
		if (focus_idx < 4) {
			cout << "OLED BLACK" << endl;
			adap_th1 = 415;

			if (focus_idx < 2) {
				good_canny_start *= 2;
				defect_mode = OLED_DEFECT;
			}
			//defect_mode = OLED_DEFECT;
			//canny_th1 *= 2;

		}
		else
		{
			canny_th1 *= 1.5;
			adap_th2 /= 5;

		}
	}

	if ((defect_color == (BLACK + INVISIBLE))) {
		cout << "BLACK INVISIBLE" << endl;
		//canny_th1 *= 1.2;
		good_canny_start = 75;
		good_canny_end = 93;
		adap_th1 = black_invisible_adap_th1;
		adap_th2 = black_invisible_adap_th2;
		canny_th1 = black_invisible_canny_th1;
		canny_th2 = black_invisible_canny_th2;

		if (focus_idx < 5) {
			cout << "OLED INVISIBLE" << endl;
			canny_th1 *= 2;
			canny_th2 /= 2.5;
			//defect_mode = OLED_DEFECT;
			blur(tmp, tmp, Size(3, 3));
		}
		else if (focus_idx < 9) {
			//O:\SDOPTICS\WDMS\Images\Data\10\S1-0001\H-S1-0001-002 : 9 -> 7
			blur(tmp, tmp, Size(3, 3));
			//medianBlur(tmp, tmp, 11);
			canny_th2 *= 1.5;
			adap_th2 /= 2.5;
		}
		else
		{
			canny_th2 *= 2;
			adap_th2 /= 2.5;
			blur(tmp, tmp, Size(3, 3));
		}
	}

	if ((defect_color == (INVISIBLE))) {
		//canny_th1 *= 1.2;
		//canny_th2 = 50;
		cout << "INVISIBLE" << endl;
		adap_th1 = invisible_adap_th1;
		adap_th2 = invisible_adap_th2;
		canny_th1 = invisible_canny_th1;
		canny_th2 = 15;
		good_canny_start = invisible_canny_th2, good_canny_end = 87;
		if (focus_idx < 5) {
			cout << "OLED INVISIBLE" << endl;
			//canny_th2 = 50;
			//defect_mode = OLED_DEFECT;
			blur(tmp, tmp, Size(5, 5));
		}
		else
		{
			//adap_th1 /= 2;
			//O:\SDOPTICS\WDMS\Images\Measure\J-S2-0025-0001
			canny_th1 *= 1.2;
			//O:\SDOPTICS\WDMS\Images\Measure\J-S2-3289-0001
			//canny_th2 = 0;
			blur(tmp, tmp, Size(5, 5));
		}
	}

	if (defect_color == (WHITE + INVISIBLE)) {
		cout << "WHITE INVISIBLE" << endl;

		//adap_th2 /= 2;
		//adap_th2 = MAX(adap_th2, 10);
		adap_th1 = white_invisible_adap_th1;
		adap_th2 = white_invisible_adap_th2;
		canny_th1 = white_invisible_canny_th1;
		canny_th2 = white_invisible_canny_th2;
		good_canny_start = 60, good_canny_end = 85;
		if (focus_idx < 5) {
			good_canny_start = 40, good_canny_end = 60;
			cout << "OLED WHITE" << endl;
			//adap_th1 = 415;
			canny_th1 *= 1.5;
			canny_th2 *= 1;
			blur(tmp, tmp, Size(3, 3));
		}

		else if (focus_idx < 15) {

			good_canny_start = 40, good_canny_end = 55;
			//ÇÏ¾çÀÏ¶© µðÅ×ÀÏÀÌµð
			tmp = U8char_img.clone();
			//blur(tmp, tmp, Size(7,7));
			cout << "normal WHITE invisible" << endl;
			//adap_th1 = 55;
			canny_th1 *= 1.3;
			//canny_th2 = 0;
			blur(tmp, tmp, Size(5, 5));
			//defect_mode = SCRATCH_DEFECT;
		}
		else
		{
			good_canny_start = 40, good_canny_end = 65;
			if (focus_idx > 18) {
				good_canny_start = 10, good_canny_end = 20;
			}
			tmp = U8char_img.clone();
			//blur(tmp, tmp, Size(7,7));
			cout << "GLASS WHITE" << endl;
			adap_th1 /= 2;
			canny_th1 *= 1.7;
			canny_th2 = 0;
			//blur(tmp, tmp, Size(3, 3));
			defect_mode = SCRATCH_DEFECT;
		}

		//cout << avg_img(tmp) << endl;
		//cout << avg_img(tmp) << endl;
		//cout << avg_img(tmp) << endl;
		//cout << avg_img(tmp) << endl;
	}

	if ((defect_color == WHITE)) {
		cout << "WHITE" << endl;
		adap_th2 /= 2;
		adap_th1 = white_adap_th1;
		adap_th2 = white_adap_th2;
		canny_th1 = white_canny_th1;
		canny_th2 = white_canny_th2;
		good_canny_start = 40, good_canny_end = 65;
		if (focus_idx < 3) {
			cout << "OLED WHITE" << endl;
			//	adap_th1 = 415;
			defect_mode = OLED_DEFECT;
			//adap_th2 = MAX(adap_th2, 10);
		}
		else {
			//ÇÏ¾çÀÏ¶© µðÅ×ÀÏÀÌµð
			tmp = U8char_img.clone();
			cout << "GLASS WHITE" << endl;
			defect_mode = SCRATCH_DEFECT;
			//canny_th2 = 0;
			//adap_th2 = MAX(adap_th2, 3);
		}
	}

	if (defect_color == BLACK + WHITE + INVISIBLE)
	{
		blur(tmp, tmp, Size(5, 5));
		adap_th1 = ledoff_adap_th1;
		adap_th2 = ledoff_adap_th2;
		canny_th1 = ledoff_canny_th1;
		canny_th2 = ledoff_canny_th2;
		back_th = 0;
	}

	//OLED¿¡ °¡±î¿ï¼ö·Ï Canny ÀâÀ½ÀÌ ½ÉÇØÁüÁüÁü
	if (defect_mode == OLED_DEFECT) {
		cout << "OLED_DEFECT!!!" << endl;
		canny_th1 *= (double)((double)(focus_idx + 1) / (double)4);
		if ((defect_color == (BLACK))) {
			blur(tmp, tmp, Size(7, 7));
			adap_th2 *= 3;
		}
	}
	int new_ver = 0;
	if (black_white_mode == ON) {
		good_canny_start = 55;
		good_canny_end = 95;
		cout << "black_white_mode!!!" << endl;
		//adap_th2 *= 2.5;
		tmp = U8char_img.clone();
		new_ver = ON;
	}
	if (isBubble)
	{
		tmp = U8char_img.clone();
	}
	char fname[300];
	char       buf[80], DC[100];
	FILE	*fp_in, *fp_log;

	;
	switch (defect_color)
	{
	case (BLACK):
		sprintf(DC, "BLACK"); break;
	case (BLACK + INVISIBLE):
		sprintf(DC, "BLACK + INVISIBLE"); break;
	case (INVISIBLE):
		sprintf(DC, "INVISIBLE"); break;
	case (WHITE + INVISIBLE):
		sprintf(DC, "WHITE + INVISIBLE");
		//#ifdef PRINT_DEBUG
		if (black_white_mode) {
			sprintf(DC, "BLACK + WHITE");
			cout << "BLACK WHITE" << endl;
		}
		if (isBubble)
			sprintf(DC, "BUBBLE");
		//#endif
		break;
	case (WHITE):
		sprintf(DC, "WHITE"); break;
	case (BLACK + WHITE + INVISIBLE):
		sprintf(DC, "OLED OFF"); break;
	default:
		sprintf(DC, "default"); break;
	}


	cout << DC << endl;


	*best_focus_idx = focus_idx;
	*defect_class = DC;

	log_DV = defect_val;
	log_EV = canny_edge_ratio;
	log_SV = defect_std_val;
	log_RV = defect_ratio;
	log_BF = focus_idx;

	make_big_data(FolderPath, U8char_img(defect_ROI).clone(), best_focus_src(defect_ROI).clone(), defect_val, canny_edge_ratio, defect_std_val, defect_ratio, focus_idx, DC, bestfocus_avg, bestfocus_std, defect_color);
	//cout << "stdd_ROI : " << img_std(img_gray(defect_ROI).clone()) << endl;
	sprintf(buf, "BestFocusLOG");
	sprintf(fname, "%s/%s.csv", FolderPath.c_str(), buf);
	cout << "fname : " << buf << endl;
	fopen_s(&fp_log, fname, "a");
	if (!fp_log) {
		//waitKey();
		exit(-1);
	}
	fprintf(fp_log, "####################################################\n");
	fprintf(fp_log, "#######		Defect Classification         ######\n");
	fprintf(fp_log, "####################################################\n\n\n");

	fprintf(fp_log, "defect_val,tmp_avg,defect_std_val,defect_ratio,class,canny_edge_ratio,bestFocuslayer\n");

	fprintf(fp_log, "%.05f, ", defect_val);
	fprintf(fp_log, "%.05f, ", canny_edge_ratio);
	fprintf(fp_log, "%.05f, ", defect_std_val);
	fprintf(fp_log, "%.05f, ", defect_ratio);
	fprintf(fp_log, "%s, ", DC);
	fprintf(fp_log, "%.05f, ", canny_edge_ratio);
	fprintf(fp_log, "%d, ", focus_idx);

	fprintf(fp_log, "\n");

	fclose(fp_log);





	PrintFunc("tmptmptmptmptmptmptmptmptmptmp", tmp, 3);
	//3_th test;
	//WHITE_BLACK_defect_process(tmp, canny_img, adap_th1, adap_th2);

	//TrackBar(tmp, canny_img);
	//waitKey();
	cout << "adap_th1  : " << adap_th1 << endl;
	cout << "adap_th2  : " << adap_th2 << endl;
	cout << "canny_th1 : " << canny_th1 << endl;
	cout << "canny_th2 : " << canny_th2 << endl;
#ifdef PRINT_DEBUG
	waitKey();
#endif
	//return;
	Mat th_img, th_img2;
	good_canny_end = 98;
	if ((defect_color == WHITE + INVISIBLE)) {
		good_canny_end = 92;
	}
	//int gv = good_canny(tmp, canny_img, canny_th1, canny_th2, 15000, good_canny_start, good_canny_end, 3, back_th, new_ver);// , 15000, 80, 99, 2);
	int gv = good_canny_second(tmp, and_img, canny_img, 20000, 100, 15050, 20, good_canny_end, 2, 25, 1);
	//canny_img = canny_img - back_del;
	if ((defect_color == WHITE) || (defect_color == WHITE + INVISIBLE) || (defect_color == BLACK + WHITE + INVISIBLE))
	{
		tmp = abs(tmp.clone() - 255);
	}

	cout << "gv : " << gv << endl;


	result = (double)(clock() - loadClock) / CLOCKS_PER_SEC;
	cout << "good_canny Elapsed Time is " << result << " Seconds" << endl;
	loadClock = clock();
	//waitKey();

	PrintFunc("good_canny", canny_img, 2);
	waitKey();
	debug_canny = canny_img.clone();

	/////////////////////////////////////////////////////////////////
	//¿©±â¼­ ºÎÅÍ ºÒ·® Á¾·ù¿¡ µû¶ó ±æÀÌ ´Þ¶óÁü
	//°ËÁ¤ÀÏ¶§´Â µÎ¹øÇÑ´Ù adap_th1À» Å©°Ô ÁÖ°í ÀÛ°Ô ÁÖ°í µÎ¹øÇØ¼­ °°ÀÌ ¿¬»ê
	//ÇÏ¾çÀÏ¶§´Â OLEDÀÎÁö SCRATCH ÀÎÁö ±¸ºÐÇØ¼­ µû·Î °¡¾ßÇÏ°í 
	//Åõ¸íÀÏ¶§´Â ÇÏ¾çÀÌ ¼¯ÀÎ Åõ¸íÀÎÁö °ËÁ¤ÀÌ ¼¯ÀÎ Åõ¸íÀÎÁö °Á ¼ø¼öÇÑ Åõ¸íÀÎÁö ±¸ºÐÇØ¾ßÇÑ´Ù.
	//defect_mode , defect_color ¿ä°Íµé·Î ÀÏ´Ü ÀÌ°Í Àú°Í ÇØº¸ÀÚ
	//enum defect_type { OLED_DEFECT, SCRATCH_DEFECT, NORMAL_DEFECT };
	//enum color { BLACK, INVISIBLE, WHITE };




	defect_process(tmp, th_img, adap_th1, adap_th2);
	th_img2 = th_img.clone();
	if (defect_color == BLACK) {
		black_defect_process(tmp, canny_img, th_img2, adap_th1, adap_th2 / 4);
	}
	if (black_white_mode == ON) {
		cout << "black_white_mode" << endl;
		//defect_process(U8char_img, th_img2, adap_th1, adap_th2);
		black_defect_process(U8char_img, canny_img, th_img2, adap_th1, adap_th2 / 4);

		//th_img += th_img2;
	}
	result = (double)(clock() - loadClock) / CLOCKS_PER_SEC;
	cout << "black_defect_process Elapsed Time is " << result << " Seconds" << endl;
	loadClock = clock();
	//white_defect_process(U8char_img, th_img, adap_th1, adap_th2);

	//if (defect_mode == OLED_DEFECT) {
	//	before = defect.clone();
	//	canny_img = defect.clone();
	//
	//}
	th_img += th_img2;
	debug_before = th_img.clone();
	debug_thresh = th_img.clone();
	if (defect_color == BLACK) {
		//th_img -= back_del;
	}
	//¶ó½ºÆ® °ü½É ¿µ¿ª
	flood_extract(th_img, canny_img, defect);
	result = (double)(clock() - loadClock) / CLOCKS_PER_SEC;
	cout << "flood_extract Elapsed Time is " << result << " Seconds" << endl;
	loadClock = clock();
	debug_thresh = defect.clone();

	PrintFunc("U8char_img", tmp, 2);
	PrintFunc("gray_img", img_gray, 2);
	PrintFunc("debug_thresh", debug_thresh, 2);
	PrintFunc("debug_before", debug_before, 2);
	//PrintFunc("debug_thresh", debug_thresh, 2);

	//bitwise_and(defect, Max_img, defect);

	cout << "" << endl << endl;

	//À§¿¡¼­ Ã£Àº ¶ó½ºÆ® °ü½É¿µ¿ª¿¡¼­ FittingÀ» ÇÏ°Ô ÇÏ´Âµ¥...
	//ÀÇ¹Ì ÀÖ´Â ¿§Áö¸¸ °¡ÁßÄ¡ Áà¼­ ¿§Áö ºÎºÐÀ» ³²°Üº¸ÀÚ
	Mat tes, ots, back_avg, proposal_defect, equl;
	back_avg = Mat::zeros(debug_thresh.size(), CV_8UC1);
	dilate(debug_thresh, proposal_defect, kernel, Point(-1, -1), 5);
	if (defect_color == (WHITE + INVISIBLE) || defect_color == (WHITE)) {
		U8char_img = abs(U8char_img - 255);
	}
	bitwise_and(proposal_defect, U8char_img, tes);
	double avg_ = avg_img_th(tes, 5);
	back_avg += avg_;
	back_avg -= proposal_defect;
	tes += back_avg;
	PrintFunc("proposal_defect", tes, 3);
	equalizeHist(tes, equl);
	avg_ = avg_img(equl);
	if (avg_ > 127)
		threshold(equl, defect, 0, 255, THRESH_BINARY_INV | THRESH_OTSU);
	else
		threshold(equl, defect, 0, 255, THRESH_BINARY | THRESH_OTSU);
	PrintFunc("defect", defect, 3);

	cout << defect_color << endl;

	//if ((defect_color == (BLACK + INVISIBLE))) {
	//	Mat dil_test, extend_blob;
	//	dilate(debug_canny, dil_test, kernel, Point(-1, -1), 2);
	//	defect = dil_test.clone() + defect;
	//	medianBlur(defect, defect, 3);
	//	//delete_by_size(defect, defect, 80, 15000, defect, false);
	//	//if (avg_img(debug_before) != 0) {
	//	//
	//	//	flood_extract(defect, debug_before, defect);
	//	//}
	//	cout << "BLACK + INVISIBLE" << endl;
	//}
	if ((defect_color == (INVISIBLE)) || (defect_color == (BLACK + INVISIBLE)) || (black_white_mode == ON)) {
		Mat dil_test;
		dilate(debug_canny, dil_test, kernel, Point(-1, -1), 3);
		erode(dil_test, dil_test, kernel, Point(-1, -1), 1);
		//O:\SDOPTICS\WDMS\Images\Data\7\S2-35\B-S2-35-001
		//O:\SDOPTICS\WDMS\Images\Measure\I-S2-3289-205
		//int adap_th2= 10;
		//if(black_white_mode == ON)
		//	adap_th2 = 5;
		adaptiveThreshold(tes, tmp, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY_INV, 31, 20);
		//TrackBar(tes, tmp);
		defect = tmp + dil_test.clone();
		medianBlur(defect, defect, 3);
		//delete_by_size(defect, defect, 80, 15000, defect, false);

		if (avg_img(debug_before) != 0) {

			//flood_extract(defect, debug_before, defect);
		}
		cout << "INVISIBLE" << endl;
	}
	if ((defect_color == WHITE))
	{
		Mat dil_test;
		//dilate(debug_canny, dil_test, kernel, Point(-1, -1), 4);
		//erode(dil_test, dil_test, kernel, Point(-1, -1), 3);
		//defect += dil_test;
		cout << "WHITE defect" << endl;
	}
	if ((defect_color == (WHITE + INVISIBLE)) && black_white_mode == OFF)
	{
		if (defect_mode == SCRATCH_DEFECT)
		{
			//defect += debug_canny;
			//if (avg_img(defect) == 0)
			{
				Mat dil_test;
				dilate(debug_canny, dil_test, kernel, Point(-1, -1), 4);
				erode(dil_test, dil_test, kernel, Point(-1, -1), 3);
				defect = dil_test.clone() + debug_thresh;
				medianBlur(defect, defect, 3);
				cout << "SCRATCH_DEFECT" << endl;
			}
		}
		else
		{
			Mat dil_test;
			dilate(debug_canny, dil_test, kernel, Point(-1, -1), 4);
			erode(dil_test, dil_test, kernel, Point(-1, -1), 4);
			//O:\SDOPTICS\WDMS\Images\Measure\Ring-G-#3
			//adaptiveThreshold(tes, tmp, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY_INV, 31, 20);
			//TrackBar(tes, tmp);
			//defect = tmp + dil_test.clone();
			defect = dil_test.clone() + defect;
			medianBlur(defect, defect, 3);
			cout << "WHITE + INVISIBLE" << endl;
		}

	}
	if (defect_color == BLACK)
	{
		int black_adap1 = 11;
		int black_adap2 = 21;
		if (defect_mode == OLED_DEFECT)
		{
			black_adap1 = 131;
			black_adap2 = 121;
		}
		adaptiveThreshold(tes, defect, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY_INV, black_adap1, 5);
		PrintFunc("defect_color == BLACK", defect, 3);
		if (defect_ratio < 150) {
			//threshold(tes, tmp, avg_, 255, THRESH_BINARY_INV);
			//
			adaptiveThreshold(tes, tmp, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY_INV, black_adap2, 4);
			//TrackBar(tes, tmp);
			bitwise_and(tmp, proposal_defect, defect);
		}

		medianBlur(defect, defect, 3);
		cout << "BLACK" << endl;
	}

	if ((defect_color == (BLACK + WHITE + INVISIBLE))) {
		Mat dil_test;
		dilate(debug_canny, dil_test, kernel, Point(-1, -1), 3);
		erode(dil_test, dil_test, kernel, Point(-1, -1), 3);
		//O:\SDOPTICS\WDMS\Images\Data\7\S2-35\B-S2-35-001
		//O:\SDOPTICS\WDMS\Images\Measure\I-S2-3289-205
		adaptiveThreshold(tes, tmp, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY_INV, 31, 10);
		//TrackBar(tes, tmp);
		if (isBubble)
			defect = tmp + dil_test.clone();
		else
			defect = tmp;
		medianBlur(defect, defect, 3);
		//delete_by_size(defect, defect, 80, 15000, defect, false);

		if (avg_img(debug_before) != 0) {

			//flood_extract(defect, debug_before, defect);
		}
		cout << "OLED OFF" << endl;
	}
	//BLACK_OLEDµµ ÇÊ¿äÇÔ

	PrintFunc("equalizeHist", defect, 3);

#ifdef PRINT_DEBUG

	waitKey();
#endif

	dst.push_back(defect);//결과이미지
	dst.push_back(U8char_img);//배경제거 디버깅용
	dst.push_back(debug_canny);//캐니이미지 디버깅용
	dst.push_back(debug_before);//캐니이미지 디버깅용
	dst.push_back(debug_thresh);//이진화이미지 디버깅용
	dst.push_back(back_del);//배경제거 이미지 디버깅용
	dst.push_back(best_focus_src);//배경제거 이미지 디버깅용focus_img



}

bool CompPOSX(Point2f a, Point2f b) {
	return (a.x < b.x);
}

bool CompPOSY(Point2f a, Point2f b) {
	return (a.y < b.y);
}


//src.back() 에 들어갈 영역 받기
//1차 ROI return 넘겨주기
Rect find_ROI(Mat obj_img, Mat src_img, Mat dst, Rect ROI)
{
	
	//src object, target
	Mat img_gray;

	Mat img_1;
	Mat img_2;
	bool isLearning = TRUE;
	int highSpeedRatio = 2;
	vector<KeyPoint> keypoints1, keypoints2;
	Mat descriptors1, descriptors2;

	Ptr<ORB>orbF = ORB::create(1000);
	Ptr<BRISK> briskF = BRISK::create();
	double result;
	double minDist, maxDist;
	double fTh;
	double minx, miny, maxx, maxy;
	vector<Point2f>obj;
	vector<Point2f>scene;

	vector<Point2f>objP(4);
	vector<Point2f> sceneP(4);

	Point2f center;

	string FN;

	clock_t loadClock = clock();

	cvtColor(obj_img, img_1, CV_BGR2GRAY);
	cvtColor(src_img(ROI), img_2, CV_BGR2GRAY);


	resize(img_1, img_1, Size(img_1.cols / highSpeedRatio, img_1.rows / highSpeedRatio));
	resize(img_2, img_2, Size(img_2.cols / highSpeedRatio, img_2.rows / highSpeedRatio));

	//blur(img_1, img_1, Size(5,5));
	//blur(img_2, img_2, Size(5,5));

	
	if (!isLearning) {
		orbF->detectAndCompute(img_1, noArray(), keypoints1, descriptors1);
		briskF->detectAndCompute(img_1, noArray(), keypoints1, descriptors1);
		//save Mat, KeyPoints1;
		FN = "descriptors1.bmp";
		imwrite(FN, descriptors1);
		cout << descriptors1.depth() << endl;
		FileStorage fs("keypoints.yml", FileStorage::WRITE);
		write(fs, "aNameYouLike", keypoints1);
		fs.release();
	}
	else
	{
		//load Mat, KeyPoints1;
		descriptors1 = imread("descriptors1.bmp",0);
		FileStorage fs2("keypoints.yml", FileStorage::READ);
		FileNode kptFileNode = fs2["aNameYouLike"];
		read(kptFileNode, keypoints1);
		fs2.release();
	}

	orbF->detectAndCompute(img_2, noArray(), keypoints2, descriptors2);
	briskF->detectAndCompute(img_2, noArray(), keypoints2, descriptors2);

	result = (double)(clock() - loadClock) / CLOCKS_PER_SEC;
	cout << "detectAndCompute Elapsed Time is " << result << " Seconds" << endl;
	loadClock = clock();

	//step3: Matching descriptor vectors
	vector<DMatch> matches;
	BFMatcher matcher(NORM_HAMMING);
	matcher.match(descriptors1, descriptors2, matches);

	cout << "matches.size() = " << matches.size() << endl;
	if (matches.size() < 4)
		return Rect(0,0,1,1);

	
	minDist = maxDist = matches.front().distance;
	for (int i = 1; i < matches.size(); i++)
	{
		double dist = matches[i].distance;
		if (dist < minDist) minDist = dist;
		if (dist > maxDist) maxDist = dist;
	}
	cout << "minDist" << minDist << endl;
	cout << "maxDist" << maxDist << endl;

	vector<DMatch>goodMatches;
	fTh = 4 * minDist;
	for (int i = 1; i < matches.size(); i++)
	{
		if (matches[i].distance <= max(fTh, 0.02))
			goodMatches.push_back(matches[i]);
	}

	cout << "goodMatches.size() = " << goodMatches.size() << endl;
	if (goodMatches.size() < 4)
		return Rect(0, 0, 1, 1);

	result = (double)(clock() - loadClock) / CLOCKS_PER_SEC;
	cout << "goodMatches Elapsed Time is " << result << " Seconds" << endl;
	loadClock = clock();

	Mat imgMatches;
	drawMatches(img_1, keypoints1, img_2, keypoints2,
		goodMatches, imgMatches, Scalar::all(-1), Scalar::all(-1),
		vector<char>(), DrawMatchesFlags::NOT_DRAW_SINGLE_POINTS);

	for (int i = 0; i < goodMatches.size(); i++)
	{
		obj.push_back(keypoints1[goodMatches[i].queryIdx].pt);
		scene.push_back(keypoints2[goodMatches[i].trainIdx].pt);
	}
	Mat H = findHomography(obj, scene, RANSAC);

	objP[0] = Point2f(0, 0);
	objP[1] = Point2f(img_1.cols, 0);
	objP[2] = Point2f(img_1.cols, img_1.rows);
	objP[3] = Point2f(0, img_1.rows);

	perspectiveTransform(objP, sceneP, H);
	for (int i = 0; i < 4; i++)
		sceneP[i] += Point2f(img_1.cols, 0);
	for (int i = 0; i < 4; i++)
		line(imgMatches, sceneP[i], sceneP[(i + 1) % 4], Scalar(255, 0, 0), 4);

	dst = src_img.clone();

	//평균 좌표 구하기이미지 스티칭
	double cx = 0;
	double cy = 0;
	int num = 1;
	for (int i = 0; i < scene.size(); i++) {
		{
			//circle(PrintIMG, corners[i], 15, Scalar(255, 0, 0));
			cx = (cx*(num)+scene[i].x) / (num + 1);
			cy = (cy*(num)+scene[i].y) / (num + 1);
			num++;
		}
	}
	cx *= highSpeedRatio;
	cy *= highSpeedRatio;

	//최대 최소 좌표 구하기
	sort(scene.begin(), scene.end(), CompPOSX);
	minx = scene.front().x;
	maxx = scene.back().x;
	sort(scene.begin(), scene.end(), CompPOSY);
	miny = scene.front().y;
	maxy = scene.back().y;

	center.x = (minx + (maxx - minx) / 2)*highSpeedRatio;
	center.y = (miny + (maxy - miny) / 2)*highSpeedRatio;



	circle(dst, Point2f(cx,cy), 55, Scalar(0, 255, 255), 50);
	circle(dst, center, 55, Scalar(255, 255, 0), -1);
	PrintFunc("dst", dst, 5);
	PrintFunc("imgMathces", imgMatches, 2);
	result = (double)(clock() - loadClock) / CLOCKS_PER_SEC;
	cout << "PrintFunc Elapsed Time is " << result << " Seconds" << endl;
	loadClock = clock();
	waitKey(0);

}

void Panorama(vector<Mat> &src, Mat dst)
{
	PrintFunc("pano", src[0], 1);

	Stitcher stitcher = Stitcher::createDefault();


	unsigned long AAtime = 0, BBtime = 0; //check processing time
	AAtime = getTickCount(); //check processing time

	Stitcher::Status status = stitcher.stitch(src, dst);

	BBtime = getTickCount(); //check processing time 
	printf("%.2lf sec \n", (BBtime - AAtime) / getTickFrequency()); //check processing time

	if (Stitcher::OK == status)
		imshow("Stitching Result", dst);
	else
		printf("Stitching fail.");

	waitKey(0);
}