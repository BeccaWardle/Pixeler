#include <iostream>
#include <string>
#include <cmath>
#include <unistd.h>
#include <stdexcept>

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
using namespace cv;


class Image 
{
    public:
        int blockH, blockW;
        short mode;
        Mat imImg, outImg;
        Image(std::string image_name, int blockH = 5, int blockW = 5, bool average, bool white)
        {
            // read in image
            img = imread(image_name, IMREAD_COLOR);
            
            if (img.empty())
            {
                std::cout << "Could not read image: " << image_name << std::endl;
                return 1;
            }

            outImg = Mat(inImg.rows, inImg.cols, CV_8UC3, Scalar(0, 0, 0));

            // compress 2 bools to single variable
            if (average && white)
                mode = 3;
            else if (average && ! white)
                mode = 2;
            else if (!average && white)
                mode = 1;
            else
                mode = 0;
        }

        void pixelate()
        {
            // TODO: write without double for loop
            // parallise using #pragma omp parallel for
            for (int x = 0; x < inImg.cols - blockW; x += blockW)
            {
                for (int y = 0; y < inImg.rows - blockH; y += blockH)
                {
                    // cycle through all channels (necessary?)
                    //for (int c = 0; c < 3; c++)
                    //{

                        if (stoi(argv[3]) == 1)
                        {
                            setBlock(getBrightest(img, x, y, blockW, blockH, c), x, y, blockW, blockH, c, newImg);
                        }
                        else
                        {
                            //gets the average of the pixel block and then sets all the pixels in that block to that value
                            setBlock(getAverage(img, x, y, blockW, blockH, c), x, y, blockW, blockH, c, newImg);
                        }
                    }
                }
            }

        }

        void setBlock(int value, int startW, int startH, int blockW, int blockH, int channel, Mat &img, float increase = 1.0)
        {
            for (int w = 0; w < blockW; w++)
            {
                for (int h = 0; h < blockH; h++)
                {
                    if (value > 255)
                    {
                        value = 255;
                    }
                    else if (value < 0)
                    {
                        value = 0;
                    }
                    img.at<Vec3b>(startH + h, startW + w)[channel] = value * increase;
                }
            }
        }

        int getBrightest()
        {
            int brightest = Scalar(0, 0, 0);
            for (int x = 0; x < blockW; x++)
            {
                for (int y = 0; y < blockH; y++)
                {
                    int current = img.at<Vec3b>(height + y, widths + x)[channel];
                    if (current > brightest)
                    {
                        brightest = current;
                    }
                }
            }
            // TODO
            // call getAverage if brightest is white(?)
            return brightest;
        }

        int getAverage()
        {
            int total = Scalar(0, 0, 0);
            for (int x = 0; x < blockW; x++)
            {
                for (int y = 0; y < blockH; y++)
                {
                    total += img.at<Vec3b>(height + y, widths + x)[channel];
                }
            }
            int area = blockH * blockW;
            int average = (total / area);
            return average;
        }

};

int main(int argc, char *argv[])
{
    /*
     argv[0]: program call
     argv[1]: height of pixelling block
     argv[2]: width of pixelling block
     argv[3]: brightest pixel or average (1 = brightest)
     argv[4]: image to be pixelled
     argv[5]: (optional) ouptut file
     */

    // TODO
    // replace with proper argument flags and processing
    if (argc < 5)
    {
        cout << "Error not enough arguments\n1: the height of the pixelling blocks\n2: the width of the pixelling blocks\n3: use brightest pixel (1) or average (any other value)\n4: path of image to be pixelled\n5: (optional) file path of output, if non-specified the image will just be displayed\n";
        return 2;
    }
    int blockH = stoi(argv[1]);
    int blockW = stoi(argv[2]);

    //int height = (img.rows / blockH) * blockH;
    //int widths = (img.cols / blockW) * blockW;

    if (argc >= 6)
    {
        string fileName(argv[5]);
        //fileName += ".png";
        imwrite(fileName, newImg);
    }
    else
    {
        //ensures the window is always placed in the top left corner and the name can be reused later in the program
        char winName[] = "image";
        namedWindow(winName, WINDOW_AUTOSIZE);
        moveWindow(winName, 0, 0);

        imshow(winName, img);
        waitKey(0);
        imshow(winName, newImg);
        waitKey(0);
        imwrite("wtest.png", newImg);
    }
    return 0;
}
