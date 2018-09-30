#pragma once
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

class TrackManager
{
private:
	std::vector<cv::Rect> object_list;	
public:
	TrackManager::TrackManager();
	TrackManager::TrackManager(std::vector<cv::Rect> inputdata);

	cv::Mat TrackManager::drawObject(cv::Mat src);
	void TrackManager::setObject(std::vector<cv::Rect> inputdata);
	void TrackManager::composeObject(std::vector<cv::Rect>& object, std::vector<bool>& flags);
};
