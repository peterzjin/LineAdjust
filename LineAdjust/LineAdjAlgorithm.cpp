#include "StdAfx.h"
/* This is a standalone program. Pass an image name as a first parameter of the program.
Switch between standard and probabilistic Hough transform by changing "#if 1" to "#if 0" and back */
#include "opencv2/opencv.hpp"
//#include "opencv2/opencv/highgui.hpp"
#include <math.h>
#include <windows.h>

#define GAUGE_LINE_LENGTH_MM 200

//down area is invalid 
int invalidAreaSplit = 150;

//areaSplite is the divider of reference point area
double hAreaSplit = 8;
double vAreaSplit = 4;

//laserSplit is the divider of reference point area
double laserSplit = 2.5; 

bool findCrossPoint(const CvPoint *lineH,const CvPoint *lineV,CvPoint2D32f *px){ 
	if(lineH[0].y == lineH[1].y && lineV[0].x == lineV[1].x){
		px->x = lineV[0].x;
		px->y = lineH[0].y;
	}else if(lineH[0].y == lineH[1].y){
		px->y = lineH[0].y;
		float rCD = float(lineV[0].y-lineV[1].y)/float(lineV[0].x-lineV[1].x);
		px->x = (px->y-lineV[1].y)/rCD + lineV[1].x;
	}else if(lineV[0].x == lineV[1].x){
		px->x = lineV[0].x;
		float rAB = float(lineH[0].y-lineH[1].y)/float(lineH[0].x-lineH[1].x);
		px->y = rAB*(px->x-lineH[1].x)+lineH[1].y;
	}else{
		float rAB = float(lineH[0].y-lineH[1].y)/float(lineH[0].x-lineH[1].x);
		float rCD = float(lineV[0].y-lineV[1].y)/float(lineV[0].x-lineV[1].x);
		px->x = (lineV[1].y-lineH[1].y-rCD*lineV[1].x+rAB*lineH[1].x)/(rAB-rCD);
		px->y = rAB*(px->x-lineH[1].x)+lineH[1].y;
	}
	return true;
}

bool findCrossPoint32fInt(const CvPoint2D32f *lineH,const CvPoint *lineV,CvPoint2D32f *px){
	CvPoint lineHInt[2],lineVInt[2];
	lineHInt[0] = cvPointFrom32f(lineH[0]);
	lineHInt[1] = cvPointFrom32f(lineH[1]);
	return findCrossPoint(lineHInt,lineV,px);
}

