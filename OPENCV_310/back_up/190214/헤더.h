
#include "Util.h"

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
int ImageSize, IsRotation;
string FolderPath, FN;
Mat loadImg, resizeImg;
Mat Max_img, Min_img, tmp, thersh, White_Defect, Black_Defect;
RotatedRect ROIRect;
Rect ROIBox;
int ZoomFactor, ResultDisplay, IsSimulator;
int job_code,index_num;

//TOF용 검사 flag
enum BIT_MASK {
	DO_MODELING		= (1 << 0),
	DO_MEASUREMENT	= (1 << 1)
};
ConfigureInfo LoadConfigure;
vector<CalibData> calib_data;

//Z축 캘리브레이션용
vector<int> adj_focus;
vector<int> set_focus;

void PrintFunc(string Fn, const Mat in, int Zoom)
{

#ifdef PRINT_DEBUG
	if (in.empty())
		return;
	Mat src, src1, src2;
	src = in.clone();
	resize(src, src1, Size(src.cols / Zoom, src.rows / Zoom));

	namedWindow(Fn, WINDOW_AUTOSIZE);
	imshow(Fn, src1);
#endif
}
bool readCellInfo(string cell_info_name, vector<int>& adj_focus, vector<int>& set_focus)
{
	adj_focus.clear();
	set_focus.clear();

	string file_name = LoadConfigure.input_directory + cell_info_name;  //"\\CellInfo_" + TID + "_WMP.1.txt";

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
					//while (1) {
					//	size_t dot = pre_line.find(",", xpos + 1);
					//	if (dot > ypos)
					//		break;
					//	if (dot == -1)
					//		adj_focus_list = pre_line.substr(xpos + 1, ypos - xpos - 1);
					//	else
					//		adj_focus_list = pre_line.substr(xpos + 1, dot - xpos - 1);
					//	cout << adj_focus_list << endl;
					//	adj_focus.push_back(stoi(adj_focus_list));
					//	xpos = dot;
					//	if (xpos == -1)
					//		break;
					//}
					stringstream tokens(adj_focus_list);
					while (getline(tokens, adj_focus_list, ','))
					{
						adj_focus.push_back(stoi(adj_focus_list));
						cout << adj_focus_list << endl;
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
					stringstream tokens(set_focus_list);
					while (getline(tokens, set_focus_list, ','))
					{
						set_focus.push_back(stoi(set_focus_list));
						cout << set_focus_list << endl;
					}
				}
			}
		}
		if (adj_focus.empty()){
			//std::copy(set_focus.begin(), set_focus.end(), adj_focus.begin());
			//벡터 복사할때 문제 생김 왜그러징??
			for (int i = 0; i < set_focus.size(); i++)
				adj_focus.push_back(set_focus[i]);
		}
		//cout << set_focus[2] << endl;
	}
	else
	{
		//
		adj_focus.clear();
		adj_focus.reserve(10);
		for (int i = 0; i < 10; i++)
			adj_focus.push_back(160);
		return false;
	}
	return true;
}
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
			if (!strcmp(ChkString, "ImageName")) { fscanf(fp_in, "%s\n", ImageName); ImageNameList.push_back(ImageName); }
			if (!strcmp(ChkString, "ImageSize"))	 fscanf(fp_in, "%d\n", &ImageSize);
			if (!strcmp(ChkString, "tmplName")) { fscanf(fp_in, "%s\n", tmplName); tmplNameList.push_back(tmplName); }
			if (!strcmp(ChkString, "job_code"))	 fscanf(fp_in, "%d\n", &job_code);
			if (!strcmp(ChkString, "ResultDisplay"))	 fscanf(fp_in, "%d\n", &ResultDisplay);
			if (!strcmp(ChkString, "IsSimulator"))	 fscanf(fp_in, "%d\n", &IsSimulator);
			if (!strcmp(ChkString, "index_num"))	 fscanf(fp_in, "%d\n", &index_num);
			if (!strcmp(ChkString, "IsRotation"))	 fscanf(fp_in, "%d\n", &IsRotation);
			if (!strcmp(ChkString, "ImageFolderPath")) {				
				fgets(DummyString, 300, fp_in); // fscanf(fp_in, "%[^\n]", input_directory);
				string line = DummyString;
				size_t colon_pos;
				//모든 공백을 읽어야 하니 앞의 공백까지 읽는 사태가 발생
				//콜론을 찾아서 콜론의 앞쪽에서 부터 읽도록 하게 만듬
				if ((colon_pos = line.find(":")) != string::npos) {
					string pre_line = line.substr(colon_pos-1, line.size() - colon_pos);
					cout << pre_line << endl;
					sprintf(input_directory, "%s", pre_line.c_str());
				}
			}

		}
	}
	cout << input_directory << endl;
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


	//==========< Configuration by SW >============//

	if (argc > 1) {
		strcpy(DummyString, argv[1]);
		job_code = stoi(DummyString);
		strcpy(DummyString, argv[2]);
		index_num = stoi(DummyString);
		strcpy(ProductName, argv[3]);
		strcpy(InputFileType, argv[4]);
		strcpy(OutputFileType, argv[5]);
		strcpy(input_directory, argv[6]);
		strcpy(DummyString, argv[7]);
		ResultDisplay = stoi(DummyString);
		strcpy(DummyString, argv[8]);
		ResultDisplay = stoi(DummyString);
		strcpy(DummyString, argv[9]);
		IsSimulator = stoi(DummyString);

		//마지막에 Defect Map 변수 0을 넣어달라 그러자
		//ResultDisplay			1
		//AlignmentDisplay		1
		//IsSimulator			1
		cout << job_code << endl;
		cout << ProductName << endl;
		cout << index_num << endl;
		cout << InputFileType << endl;
		cout << OutputFileType << endl;
		cout << input_directory << endl;
	}
	//각 객체마다 전달할 INI 정보들
	LoadConfigure.InputFileType = InputFileType;
	LoadConfigure.input_directory = input_directory;
	LoadConfigure.OutputFileType = OutputFileType;
	LoadConfigure.ProductName = ProductName;
	LoadConfigure.ResultDisplay = ResultDisplay;
	LoadConfigure.IsSimulator = IsSimulator;
	LoadConfigure.index_num = index_num;
	LoadConfigure.job_code = job_code;
	return true;
}
void readCalibData(vector<CalibData>& calib_data, string calib_csv)
{
	calib_data.clear();
	calib_data.resize(320);
	int calib_cnt = 0;
	ifstream config_file;
	config_file.open(calib_csv);
	if (config_file.is_open())
	{
		string line;
		while (getline(config_file, line))
		{
			stringstream linestream(line);

			string value;
			getline(linestream, value, ',');
			//cout << "linestream is " << value << endl;
			
			if (value != "step") {
				int num = stoi(value);
				getline(linestream, value);
				//cout << num << " is " << value << endl;
				stringstream tokens(value);
				vector <double> read_double;
				while (getline(tokens, value, ','))
				{
					read_double.push_back(stod(value));
				}
				calib_data[num].scale_x = read_double[0];
				calib_data[num].scale_y = read_double[1];
				calib_data[num].shift_x = read_double[2];
				calib_data[num].shift_y = read_double[3];
				calib_data[num].pitch_x = read_double[4];
				//calib_data_3x[num].pitch_y = read_double[0];
				//cout << num << "'s scale_x : " << calib_data_3x[num].scale_x << endl;
				//cout << num << "'s scale_y : " << calib_data_3x[num].scale_y << endl;
				//cout << num << "'s shift_x : " << calib_data_3x[num].shift_x << endl;
				//cout << num << "'s shift_y : " << calib_data_3x[num].shift_y << endl;
				//cout << num << "'s pitch_x : " << calib_data_3x[num].pitch_x << endl;
			}
		}
	}
	else
	{
		for (int num = 0; num < 320; num++)
		{
			calib_data[num].scale_x = 1;
			calib_data[num].scale_y = 1;
			calib_data[num].shift_x = 1;
			calib_data[num].shift_y = 1;
			calib_data[num].pitch_x = 1.500450767;
		}
	}
}

