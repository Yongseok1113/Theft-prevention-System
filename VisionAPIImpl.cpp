#include "VisionAPIImpl.h"

#include <fstream>
#include <iostream>
//#include <windows.h> //linux 용으로 바꿔야함


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

// 사용할 영상을 매개변수로 받는 생성자 (초기화)
ObjTrack::ObjTrack(cv::Mat src, int threshold)
{
	this->img = src;
	this->ori = src.clone();
	this->threshold = threshold;
}

// 사용할 영상 설정
void ObjTrack::setImg(cv::Mat src)
{
	this->img = src;
	this->ori = src.clone();
}

// 사용할 트레숄드(임계값) 설정
void ObjTrack::setThreshold(int threshold)
{
	this->threshold = threshold;
}

// 현재 처리된 영상 반환
cv::Mat ObjTrack::getImg()
{
	return this->img;
}

// 초기 입력받은 원본 영상 반환
cv::Mat ObjTrack::getOriginalImg()
{
	return this->ori;
}

// 사용중인 트레숄드(입계값) 반환
int ObjTrack::getThreshold()
{
	return this->threshold;
}

std::vector<cv::Rect> ObjTrack::getObject()
{
	return this->boundRect;
}

// 이미지 블러링 (영상의 품질을 저하, 노이즈 제거)
void ObjTrack::blur(int kernel)
{
	// 매개변수로 받은 커널의 크기를 기준으로 필터를 설정
	cv::GaussianBlur(img, img, cv::Size(kernel, kernel), 1.5);   // 가우시안 블러링 (영상의 고주파 성분을 제거)
	cv::medianBlur(img, img, kernel);                  // 메디안 블러링 (salt & papper 노이즈 제거)
}

// 컬러 영상의 히스토그램 평활화 (밝기를 기준으로, YCrCb)

void ObjTrack::colorHistogram()
{
	std::vector<cv::Mat> channels;         // 영상의 채널을 담을 벡터를 생성
	cv::cvtColor(img, img, CV_BGR2YCrCb);   // 영상을 BGR혁식의 영상을 YCrCb형식으로 변환

	cv::split(img, channels);               // 영상의 각 채널을 분할하여 벡터에 저장
	cv::equalizeHist(channels[0], channels[0]);   // 영상의 Y(휘도)성분을 기준으로 히스토그램 평활화
	cv::merge(channels, img);               // 평활화가 끝난 뒤 각 성분을 다시 하나로 합침

	cv::cvtColor(img, img, CV_YCrCb2BGR);      // YCrCb 형식의 영상을 다시 BGR형식의 영상으로 변환
}
/*
//hsv
void ObjTrack::colorHistogram()
{
std::vector<cv::Mat> channels;         // 영상의 채널을 담을 벡터를 생성
cv::split(img, channels);               // 영상의 각 채널을 분할하여 벡터에 저장
cv::cvtColor(img, img, CV_BGR2HSV);   // 영상을 BGR혁식의 영상을 YCrCb형식으로 변환


cv::equalizeHist(channels[0], channels[0]);   // 영상의 Y(휘도)성분을 기준으로 히스토그램 평활화
cv::merge(channels, img);               // 평활화가 끝난 뒤 각 성분을 다시 하나로 합침

cv::cvtColor(img, img, CV_BGR2HSV);      // YCrCb 형식의 영상을 다시 BGR형식의 영상으로 변환
}
*/

// 영상 샤프닝 (에지 강화)
void ObjTrack::sharp()
{
	// 영상 샤프닝을 위한 커널 생성
	cv::Mat kernel = (cv::Mat_<float>(3, 3) <<
		1, 1, 1,
		1, -8, 1,
		1, 1, 1);

	cv::Mat imglaplacian;
	cv::Mat sharp = img.clone();   // 원본 이미지를 복사하여 사용   
	cv::filter2D(sharp, imglaplacian, CV_32F, kernel);   // 이미지 샤프닝

	img.convertTo(sharp, CV_32F);
	img = sharp - imglaplacian;
	img.convertTo(img, CV_8UC3);
}

// 영상의 윤곽선을 검출
void ObjTrack::findContours()
{
	cv::Mat img_gray;                     // 1채널 영상을 담을 변수
	cv::cvtColor(img, img_gray, CV_BGR2GRAY);   // 컬러 영상을 1채널 회색 영상으로 변환
	cv::Mat threshold_output;               // 윤곽선을 검출하여 임시로 저장할 변수

	int thresh = this->threshold;            // 임계값을 이용하여 윤곽선을 검출
	cv::threshold(img_gray, threshold_output, thresh, 255, cv::THRESH_BINARY);
	cv::findContours(threshold_output, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0));

	contours_poly = std::vector<std::vector<cv::Point> >(contours.size());   // 영상의 윤곽선을 폴리곤으로 받아옴
	boundRect = std::vector<cv::Rect>(contours.size());               // 폴리곤의 위치를 사각형으로 받아옴

	for (int i = 0; i < contours.size(); i++)
	{
		cv::approxPolyDP(cv::Mat(contours[i]), contours_poly[i], 3, true);
		boundRect[i] = cv::boundingRect(cv::Mat(contours_poly[i]));
	}
}

