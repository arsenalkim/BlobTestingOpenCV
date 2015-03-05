#include <iostream>
#include <fstream>

#include <opencv/cv.h>
#include <opencv/highgui.h>

#include "BlobLabeling.h"
#include "CvSobel_Ji.h"
#include "CvHistogram_Ji.h"

using namespace cv;
using namespace std;

 
const char* filename = "seoul_s21.bmp";

IplImage*     g_image     = NULL;
IplImage*     g_drawImage = NULL;
IplImage*	  g_rotImage  = NULL;

IplImage*     g_gray	  = NULL;
IplImage*     g_binary	  = NULL;
IplImage*     g_invbinary = NULL;
IplImage*	  g_dstImage  = NULL;
IplImage*     g_sobelImage = NULL;
IplImage*     g_drawSobelImage = NULL;
int			  g_threshold = 115;
int			  g_sbThreshold = 80;
int           g_thresPixMin = 1000;
int			  g_thresPixMax = 4000;
int			  g_labelingMargin = 30;
int			  g_intvAlgNgb = 80;					// Interval
int           g_idxAlgNgb = 2;						// AlignNeighboredPattern Index (0~5)

int			  g_dwMVPsb = 155;						// Down Measure value of Sobel pixel
int			  g_upMVPsb = 155;						// Up Measure value of Sobel pixel
int			  g_lfMVPsb = 150;						// Left Measure value of Sobel pixel
int			  g_rgMVPsb = 150;						// Right Measure value of Sobel pixel







void on_trackbar(int pos)
{
	g_gray    = cvCreateImage( cvGetSize(g_image), 8, 1 );
	g_binary  = cvCreateImage( cvGetSize(g_image), 8, 1 );

	cvCopy( g_image, g_gray );
	cvThreshold( g_gray, g_gray, g_threshold, 255, CV_THRESH_BINARY);
	//cvAdaptiveThreshold( g_gray, g_gray, 255, CV_ADAPTIVE_THRESH_MEAN_C, CV_THRESH_BINARY, 55, 2 );
	cvCopy( g_gray, g_binary );
	cvZero( g_gray );

	cvShowImage( "Binary", g_binary );
}

