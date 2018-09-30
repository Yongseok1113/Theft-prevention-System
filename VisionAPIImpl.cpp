#include "VisionAPIImpl.h"

#include <fstream>
#include <iostream>
//#include <windows.h> //linux ������ �ٲ����


using namespace std;
double compareImg(cv::Mat img1, cv::Mat img2);

objectManager::objectManager()
{
}

objectManager::objectManager(std::vector<cv::Rect> inputdata)
{
	std::cout << "Gen Object" << std::endl;
	std::vector<cv::Rect> temp = std::vector<cv::Rect>(inputdata.size());

	int j = 0;
	for (int i = 0; i < inputdata.size(); i++)
	{
		if ((inputdata[i].width <= 25) || (inputdata[i].height <= 25) || (inputdata[i].width >= 600) || (inputdata[i].height >= 600))
		{
			continue;
		}
		temp[j] = inputdata[i];
		j++;
	}
	object_list = std::vector<cv::Rect>(j);
	flags = std::vector<bool>(j);
	for (int i = 0; i < temp.size(); i++)
	{
		object_list[i] = temp[i];
		flags[i] = true;
	}

	return;
}

void objectManager::setObject(std::vector<cv::Rect> inputdata)
{
	
	std::cout << "Set Object : " << inputdata.size() << std::endl;
	std::vector<cv::Rect> temp = std::vector<cv::Rect>(inputdata.size());

	int j = 0;
	for (int i = 0; i < inputdata.size(); i++)
	{
		if ((inputdata[i].width <= 25) || (inputdata[i].height <= 25) || (inputdata[i].width >= 600) || (inputdata[i].height >= 600))
		{
			continue;
		}
		temp[j] = inputdata[i];
		j++;
	}
	object_list = std::vector<cv::Rect>(j);
	flags = std::vector<bool>(j);
	for (int i = 0; i < j; i++)
	{
		object_list[i] = temp[i];
		std::cout << i << " size : " << object_list[i].area() << std::endl;
	}

	return;
}

cv::Mat objectManager::drawObject(cv::Mat src)
{
	for (int i = 0; i < this->object_list.size(); i++)
	{
		cv::Scalar color = cv::Scalar(0, 0, 255);
		if (!flags[i])   //
		{

			color = cv::Scalar(0, 255, 0);
			cv::rectangle(src, object_list[i].tl(), object_list[i].br(), color, 2, 8, 0);
		}
		else if (flags[i]) {
			cv::Scalar color = cv::Scalar(0, 0, 255);
			cv::rectangle(src, object_list[i].tl(), object_list[i].br(), color, 2, 8, 0);
		}
	}
	return src.clone();
}
cv::Mat objectManager::drawTrackObject(cv::Mat src)
{
	for (int i = 0; i < this->object_list.size(); i++)
	{
		if (!flags[i]) {
			cv::Scalar color = cv::Scalar(0, 0, 255);
			cv::rectangle(src, object_list[i].tl(), object_list[i].br(), color, 2, 8, 0);
		}
	}
	return src.clone();
}

void objectManager::setObject(cv::Rect rect, int index, int flag)
{
	if (flag == 1)
	{
		this->object_list[index] = rect;
	}
	if (flag == 0)
	{
		this->flags[index] = false;
	}
}

std::vector<cv::Rect>& objectManager::getObjectList()
{
	return object_list;
}

std::vector<bool>& objectManager::getFlagList()
{
	return this->flags;
}


//ObjTrack
ObjTrack::ObjTrack()
{
	return;
}

// ����� ������ �Ű������� �޴� ������ (�ʱ�ȭ)
ObjTrack::ObjTrack(cv::Mat src, int threshold)
{
	this->img = src;
	this->ori = src.clone();
	this->threshold = threshold;
}

// ����� ���� ����
void ObjTrack::setImg(cv::Mat src)
{
	this->img = src;
	this->ori = src.clone();
}

// ����� Ʈ�����(�Ӱ谪) ����
void ObjTrack::setThreshold(int threshold)
{
	this->threshold = threshold;
}

// ���� ó���� ���� ��ȯ
cv::Mat ObjTrack::getImg()
{
	return this->img;
}

// �ʱ� �Է¹��� ���� ���� ��ȯ
cv::Mat ObjTrack::getOriginalImg()
{
	return this->ori;
}

// ������� Ʈ�����(�԰谪) ��ȯ
int ObjTrack::getThreshold()
{
	return this->threshold;
}

std::vector<cv::Rect> ObjTrack::getObject()
{
	return this->boundRect;
}

