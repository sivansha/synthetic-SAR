#include <opencv2/opencv.hpp>
#include <random>
#include <time.h>
#include "volcano.h"
#include "volcanoDataSet.h"
#include "utils.h"

using namespace cv;
using namespace std;

int main (int argc, char** argv)
{
    // measure run time
    clock_t tStart = clock();

    // Random devices
    std::srand(std::time(nullptr));
    std::random_device rd;

    const int numberOfVolcanoes = 251;
    ImagesSet imagesSet[numberOfVolcanoes];
    DataSet dataSet[numberOfVolcanoes];

    //VolcanoData test = getTestData();
    VolcanoData vd = VolcanoData();

    Mat demP, refP, demPBG, refPNG;

    string is;
    string path (".//data//dataset-1//");
    unsigned int randID = std::rand() % 20000;

    // data generatin
    for (int i = 0; i < numberOfVolcanoes; i++)
    {
        cout << i << endl;
        is = path + to_string(i) + "_" + to_string(randID) + "_";

        std::mt19937 generator(rd());
        vd = generateVolcanoData(generator);
//        imagesSet[i].vd = vd;

        syntheticVolcano::Volcano volcano(vd, 851);

        demP = volcano.getDEM2SAR().clone();
        refP = volcano.getReflection2SAR().clone();
        normalize(refP, refP, 0.0, 1.0, cv::NORM_MINMAX, CV_32FC1);

        demPBG = gradients(demP);
//        refPNG = gradients(refP);
//        normalize(demPBG, demPBG, 0.0, 1.0, cv::NORM_MINMAX, CV_32FC2);
//        normalize(refPNG, refPNG, 0.0, 1.0, cv::NORM_MINMAX, CV_32FC2);

        cv::imwrite(is + "_ProjGradDEM.exr", demPBG);
        //cv::imwrite(is + "_ProjGradRef.exr", refPNG);
        cv::imwrite(is + "_ProjRef.exr", refP);

        // DO NOT use when generating data. running out of memeory!
//        imagesSet[i].DEM = volcano.getDEM().clone();
//        imagesSet[i].Normals = volcano.getNormals().clone();
//        imagesSet[i].Reflection = volcano.getReflection().clone();
//        imagesSet[i].DEMProjected = volcano.getDEM2SAR().clone();
//        imagesSet[i].ReflectionProjected = volcano.getReflection2SAR().clone();
    }

    // visualization
//    Mat outMat;;
//    for (int i = 0; i < numberOfVolcanoes; i++)
//    {
//        is = to_string(i) + "_" + to_string(randID) + "_";
//
////        normalize(imagesSet[i].DEM, outMat, 0.0, 1.0, cv::NORM_MINMAX, CV_32F);
//        normalize(imagesSet[i].DEM, outMat, 0, 255, cv::NORM_MINMAX, CV_8U);
////        cout << imagesSet[i].vd;
////        imshow("DEM", outMat);
////        cv::waitKey();
//        cv::imwrite(is + "DEM.png", outMat);
//
////        normalize(imagesSet[i].Normals, outMat, 0.0, 1.0, cv::NORM_MINMAX, CV_32F);
//        normalize(imagesSet[i].Normals, outMat, 0, 255, cv::NORM_MINMAX, CV_8UC3);
////        cout << imagesSet[i].vd;
////        imshow("Normals", outMat);
////        cv::waitKey();
//        cv::imwrite(is + "Normals.png", outMat);
//
////        normalize(imagesSet[i].Reflection, outMat, 0.0, 1.0, cv::NORM_MINMAX, CV_32F);
//        normalize(imagesSet[i].Reflection, outMat, 0, 255, cv::NORM_MINMAX, CV_8U);
////        cout << imagesSet[i].vd;
////        imshow("Reflection", outMat);
////        cv::waitKey();
//        cv::imwrite(is + "Reflection.png", outMat);
//
////        normalize(imagesSet[i].DEMProjected, outMat, 0.0, 1.0, cv::NORM_MINMAX, CV_32F);
//        normalize(imagesSet[i].DEMProjected, outMat, 0, 255, cv::NORM_MINMAX, CV_8UC3);
////        cout << imagesSet[i].vd;
////        imshow("DEMProjected", outMat);
////        cv::waitKey();
//        cv::imwrite(is + "DEMProjected.png", outMat);
//
////        normalize(imagesSet[i].ReflectionProjected, outMat, 0.0, 1.0, cv::NORM_MINMAX, CV_32F);
//        normalize(imagesSet[i].ReflectionProjected, outMat, 0, 255, cv::NORM_MINMAX, CV_8U);
////        cout << imagesSet[i].vd;
////        imshow("ReflectionProjected", outMat);
////        cv::waitKey();
//        cv::imwrite(is + "ReflectionProjected.png", outMat);
//    }

    cout << ("Run time:\n", (double)(clock() - tStart)/CLOCKS_PER_SEC);

    return 0;
}