#include "BlobLabeling.h"


CBlobLabeling::CBlobLabeling(void)
{
	m_nThresPixMax = 0;
	m_nBlobs		  = _DEF_MAX_BLOBS;
	m_Image			  = NULL;
	m_recBlobs		  = NULL;

}

CBlobLabeling::~CBlobLabeling(void)
{
	if( m_Image != NULL )	cvReleaseImage( &m_Image );	

	if( m_recBlobs != NULL )
	{
		delete m_recBlobs;
		m_recBlobs = NULL;
	}
}

void CBlobLabeling::SetParam( IplImage* image, int nThresPixMin, int nThresPixMax, int margin/*=0*/ )
{
	if( m_recBlobs != NULL )
	{
		delete m_recBlobs;

		m_recBlobs	= NULL;
		m_nBlobs	= _DEF_MAX_BLOBS;
	}

	if( m_Image != NULL )	cvReleaseImage( &m_Image );

	m_Image			= cvCloneImage( image );

	m_nThresPixMax	= nThresPixMax;
	m_nThresPixMin  = nThresPixMin;
	m_margin        = margin;
}

void CBlobLabeling::DoLabeling()
{
	m_nBlobs = Labeling(m_Image, m_nThresPixMin, m_nThresPixMax, m_margin);
}

int CBlobLabeling::Labeling( IplImage* image, int nThresPixMin, int nThresPixMax, int margin/*=0*/ )
{
	if( image->nChannels != 1 ) 	return 0;		// only binary image, one channel

	int nNumber;		// ReturnValue : Labeling Count

	int nWidth	= image->width;
	int nHeight = image->height;

	unsigned char* tmpBuf = new unsigned char [nWidth * nHeight];

	register int i,j;

	// ���̺� ���� �ӽ� ������(�̹��� ������) ��(����) ����
	// ���̺� ���� ǥ��(numbering) �뵵
	for(j=0;j<nHeight;j++)
		for(i=0;i<nWidth ;i++)
			tmpBuf[j*nWidth+i] = (unsigned char)image->imageData[j*image->widthStep+i];

	// ���̺��� ���� ����Ʈ �ʱ�ȭ
	InitvPoint(nWidth, nHeight);

	// ���̺�
	nNumber = _Labeling(tmpBuf, nWidth, nHeight, nThresPixMin, nThresPixMax, margin);

	// ����Ʈ �޸� ����
	DeletevPoint();

	// ���̺� ���� ��ŭ Blob ��ǥ ���� ���� ����
	if( nNumber != _DEF_MAX_BLOBS )		m_recBlobs = new CvRect [nNumber];

	if( nNumber != 0 )	DetectLabelingRegion(nNumber, tmpBuf, nWidth, nHeight);

	for(j=0;j<nHeight;j++)
		for(i=0;i<nWidth ;i++)
			image->imageData[j*image->widthStep+i] = tmpBuf[j*nWidth+i];

	delete tmpBuf;
	return nNumber;
}

// m_vPoint �湮 ���� �ʱ�ȭ �Լ�
void CBlobLabeling::InitvPoint(int nWidth, int nHeight)
{
	int nX, nY;

	m_vPoint = new Visited [nWidth * nHeight];

	for(nY = 0; nY < nHeight; nY++)
	{
		for(nX = 0; nX < nWidth; nX++)
		{
			m_vPoint[nY * nWidth + nX].bVisitedFlag		= FALSE;
			m_vPoint[nY * nWidth + nX].ptReturnPoint.x	= nX;
			m_vPoint[nY * nWidth + nX].ptReturnPoint.y	= nY;
		}
	}
}

void CBlobLabeling::DeletevPoint()
{
	delete m_vPoint;
}

