#include "DefectFinder.h"
using namespace cv;


//Load 변수들
struct ImagePack {
	Mat Original;
	Mat R, G, B;
	string ImageName;
};
vector <ImagePack> Img;
vector <Mat> OriginalImg;
vector <Mat> templSet;

vector <Rect> DefectRoi;
vector <int> DefectSize;
vector <Mat> BGR;
vector <string> ImageNameList;
vector <string> tmplNameList;
//INI와 Load 변수 받는 부분
FILE	*fp_in, *fp_log, *fp_result, *fp_csv;
char input_directory[300], input_name[300], result_name[500], ChkString[300], TextString[500], Version[100], ImageName[500], tmplName[500], tmpl_directory[500];
char ProductName[300], InputFileType[10], OutputFileType[10];
char fname[300], output_name[300], DummyString[300], templ_path[300], boxinfo_path[300];
int ImageSize;
string FolderPath, FN;
Mat loadImg, resizeImg;
Mat Max_img, Min_img, tmp, thersh, White_Defect, Black_Defect;
RotatedRect ROIRect;
Rect ROIBox;
int ZoomFactor;


bool Load_argv(int argc, char **argv)
{
	//==========< General Control >============//
	fopen_s(&fp_in, "input_file_list.ini", "r");
	if (!fp_in) exit(-10);

	fscanf(fp_in, "%s", ChkString);
	if (strcmp(ChkString, "[InformationBEGIN]")) exit(-11);
	fscanf(fp_in, "%s", ChkString);
	if (strcmp(ChkString, "VERSION"))			exit(-12);
	fscanf(fp_in, "%s", ChkString);
	if (strcmp(ChkString, cVersion))				exit(-13);
	fscanf(fp_in, "%s", ChkString);				// [InformationEnd]

	fscanf(fp_in, "%s", ChkString);
	if (!strcmp(ChkString, "[AlgorithmSimulationBEGIN]")) {
		while (1) {
			if (fscanf(fp_in, "%s", ChkString) == EOF) break;
			if (!strcmp(ChkString, "[AlgorithmSimulationEND]")) break;
			if (!strcmp(ChkString, "##")) {
				while (1) if (getc(fp_in) == '\n') break;
			}

			if (!strcmp(ChkString, "ProductName"))		 fscanf(fp_in, "%s\n", ProductName);
			if (!strcmp(ChkString, "ImageFileType"))		 fscanf(fp_in, "%s %s\n", InputFileType, OutputFileType);
			if (!strcmp(ChkString, "ImageFolderPath"))	 fscanf(fp_in, "%s\n", input_directory);
			if (!strcmp(ChkString, "ImageName")) { fscanf(fp_in, "%s\n", ImageName); ImageNameList.push_back(ImageName); }
			if (!strcmp(ChkString, "ImageSize"))	 fscanf(fp_in, "%d\n", &ImageSize);
			if (!strcmp(ChkString, "tmplName")) { fscanf(fp_in, "%s\n", tmplName); tmplNameList.push_back(tmplName); }

		}
	}

	fscanf(fp_in, "%s", ChkString);
	if (!strcmp(ChkString, "[ConfigurationBEGIN]")) {
		while (1) {
			if (fscanf(fp_in, "%s", ChkString) == EOF) break;
			if (!strcmp(ChkString, "[ConfigurationEND]")) break;
			if (!strcmp(ChkString, "##")) {
				while (1) if (getc(fp_in) == '\n') break;
			}


		}
	}
	fclose(fp_in);


	//==========< Configuration by Lens Type >============//

	if (argc > 1) {
		strcpy(ProductName, argv[1]);
		strcpy(InputFileType, argv[4]);
		strcpy(OutputFileType, argv[5]);
		strcpy(input_directory, argv[6]);
	}

	return true;
}

