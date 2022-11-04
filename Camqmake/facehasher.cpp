#include "facehasher.h"
#include <boost/lexical_cast.hpp>

FaceHasher::FaceHasher()
{
    // The first thing we are going to do is load all our models.
    dlib::deserialize("shape_predictor_5_face_landmarks.dat") >> sp;
    dlib::deserialize("dlib_face_recognition_resnet_model_v1.dat") >> net;
} 

std::vector<dlib::matrix<float,0,1>> FaceHasher::rgb_to_landmarkvector(cv::Mat img_mat) {
    //loading the image to test
    // Turn OpenCV's Mat into something dlib can deal with.  Note that this just
    // wraps the Mat object, it doesn't copy anything.  So cimg is only valid as
    // long as temp is valid.  Also don't do anything to temp that would cause it
    // to reallocate the memory which stores the image as that will make cimg
    // contain dangling pointers.  This basically means you shouldn't modify temp
    // while using cimg.
    dlib::cv_image<dlib::bgr_pixel> img(img_mat);
    dlib::matrix<dlib::rgb_pixel> matrix;
    assign_image(matrix, img);  // memory copy happens here and only here
    // Run the face detector on the image, and for each face extract a
    // copy that has been normalized to 150x150 pixels in size and appropriately rotated
    // and centered.
    std::vector<dlib::matrix<dlib::rgb_pixel>> faces;
    for (auto face : detector(img))
    {
        std::cout << "Found something" << std::endl;
        auto shape = sp(img, face);
        dlib::matrix<dlib::rgb_pixel> face_chip;
        extract_image_chip(img, get_face_chip_details(shape,150,0.25), face_chip);
        faces.push_back(std::move(face_chip));

    }

    if (faces.size() == 0)
    {
        std::cout << "No faces found in image!" << std::endl;
        //dlib::matrix<float,0,1> empty_matrix;
        //empty_matrix = 0.0;
        std::vector<dlib::matrix<float,0,1>> empty_vector;
        //empty_vector.push_back(empty_matrix);
        return empty_vector;

    }

    // This call asks the DNN to convert each face image in faces into a 128D vector.
      // In this 128D vector space, images from the same person will be close to each other
      // but vectors from different people will be far apart.  So we can use these vectors to
      // identify if a pair of images are from the same person or from different people.
      std::vector<dlib::matrix<float,0,1>> face_descriptors = net(faces);
      return face_descriptors;
};

uint FaceHasher::step_into_encoding(std::vector<std::string> input, uint steps, uint variety){
    uint next_index = 0;
    //start first first entry of vector, use last three digits % size of vector as new index etc...
    for (uint i=0; i <= steps; i++){
        next_index = stoi(input[next_index].substr(input[next_index].size() - 3, 3)) % input.size();
    }
    uint result = stoi(input[next_index].substr(input[next_index].size() - 4, 4)) % variety;
    return result;
}

