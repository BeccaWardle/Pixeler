#include <iostream>
#include <string>
#include <cmath>
//#include <unistd.h>
//#include <stdexcept>

#include <opencv2/core.hpp>
#include <opencv2/core/types.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include "pixMat.h"


// Public Block
pixMat::pixMat() = default;

pixMat::pixMat(std::string im_name)
{
    bool succ = load(im_name);

    if (!succ)
    {
        std::cout << "Failed to load image";
        exit(EXIT_FAILURE);
    }
}

pixMat::pixMat(std::string im_name, bool overwrite)
{
    load(im_name);

    if (overwrite)
        this->outMat = cv::Mat(this->mat);
    else
        this->outMat = cv::Mat(this->mat.rows, this->mat.cols, CV_8UC3, cv::Scalar(0, 0, 0));
}

pixMat::pixMat::pixMat(std::string im_name, bool average, bool white)
{
    this->average = average;
    this->white_override = white;
    load(im_name);
}

pixMat::pixMat(std::string im_name, uint32_t blockW, uint32_t blockH)
{
    this->blockW = blockW;
    this->blockH = blockH;
    load(im_name);
}

pixMat::pixMat(std::string im_name, uint32_t blockW, uint32_t blockH, bool average, bool white)
{
    this->blockW = blockW;
    this->blockH = blockH;
    this->average = average;
    this->white_override = white;
    load(im_name);
}

bool pixMat::load(std::string file_name)
{
    this->mat = cv::imread(file_name, cv::IMREAD_COLOR);

    if (this->mat.empty())
        return false;
    else
        return true;
}

bool pixMat::write(std::string name)
{
    // This function just calls the built in opencv function
    // Don't re-write the wheel
    return cv::imwrite(name, mat);
}

void pixMat::show()
{
    show("image", 0);
}

void pixMat::show(std::string window_name, const int wait)
{
    //ensures the window is always placed in the top left corner and the name can be reused later in the program
    std::string winName = window_name;
    cv::namedWindow(winName, cv::WINDOW_AUTOSIZE);
    cv::moveWindow(winName, 0, 0);

    cv::imshow(winName, mat);
    cv::waitKey(wait);
    cv::destroyAllWindows();
}

void pixMat::blockPixel()
{
    std::cout << "ImgH: " << mat.rows << "\tImgW: " << mat.cols << std::endl;
    std::cout << "blockW: " << blockW << "\tblockH: " << blockH << std::endl;

    // TODO: write without double for loop
    // parallise using #pragma omp parallel for
    for (int x = 0; x < this->mat.cols - blockW; x += blockW)
    {
        for (int y = 0; y < this->mat.rows - blockH; y += blockH)
        {
            cv::Scalar vals;

            if (average)
            {
                //std::cout << "Called average";
                vals = getAverage(x, y);
            }
            else
            {
                //gets the brightest pixel in block and then sets all the pixels in that block to that value
                //std::cout << "Called brightest";
                vals = getBrightest(x, y);
            }

            std::cout << "max x:" << x + blockW << ", max y: " << y + blockH << " vals: " << vals << "    ";
            setBlock(vals, x, y);
        }

        std::cout << std::endl;
    }

}

void pixMat::blockPixel(uint32_t dim)
{
    this->blockW = this->blockH = dim;
    blockPixel();
}

void pixMat::blockPixel(bool input)
{
    this->average = this->white_override = input;
    blockPixel();
}

void pixMat::blockPixel(uint32_t blockW, uint32_t blockH)
{
    this->blockW = blockW;
    this->blockH = blockH;
    blockPixel();
}

void pixMat::blockPixel(bool average, bool white)
{
    this->average = average;
    this->white_override = white;
    blockPixel();
}

void pixMat::blockPixel(uint32_t blockW, uint32_t blockH, bool average, bool white)
{
    this->blockW = blockW;
    this->blockH = blockH;
    this->average = average;
    this->white_override = white;
    blockPixel();
}


// Private Block
void pixMat::setBlock(cv::Scalar value, uint32_t startW, uint32_t startH, const float increase)
{

    for (int w = 0; w < blockW; w++)
    {
        for (int h = 0; h < blockH; h++)
        {
            // Loop over cv::Scalar and check value within min/max
            // warning is restricted to 4 which only holds for cv::Scalar data type
            for (short i = 0; i < 4; i++)
            {
                if (value[i] > 255)
                    value[i] = 255;
                else if (value[i] < 0)
                    value[i] = 0;
            }

            // std::cout << "Setting " << startH + h << " x "  << startW + w << " = " << value.mul(increase) << std::endl;
            mat.at<cv::Scalar>(startH + h, startW + w) = value.mul(increase);
        }
    }
}

cv::Scalar pixMat::getBrightest(uint32_t x, uint32_t y, const uint32_t threshold)
{
    cv::Scalar brightest = cv::Scalar(0, 0, 0);

    for (int x = 0; x < blockW; x++)
    {
        for (int y = 0; y < blockH; y++)
        {
            // std::cout << "Pixel: " << blockH + y << ", " << blockW + x << std::endl;
            cv::Scalar current = this->mat.at<cv::Scalar>(blockH + y, blockW + x);

            // std::cout << "Allocated current: " << current;
            for (int i = 0; i < 3; i++)
            {
                if (current[i] > brightest[i])
                    brightest = current;
            }
        }
    }

    int aver = 0;

    if (this->white_override)
    {
        for (int i = 0; i < 3; i++)
        {
            if (brightest[i] > 250)
                aver++;
            else
                continue;
        }
    }


    if (aver == 3)
        return (getAverage(x, y));
    else
        return brightest;
}

cv::Scalar pixMat::getAverage(uint32_t x, uint32_t y)
{
    // TODO: look at matrix manipulations to avoid double for loops
    cv::Scalar total = cv::Scalar(0, 0, 0);

    for (int x = 0; x < blockW; x++)
    {
        for (int y = 0; y < blockH; y++)
            total = total + this->mat.at<cv::Scalar>(blockH + y, blockW + x);
    }

    cv::Scalar average = (total / (double) (blockH * blockW));
    return average;
}