// Size�� nWidth�̰� nHeight�� DataBuf���� 
// nThreshold���� ���� ������ ������ �������� blob���� ȹ��
int CBlobLabeling::_Labeling( unsigned char *DataBuf, int nWidth, int nHeight, int nThresPixMin, int nThresPixMax, int margin )
{
	int Index = 0, num = 0;
	int nX, nY, k, l;
	int StartX , StartY, EndX , EndY;

	// Find connected components
	for(nY = 0; nY < nHeight; nY++)
	{
		for(nX = 0; nX < nWidth; nX++)
		{
			if(DataBuf[nY * nWidth + nX] == OBJECT)		// Is this a new component?, 255 == Object
			{
 				num++;

				DataBuf[nY * nWidth + nX] = num;

				StartX = nX, StartY = nY, EndX = nX, EndY= nY;

				__NRFIndNeighbor(DataBuf, nWidth, nHeight, nX, nY, &StartX, &StartY, &EndX, &EndY);

				// nThresholdPixel ���� ���� Blob �� �ȼ� �� 0 ó��
				// labeling number marking ���
				int nPixelArea = __Area(DataBuf, StartX, StartY, EndX, EndY, nWidth, num);
				if( nPixelArea < nThresPixMin || nPixelArea > nThresPixMax  )
				{
					for(k = StartY; k <= EndY; k++)
					{
						for(l = StartX; l <= EndX; l++)
						{
							if(DataBuf[k * nWidth + l] == num)
								DataBuf[k * nWidth + l] = 0;
						}
					}
					--num;

					// because of, OBJECT == 255
					// nThresholdPixel ������ blob ������ 250 �̻��� �� 0 ��ȯ
					if(num > 250)
					{
						printf("_Labeling num > 250\n");
						return  0;
					}
				}
				else if (StartY < margin || EndY > nHeight-margin || StartX < margin || EndX > nWidth-margin)		// margin
				{
					for(k = StartY; k <= EndY; k++)
					{
						for(l = StartX; l <= EndX; l++)
						{
							if(DataBuf[k * nWidth + l] == num)
								DataBuf[k * nWidth + l] = 0;
						}
					}
					--num;

					// because of, OBJECT == 255
					// nThresholdPixel ������ blob ������ 250 �̻��� �� 0 ��ȯ
					if(num > 250)
					{
						printf("_Labeling num > 250\n");
						return  0;
					}
				}
/*				printf("num : %d\n", num);*/
			}
		}
	}

	return num;	
}

// Blob labeling�ؼ� ����� ����� rec�� �� 
void CBlobLabeling::DetectLabelingRegion(int nLabelNumber, unsigned char *DataBuf, int nWidth, int nHeight)
{
	int nX, nY;
	int nLabelIndex ;

	bool bFirstFlag[255] = {FALSE,};

	for(nY = 1; nY < nHeight - 1; nY++)
	{
		for(nX = 1; nX < nWidth - 1; nX++)
		{
			nLabelIndex = DataBuf[nY * nWidth + nX];

			if(nLabelIndex != 0)	// Is this a new component?, 255 == Object
			{
				if(bFirstFlag[nLabelIndex] == FALSE)
				{
					m_recBlobs[nLabelIndex-1].x			= nX;
					m_recBlobs[nLabelIndex-1].y			= nY;
					m_recBlobs[nLabelIndex-1].width		= 0;
					m_recBlobs[nLabelIndex-1].height	= 0;

					bFirstFlag[nLabelIndex] = TRUE;
				}
				else
				{
					int left	= m_recBlobs[nLabelIndex-1].x;
					int right	= left + m_recBlobs[nLabelIndex-1].width;
					int top		= m_recBlobs[nLabelIndex-1].y;
					int bottom	= top + m_recBlobs[nLabelIndex-1].height;

					if( left   >= nX )	left	= nX;
					if( right  <= nX )	right	= nX;
					if( top    >= nY )	top		= nY;
					if( bottom <= nY )	bottom	= nY;

					m_recBlobs[nLabelIndex-1].x			= left;
					m_recBlobs[nLabelIndex-1].y			= top;
					m_recBlobs[nLabelIndex-1].width		= right - left;
					m_recBlobs[nLabelIndex-1].height	= bottom - top;

				}
			}

		}
	}

}

