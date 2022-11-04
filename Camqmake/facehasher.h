#ifndef FACEHASHER_H
#define FACEHASHER_H
#include <dlib/dnn.h>
#include <dlib/matrix.h>
#include <dlib/opencv.h>
#include <dlib/gui_widgets.h>
#include <dlib/clustering.h>
#include <dlib/string.h>
#include <dlib/image_io.h>
#include <dlib/image_processing/frontal_face_detector.h>


#include <fstream>
#include <sstream>

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

class FaceHasher
{

public:
    dlib::frontal_face_detector detector  = dlib::get_frontal_face_detector();
    dlib::shape_predictor sp;
    // And finally we load the DNN responsible for face recognition.
    anet_type net;

    FaceHasher();
    //facehasher needs rgb image! But LBPH - Algo needs grayscale!
    std::vector<dlib::matrix<float,0,1>> rgb_to_landmarkvector(cv::Mat img);
    uint step_into_encoding(std::vector<std::string> input, uint steps, uint variety);
};

#endif // FACEHASHER_H
