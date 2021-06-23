#include <iostream>
#include <string>
#include <cmath>
#include <unistd.h>
#include <stdexcept>

#include <opencv2/core.hpp>
#include <opencv2/core/types.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
using namespace cv;


// expand on Scalar operations (adding, multiplying, logical operations)
class pixMat
{
    // build off CV2 Mat (allows for show and save operations)
    public:
        int blockH, blockW;
        bool average, white;
        Mat img;

        pixMat(std::string im_name)
        {
            this->img = imread(im_name, IMREAD_COLOR);

            // TODO: replace with proper error raising
            if (this->img.empty())
            {
                std::cout << "Could not read image: " << im_name << std::endl;
                exit(EXIT_FAILURE);
            }

            // outImg = Mat(inImg.rows, inImg.cols, CV_8UC3, Scalar(0, 0, 0));
        }

        void write(std::string name)
        {
            imwrite(name, img);
        }

        void show()
        {

            //ensures the window is always placed in the top left corner and the name can be reused later in the program
            String winName = "image";
            namedWindow(winName, WINDOW_AUTOSIZE);
            moveWindow(winName, 0, 0);

            imshow(winName, img);
            std::cout << "Waiting on key to release\n";
            waitKey(0);
        }

        void pixelate(int blockW, int blockH, bool average, bool white)
        {
            this->blockW = blockW;
            this->blockH = blockH;
            this->average = average;
            this->white = white;

            // std::cout << "ImgH: " << img.rows << "\tImgW: " << img.cols << std::endl;
            // std::cout << "blockW: " << blockW << "\nblockH: " << blockH << std::endl;
            // TODO: write without double for loop
            // parallise using #pragma omp parallel for
            for (int x = 0; x < this->img.cols - blockW; x += blockW)
            {
                for (int y = 0; y < this->img.rows - blockH; y += blockH)
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
                            Scalar output = getBrightest(x, y);
                            // std::cout << "Brightest: " << output << std::endl;
                            setBlock(output, x, y);
                        }
                    //}
                }
            }
        }


    private:
        void setBlock(Scalar value, int startW, int startH, float increase = 1.0)
        {
            for (int w = 0; w < blockW; w++)
            {
                for (int h = 0; h < blockH; h++)
                {
                    // Loop over Scalar and check value within min/max
                    // warning is restricted to 4 which only holds for Scalar data type
                    for (short i = 0; i < 4; i++)
                    {
                        if (value[i] > 255)
                            value[i] = 255;
                        else if (value[i] < 0)
                            value[i] = 0;
                    }
                    // std::cout << "Setting " << startH + h << " x "  << startW + w << " = " << value.mul(increase) << std::endl;
                    img.at<Scalar>(startH + h, startW + w) = value.mul(increase);
                }
            }
        }

        Scalar getBrightest(int x, int y)
        {
            Scalar brightest = Scalar(0, 0, 0);
            for (int x = 0; x < blockW; x++)
            {
                for (int y = 0; y < blockH; y++)
                {
                    // std::cout << "Pixel: " << blockH + y << ", " << blockW + x << std::endl;
                    Scalar current = this->img.at<Scalar>(blockH + y, blockW + x);
                    // std::cout << "Allocated current: " << current;
                    for (int i = 0; i < 3; i++)
                    {
                        if (current[i] > brightest[i])
                        {
                            brightest = current;
                        }
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
        Scalar getAverage(int x, int y)
        {
            Scalar total = Scalar(0, 0, 0);
            for (int x = 0; x < blockW; x++)
            {
                for (int y = 0; y < blockH; y++)
                {
                    total = total + this->img.at<Scalar>(blockH + y, blockW + x);
                }
            }
            Scalar average = (total / (blockH * blockW));
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
    //if (argc < 5)
    //{
        //std::cout << "Error not enough arguments\n1: the height of the pixelling blocks\n2: the width of the pixelling blocks\n3: use brightest pixel (1) or average (any other value)\n4: path of image to be pixelled\n5: (optional) file path of output, if non-specified the image will just be displayed\n";
        //return 2;
    //}

    String inName, outName = "";
    bool average, white = false;
    int blockH, blockW = 10;

    int opt;

    while ((opt = getopt(argc, argv, "h:w:ano:i:")) != EOF)
    {
        switch(opt)
        {
            case 'a':
                average = true;
                break;
            case 'n':
                white = true;
                break;
            case 'h':
                blockH = std::stoi(optarg, NULL, 10);
                break;
            case 'w':
                blockW = std::stoi(optarg, NULL, 10);
                break;
            case 'o':
                outName = (String) optarg;
                break;
            case 'i':
                inName = (String) optarg;
                break;
            case '?':
                if (optopt == 'w' || optopt == 'w' || optopt == 'i' || optopt == 'o')
                    fprintf(stderr, "-%c requires an argument\n", optopt);
                else if (isprint(optopt))
                    fprintf(stderr, "Unknown option -%c", optopt);
                else
                    fprintf(stderr, "Unknown option character %x\n", optopt);
            default:
                exit(EXIT_FAILURE);

        }
    }
    if (inName.size() == 0)
    {
        fprintf(stderr, "Program requires and input with -i flag\n");
        exit(EXIT_FAILURE);
    }

    pixMat img = pixMat(inName);
    std::cout << "Created basic image object\n";
    // pixMat img2 = static_cast<pixMat>(imread(name, IMREAD_COLOR));
    //int height = (img.rows / blockH) * blockH;
    //int widths = (img.cols / blockW) * blockW;

    std::cout << "Calling im.show()\n";
    img.show();
    img.pixelate(blockH, blockW, average, white);
    if (outName.size() != 0)
    {
        img.write(outName);
    }
    img.show();
    return 0;
}
