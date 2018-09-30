/*
*	�ۼ��� : Oh YongSeok
*	��  ¥ : 2017.05.02
*	��  �� : �������� �ý���
*   �̸��� : ysoh1113@gmail.com
*/
#include <iostream>
#include <Windows.h>
#include "ObjTrack.hpp"
#include "ObjectManager.hpp"
#include "TrackManager.hpp"
#include <fstream>

using namespace std;
double compareImg(cv::Mat img1, cv::Mat img2);
int main()
{
	//1 ī�޶� �ʱ�ȭ
	ofstream fout("output.txt");
	cv::VideoCapture camera;	// ����� ī�޶� ����ϱ����� ��ü ����
	camera.open(0);				// ����� 0��ī�޶� (��ž�� ��� ��ķ) ���
	
	if (!camera.isOpened())		// ī�޶� ���������� ������ ������� ���� ó��
	{
		printf("Camera not found!\n");
		return -1;
	}
	cv::namedWindow("Camera", CV_WINDOW_AUTOSIZE);	// ī�޶��� ������ �ǽð����� ����� ������ ����
	//cv::namedWindow("Object", CV_WINDOW_AUTOSIZE);
	
	//1 ��
	
	//2 Image Detection
	cv::Mat frame;			// ī�޶󿡼� �޾ƿ� ������ ���� ����
	cv::Mat back_frame;		// �������� �̹��� ���庯��
	cv::Mat save_frame;
	
	ObjTrack tracker;
	objectManager manager;
	TrackManager trackManager;
	//Sleep(1000);
	camera.read(frame);

	while (true)
	{
		//2. Image Read
		back_frame = frame.clone();	//���� �̹���.
		camera.read(frame);		// ī�޶󿡼� ���� ������ �޾� frame ������ ����
		//camera.read(save_frame);
		if (frame.empty())		// ī�޶� ������ �۵� �� ���� ó��
		{
			continue;
		}
		double com = compareImg(frame, back_frame);
		if (com <= 0.95)
		{
			continue;
		}
		//2. end

		//3 object Detection
		//���� Ž��.
		cv::flip(frame, frame, 1);	// ī�޶󿡼� �޾ƿ� ������ �����Ǿ� ������ ������ ����������.
		tracker = ObjTrack();
		tracker.setImg(frame.clone());

		tracker.blur(3);
		tracker.colorHistogram();

		tracker.sharp();
		tracker.blur(3);
		tracker.findContours(); //������ ����
		tracker.findObject();

		manager.setObject(tracker.getObject());
		manager.drawObject(frame);
		//cv::imshow("Camera", test);

		imshow("Camera", frame);
		//3 end
		

		//4 chmod Image Detection to Thef Detection
		if (cv::waitKey(10) == 27)
		{
			fout << "Set Object : " << tracker.getObject().size() << endl;
			for (int i = 0; i < manager.object_list.size(); i++) {
				fout << i << " size : " << manager.object_list[i].area() << endl;
			}
			fout << "compare(CV_COMP_CORREL): " << com << endl;
			break;
		}

		//cv::Mat test = tracker.print_findContours();
		
	}
	//2 function end
	
	//3 Thef Detecting function
	while (true)
	{
		back_frame = frame.clone();
		camera.read(frame);		// ī�޶󿡼� ���� ������ �޾� frame ������ ����

		if (frame.empty())		// ī�޶� ������ �۵� �� ���� ó��
		{
			continue;
		}
		double com = compareImg(frame, back_frame);
		if (com <= 0.95)
		{
			continue;
		}

		cv::flip(frame, frame, 1);	// ī�޶󿡼� �޾ƿ� ������ ������Ŵ
		tracker = ObjTrack();
		tracker.setImg(frame.clone());

		tracker.blur(3);
		tracker.colorHistogram();
		tracker.sharp();
		tracker.blur(3);
		tracker.findContours();
		tracker.findObject();

		trackManager.setObject(tracker.getObject());

		trackManager.composeObject(manager.getObjectList(), manager.getFlagList());
		cv::Mat img = frame.clone();
		manager.drawTrackObject(img);
		
		//trackManager.drawObject(img);

		imshow("Camera", img);

		if (cv::waitKey(30) == 27)
		{
			fout << "Set Object : " << tracker.getObject().size() << endl;
			fout << "intail Object number : " << manager.getObjectList().size() << "\tCurrent Object number : " << manager.object_list.size() << std::endl;
			for (int i = 0; i < manager.object_list.size(); i++) {
				fout << i << " size : " << manager.object_list[i].area() << endl;
			}
			
			fout << "compare(CV_COMP_CORREL): " << com << endl;
			fout.close();
			break;
		}
	}
	//3 end
}

//�̹�����(�̹���1, �̹���2)
double compareImg(cv::Mat img1, cv::Mat img2)
{
	//��ü����
	cv::Mat img1_gray, img2_gray;//�̹��������� ������׷����� ����� ��ü.
	//�׷��̽�����
	cv::cvtColor(img1, img1_gray, CV_BGR2GRAY);
	cv::cvtColor(img2, img2_gray, CV_BGR2GRAY);

	int histSize = 256;
	float range[] = { 0, 255 };
	const float* histRange = { range };
	bool uniform = true;
	bool accumulate = false;
	cv::Mat a1_hist, a2_hist;
	//���۾��� histogram�� �ְ� ������� callbackparam�� �־����. �̰��� �̹��� ��ü�����Ѱ��̹Ƿ�. ���ɿ���(ROI)�� �ٲ����.
	cv::calcHist(&img1_gray, 1, 0, cv::Mat(), a1_hist, 1, &histSize, &histRange, uniform, accumulate);
	cv::calcHist(&img2_gray, 1, 0, cv::Mat(), a2_hist, 1, &histSize, &histRange, uniform, accumulate);

	double compar_c = cv::compareHist(a1_hist, a2_hist, CV_COMP_CORREL);
	std::cout << "compare(CV_COMP_CORREL): " << compar_c << "\n";

	return compar_c;
}
