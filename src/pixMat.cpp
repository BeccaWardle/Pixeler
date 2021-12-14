#include <iostream>
#include <string>
#include <cmath>
#include <vector>
//#include <unistd.h>
//#include <stdexcept>

#include <opencv2/core.hpp>
#include <opencv2/core/types.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include "pixMat.h"

using iScalar = cv::Scalar_<uint8_t>;

// Public Block
pixMat::pixMat() = default;

pixMat::pixMat(std::string im_name)
{
    bool succ = load(im_name);

    if (!succ)
    {
        fprintf(stderr, "Failed to load image");
        exit(EXIT_FAILURE);
    }
}

pixMat::pixMat(std::string im_name, bool overwrite)
{
    load(im_name);

    if (overwrite)
        this->outMat = cv::Mat(this->mat);
    else
        this->outMat = cv::Mat(this->mat.rows, this->mat.cols, CV_8UC3, iScalar(0.0, 0.0, 0.0));
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

void pixMat::show(int8_t image)
{
    show(image, "image", 0);
}

void pixMat::show(int8_t image ,std::string window_name, const int wait)
{
    // ensures the window is always placed in the top left corner and the name can be reused later in the program
    std::string winName = window_name;
    cv::namedWindow(winName, cv::WINDOW_AUTOSIZE);
    cv::moveWindow(winName, 0, 0);
    
    if (image == 0)
        cv::imshow(winName, mat);
    else
        cv::imshow(winName, outMat);
    cv::waitKey(wait);
    cv::destroyAllWindows();
}

void pixMat::blockPixel()
{
    fprintf(stderr, "\033[0;36mImgH: %i\tImgW: %i\n", mat.rows, mat.cols);
    fprintf(stderr, "\033[0;36mblockW: %i\tblockH: %i\n", blockW, blockH);

    // TODO: write without double for loop
    // parallise using #pragma omp parallel for
    for (uint32_t x = 0; x < this->mat.cols - blockW; x += blockW)
    {
        for (uint32_t y = 0; y < this->mat.rows - blockH; y += blockH)
        {
            iScalar vals;

            if (average)
            {
                vals = getAverage(x, y);
            }
            else
            {
                //gets the brightest pixel in block and then sets all the pixels in that block to that value
                vals = getBrightest(x, y);
            }

            //std::cout << "max x:" << x + blockW << ", max y: " << y + blockH << " vals: " << vals << "    ";
            setBlock(vals, x, y);
        }

        //std::cout << std::endl;
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
void pixMat::setBlock(iScalar value, uint32_t startW, uint32_t startH, const float increase)
{

    fprintf(stderr, " \033[0;31mSetting block: %i, %i\n", startW, startH);

    for (int w = 0; w < blockW; w++)
    {
        for (int h = 0; h < blockH; h++)
        {
            // Loop over iScalar and check value within min/max
            // warning is restricted to 4 which only holds for iScalar data type
            for (short i = 0; i < 4; i++)
            {
                if (value[i] > 255.0)
                    value[i] = 255.0;
                else if (value[i] < 0.0)
                    value[i] = 0.0;
            }

            // std::cout << "Setting " << startH + h << " x "  << startW + w << " = " << value.mul(increase) << std::endl;
            //if ((startH + h > 530) && (startW + w > 750))
            fprintf(stderr, " \033[0;37m[%i, %i]: (%i, %i, %i)", startH + h, startW + w, value[0], value[1], value[2]);

            this->outMat.at<iScalar>(startH + h, startW + w) = value;
        }

        fprintf(stderr, "\n");
    }
}

iScalar pixMat::getBrightest(uint32_t x, uint32_t y)
{
    fprintf(stderr, " \033[0;35mGetting Brit: %i, %i\n", x, y);
    std::vector<uint8_t> brightest(mat.channels(), 0);
    double brightest_sum = 0;

    for (int w= 0; w < blockW; w++)
    {
        for (int h = 0; h < blockH; h++)
        {
            // std::cout << "Pixel: " << blockH + y << ", " << blockW + x << std::endl;
            std::vector<uint8_t> current = this->mat.at<uint8_t>(h + y, w + x);
            double current_sum = 0;

            // std::cout << "Allocated current: " << current;
            for (int i = 0; i < brightest.size(); i++)
            {


                current_sum += current[i];
            }

            if (current_sum > brightest_sum)
            {
                brightest = current;
                brightest_sum = current_sum;
            }

            fprintf(stderr, "\033[0;33m [%u, %u]: (%u, %u, %u)\n", y + h, x + w, current[0], current[1], current[2]);
        }

    }

    int aver = 0;

    for (int i = 0; i < 3; i++)
    {
        if (brightest[i] > (this->threshold * 3))
            aver++;
    }
    fprintf(stderr, " \033[0;37mBrightest: %u, %u, %u\n", brightest[0], brightest[1], brightest[2]);

    if (this->white_override && aver == 3)
        brightest =  getAverage(x, y);
    else if (aver == 3)
        brightest = iScalar(this->threshold, this->threshold, this->threshold);

    fprintf(stderr, " \033[0;37mBrightest: %u, %u, %u\n", brightest[0], brightest[1], brightest[2]);
    return brightest;
}

iScalar pixMat::getAverage(uint32_t x, uint32_t y)
{
    // TODO: look at matrix manipulations to avoid double for loops
    iScalar total = iScalar(0, 0, 0);

    for (int x = 0; x < blockW; x++)
    {
        for (int y = 0; y < blockH; y++)
            total = total + this->mat.at<iScalar>(blockH + y, blockW + x);
    }

    iScalar average = (total / (double) (blockH * blockW));
    return average;
}
