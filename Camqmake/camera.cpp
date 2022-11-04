#include "camera.h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include "stdlib.h"
#include <fstream>

    camera::camera(int device_num) {
    device_number = device_num;
    filecounter = 0;
    cam = cv::VideoCapture(device_number, cv::CAP_V4L);
    if (!cam.isOpened()) {
        std::cerr << "ERROR: Could not open camera, returned Matrix is empty" << std::endl;
    }
}

int camera::close_stream(){
    cam.release();
    cv::destroyAllWindows();
    return 1;
};



