#include "KinectCV.h"

CVFreenectDevice::CVFreenectDevice(freenect_context *_ctx, int _index)
    : Freenect::FreenectDevice(_ctx, _index), m_buffer_depth(FREENECT_DEPTH_11BIT),
    m_buffer_rgb(FREENECT_VIDEO_RGB), m_gamma(2048), m_new_rgb_frame(false),
    m_new_depth_frame(false), depthMat(cv::Size(640,480),CV_16UC1),
    rgbMat(cv::Size(640,480), CV_8UC3, cv::Scalar(0)),
    ownMat(cv::Size(640,480),CV_8UC3,cv::Scalar(0)) {

    for( unsigned int i = 0 ; i < 2048 ; i++) {
        float v = i/2048.0;
        v = std::pow(v, 3)* 6;
        m_gamma[i] = v*6*256;
    }

    pthread_mutex_init(&m_rgb_mutex, NULL);
    pthread_mutex_init(&m_depth_mutex, NULL);
}

// Do not call directly even in child
void CVFreenectDevice::VideoCallback(void* _rgb, uint32_t timestamp) {
   // std::cout << "RGB callback" << std::endl;
    pthread_mutex_lock(&m_rgb_mutex);
    uint8_t* rgb = static_cast<uint8_t*>(_rgb);
    rgbMat.data = rgb;
    m_new_rgb_frame = true;
    pthread_mutex_unlock(&m_rgb_mutex);
};

// Do not call directly even in child
void CVFreenectDevice::DepthCallback(void* _depth, uint32_t timestamp) {
    std::cout << "Depth callback" << std::endl;
    pthread_mutex_lock(&m_depth_mutex);
    uint16_t* depth = static_cast<uint16_t*>(_depth);
    depthMat.data = (uchar*) depth;
    m_new_depth_frame = true;
    pthread_mutex_unlock(&m_depth_mutex);
}

bool CVFreenectDevice::getVideo(cv::Mat& output) {
    pthread_mutex_lock(&m_rgb_mutex);
    if(m_new_rgb_frame) {
        cv::cvtColor(rgbMat, output, CV_RGB2BGR);
        m_new_rgb_frame = false;
        pthread_mutex_unlock(&m_rgb_mutex);
        return true;
    } else {
        pthread_mutex_unlock(&m_rgb_mutex);
        return false;
    }
}

bool CVFreenectDevice::getDepth(cv::Mat& output) {
    pthread_mutex_lock(&m_depth_mutex);
    if(m_new_depth_frame) {
        depthMat.copyTo(output);
        m_new_depth_frame = false;
        pthread_mutex_unlock(&m_depth_mutex);
        return true;
    } else {
        pthread_mutex_unlock(&m_depth_mutex);
        return false;
    }
}