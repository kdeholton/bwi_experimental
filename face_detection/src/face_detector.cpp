#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"

#include <iostream>
#include <stdio.h>

#include "ros/ros.h"
#include "std_msgs/String.h"
#include "sensor_msgs/Image.h"
#include <sstream>

#include <cv_bridge/cv_bridge.h>

using namespace std;
using namespace cv;

/** Function Headers */
void detectAndDisplay( Mat frame, ros::Publisher* face_pub );

int counter = 0;

uint32_t frame_counter = 0;
/** Global variables */
String face_cascade_name = "/home/bwi/opencv/haarcascade_frontalface_alt.xml";
CascadeClassifier face_cascade;

ros::Publisher face_pub;

int detectFaces(cv_bridge::CvImagePtr cvImagePtr);

void callback_function(const sensor_msgs::Image msg)
{
  cv_bridge::CvImagePtr cv_ptr;
  try
  {
    cv_ptr = cv_bridge::toCvCopy(msg);
    //printf("GOT FRAME\n");
    detectFaces(cv_ptr);
  }
  catch (cv_bridge::Exception& e)
  {
    ROS_ERROR("cv_bridge exception: %s", e.what());
    return;
  }
}



/** @function main */
int main( int argc, char** argv )
{

  //printf("01\n");
  ros::init(argc, argv, "faceDetector");
  //printf("02\n");
  //printf("03\n");
  ros::NodeHandle n;
  //printf("04\n");
  face_pub = n.advertise<std_msgs::String>("faceDetection",1000);
  //printf("05\n");

  ros::Subscriber sub = n.subscribe("/camera/rgb/image_color",1000,callback_function);
  //printf("06\n");
  ros::spin();
  //printf("07\n");
}

int detectFaces(cv_bridge::CvImagePtr cvImagePtr){
  Mat frame;
  frame = cvImagePtr->image;

  //-- 1. Load the cascades
  if( !face_cascade.load( face_cascade_name ) ){ printf("--(!)Error loading\n"); return -1; };

  //-- 2. Read the video stream
  //while( true )
  //{
    frame_counter++;

    //imshow("test",frame);
    detectAndDisplay( frame , &face_pub ); 

    if(!ros::ok()) { return -1; }

    int c = waitKey(10);
  //}
  return 0;
}
/** @function detectAndDisplay */
void detectAndDisplay( Mat frame , ros::Publisher* face_pub)
{
  //printf("10\n");
  ros::Rate loop_rate(10);
  //printf("11\n");
  std::vector<Rect> faces;
  //printf("12\n");
  Mat frame_gray;
  //printf("13\n");

  cvtColor( frame, frame_gray, CV_BGR2GRAY );
  //printf("14\n");
  equalizeHist( frame_gray, frame_gray );
  //printf("15\n");

  //-- Detect faces
  face_cascade.detectMultiScale( frame_gray, faces, 1.1, 2, 0|CV_HAAR_SCALE_IMAGE, Size(30, 30) );
  //printf("16\n");


  ROS_INFO("Frame %d\n",frame_counter);
  //printf("17\n");
  for( size_t i = 0; i < faces.size(); i++ )
  {
  //printf("18\n");
    std_msgs::String msg;
  //printf("19\n");
    std::stringstream ss;
  //printf("20\n");
    ss << "Face Detected #" << counter++ << " X:" << faces[i].x << " Y:" << faces[i].y;
  //printf("21\n");
    msg.data = ss.str();
  //printf("22\n");
    ROS_INFO("%s", msg.data.c_str());
  //printf("23\n");

    face_pub->publish(msg);
  //printf("24\n");

    //ros::spinOnce();
  //printf("25\n");

    loop_rate.sleep();
  }
  //-- Show what you got
  //imshow( window_name, frame );
}
