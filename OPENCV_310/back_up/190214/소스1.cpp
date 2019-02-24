#include "헤더.h"
#include "Modeling.h"
#include "BallWire.h"
#include "Util.h"

//급 Connector 시작
//패턴 매칭으로 검사 영역 부분을 찾고 그부분에서만 길이 찾아서 해보자

//190124
//목표 : 기존의 TOF BallWire를 수정해서 쓰자
//Step1 : 모델링 할만한게 없으니 걍 BallWire의 패턴 매칭 가져다 쓰자

int main(int argc, char** argv)
{
	

	if (!Load_all(argc, argv))
		exit(-1);
	vector<Mat> src;
	Mat dst;
	job_code = DO_MODELING | DO_MEASUREMENT;


	
	//Modeling Part, First와 Second로 나누어서 모델링해야함
	int modeling_result = 0;
	int placement_result = 0;
	int defect_result = 0;
	int ballwire_result = 0;
	if ((job_code & DO_MODELING) == DO_MODELING)
	{

		cout << "DO_MODELING1" << endl;
		//1. Modeling.ini에서 필요한 이미지만 읽기
		//선언
		
		Modeling mo(Img, LoadConfigure);
		//if (mo.LoadModeingINI())
		//	cout << "good read" << endl;
		//else
		//	cout << "bad" << endl;
		

		mo.loadModeingINIByField();
		//First Modeling Part
		
		mo.calSkewFunc();

		if (!mo.ChipModeling())
			modeling_result |= DEFECT_MODELING_CHIP;
		
		


		mo.ModelingResultFirst(modeling_result);
	
		mo.MeasureDist();

	}

	waitKey();
	return 0;
}