// �̹��� ���� (������ ǰ���� ����, ������ ����)
void ObjTrack::blur(int kernel)
{
	// �Ű������� ���� Ŀ���� ũ�⸦ �������� ���͸� ����
	cv::GaussianBlur(img, img, cv::Size(kernel, kernel), 1.5);   // ����þ� ���� (������ ������ ������ ����)
	cv::medianBlur(img, img, kernel);                  // �޵�� ���� (salt & papper ������ ����)
}

// �÷� ������ ������׷� ��Ȱȭ (��⸦ ��������, YCrCb)

void ObjTrack::colorHistogram()
{
	std::vector<cv::Mat> channels;         // ������ ä���� ���� ���͸� ����
	cv::cvtColor(img, img, CV_BGR2YCrCb);   // ������ BGR������ ������ YCrCb�������� ��ȯ

	cv::split(img, channels);               // ������ �� ä���� �����Ͽ� ���Ϳ� ����
	cv::equalizeHist(channels[0], channels[0]);   // ������ Y(�ֵ�)������ �������� ������׷� ��Ȱȭ
	cv::merge(channels, img);               // ��Ȱȭ�� ���� �� �� ������ �ٽ� �ϳ��� ��ħ

	cv::cvtColor(img, img, CV_YCrCb2BGR);      // YCrCb ������ ������ �ٽ� BGR������ �������� ��ȯ
}
/*
//hsv
void ObjTrack::colorHistogram()
{
std::vector<cv::Mat> channels;         // ������ ä���� ���� ���͸� ����
cv::split(img, channels);               // ������ �� ä���� �����Ͽ� ���Ϳ� ����
cv::cvtColor(img, img, CV_BGR2HSV);   // ������ BGR������ ������ YCrCb�������� ��ȯ


cv::equalizeHist(channels[0], channels[0]);   // ������ Y(�ֵ�)������ �������� ������׷� ��Ȱȭ
cv::merge(channels, img);               // ��Ȱȭ�� ���� �� �� ������ �ٽ� �ϳ��� ��ħ

cv::cvtColor(img, img, CV_BGR2HSV);      // YCrCb ������ ������ �ٽ� BGR������ �������� ��ȯ
}
*/

// ���� ������ (���� ��ȭ)
void ObjTrack::sharp()
{
	// ���� �������� ���� Ŀ�� ����
	cv::Mat kernel = (cv::Mat_<float>(3, 3) <<
		1, 1, 1,
		1, -8, 1,
		1, 1, 1);

	cv::Mat imglaplacian;
	cv::Mat sharp = img.clone();   // ���� �̹����� �����Ͽ� ���   
	cv::filter2D(sharp, imglaplacian, CV_32F, kernel);   // �̹��� ������

	img.convertTo(sharp, CV_32F);
	img = sharp - imglaplacian;
	img.convertTo(img, CV_8UC3);
}

// ������ �������� ����
void ObjTrack::findContours()
{
	cv::Mat img_gray;                     // 1ä�� ������ ���� ����
	cv::cvtColor(img, img_gray, CV_BGR2GRAY);   // �÷� ������ 1ä�� ȸ�� �������� ��ȯ
	cv::Mat threshold_output;               // �������� �����Ͽ� �ӽ÷� ������ ����

	int thresh = this->threshold;            // �Ӱ谪�� �̿��Ͽ� �������� ����
	cv::threshold(img_gray, threshold_output, thresh, 255, cv::THRESH_BINARY);
	cv::findContours(threshold_output, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0));

	contours_poly = std::vector<std::vector<cv::Point> >(contours.size());   // ������ �������� ���������� �޾ƿ�
	boundRect = std::vector<cv::Rect>(contours.size());               // �������� ��ġ�� �簢������ �޾ƿ�

	for (int i = 0; i < contours.size(); i++)
	{
		cv::approxPolyDP(cv::Mat(contours[i]), contours_poly[i], 3, true);
		boundRect[i] = cv::boundingRect(cv::Mat(contours_poly[i]));
	}
}

cv::Mat ObjTrack::print_findContours()
{

	cv::Mat img_gray;                     // 1ä�� ������ ���� ����
	cv::cvtColor(img, img_gray, CV_BGR2GRAY);   // �÷� ������ 1ä�� ȸ�� �������� ��ȯ
	cv::Mat threshold_output;               // �������� �����Ͽ� �ӽ÷� ������ ����

	int thresh = this->threshold;            // �Ӱ谪�� �̿��Ͽ� �������� ����
	cv::threshold(img_gray, threshold_output, thresh, 255, cv::THRESH_BINARY);
	cv::findContours(threshold_output, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0));
	cv::drawContours(threshold_output, contours, -1, cv::Scalar(0), 2);
	contours_poly = std::vector<std::vector<cv::Point> >(contours.size());   // ������ �������� ���������� �޾ƿ�
	boundRect = std::vector<cv::Rect>(contours.size());               // �������� ��ġ�� �簢������ �޾ƿ�

	for (int i = 0; i < contours.size(); i++)
	{
		cv::approxPolyDP(cv::Mat(contours[i]), contours_poly[i], 3, true);
		boundRect[i] = cv::boundingRect(cv::Mat(contours_poly[i]));
	}
	return threshold_output;
}
// ����� �������� ����Ͽ� ������Ʈ�� ��ġ�� ����
void ObjTrack::findObject()
{
}

