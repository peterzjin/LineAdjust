#include "StdAfx.h"
/* This is a standalone program. Pass an image name as a first parameter of the program.
Switch between standard and probabilistic Hough transform by changing "#if 1" to "#if 0" and back */
#include "opencv2/opencv.hpp"
//#include "opencv2/opencv/highgui.hpp"
#include <math.h>
#include <windows.h>

#define GAUGE_LINE_LENGTH_MM 200

//areaSplite is the divider of reference point area
int areaSplit = 6;
//laserSplit is the divider of reference point area
int laserSplit = 2.3; 
#if 0
bool findCrossPoint(CvPoint a,CvPoint b,CvPoint c,CvPoint d,CvPoint2D32f *px){  
	printf("start....\n");
    // 三角形abc 面积的2倍  
    double area_abc = (a.x - c.x) * (b.y - c.y) - (a.y - c.y) * (b.x - c.x);  
  
    // 三角形abd 面积的2倍  
    double area_abd = (a.x - d.x) * (b.y - d.y) - (a.y - d.y) * (b.x - d.x);   
  
    // 面积符号相同则两点在线段同侧,不相交 (对点在线段上的情况,本例当作不相交处理);  
    if ( area_abc*area_abd>=0 ) {  
        return false;  
    }  
  
    // 三角形cda 面积的2倍  
    double area_cda = (c.x - a.x) * (d.y - a.y) - (c.y - a.y) * (d.x - a.x);  
    // 三角形cdb 面积的2倍  
    // 注意: 这里有一个小优化.不需要再用公式计算面积,而是通过已知的三个面积加减得出.  
    double area_cdb = area_cda + area_abc - area_abd ;  
    if (  area_cda * area_cdb >= 0 ) {  
        return false;  
    }  
  
    //计算交点坐标  
    double t = area_cda / ( area_abd- area_abc );  
    double dx= t*(b.x - a.x),  
        dy= t*(b.y - a.y);  
	px->x = a.x + dx;
	px->y = a.y + dy;
	return true;
 //   return { x: a.x + dx , y: a.y + dy };  
  
}  
#else

