#ifndef HEIGHTMAP_UTILS_H
#define HEIGHTMAP_UTILS_H

#include <opencv2/opencv.hpp>
#include "volcanoDataSet.h"
#include "volcanoDataSet.h"
#include "PerlinNoise.h"

using namespace cv;
using namespace std;

void printMatAligned(Mat m);
VolcanoData getTestData();
float perlinNoise (Point, unsigned, unsigned , PerlinNoise);
void extrapolate_mat(cv::Mat&, int kernel_size=5);
cv::Mat gradients(cv::Mat &);

#endif //HEIGHTMAP_UTILS_H