bool Load_INI()
{

	//여기서 다양한 Dentium_FS_Extra.ini만들어서쓰자
	fopen_s(&fp_in, "Dentium_FS_Extra.ini", "r");
	if (fp_in) {

		fscanf(fp_in, "%s", ChkString);
		if (!strcmp(ChkString, "[ModelingBEGIN]")) {
			while (1) {
				if (fscanf(fp_in, "%s", ChkString) == EOF) break;
				if (!strcmp(ChkString, "[ModelingEND]")) break;
				if (!strcmp(ChkString, "##")) {
					while (1) if (getc(fp_in) == '\n') break;
				}
				string temp;
				//if( !strcmp( ChkString, "FirstModelingImgName" ) ){					fscanf( fp_in, "%s\n", TextString); FirstROIImgName.push_back(TextString);}
			}
		}
		fclose(fp_in);
	}

	return true;
}
bool Load_Img()
{
	cout << input_directory << endl;
	//여기서 ImgName을 읽은것만큼 이미지 가져와서 프로세싱 할꺼임
	ImageNameList;


	//폴더내에 bmp나 jpg 싹다 읽어들이자
	_finddata_t fd;
	long handle;
	int result = 1;
	FolderPath = input_directory;
	FolderPath += "\\*.*";
	handle = _findfirst(FolderPath.c_str(), &fd);


	ImageNameList.clear();
	if (handle == -1)
	{
		cout << "no files check plz" << endl;
		exit(0);
	}

	while (result != -1)
	{
		result = _findnext(handle, &fd);
		string base = fd.name;
		if ((base.find(".BMP") != string::npos) || (base.find(".JPG") != string::npos) || (base.find(".PNG") != string::npos) || (base.find(".bmp") != string::npos) || (base.find(".jpg") != string::npos) || (base.find(".png") != string::npos)) { // "world!"라는 문자열을 찾았을 때의 동작 
			ImageNameList.push_back(fd.name);
			cout << "File : " << fd.name << endl;
		}

	}

	_findclose(handle);

	for (size_t i = 0; i < ImageNameList.size(); i++)
	{
		FolderPath = input_directory;
		//if (!strcmp(InputFileType, "bmp") || !strcmp(InputFileType, "jpg") || !strcmp(InputFileType, "png"))
		//	FN = FolderPath + "\\" + ImageNameList[i] + "." + InputFileType;
		//else
		FN = FolderPath + "\\" + ImageNameList[i];
		Mat loadImg = imread(FN, 1);
		if (loadImg.empty())
		{
			cout << "no img : " << FN << endl;
			continue;
		}
		else
		{
			cout << "read img : " << FN << endl;
		}
		//resize( loadImg, loadImg, cv::Size( loadImg.cols/2,loadImg.rows/2 ), 0, 0, CV_INTER_LINEAR );
		//split(loadImg,BGR);
		Mat image_r(loadImg.rows, loadImg.cols, CV_8UC1);
		Mat image_g(loadImg.rows, loadImg.cols, CV_8UC1);
		Mat image_b(loadImg.rows, loadImg.cols, CV_8UC1);
		Mat out[] = { image_b, image_g, image_r };
		int from_to[] = { 0,0, 1,1, 2,2 };
		mixChannels(&loadImg, 1, out, 3, from_to, 3);
		OriginalImg.push_back(loadImg);
		ImagePack temp;
		temp.Original = loadImg;
		temp.B = image_b;
		temp.G = image_g;
		temp.R = image_r;
		string ImgName = ImageNameList[i];
		//if (ImageNameList[i].find(".BMP") != string::npos)
		{
			std::string::size_type dot = ImageNameList[i].find_last_of('.');
			ImgName = ImageNameList[i].substr(0, dot);
			cout << ImgName << endl;
		}
		temp.ImageName = ImgName;
		Img.push_back(temp);
	}

	//waitKey();
	return true;
}
bool Load_all(int argc, char **argv)
{

	if (!Load_argv(argc, argv))
		return false;
	if (!Load_INI())
		return false;
	if (!Load_Img())
		return false;

	return true;
}

#define FOCAL 4
Point p1, p2;
vector <Rect> ROI;
vector <Rect> templROI;
int cnt[14] = { 0, };
//마우스 이벤트 함수
void onMouseEvent(int event, int x, int y, int flags, void* dstImage)
{
	Mat mouseImage = *(Mat*)dstImage;
	Mat draw = mouseImage.clone();
	switch (event)
	{
	case CV_EVENT_MOUSEMOVE:
		if (flags & CV_EVENT_LBUTTONDOWN)
		{
			p2.x = x*FOCAL;
			p2.y = y*FOCAL;
			rectangle(draw, p1, p2, 0, 3); //mark drawn blob 
										   //rectangle(draw,Point(x*3,y*3),10,Scalar::all(255),-1);
			cout << "x : " << x*FOCAL << endl;
			cout << "y : " << y*FOCAL << endl;
		}
		break;
	case CV_EVENT_LBUTTONDOWN:
		p1.x = x*FOCAL;
		p1.y = y*FOCAL;
		break;
	case CV_EVENT_LBUTTONUP:
		p2.x = x*FOCAL;
		p2.y = y*FOCAL;
		rectangle(draw, p1, p2, 0, 3); //mark drawn blob 
		break;
	}

	rectangle(draw, p1, p2, 0, 3); //mark drawn blob 

	PrintFunc(useMouse, draw, FOCAL);
	//imshow(useMouse,mouseImage);
}



