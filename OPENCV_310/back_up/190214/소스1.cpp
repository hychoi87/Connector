#include "���.h"
#include "Modeling.h"
#include "BallWire.h"
#include "Util.h"

//�� Connector ����
//���� ��Ī���� �˻� ���� �κ��� ã�� �׺κп����� ���� ã�Ƽ� �غ���

//190124
//��ǥ : ������ TOF BallWire�� �����ؼ� ����
//Step1 : �𵨸� �Ҹ��Ѱ� ������ �� BallWire�� ���� ��Ī ������ ����

int main(int argc, char** argv)
{
	

	if (!Load_all(argc, argv))
		exit(-1);
	vector<Mat> src;
	Mat dst;
	job_code = DO_MODELING | DO_MEASUREMENT;


	
	//Modeling Part, First�� Second�� ����� �𵨸��ؾ���
	int modeling_result = 0;
	int placement_result = 0;
	int defect_result = 0;
	int ballwire_result = 0;
	if ((job_code & DO_MODELING) == DO_MODELING)
	{

		cout << "DO_MODELING1" << endl;
		//1. Modeling.ini���� �ʿ��� �̹����� �б�
		//����
		
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