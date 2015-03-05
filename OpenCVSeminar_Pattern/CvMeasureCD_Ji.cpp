#include "CvMeasureCD_Ji.h"

CCvMeasureCD_Ji::CCvMeasureCD_Ji(void)
{
}

CCvMeasureCD_Ji::~CCvMeasureCD_Ji(void)
{
}

void CCvMeasureCD_Ji::cvMeasureCD_Ji( IplImage* img, int inspMode )
{
	int nW = img->width;
	int nH = img->height;
	unsigned char* tmpBuf = new unsigned char[nW * nH];
	for(register int j = 0; j < nH; j++ )
		for(register int i = 0; i < nW; i++ )
			tmpBuf[j*nW+i] = (unsigned char)img->imageData[j * img->widthStep + i];


	float totalCDsize = 0;
	int failMeasure = 0;
	int min_CDsize = 50;
	int max_CDsize = 50;

	

}
