////////////////////////////////////////////////////
//
//  클래스 : CBlobLabeling
//
//                    by 마틴(http://martinblog.net)
#pragma once

#include <opencv/cv.h>
#include <opencv/highgui.h>

#define _DEF_MAX_BLOBS		10000
#define OBJECT			      255		// GrayscalePixel 255 : White

typedef struct 
{
	bool	bVisitedFlag;
	CvPoint ptReturnPoint;				// 방문한 포인트(좌표)
} Visited;

class  CBlobLabeling
{
public:
	CBlobLabeling(void);
public:
	~CBlobLabeling(void);

public:
	IplImage*	m_Image;				// 레이블링을 위한 이미지
	int			m_nThresPixMax;		// 레이블링 스레스홀드 픽셀 수(Blob 인식 Pixel 수)
	int			m_nThresPixMin;
	Visited*	m_vPoint;				// 레이블링시 방문정보
	int			m_nBlobs;				// 레이블의 갯수
	CvRect*		m_recBlobs;				// 각 레이블 정보
	int         m_margin;

public:	
	// 레이블링 이미지 선택
	void SetParam(IplImage* image, int nThresPixMin, int nThresPixMax, int margin=0);

	// 레이블링(실행)
	void DoLabeling();

private:
	// 레이블링(동작)
	int Labeling(IplImage* image, int nThresPixMin, int nThresPixMax, int margin=0);

	// 포인트 초기화
	void InitvPoint(int nWidth, int nHeight);
	void DeletevPoint();

	// 레이블링 결과 얻기
	void DetectLabelingRegion(int nLabelNumber, unsigned char *DataBuf, int nWidth, int nHeight);

	// 레이블링(실제 알고리즘)
	int _Labeling(unsigned char *DataBuf, int nWidth, int nHeight, int nThresPixMin, int nThresPixMax, int margin);
	
	// _Labling 내부 사용 함수
	int		__NRFIndNeighbor(unsigned char *DataBuf, int nWidth, int nHeight, int nPosX, int nPosY, int *StartX, int *StartY, int *EndX, int *EndY );
	int		__Area(unsigned char *DataBuf, int StartX, int StartY, int EndX, int EndY, int nWidth, int nLevel);
};