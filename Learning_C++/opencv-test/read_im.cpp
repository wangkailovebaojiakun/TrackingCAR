/*
* File name  : read_im.c8pp
* Author     : wangkai001_tg@163.com
* Version    : v1.0
* Date       : 2016/05/28
*/
#include <stdio.h>
#include <iostream>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/core/utility.hpp>

using namespace cv;
using namespace std;

int main(){
    cv::Mat img;
    img =  cv::imread("/home/wk/Pictures/sex.jpg");
    if (!img.data){
	cout<<"No image data.\n"<<endl;
    	return -1;
    }
    cv::namedWindow("Display Image",WINDOW_AUTOSIZE);
    cv::imshow("Display Image",img);
    cv::waitKey(0);
    return 0;
}
