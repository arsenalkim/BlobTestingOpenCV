#include "CvSobel_Ji.h"

CCvSobel_Ji::CCvSobel_Ji(void)
{
}

CCvSobel_Ji::~CCvSobel_Ji(void)
{
}

void CCvSobel_Ji::cvSobel_Ji( const IplImage *src, IplImage *dst )
{
	register int i, j;

	int W = src->width;
	int H = src->height;		

	unsigned char* tmpBuf = new unsigned char[W * H];
	for( j = 0; j < H; j++ )
		for(i = 0; i < W; i++ )
			tmpBuf[j*W+i] = (unsigned char)(src->imageData[j*src->widthStep+i]);

	int h1, h2;
	double hval;
	for( j = 1 ; j < H-1 ; j++ )
	{
		for( i = 1 ; i < W-1 ; i++ )
		{
			h1 = - tmpBuf[(j-1)*W+(i-1)] - 2*tmpBuf[(j-1)*W+(i)] - tmpBuf[(j-1)*W+(i+1)]
		         + tmpBuf[(j+1)*W+(i-1)] + 2*tmpBuf[(j+1)*W+(i)] + tmpBuf[(j+1)*W+(i+1)];

			h2 = - tmpBuf[(j-1)*W+(i-1)] - 2*tmpBuf[(j)*W+(i-1)] - tmpBuf[(j+1)*W+(i-1)]
			     + tmpBuf[(j-1)*W+(i+1)] + 2*tmpBuf[(j)*W+(i+1)] + tmpBuf[(j+1)*W+(i+1)];

			hval = sqrt( (double)h1*h1 + h2*h2 );

			dst->imageData[j*dst->widthStep+i] = (unsigned char)( limit(hval) );
		}
	}

	delete tmpBuf;
}
