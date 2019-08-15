#include "volcano.h"

namespace syntheticVolcano
{
    Ellipse::Ellipse() = default;

    Ellipse::Ellipse(const Point& _center, int _axisA, int _axisB)
    {
        center.x = _center.x;
        center.y = _center.y;

        axes[0] = _axisA;
        axes[1] = _axisB;
    }

    float Ellipse::dist2center(const Point& p)
    {
        float x = p.x - center.x;
        float y = p.y - center.y;

        return(hypot(x, y));
    }

    float Ellipse::angle2center(const Point& p)
    {
        float x = p.x - center.x;
        float y = p.y - center.y;

        return atan2(y, x);
    }

    // assuming center is at the origin
    float Ellipse::radius4angle(float radius)
    {
        return (axes[0]*axes[1] / (sqrt(pow(axes[1]*cos(radius),2) + pow(axes[0]*sin(radius),2))));
    }

    Point Ellipse::getCenter()
    {
        return center;
    }

    void Ellipse::getAxes(unsigned * ip)
    {
        ip[0] = axes[0];
        ip[1] = axes[1];
    }

    unsigned Ellipse::getLongAxis()
    {
        return axes[0];
    }

    unsigned Ellipse::getShortAxis()
    {
        return axes[1];
    }

    void Ellipse::setCenter(const Point& _center)
    {
        setCenter(_center.x, _center.y);
    }

    void Ellipse::setCenter(int x, int y)
    {
        center.x = x;
        center.y = y;
    }

    void Ellipse::setAxis(const float* _axes)
    {
        setAxis(_axes[0], _axes[1]);
    }

    void Ellipse::setAxis(float a, float b)
    {
        axes[0] = a;
        axes[1] = b;
    }

    bool Ellipse::isPointInside(const Point& p)
    {
        if(!pow(axes[0],2 || !pow(axes[1],2))) return false;
        return (pow(p.x-center.x,2)/pow(axes[0],2) + pow((p.y-center.y),2)/pow(axes[1],2)) <= 1;
    }

    bool Ellipse::isPointInside(int x, int y)
    {
        return (isPointInside(Point(x,y)));
    }

    float Ellipse::pointRatioConcave(const Point& p)
    {
        if(!pow(axes[0],2 || !pow(axes[1],2))) return 0;
        return 1-(pow(p.x-center.x,2)/pow(axes[0],2) + pow((p.y-center.y),2)/pow(axes[1],2));
    }

    float Ellipse::pointRatioConcave(int x, int y)
    {
        return (pointRatioConcave(Point(x,y)));
    }

    float Ellipse::pointRatioConvex(const Point& p, float power)
    {
        float val = pointRatioConcave(p);
        float retVal = pow(val, power);
        // check if nan:
        if(isnan(retVal)) retVal = pointRatioLinear(p);
        else if(power==rintf(power) && (int)power%2 == 0 && val<0) retVal *= -1;

        return retVal;
    }

    float Ellipse::pointRatioConvex(int x, int y, float pow)
    {
        return (pointRatioConvex(Point(x,y), pow));
    }

    float Ellipse::pointRatioLinear(const Point& p)
    {
        // since the radius is calculated assuming the center is at the origin
        Point shifted;
        shifted.x = abs(p.x-center.x);
        shifted.y = abs(p.y-center.y);

        float pDist = hypot(shifted.x, shifted.y);
        float angle4point = atan2(shifted.y, shifted.x);
        float radius = radius4angle(angle4point);

        float val = radius == 0 ? 0 : 1-(pDist/radius);
        return val;
    }

    float Ellipse::pointRatioLinear(int x, int y)
    {
        return pointRatioLinear(Point(x,y));
    }

    float Ellipse::pointRatioCircleBased(const Point& p, AXES mode)
    {
        float pDist = dist2center(p);
        float radius = mode ? axes[1] : axes[0];

        float val = radius == 0 ? 0 : 1- pDist/radius;
        return val;
    }

    float Ellipse::pointRatioCircleBased(int x, int y, AXES mode)
    {
        return pointRatioCircleBased(Point(x, y), mode);
    }

    std::ostream& operator<<(std::ostream& os, Ellipse ell)
    {
        return os << "Ellipse center x: "     << ell.getCenter().x  <<
                     "\nEllipse center y: "   << ell.getCenter().y  <<
                     "\nEllipse long axis: "  << ell.getLongAxis()  <<
                     "\nEllipse short axis: " << ell.getShortAxis() <<
                     endl;
    }
    //-------------------------------------------------------------------------

