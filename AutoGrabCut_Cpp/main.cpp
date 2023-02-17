#include <cassert>
#include <iostream>
#include <string>

#include <opencv4/opencv2/core.hpp>
#include <opencv4/opencv2/highgui.hpp>
#include <opencv4/opencv2/imgproc.hpp>
#include <opencv4/opencv2/videoio.hpp>
#include <opencv4/opencv2/imgcodecs.hpp>

#include "utils.hpp"

using namespace std;
using namespace cv;

#define _DEBUG

struct AutoGrabCut
{

  int skyPercentageInImageForGrabCutInit = 50;
  int genericInt = 5;
  int genericInt2 = 5;

  int scaling = 8;

  Mat bgrSmall;
  Mat bgrSmaller;

  Mat bgrFiltered;

  Mat bgdModel, fgdModel;
  Mat res;

  Mat detected_edges;
  Mat src_gray;

  Mat element7 = getStructuringElement(MORPH_RECT, Size(7, 7));
  Mat element5 = getStructuringElement(MORPH_RECT, Size(5, 5));
  Mat element3 = getStructuringElement(MORPH_RECT, Size(3, 3));

  static void cb_sky(int value, void *ptr)
  {
    auto T = (AutoGrabCut *)ptr;
    T->skyPercentageInImageForGrabCutInit = value;
  }

  static void cb_generic_forceImpair(int value, void *ptr)
  {
    auto T = (AutoGrabCut *)ptr;
    if (value % 2 == 0)
      value += 1;
    T->genericInt = max(3, value);
  }

  static void cb_generic(int value, void *ptr)
  {
    auto T = (AutoGrabCut *)ptr;
    T->genericInt = max(1, value);
  }

  static void cb_generic2(int value, void *ptr)
  {
    auto T = (AutoGrabCut *)ptr;
    T->genericInt2 = max(1, value);
  }

  static void cb_scaling(int value, void *ptr)
  {
    auto T = (AutoGrabCut *)ptr;
    T->scaling = max(2, value);
  }

public:
  AutoGrabCut()
  {
    createTrackbar("Sky Percentage init", "Result", &skyPercentageInImageForGrabCutInit, 100, cb_sky, this);
    createTrackbar("Generic Int", "Result", &genericInt, 255, cb_generic, this);
    createTrackbar("Generic Int2", "Result", &genericInt2, 255, cb_generic2, this);
    createTrackbar("ImageScaling", "Result", &scaling, 16, cb_scaling, this);
  }

  // void updateSkyPercentatge(int val)
  // {
  //   skyPercentageInImageForGrabCutInit = val;
  // }

  void imshowDebug(String name, Mat img)
  {
#ifdef _DEBUG
    imshow(name, img);
#endif
  }

  Mat detectEdges(const Mat &src_gray)
  {
    Mat grad_x, grad_y, edges;
    blur(src_gray, src_gray, Size(5, 5));

    // Canny( src_gray, detected_edges, 50, 60);
    // dilate(detected_edges, detected_edges, element);
    // morphologyEx(detected_edges, detected_edges, MORPH_CLOSE, element);
    // imshow("detected_edges", detected_edges);
    // mask = mask - detected_edges;

    Scharr(src_gray, grad_x, CV_16S, 1, 0, 1, 0, BORDER_DEFAULT);
    Scharr(src_gray, grad_y, CV_16S, 0, 1, 1, 0, BORDER_DEFAULT);
    // converting back to CV_8U
    convertScaleAbs(grad_x, grad_x);
    convertScaleAbs(grad_y, grad_y);
    addWeighted(grad_x, 0.5, grad_y, 0.5, 0, edges);
    edges.convertTo(edges, CV_8U);
    // morphologyEx(sobel, sobel, MORPH_OPEN, element);

    threshold(edges, edges, 64, 255, THRESH_BINARY);

    return edges;
  }

  Mat histogramBackProject(const Mat &image, Mat mask)
  {

    // return;

    Mat res = image.clone();
    Mat colorSpaced;
    // cvtColor(image, colorSpaced, COLOR_BGR2YCrCb);

    if (mask.cols != image.cols || mask.rows != image.rows)
    {
      resize(mask, mask, {image.cols, image.rows}, 0, 0, INTER_LINEAR);
    }

    mask = mask & 0x01; // select both GC_FGD    = 1 and  GC_PR_FGD = 3

    int binSize = genericInt2;

    // we compute the histogram from the 0-th and 1-st channels
    int channels[] = {0, 2}; // b g r

    // Quantize the hue to 30 levels
    // and the saturation to 32 levels
    int c0Bins = binSize, c1Bins = binSize, c2Bins = binSize;
    int histSize[] = {c0Bins, c1Bins};

    float c0Ranges[] = {0, 255}; // hue varies from 0 to 179, see cvtColor
    // saturation varies from 0 (black-gray-white) to
    // 255 (pure spectrum color)
    float c1Ranges[] = {0, 255};
    float c2Ranges[] = {0, 255};
    const float *ranges[] = {c0Ranges, c1Ranges};
    Mat hist;

    calcHist(&image, 1, channels, mask,
             hist, 2, histSize, ranges,
             true, // the histogram is uniform
             false);

    // normalize histogram 0 - 255
    // int maskArea = countNonZero(mask);
    hist = (hist * 255) / sum(hist);

    // cout << sum(hist) << " " << endl;

    Mat histVisu;
    resize(hist, histVisu, {256, 256}, 0, 0, INTER_NEAREST);
    imshowDebug("hist", histVisu * 255);

    Mat backproj;
    calcBackProject(&image, 1, channels, hist, backproj, ranges, 255, true);

    imshowDebug("BackProj", backproj);
    threshold(backproj, backproj, genericInt, 255, THRESH_BINARY);
    imshowDebug("backproj_tresh", backproj);

    return backproj;
  }

