#ifndef CAMERA_H
#define CAMERA_H

#include "iostream"
#include <opencv2/core/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <simplefacedetector.h>
#include <facehasher.h>

class camera
{
public:
    int device_number;
    int filecounter = 0;
    cv::VideoCapture cam;

    camera(int device_num);
    int close_stream();
};

#endif // CAMERA_H