int calculateOffset(IplImage *orgImg,IplImage *debugDest,double *offset){
	int ret = -1;
	int i, j;
	if (orgImg)
	{
		int gWidth,gHeight;
		gWidth = orgImg->width; gHeight = orgImg->height;

		double validHeight = gHeight-invalidAreaSplit;

		double validLeftH = gWidth/hAreaSplit;
		double validRightH = gWidth*(hAreaSplit-1)/hAreaSplit;
		double validUpV = validHeight/vAreaSplit;
		double validDownV = validHeight*(vAreaSplit-1)/vAreaSplit;

		double validLaserUpV = validHeight/laserSplit;
		double validLaserDownV = validHeight*(laserSplit-1)/laserSplit; 

		if(debugDest){
			cvLine( debugDest,cvPoint(0,validHeight),cvPoint(gWidth,validHeight),CV_RGB(0,255,0), 1, CV_AA, 0 );

			cvLine( debugDest,cvPoint(0,validUpV), cvPoint(gWidth,validUpV), CV_RGB(0,255,0), 1, CV_AA, 0 );
			cvLine( debugDest,cvPoint(0,validDownV), cvPoint(gWidth,validDownV), CV_RGB(0,255,0), 1, CV_AA, 0 );

			cvLine( debugDest,cvPoint(validLeftH,0), cvPoint(validLeftH,validHeight), CV_RGB(0,255,0), 1, CV_AA, 0 );
			cvLine( debugDest,cvPoint(validRightH,0), cvPoint(validRightH,validHeight), CV_RGB(0,255,0), 1, CV_AA, 0 );

			cvLine( debugDest,cvPoint(0,validLaserUpV),cvPoint(gWidth,validLaserUpV),CV_RGB(0,255,0), 1, CV_AA, 0 );
			cvLine( debugDest,cvPoint(0,validLaserDownV),cvPoint(gWidth,validLaserDownV), CV_RGB(0,255,0), 1, CV_AA, 0 );
		}

		//select valid area
		IplImage *validImgColor = cvCreateImage(cvSize(gWidth,validHeight),orgImg->depth,orgImg->nChannels);
		IplImage *validImgGray = cvCreateImage(cvSize(gWidth,validHeight),orgImg->depth,1);
		
		cvSetImageROI(orgImg,cvRect(0,0,gWidth,validHeight));
		cvCopy(orgImg,validImgColor);
		cvResetImageROI(orgImg);

//		cvSmooth(validImg,smoothImg,CV_MEDIAN ,3,3);
		cvCvtColor( validImgColor, validImgGray, CV_BGR2GRAY );

		IplImage *verifyImg = validImgGray;
		IplImage *laserImg = validImgColor;

//		cvSetImageROI(validImg,cvRect(0,0,validLeftH,validUpV));
		
		//start
		IplImage *destVerify = cvCreateImage(cvGetSize(validImgColor),IPL_DEPTH_8U,1);
		IplImage *destLaser = cvCloneImage(destVerify);

		CvSeq *verifyLines = 0,*laserAllLines = 0;
		
		cvCanny(verifyImg,destVerify,40,100,3);
		cvCanny(laserImg,destLaser,50,150,3);
		
		CvMemStorage *storage = cvCreateMemStorage();
		CvMemStorage *storageLaser = cvCreateMemStorage();
		verifyLines = cvHoughLines2(destVerify,storage,CV_HOUGH_PROBABILISTIC,1,CV_PI/180,20,10,10);
		laserAllLines = cvHoughLines2(destLaser,storageLaser,CV_HOUGH_PROBABILISTIC,1,CV_PI/180,80,80,50);

		CvPoint2D32f pointLU, pointLD, pointRU, pointRD;

		CvPoint *lineVLU = 0, *lineHLU = 0,
				*lineVLD = 0, *lineHLD = 0,
				*lineVRU = 0, *lineHRU = 0,
				*lineVRD = 0, *lineHRD = 0;
		
		CvPoint *lineLsL = 0, *lineLsR = 0;

		CvPoint* line;

		for (i=0;i<verifyLines->total;i++){
			line = (CvPoint*)cvGetSeqElem(verifyLines,i);

			//detect verify points
			if(line[0].x < validLeftH && line[1].x < validLeftH){
				//left
				if(line[0].y <validUpV && line[1].y <validUpV){
					//up
					if(abs(line[0].x-line[1].x) > abs(line[0].y-line[1].y)){
						if(!lineHLU || (lineHLU[0].y+lineHLU[1].y) > (line[0].y+line[1].y)) lineHLU = line;	
					}else{
						if(!lineVLU || (lineVLU[0].x+lineVLU[1].x) > (line[0].x+line[1].x)) lineVLU = line;
					}
				}else if(line[0].y > validDownV && line[1].y > validDownV){
					//down
					if(abs(line[0].x-line[1].x) > abs(line[0].y-line[1].y)){
						if(!lineHLD || (lineHLD[0].y+lineHLD[1].y) < (line[0].y+line[1].y)) lineHLD = line;	
					}else{
						if(!lineVLD || (lineVLD[0].x+lineVLD[1].x) > (line[0].x+line[1].x)) lineVLD = line;
					}
				}
			}else if(line[0].x > validRightH && line[1].x > validRightH){
				//right
				if(line[0].y <validUpV && line[1].y <validUpV){
					//up
					if(abs(line[0].x-line[1].x) > abs(line[0].y-line[1].y)){
						if(!lineHRU || (lineHRU[0].y+lineHRU[1].y) >(line[0].y+line[1].y)) lineHRU = line;	
					}else{
						if(!lineVRU || (lineVRU[0].x+lineVRU[1].x) < (line[0].x+line[1].x)) lineVRU = line;
					}
				}else if(line[0].y > validDownV && line[1].y > validDownV){
					//down
					if(abs(line[0].x-line[1].x) > abs(line[0].y-line[1].y)){
						if(!lineHRD || (lineHRD[0].y+lineHRD[1].y) < (line[0].y+line[1].y)) lineHRD = line;	
					}else{
						if(!lineVRD || (lineVRD[0].x+lineVRD[1].x) < (line[0].x+line[1].x)) lineVRD = line;
					}
				}
			}
		}
		if(!lineVLU || !lineHLU || !lineVLD || !lineHLD || !lineVRU || !lineHRU || !lineVRD || !lineHRD){
			printf("Can't find reference lines!!!!!!!!!!!!!!!!!!!");
			ret = -1;
			goto END;
		}

		if(debugDest){
			//if(lineVLU) cvLine( debugDest, lineVLU[0], lineVLU[1], CV_RGB(255,0,0), 1, CV_AA, 0 );
			//if(lineHLU) cvLine( debugDest, lineHLU[0], lineHLU[1], CV_RGB(255,0,0), 1, CV_AA, 0 );

			//if(lineVLD) cvLine( debugDest, lineVLD[0], lineVLD[1], CV_RGB(0,255,0), 1, CV_AA, 0 );
			//if(lineHLD) cvLine( debugDest, lineHLD[0], lineHLD[1], CV_RGB(0,255,0), 1, CV_AA, 0 );

			//if(lineVRU) cvLine( debugDest, lineVRU[0], lineVRU[1],CV_RGB(0,0,255), 1, CV_AA, 0 );
			//if(lineHRU) cvLine( debugDest, lineHRU[0], lineHRU[1],CV_RGB(0,0,255), 1, CV_AA, 0 );

			//if(lineVRD) cvLine( debugDest, lineVRD[0], lineVRD[1], CV_RGB(255,255,0), 1, CV_AA, 0 );
			//if(lineHRD) cvLine( debugDest, lineHRD[0], lineHRD[1], CV_RGB(255,255,0), 1, CV_AA, 0 );
		}

		//calculate four reference points
		if(!findCrossPoint(lineHLU,lineVLU,&pointLU)
			||!findCrossPoint(lineHLD,lineVLD,&pointLD) 
			||!findCrossPoint(lineHRU,lineVRU,&pointRU )
			||!findCrossPoint(lineHRD,lineVRD,&pointRD )){
			printf("Can't find reference points!!!!!!!!!!!!!!!!!!!");
			ret = -1;
			goto END;
		}
		if(debugDest){
			cvCircle(debugDest,cvPointFrom32f(pointLU),2,CV_RGB(0,255,0),-1,8,0);
			cvCircle(debugDest,cvPointFrom32f(pointLD),2,CV_RGB(0,255,0),-1,8,0);
			cvCircle(debugDest,cvPointFrom32f(pointRU),2,CV_RGB(0,255,0),-1,8,0);
			cvCircle(debugDest,cvPointFrom32f(pointRD),2,CV_RGB(0,255,0),-1,8,0);
		}
	
		//generate horizontal reference line
		CvPoint2D32f lineHCenter[2]; 
		CvPoint2D32f pointCenter;
//		lineHCenter[0].x = (pointLU.x+pointLD.x)/2.0;
//		lineHCenter[0].y = (pointLU.y+pointLD.y)/2.0;
//		lineHCenter[1].x = (pointRU.x+pointRD.x)/2.0;
//		lineHCenter[1].y = (pointRU.y+pointRD.y)/2.0;

		lineHCenter[0].x = (pointLU.x+pointLD.x)/2.0;
		lineHCenter[0].y = (pointLU.y+pointLD.y)/2.0;
		lineHCenter[1].x = (pointRU.x+pointRD.x)/2.0;
		lineHCenter[1].y = (pointRU.y+pointRD.y)/2.0;

		pointCenter.x = (lineHCenter[0].x + lineHCenter[1].x)/2.0;
		pointCenter.y = (lineHCenter[0].y + lineHCenter[1].y)/2.0;

		if(debugDest){
			cvCircle(debugDest,cvPointFrom32f(lineHCenter[0]),2,CV_RGB(0,255,0),-1,8,0);
			cvCircle(debugDest,cvPointFrom32f(lineHCenter[1]),2,CV_RGB(0,255,0),-1,8,0);
			cvLine( debugDest, cvPointFrom32f(lineHCenter[0]), cvPointFrom32f(lineHCenter[1]), CV_RGB(0,0,255), 1, CV_AA, 0 );
		}
		for (i=0;i<laserAllLines->total;i++){
			line = (CvPoint*)cvGetSeqElem(laserAllLines,i);

			if((line[0].x > validLeftH && line[0].x < validRightH)
				&&(line[1].x > validLeftH && line[1].x < validRightH)){
				//laser lines
				if((line[0].y < validLaserUpV && line[1].y > validLaserDownV)
				|| (line[1].y < validLaserUpV && line[0].y > validLaserDownV))
				{
					//cvLine( debugDest, line[0], line[1], CV_RGB(0,255,255), 1, CV_AA, 0 );
					if(!lineLsL || (lineLsL[0].x+lineLsL[1].x) > (line[0].x+line[1].x)) lineLsL = line;
					if(!lineLsR || (lineLsR[0].x+lineLsR[1].x) < (line[0].x+line[1].x)) lineLsR = line;
				}
			}
		}
		if(!lineLsL || !lineLsR){
			printf("Can't find laser!!!!!!!!!!!!!!!!!!!");
			ret = 1;
			goto END;
		}
		if(debugDest){
			if(lineLsL) cvLine( debugDest, lineLsL[0], lineLsL[1], CV_RGB(0,255,255), 1, CV_AA, 0 );
			if(lineLsR) cvLine( debugDest, lineLsR[0], lineLsR[1], CV_RGB(0,255,255), 1, CV_AA, 0 );
		}
		//calculate left and right laser line ***************important*****************
		CvPoint2D32f pointLsL, pointLsR;
		
		if(!findCrossPoint32fInt(lineHCenter,lineLsL,&pointLsL)){
			printf("Can't find left cross point!!!!!!!!!!!!!!!!!!!");
			ret = 1;
			goto END;
		}

		if(!findCrossPoint32fInt(lineHCenter,lineLsR,&pointLsR)){
			printf("Can't find right cross point!!!!!!!!!!!!!!!!!!!");
			ret = 1;
			goto END;
		}

		if(pointLsR.x-pointLsL.x<2){
			printf("Invalid cross point!!!!!!!!!!!!!!!!!!!");
			ret = 1;
			goto END;
		}

		//generate vertical center laser point crossed with horizontal reference line
		CvPoint2D32f lsCenter; 
		lsCenter.x = (pointLsL.x+pointLsR.x)/2;
		lsCenter.y = (pointLsL.y+pointLsR.y)/2;

		if(debugDest){
			cvCircle(debugDest,cvPointFrom32f(lsCenter),3,CV_RGB(0,255,0),-1,8,0);
		}

		//calculate percent to right
//		double offsetPercentRight = sqrt(pow(lsCenter.x-lineHCenter[1].x,2)+pow(lsCenter.y-lineHCenter[1].y,2))
//			/sqrt(pow((lineHCenter[0].x-lineHCenter[1].x),2)+pow((lineHCenter[0].y-lineHCenter[1].y),2));
//		double offsetPercentLeft = sqrt(pow(lsCenter.x-lineHCenter[0].x,2)+pow(lsCenter.y-lineHCenter[0].y,2))
//			/sqrt(pow((lineHCenter[0].x-lineHCenter[1].x),2)+pow((lineHCenter[0].y-lineHCenter[1].y),2));
//		double offsetPercent = sqrt(pow(lsCenter.x-lineHCenter[1].x,2)+pow(lsCenter.y-lineHCenter[1].y,2))
//			/sqrt(pow(lsCenter.x-lineHCenter[0].x,2)+pow(lsCenter.y-lineHCenter[0].y,2));
//		offsetPercent = 1.0/(offsetPercent+1.0);
//		double offsetPercent = fabs(lineHCenter[0].x-lsCenter.x)/fabs(lineHCenter[0].x-lineHCenter[1].x);
//		*offset = offsetPercent*GAUGE_LINE_LENGTH_MM;

		double centerOffsetPercent = sqrt(pow(lsCenter.x-pointCenter.x,2)+pow(lsCenter.y-pointCenter.y,2))/sqrt(pow((lineHCenter[0].x-lineHCenter[1].x),2)+pow((lineHCenter[0].y-lineHCenter[1].y),2));

		*offset = lsCenter.x-pointCenter.x > 0? centerOffsetPercent*GAUGE_LINE_LENGTH_MM + 100.0 : -centerOffsetPercent*GAUGE_LINE_LENGTH_MM + 100.0;

		if(debugDest){
			CvFont font;
			cvInitFont( &font, CV_FONT_VECTOR0,1.0, 1.0, 0, 1, 8);
			char tmp[64]={0};
			sprintf(tmp,"%.2fmm",*offset);
			cvPutText(debugDest, tmp , cvPoint(lsCenter.x-200, lsCenter.y-5), &font, CV_RGB(0,0,255));
		}
		ret = 0;
END:
	if(ret != 0) *offset = 0;
//	cvNamedWindow("Image-out",CV_WINDOW_AUTOSIZE);  
//	cvShowImage("Image-out",destLaser);
//	cvWaitKey(); 
//	cvDestroyWindow("Image-out");

	cvReleaseImage(&validImgColor);
	cvReleaseImage(&validImgGray);
	cvReleaseImage(&destVerify);
	cvReleaseImage(&destLaser);
	cvReleaseMemStorage(&storage);
	cvReleaseMemStorage(&storageLaser);
	}
	return ret;
}