#define MAKE_ROI
#ifdef MAKE_ROI

int main(int argc, char** argv)
{
	//Mat Gradation = Mat::zeros(Size(450, 450), CV_8UC1);
	//for (int i = 25; i > 0; i--)
	//{
	//	rectangle(Gradation,Point(0,0),Point(200 + i * 10, 200 + i * 10),i*10,-1);
	//	cout << 200 + i * 10 << endl;
	//	
	//}
	//imwrite("Gradation.bmp", Gradation);
	//PrintFunc("Gradation", Gradation, 1);
	//waitKey();
	// Load image
	if (!Load_all(argc, argv))
		exit(-1);


	//이미지 가져온따로 잘라서 저장해야겠다... 제대로 나눴지도 확인해야 하고
	//나중에 멋있게 TXT로 학습할 이미지랑 위치랑 class 적어서 두는걸로
	int train_size = ImageSize;
	//요고 간격은 걍 비율로 하자 ㅇㅇ 비율로
	double stride = 3.5;

	//CSV읽어서 일단 저장하자
	struct infomation {
		string name;
		string classification;
	};
	struct piece_info {
		Mat image;
		double std_val;
	};
	vector <infomation> make_data;
	FolderPath = input_directory;
	FolderPath += "\\MakeDATA.csv";
	ifstream ifile;

	char line[200]; // 한 줄씩 읽어서 임시로 저장할 공간

	ifile.open(FolderPath.c_str());  // 파일 열기

	if (ifile.is_open()) {
		while (ifile.getline(TextString, sizeof(TextString))) // 한 줄씩 읽어 처리를 시작한다.
		{
			cout << TextString << endl; // 내용 출력
			infomation temp;
			double DV, EV, SV, RV, BF, EV2, SV2;
			int name;
			string TS = TextString;
			std::string::size_type dot = TS.find_last_of(',');
			temp.name = TS.substr(0, dot);
			temp.classification = TS.substr(dot+1, TS.length());
			make_data.push_back(temp);
		}


		

		ifile.close();
	}


	for (size_t i = 0; i < Img.size(); i++) {
		//
		Mat img_gray;
		if (Img[i].Original.channels() != 1)
			cvtColor(Img[i].Original, img_gray, CV_BGR2GRAY);
		else
			img_gray = Img[i].Original.clone();

		//이녀석 클래스 미리 분류하기
		string class_name = "none";
		string base = Img[i].ImageName;

		FolderPath = input_directory;
		FolderPath += "\\" + class_name;
		_mkdir(FolderPath.c_str());

		FolderPath = input_directory;
		FolderPath += "\\" + class_name;
		FolderPath += "\\train";
		_mkdir(FolderPath.c_str());

		FolderPath = input_directory;
		FolderPath += "\\" + class_name;
		FolderPath += "\\val";
		_mkdir(FolderPath.c_str());

		FolderPath = input_directory;
		FolderPath += "\\" + class_name;
		FolderPath += "\\test";
		_mkdir(FolderPath.c_str());

		for (int j = 0; j < make_data.size(); j++)
		{			
			if ((base.compare(make_data[j].name) == 0))
			{
				class_name = make_data[j].classification;
				break;
			}
		}
		cout << class_name << endl;

		FolderPath = input_directory;
		FolderPath += "\\" + class_name;
		int nResult = _access(FolderPath.c_str(),0);

		if (nResult == 0)
		{
			//폴더가 있음요
			//cout << "폴더 있거든?!!?!" << endl;
			//_mkdir(FolderPath.c_str());
			//FolderPath += "\\none";
			//_mkdir(FolderPath.c_str());
		}
		else if(nResult == -1)
		{
			//폴더가 없음요
			//cout << "폴더 없거든?!!?!" << endl;
			_mkdir(FolderPath.c_str());

			FolderPath = input_directory;
			FolderPath += "\\" + class_name;
			FolderPath += "\\train";
			_mkdir(FolderPath.c_str());

			FolderPath = input_directory;
			FolderPath += "\\" + class_name;
			FolderPath += "\\val";
			_mkdir(FolderPath.c_str());

			FolderPath = input_directory;
			FolderPath += "\\" + class_name;
			FolderPath += "\\test";
			_mkdir(FolderPath.c_str());
		}

		vector<piece_info> piece;

		Rect ROI;
		int width = img_gray.cols;
		int height = img_gray.rows;

		int w_itc, h_itc;

		int ratio = (int)((double)train_size / (double)stride);
		w_itc = (width - train_size)  / ratio + 1;
		h_itc = (height - train_size) / ratio + 1;

		bool tensorflow_image = true;
		if (1)//여긴 그냥 전체 이미지 학습하기 위한 부분
		{
			w_itc = 1;
			h_itc = 1;
		}

		double avg_val = avg_img_th(img_gray,1);
		double std_avg = 0;
		int num = 0;
		for (int r = 0; r < 5; r++) {
			Mat affine_img = img_gray.clone();
			Point2f center;
			center.x =(affine_img.cols / 2);
			center.y =(affine_img.rows / 2);
			Mat rot = getRotationMatrix2D(center, r*30, 1.0);
			warpAffine(img_gray, affine_img, rot, img_gray.size());
			//PrintFunc("affine_img", affine_img, 1);
			rectangle(affine_img,Rect(0,0, center.x*2, center.y*2),0,5);
			//PrintFunc("affine_imgrectangle", affine_img, 1);
			floodFill(affine_img,Point(1,1),avg_val);
			medianBlur(affine_img, affine_img, 3);
			//PrintFunc("floodFill", affine_img, 1);
			//waitKey();
			if (tensorflow_image)
			{
				int width, height;
				width = affine_img.cols;
				height = affine_img.rows;
				for (int x = 0; x < 5; x++)
				{
					switch (x)
					{
					case 0: ROI.x = 0; ROI.y = 0; ROI.width = width; ROI.height = height; break;
					case 1: ROI.x = 0; ROI.y = 0; ROI.width = 0; ROI.height = 0; break;
					case 2: ROI.x = 0; ROI.y = 0; ROI.width = 0; ROI.height = 0; break;
					case 3: ROI.x = 0; ROI.y = 0; ROI.width = 0; ROI.height = 0; break;
					case 4: ROI.x = 0; ROI.y = 0; ROI.width = 0; ROI.height = 0; break;
					default: break;
					}
				}
			}
			else {
				for (int y = 0; y < h_itc; y++)
				{
					for (int x = 0; x < w_itc; x++)
					{
						ROI.x = x * ratio;
						ROI.y = y * ratio;
						ROI.width = train_size;
						ROI.height = train_size;
						piece_info temp;
						temp.image = affine_img(ROI).clone();
						temp.std_val = img_std(temp.image);
						piece.push_back(temp);
						std_avg += temp.std_val;
						//cout << num++ << endl;
					}
				}
			}
		}
		std_avg /= piece.size(); 
		//

		int sets = 0;
		int none_set = 0;
		for (int ii = 0; ii < piece.size(); ii++)
		{

			FolderPath = input_directory;
			FolderPath += "\\" + class_name;
			Mat test = piece[ii].image.clone();
			//cout <<"avg_img :"<< avg_img(piece[ii]) << endl;
			//cout <<"img_std :"<< piece[ii].std_val << endl;
			if (std_avg > piece[ii].std_val)
			{
				cv::line(test, Point(0, 0), Point(test.cols, test.rows), 255, 6);
				cv::line(test, Point(test.cols, 0), Point(0, test.rows), 255, 6);
				FolderPath = input_directory;
				string sets_name;
				switch (none_set)
				{
				case 0:
				case 1:
				case 2:
				case 3: sets_name = "train"; break;
				case 4: sets_name = "val"; break;
				case 5:
				case 6: sets_name = "test"; break;
				default :sets_name = "train"; break;
				}
				none_set++;
				FolderPath += "\\none\\" + sets_name + "\\" + base + "-" + intToString(ii) + "." + OutputFileType;
				none_set %= 7;
			}
			else{
				string sets_name;
				switch (sets)
				{
				case 0:
				case 1:
				case 2:
				case 3: sets_name = "train"; break;
				case 4: sets_name = "val"; break;
				case 5:
				case 6: sets_name = "test"; break;
				default:sets_name = "train"; break;
				}
				sets++;
				sets %= 7;
				FolderPath += "\\" + sets_name + "\\" + base  + "-" + intToString(ii) + "." + OutputFileType;
			}
			//cout << FolderPath << endl;
			imwrite(FolderPath, piece[ii].image);
			//PrintFunc("piece", test,1);
			//waitKey();
		}
		//mkdir();




		//PrintFunc("test", img_gray, 1);
		//waitKey();

	}
	return 0;
}


#else

#endif