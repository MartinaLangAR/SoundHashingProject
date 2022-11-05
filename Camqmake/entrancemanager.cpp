#include "entrancemanager.h"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/face/facerec.hpp>
#include <boost/lexical_cast.hpp>
#include <thread>

#define SHOW(X) std::cout << # X " = " << (X) << std::endl


EntranceManager::EntranceManager(int camera_device_nr) : cam(camera_device_nr){

}
uint hashing(std::vector<dlib::matrix<float,0,1>> encoding, FaceHasher& facehasher){
    std::vector<float> compressed(16); //init vector with 16 slots (8 summed up floats per slot)
        dlib::matrix<float,0,1> values = encoding.at(0);
        // fill vector with summed up floats
        for (long j = 0; j < 128; j++){
            int slot = j % 16;
            compressed[slot] += values(0,j);
        }
    //convert floats to strings
    std::vector<std::string> values_as_strings;
    for(const auto& com: compressed) {
        values_as_strings.push_back(boost::lexical_cast<std::string>(com));
    }
    int hashcode = facehasher.step_into_encoding(values_as_strings, 1, 200);
    return hashcode;
}

void display_hashcode(cv::Mat frame, uint hashcode, std::string windowName) {
    std::string stringcode = "Your Hashcode is: " + std::to_string(hashcode);
    cv::Size textSize = getTextSize(stringcode, cv::FONT_HERSHEY_SIMPLEX,
                                1.0, 2, 0);
    // center the text
    cv::Point textOrg((frame.cols)/2 - textSize.width/2,
                  (frame.rows + textSize.height)/2 - textSize.height);
    // draw the box
    cv::rectangle(frame, textOrg + cv::Point(0, 2),
              textOrg + cv::Point(textSize.width, - textSize.height),
              cv::Scalar(255,255,255), cv::FILLED);
    cv::putText(frame, //target image
                stringcode , //text
                textOrg, //top-left position
                cv::FONT_HERSHEY_SIMPLEX,
                1.0,
                CV_RGB(118, 185, 0), //font color
                2);
    cv::imshow(windowName, frame);
    cv::waitKey(20);
}

void display_text(cv::Mat frame, std::string text, std::string windowName) {
    cv::Size textSize = getTextSize(text, cv::FONT_HERSHEY_SIMPLEX,
                                1.0, 2, 0);
    // center the text
    cv::Point textOrg((frame.cols - textSize.width)/2 - textSize.width/2,
                  (frame.rows + textSize.height)/2 - textSize.height);
    // draw the box
    cv::rectangle(frame, textOrg + cv::Point(0, 2),
              textOrg + cv::Point(textSize.width, -textSize.height),
              cv::Scalar(255,255,255), cv::FILLED);
    cv::putText(frame, //target image
                text , //text
                textOrg, //top-left position
                cv::FONT_HERSHEY_SIMPLEX,
                1.0,
                CV_RGB(118, 185, 0), //font color
                2);
    cv::imshow(windowName, frame);
    cv::waitKey(15);
    //cv::destroyWindow(windowName);

}

bool EntranceManager::draw_on_frame(cv::Mat &frame, int w, int h){
    //rectangle behind help-text
    cv::rectangle(frame, cv::Point(0,0),
              cv::Point(frame.cols, 20),
              cv::Scalar(0,0,0), cv::FILLED);
    // help-text
    cv::putText(frame, //target image
                "Please align your face within the white rectangle" , //text
                cv::Point(2 , 10), //top-left position
                cv::FONT_HERSHEY_SIMPLEX,
                0.5,
                CV_RGB(118, 185, 0), //font color
                1);
    int x1 = (frame.cols/2) - (w/2);
    int y1 = (frame.rows/2) - (h/2);
    int x2 = (frame.cols/2) + (w/2);
    int y2 = (frame.cols/2) + (w/2);
    cv::rectangle(frame, cv::Point(x1,y1),
              cv::Point(x2, y2),
              cv::Scalar(255,255,255), 4);
    auto rectangles = detector.detect_face_rectangles(frame);
            cv::Scalar color(0, 105, 205);
            int frame_thickness = 4;
            for(const auto & r : rectangles){
                cv::rectangle(frame, r, color, frame_thickness);
                if (r.x >= x1 and r.y >= y1 and (r.x + r.width) <= (x1 + w) and (r.y + r.height <= y1 + h))  {
                   return true;
                }
            }
     return false;
}


void train_model(cv::Ptr<cv::face::LBPHFaceRecognizer>& model, cv::Mat img, int hashcode) {
    cv::Mat grayscale;
    cvtColor(img, grayscale, cv::COLOR_BGR2GRAY);
    std::vector<cv::Mat> images {grayscale};
    std::vector<int> labels {hashcode};
    model -> train(images, labels);
    return ;
}

void EntranceManager::show_stream(std::string windowName, cv::Ptr<cv::face::LBPHFaceRecognizer>& model, AudioPlayer* audioplayer){
    namedWindow(windowName, cv::WINDOW_NORMAL);
    cv::Mat frame;
    while(1){
        // Capture frame-by-frame
        cam.cam >> frame;
        // If the frame is empty, break immediately
        if (frame.empty())
          break;
        //the box
        int h = 300;
        int w  = 200;
        bool captured = draw_on_frame(frame, w, h);
        //SHOW (captured);
        if (captured){
            cv::Rect inside_rect(frame.cols/2 - (w/2), (frame.rows/2) - (h/2), w, h);
            cv::Mat croppedRef(frame, inside_rect);
            cv::Mat cropped;
            // Copy the data into new matrix
            croppedRef.copyTo(cropped);
            std::vector<dlib::matrix<float,0,1>> encoding = facehasher.rgb_to_landmarkvector(cropped);
            if (encoding.empty()){
                std::cout << "Studpid Algo thought there was face, but NOOO!!!" << std::endl;
                display_text(frame, "Please wait", "Entrance");
                return;
            }
            //calc hashcode
            uint hashcode = hashing(encoding, facehasher);
            for (size_t i = 0; i < encoding.size(); ++i)
                {
                    for (size_t j = 0; j < encodings.size(); ++j)
                    {
                        //compare new encoding with every encoding so far
                        // Faces are connected in the graph if they are close enough.  Here we check if
                        // the distance between two face descriptors is less than 0.6, which is the
                        // decision threshold the network was trained to use.  Although you can
                        // certainly use any other threshold you find useful.
                        if (length(encoding[i]-encodings[j]) < 0.6){

                           display_text(frame, "Checked In", "Entrance");
                           std::cout << "Nothing new here to see!" << std::endl;
                           return;
                        }
                    }
                }
            // add encoding to encodings
            if (not encoding.empty()){
                encodings.push_back(encoding[0]);
                std::cout << "About to save new encoding!";
                audioplayer->play_hashcode(hashcode);
                display_hashcode(frame, hashcode, "Entrance");
                train_model(model, cropped, hashcode);
            }
        }
        cv::imshow("Entrance", frame );
        cv::waitKey(50);
      }
}

void EntranceManager::show_in_background(std::string windowName, cv::Ptr<cv::face::LBPHFaceRecognizer>& model, AudioPlayer* audioplayer){
    std::thread t(&EntranceManager::show_stream, this, windowName, std::ref(model), std::ref(audioplayer));
    t.join();
}