  Mat runAutoSkySegmentation(const Mat &imageInputBGR)
  {

    cvtColor(imageInputBGR, src_gray, COLOR_BGR2GRAY);
    Mat edges = detectEdges(src_gray);
    imshowDebug("edges", edges);

    int pyramideScale1 = scaling;
    int pyramideScale2 = scaling/2;

    resize(imageInputBGR, bgrSmall, {imageInputBGR.cols / pyramideScale1, imageInputBGR.rows / pyramideScale1}, 0, 0, INTER_LINEAR);
    // blur(bgrSmall, bgrSmall, {3, 3});
    medianBlur(bgrSmall, bgrSmall, 3);
    imshowDebug("bgrSmall", bgrSmall);

    resize(bgrSmall, bgrSmaller, {bgrSmall.cols / pyramideScale2, bgrSmall.rows / pyramideScale2}, 0, 0, INTER_LINEAR);
    // medianBlur(bgrSmaller, bgrSmaller, 3);
    // blur(bgrSmaller, bgrSmaller, {3, 3});

    // take first X% as sky region init
    cv::Mat mask = cv::Mat::zeros(cv::Size(bgrSmaller.cols, bgrSmaller.rows), CV_8UC1);
    mask = GC_PR_BGD; // probable background == not sky
    Rect rect(0, 0, bgrSmaller.cols, bgrSmaller.rows * skyPercentageInImageForGrabCutInit / 100.);
    rectangle(mask, rect, GC_PR_FGD, -1); // set the top as probable foreground == sky

    // remove edges from probabble sky
    Mat edges_small;
    resize(edges, edges_small, {mask.cols, mask.rows}, 0, 0, INTER_NEAREST);
    // mask should be GC_PR_BGD = 2,  //!< a possible background pixel where there is an edge
    mask = mask - edges_small + edges_small / 128;

    imshowDebug("mask_init", mask * 64);
    grabCut(bgrSmaller, mask, rect, bgdModel, fgdModel, 1, GC_INIT_WITH_MASK);
    imshowDebug("mask_after_grabCut", mask * 64);

    // histogramTest(imageInputBGR, mask);

    imshowDebug("pass1", getImageWithMaskOverlay(imageInputBGR, mask));

    resize(mask, mask, {bgrSmall.cols, bgrSmall.rows}, 0, 0, INTER_NEAREST);

    erode(mask, mask, element3);

    resize(edges, edges_small, {mask.cols, mask.rows}, 0, 0, INTER_NEAREST);
    // mask should be GC_PR_BGD = 2,  //!< a possible background pixel where there is an edge
    mask = mask - edges_small + edges_small / 128;

    erode(mask, mask, element3);
    imshowDebug("mask eroded", mask * 64);

    imshowDebug("pass1 eroded", getImageWithMaskOverlay(imageInputBGR, mask));

    try
    {
      grabCut(bgrSmall, mask, rect, bgdModel, fgdModel, 1, GC_INIT_WITH_MASK);
    }
    catch (const std::exception &e)
    {
      grabCut(bgrSmall, mask, rect, bgdModel, fgdModel, 1, GC_EVAL);
    }

    mask = mask & 0x01;
    resize(mask, mask, {imageInputBGR.cols, imageInputBGR.rows}, 0, 0, INTER_LINEAR);

    mask = mask - edges;
    mask = mask & 0x01;
    // morphologyEx(mask, mask, cv::MORPH_DILATE, element);
    Mat maskD;
    Mat elementE = getStructuringElement(MORPH_ELLIPSE, Size(5, 5));

    blur(mask, mask, {3, 3});
    mask *= 255;

    // dilate(mask, mask, elementE);

    // Mat backProj = histogramBackProject(imageInputBGR, mask);
    // int erosion_size2 = 7;
    // Mat element2 = getStructuringElement(MORPH_RECT, Size(2 * erosion_size2 + 1, 2 * erosion_size2 + 1), Point(erosion_size2, erosion_size2));
    // Mat maskBigger;
    // Mat maskSmaller;
    // dilate(mask, maskBigger, element2);
    // erode(mask, maskSmaller, element2);
    // maskBigger -= maskSmaller;
    // //dilate(edges, maskBigger, element);
    // imshow("maskBorder", (maskBigger &(backProj & 1)) * 255);

    // mask =  (mask - maskBigger) | (maskBigger & (backProj & 1));

    return mask;
  }
};

bool endsWith(const string &s, const string &suffix)
{
  return s.size() >= suffix.size() && s.rfind(suffix) == (s.size() - suffix.size());
}

int main(int argc, char *argv[])
{
  string filename = "/home/cstrauss/PERSO/StreamFlow/lofoten.mp4";
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

  bool isImage = endsWith(filename, "png") || endsWith(filename, "jpg") || endsWith(filename, "jpeg");
  ;

  VideoCapture cap;
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
    hconcat(image, result, result);
    resize(result, result, Size(result.cols / 2, result.rows / 2));

    resize(mask, mask, Size(mask.cols / 2, mask.rows / 2));
    if (result.cols > 1920)
    {
      resize(result, result, Size(result.cols / 2, result.rows / 2));
      resize(mask, mask, Size(mask.cols / 2, mask.rows / 2));
    }
     

    imshow("sky mask", mask*255);

    imshow("Result", result);
    // imshow("input",image);

    key = waitKey(1);
  }

  return 0;
}
