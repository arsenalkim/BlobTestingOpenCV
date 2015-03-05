#include "CvHistogram_Ji.h"

CCvHistogram_Ji::CCvHistogram_Ji(void)
{
}

CCvHistogram_Ji::~CCvHistogram_Ji(void)
{
}

void CCvHistogram_Ji::cvHistogram_Ji( const IplImage *src, float histo[256] )
{
	register int i, j;

	int W = src->width;
	int H = src->height;

	unsigned char* tmpBuf = new unsigned char[W * H];
	for( j = 0; j < H; j++ )
		for(i = 0; i < W; i++ )
			tmpBuf[j * W + i] = (unsigned char)(src->imageData[j * src->widthStep + i]);

	// Calc. Histogram
	int temp[256];
	memset(temp, 0, sizeof(int) * 256);
	for(j = 0; j < H; j++)
	{
		for(i = 0; i < W; i++)
		{
			temp[tmpBuf[j * W + i]]++;
		}
	}

	// histogram normalization
	float area = (float)(W * H);
	for(i = 0; i < 256; i++)
		histo[i] = temp[i] / area;
}

void CCvHistogram_Ji::cvHistogram_Ji( const IplImage *src, float histo[256], int W, int H )
{
	register int i, j;

	unsigned char* tmpBuf = new unsigned char[W * H];
	for( j = 0; j < H; j++ )
		for(i = 0; i < W; i++ )
			tmpBuf[j * W + i] = (unsigned char)(src->imageData[j * src->widthStep + i]);

	// Calc. Histogram
	int temp[256];
	memset(temp, 0, sizeof(int) * 256);
	for(j = 0; j < H; j++)
	{
		for(i = 0; i < W; i++)
		{
			temp[tmpBuf[j * W + i]]++;
		}
	}

	// histogram normalization
	float area = (float)(W * H);
	for(i = 0; i < 256; i++)
		histo[i] = temp[i] / area;
}