bool Load_Calib()
{
	//여기서 다양한 Dentium_FS_Extra.ini만들어서쓰자
	//fopen_s(&fp_in, "tof30_calib.csv", "r");


	String s1 = LoadConfigure.input_directory;

	//\\ 제일 마지막에 있는거 찾아서 넣어주자
	size_t location = s1.find_last_of("\\");
	string TID = s1.substr(location + 1, s1.size() - location);
	string cell_info_name = "\\CellInfo_" + TID + "_WMP.1.txt";
	readCellInfo(cell_info_name, adj_focus, set_focus);

	readCalibData(calib_data, "tof30_calib.csv");
	//for (int i = 0; i < calib_data_3x.size(); i++)
	//{
	//	cout << i << "'s shift_y : " << calib_data_3x[i].shift_y << endl;
	//	cout << i << "'s pitch_x : " << calib_data_3x[i].pitch_x << endl;
	//}
	return true;
}
bool Load_Img()
{
	cout << input_directory << endl;
	//여기서 ImgName을 읽은것만큼 이미지 가져와서 프로세싱 할꺼임
	ImageNameList;

	//근데 읽기전에 CellInfo도 읽어올꺼임
	//isCellInfo를 봐서 있으면 

	//폴더내에 bmp나 jpg 싹다 읽어들이자
	_finddata_t fd;
	long long handle;
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
			if ((base.find("I") != string::npos))
			{
				ImageNameList.push_back(fd.name);
				cout << "File : " << fd.name << endl;
			}
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
			//cout << "no img : " << FN << endl;
			continue;
		}
		else
		{
			cout << "read img : " << FN << endl;
		}
		//회전을 해야한다면 여기서 하자
		//세로보다 가로가 더 크다면
		//if (loadImg.cols > loadImg.rows)
		//	IsRotation = true;
		//else
		//	IsRotation = false;
		//if (job_code == 1 && IsRotation)
		//{
		//	double skew = -90;
		//	//PrintFunc("before", loadImg, 4);
		//	//출처: http://crazymind.net/47 [현실감각 0%]
		//	cout << "skew : " << skew << endl;
		//	Mat t_img;
		//	transpose(loadImg, t_img);
		//	flip(t_img, loadImg,1);
		//	//PrintFunc("after", loadImg, 4);
		//	//waitKey();
		//}
		//여기서 Cell_Info읽어와서 

		//resize( loadImg, loadImg, cv::Size( loadImg.cols/2,loadImg.rows/2 ), 0, 0, CV_INTER_LINEAR );
		//split(loadImg,BGR);
		//Mat image_r(loadImg.rows, loadImg.cols, CV_8UC1);
		//Mat image_g(loadImg.rows, loadImg.cols, CV_8UC1);
		//Mat image_b(loadImg.rows, loadImg.cols, CV_8UC1);
		Mat gray;
		//Mat out[] = { image_b, image_g, image_r };
		//int from_to[] = { 0,0, 1,1, 2,2 };
		//mixChannels(&loadImg, 1, out, 3, from_to, 3);
		cvtColor(loadImg, gray, CV_BGR2GRAY);
		ImagePack temp;
		temp.Original = loadImg;
		//temp.B = image_b;
		//temp.G = image_g;
		//temp.R = image_r;
		//imshow(ImageNameList[i],gray);
		temp.Gray = gray;
		string ImgName = ImageNameList[i];
		//if (ImageNameList[i].find(".BMP") != string::npos)
		{
			std::string::size_type dot = ImageNameList[i].find_last_of('.');
			ImgName = ImageNameList[i].substr(0, dot);
			//cout << ImgName << endl;
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
	if (!Load_Calib())
		return false;
	if (!Load_Img())
		return false;

	return true;
}

