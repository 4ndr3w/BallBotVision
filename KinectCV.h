//
// Created by Andrew on 5/5/16.
//


#include <opencv2/opencv.hpp>
#include <libfreenect/libfreenect.hpp>
#include <pthread.h>

#ifndef BALLFINDER_KINECTCV_H_H
#define BALLFINDER_KINECTCV_H_H

class CVFreenectDevice : public Freenect::FreenectDevice {
    public:
        CVFreenectDevice(freenect_context *_ctx, int _index);
        void VideoCallback(void* _rgb, uint32_t timestamp);
        void DepthCallback(void* _depth, uint32_t timestamp);
        bool getVideo(cv::Mat& output);
        bool getDepth(cv::Mat& output);
    private:
        std::vector<uint8_t> m_buffer_depth;
        std::vector<uint8_t> m_buffer_rgb;
        std::vector<uint16_t> m_gamma;
        cv::Mat depthMat;
        cv::Mat rgbMat;
        cv::Mat ownMat;
        pthread_mutex_t m_rgb_mutex;
        pthread_mutex_t m_depth_mutex;
        bool m_new_rgb_frame;
        bool m_new_depth_frame;
};

#endif //BALLFINDER_KINECTCV_H_H
