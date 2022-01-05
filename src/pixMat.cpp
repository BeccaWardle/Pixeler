#include <iostream>
#include <string>
#include <cmath>
#include <vector>
#include <algorithm>
//#include <unistd.h>
//#include <stdexcept>

#include <opencv2/core.hpp>
#include <opencv2/core/types.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include "pixMat.h"

// Public Block
pixMat::pixMat() = default;

pixMat::pixMat(std::string im_name) : pixMat(im_name, false)
{}

pixMat::pixMat(std::string im_name, bool overwrite)
{
    bool succ = load(im_name);

    if (!succ)
    {
        fprintf(stderr, "Failed to load image");
        exit(EXIT_FAILURE);
    }

    if (overwrite)
        this->outMat = cv::Mat(this->mat);
    else
        // TODO: Scalar is not necessary as the data should be overwritten
        this->outMat = cv::Mat(this->mat.rows, this->mat.cols, CV_8UC3, cv::Scalar(0, 0, 0));
}

pixMat::pixMat::pixMat(std::string im_name, bool average, bool white) : pixMat(im_name, false)
{
    this->average = average;
    this->white_override = white;
}

pixMat::pixMat(std::string im_name, uint32_t blockW, uint32_t blockH) : pixMat(im_name, false)
{
    this->blockW = blockW;
    this->blockH = blockH;
}

pixMat::pixMat(std::string im_name, uint32_t blockW, uint32_t blockH, bool average, bool white) : pixMat(im_name, false)
{
    this->blockW = blockW;
    this->blockH = blockH;
    this->average = average;
    this->white_override = white;
}

bool pixMat::load(std::string file_name)
{
    this->mat = cv::imread(file_name, cv::IMREAD_COLOR);
    fprintf(stderr, "Mat dims: %i, %i\n", mat.rows, mat.cols);

    if (this->mat.empty())
        return false;
    else
        return true;
}

bool pixMat::write(std::string name)
{
    // This function just calls the built in opencv function
    // Don't re-write the wheel
    return cv::imwrite(name, outMat);
}

void pixMat::show(int8_t image)
{
    show(image, "image", 0);
}

void pixMat::show(int8_t image, std::string window_name, const int wait)
{
    // ensures the window is always placed in the top left corner and the name can be reused later in the program
    std::string winName = window_name;
    cv::namedWindow(winName, cv::WINDOW_NORMAL);
    cv::moveWindow(winName, 50, 50);

    if (image == 0)
        cv::imshow(winName, this->mat);
    else
        cv::imshow(winName, this->outMat);

    cv::waitKey(wait);
    cv::destroyAllWindows();
}