//일단 여기다 구현해놓고 나중에 다른곳에 옮기자
static  Mat createDataMatrix(const vector<Mat> &images)
{
	cout << "Creating data matrix from images ...";

	// Allocate space for all images in one data matrix.
	// The size of the data matrix is
	//
	// ( w  * h  * 3, numImages )
	//
	// where,
	//
	// w = width of an image in the dataset.
	// h = height of an image in the dataset.
	// 3 is for the 3 color channels.
	// numImages = number of images in the dataset.
	int channels_num = images.front().channels();
	Mat data(static_cast<int>(images.size()), images[0].rows * images[0].cols * channels_num, CV_32F);

	// Turn an image into one row vector in the data matrix
	for (unsigned int i = 0; i < images.size(); i++)
	{
		// Extract image as one long vector of size w x h x 3
		Mat image = images[i].reshape(1, 1);

		// Copy the long vector into one row of the destm
		image.copyTo(data.row(i));	
	}
	//Mat img;
	//Mat tt = data.row(0).reshape(channels_num, images[0].rows);
	//tt /= 255.0;
	//imshow("resize_gray.front", tt);
	//waitKey();
	cout << " DONE" << endl;
	return data;
}

void doPCA()
{
	vector<Mat> images;
	vector<Mat> eigenFaces;
	Size sz = Img[0].Gray.size();
	for (int i = 0; i < Img.size(); i++)
	{
		//일정한 사이즈랑 포멧 맞춰주기, 평균에 맞게 정규화시켜주기
		Mat resize_gray;
		resize(Img[i].Gray, resize_gray, sz);
		resize_gray.convertTo(resize_gray, CV_32F, 1.0 / 255.0);
		images.push_back(resize_gray);

	}

	//Resize 및 Equalize 수행해야함
	Mat eigenVectors = imread(".\\DB\\eigenVectors.exr", cv::IMREAD_ANYDEPTH);
	Mat eigenValue = imread(".\\DB\\eigenValue.exr", cv::IMREAD_ANYDEPTH);
	Mat averageFace = imread(".\\DB\\averageFace.exr", cv::IMREAD_ANYDEPTH);

	//if (eigenVectors.empty()) 
	{

		// Create data matrix for PCA.
		Mat data = createDataMatrix(images);

		cout << "Calculating PCA ...";
		//NUM_EIGEN_FACES = 10;
		PCA pca(data, Mat(), PCA::DATA_AS_ROW, 20);
		cout << " DONE" << endl;
		// Extract mean vector and reshape it to obtain average face
		averageFace = pca.mean.reshape(1, sz.height);
		// Find eigen vectors.
		eigenVectors = pca.eigenvectors;
		eigenValue = pca.eigenvalues;

		cout << eigenValue << endl;
		//FolderPath = LoadConfigure.input_directory;
		FolderPath = ".\\DB\\averageFace.exr";
		imwrite(FolderPath, averageFace);
		FolderPath = ".\\DB\\eigenVectors.exr";
		imwrite(FolderPath, eigenVectors);
		//FolderPath = LoadConfigure.input_directory;
		FolderPath = ".\\DB\\eigenValue.exr";
		imwrite(FolderPath, eigenValue);

		//Mat new_covs, mu;
		//calcCovarMatrix(data, new_covs, mu, CV_COVAR_NORMAL | CV_COVAR_ROWS);
		//eigen(new_covs, eigenValue, eigenVectors);

	}
	cout << eigenValue << endl;
	sz = averageFace.size();
	Mat test, resize_gray = imread("E:\\img\\연구활동\\PCA_TEST\\TEST\\TEST.png", 0);
	resize(resize_gray, resize_gray, sz);
	resize_gray.convertTo(resize_gray, CV_32F, 1.0 / 255.0);
	// Reshape Eigenvectors to obtain EigenFaces
	int eigen_nume = 15; Img.size();
	eigenFaces.clear();
	for (int i = 0; i < eigen_nume; i++)
	{
		Mat eigen = eigenVectors.row(i).reshape(1, sz.height);
		eigenFaces.push_back(eigen);
	}

	Mat v_k = -1 * createDataMatrix(eigenFaces);
	Mat test_img = (resize_gray);
	transpose(test_img.reshape(1, 1), test_img);// resize_gray;
	//PrintFunc("averageFace", averageFace, 1);
	//PrintFunc("test_img", resize_gray, 1);
	cout << "test_img.size() : " << test_img.size() << endl;
	cout << "eigenVectors.size() : " << v_k.size() << endl;
	Mat y_k = v_k * test_img;
	cout << "ga.size() : " << y_k.size() << endl;
	cout << "ga : " << y_k << endl;
	Mat dd;
	transpose(y_k, y_k);
	cout << "eigenVectors.size() : " << v_k.size() << endl;
	cout << "ga.size() : " << y_k.size() << endl;
	Mat x_recons = (y_k * v_k);
	FileStorage fs(".\\DB\\v_k.yml", FileStorage::WRITE);
	write(fs, "v_k", v_k);
	fs.release();
	cout << "t_eigen.size() : " << x_recons.size() << endl;
	Mat reconstructed_IMG = x_recons.reshape(1, sz.height);
	//PrintFunc("reconstructed image", reconstructed_IMG, 1);
	reconstructed_IMG.convertTo(reconstructed_IMG, CV_8U, 255);
	FolderPath = input_directory;
	FolderPath += "\\reconFace" + int2str(eigen_nume) + ".bmp";
	imwrite(FolderPath, reconstructed_IMG);
	resize_gray = imread("E:\\img\\연구활동\\PCA_TEST\\TEST\\TEST.png", 0);
	resize(resize_gray, resize_gray, sz);
	//PrintFunc("reconstructed-image", abs(reconstructed_IMG - resize_gray), 1);

	waitKey();
	cout << x_recons << endl;
}