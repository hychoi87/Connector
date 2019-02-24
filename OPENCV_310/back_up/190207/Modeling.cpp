#include "Modeling.h"
struct MouseParams
{
	Mat img;
	Point pt;
	int lucky;
	string test;
};
Mat MouseImg;
bool click;
Point p1, p2;
string mpf;
int zoom_factor;
void CallBackFunc(int event, int x, int y, int flags, void* userdata)
{
	//MouseParams *mp = (MouseParams *)userdata;
	//Mat & img = mp->img;
	if (event == EVENT_LBUTTONDOWN && !click)
	{
		//std::cout << " Clicked - position (" << x << ", " << y << ")" << std::endl;
		//cout <<  << endl;
		p1 = Point(x, y);
		click = true;
	}
	if (event == EVENT_MOUSEMOVE && click)
	{
		Mat te = MouseImg.clone();
		rectangle(te,p1,Point(x,y),125,3);
		imshow(mpf, te);
	}
	if (event == EVENT_LBUTTONUP && click)
	{
		p2 = Point(x, y);
		click = false;
	}
}

Rect MousePrintFunc(string Fn, const Mat in, int Zoom)
{
#ifdef PRINT_DEBUG
	if (in.empty())
		return Rect(0,0,10,10);
	Mat src, src1, src2;
	src = in.clone();
	resize(src, src1, Size(src.cols / Zoom, src.rows / Zoom));
	namedWindow(Fn, WINDOW_AUTOSIZE);
	MouseParams mp;
	MouseImg = src1;
	mpf = Fn;
	zoom_factor = Zoom;
	click = false;
	//MouseImg = src1.clone();
	setMouseCallback(Fn, CallBackFunc, (void*) &mp);
	imshow(mpf, src1);
	return Rect(p1*Zoom, p2*Zoom);
#endif
}
Modeling::Modeling()
{

}
Modeling::~Modeling()
{
	fclose(fp_log);
	fclose(fp_csv);
}

Modeling::Modeling(vector <ImagePack> &in, ConfigureInfo Configure)
{
	loadClock = clock();
	//복사하기
	IMG.resize((int)(in.size()));

	std::copy(in.begin(), in.end(), IMG.begin());
	LoadConfigure = Configure;
	//출처: http://terrorjang.tistory.com/85 [개발자 공간]
	string FolderPath = LoadConfigure.input_directory;
	char fname[300];
	sprintf(fname, "%s/log_modeling.log", FolderPath.c_str());
	log_fname = fname;
	cout << "fname : " << log_fname << endl;
	fopen_s(&fp_log, log_fname.c_str(), "w");
	if (!fp_log) {
		//waitKey();
		exit(-1);
	}
	fprintf(fp_log, "#############################################\n");
	fprintf(fp_log, "##     Company	:   SD Optics Inc.			##\n");
	fprintf(fp_log, "##     File Name	:   log_modeling.txt		##\n");
	fprintf(fp_log, "##     Ver	:   1.0.0.1				##\n");
	fprintf(fp_log, "##     Data	:   16 Dec 2018			##\n");
	fprintf(fp_log, "#############################################\n\n");

	fprintf(fp_log, ">>>>>>>>>> Input Configure\n");
	fprintf(fp_log, "- input_directory	= %s\n", LoadConfigure.input_directory.c_str());
	fprintf(fp_log, "- ProductName	= %s\n", LoadConfigure.ProductName.c_str());
	fprintf(fp_log, "- InputFileType	= %s\n", LoadConfigure.InputFileType.c_str());
	fprintf(fp_log, "- OutputFileType	= %s\n", LoadConfigure.OutputFileType.c_str());

	matching_info.resize(3);

	clock_result = (double)(clock() - loadClock) / CLOCKS_PER_SEC;
	fprintf(fp_log, "\n>>>>> initial Elapsed Time is %lf Seconds\n", clock_result);
	cout << "initial Elapsed Time : " << clock_result << endl;


	sprintf(fname, "%s/Result.csv", FolderPath.c_str());
	log_fname = fname;
	cout << "fname : " << log_fname << endl;
	fopen_s(&fp_csv, log_fname.c_str(), "a");
}

