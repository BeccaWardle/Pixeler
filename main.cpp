#include <iostream>
#include <string>
#include <unistd.h>
#include <stdexcept>

#include "include/pixMat.h"


int main(int argc, char *argv[])
{
    /*
        -a use (average) <flag>, program uses average block value instead of peak
        -r (replace white) <flag>, if this flag is passed white blocks will use the average brightness instead
        -w (width) <val>, width of block to pass over the image in pixels
        -h (height) <val>, height of block to pass over the image in pixels
        -o (output) <name>, file path to save file to
        -i (input) <name>, file path of image to open
    */

    //freopen("stderr.txt", "w", stderr);
    std::string inName, outName = "";
    bool average = false, white = false;
    int blockH = 10, blockW = 10;

    int opt;

    while ((opt = getopt(argc, argv, "h:w:ano:i:")) != EOF)
    {
        switch(opt)
        {
        case 'a':
            average = true;
            break;

        case 'r':
            white = true;
            break;

        case 'h':
            blockH = std::stoi(optarg, NULL, 10);
            break;

        case 'w':
            blockW = std::stoi(optarg, NULL, 10);
            break;

        case 'o':
            outName = (std::string) optarg;
            break;

        case 'i':
            inName = (std::string) optarg;
            break;

        //case 'help':
        //printf("-a use (average) <flag>, program uses average block value instead of peak \n-r (replace white) <flag>, if this flag is passed white blocks will use the average brightness instead \n-w (width) <val>, width of block to pass over the image in pixels \n-h (height) <val>, height of block to pass over the image in pixels \n-o (output) <name>, file path to save file to \n-i (input) <name>, file path of image to open ");
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
        fprintf(stderr, "Program requires an input with -i flag\n");
        exit(EXIT_FAILURE);
    }

    pixMat img = pixMat(inName);
    fprintf(stderr, "Created basic image object\n");
    img.write("tmp_out.png");

    fprintf(stderr, "Calling im.show()\n");
    img.show(0);

    img.blockPixel(blockH, blockW, average, white);

    fprintf(stderr, "Saving to out.jpg\n");
    img.write("out.png");


    fprintf(stderr, "Displaying image\n");
    img.show(1);

    //if (outName.size() != 0)
    //img.write(outName);

    exit(EXIT_SUCCESS);
}
