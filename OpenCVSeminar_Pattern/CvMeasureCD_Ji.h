#pragma once

#include <opencv/cv.h>

class CCvMeasureCD_Ji
{
public:
	CCvMeasureCD_Ji(void);
	~CCvMeasureCD_Ji(void);

public:
	void cvMeasureCD_Ji(IplImage* img, int inspMode);

};