void Modeling::PrintFunc(string Fn, const Mat in, int Zoom)
{

#ifdef PRINT_DEBUG
	if (in.empty())
		return;
	Mat src, src1, src2;
	src = in.clone();
	resize(src, src1, Size(src.cols / Zoom, src.rows / Zoom));
	namedWindow(Fn, WINDOW_NORMAL);
	MouseParams mp;
	mp.img = src1.clone();
	mp.test = Fn;
	//setMouseCallback(Fn, CallBackFunc, (void*) &mp);
	imshow(Fn, src1);
#endif
}
void Modeling::Thinning(Mat input, int row, int col)
{
	int x, y, p, q, xp, yp, xm, ym, cc, cd;
	int np1, sp1, hv;
	int cnt = 0, chk = 0, flag = 0;

	unsigned char *m_BufImage;
	unsigned char *m_ResultImg;
	m_BufImage = (unsigned char*)malloc(sizeof(unsigned char)*row*col);
	m_ResultImg = (unsigned char*)malloc(sizeof(unsigned char)*row*col);

	// Result image에 Mat format의 input image Copy
	for (y = 0; y < row; y++) {
		for (x = 0; x < col; x++) {
			*(m_ResultImg + (col*y) + x) = input.at<uchar>(y, x);
		}
	}

	do {
		// Image Buffer를 0으로 셋팅
		for (y = 1; y < row - 1; y++) {
			for (x = 1; x < col - 1; x++) {
				*(m_BufImage + (col*y) + x) = 0;
			}
		}

		// 천이 추출
		if (chk == 0) flag = 0;
		chk = cnt % 2;
		cnt++;

		for (y = 1; y < row - 1; y++) {
			ym = y - 1;
			yp = y + 1;
			for (x = 1; x < col - 1; x++) {
				if (*(m_ResultImg + (col*y) + x) == 0) continue;

				np1 = 0;
				for (q = y - 1; q <= y + 1; q++) {
					for (p = x - 1; p <= x + 1; p++) {
						if (*(m_ResultImg + (col*q) + p) != 0) np1++;
					}
				}

				if (np1 < 3 || np1 > 7) {
					*(m_BufImage + (col*y) + x) = 255;
					continue;
				}

				xm = x - 1;
				xp = x + 1;
				sp1 = 0;

				if (*(m_ResultImg + (col*ym) + x) == 0 && *(m_ResultImg + (col*ym) + xp) != 0) sp1++;
				if (*(m_ResultImg + (col*ym) + xp) == 0 && *(m_ResultImg + (col*y) + xp) != 0) sp1++;
				if (*(m_ResultImg + (col*y) + xp) == 0 && *(m_ResultImg + (col*yp) + xp) != 0) sp1++;
				if (*(m_ResultImg + (col*yp) + xp) == 0 && *(m_ResultImg + (col*yp) + x) != 0) sp1++;
				if (*(m_ResultImg + (col*yp) + x) == 0 && *(m_ResultImg + (col*yp) + xm) != 0) sp1++;
				if (*(m_ResultImg + (col*yp) + xm) == 0 && *(m_ResultImg + (col*y) + xm) != 0) sp1++;
				if (*(m_ResultImg + (col*y) + xm) == 0 && *(m_ResultImg + (col*ym) + xm) != 0) sp1++;
				if (*(m_ResultImg + (col*ym) + xm) == 0 && *(m_ResultImg + (col*ym) + x) != 0) sp1++;

				if (sp1 != 1) {
					*(m_BufImage + (col*y) + x) = 255;
					continue;
				}

				if (chk == 0) {
					cc = *(m_ResultImg + (col*ym) + x) * *(m_ResultImg + (col*y) + xp);
					cc = cc * *(m_ResultImg + (col*yp) + x);

					cd = *(m_ResultImg + (col*y) + xp) * *(m_ResultImg + (col*yp) + x);
					cd = cd * *(m_ResultImg + (col*y) + xm);
				}
				else {
					cc = *(m_ResultImg + (col*ym) + x) * *(m_ResultImg + (col*y) + xp);
					cc = cc * *(m_ResultImg + (col*y) + xm);

					cd = *(m_ResultImg + (col*ym) + x) * *(m_ResultImg + (col*yp) + x);
					cd = cd * *(m_ResultImg + (col*y) + xm);
				}

				if (cc != 0 || cd != 0) {
					*(m_BufImage + (col*y) + x) = 255;
					continue;
				}
				flag = 1;
			}
		}

		for (y = 1; y < row - 1; y++) {
			for (x = 1; x < col - 1; x++) {
				*(m_ResultImg + (col*y) + x) = *(m_BufImage + (col*y) + x);
			}
		}
	} while (!(chk == 1 && flag == 0));

	// 4연결점 처리
	for (y = 1; y < row - 1; y++) {
		yp = y + 1;
		ym = y - 1;
		for (x = 1; x < col - 1; x++) {
			if (*(m_ResultImg + (col*y) + x) == 0) continue;

			xm = x - 1;
			xp = x + 1;
			sp1 = 0;
			if (*(m_ResultImg + (col*ym) + x) == 0 && *(m_ResultImg + (col*ym) + xp) != 0) sp1++;
			if (*(m_ResultImg + (col*ym) + xp) == 0 && *(m_ResultImg + (col*y) + xp) != 0) sp1++;
			if (*(m_ResultImg + (col*y) + xp) == 0 && *(m_ResultImg + (col*yp) + xp) != 0) sp1++;
			if (*(m_ResultImg + (col*yp) + xp) == 0 && *(m_ResultImg + (col*yp) + x) != 0) sp1++;
			if (*(m_ResultImg + (col*yp) + x) == 0 && *(m_ResultImg + (col*yp) + xm) != 0) sp1++;
			if (*(m_ResultImg + (col*yp) + xm) == 0 && *(m_ResultImg + (col*y) + xm) != 0) sp1++;
			if (*(m_ResultImg + (col*y) + xm) == 0 && *(m_ResultImg + (col*ym) + xm) != 0) sp1++;
			if (*(m_ResultImg + (col*ym) + xm) == 0 && *(m_ResultImg + (col*ym) + x) != 0) sp1++;

			hv = 0;
			if (sp1 == 2) {
				if ((*(m_ResultImg + (col*ym) + x) & *(m_ResultImg + (col*y) + xp)) != 0) hv++;
				else if ((*(m_ResultImg + (col*y) + xp) & *(m_ResultImg + (col*yp) + x)) != 0) hv++;
				else if ((*(m_ResultImg + (col*yp) + x) & *(m_ResultImg + (col*y) + xm)) != 0) hv++;
				else if ((*(m_ResultImg + (col*y) + xm) & *(m_ResultImg + (col*ym) + x)) != 0) hv++;

				if (hv == 1) *(m_ResultImg + (col*y) + x) = 0;
			}
			else if (sp1 == 3) {
				if ((*(m_ResultImg + (col*ym) + x) & *(m_ResultImg + (col*y) + xm) & *(m_ResultImg + (col*y) + xp)) != 0) hv++;
				else if ((*(m_ResultImg + (col*yp) + x) & *(m_ResultImg + (col*y) + xm) & *(m_ResultImg + (col*y) + xp)) != 0) hv++;
				else if ((*(m_ResultImg + (col*ym) + x) & *(m_ResultImg + (col*yp) + x) & *(m_ResultImg + (col*y) + xm)) != 0) hv++;
				else if ((*(m_ResultImg + (col*ym) + x) & *(m_ResultImg + (col*yp) + x) & *(m_ResultImg + (col*y) + xp)) != 0) hv++;

				if (hv == 1) *(m_ResultImg + (col*y) + x) = 0;
			}
		}
	}

	// 들어온 배열에 재복사
	for (y = 0; y < row; y++) {
		for (x = 0; x < col; x++) {
			input.at<uchar>(y, x) = *(m_ResultImg + (col*y) + x);
		}
	}

	free(m_BufImage);
	free(m_ResultImg);
}
double Modeling::avg_img(Mat in)
{
	Mat src = in.clone();

	int w = src.size().width;

	int h = src.size().height;

	double avg = 0.0;

	int num = 1;
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
bool Modeling::readCellInfo()
{
	String s1 = LoadConfigure.input_directory;

	//\\ 제일 마지막에 있는거 찾아서 넣어주자
	size_t location = s1.find_last_of("\\");
	string TID = s1.substr(location + 1, s1.size() - location);
	string file_name = LoadConfigure.input_directory + "\\CellInfo_" + TID + "_WMP.1.txt";

	cout << file_name << endl;

	//여기서 다양한 Dentium_FS_Extra.ini만들어서쓰자
	ifstream cell_info;
	cell_info.open(file_name.c_str());
	if (cell_info.is_open())
	{
		string line;
		while (getline(cell_info, line))
		{
			stringstream linestream(line);
			string value;

			size_t adj_pos, focus_pos;
			//adj 부터 찾자
			if ((adj_pos = line.find("adj")) != string::npos) {
				cout << line << endl;
				//adj가 FOCUS 뒤에 올때도 있고 아닐때도 있어서 일단 adj 뒤로 다 가져온다
				string pre_line = line.substr(adj_pos, line.size() - adj_pos);
				cout << pre_line << endl;
				size_t xpos, ypos;
				//pre_line 에서 {,} 를 찾아서 그만큼 가져오면 숫자랑 쉼표만 남으니 그걸 이용한다.
				if ((xpos = pre_line.find("{")) != string::npos && (ypos = pre_line.find("}")) != string::npos) {
					string adj_focus_list = pre_line.substr(xpos + 1, ypos - xpos - 1);
					cout << adj_focus_list << endl;
					//pre_line에서 시작좌표랑 끝좌표를 아니깐 중간의 , 좌표 찾아서 잘라서 쓴다
					while (1) {
						size_t dot = pre_line.find(",", xpos + 1);
						if (dot > ypos)
							break;
						if (dot == -1)
							adj_focus_list = pre_line.substr(xpos + 1, ypos - xpos - 1);
						else
							adj_focus_list = pre_line.substr(xpos + 1, dot - xpos - 1);
						cout << adj_focus_list << endl;
						adj_focus.push_back(stoi(adj_focus_list));
						xpos = dot;
						if (xpos == -1)
							break;
					}
				}
			}
			if ((focus_pos = line.find("Focus")) != string::npos) {
				cout << line << endl;
				//adj가 FOCUS 뒤에 올때도 있고 아닐때도 있어서 일단 adj 뒤로 다 가져온다
				string pre_line = line.substr(focus_pos, line.size() - focus_pos);
				cout << pre_line << endl;
				size_t xpos, ypos;
				//pre_line 에서 {,} 를 찾아서 그만큼 가져오면 숫자랑 쉼표만 남으니 그걸 이용한다.
				if ((xpos = pre_line.find("{")) != string::npos && (ypos = pre_line.find("}")) != string::npos) {
					string set_focus_list = pre_line.substr(xpos + 1, ypos - xpos - 1);
					cout << set_focus_list << endl;
					//pre_line에서 시작좌표랑 끝좌표를 아니깐 중간의 , 좌표 찾아서 잘라서 쓴다
					while (1) {
						size_t dot = pre_line.find(",", xpos + 1);
						if (dot > ypos)
							break;

						if (dot == -1)
							set_focus_list = pre_line.substr(xpos + 1, ypos - xpos - 1);
						else
							set_focus_list = pre_line.substr(xpos + 1, dot - xpos - 1);
						cout << set_focus_list << endl;
						set_focus.push_back(stoi(set_focus_list));
						xpos = dot;
						if (xpos == -1)
							break;
					}
				}
			}
		}
	}
	else
	{
		return false;
	}
	return true;
}


Mat Modeling::ParseImage(int img_num, int focus_num, img_type img_type)
{
	Mat return_img;
	for (int i = 0; i < IMG.size(); i++)
	{
		string str = IMG[i].ImageName;
		string tmp = "I" + int2str(img_num) + "-" + int2str(focus_num);
		string base = IMG[i].ImageName;
		//cout << tmp << endl;
		if ((base.find(tmp) != string::npos)) {
			Mat return_img;
			switch (img_type) {
			case ORIGINAL: return_img = IMG[i].Original.clone(); break;
			case GRAY: return_img = IMG[i].Gray.clone(); break;
			case RED: return_img = IMG[i].R.clone(); break;
			case GREEN: return_img = IMG[i].G.clone(); break;
			case BLUE: return_img = IMG[i].B.clone(); break;
			default: return_img = IMG[i].Original.clone(); break;
			}
			return return_img;
		}

	}
	return return_img;
}

bool Modeling::LoadModeingINI()
{
	FILE	*fp_in;
	string file_name = ".\\DB\\" + LoadConfigure.ProductName + "\\INI\\Modeling.ini";
	cout << file_name << endl;
	//여기서 다양한 Dentium_FS_Extra.ini만들어서쓰자
	//fopen_s(&fp_log, log_fname.c_str(), "a");
	fprintf(fp_log, "\n");
	fprintf(fp_log, ">>>>>>>>>> Input Modeling.ini\n");
	fprintf(fp_log, "- input_directory  = %s\n", file_name.c_str());
	
	
	fopen_s(&fp_in, file_name.c_str(), "r");
	if (fp_in) {

		fscanf(fp_in, "%s", ChkString);
		if (!strcmp(ChkString, "[ModelingBEGIN]")) {
			while (1) {
				if (fscanf(fp_in, "%s", ChkString) == EOF) break;
				if (!strcmp(ChkString, "[ModelingEND]")) break;
				if (!strcmp(ChkString, "##")) {
					while (1) if (getc(fp_in) == '\n') break;
				}
				ModelingInfo temp;
				if (!strcmp(ChkString, "Chip")) {
					fscanf(fp_in, "%d %d %d %d %d %d %d %d %lf %lf %d %lf %lf %d\n", &temp.img_num, &temp.focus_num, &temp.ROI.x, &temp.ROI.y, &temp.ROI.width, &temp.ROI.height, &temp.thickness, &temp.method, &temp.th1, &temp.th2, &temp.pre_process, &temp.pre_th1, &temp.pre_th2, &temp.location);
					//fprintf(fp_log, "%s : ", ChkString);
					//fprintf(fp_log, "%d %d %d %d %d %d %d %d %lf %lf %lf %lf\n", temp.img_num, temp.focus_num, temp.ROI.x, temp.ROI.y, temp.ROI.width, temp.ROI.height, temp.method, temp.location, temp.th1, temp.th2, temp.pre_th1, temp.pre_th2);
					Chip_List.push_back(temp);
				}
				if (!strcmp(ChkString, "INSIDE")) {
					fscanf(fp_in, "%d %d %d %d %d %d %d %d %lf %lf %d %lf %lf %d\n", &temp.img_num, &temp.focus_num, &temp.ROI.x, &temp.ROI.y, &temp.ROI.width, &temp.ROI.height, &temp.thickness, &temp.method, &temp.th1, &temp.th2, &temp.pre_process, &temp.pre_th1, &temp.pre_th2, &temp.location);
					//fprintf(fp_log, "%s : ", ChkString);
					//fprintf(fp_log, "%d %d %d %d %d %d %d %d %lf %lf %lf %lf\n", temp.img_num, temp.focus_num, temp.ROI.x, temp.ROI.y, temp.ROI.width, temp.ROI.height, temp.method, temp.location, temp.th1, temp.th2, temp.pre_th1, temp.pre_th2);
					Inside_List.push_back(temp);
				}
				if (!strcmp(ChkString, "OUTSIDE")) {
					fscanf(fp_in, "%d %d %d %d %d %d %d %d %lf %lf %d %lf %lf %d\n", &temp.img_num, &temp.focus_num, &temp.ROI.x, &temp.ROI.y, &temp.ROI.width, &temp.ROI.height, &temp.thickness, &temp.method, &temp.th1, &temp.th2, &temp.pre_process, &temp.pre_th1, &temp.pre_th2, &temp.location);
					//fprintf(fp_log, "%s : ", ChkString);
					//fprintf(fp_log, "%d %d %d %d %d %d %d %d %lf %lf %lf %lf\n", temp.img_num, temp.focus_num, temp.ROI.x, temp.ROI.y, temp.ROI.width, temp.ROI.height, temp.method, temp.location, temp.th1, temp.th2, temp.pre_th1, temp.pre_th2);
					Outside_List.push_back(temp);
				}
				if (!strcmp(ChkString, "SKEW")) {
					fscanf(fp_in, "%d %d %d %d %d %d %d %d %lf %lf %d %lf %lf %d\n", &temp.img_num, &temp.focus_num, &temp.ROI.x, &temp.ROI.y, &temp.ROI.width, &temp.ROI.height, &temp.thickness, &temp.method, &temp.th1, &temp.th2, &temp.pre_process, &temp.pre_th1, &temp.pre_th2, &temp.location);
					//fprintf(fp_log, "%s : ", ChkString);
					//fprintf(fp_log, "%d %d %d %d %d %d %d %d %lf %lf %lf %lf\n", temp.img_num, temp.focus_num, temp.ROI.x, temp.ROI.y, temp.ROI.width, temp.ROI.height, temp.method, temp.location, temp.th1, temp.th2, temp.pre_th1, temp.pre_th2);
					SKEW_List.push_back(temp);
				}
				if (!strcmp(ChkString, "HOUGH")) {
					fscanf(fp_in, "%d %d %d %d %d %d %d %d %lf %lf %d %lf %lf %d\n", &temp.img_num, &temp.focus_num, &temp.ROI.x, &temp.ROI.y, &temp.ROI.width, &temp.ROI.height, &temp.thickness, &temp.method, &temp.th1, &temp.th2, &temp.pre_process, &temp.pre_th1, &temp.pre_th2, &temp.location);
					//fprintf(fp_log, "%s : ", ChkString);
					//fprintf(fp_log, "%d %d %d %d %d %d %d %d %lf %lf %lf %lf\n", temp.img_num, temp.focus_num, temp.ROI.x, temp.ROI.y, temp.ROI.width, temp.ROI.height, temp.method, temp.location, temp.th1, temp.th2, temp.pre_th1, temp.pre_th2);
					HOUGH_List.push_back(temp);
				}


			}
		}
		fclose(fp_in);
	}
	else{
		fprintf(fp_log, "Fail Load INI\n");
		fprintf(fp_log, "end program\n");
		//fclose(fp_log);
		return false;
	}
	//fclose(fp_log);
	return true;
}
Mat Modeling::ParseImage(int img_num, int focus_num, int index_num, img_type img_type)
{
	Mat return_img;
	for (int i = 0; i < IMG.size(); i++)
	{
		string str = IMG[i].ImageName;
		string tmp = "I" + int2str(img_num) + "-" + int2str(focus_num) + "-" + int2str(index_num);
		string base = IMG[i].ImageName;
		//cout << tmp << endl;
		if ((base.find(tmp) != string::npos)) {
			switch (img_type) {
			case ORIGINAL: return_img = IMG[i].Original.clone(); break;
			case GRAY: return_img = IMG[i].Gray.clone(); break;
				//case RED: return_img = IMG[i].R.clone(); break;
				//case GREEN: return_img = IMG[i].G.clone(); break;
				//case BLUE: return_img = IMG[i].B.clone(); break;
			default: return_img = IMG[i].Original.clone(); break;
			}
			return return_img;
		}

	}
	return return_img;
}
Mat Modeling::ParseImage(int img_num, int focus_num, int R, int C, img_type img_type)
{
	Mat return_img;
	for (int i = 0; i < IMG.size(); i++)
	{
		string str = IMG[i].ImageName;
		string tmp = "I" + int2str(img_num) + "-" + int2str(focus_num) + "-R" + int2str(R) + "C" + int2str(C);
		string base = IMG[i].ImageName;
		//cout << tmp << endl;
		if ((base.find(tmp) != string::npos)) {			
			switch (img_type) {
			case ORIGINAL: return_img = IMG[i].Original.clone(); break;
			case GRAY: return_img = IMG[i].Gray.clone(); break;
			//case RED: return_img = IMG[i].R.clone(); break;
			//case GREEN: return_img = IMG[i].G.clone(); break;
			//case BLUE: return_img = IMG[i].B.clone(); break;
			default: return_img = IMG[i].Original.clone(); break;
			}
			return return_img;
		}

	}
	return return_img;
}
Point2f Modeling::PosCenter(Point2f p1, Point2f p2)
{
	Point2f distance;

	// 피타고라스의 정리
	// pow(x,2) x의 2승,  sqrt() 제곱근
	float MAXVal, MINVal;
	MAXVal = MAX(p1.x, p2.x);
	MINVal = MIN(p1.x, p2.x);
	distance.x = ((MAXVal - MINVal) / 2.0f) + MINVal;

	MAXVal = MAX(p1.y, p2.y);
	MINVal = MIN(p1.y, p2.y);
	distance.y = ((MAXVal - MINVal) / 2.0f) + MINVal;

	return distance;

}

bool Modeling::calSkewFunc()
{
	fprintf(fp_log, "\n>>>>>>>>>> Start calSkewFunc\n");
	Mat test, ROI_IMG, th_img, canny_img, defect, src, img_gray, img_result;
	ModelingInfo info, Hough;
	Hough = HOUGH_List.back();
	for (int i = 0; i < SKEW_List.size(); i++) {
		info = SKEW_List[i];
		src = ParseImage(info.img_num, info.focus_num, GRAY);
		if (src.empty())
		{
			fprintf(fp_log, "No ChipModeling Image I%d-%d....\n", info.img_num, info.focus_num);
			//fclose(fp_log);
			return false;
		}
		else
		{
			fprintf(fp_log, "Load I%d-%d Image done...\n", info.img_num, info.focus_num);
		}
		if (src.channels() == 3)
			cvtColor(src, src, CV_BGR2GRAY);
		else
			src = src.clone();
		threshold(src, img_result, 0, 255, THRESH_BINARY | THRESH_OTSU);
		Moments m = moments(img_result);
		Point2f mo_cen;
		mo_cen.x = (float)(m.m10 / m.m00);
		mo_cen.y = (float)(m.m01 / m.m00);//y_avg; //
		cvtColor(img_result, th_img,CV_GRAY2BGR);
		circle(th_img,mo_cen,55,CV_RGB(255,0,0),-1);
		info.ROI.x = mo_cen.x + info.ROI.x;
		info.ROI.y = mo_cen.y + info.ROI.y;
		Center = mo_cen;
		rectangle(th_img,info.ROI,CV_RGB(0,0,255),15);
		p1.x = info.ROI.x;
		p1.y = info.ROI.y;
		p2.x = info.ROI.x+  info.ROI.width;
		p2.y = info.ROI.y + info.ROI.height;
		PrintFunc("THRESH_OTSU", th_img, 2);
		p1 /= 2;
		p2 /= 2;
		//마우스스스스
		info.ROI = MousePrintFunc("Draw ROI", src, 2);
		waitKey();

		info.ROI = Rect(p1 * 2, p2 * 2);
		if (info.ROI.width < 0 && info.ROI.x == 0)
			info.ROI.width = src.cols - info.ROI.x;
		if (info.ROI.height < 0 && info.ROI.y == 0)
			info.ROI.height = src.rows - info.ROI.y;;

		if ((info.ROI.width + info.ROI.x) > src.cols)
			info.ROI.width = src.cols - info.ROI.x;
		if ((info.ROI.height + info.ROI.y) > src.rows)
			info.ROI.height = src.rows - info.ROI.y;

		img_gray = src(info.ROI);

		if (info.method == HARD_MASKING_MODE) {
			//하드마스킹은 그냥 하양것만 넘겨준다 그렇게 한 후에 나중에 하드만 모아서 빼주기 연산해줘야함
			threshold(img_gray, img_result, 0, 255, THRESH_BINARY);
		}
		else if (info.method == THRESHOLD_MODE) {
			//단순 TH
			threshold(img_gray, img_result, info.th1, 255, THRESH_BINARY);
		}
		else if (info.method == THRESHOLD_INV_MODE) {
			//단순 TH_INV
			threshold(img_gray, img_result, info.th1, 255, THRESH_BINARY_INV);
		}
		else if (info.method == ADAPTHRESHOLD_MODE) {
			adaptiveThreshold(img_gray, img_result, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, info.th1, info.th2);
		}
		else if (info.method == ADAPTHRESHOLD_INV_MODE) {
			adaptiveThreshold(img_gray, img_result, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY_INV, info.th1, info.th2);
		}
		else if (info.method == CANNY_MODE) {
			Canny(img_gray, img_result, info.th1, info.th2);
		}
		else if (info.method == OTSU_CANNY_MODE) {
			double otsu_val = threshold(img_gray, img_result, 0, 255, THRESH_BINARY | THRESH_OTSU);
			Canny(img_gray, img_result, otsu_val*info.th1, otsu_val*info.th2);
		}
		else {
			//좌표순서대로 라인그려주자
			adaptiveThreshold(img_gray, img_result, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, info.th1, info.th2);
			int label_num = 1;
			Mat img_labels, stats, centroids;
			int numOfLables = connectedComponentsWithStats(img_result, img_labels,
				stats, centroids, 8, CV_32S);

			PrintFunc("adaptiveThreshold", img_result, 1);
			defect = Mat::zeros(img_result.size(), CV_8UC1);
			int bottom = 0;
			int left, top, height, width;
			Point tmp;
			vector<Point> left_top;
			for (int j = 1; j < numOfLables; j++) {
				int area = stats.at<int>(j, CC_STAT_AREA);
				left = stats.at<int>(j, CC_STAT_LEFT);
				top = stats.at<int>(j, CC_STAT_TOP);
				height = stats.at<int>(j, CC_STAT_HEIGHT);
				width = stats.at<int>(j, CC_STAT_WIDTH);
				//가로 길이가 가장 비슷한것만 골라내자
				if (info.pre_th1<area && area < info.pre_th2)
				{
				
					cout << area << endl;
					//if(height*2 > width)
					{
						cout << "height * 2 : " << height * 2 << endl;
						cout << "width      : " << width << endl;
						left_top.push_back(Point(left,top));
					}
				}
			}
			sort(left_top.begin(), left_top.end(), CompPOSX);
			for (int i = 0; i < left_top.size() - 1; i++){
				PosRadius(left_top[i], left_top[i + 1]);
				line(defect, left_top[i], left_top[i + 1], 255, 1);
			}
			img_result = defect.clone();
		}
		Mat MapROI;
		cvtColor(img_result, MapROI, CV_GRAY2BGR);
		Mat imageROI(th_img, info.ROI);
		MapROI.copyTo(imageROI, MapROI);

		PrintFunc("src", src, 2);
		PrintFunc("img_result", img_result, 1);
		PrintFunc("img_gray", img_gray, 2);
		waitKey();
		//Thinning(img_result, img_result.rows, img_result.cols);
		Canny(img_result, img_result, 120, 360);
		test = img_result.clone();
		PrintFunc("Canny", img_result, 1);
		//waitKey();
		vector<vector<Point>> contours;
		cv::findContours(img_result, contours, cv::RETR_LIST, cv::CHAIN_APPROX_SIMPLE);
		cv::Vec4f line;
		int max_idx,max_val;
		max_val = 0;
		max_idx = 0;
		for (int i = 0; i < contours.size(); i++) {
			if (contours[i].size() > max_val) {
				max_val = contours[i].size();
				max_idx = i;
			}
		}

		cv::fitLine(cv::Mat(contours[max_idx]), line,
			CV_DIST_L2, // 거리 유형 CV_DIST_HUBER CV_DIST_L2
			0,   // L2 거리를 사용하지 않음
			0.001, 0.001); // 정확도
						 // 단위 방향 벡터(cv::Vec4f의 첫 두개 값), 
						 // 선에 놓인 한 점의 좌표(cv::Vec4f의 마지막 두 값) 형태인 선 방정식의 파라미터를 제공
						 // 마지막 두 파라미터는 선 파라미터에 대한 요구 정확도를 지정
						 // 함수에서 요구 하는 std::vector 내에 포함된 입력 점은 cv::Mat로 전달

						 // 선 방정식은 일부 속성계산에 사용
						 // 올바른 선을 계산하는지 확인하기 위해 영상에 예상 선을 그림
						 // 200화소 길이와 3화소 두께를 갖는 임의의 검은 세그먼트를 그림
		
		int x0 = line[2]; // 선에 놓은 한 점
		int y0 = line[3];
		int x1 = x0 - 1200 * line[0]; // 200 길이를 갖는 벡터 추가
		int y1 = y0 - 1200 * line[1]; // 단위 벡터 사용
		cv::line(img_gray, cv::Point(x0, y0), cv::Point(x1, y1), cv::Scalar(120), 3);
		cv::line(test, cv::Point(x0, y0), cv::Point(x1, y1), cv::Scalar(120), 3);
		Point th_pos;
		th_pos.x = info.ROI.x;
		th_pos.y = info.ROI.y;
		cv::line(th_img, cv::Point(x0, y0)+ th_pos, cv::Point(x1, y1)+ th_pos, cv::Scalar(255,0,0), 3);
		
		double rad = (double)(atan2l( ((double)y0 - y1), ((double)x0 - x1)) * 180.0 / CV_PI);//+0.018; // -1 보정
		PrintFunc("img_result2", test, 1);
		test = deskew(src, rad);
		main_rad = rad;
		//cout << th * (180 / 3.14) << endl;
		PrintFunc("th_img", th_img, 1);
		PrintFunc("houghImg", img_gray, 1);
		PrintFunc("deskew", test,4);
		waitKey();
		deskew_all();
	}
	
}

Mat Modeling::deskew(Mat& img, double skew)
{

	//출처: http://crazymind.net/47 [현실감각 0%]
	cout << "skew : " << skew << endl;
	// Calculate affine transform to correct skewness. 
	//Mat warpMat = (Mat_<double>(2, 3) << 1, skew, -0.5*SZ*skew, 0, 1, 0);
	Point2f pos;
	pos.x = img.cols / 2;
	pos.y = img.rows / 2;
	Mat warpMat = getRotationMatrix2D(pos, skew, 1.0);
	cout << "fiducial_center : " << skew << endl;
	Mat imgOut = Mat::zeros(img.rows, img.cols, img.type());
	warpAffine(img, imgOut, warpMat, imgOut.size());

	return imgOut;
}
void Modeling::deskew_all()
{
	//parrel for를 쓸꺼야
	//vector<ImagePack> skew_IMG(IMG.size());skew = fiducial_angle;
#pragma omp parallel for
	for (int i = 0; i < IMG.size(); i++)
	{
		Mat rot_img = deskew(IMG[i].Original, main_rad);
		IMG[i].Original = rot_img.clone();
		cvtColor(rot_img, IMG[i].Gray, CV_BGR2GRAY);
		vector <Mat> BGR;
		split(rot_img,BGR);
		Mat image_r(rot_img.rows, rot_img.cols, CV_8UC1);
		Mat image_g(rot_img.rows, rot_img.cols, CV_8UC1);
		Mat image_b(rot_img.rows, rot_img.cols, CV_8UC1);
		Mat out[] = { image_b, image_g, image_r };
		int from_to[] = { 0,0, 1,1, 2,2 };
		mixChannels(&rot_img, 1, out, 3, from_to, 3);
		IMG[i].R = image_r.clone();
		IMG[i].G = image_g.clone();
		IMG[i].B = image_b.clone();
		//rot_img = deskew(IMG[i].Gray, Fiducial_Center);
		//IMG[i].Gray = rot_img.clone();
		//string fn = LoadConfigure.input_directory;
		//fn = fn+"\\" + IMG[i].ImageName + "_rot.bmp";
		//imwrite(fn, rot_img);
	}
}
Point2d Modeling::processInside(Mat src, ModelingInfo info)
{
	Mat test, ROI_IMG, th_img, canny_img, defect, img_gray, img_result;
	//예외처리 시작
	if (info.ROI.width < 0) {
		int resize = abs(info.ROI.width);
		info.ROI.width = src.cols - info.ROI.x;
		//info.ROI.width /= resize;
	}
	if (info.ROI.height < 0) {
		int resize = abs(info.ROI.height);
		info.ROI.height = src.rows - info.ROI.y;;
		//info.ROI.height /= resize;
	}
	if ((info.ROI.width + info.ROI.x) > src.cols)
		info.ROI.width = src.cols - info.ROI.x;
	if ((info.ROI.height + info.ROI.y) > src.rows)
		info.ROI.height = src.rows - info.ROI.y;

	img_gray = src(info.ROI);
	PrintFunc("inside src", src, 1);
	PrintFunc("inside ROI", img_gray, 1);
	//패턴 매칭으로 근처라도 가자
	Point2f pre_pos, find_pos;
	Rect pre_ROI;
	Mat tmpl;
	string file_name = ".\\DB\\" + LoadConfigure.ProductName + "\\KeyImage\\Modeling\\Pattern" + int2str(info.pre_process) + ".bmp";
	tmpl = imread(file_name);
	if (tmpl.empty())
	{
		file_name = ".\\DB\\" + LoadConfigure.ProductName + "\\KeyImage\\Modeling\\Pattern" + int2str(info.pre_process) + ".jpg";
		tmpl = imread(file_name);
		if (tmpl.empty())
		{
			cout << "no img : " << file_name << endl;
			waitKey();
		}
	}
	//cout << file_name << endl;
	//템플릿 없는것도 로그화 예외처리 해야함.
	Mat matching_map = patternMatchingMethod(img_gray, tmpl);

	int size = ((tmpl.cols + tmpl.rows) / 4) * 42 + 1; //force size to be odd 42
	adaptiveThreshold(matching_map, defect, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, size, info.pre_th1);
	//-128
	//rectangle(ROI_IMG, Ball_ROI, CV_RGB(255, 0, 100), 5);
	//PrintFunc("tmpl", tmpl, 2);
	PrintFunc("inside matching_map", matching_map, 2);
	PrintFunc("inside defect", defect, 2);
	//PrintFunc("defect", defect, 2);
	//PrintFunc("matching_map", matching_map, 2);
	double extend_size_x = 0;;//tmpl.cols;
	double extend_size_y = 0;;//tmpl.rows;
	ModelingInfo match_tmp;
	match_tmp.ROI.x = 0;
	match_tmp.ROI.y = 0;
	match_tmp.ROI.width = 1;
	match_tmp.ROI.height = 1;
	//while (true)
	{
		//변수화 필요
		double minval, maxval, threshold = info.pre_th2;
		Point minloc, maxloc;
		//bitwise_and();
		minMaxLoc(matching_map, &minval, &maxval, &minloc, &maxloc);
		//로그필요
		cout << "maxval : " << maxval << endl;
		if (maxval >= threshold)
		{

			//패턴매칭된 제대로된 위치를 찾기위한것
			floodFill(defect, maxloc, 0); //mark drawn blob     
			bitwise_and(defect, matching_map, matching_map);

			if (1)
			{

				//2차 볼찾기에서 유리하려면 여기서 찾은범위보다 더 넓게 찾아야한다.
				//ModelingInfo match_tmp;
				match_tmp.corners[1].x = maxloc.x;
				match_tmp.corners[1].y = maxloc.y;
				match_tmp.corners[2].x = maxloc.x + tmpl.cols;
				match_tmp.corners[2].y = maxloc.y + tmpl.rows;
				match_tmp.ROI.x = maxloc.x + info.ROI.x - extend_size_x;
				match_tmp.ROI.y = maxloc.y + info.ROI.y - extend_size_y;
				match_tmp.ROI.width = tmpl.cols + (extend_size_x * 2);
				match_tmp.ROI.height = tmpl.rows + (extend_size_y * 2);
				Mat tt = img_gray.clone();
				rectangle(tt, match_tmp.corners[1], match_tmp.corners[2], 155, 1);
				PrintFunc("inside rectangle ROI", tt, 1);
				//break;
			}
		}
		else
		{
			cout << "패턴매칭 실패" << endl;
			return Point2d(0, 0);
			//break;
		}
	}
	img_gray = src(match_tmp.ROI);

	//resize(img_gray, img_gray, Size(), 0.25, 0.25);
	if (info.method == HARD_MASKING_MODE) {
		//하드마스킹은 그냥 하양것만 넘겨준다 그렇게 한 후에 나중에 하드만 모아서 빼주기 연산해줘야함
		threshold(img_gray, img_result, 0, 255, THRESH_BINARY);
	}
	else if (info.method == THRESHOLD_MODE) {
		//단순 TH
		threshold(img_gray, img_result, info.th1, 255, THRESH_BINARY);
	}
	else if (info.method == THRESHOLD_INV_MODE) {
		//단순 TH_INV
		threshold(img_gray, img_result, info.th1, 255, THRESH_BINARY_INV);
	}
	else if (info.method == ADAPTHRESHOLD_MODE) {
		adaptiveThreshold(img_gray, img_result, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, info.th1, info.th2);
	}
	else if (info.method == ADAPTHRESHOLD_INV_MODE) {
		adaptiveThreshold(img_gray, img_result, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY_INV, info.th1, info.th2);
	}
	else if (info.method == CANNY_MODE) {
		Canny(img_gray, img_result, info.th1, info.th2);
	}
	else if (info.method == OTSU_CANNY_MODE) {
		double otsu_val = threshold(img_gray, img_result, 0, 255, THRESH_BINARY | THRESH_OTSU);
		Canny(img_gray, img_result, otsu_val*info.th1, otsu_val*info.th2);
	}

	//threshold(img_result, img_result, 0, 255, THRESH_BINARY | THRESH_OTSU);
	if (info.thickness >= 1){
		rectangle(img_result, Rect(0, 0, match_tmp.ROI.width, match_tmp.ROI.height), 255, info.thickness);
		floodFill(img_result, Point(1, 1), 0); //mark drawn blob  
	}

	PrintFunc("Inside img_result", img_result, 1);
	int label_num = 1;
	Mat img_labels, stats, centroids;
	int numOfLables = connectedComponentsWithStats(img_result, img_labels,
		stats, centroids, 8, CV_32S);

	defect = Mat::zeros(th_img.size(), CV_8UC1);
	int bottom = 0;
	int left, top, height, width;
	Rect tmp;
	for (int j = 1; j < numOfLables; j++) {
		int area = stats.at<int>(j, CC_STAT_AREA);
		left = stats.at<int>(j, CC_STAT_LEFT);
		top = stats.at<int>(j, CC_STAT_TOP);
		height = stats.at<int>(j, CC_STAT_HEIGHT);
		width = stats.at<int>(j, CC_STAT_WIDTH);
		//가로 길이가 가장 비슷한것만 골라내자
		if(100<area){
		if (height < width) {
			if(bottom < top){
				bottom = top;
				cout << area << endl;
				tmp.x = left;
				tmp.y = top;
				tmp.height = height;
				tmp.width = width;
				label_num = j;
				
			}
		}
		}
	}
	Mat S;
	inRange(img_labels, label_num, label_num, img_result);
	//PrintFunc("S", img_result, 1);
	//waitKey();
	//unsigned char* data = (unsigned char*)img_result.data;
	//int max_pos = 0;
	//int x_pos = 0;
	//S = img_result.clone();
	//for (int x = 0; x < img_result.cols; x++){
	//	int y_pos;
	//	for (int y = img_result.rows - 1; y > 1; y--) {
	//		if (data[y *  img_result.cols + x] != 0) {
	//			y_pos = y;
	//			//line(S, Point(x, img_result.rows), Point(x, y),125,1);
	//			//PrintFunc("S", S, 1);
	//			//cout << y_pos << endl;
	//			//waitKey();
	//			break;
	//		}
	//	}
	//	if (y_pos > max_pos)
	//	{
	//		max_pos = y_pos;
	//		x_pos = x;
	//	}
	//}


	circle(img_result, Point2d(match_tmp.ROI.x+ match_tmp.ROI.width/2, match_tmp.ROI.y + match_tmp.ROI.height/2), 15, 125, 3);
	PrintFunc("Inside S", img_result, 1);
	//waitKey();
	return Point2d(match_tmp.ROI.x + match_tmp.ROI.width / 2 + info.ROI.x, match_tmp.ROI.y + match_tmp.ROI.height/2 + info.ROI.y);
}
Point2d Modeling::processOutside(Mat src, ModelingInfo info)
{
	Mat test, ROI_IMG, th_img, canny_img, defect, img_gray, img_result;
	//예외처리 시작
	if (info.ROI.width < 0)
		info.ROI.width = src.cols - info.ROI.x;
	if (info.ROI.height < 0)
		info.ROI.height = src.rows - info.ROI.y;;

	if ((info.ROI.width + info.ROI.x) > src.cols)
		info.ROI.width = src.cols - info.ROI.x;
	if ((info.ROI.height + info.ROI.y) > src.rows)
		info.ROI.height = src.rows - info.ROI.y;

	img_gray = src(info.ROI);
	PrintFunc("processOutside src", src, 1);
	PrintFunc("processOutside ROI", img_gray, 1);
	//waitKey();

	//패턴 매칭으로 근처라도 가자
	Point2f pre_pos, find_pos;
	Rect pre_ROI;
	Mat tmpl;
	string file_name = ".\\DB\\" + LoadConfigure.ProductName + "\\KeyImage\\Modeling\\Pattern" + int2str(info.pre_process) + ".bmp";
	tmpl = imread(file_name);
	if (tmpl.empty())
	{
		file_name = ".\\DB\\" + LoadConfigure.ProductName + "\\KeyImage\\Modeling\\Pattern" + int2str(info.pre_process) + ".jpg";
		tmpl = imread(file_name);
		if (tmpl.empty())
		{
			cout << "no img : " << file_name << endl;
			waitKey();
		}
	}
	//cout << file_name << endl;
	//템플릿 없는것도 로그화 예외처리 해야함.
	Mat matching_map = patternMatchingMethod(img_gray, tmpl);

	int size = ((tmpl.cols + tmpl.rows) / 4) * 42 + 1; //force size to be odd 42
	adaptiveThreshold(matching_map, defect, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, size, info.pre_th1);
	//-128
	//rectangle(ROI_IMG, Ball_ROI, CV_RGB(255, 0, 100), 5);
	//PrintFunc("tmpl", tmpl, 2);
	PrintFunc("processOutside matching_map", matching_map, 2);
	PrintFunc("processOutside defect", defect, 2);
	//PrintFunc("defect", defect, 2);
	//PrintFunc("matching_map", matching_map, 2);
	double extend_size_x = 0;;//tmpl.cols;
	double extend_size_y = 0;;//tmpl.rows;
	ModelingInfo match_tmp;
	match_tmp.ROI.x = 0;
	match_tmp.ROI.y = 0;
	match_tmp.ROI.width = 1;
	match_tmp.ROI.height = 1;
	//while (true)
	{
		//변수화 필요
		double minval, maxval, threshold = info.pre_th2;
		Point minloc, maxloc;
		//bitwise_and();
		minMaxLoc(matching_map, &minval, &maxval, &minloc, &maxloc);
		//로그필요
		cout << "maxval : " << maxval << endl;
		if (maxval >= threshold)
		{

			//패턴매칭된 제대로된 위치를 찾기위한것
			floodFill(defect, maxloc, 0); //mark drawn blob     
			bitwise_and(defect, matching_map, matching_map);

			if (1)
			{

				//2차 볼찾기에서 유리하려면 여기서 찾은범위보다 더 넓게 찾아야한다.
				
				match_tmp.corners[1].x = maxloc.x;
				match_tmp.corners[1].y = maxloc.y;
				match_tmp.corners[2].x = maxloc.x + tmpl.cols;
				match_tmp.corners[2].y = maxloc.y + tmpl.rows;
				match_tmp.ROI.x = maxloc.x + info.ROI.x - extend_size_x;
				match_tmp.ROI.y = maxloc.y + info.ROI.y - extend_size_y;
				match_tmp.ROI.width = tmpl.cols + (extend_size_x * 2);
				match_tmp.ROI.height = tmpl.rows + (extend_size_y * 2);
				Mat tt = img_gray.clone();
				rectangle(tt, match_tmp.corners[1], match_tmp.corners[2], 155,1);
				PrintFunc("processOutside rectangle ROI", tt, 1);
				//break;
			}
		}
		else
		{
			cout << "패턴매칭 실패" << endl;
			return Point2d(0,0);
		}
	}
	img_gray = src(match_tmp.ROI);

	if (info.method == HARD_MASKING_MODE) {
		//하드마스킹은 그냥 하양것만 넘겨준다 그렇게 한 후에 나중에 하드만 모아서 빼주기 연산해줘야함
		threshold(img_gray, img_result, 0, 255, THRESH_BINARY);
	}
	else if (info.method == THRESHOLD_MODE) {
		//단순 TH
		threshold(img_gray, img_result, info.th1, 255, THRESH_BINARY);
	}
	else if (info.method == THRESHOLD_INV_MODE) {
		//단순 TH_INV
		threshold(img_gray, img_result, info.th1, 255, THRESH_BINARY_INV);
	}
	else if (info.method == ADAPTHRESHOLD_MODE) {
		adaptiveThreshold(img_gray, img_result, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, info.th1, info.th2);
	}
	else if (info.method == ADAPTHRESHOLD_INV_MODE) {
		adaptiveThreshold(img_gray, img_result, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY_INV, info.th1, info.th2);
	}
	else if (info.method == CANNY_MODE) {
		Canny(img_gray, img_result, info.th1, info.th2);
	}
	else if (info.method == OTSU_CANNY_MODE) {
		double otsu_val = threshold(img_gray, img_result, 0, 255, THRESH_BINARY | THRESH_OTSU);
		Canny(img_gray, img_result, otsu_val*info.th1, otsu_val*info.th2);
	}

	if (info.thickness >= 1){

		rectangle(img_result, Rect(0, 0, info.ROI.width, info.ROI.height), 255, info.thickness);
		floodFill(img_result, Point(1,1), 0); //mark drawn blob  
	}
	PrintFunc("processOutside img_result", img_result, 1);
	int label_num = 2;
	Mat img_labels, stats, centroids;
	int numOfLables = connectedComponentsWithStats(img_result, img_labels,
		stats, centroids, 8, CV_32S);

	int bottom = img_result.rows;
	int left, top, height, width;
	Rect tmp;
	for (int j = 1; j < numOfLables; j++) {
		int area = stats.at<int>(j, CC_STAT_AREA);
		left = stats.at<int>(j, CC_STAT_LEFT);
		top = stats.at<int>(j, CC_STAT_TOP);
		height = stats.at<int>(j, CC_STAT_HEIGHT);
		width = stats.at<int>(j, CC_STAT_WIDTH);
		//가로 길이가 가장 비슷한것만 골라내자
		if (50 < area) {
			if (height < width)	{
				if (bottom > top) {
					bottom = top;

					tmp.x = left;
					tmp.y = top;
					tmp.height = height;
					tmp.width = width;
					label_num = j;
					cout << area << endl;
				}
			}
		}
	}
	Mat S;
	inRange(img_labels, label_num, label_num, img_result);
	//PrintFunc("S", img_result, 1);
	
	circle(img_result, Point2d(match_tmp.ROI.x + match_tmp.ROI.width / 2, match_tmp.ROI.height/2 + match_tmp.ROI.y), 15, 125, 3);
	PrintFunc("processOutside S", img_result, 1);
	return Point2d(match_tmp.ROI.x + match_tmp.ROI.width / 2 + info.ROI.x, match_tmp.ROI.y+ match_tmp.ROI.height/2 + info.ROI.y);
}
bool Modeling::MeasureDist()
{
	destroyAllWindows();
	//출처: http://terrorjang.tistory.com/85 [개발자 공간]
	string FolderPath = LoadConfigure.input_directory;
	Mat test, ROI_IMG, th_img, canny_img, defect,src;
	
	for (int i = 0; i < Inside_List.size(); i++)
	{
		cout <<"Inside_List.location : "<< Inside_List[i].location << endl;
		if (Inside_List[i].location == BOTTOM)
			Inside_Bottom = Inside_List[i];
		if (Inside_List[i].location == TOP)
			Inside_Top = Inside_List[i];
	}
	for (int i = 0; i < Outside_List.size(); i++)
	{
		cout << "Outside_List.location : " << Outside_List[i].location << endl;
		if (Outside_List[i].location == BOTTOM)
			Outside_Bottom = Outside_List[i];
		if (Outside_List[i].location == TOP)
			Outside_Top = Outside_List[i];
	}
	ModelingInfo temp;
	for (int loc = 0; loc < matching_info.size()-1; loc++) {
		fprintf(fp_csv, "%d 열 ,", loc+1);
		for (int i = 0; i < matching_info[loc].size(); i++) {
			if (matching_info[loc][i].location == TOP)
				temp = Outside_Top;
			else
				temp = Outside_Bottom;
			test = ParseImage(temp.img_num, temp.focus_num, GRAY);
			cout << matching_info[loc][i].ROI << endl;
			src = test(matching_info[loc][i].ROI);

			//src = deskew(src, main_rad);
			Point2d Outer_point = processOutside(src, temp);
			
			if (matching_info[loc][i].location == TOP)
				temp = Inside_Top;
			else
				temp = Inside_Bottom;
			test = ParseImage(temp.img_num, temp.focus_num, GRAY);
			src = test(matching_info[loc][i].ROI);		

			//src = deskew(src, main_rad);
			Point2d inner_point = processInside(src, temp);
			//circle(src, inner_point, 15, 125, 3);
			PrintFunc("Inside result", src, 1);

			//double otsu_val = threshold(src, test, 0, 255, THRESH_BINARY | THRESH_OTSU);
			//Canny(src, test, otsu_val*0.3, otsu_val * 0.5);
			//PrintFunc("Inside otsu_val*0.3", test, 1);
			//Canny(src, test, otsu_val*0.5, otsu_val * 0.7);
			//PrintFunc("Inside otsu_val*0.5", test, 1);
			//Canny(src, test, otsu_val*0.7, otsu_val * 1);
			//PrintFunc("Inside otsu_val*0.7", test, 1);
			//
			//waitKey();
			
			circle(src, inner_point, 15, 125, 3);
			circle(src, Outer_point, 15, 125, 3);
			Point2d pos1, pos2, tmpl;
			tmpl.x = matching_info[loc][i].ROI.x;
			tmpl.y = matching_info[loc][i].ROI.y;
			pos1 = inner_point + tmpl;
			pos2 = Outer_point + tmpl;
			//다 모아서 해보자
			matching_info[loc][i].corners[0] = pos1;
			matching_info[loc][i].corners[1] = pos2;
			pos1.x = tmpl.x + matching_info[loc][i].ROI.width / 2;
			pos2.x = tmpl.x + matching_info[loc][i].ROI.width / 2;
			Scalar Pass(180, 50, 50);
			Scalar NG(0, 50, 180);
			Scalar font_color;
			char text[100];
			double font_size = Modeling_src.cols* 0.0003;// 1.0;
			double distance = PosDistance(pos1, pos2);
			distance *= 6.1;//2.25;
			std::sprintf(text, "%2.1fum", distance);
			Point2f center = PosCenter(pos1, pos2);
			if (220 < distance && distance < 280)
				font_color = Pass;
			else
				font_color = NG;
			cv::putText(Modeling_src, text, center + Point2f(10, 0), HersheyFonts::FONT_HERSHEY_PLAIN, font_size, Scalar(180, 180, 180) , 1.5 * font_size, LINE_AA);
			cv::putText(Modeling_src, text, center + Point2f(10, 0), HersheyFonts::FONT_HERSHEY_PLAIN, font_size, font_color, 1 * font_size, LINE_AA);
			rectangle(Modeling_src, matching_info[loc][i].ROI, font_color, 5);
			line(Modeling_src, pos1, pos2, font_color, 1);
			fprintf(fp_csv, "%lf,", distance);
			PrintFunc("Outside result", src, 1);
			waitKey();
		}
		fprintf(fp_csv, "\n");
	}

	fprintf(fp_csv, "\n");

	/*//피팅된 선 찾아보기
	for (int loc = 0; loc < matching_info.size() - 1; loc++) {
		vector<Point2f> in_point(matching_info[loc].size());
		vector<Point2f> out_point(matching_info[loc].size());
		for (int i = 0; i < matching_info[loc].size(); i++) {
			in_point[i] = matching_info[loc][i].corners[0];
			out_point[i] = matching_info[loc][i].corners[1];
		}
		cv::Vec4f line;
		cv::fitLine(cv::Mat(in_point), line,
			CV_DIST_L2, // 거리 유형 CV_DIST_HUBER CV_DIST_L2
			0,   // L2 거리를 사용하지 않음
			0.001, 0.001); // 정확도
		int vec_line = 2000;
		int x0 = line[2]; // 선에 놓은 한 점
		int y0 = line[3];
		int x1 = x0 - vec_line * line[0]; // 200 길이를 갖는 벡터 추가
		int y1 = y0 - vec_line * line[1]; // 단위 벡터 사용
		cv::line(Modeling_src, cv::Point(x0, y0), cv::Point(x1, y1), Scalar(180, 180, 180), 1);
		x1 = x0 + vec_line * line[0]; // 200 길이를 갖는 벡터 추가
		y1 = y0 + vec_line * line[1]; // 단위 벡터 사용
		cv::circle(Modeling_src, cv::Point(x0, y0), 15, Scalar(180, 180, 180), 1);
		cv::line(Modeling_src, cv::Point(x0, y0), cv::Point(x1, y1), Scalar(180, 180, 180), 1);
		cv::fitLine(cv::Mat(out_point), line,
			CV_DIST_L2, // 거리 유형 CV_DIST_HUBER CV_DIST_L2
			0,   // L2 거리를 사용하지 않음
			0.001, 0.001); // 정확도

		x0 = line[2]; // 선에 놓은 한 점
		y0 = line[3];
		x1 = x0 - vec_line * line[0]; // 200 길이를 갖는 벡터 추가
		y1 = y0 - vec_line * line[1]; // 단위 벡터 사용
		cv::line(Modeling_src, cv::Point(x0, y0), cv::Point(x1, y1), Scalar(180, 180, 180), 1);
		x1 = x0 + vec_line * line[0]; // 200 길이를 갖는 벡터 추가
		y1 = y0 + vec_line * line[1]; // 단위 벡터 사용
		cv::circle(Modeling_src, cv::Point(x0, y0), 15, Scalar(180, 180, 180), 1);
		cv::line(Modeling_src, cv::Point(x0, y0), cv::Point(x1, y1), Scalar(180, 180, 180), 1);
		for (int i = 0; i < matching_info[loc].size(); i++) {
			cv::circle(Modeling_src, in_point[i], 15, Scalar(0, 0, 255), 1);
			cv::circle(Modeling_src, out_point[i], 15, Scalar(0, 0, 255), 1);
			cv::circle(Modeling_src, in_point[i], 1, Scalar(0, 0, 0), 1);
			cv::circle(Modeling_src, out_point[i], 1, Scalar(0, 0, 0), 1);
		}
	}
	*/


	FolderPath = LoadConfigure.input_directory;
	//FolderPath += "\\result_modeling." + LoadConfigure.OutputFileType;
	FolderPath += "\\result_" + int2str(temp.img_num) + "_" + int2str(temp.focus_num) + "." + LoadConfigure.OutputFileType;
	imwrite(FolderPath, Modeling_src);
	PrintFunc("Modeling_src", Modeling_src, 7);
	//waitKey();

	clock_result = (double)(clock() - loadClock) / CLOCKS_PER_SEC;
	fprintf(fp_log, "\n>>>>> Modeling Result Elapsed Time is %lf Seconds\n", clock_result);
	cout << "Modeling Elapsed Time : " << clock_result << endl;
	return true;
}

bool Modeling::ChipModeling()
{
	//fopen_s(&fp_log, log_fname.c_str(), "a");
	fprintf(fp_log, "\n>>>>>>>>>> Start ChipModeling\n");
	Mat test, ROI_IMG, th_img, canny_img, defect, src, matching_map;
	ModelingInfo tmp;
	vector<Point> merged_contours;
	for (int i = 0; i < Chip_List.size(); i++)
	{
		tmp = Chip_List[i];
		src = ParseImage(tmp.img_num, tmp.focus_num, GRAY);
		if (src.empty())
		{
			fprintf(fp_log, "No ChipModeling Image I%d-%d....\n", tmp.img_num, tmp.focus_num);
			//fclose(fp_log);
			return false;
		}
		else
		{
			fprintf(fp_log, "Load I%d-%d Image done...\n", tmp.img_num, tmp.focus_num);
		}
		tmp.ROI.x += Center.x;
		tmp.ROI.y += Center.y;
		//예외처리 시작
		if (tmp.ROI.width < 0)
			tmp.ROI.width = src.cols - tmp.ROI.x;
		if (tmp.ROI.height < 0)
			tmp.ROI.height = src.rows - tmp.ROI.y;;

		if ((tmp.ROI.width + tmp.ROI.x) > src.cols)
			tmp.ROI.width = src.cols - tmp.ROI.x;
		if ((tmp.ROI.height + tmp.ROI.y) > src.rows)
			tmp.ROI.height = src.rows - tmp.ROI.y;

		if (src.channels() == 3)
			cvtColor(src, src, CV_BGR2GRAY);
		else
			src = src.clone();
		Chip_List[i] = tmp;
		ROI_IMG = src(tmp.ROI);

		// 방법 1
		// 단순 이진화 하고 가장 4각형과 가까운곳을 찾는다.

		cout << "Modeling_src.size() : " << Modeling_src.size() << endl;

		fprintf(fp_log, "TM Start\n");
		Point2f pre_pos, find_pos;
		Rect pre_ROI;
		Mat tmpl;
		string file_name = ".\\DB\\" + LoadConfigure.ProductName + "\\KeyImage\\Modeling\\Pattern" + int2str(tmp.method) + ".bmp";
		tmpl = imread(file_name);
		if (tmpl.empty())
		{
			file_name = ".\\DB\\" + LoadConfigure.ProductName + "\\KeyImage\\Modeling\\Pattern" + int2str(tmp.method) + ".jpg";
			tmpl = imread(file_name);
			if (tmpl.empty())
			{
			}
		}
		//cout << file_name << endl;
		destroyAllWindows();
		//PrintFunc("ROI_IMG", ROI_IMG, 2);
		//PrintFunc("tmpl", tmpl, 2);
		//waitKey();
		cout << ROI_IMG.size() << endl;
		//템플릿 없는것도 로그화 예외처리 해야함.
		matching_map = patternMatchingMethod(ROI_IMG, tmpl);

		int size = ((tmpl.cols + tmpl.rows) / 4) * 42 + 1; //force size to be odd 42
		adaptiveThreshold(matching_map, defect, 255, ADAPTIVE_THRESH_MEAN_C, THRESH_BINARY, size, tmp.th1);
		//-128
		//rectangle(ROI_IMG, Ball_ROI, CV_RGB(255, 0, 100), 5);
		//PrintFunc("tmpl", tmpl, 2);
		PrintFunc("matching_map", matching_map, 2);
		PrintFunc("defect", defect, 2);											
		//PrintFunc("defect", defect, 2);
		//PrintFunc("matching_map", matching_map, 2);
		double extend_size_x = 0;;//tmpl.cols;
		double extend_size_y = 0;;//tmpl.rows;
		//waitKey();
		while (true)
		{
			//변수화 필요
			double minval, maxval, threshold = tmp.th2;
			Point minloc, maxloc;
			//bitwise_and();
			minMaxLoc(matching_map, &minval, &maxval, &minloc, &maxloc);
			//로그필요
			cout << "maxval : " << maxval << endl;
			if (maxval >= threshold)
			{

				//패턴매칭된 제대로된 위치를 찾기위한것
				floodFill(defect, maxloc, 0); //mark drawn blob     
				bitwise_and(defect, matching_map, matching_map);
				//mark drawn blob 
				////패턴매칭된 제대로된 위치를 찾기위한것

				//Mat rorororo = ROI_IMG(Rect(maxloc.x, maxloc.y, tmpl.cols, tmpl.rows)).clone();
				//rectangle(ROI_IMG, Rect(maxloc.x, maxloc.y, tmpl.cols, tmpl.rows), 125, 15);
				//PrintFunc("ROI_IMG", ROI_IMG, 2);
				//PrintFunc("matching_map", matching_map, 2);
				//PrintFunc("defect", defect, 2);
				//PrintFunc("ROI", rorororo, 1);
				//waitKey();

				//181207 볼이 있는지 없는지는 뭘로 확인하지?
				//여기서 볼이 있을때만 저장하자
				//볼을 찾는김에 볼사이즈도?
				//볼사이즈는 다른곳에다 하자^
				//여기다 하려고 했는데 상위 폴더에서 하자
				if (1)
				{

					//2차 볼찾기에서 유리하려면 여기서 찾은범위보다 더 넓게 찾아야한다.
					ModelingInfo match_tmp;
					match_tmp.corners[1].x = maxloc.x + tmp.ROI.x;
					match_tmp.corners[1].y = maxloc.y + tmp.ROI.y;
					match_tmp.corners[2].x = tmpl.cols;
					match_tmp.corners[2].y = tmpl.rows;
					match_tmp.ROI.x = maxloc.x +tmp.ROI.x - extend_size_x;
					match_tmp.ROI.y = maxloc.y +tmp.ROI.y - extend_size_y;
					match_tmp.ROI.width = tmpl.cols + (extend_size_x * 2);
					match_tmp.ROI.height = tmpl.rows + (extend_size_y * 2);
					//match_tmp.corners[0].x = match_tmp.ROI.x + (double)match_tmp.ROI.width / (double)2;
					//match_tmp.corners[0].y = match_tmp.ROI.y + (double)match_tmp.ROI.height / (double)2;

					match_tmp.location = tmp.location;
					//매칭된 모든 포지션은 여기에 저장됨
					matching_info[tmp.location].push_back(match_tmp);
				}
			}
			else
				break;
		}

	}

	//소팅해주기
	sort(matching_info[0].begin(), matching_info[0].end(), CompPOS2fX);
	sort(matching_info[1].begin(), matching_info[1].end(), CompPOS2fX);

	clock_result = (double)(clock() - loadClock) / CLOCKS_PER_SEC;
	fprintf(fp_log, "\n>>>>> Chip Elapsed Time is %lf Seconds\n", clock_result);
	cout << "Chip Elapsed Time : " << clock_result << endl;
	return true;
	//waitKey();
}



bool Modeling::ModelingResultFirst(int modeling_result)
{

	
	//출처: http://terrorjang.tistory.com/85 [개발자 공간]
	string FolderPath = LoadConfigure.input_directory;
	char fname[300];
	sprintf(fname, "%s/Result_Modeling1.txt", FolderPath.c_str());
	cout << "fname : " << fname << endl;
	fopen_s(&fp_result, fname, "w");
	if (!fp_result) {
		//waitKey();
		exit(-1);
	}

	Mat test, ROI_IMG, th_img, canny_img, defect;
	ModelingInfo temp = Outside_List.front();
	Modeling_src = ParseImage(temp.img_num, temp.focus_num, ORIGINAL);
	ROI_IMG = Modeling_src.clone();
	cout << "Modeling_src.size() : " << Modeling_src.size() << endl;
	if (Modeling_src.empty())
	{
		fprintf(fp_log, "No ModelingResultFirst Image I%d-%d....\n", temp.img_num, temp.focus_num);
		//fclose(fp_log);
		return false;
	}
	else
	{
		fprintf(fp_log, "Load I%d-%d Image done...\n", temp.img_num, temp.focus_num);
	}


	for (int loc = 0; loc < matching_info.size(); loc++) {
		rectangle(Modeling_src, Chip_List[loc].ROI, CV_RGB(50, 50, 55), 1);
		for (int i = 0; i < matching_info[loc].size(); i++) {
			//rectangle(Modeling_src, matching_info[loc][i].ROI, CV_RGB(0,0,255), 5);
			char text[100];
			double font_size = 1.0;
			std::sprintf(text, "%d-%d",loc+1, i+1);
			Point2f center;
			center.x = matching_info[loc][i].ROI.x;
			center.y = matching_info[loc][i].ROI.y;
			cv::putText(Modeling_src, text, center + Point2f(0, -10 * font_size), HersheyFonts::FONT_HERSHEY_PLAIN, font_size, Scalar(180, 180, 180), 1.5 * font_size, LINE_AA);
			cv::putText(Modeling_src, text, center + Point2f(0, -10 * font_size), HersheyFonts::FONT_HERSHEY_PLAIN, font_size, Scalar(0, 50, 180), 1 * font_size, LINE_AA);		
			defect = ROI_IMG(matching_info[loc][i].ROI).clone();
			FolderPath = LoadConfigure.input_directory;
			//FolderPath += "\\result_modeling." + LoadConfigure.OutputFileType;
			FolderPath += "\\crop\\"+ int2str(loc) +"_"+ int2str(i) + "." + LoadConfigure.OutputFileType;
			imwrite(FolderPath, defect);
			//PrintFunc("Modeling_src", Modeling_src, 7);
			//waitKey();
		}
	}


	FolderPath = LoadConfigure.input_directory;
	//FolderPath += "\\result_modeling." + LoadConfigure.OutputFileType;
	FolderPath += "\\result_" + int2str(temp.img_num) + "_" + int2str(temp.focus_num) +"."+ LoadConfigure.OutputFileType;
	imwrite(FolderPath, Modeling_src);
	PrintFunc("Modeling_src", Modeling_src, 7);
	//waitKey();

	clock_result = (double)(clock() - loadClock) / CLOCKS_PER_SEC;
	fprintf(fp_log, "\n>>>>> Modeling Result Elapsed Time is %lf Seconds\n", clock_result);
	cout << "Modeling Elapsed Time : " << clock_result << endl;
	return true;
}

Mat Modeling::patternMatchingMethod(Mat src, Mat tmpl)
{
	//패턴매칭이냐 아님 특징점 매칭이냐
	//단순 탬플릿매칭 쓰자
	Mat img_gray, tpl_gray;

	//////////////////////////////////////////////////////
	if (src.channels() == 3)
		cvtColor(src, img_gray, CV_BGR2GRAY);
	else
		img_gray = src.clone();
	/////////////////////////////////////////////////////
	if (tmpl.channels() == 3)
		cvtColor(tmpl, tpl_gray, CV_BGR2GRAY);
	else
		tpl_gray = tmpl.clone();
	Mat res_32f(img_gray.rows - tpl_gray.rows + 1, img_gray.cols - tpl_gray.cols + 1, CV_32F);
	matchTemplate(img_gray, tpl_gray, res_32f, CV_TM_CCOEFF_NORMED);
	Mat res, res1;
	res_32f.convertTo(res, CV_8U, 255.0);
	double minval, maxval;
	Point minloc, maxloc;
	minMaxLoc(res, &minval, &maxval, &minloc, &maxloc);
	Point2f result_pos;
	result_pos.x = maxloc.x + tmpl.cols/2;
	result_pos.y = maxloc.y + tmpl.rows/2;
	return res;
}

bool CompPOS2fX(ModelingInfo a, ModelingInfo b)
{
	return (a.ROI.x > b.ROI.x);
}
bool CompPOSX(Point a, Point b)
{
	return (a.x < b.x);
}