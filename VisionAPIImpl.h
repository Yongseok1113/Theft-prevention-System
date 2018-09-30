#include <opencv/cxcore.h>
#include <opencv2/opencv.hpp>
#include <opencv/highgui.h>
#include <opencv2\opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2\imgproc\imgproc.hpp>
#include <opencv2\highgui\highgui.hpp>
#include <opencv2/features2d/features2d.hpp>
#include <opencv2/opencv.hpp>
#include <cstdlib>
#include "raspicam/raspicam_cv.h"


class TrackManager
{
private:
	std::vector<cv::Rect> object_list;   // 오브젝트 관리
										 //std::vector<bool> Trackflags;
public:
	TrackManager::TrackManager();
	TrackManager::TrackManager(std::vector<cv::Rect> inputdata);
	void TrackManager::setObject(std::vector<cv::Rect> inputdata);
	cv::Mat TrackManager::drawObject(cv::Mat src);

	void TrackManager::composeObject(std::vector<cv::Rect>& object, std::vector<bool>& flags);
};

class ObjTrack
{
private:
	cv::Mat img;         // 사용할 영상
	cv::Mat ori;         // 원본 영상
	int threshold = 100;   // 트레숄드 값

	std::vector<std::vector<cv::Point>> contours;   // 윤곽선
	std::vector<cv::Vec4i> hierarchy;            // 하르라키

	std::vector<std::vector<cv::Point>> contours_poly;   // 오브젝트 폴리곤
	std::vector<cv::Rect> boundRect;               // 오브젝트 사각형

public:
	//생성자
	ObjTrack::ObjTrack();   // 기본 생성자
	ObjTrack::ObjTrack(cv::Mat src, int threshold = 100);   // Mat를 입력으로 받는 생성자

															// 변수 설정
	void ObjTrack::setImg(cv::Mat src);         // 영상 설정
	void ObjTrack::setThreshold(int threshold);   // 트레숄드(임계값) 설정

												  // 변수 반환
	cv::Mat ObjTrack::getImg();         // 현재 영상 반환
	cv::Mat ObjTrack::getOriginalImg();   // 처리되지 않은 원본 영상 반환
	int ObjTrack::getThreshold();      // 트레숄드(임계값) 반환
	std::vector<cv::Rect> ObjTrack::getObject();   // 검출된 오브젝트 리스트 반환

												   // 분할 함수
	void ObjTrack::blur(int kernel);   // 이미지 블러링
	void ObjTrack::colorHistogram();   // 컬러 영상 히스토그램 평활화 (밝기)
	void ObjTrack::sharp();   // 이미지 샤프닝 (에지 강조)
	void ObjTrack::findContours();      // 외곽선 검출
	cv::Mat ObjTrack::print_findContours();
	void ObjTrack::findObject();         // 오브젝트의 위치를 검출

										 // 그리기 함수
	cv::Mat ObjTrack::drawContours(cv::Mat src);   // 소스 영상에 외곽선 그리기
	cv::Mat ObjTrack::drawObject(cv::Mat src);      // 소스 영상에 오브젝트 위치 그리기
};

class objectManager
{
public:
	std::vector<cv::Rect> object_list;   // 오브젝트 관리
	std::vector<bool> flags;

public:
	objectManager::objectManager();
	objectManager::objectManager(std::vector<cv::Rect> inputdata);
	void objectManager::setObject(std::vector<cv::Rect> inputdata);
	cv::Mat objectManager::drawObject(cv::Mat src);
	cv::Mat objectManager::drawTrackObject(cv::Mat src);
	void objectManager::setObject(cv::Rect rect, int index, int flag);

	std::vector<cv::Rect>& objectManager::getObjectList();
	std::vector<bool>& objectManager::getFlagList();
};

class ThefDetector {
private:
	raspicam::RaspiCam_Cv camera;   // 장비의 카메라를 사용하기위한 객체 생성
	cv::Mat img;
	cv::Mat frame;         // 카메라에서 받아올 영상을 담을 변수
	cv::Mat back_frame;
	cv::Mat save_frame;
	ObjTrack tracker;
	objectManager manager;
	TrackManager trackManager;
	double compareImg(cv::Mat img1, cv::Mat img2);
public:
	int InitCamera();
	int ImgRead();
	void EdgeDetecting();
	int ModeChange();
	void ImageShow();
	void ThefDetecting();
	void ThefImgShow();
};