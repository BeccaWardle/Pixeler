#ifndef pixMat_H
#define pixMat_H

#include <string>

#include <opencv2/core.hpp>


class pixMat
{
    public:
        int blockH, blockW;
        bool average, white;
        cv::Mat mat;

        pixMat();
        pixMat(std::string im_name);

        bool load(std::string file_name);
        bool write(std::string file_name);
        bool show();
        bool show(std::string window_name);

        bool blockPixel(int blockW, int blockH, bool average, bool white_overload);

    private:
        void setBlock(cv::Scalar value, int startW, int startH, float increase = 1.0, const int threshold = 255 * 3);
        cv::Scalar getBrightest(int x, int y);
        cv::Scalar getAverage(int x, int y);
};
#endif