bool findCrossPoint(const CvPoint *lineH,const CvPoint *lineV,CvPoint2D32f *px){ 
	if(lineH[0].y == lineH[1].y && lineV[0].x == lineV[1].x){
		px->x = lineV[0].x;
		px->y = lineH[0].y;
	}else if(lineH[0].y == lineH[1].y){
		px->y = lineH[0].y;
		double rCD = (lineV[0].y-lineV[1].y)/(lineV[0].x-lineV[1].x);
		px->x = (px->y-lineV[1].y)/rCD + lineV[1].x;
	}else if(lineV[0].x == lineV[1].x){
		px->x = lineV[0].x;
		double rAB = (lineH[0].y-lineH[1].y)/(lineH[0].x-lineH[1].x);
		px->y = rAB*(px->x-lineH[1].x)+lineH[1].y;
	}else{
		double rAB = (lineH[0].y-lineH[1].y)/(lineH[0].x-lineH[1].x);
		double rCD = (lineV[0].y-lineV[1].y)/(lineV[0].x-lineV[1].x);
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

#endif
int calculateOffset(const IplImage *orgImg,IplImage *debugDest,double *offset){
	int ret = -1;
	int i, j;
	if (orgImg)
	{
		int gWidth,gHeight;
		IplImage *dest = cvCreateImage(cvGetSize(orgImg),IPL_DEPTH_8U,1);
		gWidth = dest->width; gHeight = dest->height;

		CvMemStorage *storage = cvCreateMemStorage();

		CvSeq *allLines = 0;
		
		cvCanny(orgImg,dest,50,200,3);
		
		allLines = cvHoughLines2(dest,storage,CV_HOUGH_PROBABILISTIC,1,CV_PI/180,20,50,50);

		CvPoint2D32f pointLU, pointLD, pointRU, pointRD;

		CvPoint* line;

		CvPoint *lineVLU = 0, *lineHLU = 0,
				*lineVLD = 0, *lineHLD = 0,
				*lineVRU = 0, *lineHRU = 0,
				*lineVRD = 0, *lineHRD = 0;
		
		CvPoint *lineLsL = 0, *lineLsR = 0;

		//find all userful lines
		for (i=0;i<allLines->total;i++){
			line = (CvPoint*)cvGetSeqElem(allLines,i);
			
			if(line[0].x < gWidth/areaSplit && line[0].y < gHeight/areaSplit 
				&& line[1].x < gWidth/areaSplit && line[1].y < gHeight/areaSplit){
					//left up
					//cvLine( colorDest, line[0], line[1], CV_RGB(255,0,0), 1, CV_AA, 0 );
					if(abs(line[0].x-line[1].x) > abs(line[0].y-line[1].y)){
						if(!lineHLU || lineHLU->y > line->y) lineHLU = line;	
					}else{
						if(!lineVLU || lineVLU->x > line->x) lineVLU = line;
					}
					
			}else if(line[0].x < gWidth/areaSplit && line[0].y > gHeight*(areaSplit-1)/areaSplit 
				&& line[1].x < gWidth/areaSplit && line[1].y > gHeight*(areaSplit-1)/areaSplit){
					//left down
					//cvLine( colorDest, line[0], line[1], CV_RGB(0,255,0), 1, CV_AA, 0 );
					if(abs(line[0].x-line[1].x) > abs(line[0].y-line[1].y)){
						if(!lineHLD || lineHLD->y < line->y) lineHLD = line;	
					}else{
						if(!lineVLD || lineVLD->x > line->x) lineVLD = line;
					}
					
			}else if(line[0].x > gWidth*(areaSplit-1)/areaSplit && line[0].y < gHeight/areaSplit 
				&& line[1].x > gWidth*(areaSplit-1)/areaSplit && line[1].y < gHeight/areaSplit){
					//right up
					//cvLine( colorDest, line[0], line[1], CV_RGB(0,0,255), 1, CV_AA, 0 );
					if(abs(line[0].x-line[1].x) > abs(line[0].y-line[1].y)){
						if(!lineHRU || lineHRU->y > line->y) lineHRU = line;	
					}else{
						if(!lineVRU || lineVRU->x < line->x) lineVRU = line;
					}
					
			}else if(line[0].x > gWidth*(areaSplit-1)/areaSplit && line[0].y > gHeight*(areaSplit-1)/areaSplit 
				&& line[1].x > gWidth*(areaSplit-1)/areaSplit && line[1].y > gHeight*(areaSplit-1)/areaSplit){
					//right down
					//cvLine( colorDest, line[0], line[1], CV_RGB(255,255,0), 1, CV_AA, 0 );
					if(abs(line[0].x-line[1].x) > abs(line[0].y-line[1].y)){
						if(!lineHRD || lineHRD->y < line->y) lineHRD = line;	
					}else{
						if(!lineVRD || lineVRD->x < line->x) lineVRD = line;
					}
					
			}else {
				//laser lines
				//cvLine( colorDest, line[0], line[1], CV_RGB(255,0,255), 1, CV_AA, 0 );
				if((line[0].y > gHeight/laserSplit && line[1].y < gHeight*(laserSplit-1)/laserSplit) 
				|| (line[0].y < gHeight/laserSplit && line[1].y > gHeight*(laserSplit-1)/laserSplit))
				{
					if(!lineLsL || lineLsL->x > line->x) lineLsL = line;
					if(!lineLsR || lineLsR->x < line->x) lineLsR = line;
				}
				
			}
			
		}
		if(!lineVLU || !lineHLU || !lineVLD || !lineHLD || !lineVRU || !lineHRU || !lineVRD || !lineHRD){
			printf("Can't find reference lines!!!!!!!!!!!!!!!!!!!");
			ret = -1;
			goto END;
		}
		if(debugDest){
			if(lineVLU) cvLine( debugDest, lineVLU[0], lineVLU[1], CV_RGB(255,0,0), 1, CV_AA, 0 );
			if(lineHLU) cvLine( debugDest, lineHLU[0], lineHLU[1], CV_RGB(255,0,0), 1, CV_AA, 0 );

			if(lineVLD) cvLine( debugDest, lineVLD[0], lineVLD[1], CV_RGB(0,255,0), 1, CV_AA, 0 );
			if(lineHLD) cvLine( debugDest, lineHLD[0], lineHLD[1], CV_RGB(0,255,0), 1, CV_AA, 0 );

			if(lineVRU) cvLine( debugDest, lineVRU[0], lineVRU[1],CV_RGB(0,0,255), 1, CV_AA, 0 );
			if(lineHRU) cvLine( debugDest, lineHRU[0], lineHRU[1],CV_RGB(0,0,255), 1, CV_AA, 0 );

			if(lineVRD) cvLine( debugDest, lineVRD[0], lineVRD[1], CV_RGB(255,255,0), 1, CV_AA, 0 );
			if(lineHRD) cvLine( debugDest, lineHRD[0], lineHRD[1], CV_RGB(255,255,0), 1, CV_AA, 0 );
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
			cvCircle(debugDest,cvPointFrom32f(pointLU),3,CV_RGB(0,255,0),-1,8,0);
			cvCircle(debugDest,cvPointFrom32f(pointLD),3,CV_RGB(0,255,0),-1,8,0);
			cvCircle(debugDest,cvPointFrom32f(pointRU),3,CV_RGB(0,255,0),-1,8,0);
			cvCircle(debugDest,cvPointFrom32f(pointRD),3,CV_RGB(0,255,0),-1,8,0);
		}
	
		//generate horizontal reference line
		CvPoint2D32f lineHCenter[2]; 
		lineHCenter[0].x = (pointLU.x+pointLD.x)/2.0;
		lineHCenter[0].y = (pointLU.y+pointLD.y)/2.0;
		lineHCenter[1].x = (pointRU.x+pointRD.x)/2.0;
		lineHCenter[1].y = (pointRU.y+pointRD.y)/2.0;
		if(debugDest){
			cvLine( debugDest, cvPointFrom32f(lineHCenter[0]), cvPointFrom32f(lineHCenter[1]), CV_RGB(0,0,255), 1, CV_AA, 0 );
		}
	
		if(!lineLsL || !lineLsR){
			printf("Can't find laser!!!!!!!!!!!!!!!!!!!");
			ret = -1;
			goto END;
		}
		if(debugDest){
			if(lineLsL) cvLine( debugDest, lineLsL[0], lineLsL[1], CV_RGB(0,0,255), 1, CV_AA, 0 );
			if(lineLsR) cvLine( debugDest, lineLsR[0], lineLsR[1], CV_RGB(0,0,255), 1, CV_AA, 0 );
		}
		//calculate left and right laser line ***************important*****************
		CvPoint2D32f pointLsL, pointLsR;
		
		if(!findCrossPoint32fInt(lineHCenter,lineLsL,&pointLsL)){
			printf("Can't find left cross point!!!!!!!!!!!!!!!!!!!");
			ret = -1;
			goto END;
		}

		if(!findCrossPoint32fInt(lineHCenter,lineLsR,&pointLsR)){
			printf("Can't find right cross point!!!!!!!!!!!!!!!!!!!");
			ret = -1;
			goto END;
		}
		//generate vertical center laser point crossed with horizontal reference line
		CvPoint2D32f lsCenter; 
		lsCenter.x = (pointLsL.x+pointLsR.x)/2;
		lsCenter.y = (pointLsL.y+pointLsR.y)/2;

		if(debugDest){
			cvCircle(debugDest,cvPointFrom32f(lsCenter),3,CV_RGB(0,0,255),-1,8,0);
		}

		//calculate percent to right
		double offsetPercent = sqrt(pow(lsCenter.x-lineHCenter[1].x,2)+pow(lsCenter.y-lineHCenter[1].y,2))
			/sqrt(pow((lineHCenter[0].x-lineHCenter[1].x),2)+pow((lineHCenter[0].y-lineHCenter[1].y),2));

		*offset = offsetPercent*GAUGE_LINE_LENGTH_MM;

		if(debugDest){
			CvFont font;
			char tmp[64]={0};
			sprintf(tmp,"%.2fmm",*offset);
			cvInitFont( &font, CV_FONT_VECTOR0,0.5, 0.5, 0, 1, 8);
			cvPutText(debugDest, tmp , cvPoint(lsCenter.x+20, lsCenter.y-5), &font, CV_RGB(0,0,255));
		}

END:
	cvReleaseImage(&dest);
	cvReleaseMemStorage(&storage);
	ret = 0;
	}
	return ret;
}
#if 0
int main(void)
{
	int ret = 0;
	double offset = 0;
	IplImage *src = cvLoadImage("d:\\2_1200.png",CV_LOAD_IMAGE_GRAYSCALE);
	IplImage *colorDest = cvLoadImage("d:\\2_1200.png",CV_LOAD_IMAGE_COLOR);// cvCreateImage(cvGetSize(src),IPL_DEPTH_8U,3);cvCloneImage(src);
	ret = calculateOffset(src,colorDest,&offset);
	if(ret == 0){
		printf("offset is %.2fmm/n", offset); 
	}else{
		printf("error happened!!!!!!!/n");
	}
	if(src){
	//	cvNamedWindow("Source");
	//	cvShowImage("Source",src);
	//	cvReleaseImage(&src);
	}
	if(colorDest){
		cvNamedWindow("Result");
		cvShowImage("Result",colorDest);
		cvReleaseImage(&colorDest);
	}
	cvWaitKey(0);
	cvDestroyAllWindows();
	return ret;
}

#endif