#ifndef SIMPLEFACEDETECTOR_H
#define SIMPLEFACEDETECTOR_H

#include <opencv4/opencv2/dnn.hpp>

class SimpleFaceDetector
{
private:
    /// Face detection network
    cv::dnn::Net network_;
    /// Input image width
    const int input_image_width_;
    /// Input image height
    const int input_image_height_;
    /// Scale factor when creating image blob
    const double scale_factor_;
    /// Mean normalization values network was trained with
    const cv::Scalar mean_values_;
    /// Face detection confidence threshold
    const float confidence_threshold_;
public:
    SimpleFaceDetector();
    /// Detect faces in an image frame
    /// \param frame Image to detect faces in
    /// \return Vector of detected faces
    std::vector<cv::Rect> detect_face_rectangles(const cv::Mat &frame);
};

#endif // SIMPLEFACEDETECTOR_H