// ���󿡼� ����� �������� �ҽ� ���� �׸��� ��ȯ
cv::Mat ObjTrack::drawContours(cv::Mat src)
{
	cv::drawContours(src, contours, -1, cv::Scalar(0, 0, 255), 3);   // ���������� �׸�
	return src;
}

// ���󿡼� ����� ��ü���� ��ġ�� �簢������ �ҽ� ���� �׸��� ��ȯ
cv::Mat ObjTrack::drawObject(cv::Mat src)
{
	for (int i = 0; i < this->contours.size(); i++)
	{
		cv::Scalar color = cv::Scalar(0, 0, 255);
		//cv::drawContours(ori, contours_poly, i, color, 1, 8, std::vector<cv::Vec4i>(), 0, cv::Point());
		cv::rectangle(ori, boundRect[i].tl(), boundRect[i].br(), color, 2, 8, 0);
	}
	return ori.clone();
}



//TrackManager
TrackManager::TrackManager()
{
}

TrackManager::TrackManager(std::vector<cv::Rect> inputdata)
{
	std::cout << "Gen Object" << std::endl;
	std::vector<cv::Rect> temp = std::vector<cv::Rect>(inputdata.size());

	int j = 0;
	for (int i = 0; i < inputdata.size(); i++)
	{
		if ((inputdata[i].width <= 25) || (inputdata[i].height <= 25) || (inputdata[i].width >= 600) || (inputdata[i].height >= 600))
		{
			continue;
		}
		temp[j] = inputdata[i];
		j++;
	}
	object_list = std::vector<cv::Rect>(j - 1);
	for (int i = 0; i < temp.size(); i++)
	{
		object_list[i] = temp[i];
	}

	return;
}

void TrackManager::setObject(std::vector<cv::Rect> inputdata)
{
	//object_list = std::vector<cv::Rect>(inputdata.size());
	std::cout << "Set Object : " << inputdata.size() << std::endl;
	std::vector<cv::Rect> temp = std::vector<cv::Rect>(inputdata.size());

	int j = 0;
	for (int i = 0; i < inputdata.size(); i++)
	{
		if ((inputdata[i].width <= 25) || (inputdata[i].height <= 25) || (inputdata[i].width >= 600) || (inputdata[i].height >= 600))
		{
			continue;
		}
		temp[j] = inputdata[i];
		j++;
	}
	std::cout << "object size : " << j << std::endl;
	object_list = std::vector<cv::Rect>(j);

	for (int i = 0; i < j; i++)
	{
		object_list[i] = temp[i];
	}

	return;
}

cv::Mat TrackManager::drawObject(cv::Mat src)
{

	for (int i = 0; i < this->object_list.size(); i++)
	{
		cv::Scalar color = cv::Scalar(0, 255, 0);
		//cv::rectangle(src, object_list[i].tl(), object_list[i].br(), color, 2, 8, 0);
		//if(Trackflags[i] == false)
		cv::rectangle(src, object_list[i].tl(), object_list[i].br(), color, 3, 8, 0);
	}
	return src.clone();
}
//object = ������ ������Ʈ ����Ʈ,
void TrackManager::composeObject(std::vector<cv::Rect>& object, std::vector<bool>& flags)
{
	std::cout << "intail Object number : " << object.size() << "\tCurrent Object number : " << object_list.size() << std::endl;
	for (int i = 0; i < object.size(); i++)
	{
		bool flag = false;
		//if (flags[i] == false)
		for (int j = 0; j < object_list.size(); j++)
		{
			bool size_flag = false;
			bool x_flag = false;
			bool y_flag = false;
			if (object_list[j].area() * 0.8 <= (double)object[i].area() &&
				(double)object[i].area() <= object_list[j].area() * 1.2) //������Ʈ�� ����� +- 20�ۼ�Ʈ�̳��϶�
			{
				size_flag = true;
			}
			if (object_list[j].x - 20 <= object[i].x && object[i].x <= object_list[j].x + 20) // ������Ʈ�� x��ǥ�� +-20�ȼ� �̳��϶�
			{
				x_flag = true;
			}
			if (object_list[j].y - 20 <= object[i].y && object[i].y <= object_list[j].y + 20) // ������Ʈ�� y��ǥ�� +-20�ȼ� �̳��϶�
			{
				y_flag = true;
			}
			if (size_flag && x_flag && y_flag) // �� �������� ��� �����Ѵٸ� = ���� ��ü�ΰɷ� ����.
			{
				object[i] = object_list[j];
				flag = true;
			}
			//std::cout << "size flag : " << size_flag << " \tx flag : " << x_flag << "\ty flag : " << y_flag << std::endl;
		}
		if (!flag)
		{
			flags[i] = false;

		}
		else if (flag) {
			flags[i] = true;
		}
	}
}

