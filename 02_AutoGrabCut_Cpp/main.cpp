#include <cassert>
#include <iostream>
#include <string>

#include <opencv4/opencv2/core.hpp>
#include <opencv4/opencv2/highgui.hpp>
#include <opencv4/opencv2/imgproc.hpp>
#include <opencv4/opencv2/videoio.hpp>
#include <opencv4/opencv2/imgcodecs.hpp>

#include "utils.hpp"
#include "AutoGrabCut.hpp"

using namespace std;
using namespace cv;

// #define _DEBUG


bool endsWith(const string &s, const string &suffix)
{
  return s.size() >= suffix.size() && s.rfind(suffix) == (s.size() - suffix.size());
}

int main(int argc, char *argv[])
{
  string filename = "";
  string outputFilename = "";
  if (argc > 1)
  {
    // The first argument (argv[0]) is the name of the program
    // The second argument (argv[1]) is the first command-line argument
    std::cout << "The argument is: " << argv[1] << std::endl;
    filename = argv[1];
  }
  else
  {
    std::cout << "No argument provided." << std::endl;
  }

  bool interactive = true;
  if (argc == 3)
  {
    outputFilename = argv[2];
    interactive = false;
  }

  bool isImage = endsWith(filename, "png") || endsWith(filename, "jpg") || endsWith(filename, "jpeg");
  ;

  VideoCapture cap;
  VideoWriter videoWriter;

  namedWindow("Result", WINDOW_AUTOSIZE); // Create Window

  Mat image;

  if (!isImage)
  {
    cap.open(filename);
    cout << "open video" << endl;
    if (!cap.isOpened())
    {
      cerr << "cannot open video " << filename << endl;
      throw;
    }
    Mat firstImage;
    cap.read(firstImage);
    int codec = VideoWriter::fourcc('M', 'P', '4', 'V');
    videoWriter.open(outputFilename, codec, 25, firstImage.size());
  }
  else
  {
    image = imread(filename, IMREAD_COLOR);
  }

  char key = 0;
  AutoGrabCut skyGrabCut;
  Mat mask;

  while (key != 'q')
  {

    if (!isImage)
    {
      cap.read(image);
    }
    else
    {
      image = image;
    }

    if (image.empty())
      break;

    mask = skyGrabCut.runAutoSkySegmentation(image);

    // imshow("Overlay", getImageWithMaskOverlay(image, mask, {140, 72, 15}));
    Mat result = getImageWithMaskOverlay(image, mask);
    Mat concat;
    hconcat(image, result, concat);
    resize(concat, concat, Size(concat.cols / 2, concat.rows / 2));

    if (concat.cols > 1920)
    {
      resize(concat, concat, Size(concat.cols / 2, concat.rows / 2));
      resize(mask, mask, Size(mask.cols / 4, mask.rows / 4));
    }

    mask = mask * 255;
    if (interactive)
    {
      imshow("sky mask", mask);
      imshow("Result", concat);
      // imshow("input",image);

      //videoWriter.write(result);
      key = waitKey(1);
    }
    else
    {
      if (isImage)
      {
        imwrite(outputFilename + ".CppMask.png", mask);
        imwrite(outputFilename + ".CppComposite.jpg", result);
        break;
      }
      else
      {
          videoWriter.write(result);
      }
    }
  }

  return 0;
}