cv::Mat ObjTrack::print_findContours()
{

	cv::Mat img_gray;                     // 1채널 영상을 담을 변수
	cv::cvtColor(img, img_gray, CV_BGR2GRAY);   // 컬러 영상을 1채널 회색 영상으로 변환
	cv::Mat threshold_output;               // 윤곽선을 검출하여 임시로 저장할 변수

	int thresh = this->threshold;            // 임계값을 이용하여 윤곽선을 검출
	cv::threshold(img_gray, threshold_output, thresh, 255, cv::THRESH_BINARY);
	cv::findContours(threshold_output, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0));
	cv::drawContours(threshold_output, contours, -1, cv::Scalar(0), 2);
	contours_poly = std::vector<std::vector<cv::Point> >(contours.size());   // 영상의 윤곽선을 폴리곤으로 받아옴
	boundRect = std::vector<cv::Rect>(contours.size());               // 폴리곤의 위치를 사각형으로 받아옴

	for (int i = 0; i < contours.size(); i++)
	{
		cv::approxPolyDP(cv::Mat(contours[i]), contours_poly[i], 3, true);
		boundRect[i] = cv::boundingRect(cv::Mat(contours_poly[i]));
	}
	return threshold_output;
}
// 검출된 윤곽선을 사용하여 오브젝트의 위치를 검출
void ObjTrack::findObject()
{
}

// 영상에서 검출된 윤곽선을 소스 영상에 그린뒤 반환
cv::Mat ObjTrack::drawContours(cv::Mat src)
{
	cv::drawContours(src, contours, -1, cv::Scalar(0, 0, 255), 3);   // 빨간색으로 그림
	return src;
}

// 영상에서 검출된 물체들의 위치를 사각형으로 소스 영상에 그린뒤 반환
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
//object = 설정된 오브젝트 리스트,
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
				(double)object[i].area() <= object_list[j].area() * 1.2) //오브젝트의 사이즈가 +- 20퍼센트이내일때
			{
				size_flag = true;
			}
			if (object_list[j].x - 20 <= object[i].x && object[i].x <= object_list[j].x + 20) // 오브젝트의 x좌표가 +-20픽셀 이내일때
			{
				x_flag = true;
			}
			if (object_list[j].y - 20 <= object[i].y && object[i].y <= object_list[j].y + 20) // 오브젝트의 y좌표가 +-20픽셀 이내일때
			{
				y_flag = true;
			}
			if (size_flag && x_flag && y_flag) // 위 세조건을 모두 만족한다면 = 기존 물체인걸로 추측.
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
	cv::Mat frame;            //물체 이미지
							  //POINT centerpoint;         //물체 중심점
	int size;               //물체 사이즈
	int shapePointcount;      //물체 모양특징점수
	int colorBGRY[4];         //물체 색상값 Blue, Green, Red, Yellow
};

//이미지비교(이미지1, 이미지2)
double ThefDetector::compareImg(cv::Mat img1, cv::Mat img2)
{
	//객체생성
	cv::Mat img1_gray, img2_gray;
	//그레이스케일
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
	camera.set(CV_CAP_PROP_FRAME_HEIGHT, 480);         // 장비의 0번카메라 (랩탑의 경우 웹캠) 사용

	if (!camera.open())
	{
		printf("Camera not found!\n");
		return 0;
	}
	cv::namedWindow("Camera", CV_WINDOW_AUTOSIZE);   // 카메라의 영상을 실시간으로 출력할 윈도우 생성
	cv::namedWindow("Map", CV_WINDOW_AUTOSIZE);												 //cv::namedWindow("Object", CV_WINDOW_AUTOSIZE);
	return 1;
}

int ThefDetector::ImgRead()
{
	camera.read(back_frame);
	camera.read(frame);
	camera.read(save_frame);
	if (frame.empty())      // 카메라가 비정상 작동 시 예외 처리
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
	//엣지 탐지.
	cv::flip(frame, frame, 1);   // 카메라에서 받아온 영상은 반전되어 들어오기 때문에 반전시켜줌.
	tracker = ObjTrack();
	tracker.setImg(frame.clone());

	tracker.blur(3);
	tracker.colorHistogram();

	tracker.sharp();
	tracker.blur(3);
	tracker.findContours(); //윤곽선 검출
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
	cv::flip(frame, frame, 1);   // 카메라에서 받아온 영상을 반전시킴
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