    Volcano::Volcano(VolcanoData _vd, unsigned _SARAvHeight, float _angle2sat) :
                     SARAvHeight(_SARAvHeight), angle2sat(_angle2sat)
    {
        vd = _vd;

        base = Ellipse(vd.baseCenter, vd.baseLongAxisPixels, vd.baseShortAxisPixels);
        crater = Ellipse(vd.craterCenter, vd.craterLongAxisPixels, vd.craterShortAxisPixels);

        coorTranVector.x = base.getCenter().x - SARAvHeight/2;
        coorTranVector.y = base.getCenter().y - SARAvHeight/2;

        v2sat = Vec3f(-sin(angle2sat), 0, cos(angle2sat));

        makeDEM();
        makeReflection();
        project();
    }

    void Volcano::project()
    {
        cout << "Volcano Object: projecting" << endl;

        Mat Range = Mat(DEM.rows, DEM.cols, CV_32FC1, 0.0);

        for (int y = 0; y < DEM.rows; y++)
        {
            for (int x = 0; x < DEM.cols; x++)
            {
                Vec3f demP =  Vec3f(x, y, DEM.at<float>(y, x));
                Range.at<float>(y, x) = demP.dot(v2sat);
            }
        }

        double min, max;
        minMaxLoc(Range, &min, &max);
        if(min < 0)
        {
            Range += abs(min);
            minMaxLoc(Range, &min, &max);
        }

        DEM2SAR = Mat(DEM.rows, (int)max, CV_32FC1, cv::Scalar(-1));
        Reflection2SAR = Mat(DEM.rows, (int)max, CV_32FC1, cv::Scalar(-1));

        int xVal;
        for (int y = 0; y < DEM.rows; y++)
        {
            for (int x = 0; x < DEM.cols; x++)
            {
                xVal = Range.at<float>(y, x);
                DEM2SAR.at<float>(y, xVal) = DEM.at<float>(y, x);
                Reflection2SAR.at<float>(y, xVal) = Reflection.at<float>(y, x);
            }
        }

        extrapolate_mat(DEM2SAR);
        extrapolate_mat(Reflection2SAR);

        speckle(Reflection2SAR);
    }

    void Volcano::makeReflection()
    {
        cout << "Volcano Object: reflecting DEM" << endl;

        Reflection = Mat(DEM.rows, DEM.cols, CV_32FC1, 0.0);
        Normals = Mat(DEM.rows, DEM.cols, CV_32FC3, 0.0);

        unsigned int seed = std::rand();
        PerlinNoise pn(seed);

        for (int y = 0; y < DEM.rows; y++)
        {
            for (int x = 0; x < DEM.cols; x++)
            {
                Point p(x, y);

                float dzdx = (DEM.at<float>(y, x + 1) - DEM.at<float>(y, x - 1)) / 2.0;
                float dzdy = (DEM.at<float>(y + 1, x) - DEM.at<float>(y - 1, x)) / 2.0;

//                Vec3f norm(-dzdx, -dzdy, -1.0f);
//                normalize(norm, norm, cv::NORM_L1, CV_32FC3);
                float hyp = sqrt(pow(dzdx, 2) + pow(dzdy, 2) + 1);
                dzdx /= hyp;
                dzdy /= hyp;
                Vec3f norm(-dzdx, -dzdy, -1.0f/hyp);

                Normals.at<cv::Vec3f>(y, x) = norm;

                // reflection = cos(a) times albedo
                float dot_product = v2sat.dot(norm);
                if (dot_product < 0) dot_product = std::numeric_limits<float>::min();

                // albedo
                float albedo = abs(perlinNoise(p, DEM.rows, DEM.cols, pn));
                Reflection.at<float>(y, x) = dot_product * albedo;
            }
        }

        double min, max;
        minMaxLoc(Reflection, &min, &max);
        if(min < 0) Reflection += abs(min);
    }

