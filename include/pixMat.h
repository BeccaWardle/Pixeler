#ifndef pixMat_H
#define pixMat_H

#include <string>

#include <opencv2/core.hpp>


class pixMat
{
    public:
        int blockH, blockW = 0;
        bool average, white_overload = false;

        pixMat();
        pixMat(std::string im_name);
        pixMat(std::string im_name, bool overwrite);
        pixMat(std::string im_name, bool average, bool white);
        pixMat(std::string im_name, int blockW, int blockH);
        pixMat(std::string im_name, int blockW, int blockH, bool average, bool white);

        bool load(std::string file_name);
        bool write(std::string file_name);
        void show();
        void show(std::string window_name, const int wait = 0);

        void blockPixel();
        void blockPixel(int dim);
        void blockPixel(bool input);
        void blockPixel(int blockW, int blockH);
        void blockPixel(bool average, bool white);
        void blockPixel(int blockW, int blockH, bool average, bool white_overload);

    private:
        cv::Mat mat;
        cv::Mat outMat;
        void setBlock(cv::Scalar value, int startW, int startH, const float increase = 1.0);
        cv::Scalar getBrightest(int x, int y, const int threshold = 3 * 250);
        cv::Scalar getAverage(int x, int y);
};
#endif
