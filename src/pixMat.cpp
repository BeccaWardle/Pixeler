#include <iostream>
#include <string>
#include <cmath>
#include <unistd.h>
#include <stdexcept>

#include <opencv2/core.hpp>
#include <opencv2/core/types.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>


// expand on cv::Scalar operations (adding, multiplying, logical operations)
class pixMat
{
        // build off CV2 Mat (allows for show and save operations)
    public:
        int blockH, blockW;
        bool average, white;
        cv::Mat mat;

        pixMat(std::string im_name)
        {
            this->mat = imread(im_name, IMREAD_COLOR);

            // TODO: replace with proper error raising
            if (this->mat.empty())
            {
                std::cout << "Could not read image: " << im_name << std::endl;
                exit(EXIT_FAILURE);
            }

            // outImg = Mat(inImg.rows, inImg.cols, CV_8UC3, cv::Scalar(0, 0, 0));
        }

        void write(std::string name)
        {
            imwrite(name, mat);
        }

        void show()
        {
            //ensures the window is always placed in the top left corner and the name can be reused later in the program
            String winName = "image";
            namedWindow(winName, WINDOW_AUTOSIZE);
            moveWindow(winName, 0, 0);

            imshow(winName, mat);
            waitKey(0);
            destroyAllWindows();
        }

        void blockPixel(int blockW, int blockH, bool average, bool white)
        {
            this->blockW = blockW;
            this->blockH = blockH;
            this->average = average;
            this->white = white;

            // std::cout << "ImgH: " << mat.rows << "\tImgW: " << mat.cols << std::endl;
            // std::cout << "blockW: " << blockW << "\nblockH: " << blockH << std::endl;
            // TODO: write without double for loop
            // parallise using #pragma omp parallel for
            for (int x = 0; x < this->mat.cols - blockW; x += blockW)
            {
                for (int y = 0; y < this->mat.rows - blockH; y += blockH)
                {
                    // cycle through all channels (necessary?)
                    //for (int c = 0; c < 3; c++)
                    //{
                    // std::cout << "Now working on: " << x << " x " << y << std::endl;
                    if (average)
                    {
                        // std::cout << "Called average\n";
                        setBlock(getAverage(x, y), x, y);
                    }
                    else
                    {
                        //gets the brightest pixel in block and then sets all the pixels in that block to that value
                        // std::cout << "Called brightest\n";
                        cv::Scalar output = getBrightest(x, y);
                        // std::cout << "Brightest: " << output << std::endl;
                        setBlock(output, x, y);
                    }

                    //}
                }
            }
        }


    private:
        void setBlock(cv::Scalar value, int startW, int startH, float increase = 1.0)
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

        cv::Scalar getBrightest(int x, int y)
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

            if (white)
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

        // TODO: look at matrix manipulations to avoid double for loops
        cv::Scalar getAverage(int x, int y)
        {
            cv::Scalar total = cv::Scalar(0, 0, 0);

            for (int x = 0; x < blockW; x++)
            {
                for (int y = 0; y < blockH; y++)
                    total = total + this->mat.at<cv::Scalar>(blockH + y, blockW + x);
            }

            cv::Scalar average = (total / (blockH * blockW));
            return average;
        }
};