    void Volcano::makeDEM() {
        cout << "Volcano Object: making DEM" << endl;

        // create image
        DEM = Mat(SARAvHeight, SARAvHeight, CV_32FC1, 0.0);

        unsigned int seed = std::rand();
        PerlinNoise pn(seed);
        int surfaceDetails = 10;

        float maxBaseS = 0;
        std::vector<float> maxBaseV;
        for (int y = 0; y < DEM.rows; y++)
        {
            for (int x = 0; x < DEM.cols; x++)
            {
                Point p(x, y);
                float noise = perlinNoise(p, DEM.rows, DEM.cols, pn);

                if(base.isPointInside(imCoor2EllCoor(p)) && !crater.isPointInside(imCoor2EllCoor(p)))
                {
//                    float ratio = base.pointRatioLinear(imCoor2EllCoor(p));
//                    float ratio = base.pointRatioConcave(imCoor2EllCoor(p));
//                    float ratio = base.pointRatioConvex(imCoor2EllCoor(p), 2.5);
                    float ratio = base.pointRatioCircleBased(imCoor2EllCoor(p), LONG_AXIS);
//                    float ratio = base.pointRatioCircleBased(imCoor2EllCoor(p), SHORT_AXIS);

                    float craterPointH = vd.height * ratio + noise * surfaceDetails;
                    DEM.at<float>(y,x) = craterPointH;

                    if (craterPointH > maxBaseS) maxBaseS = craterPointH;
                    if(crater.isPointInside(imCoor2EllCoor(Point(p.x+1, p.y))) ||
                       crater.isPointInside(imCoor2EllCoor(Point(p.x-1, p.y))) ||
                       crater.isPointInside(imCoor2EllCoor(Point(p.x, p.y+1))) ||
                       crater.isPointInside(imCoor2EllCoor(Point(p.x, p.y-1))) ||
                       crater.isPointInside(imCoor2EllCoor(Point(p.x+1, p.y+1))) ||
                       crater.isPointInside(imCoor2EllCoor(Point(p.x+1, p.y-1))) ||
                       crater.isPointInside(imCoor2EllCoor(Point(p.x-1, p.y+1))) ||
                       crater.isPointInside(imCoor2EllCoor(Point(p.x-1, p.y-1))))
                    {
                        maxBaseV.push_back(craterPointH);
                    }
                }
            }
        }

        float minOfVector = MAXFLOAT;
        if(!maxBaseV.empty()) minOfVector = *std::min_element(std::begin(maxBaseV), std::end(maxBaseV));
        float maxH = minOfVector < maxBaseS ? minOfVector : maxBaseS;
        float craterMinH = maxH * vd.craterMinHeightRatio;
        float craterFall = (maxH - craterMinH) * vd.craterFallRatio;

        for (int y = 0; y < DEM.rows; y++)
        {
            for (int x = 0; x < DEM.cols; x++)
            {
                Point p(x, y);
                float noise = perlinNoise(p, DEM.rows, DEM.cols, pn);

                if(crater.isPointInside(imCoor2EllCoor(p)))
                {
//                    float ratioC = crater.pointRatioLinear(imCoor2EllCoor(p));
//                    float ratioC = crater.pointRatioConcave(imCoor2EllCoor(p));
                    float ratioC = crater.pointRatioConvex(imCoor2EllCoor(p), 2.5);
//                    float ratioC = crater.pointRatioCircleBased(imCoor2EllCoor(p), LONG_AXIS);
//                    float ratioC = crater.pointRatioCircleBased(imCoor2EllCoor(p), SHORT_AXIS);

                    float craterPointH = (1-ratioC) * (maxH - craterFall);
                    craterPointH = craterPointH > craterMinH ? craterPointH : craterMinH;
                    DEM.at<float>(y,x) = craterPointH;
                }
                else if(!(base.isPointInside(imCoor2EllCoor(p))))
                {
//                    float ratio = base.pointRatioLinear(imCoor2EllCoor(p));
                    float ratio = base.pointRatioConcave(imCoor2EllCoor(p));
//                    float ratio = base.pointRatioConvex(imCoor2EllCoor(p), 2.5);
//                    float ratio = base.pointRatioCircleBased(imCoor2EllCoor(p), LONG_AXIS);
//                    float ratio = base.pointRatioCircleBased(imCoor2EllCoor(p), SHORT_AXIS);

                    DEM.at<float>(y,x) = maxH * ratio + noise * surfaceDetails;
                }
            }
        }

        double min, max;
        minMaxLoc(DEM, &min, &max);
        if(min < 0) DEM += abs(min);
    }

    void Volcano::speckle(cv::Mat& mat, float val)
    {
        std::default_random_engine generator;
        std::gamma_distribution<double> distribution(2.0,2.0);

        for(int i = 0; i < mat.rows; i++)
        {
            for(int j = 0; j < mat.cols; j++)
            {
                mat.at<float>(i, j) += distribution(generator);
            }
        }
    }

    Point Volcano::imCoor2EllCoor(Point p)
    {
        return Point (p.x + coorTranVector.x, p.y + coorTranVector.y);
    }

    cv::Mat Volcano::getDEM() { return DEM; }
    cv::Mat Volcano::getDEMNoise() { return DEMNoise; }
    cv::Mat Volcano::getReflection() { return Reflection; }
    cv::Mat Volcano::getNormals() { return Normals; }
    cv::Mat Volcano::getDEM2SAR() { return DEM2SAR; }
    cv::Mat Volcano::getReflection2SAR() { return Reflection2SAR; }
    VolcanoData Volcano::getVd() { return vd; }
    Ellipse Volcano::getEllipse(Ellipses e) { return e ? crater : base; }

    std::ostream& operator<<(std::ostream& os, Volcano vol)
    {
        VolcanoData vd = vol.getVd();
        Ellipse be     = vol.getEllipse(BASE);
        Ellipse ce     = vol.getEllipse(CRATER);

        return os << "Volcano Data: "     << vd <<
                     "\nBase Ellipse: "   << be <<
                     "\nCraetr Ellipse: " << ce <<
                     endl;
    }
    //-------------------------------------------------------------------------
}