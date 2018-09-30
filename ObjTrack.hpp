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

class ObjTrack
{
private:
	cv::Mat img;			// ����� ����
	cv::Mat ori;			// ���� ����
	int threshold = 100;	// Ʈ����� ��

	std::vector<std::vector<cv::Point>> contours;	// ������
	std::vector<cv::Vec4i> hierarchy;				// �ϸ���Ű

	std::vector<std::vector<cv::Point>> contours_poly;	// ������Ʈ ������
	std::vector<cv::Rect> boundRect;					// ������Ʈ �簢��

public:
	//������
	ObjTrack::ObjTrack();	// �⺻ ������
	ObjTrack::ObjTrack(cv::Mat src, int threshold = 100);	// Mat�� �Է����� �޴� ������

	// ���� ����
	void ObjTrack::setImg(cv::Mat src);			// ���� ����
	void ObjTrack::setThreshold(int threshold);	// Ʈ�����(�Ӱ谪) ����

	// ���� ��ȯ
	cv::Mat ObjTrack::getImg();			// ���� ���� ��ȯ
	cv::Mat ObjTrack::getOriginalImg();	// ó������ ���� ���� ���� ��ȯ
	int ObjTrack::getThreshold();		// Ʈ�����(�Ӱ谪) ��ȯ
	std::vector<cv::Rect> ObjTrack::getObject();	// ����� ������Ʈ ����Ʈ ��ȯ

	// ���� �Լ�
	void ObjTrack::blur(int kernel);	// �̹��� ����
	void ObjTrack::colorHistogram();	// �÷� ���� ������׷� ��Ȱȭ (���)
	void ObjTrack::sharp();	// �̹��� ������ (���� ����)
	void ObjTrack::findContours();		// �ܰ��� ����
	cv::Mat ObjTrack::print_findContours();
	void ObjTrack::findObject();			// ������Ʈ�� ��ġ�� ����

	// �׸��� �Լ�
	cv::Mat ObjTrack::drawContours(cv::Mat src);	// �ҽ� ���� �ܰ��� �׸���
	cv::Mat ObjTrack::drawObject(cv::Mat src);		// �ҽ� ���� ������Ʈ ��ġ �׸���
};

