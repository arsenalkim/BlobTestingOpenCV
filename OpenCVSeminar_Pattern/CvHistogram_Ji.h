#pragma once

#include <opencv/cv.h>

class CCvHistogram_Ji
{
public:
	CCvHistogram_Ji(void);
	~CCvHistogram_Ji(void);

public:
	void cvHistogram_Ji(const IplImage *src, float histo[256]);
	void cvHistogram_Ji(const IplImage *src, float histo[256], int W, int H);
};
