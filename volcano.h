#ifndef HEIGHTMAP_VOLCANO_H
#define HEIGHTMAP_VOLCANO_H

#include <opencv2/opencv.hpp>
#include <fstream>
#include "volcanoDataSet.h"
#include "PerlinNoise.h"
#include "utils.h"

using namespace cv;
using namespace std;

namespace syntheticVolcano
{
    enum AXES
    {
        LONG_AXIS,
        SHORT_AXIS
    };

    enum Ellipses
    {
        BASE,
        CRATER
    };

    class Ellipse
    {
    private:
        Point center;
        unsigned axes[2] = {0,0};

        float dist2center(const Point&);
        float angle2center(const Point&);
        float radius4angle(float);

    public:
        Ellipse();
        Ellipse(const Point&, int, int);

        Point getCenter();
        void getAxes(unsigned *);
        unsigned getLongAxis();
        unsigned getShortAxis();
        void setCenter(const Point&);
        void setCenter(int, int);
        void setAxis(const float*);
        void setAxis(float, float);

        bool isPointInside(const Point&);
        bool isPointInside(int x, int y);
        float pointRatioConcave(const Point&);
        float pointRatioConcave(int x, int y);
        float pointRatioConvex(const Point&, float);
        float pointRatioConvex(int x, int y, float);
        float pointRatioLinear(const Point&);
        float pointRatioLinear(int x, int y);
        float pointRatioCircleBased(const Point&, AXES);
        float pointRatioCircleBased(int x, int y, AXES);
    };

    std::ostream& operator<<(std::ostream&, Ellipse);

    class Volcano {
    private:
        VolcanoData vd;
        Ellipse base;
        Ellipse crater;
        unsigned SARAvHeight;
        Point coorTranVector;

        float angle2sat;
        cv::Vec3f v2sat;

        cv::Mat DEM;
        cv::Mat DEMNoise;
        cv::Mat Reflection;
        cv::Mat Normals;
        cv::Mat DEM2SAR;
        cv::Mat Reflection2SAR;

        void makeDEM();
        void makeReflection();
        void project();

        void speckle(cv::Mat&, float val=1);

        Point imCoor2EllCoor(Point);

    public:
        explicit Volcano(VolcanoData, unsigned _SARAvHeight=851, float _angle2sat=1.39626);

        cv::Mat getDEM();
        cv::Mat getDEMNoise();
        cv::Mat getReflection();
        cv::Mat getNormals();
        cv::Mat getDEM2SAR();
        cv::Mat getReflection2SAR();

        VolcanoData getVd ();
        Ellipse getEllipse(Ellipses);
    };

    std::ostream& operator<<(std::ostream&, Volcano);
}

#endif //HEIGHTMAP_VOLCANO_H