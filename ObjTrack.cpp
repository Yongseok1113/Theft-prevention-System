#include "ObjTrack.hpp"
////////////////////////////////////////////////////////////////////////////////////
//								ObjTrack ������									  //
////////////////////////////////////////////////////////////////////////////////////
// �⺻ ������
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
	cv::GaussianBlur(img, img, cv::Size(kernel, kernel), 1.5);	// ����þ� ���� (������ ������ ������ ����)
	cv::medianBlur(img, img, kernel);						// �޵�� ���� (salt & papper ������ ����)
}

// �÷� ������ ������׷� ��Ȱȭ (��⸦ ��������, YCrCb)

//YCrCb
/*
void ObjTrack::colorHistogram()
{
	std::vector<cv::Mat> channels;			// ������ ä���� ���� ���͸� ����
	cv::cvtColor(img, img, CV_BGR2YCrCb);	// ������ BGR������ ������ YCrCb�������� ��ȯ

	cv::split(img, channels);					// ������ �� ä���� �����Ͽ� ���Ϳ� ����
	cv::equalizeHist(channels[0], channels[0]);	// ������ Y(�ֵ�)������ �������� ������׷� ��Ȱȭ
	cv::merge(channels, img);					// ��Ȱȭ�� ���� �� �� ������ �ٽ� �ϳ��� ��ħ

	cv::cvtColor(img, img, CV_YCrCb2RGB);		// YCrCb ������ ������ �ٽ� BGR������ �������� ��ȯ
}
*/
//hsv
void ObjTrack::colorHistogram()
{
	std::vector<cv::Mat> channels;			// ������ ä���� ���� ���͸� ����
	cv::split(img, channels);					// ������ �� ä���� �����Ͽ� ���Ϳ� ����
	cv::cvtColor(img, img, CV_BGR2HSV);	// ������ BGR������ ������ hsv�������� ��ȯ

	
	cv::equalizeHist(channels[0], channels[0]);	// ������ Y(�ֵ�)������ �������� ������׷� ��Ȱȭ
	cv::merge(channels, img);					// ��Ȱȭ�� ���� �� �� ������ �ٽ� �ϳ��� ��ħ

	cv::cvtColor(img, img, CV_BGR2HSV);		// hsv ������ ������ �ٽ� BGR������ �������� ��ȯ
}


// ���� ������ (���� ��ȭ)
void ObjTrack::sharp()
{
	// ���� �������� ���� Ŀ�� ����
	cv::Mat kernel = (cv::Mat_<float>(3, 3) <<
		1, 1, 1,
		1, -8, 1,
		1, 1, 1);

	cv::Mat imglaplacian;
	cv::Mat sharp = img.clone();	// ���� �̹����� �����Ͽ� ���	
	cv::filter2D(sharp, imglaplacian, CV_32F, kernel);	// �̹��� ������

	img.convertTo(sharp, CV_32F);
	img = sharp - imglaplacian;
	img.convertTo(img, CV_8UC3);
}

// ������ �������� ����
void ObjTrack::findContours()
{
	cv::Mat img_gray;							// 1ä�� ������ ���� ����
	cv::cvtColor(img, img_gray, CV_BGR2GRAY);	// �÷� ������ 1ä�� ȸ�� �������� ��ȯ
	cv::Mat threshold_output;					// �������� �����Ͽ� �ӽ÷� ������ ����

	int thresh = this->threshold;				// �Ӱ谪�� �̿��Ͽ� �������� ����
	cv::threshold(img_gray, threshold_output, thresh, 255, cv::THRESH_BINARY);
	cv::findContours(threshold_output, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0));

	contours_poly = std::vector<std::vector<cv::Point> >(contours.size());	// ������ �������� ���������� �޾ƿ�
	boundRect = std::vector<cv::Rect>(contours.size());					// �������� ��ġ�� �簢������ �޾ƿ�

	for (int i = 0; i < contours.size(); i++)
	{
		cv::approxPolyDP(cv::Mat(contours[i]), contours_poly[i], 3, true);
		boundRect[i] = cv::boundingRect(cv::Mat(contours_poly[i]));
	}
}

cv::Mat ObjTrack::print_findContours()
{
	
	cv::Mat img_gray;							// 1ä�� ������ ���� ����
	cv::cvtColor(img, img_gray, CV_BGR2GRAY);	// �÷� ������ 1ä�� ȸ�� �������� ��ȯ
	cv::Mat threshold_output;					// �������� �����Ͽ� �ӽ÷� ������ ����

	int thresh = this->threshold;				// �Ӱ谪�� �̿��Ͽ� �������� ����
	cv::threshold(img_gray, threshold_output, thresh, 255, cv::THRESH_BINARY);
	cv::findContours(threshold_output, contours, hierarchy, CV_RETR_TREE, CV_CHAIN_APPROX_SIMPLE, cv::Point(0, 0));
	cv::drawContours(threshold_output, contours, -1, cv::Scalar(0), 2);
	contours_poly = std::vector<std::vector<cv::Point> >(contours.size());	// ������ �������� ���������� �޾ƿ�
	boundRect = std::vector<cv::Rect>(contours.size());					// �������� ��ġ�� �簢������ �޾ƿ�

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
	cv::drawContours(src, contours, -1, cv::Scalar(0, 0, 255), 3);	// ���������� �׸�
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