void pixMat::blockPixel()
{
    fprintf(stderr, "\033[0;36mImgH: %i\tImgW: %i\n", mat.rows, mat.cols);
    fprintf(stderr, "\033[0;36mblockW: %i\tblockH: %i\n", blockW, blockH);

    this->print = true;

    if (!mat.isContinuous() || !outMat.isContinuous())
    {
        fprintf(stderr, "Images (mat || outMat) are not continuous\n");
        exit(EXIT_FAILURE);
    }

    // TODO: write without double for loop
    // parallise using #pragma omp parallel for
    //#pragma omp parallel for

    for (uint32_t y = 0; y <= mat.rows - blockH; y += blockH)
    {
        for (uint32_t x = 0; x <= mat.cols - blockW; x += blockW)
        {
            std::vector<uint8_t> vals;

            if (average)
                vals = getAverage(y, x);
            else
                //gets the brightest piyel in block and then sets all the piyels in that block to that value
                vals = getBrightest(y, x);

            setBlock(vals, y, x);
        }
    }

    return;
    // tmp variables to maintain the block dims
    auto main_blockH = this->blockH, main_blockW = this->blockW;

    fprintf(stderr, "\nWriting collumn\n");
    uint32_t x = mat.cols - (mat.cols % blockW) - 1;
    this->blockW = mat.cols % blockW;

    fprintf(stderr, "BlockH: %u\tBlockW: %u", this->blockH, this->blockW);

    for (uint32_t y = 0; y < mat.rows - blockH; y += blockH)
    {
        fprintf(stderr, "\nx, y: %u, %u\tmaxX, maxY: %u, %u", x, y, x + blockW, y + blockH);
        std::vector<uint8_t> vals;

        if (average)
            vals = getAverage(y, x);
        else
            // gets the brightest piyel in block and then sets all the piyels in that block to that value
            vals = getBrightest(y, x);

        setBlock(vals, y, x);
        // this->print = false;
    }

    return;

    fprintf(stderr, "\nWriting row\n");
    // reset blockW size
    this->blockW = main_blockW;

    uint32_t y = mat.rows - (mat.rows % blockH);
    this->blockH = mat.rows % blockH;

    fprintf(stderr, "BlockH: %u\tBlockW: %u", this->blockH, this->blockW);

    for (uint32_t x = 0; x < mat.cols - blockW; x += blockW)
    {
        write("out.png");
        std::vector<uint8_t> vals;
        // fprintf(stderr, "x, y: %u, %u\n", x, y);

        fprintf(stderr, "\nx, y: %u, %u\tmaxX, maxY: %u, %u", x, y, x + blockW, y + blockH);

        if (average)
            vals = getAverage(y, x);
        else
            //gets the brightest piyel in block and then sets all the piyels in that block to that value
            vals = getBrightest(y, x);

        setBlock(vals, y, x);
    }

    this->blockW = main_blockW;
    this->blockH = main_blockH;

    fprintf(stderr, "\n");
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
void pixMat::setBlock(std::vector<uint8_t> value, uint32_t x, uint32_t y, const float increase)
{
    uint8_t *pixelPtr = (uint8_t *) outMat.data;
    uint32_t index = 0;
    std::string StrIndex = "", StrVector = "";
    int printed = 0;

    //fprintf(stderr, " \033[0;31mSetting block: %u, %u\n", x, y);
    for (uint32_t h = 0; h < blockH; h++)
    {
        for (uint32_t w = 0; w < blockW; w++)
        {
            // Loop over iScalar and check value within min/max

            // std::cout << "Setting " << startH + h << " x "  << startW + w << " = " << value.mul(increase) << std::endl;
            //if ((startH + h > 530) && (startW + w > 750))
            for (uint8_t i = 0; i < value.size(); i++)
            {
                index = (x + w) * mat.cols * mat.channels() + (y + h) * mat.channels() + i;
                StrIndex.append(std::to_string(index) + ", ");
                StrVector.append(std::to_string(value.at(i)) + ", ");

                pixelPtr[index] = value.at(i);
            }

            if (print)
            {
                printed += 1;
                fprintf(stderr, "Set %s: [%s]\t", StrIndex.c_str(), StrVector.c_str());
            }

            //this->outMat.at<iScalar>(startH + h, startW + w) = value;
            StrIndex = "", StrVector = "";

        }

        if (print && printed == 0)
            fprintf(stderr, "Didn't print: [x,y]= [%u, %u]\n", x, y);
        else if (print)
            fprintf(stderr, "1\n");
    }

}

std::vector<uint8_t> pixMat::getBrightest(uint32_t x, uint32_t y)
{
    if (print)
        fprintf(stderr, "\n");

    //fprintf(stderr, " \033[0;35mGetting Brit: %u, %u\n", x, y);
    uint8_t chan_n = mat.channels();
    std::vector<uint8_t> brightest(chan_n, 0);
    double brightest_sum = 0;

    uint32_t index = 0;
    uint8_t *pixelPtr = (uint8_t *) mat.data;

    std::string StrIndex = "", StrVector = "";
    int printed = 0;

    for (uint32_t h = 0; h < blockH; h++)
    {
        for (uint32_t w = 0; w < blockW; w++)
        {
            // std::cout << "Pixel: " << blockH + y << ", " << blockW + x << std::endl;
            std::vector<uint8_t> current(chan_n);
            double current_sum = 0;

            // std::cout << "Allocated current: " << current;
            for (uint32_t i = 0; i < brightest.size(); i++)
            {
                index = (x + w) * mat.cols * chan_n + (y + h) * chan_n + i;

                StrIndex.append(std::to_string(index) + ", ");
                StrVector.append(std::to_string(current.at(i)) + ", ");

                current.at(i) = pixelPtr[index];
                current_sum += current.at(i);
            }

            if (current_sum > brightest_sum)
            {
                brightest = current;
                brightest_sum = current_sum;
            }

            if (print)
                fprintf(stderr, "Got %s: [%s]\t", StrIndex.c_str(), StrVector.c_str());

            if (StrIndex == "")
            {
                fprintf(stderr, "h, w: [%u, %u]", h, w);
                printed += 1;
            }

            StrIndex = "", StrVector = "";
        }

        if (print && printed == 0)
            fprintf(stderr, "Didn't print: [x,y]= [%u, %u]\n", x, y);
        else if (print)
            fprintf(stderr, "3\n");
    }

    uint8_t aver = 0;

    for (uint8_t i = 0; i < brightest.size(); i++)
    {
        // unnecessary?
        if (brightest.at(i) > threshold)
            aver++;
        else if (brightest.at(i) < 0)
            brightest.at(i) = 0;
    }

    if (this->white_override && aver == 3)
        brightest =  getAverage(x, y);
    else if (aver == 3)
        std::fill(brightest.begin(), brightest.end(), threshold);

    //fprintf(stderr, " \033[0;37mBrightest: %u, %u, %u\n", brightest[0], brightest[1], brightest[2]);
    return brightest;
}

std::vector<uint8_t> pixMat::getAverage(uint32_t x, uint32_t y)
{
    // TODO: look at matrix manipulations to avoid double for loops
    std::vector<uint8_t> average (mat.channels(), 0);
    uint8_t *pixelPtr = (uint8_t *) mat.data;
    uint8_t chan_n = mat.channels();

    for (uint32_t h = 0; h < blockH; h++)
    {
        for (uint32_t w = 0; w < blockW; w++)
        {
            for (uint8_t i = 0; i < chan_n; i++)

                average.at(i) += pixelPtr[(x + w) * mat.cols * chan_n + (y + h) * chan_n + i];
        }
    }

    uint32_t size = blockH * blockW;

    std::transform(average.begin(), average.end(), average.begin(), [size](uint8_t c)
    {
        return c / (size);
    });
    return average;
}
