#ifndef pixMat_H
#define pixMat_H

#include <string>
#include <cstdint>

#include <opencv2/core.hpp>


class pixMat
{
    public:
        uint32_t blockH = 10, blockW = 10, max_thresh = 255;
        bool average = false, white_override = true;

        pixMat();
        pixMat(std::string im_name);
        pixMat(std::string im_name, bool overwrite);
        pixMat(std::string im_name, bool average, bool white);
        pixMat(std::string im_name, uint32_t blockW, uint32_t blockH);
        pixMat(std::string im_name, uint32_t blockW, uint32_t blockH, bool average, bool white);

        bool load(std::string file_name);
        bool write(std::string file_name);
        void show();
        void show(std::string window_name, const int wait = 0);

        void blockPixel();
        void blockPixel(uint32_t dim);
        void blockPixel(bool input);
        void blockPixel(uint32_t blockW, uint32_t blockH);
        void blockPixel(bool average, bool white);
        void blockPixel(uint32_t blockW, uint32_t blockH, bool average, bool white_override);

    private:
        cv::Mat mat;
        cv::Mat outMat;
        void setBlock(cv::Scalar value, uint32_t startW, uint32_t startH, const float increase = 1.0);
        cv::Scalar getBrightest(uint32_t x, uint32_t y, const uint32_t threshold = 3 * 250);
        cv::Scalar getAverage(uint32_t x, uint32_t y);
};
#endif