// Blob Labeling�� ���� ���ϴ� function
// 2000�� ����ó����ȸ�� �Ǹ� �� ����
// *StartX, *StartY, *EndX, *EndY : Call by Reference
int CBlobLabeling::__NRFIndNeighbor(unsigned char *DataBuf, int nWidth, int nHeight, int nPosX, int nPosY, int *StartX, int *StartY, int *EndX, int *EndY )
{
	CvPoint CurrentPoint;

	CurrentPoint.x = nPosX;
	CurrentPoint.y = nPosY;

	m_vPoint[CurrentPoint.y * nWidth +  CurrentPoint.x].bVisitedFlag    = TRUE;
	m_vPoint[CurrentPoint.y * nWidth +  CurrentPoint.x].ptReturnPoint.x = nPosX;
	m_vPoint[CurrentPoint.y * nWidth +  CurrentPoint.x].ptReturnPoint.y = nPosY;

	while(1)
	{
		if( (CurrentPoint.x != 0) && (DataBuf[CurrentPoint.y * nWidth + CurrentPoint.x - 1] == OBJECT) )   // ó�� X ��ǥ ����, -X ����(Left)
		{
			if( m_vPoint[CurrentPoint.y * nWidth + CurrentPoint.x - 1].bVisitedFlag == FALSE )
			{
				DataBuf[CurrentPoint.y  * nWidth + CurrentPoint.x - 1]				 = DataBuf[CurrentPoint.y * nWidth + CurrentPoint.x];	// If so, mark it
				m_vPoint[CurrentPoint.y * nWidth + CurrentPoint.x - 1].bVisitedFlag	 = TRUE;
				m_vPoint[CurrentPoint.y * nWidth + CurrentPoint.x - 1].ptReturnPoint = CurrentPoint;
				CurrentPoint.x--;

				// X pos. Start Limit
				if(CurrentPoint.x <= 0)
					CurrentPoint.x = 0;

				if(*StartX >= CurrentPoint.x)
					*StartX = CurrentPoint.x;				

				continue;
			}
		}

		if( (CurrentPoint.x != nWidth - 1) && (DataBuf[CurrentPoint.y * nWidth + CurrentPoint.x + 1] == OBJECT) )   // ������ X ��ǥ ����, +X ����(Right)
		{
			if( m_vPoint[CurrentPoint.y * nWidth + CurrentPoint.x + 1].bVisitedFlag == FALSE )
			{
				DataBuf[CurrentPoint.y * nWidth + CurrentPoint.x + 1]				 = DataBuf[CurrentPoint.y * nWidth + CurrentPoint.x];	// If so, mark it
				m_vPoint[CurrentPoint.y * nWidth + CurrentPoint.x + 1].bVisitedFlag	 = TRUE;
				m_vPoint[CurrentPoint.y * nWidth + CurrentPoint.x + 1].ptReturnPoint = CurrentPoint;
				CurrentPoint.x++;

				// X pos. End Limit
				if(CurrentPoint.x >= nWidth - 1)
					CurrentPoint.x = nWidth - 1;

				if(*EndX <= CurrentPoint.x)
					*EndX = CurrentPoint.x;

				continue;
			}
		}

		if( (CurrentPoint.y != 0) && (DataBuf[(CurrentPoint.y - 1) * nWidth + CurrentPoint.x] == OBJECT) )   // ó�� y ��ǥ ����, -y ����(Top)
		{
			if( m_vPoint[(CurrentPoint.y - 1) * nWidth +  CurrentPoint.x].bVisitedFlag == FALSE )
			{
				DataBuf[(CurrentPoint.y - 1) * nWidth + CurrentPoint.x]					= DataBuf[CurrentPoint.y * nWidth + CurrentPoint.x];	// If so, mark it
				m_vPoint[(CurrentPoint.y - 1) * nWidth +  CurrentPoint.x].bVisitedFlag	= TRUE;
				m_vPoint[(CurrentPoint.y - 1) * nWidth +  CurrentPoint.x].ptReturnPoint = CurrentPoint;
				CurrentPoint.y--;

				if(CurrentPoint.y <= 0)
					CurrentPoint.y = 0;

				if(*StartY >= CurrentPoint.y)
					*StartY = CurrentPoint.y;

				continue;
			}
		}

		if( (CurrentPoint.y != nHeight - 1) && (DataBuf[(CurrentPoint.y + 1) * nWidth + CurrentPoint.x] == OBJECT) )   // ������ y ��ǥ ����, +y ����(Bottom)
		{
			if( m_vPoint[(CurrentPoint.y + 1) * nWidth +  CurrentPoint.x].bVisitedFlag == FALSE )
			{
				DataBuf[(CurrentPoint.y + 1) * nWidth + CurrentPoint.x]					= DataBuf[CurrentPoint.y * nWidth + CurrentPoint.x];	// If so, mark it
				m_vPoint[(CurrentPoint.y + 1) * nWidth +  CurrentPoint.x].bVisitedFlag	= TRUE;
				m_vPoint[(CurrentPoint.y + 1) * nWidth +  CurrentPoint.x].ptReturnPoint = CurrentPoint;
				CurrentPoint.y++;

				if(CurrentPoint.y >= nHeight - 1)
					CurrentPoint.y = nHeight - 1;

				if(*EndY <= CurrentPoint.y)
					*EndY = CurrentPoint.y;

				continue;
			}
		}


		// ��, ��, ��, �� ��� �ش� ���� ���
		if(		(CurrentPoint.x == m_vPoint[CurrentPoint.y * nWidth + CurrentPoint.x].ptReturnPoint.x) 
			&&	(CurrentPoint.y == m_vPoint[CurrentPoint.y * nWidth + CurrentPoint.x].ptReturnPoint.y) )
		{
			break;
		}
		else
		{
			CurrentPoint = m_vPoint[CurrentPoint.y * nWidth + CurrentPoint.x].ptReturnPoint;
		}
	}

	return 0;
}

// ������ ���� blob�� Į�� ���� ������ ũ�⸦ ȹ��, ����(pixel ����)
int CBlobLabeling::__Area(unsigned char *DataBuf, int StartX, int StartY, int EndX, int EndY, int nWidth, int nLevel)
{
	int nArea = 0;
	int nX, nY;

	for (nY = StartY; nY < EndY; nY++)
		for (nX = StartX; nX < EndX; nX++)
			if (DataBuf[nY * nWidth + nX] == nLevel)
				++nArea;

	return nArea;
}