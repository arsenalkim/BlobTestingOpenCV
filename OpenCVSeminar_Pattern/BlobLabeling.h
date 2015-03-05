////////////////////////////////////////////////////
//
//  Ŭ���� : CBlobLabeling
//
//                    by ��ƾ(http://martinblog.net)
#pragma once

#include <opencv/cv.h>
#include <opencv/highgui.h>

#define _DEF_MAX_BLOBS		10000
#define OBJECT			      255		// GrayscalePixel 255 : White

typedef struct 
{
	bool	bVisitedFlag;
	CvPoint ptReturnPoint;				// �湮�� ����Ʈ(��ǥ)
} Visited;

class  CBlobLabeling
{
public:
	CBlobLabeling(void);
public:
	~CBlobLabeling(void);

public:
	IplImage*	m_Image;				// ���̺��� ���� �̹���
	int			m_nThresPixMax;		// ���̺� ������Ȧ�� �ȼ� ��(Blob �ν� Pixel ��)
	int			m_nThresPixMin;
	Visited*	m_vPoint;				// ���̺��� �湮����
	int			m_nBlobs;				// ���̺��� ����
	CvRect*		m_recBlobs;				// �� ���̺� ����
	int         m_margin;

public:	
	// ���̺� �̹��� ����
	void SetParam(IplImage* image, int nThresPixMin, int nThresPixMax, int margin=0);

	// ���̺�(����)
	void DoLabeling();

private:
	// ���̺�(����)
	int Labeling(IplImage* image, int nThresPixMin, int nThresPixMax, int margin=0);

	// ����Ʈ �ʱ�ȭ
	void InitvPoint(int nWidth, int nHeight);
	void DeletevPoint();

	// ���̺� ��� ���
	void DetectLabelingRegion(int nLabelNumber, unsigned char *DataBuf, int nWidth, int nHeight);

	// ���̺�(���� �˰���)
	int _Labeling(unsigned char *DataBuf, int nWidth, int nHeight, int nThresPixMin, int nThresPixMax, int margin);
	
	// _Labling ���� ��� �Լ�
	int		__NRFIndNeighbor(unsigned char *DataBuf, int nWidth, int nHeight, int nPosX, int nPosY, int *StartX, int *StartY, int *EndX, int *EndY );
	int		__Area(unsigned char *DataBuf, int StartX, int StartY, int EndX, int EndY, int nWidth, int nLevel);
};