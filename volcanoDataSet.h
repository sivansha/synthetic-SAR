#ifndef HEIGHTMAP_VOLCANODATASET_H
#define HEIGHTMAP_VOLCANODATASET_H

#include <opencv2/opencv.hpp>
#include <iostream>
#include <random>
#include <math.h>

using namespace cv;
using namespace std;

struct VolcanoData
{
    float height;
    float craterMaxHeight;
    float craterMinHeight;
    float craterMinHeightRatio;
    float craterFall;
    float craterFallRatio;
    unsigned baseLongAxisPixels;
    unsigned baseShortAxisPixels;
    unsigned craterLongAxisPixels;
    unsigned craterShortAxisPixels;
    Point baseCenter;
    Point craterCenter;
};

struct ImagesSet
{
    VolcanoData vd;
    cv::Mat DEM;
    cv::Mat DEMNoise;
    cv::Mat Reflection;
    cv::Mat Normals;
    cv::Mat DEMProjected;
    cv::Mat ReflectionProjected;
};

struct DataSet
{
    cv::Mat projectedDEMNormalGrads;
    cv::Mat projectedReflectionNormalGrads;
};

std::ostream& operator<<(std::ostream&, VolcanoData);
VolcanoData generateVolcanoData (std::mt19937);

#endif //HEIGHTMAP_VOLCANODATASET_H