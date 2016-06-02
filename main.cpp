#include <iostream>

#include "KinectCV.h"
#include <opencv2/opencv.hpp>
#include "RobotReceiver.h"
#include "RobotSender.h"

using namespace std;
using namespace cv;


int main() {

   // TimeSnapshot timeSnapshot = getTimestampNow();


    bool die(false);

    Mat depthMat(Size(640,480),CV_16UC1);
    Mat depthf (Size(640,480),CV_8UC1);
    Mat rgbMat(Size(640,480),CV_8UC3,Scalar(0));
    Mat ownMat(Size(640,480),CV_8UC3,Scalar(0));

    Mat hsvMat(Size(640,480),CV_8UC3,Scalar(0));
    Mat threshMat; //(Size(640,480),CV_8UC1,Scalar(0));


    Scalar upperRange(35,255,255);
    Scalar lowerRange(20,100,50);

    // The next two lines must be changed as Freenect::Freenect
    // isn't a template but the method createDevice:
    // Freenect::Freenect<MyFreenectDevice> freenect;
    // MyFreenectDevice& device = freenect.createDevice(0);
    // by these two lines:

   // RobotReceiver recv("10.2.25.2");
    RobotSender sender;

    Freenect::Freenect freenect;
    CVFreenectDevice& device = freenect.createDevice<CVFreenectDevice>(0);

    namedWindow("rgb",CV_WINDOW_AUTOSIZE);
    namedWindow("hsv",CV_WINDOW_AUTOSIZE);
    namedWindow("depth",CV_WINDOW_AUTOSIZE);
    device.startVideo();
    //device.startDepth();
    while (!die) {
        device.getVideo(rgbMat);
        //device.getDepth(depthMat);


        //depthMat.convertTo(depthf, CV_8UC1, 255.0/2048.0);
        //cv::imshow("depth",depthf);

        cvtColor(rgbMat, hsvMat, CV_BGR2HSV);

        inRange(hsvMat, lowerRange, upperRange, threshMat);

        erode(threshMat, threshMat, Mat());
       // erode(threshMat, threshMat, Mat());
        //erode(threshMat, threshMat, Mat());
        dilate(threshMat, threshMat, Mat());
        dilate(threshMat, threshMat, Mat());
        dilate(threshMat, threshMat, Mat());
        //dilate(threshMat, threshMat, Mat());
        ///dilate(threshMat, threshMat, Mat());

        imshow("hsv", threshMat);

        vector<vector<Point>> contours;
        findContours(threshMat, contours, CV_RETR_LIST, CV_CHAIN_APPROX_SIMPLE);
        vector<vector<Point>> hull(contours.size());
        vector<vector<Point>> polygons(contours.size());
        std::vector<vector<Point>> candidates;
        std::vector<Rect> candidates_bounding;
        for ( int i = 0; i < contours.size(); i++ ) {
            convexHull(Mat(contours[i]), hull[i]);
            approxPolyDP(hull[i], polygons[i], 8, true);
            Rect bounding = boundingRect(contours[i]);

            if ( bounding.area() < 350 )
                continue;
           // if ( polygons[i].size() != 4 )
           //     continue;

            double ratio = (double)bounding.width/(double)bounding.height;
            if ( ratio >= 2 )
                continue;



            for ( int z = 0; z < contours.size(); z++ ) {
                if ( i != z ) {
                    Rect inside = boundingRect(contours[z]);
                    if ( bounding.contains(inside.tl()) && bounding.contains(inside.br()) )
                    {
                        candidates.push_back(polygons[i]);
                        candidates_bounding.push_back(bounding);
                        continue;
                    }
                }
            }
        }

        int selectedIndex = -1;
        for ( int i = 0; i < candidates.size(); i++ ) {
            if ( selectedIndex == -1 || candidates_bounding[selectedIndex].y > candidates_bounding[i].y )
                selectedIndex = i;
        }

        if ( selectedIndex != -1 ) {
            polylines(rgbMat, candidates[selectedIndex], true, Scalar(0, 255, 0), 5);

            double xPos = ((double)candidates_bounding[selectedIndex].x) / ((double)rgbMat.rows-1.0);
            double xErr = xPos-0.5574;
            double thetaErr = xErr*(57.0/2.0);
//            thetaErr += recv.getAngle();
            //sender.sendAngle(getRobotTime(&timeSnapshot), thetaErr);
            printf("xErr: %2.4f\n", xErr);
        }

        cv::imshow("rgb", rgbMat);

        char k = cvWaitKey(10);
        if( k == 27 ){
            cvDestroyWindow("rgb");
            cvDestroyWindow("hsv");
           // cvDestroyWindow("depth");
            break;
        }
       // if(iter >= 1000) break;
       // iter++;
    }

    device.stopVideo();
   //device.stopDepth();
    return 0;
}