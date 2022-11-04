#ifndef ENTRANCEMANAGER_H
#define ENTRANCEMANAGER_H

#include <simplefacedetector.h>
#include <facehasher.h>
#include <camera.h>
#include <opencv2/face/facerec.hpp>

class EntranceManager
{
public:
    SimpleFaceDetector detector;
    FaceHasher facehasher;
    std::vector<dlib::matrix<float,0,1>> encodings;
    camera cam;

    EntranceManager(int camera_device_nr);
    bool draw_on_frame(cv::Mat &frame, int w, int h);
    void show_stream(std::string windowName, cv::Ptr<cv::face::LBPHFaceRecognizer>& model);
};

#endif // ENTRANCEMANAGER_H
