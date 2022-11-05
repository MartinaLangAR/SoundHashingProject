#include <opencv2/core/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/videoio.hpp>
#include <camera.h>
#include <facehasher.h>
#include <audioplayer.h>
#include <entrancemanager.h>
#include <exitmanager.h>
#include "iostream"
#include "stdlib.h"
#include <opencv2/face/facerec.hpp>
#include <opencv2/imgproc/imgproc.hpp>

#include <dlib/dnn.h>
#include <dlib/opencv.h>
#include <dlib/gui_widgets.h>
#include <dlib/clustering.h>
#include <dlib/string.h>
#include <dlib/image_io.h>
#include <dlib/image_processing/frontal_face_detector.h>
#include <fstream>
#include <sstream>
#include <thread>

#define SHOW(X) std::cout << # X " = " << (X) << std::endl

// stuff for dlib detector
//Finally, note that the face detector is fastest when compiled with at least
//SSE2 instructions enabled.  So if you are using a PC with an Intel or AMD
//chip then you should enable at least SSE2 instructions.  If you are using
//cmake to compile this program you can enable them by using one of the
//following commands when you create the build project:
//    cmake path_to_dlib_root/examples -DUSE_SSE2_INSTRUCTIONS=ON
//    cmake path_to_dlib_root/examples -DUSE_SSE4_INSTRUCTIONS=ON
//    cmake path_to_dlib_root/examples -DUSE_AVX_INSTRUCTIONS=ON
//This will set the appropriate compiler options for GCC, clang, Visual
//Studio, or the Intel compiler.  If you are using another compiler then you
//need to consult your compiler's manual to determine how to enable these
//instructions.  Note that AVX is the fastest but requires a CPU from at least
//2011.  SSE4 is the next fastest and is supported by most current machines.
template <template <int,template<typename>class,int,typename> class block, int N, template<typename>class BN, typename SUBNET>
using residual = dlib::add_prev1<block<N,BN,1,dlib::tag1<SUBNET>>>;

template <template <int,template<typename>class,int,typename> class block, int N, template<typename>class BN, typename SUBNET>
using residual_down = dlib::add_prev2<dlib::avg_pool<2,2,2,2,dlib::skip1<dlib::tag2<block<N,BN,2,dlib::tag1<SUBNET>>>>>>;

template <int N, template <typename> class BN, int stride, typename SUBNET>
using block  = BN<dlib::con<N,3,3,1,1,dlib::relu<BN<dlib::con<N,3,3,stride,stride,SUBNET>>>>>;

template <int N, typename SUBNET> using ares      = dlib::relu<residual<block,N,dlib::affine,SUBNET>>;
template <int N, typename SUBNET> using ares_down = dlib::relu<residual_down<block,N,dlib::affine,SUBNET>>;

template <typename SUBNET> using alevel0 = ares_down<256,SUBNET>;
template <typename SUBNET> using alevel1 = ares<256,ares<256,ares_down<256,SUBNET>>>;
template <typename SUBNET> using alevel2 = ares<128,ares<128,ares_down<128,SUBNET>>>;
template <typename SUBNET> using alevel3 = ares<64,ares<64,ares<64,ares_down<64,SUBNET>>>>;
template <typename SUBNET> using alevel4 = ares<32,ares<32,ares<32,SUBNET>>>;

using anet_type = dlib::loss_metric<dlib::fc_no_bias<128,dlib::avg_pool_everything<
                            alevel0<
                            alevel1<
                            alevel2<
                            alevel3<
                            alevel4<
                            dlib::max_pool<3,3,2,2,dlib::relu<dlib::affine<dlib::con<32,7,7,2,2,
                            dlib::input_rgb_image_sized<150>
                            >>>>>>>>>>>>;

// ----------------------------------------------------------------------------------------

std::vector<dlib::matrix<dlib::rgb_pixel>> jitter_image(
    const dlib::matrix<dlib::rgb_pixel>& img
);

// ----------------------------------------------------------------------------------------

cv::Mat read_image(std::string filename, bool gray) {
    std::string image_path = cv::samples::findFile(filename);
    cv::Mat img = cv::imread(image_path);
    if(img.empty())
    {
        std::cout << "Could not read the image: " << image_path << std::endl;
    }
    if (gray){
        cv::Mat grayscale;
        cvtColor(img, grayscale, cv::COLOR_BGR2GRAY);
        return grayscale;
    }
    return img;
}


int main(int, char**) {
    // The following lines create an LBPH model for
    // face recognition and train it with the images and
    // labels read from the given CSV file.
    //
    // The LBPHFaceRecognizer uses Extended Local Binary Patterns
    // (it's probably configurable with other operators at a later
    // point), and has the following default values
    //
    //      radius = 1
    //      neighbors = 8
    //      grid_x = 8
    //      grid_y = 8
    //
    // So if you want a LBPH FaceRecognizer using a radius of
    // 2 and 16 neighbors, call the factory method with:
    //
    //      cv::face::LBPHFaceRecognizer::create(2, 16);
    //
    // And if you want a threshold (e.g. 123.0) call it with its default values:
    //
    //      cv::face::LBPHFaceRecognizer::create(1,8,8,8,123.0)
    //
    cv::Ptr<cv::face::LBPHFaceRecognizer> model = cv::face::LBPHFaceRecognizer::create();
    // next is self-written FaceHasher to calc the hashcode from a 128-D array
    FaceHasher facehasher;
    EntranceManager entrancemanager(0);

    //init AudioPlayer and load soundfiles
    AudioPlayer audioplayer;
    audioplayer.create_sounds();
    entrancemanager.show_in_background("Entrance", model, &audioplayer);

    // The following line predicts the label of a given
    // test image:
    //int predictedLabel = model->predict(test_wrong_frame);

    return 0;
}
