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

        pixMat(std::string im_name, int blockH = 5, int blockW = 5, bool average = true, bool white = false)
        {
            // TODO: unsure what this-> should refer to for imread
            this->img = imread(im_name, IMREAD_COLOR);


            // TODO: replace with proper error raising
            if (this->img.empty())
            {
                std::cout << "Could not read image: " << im_name << std::endl;
            }

            // outImg = Mat(inImg.rows, inImg.cols, CV_8UC3, Scalar(0, 0, 0));

            this->white = average;
            this->white = white;
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
        }

        void pixelate()
        {
            // TODO: write without double for loop
            // parallise using #pragma omp parallel for
            for (int x = 0; x < this->img.cols - blockW; x += blockW)
            {
                for (int y = 0; y < this->img.rows - blockH; y += blockH)
                {
                    // cycle through all channels (necessary?)
                    for (int c = 0; c < 3; c++)
                    {
                        if (average)
                        {
                            setBlock(getBrightest(x, y), x, y);
                        }
                        else
                        {
                            //gets the average of the pixel block and then sets all the pixels in that block to that value
                            setBlock(getAverage(x, y), x, y);
                        }
                    }
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
                    Scalar current = this->img.at<Scalar>(blockH + y, blockW + x);
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
    std::cout << "Run\n";
    // TODO
    // replace with proper argument flags and processing
    if (argc < 5)
    {
        std::cout << "Error not enough arguments\n1: the height of the pixelling blocks\n2: the width of the pixelling blocks\n3: use brightest pixel (1) or average (any other value)\n4: path of image to be pixelled\n5: (optional) file path of output, if non-specified the image will just be displayed\n";
        return 2;
    }

    String name = argv[3];
    bool average = true, white=false;
    int blockH = std::stoi(argv[1]);
    int blockW = std::stoi(argv[2]);

    pixMat img = pixMat(name, blockH, blockW, average, white);
    // pixMat img2 = static_cast<pixMat>(imread(name, IMREAD_COLOR));
    //int height = (img.rows / blockH) * blockH;
    //int widths = (img.cols / blockW) * blockW;


    if (argc >= 6)
    {
        String fileName(argv[5]);
        //fileName += ".png";
        img.write(fileName);
    }
    else
    {
        img.show();
        img.pixelate();
        waitKey(0);
        img.show();
        waitKey(0);
    }
    return 0;
}