//main

struct CallbackParam {
	cv::Mat frame;            //��ü �̹���
							  //POINT centerpoint;         //��ü �߽���
	int size;               //��ü ������
	int shapePointcount;      //��ü ���Ư¡����
	int colorBGRY[4];         //��ü ���� Blue, Green, Red, Yellow
};

//�̹�����(�̹���1, �̹���2)
double ThefDetector::compareImg(cv::Mat img1, cv::Mat img2)
{
	//��ü����
	cv::Mat img1_gray, img2_gray;
	//�׷��̽�����
	cv::cvtColor(img1, img1_gray, CV_BGR2GRAY);
	cv::cvtColor(img2, img2_gray, CV_BGR2GRAY);

	int histSize = 256;
	float range[] = { 0, 255 };
	const float* histRange = { range };
	bool uniform = true;
	bool accumulate = false;
	cv::Mat a1_hist, a2_hist;

	cv::calcHist(&img1_gray, 1, 0, cv::Mat(), a1_hist, 1, &histSize, &histRange, uniform, accumulate);
	cv::calcHist(&img2_gray, 1, 0, cv::Mat(), a2_hist, 1, &histSize, &histRange, uniform, accumulate);

	double compar_c = cv::compareHist(a1_hist, a2_hist, CV_COMP_CORREL);
	std::cout << "compare(CV_COMP_CORREL): " << compar_c << "\n";

	return compar_c;
}

int ThefDetector::InitCamera()
{
	camera.set(CV_CAP_PROP_FORMAT, CV_8UC3);
	camera.set(CV_CAP_PROP_FRAME_WIDTH, 640);
	camera.set(CV_CAP_PROP_FRAME_HEIGHT, 480);         // ����� 0��ī�޶� (��ž�� ��� ��ķ) ���

	if (!camera.open())
	{
		printf("Camera not found!\n");
		return 0;
	}
	cv::namedWindow("Camera", CV_WINDOW_AUTOSIZE);   // ī�޶��� ������ �ǽð����� ����� ������ ����
	cv::namedWindow("Map", CV_WINDOW_AUTOSIZE);												 //cv::namedWindow("Object", CV_WINDOW_AUTOSIZE);
	return 1;
}

int ThefDetector::ImgRead()
{
	camera.read(back_frame);
	camera.read(frame);
	camera.read(save_frame);
	if (frame.empty())      // ī�޶� ������ �۵� �� ���� ó��
	{
		return 0;
	}
	double com = compareImg(frame, back_frame);
	if (com <= 0.95)
	{
		return 0;
	}
	return 1;
}

void ThefDetector::EdgeDetecting()
{
	//���� Ž��.
	cv::flip(frame, frame, 1);   // ī�޶󿡼� �޾ƿ� ������ �����Ǿ� ������ ������ ����������.
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
}

int ThefDetector::ModeChange()
{
	if (cv::waitKey(30) == 27)
	{
		////fout << "Set Object : " << tracker.getObject().size() << endl;
		////fout << "intail Object number : " << manager.getObjectList().size() << "\tCurrent Object number : " << manager.object_list.size() << std::endl;
		//for (int i = 0; i < manager.object_list.size(); i++) {
		//   fout << i << " size : " << manager.object_list[i].area() << endl;
		//}
		//fout << "compare(CV_COMP_CORREL): " << com << endl;
		//fout.close();
		return 1;
	}
	return 0;
}

void ThefDetector::ImageShow()
{
	cv::imshow("Camera", frame);
}

void ThefDetector::ThefDetecting()
{
	cv::flip(frame, frame, 1);   // ī�޶󿡼� �޾ƿ� ������ ������Ŵ
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
	img = frame.clone();
	manager.drawTrackObject(img);


}

void ThefDetector::ThefImgShow()
{
	cv::imshow("Camera", img);
}