int main(void)
{
	// Seq 1. Initialize
	char path[255];
	strcpy(path, "C:/Users/jskim/Documents/Visual Studio 2008/Projects/OpenCVSeminar_Pattern/OpenCVSeminar_Pattern/ProcessingImg/");
	char tmpPath[255];
	strcpy(tmpPath, path);

	g_image = cvLoadImage(filename, CV_LOAD_IMAGE_UNCHANGED);	// Depth : 8bit, Grayscale Image(1ch)
	g_drawImage = cvLoadImage(filename, CV_LOAD_IMAGE_COLOR);		// Depth : 24bit, Color Image(3ch)


	// Seq 2. Analysis, Histogram
	float histo[256] = {0.f, };
	CCvHistogram_Ji chisto;
	chisto.cvHistogram_Ji(g_image, histo, 50, 50);
		
	float max_value = histo[0];
	int max_value_idx = 0;
	for(register int i = 0 ; i < 256 ; i++ ) 
	{
		if( histo[i] > max_value )
		{
			max_value_idx = i;
			max_value = histo[i];
		}
	}
	
	
	// Seq 3. Threshold
	cvNamedWindow( "PatternImage", CV_WINDOW_AUTOSIZE );
	cvCreateTrackbar( "Threshold", "PatternImage", &g_threshold, 255, on_trackbar );
	on_trackbar(0);
	cvWaitKey(0);

	// Threshold_Inverse
	g_invbinary = cvCreateImage( cvGetSize(g_image), 8, 1 );
	cvCopy(g_image, g_invbinary);
	cvThreshold( g_binary, g_invbinary, 1, 255, CV_THRESH_BINARY_INV );		
	
	strcat(tmpPath, "1_InvBinary.bmp");
	cvSaveImage( tmpPath, g_invbinary );
	strcpy(tmpPath, path);

// 	cvShowImage( "InverseBinaryImage", g_invbinary );
// 	cvWaitKey(0);



	// Seq 4. Sobel derivative
	g_sobelImage = cvCreateImage( cvGetSize(g_image), IPL_DEPTH_8U, 1 );
	CCvSobel_Ji csobel;
	csobel.cvSobel_Ji(g_image, g_sobelImage);
	
	strcat(tmpPath, "2_Sobel.bmp");
	cvSaveImage( tmpPath, g_sobelImage );
	strcpy(tmpPath, path);
	
// 	cvShowImage("2_Sobel", g_sobelImage);
// 	waitKey(0);




	// Seq 5. Labeling-1
	int cX, cY;								// Center X, Y
	int Alg_cX, Alg_cY;						// AlignCenter X, Y
	int AlgNgb_cX[6], AlgNgb_cY[6];			// AlignNeighboredCenter X, Y
	int nAlg_Label;							// AlignCenter X, Y Label Number
	
	CBlobLabeling BeforeBlob;
	BeforeBlob.SetParam(g_invbinary, g_thresPixMin, g_thresPixMax, g_labelingMargin+200);	// 레이블링 할 이미지와 최소 픽셀수 등을 설정
	BeforeBlob.DoLabeling();

	nAlg_Label = BeforeBlob.m_nBlobs / 2;	
	Alg_cX = BeforeBlob.m_recBlobs[nAlg_Label].x + (BeforeBlob.m_recBlobs[nAlg_Label].width / 2);
	Alg_cY = BeforeBlob.m_recBlobs[nAlg_Label].y + (BeforeBlob.m_recBlobs[nAlg_Label].height / 2);

	int j = 0;
	for( register int i=0; i < BeforeBlob.m_nBlobs; i++ )
	{		
		CvPoint	pt1 = cvPoint(BeforeBlob.m_recBlobs[i].x, BeforeBlob.m_recBlobs[i].y);
		CvPoint pt2 = cvPoint(pt1.x + BeforeBlob.m_recBlobs[i].width, pt1.y + BeforeBlob.m_recBlobs[i].height);		
		cX = BeforeBlob.m_recBlobs[i].x + (BeforeBlob.m_recBlobs[i].width / 2);
		cY = BeforeBlob.m_recBlobs[i].y + (BeforeBlob.m_recBlobs[i].height / 2);
		CvPoint pt3 = cvPoint(cX, cY);
		cvDrawCircle(g_drawImage, pt3, 2, CV_RGB(0, 0, 255));				// Center Axis.

		if(nAlg_Label == i)		
			cvDrawRect(g_drawImage, pt1, pt2, CV_RGB(0, 255, 0), 1);		// Align Center Pattern	
		else if( (cX > Alg_cX - g_intvAlgNgb) && (cX < Alg_cX + g_intvAlgNgb) && (cY > Alg_cY - g_intvAlgNgb) && (cY < Alg_cY + g_intvAlgNgb) && j < 6 )
		{
			AlgNgb_cX[j] = cX;
			AlgNgb_cY[j] = cY;
			if(j == g_idxAlgNgb)
				cvDrawRect(g_drawImage, pt1, pt2, CV_RGB(0, 0, 255), 1);	// Align Neighbored Pattern	
			j++;
		}
		else
			cvDrawRect(g_drawImage, pt1, pt2, CV_RGB(255, 0, 0), 1);		// 	the others
	}

	strcat(tmpPath, "3_Labeling-1.bmp");
	cvSaveImage( tmpPath, g_drawImage );
	strcpy(tmpPath, path);
/*	cvShowImage("Labeling-1", g_drawImage);*/
	cout << "Labeling total count : " << BeforeBlob.m_nBlobs << endl;
/*	waitKey(0);*/




	// Seq 6. Align+30'
	int leg_absX = abs(Alg_cX - AlgNgb_cX[g_idxAlgNgb]);
	int leg_absY = abs(Alg_cY - AlgNgb_cY[g_idxAlgNgb]);	

	float bottom, height;

	// theta - : CW / + : CCW
	//       - : leg_x / + : leg_y
	int ccw =  1;
	int cw  = -1;
	int SHIFT;
	int T_alpha;						// AlignTheta

  	if(leg_absX > leg_absY)
	{
		bottom = leg_absX;
		height = leg_absY;

		if( AlgNgb_cX[g_idxAlgNgb] > Alg_cX )
		{
			if( AlgNgb_cY[g_idxAlgNgb] < Alg_cY )
			{
				SHIFT = cw;
				T_alpha = 0;					// 1 - quadrant, 1/8
			}
			else
			{
				SHIFT = ccw;
				T_alpha = 0;				// 4 - quadrant, 8/8
			}
		}
		else if( AlgNgb_cX[g_idxAlgNgb] <= Alg_cX )
		{
			if( AlgNgb_cY[g_idxAlgNgb] < Alg_cY )	
			{
				SHIFT = ccw; 
				T_alpha = 0;				// 2 - quadrant, 4/8
			}
			else
			{
				SHIFT = cw;
				T_alpha = 0;					// 3 - quadrant, 5/8
			}
		}		
	}
	else
	{
 		bottom = leg_absY;
		height = leg_absX;

		if( AlgNgb_cY[g_idxAlgNgb] < Alg_cY )	// 1, 2 - quadrant
		{
			if( AlgNgb_cX[g_idxAlgNgb] > Alg_cX )
			{
				SHIFT = ccw;
				T_alpha = 30;					// 1 - quadrant, 2/8
			}
			else
			{
				SHIFT = cw;
				T_alpha = 30;				// 4 - quadrant, 3/8
			}
		}
		else if( AlgNgb_cY[g_idxAlgNgb] >= Alg_cY )	// 3, 4 - quadrant
		{
			if( AlgNgb_cX[g_idxAlgNgb] > Alg_cX )
			{
				SHIFT = ccw;
				T_alpha = 30;				// 2 - quadrant, 7/8
			}
			else
			{
				SHIFT = cw; 
				T_alpha = 30;					// 3 - quadrant, 6/8
			}
		}

	}

	//double hytenuse = hypot (bottom, height);
	float radian = height / bottom;
	double theta = atanf(radian) * 180 / 3.14;
	theta = theta * SHIFT + T_alpha;

	cout << "theta : " << theta << endl;

	// WarpAffine - rotation
	CvPoint2D32f center = cvPoint2D32f(g_image->width/2, g_image->height/2);
	double angle = theta;
	double scale = 1;
	CvMat* rot_mat = cvCreateMat(2, 3, CV_32FC1);
	cv2DRotationMatrix(center, angle, scale, rot_mat);
	
// 	cvWarpAffine(g_image, g_image, rot_mat, CV_WARP_FILL_OUTLIERS, CV_RGB(255, 255, 255));
// 	cvWarpAffine(g_drawImage, g_drawImage, rot_mat, CV_WARP_FILL_OUTLIERS, CV_RGB(255, 255, 255));
// 	cvWarpAffine(g_sobelImage, g_sobelImage, rot_mat, CV_WARP_FILL_OUTLIERS, CV_RGB(255, 255, 255));
	cvWarpAffine(g_image, g_image, rot_mat);
	cvWarpAffine(g_drawImage, g_drawImage, rot_mat);
	cvWarpAffine(g_sobelImage, g_sobelImage, rot_mat);

	strcat(tmpPath, "4_AlignPattern.bmp");
	cvSaveImage( tmpPath, g_drawImage );
	strcpy(tmpPath, path);
/*	cvShowImage("4_AlignPattern", g_drawImage);	*/
	
	strcat(tmpPath, "5_AlignSobel.bmp");
	cvSaveImage( tmpPath, g_sobelImage );
	strcpy(tmpPath, path);
// 	cvShowImage("5_AlignSobel", g_sobelImage);
// 	waitKey(0);	





	// Seq 7. Labeling-2
	cvCopy(g_image, g_gray);
	cvThreshold( g_gray, g_gray, g_threshold, 255, CV_THRESH_BINARY );
	cvCopy( g_gray, g_binary );
	cvZero( g_gray );

	cvThreshold( g_binary, g_invbinary, 1, 255, CV_THRESH_BINARY_INV );

	CBlobLabeling AfterBlob;
	AfterBlob.SetParam(g_invbinary, g_thresPixMin, g_thresPixMax, g_labelingMargin);
	AfterBlob.DoLabeling();

	ofstream output;
	output.open("CDsizedata.csv", ios::out);
	output << "1_Labeling Total," << AfterBlob.m_nBlobs << endl;
	output.close();




	// Seq 8. Measure to Size - 1st
	// Sobel Binary - WHITE : edge section 
	cvThreshold(g_sobelImage, g_sobelImage, g_sbThreshold, 255, CV_THRESH_BINARY);

	strcat(tmpPath, "6_AlignSobelBinary.bmp");
	cvSaveImage( tmpPath, g_sobelImage );
	strcpy(tmpPath, path);

// 	cvShowImage("SobelBinary", g_sobelImage);
// 	waitKey(0);

	int nW = g_sobelImage->width;
	int nH = g_sobelImage->height;
	unsigned char* tmpBuf = new unsigned char[nW * nH];
	for( j = 0; j < nH; j++ )
		for(register int i = 0; i < nW; i++ )
			tmpBuf[j * nW + i] = (unsigned char)g_sobelImage->imageData[j * g_sobelImage->widthStep + i];


	int imgWidth = g_image->width;	
	float totalCDsize = 0;
	int failMeasure = 0;
	int min_CDsize = 50;
	int max_CDsize = 50;

	cvZero(g_drawImage);
	cvCvtColor(g_image, g_drawImage, CV_GRAY2RGB);
	g_drawSobelImage  = cvCreateImage( cvGetSize(g_sobelImage), 8, 1 );
	cvCopy(g_sobelImage, g_drawSobelImage);

	for( register int i=0; i < AfterBlob.m_nBlobs; i++ )
	{
		// CDsize = 'down + up' or 'left + right'
		int CDsize = 0;
		int dSize = 0, uSize = 0;
		int lSize = 0, rSize = 0;

		// Labeling Center Axis(= criteria)
		cX = AfterBlob.m_recBlobs[i].x + (AfterBlob.m_recBlobs[i].width / 2);
		cY = AfterBlob.m_recBlobs[i].y + (AfterBlob.m_recBlobs[i].height / 2);
		CvPoint	pt1 = cvPoint(cX, cY);
		CvPoint pt2 = cvPoint(cX, cY);

		//cvDrawCircle(g_sobelImage, pt1, 2, CV_RGB(0, 0, 255));				// Center Axis.
	
		// temp Y & sizing Y
		int tmpY = cY;
		int szY = 0;

		int idx = tmpY * imgWidth + cX;
		int imgData = tmpBuf[idx];
		bool bFlag = true;
	
		// sizing Y - down		
		while( imgData != 0 || bFlag == true && szY < 70)
		{
			szY++;
			tmpY++;			

			idx = tmpY * imgWidth + cX;
			imgData = tmpBuf[idx];
			if(imgData == 255)
				bFlag = false;
		} 

		dSize = szY;
		pt2 = cvPoint(cX, tmpY-1);


		cvDrawLine(g_drawImage, pt1, pt2, CV_RGB(255, 255, 0), 1);
		cvDrawLine(g_drawSobelImage, pt1, pt2, CV_RGB(200, 200, 200), 1);

		// sizing Y - up
		tmpY = cY-1;
		szY = 0;

		idx = tmpY * imgWidth + cX;
		imgData = tmpBuf[idx];
		bFlag = true;

		while(  imgData != 0 || bFlag == true && szY < 70)
		{			
			szY++;
			tmpY--;

			idx = tmpY * imgWidth + cX;								
			imgData = tmpBuf[idx];	
			if(imgData == 255)
				bFlag = false;
		}
		uSize = szY;
		pt2 = cvPoint(cX, tmpY+1);


		cvDrawLine(g_drawImage, pt1, pt2, CV_RGB(255, 128, 128), 1);
		cvDrawLine(g_drawSobelImage, pt1, pt2, CV_RGB(110, 110, 110), 1);				

		CDsize = dSize + uSize;
		totalCDsize = totalCDsize + CDsize;

		if(min_CDsize > CDsize)
			min_CDsize = CDsize;
		else if(max_CDsize < CDsize)
			max_CDsize = CDsize;

		output.open("CDsizedata.csv", ios::app);
		output << i << ",size," << CDsize << "," << dSize << "," << uSize << endl;
		output.close();
		// 		cout << i << " size : " << CDsize << endl;
		
	}
	strcat(tmpPath, "6_MeasureSobel.bmp");
	cvSaveImage( tmpPath, g_drawSobelImage );
	strcpy(tmpPath, path);
	/*	cvShowImage("Mesaure Size_Sobel", g_drawSobelImage);*/

	strcat(tmpPath, "7_MeasurePattern.bmp");
	cvSaveImage( tmpPath, g_drawImage );
	strcpy(tmpPath, path);
	cvShowImage("Measure Size", g_drawImage);
	/*	cvWaitKey(0);*/

	output.open("CDsizedata.csv", ios::app);
	output << "Fail to Measure," << failMeasure << endl;	
	output << "Average," << totalCDsize / (AfterBlob.m_nBlobs-failMeasure) << ",0.0465," << totalCDsize / (AfterBlob.m_nBlobs-failMeasure) * 0.0465 << endl;	
	output.close();
	// 	cout << "avg. : " << CDsizeTotal / (AfterBlob.m_nBlobs-failMeasure) * 0.0465 << endl;



	// Seq 9. Rotation CW(-) 60'
	// WarpAffine - rotation
	center = cvPoint2D32f(g_image->width/2, g_image->height/2);
	angle = -60;
	scale = 1;	
	cv2DRotationMatrix(center, angle, scale, rot_mat);

// 	cvWarpAffine(g_image, g_image, rot_mat, CV_WARP_FILL_OUTLIERS, CV_RGB(255, 255, 255));
// 	cvWarpAffine(g_sobelImage, g_sobelImage, rot_mat, CV_WARP_FILL_OUTLIERS, CV_RGB(255, 255, 255));
	cvWarpAffine(g_image, g_image, rot_mat);
	cvWarpAffine(g_sobelImage, g_sobelImage, rot_mat);

	strcat(tmpPath, "8_Rotation60.bmp");
	cvSaveImage( tmpPath, g_image );
	strcpy(tmpPath, path);
// 	cvShowImage("8_Rotation60", g_image);
// 	waitKey(0);

	// Seq 10. Labeling-3
	cvCopy(g_image, g_gray);
	cvThreshold( g_gray, g_gray, g_threshold, 255, CV_THRESH_BINARY );
	cvCopy( g_gray, g_binary );
	cvZero( g_gray );

	cvThreshold( g_binary, g_invbinary, 1, 255, CV_THRESH_BINARY_INV );
	CBlobLabeling labeling_3;
	labeling_3.SetParam(g_invbinary, g_thresPixMin, g_thresPixMax, g_labelingMargin);		
	labeling_3.DoLabeling();

	
	output.open("CDsizedata.csv", ios::app);
	output << "2_labeling_Total," << labeling_3.m_nBlobs << endl;
	output.close();


	// Seq 8. Measure to Size
	// Sobel Binary - WHITE : edge section 
	cvThreshold(g_sobelImage, g_sobelImage, g_sbThreshold, 255, CV_THRESH_BINARY);

	strcat(tmpPath, "9_R60_AlignSobelBinary.bmp");
	cvSaveImage( tmpPath, g_sobelImage );
	strcpy(tmpPath, path);

	// 	cvShowImage("SobelBinary", g_sobelImage);
	// 	waitKey(0);

	nW = g_sobelImage->width;
	nH = g_sobelImage->height;
	tmpBuf = new unsigned char[nW * nH];
	for( j = 0; j < nH; j++ )
		for(register int i = 0; i < nW; i++ )
			tmpBuf[j * nW + i] = (unsigned char)g_sobelImage->imageData[j * g_sobelImage->widthStep + i];


	imgWidth = g_image->width;	
	totalCDsize = 0;
	failMeasure = 0;
	min_CDsize = 50;
	max_CDsize = 50;

	cvZero(g_drawImage);
	cvCvtColor(g_image, g_drawImage, CV_GRAY2RGB);
	cvZero(g_drawSobelImage);
	cvCopy(g_sobelImage, g_drawSobelImage);


	for( register int i=0; i < labeling_3.m_nBlobs; i++ )
	{
		// CDsize = 'down + up' or 'left + right'
		int CDsize = 0;
		int dSize = 0, uSize = 0;
		int lSize = 0, rSize = 0;

		// Labeling Center Axis(= criteria)
		cX = labeling_3.m_recBlobs[i].x + (labeling_3.m_recBlobs[i].width / 2);
		cY = labeling_3.m_recBlobs[i].y + (labeling_3.m_recBlobs[i].height / 2);
		CvPoint	pt1 = cvPoint(cX, cY);
		CvPoint pt2 = cvPoint(cX, cY);

		//cvDrawCircle(g_drawSobelImage, pt1, 2, CV_RGB(0, 0, 255));				// Center Axis.

		// temp Y & sizing Y
		int tmpY = cY;
		int szY = 0;

		int idx = tmpY * imgWidth + cX;
		int imgData = tmpBuf[idx];
		bool bFlag = true;

		// sizing Y - down		
		while( imgData != 0 || bFlag == true && szY < 70)
		{
			szY++;
			tmpY++;			

			idx = tmpY * imgWidth + cX;
			imgData = tmpBuf[idx];
			if(imgData == 255)
				bFlag = false;
		} 

		dSize = szY;
		pt2 = cvPoint(cX, tmpY-1);


		cvDrawLine(g_drawImage, pt1, pt2, CV_RGB(255, 255, 0), 1);
		cvDrawLine(g_drawSobelImage, pt1, pt2, CV_RGB(200, 200, 200), 1);

		// sizing Y - up
		tmpY = cY-1;
		szY = 0;

		idx = tmpY * imgWidth + cX;
		imgData = tmpBuf[idx];
		bFlag = true;

		while(  imgData != 0 || bFlag == true && szY < 70)
		{			
			szY++;
			tmpY--;

			idx = tmpY * imgWidth + cX;								
			imgData = tmpBuf[idx];	
			if(imgData == 255)
				bFlag = false;
		}
		uSize = szY;
		pt2 = cvPoint(cX, tmpY+1);


		cvDrawLine(g_drawImage, pt1, pt2, CV_RGB(255, 128, 128), 1);
		cvDrawLine(g_drawSobelImage, pt1, pt2, CV_RGB(110, 110, 110), 1);				

		CDsize = dSize + uSize;
		totalCDsize = totalCDsize + CDsize;

		if(min_CDsize > CDsize)
			min_CDsize = CDsize;
		else if(max_CDsize < CDsize)
			max_CDsize = CDsize;

		output.open("CDsizedata.csv", ios::app);
		output << i << ",size," << CDsize << "," << dSize << "," << uSize << endl;
		output.close();
		// 		cout << i << " size : " << CDsize << endl;

	}
	strcat(tmpPath, "10_R60_MeasureSobel.bmp");
	cvSaveImage( tmpPath, g_drawSobelImage );
	strcpy(tmpPath, path);
	/*	cvShowImage("Mesaure Size_Sobel", g_drawSobelImage);*/

	strcat(tmpPath, "11_R60_MeasurePattern.bmp");
	cvSaveImage( tmpPath, g_drawImage );
	strcpy(tmpPath, path);
	cvShowImage("Measure Size", g_drawImage);
	/*	cvWaitKey(0);*/

	output.open("CDsizedata.csv", ios::app);
	output << "Fail to Measure," << failMeasure << endl;	
	output << "Average," << totalCDsize / (labeling_3.m_nBlobs-failMeasure) << ",0.0465," << totalCDsize / (labeling_3.m_nBlobs-failMeasure) * 0.0465 << endl;	
	output.close();
	// 	cout << "avg. : " << CDsizeTotal / (AfterBlob.m_nBlobs-failMeasure) * 0.0465 << endl;



	// Seq 9. Rotation CW(-) 60' + 60'
	// WarpAffine - rotation
	center = cvPoint2D32f(g_image->width/2, g_image->height/2);
	angle = -60;
	scale = 1;	
	cv2DRotationMatrix(center, angle, scale, rot_mat);

	// 	cvWarpAffine(g_image, g_image, rot_mat, CV_WARP_FILL_OUTLIERS, CV_RGB(255, 255, 255));
	// 	cvWarpAffine(g_sobelImage, g_sobelImage, rot_mat, CV_WARP_FILL_OUTLIERS, CV_RGB(255, 255, 255));
	cvWarpAffine(g_image, g_image, rot_mat);
	cvWarpAffine(g_sobelImage, g_sobelImage, rot_mat);

	strcat(tmpPath, "12_Rotation120.bmp");
	cvSaveImage( tmpPath, g_image );
	strcpy(tmpPath, path);
	// 	cvShowImage("8_Rotation60", g_image);
	// 	waitKey(0);

	// Seq 10. Labeling-3
	cvCopy(g_image, g_gray);
	cvThreshold( g_gray, g_gray, g_threshold, 255, CV_THRESH_BINARY );
	cvCopy( g_gray, g_binary );
	cvZero( g_gray );

	cvThreshold( g_binary, g_invbinary, 1, 255, CV_THRESH_BINARY_INV );
	CBlobLabeling labeling_4;
	labeling_4.SetParam(g_invbinary, g_thresPixMin, g_thresPixMax, g_labelingMargin);		
	labeling_4.DoLabeling();


	output.open("CDsizedata.csv", ios::app);
	output << "3_labeling_Total," << labeling_4.m_nBlobs << endl;
	output.close();


	// Seq 8. Measure to Size
	// Sobel Binary - WHITE : edge section 
	cvThreshold(g_sobelImage, g_sobelImage, g_sbThreshold, 255, CV_THRESH_BINARY);

	strcat(tmpPath, "13_R120_AlignSobelBinary.bmp");
	cvSaveImage( tmpPath, g_sobelImage );
	strcpy(tmpPath, path);

	// 	cvShowImage("SobelBinary", g_sobelImage);
	// 	waitKey(0);

	nW = g_sobelImage->width;
	nH = g_sobelImage->height;
	tmpBuf = new unsigned char[nW * nH];
	for( j = 0; j < nH; j++ )
		for(register int i = 0; i < nW; i++ )
			tmpBuf[j * nW + i] = (unsigned char)g_sobelImage->imageData[j * g_sobelImage->widthStep + i];


	imgWidth = g_image->width;	
	totalCDsize = 0;
	failMeasure = 0;
	min_CDsize = 50;
	max_CDsize = 50;

	cvZero(g_drawImage);
	cvCvtColor(g_image, g_drawImage, CV_GRAY2RGB);
	cvZero(g_drawSobelImage);
	cvCopy(g_sobelImage, g_drawSobelImage);


	for( register int i=0; i < labeling_4.m_nBlobs; i++ )
	{
		// CDsize = 'down + up' or 'left + right'
		int CDsize = 0;
		int dSize = 0, uSize = 0;
		int lSize = 0, rSize = 0;

		// Labeling Center Axis(= criteria)
		cX = labeling_4.m_recBlobs[i].x + (labeling_4.m_recBlobs[i].width / 2);
		cY = labeling_4.m_recBlobs[i].y + (labeling_4.m_recBlobs[i].height / 2);
		CvPoint	pt1 = cvPoint(cX, cY);
		CvPoint pt2 = cvPoint(cX, cY);

		//cvDrawCircle(g_drawSobelImage, pt1, 2, CV_RGB(0, 0, 255));				// Center Axis.

		// temp Y & sizing Y
		int tmpY = cY;
		int szY = 0;

		int idx = tmpY * imgWidth + cX;
		int imgData = tmpBuf[idx];
		bool bFlag = true;

		// sizing Y - down		
		while( imgData != 0 || bFlag == true && szY < 70)
		{
			szY++;
			tmpY++;			

			idx = tmpY * imgWidth + cX;
			imgData = tmpBuf[idx];
			if(imgData == 255)
				bFlag = false;
		} 

		dSize = szY;
		pt2 = cvPoint(cX, tmpY-1);


		cvDrawLine(g_drawImage, pt1, pt2, CV_RGB(255, 255, 0), 1);
		cvDrawLine(g_drawSobelImage, pt1, pt2, CV_RGB(200, 200, 200), 1);

		// sizing Y - up
		tmpY = cY-1;
		szY = 0;

		idx = tmpY * imgWidth + cX;
		imgData = tmpBuf[idx];
		bFlag = true;

		while(  imgData != 0 || bFlag == true && szY < 70)
		{			
			szY++;
			tmpY--;

			idx = tmpY * imgWidth + cX;								
			imgData = tmpBuf[idx];	
			if(imgData == 255)
				bFlag = false;
		}
		uSize = szY;
		pt2 = cvPoint(cX, tmpY+1);


		cvDrawLine(g_drawImage, pt1, pt2, CV_RGB(255, 128, 128), 1);
		cvDrawLine(g_drawSobelImage, pt1, pt2, CV_RGB(110, 110, 110), 1);				

		CDsize = dSize + uSize;
		totalCDsize = totalCDsize + CDsize;

		if(min_CDsize > CDsize)
			min_CDsize = CDsize;
		else if(max_CDsize < CDsize)
			max_CDsize = CDsize;

		output.open("CDsizedata.csv", ios::app);
		output << i << ",size," << CDsize << "," << dSize << "," << uSize << endl;
		output.close();
		// 		cout << i << " size : " << CDsize << endl;

	}
	strcat(tmpPath, "14_R120_MeasureSobel.bmp");
	cvSaveImage( tmpPath, g_drawSobelImage );
	strcpy(tmpPath, path);
	/*	cvShowImage("Mesaure Size_Sobel", g_drawSobelImage);*/

	strcat(tmpPath, "15_R120_MeasurePattern.bmp");
	cvSaveImage( tmpPath, g_drawImage );
	strcpy(tmpPath, path);
	cvShowImage("Measure Size", g_drawImage);
	/*	cvWaitKey(0);*/

	output.open("CDsizedata.csv", ios::app);
	output << "Fail to Measure," << failMeasure << endl;	
	output << "Average," << totalCDsize / (labeling_4.m_nBlobs-failMeasure) << ",0.0465," << totalCDsize / (labeling_4.m_nBlobs-failMeasure) * 0.0465 << endl;	
	output.close();
	// 	cout << "avg. : " << CDsizeTotal / (AfterBlob.m_nBlobs-failMeasure) * 0.0465 << endl;

	
	cvReleaseImage( &g_image );
	cvReleaseImage( &g_sobelImage );
	cvReleaseImage( &g_binary );
	cvReleaseImage( &g_drawImage );
	cvReleaseImage( &g_gray );
	cvReleaseImage( &g_dstImage );	

	return 0;
}