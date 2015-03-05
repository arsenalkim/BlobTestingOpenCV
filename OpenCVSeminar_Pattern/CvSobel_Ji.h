#pragma once

#include <opencv/cv.h>


template<typename T>
inline T limit(const T& value)
{
	return ( (value > 255) ? 255 : ((value < 0) ? 0 : value) );
}

class CCvSobel_Ji
{
public:
	CCvSobel_Ji(void);
	~CCvSobel_Ji(void);

public:
	void cvSobel_Ji(const IplImage *src, IplImage *dst);
};
