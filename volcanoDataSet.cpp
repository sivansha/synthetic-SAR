#include "volcanoDataSet.h"

//-------------------------------------------------------------------------
// DEM variables

// Image and Volcano dimensions:
// resolution: base on data, approximate sentinel-1 resolution to 4m per pixel.
// Volcano Height: base on data, approximate to [2000, 6800] (assume gaussian distribution)
// Crater Depth: base on observations the crater maximal depth will be between [0.77, 0.93] from the maximal height
//   (assume gaussian distribution)
// Crater fall: the crater might have a sheer fall around the rim: between [0.03, 0.24] from crater depth
//   (assume gaussian distribution)
// Volcano Base Diameter: base on data, approximate to h/x when x=[0.051, 0.25] (assume gaussian distribution)
// Volcano Crater Diameter: base on observations the crater/base ratio will be selected from [0.1, 0.4]
//   (assume gaussian distribution)
// Volcano Eccentricity: base on observations it seems as the eccentricity of volcanoes base is rather small,
//   hence the ratio between the long and the short axes of the base ellipses will be relatively small: [0.8, 0.99]
//   craters seems to have even smaller eccentricity, hence the the ratio for the crater will be: [0.9, 1]
// Center Points: base on observations, the center point of the crater is inside circle that its center is at the
//   center of the crater, and its radius is third of the crater radius (assume gaussian distribution)

VolcanoData generateVolcanoData (std::mt19937 generator)
{
// check for underflow
int underflowIndicator = 9000;

// volcano height
    static std::normal_distribution<float> heightDis(4400, 580); // [2000, 6800]
    float heightMeters = heightDis(generator);

    static std::normal_distribution<float> CraterMinHeightDis(0.85, 0.02); // [0.77, 0.93]
    float craterMinHeightRatio = CraterMinHeightDis(generator);
    float craterMinHeightMeters = heightMeters * craterMinHeightRatio;

    static std::normal_distribution<float> CraterFallDis(0.135, 0.03); // [0.03, 0.24]
    float craterFallRatio = CraterFallDis(generator);
    float craterFallMeters = (heightMeters - craterMinHeightMeters) * craterFallRatio;

// volcano base long axis
    static std::normal_distribution<float> h2dRatioDis(0.21, 0.037); // [0.051, 0.25]
    float volcanoH2DRatio = h2dRatioDis(generator);
    float BA1Meters =  (heightMeters/volcanoH2DRatio)/(2*M_PI);
    auto BA1Pixels = static_cast<unsigned>(BA1Meters/4);

// volcano base short axis
    static std::normal_distribution<float> BaseLongAx2ShortAxDis(0.85, 0.15); // [0.8, 0.99]
    float baseLA2SARatio = BaseLongAx2ShortAxDis(generator);
    float BA2Meters = BA1Meters * baseLA2SARatio;
    auto BA2Pixels = static_cast<unsigned>(BA2Meters/4);

// volcano crater long axis
    static std::normal_distribution<float> BA2CARatioDis(0.11, 0.08); // [0.1, 0.4]
    float volcanoBA2CARatio = BA2CARatioDis(generator);
    float CA1Meters = BA1Meters * volcanoBA2CARatio;
    auto CA1Pixels = static_cast<unsigned>(CA1Meters/4);

// volcano crater short axis
    static std::normal_distribution<float> CraterLongAx2ShortAxDis(0.85, 0.15); // [0.9, 1]
    float craterLA2SARatio = CraterLongAx2ShortAxDis(generator);
    float CA2Meters = CA1Meters * craterLA2SARatio;
    auto CA2Pixels = static_cast<unsigned>(CA2Meters/4);

// base center point
    Point baseCenterPoint(static_cast<unsigned>(BA1Pixels), static_cast<unsigned>(BA2Pixels));

// crater center point
    static std::normal_distribution<float> craterCenterPointDis(0, 0.01); // [-0.33, 0.33]
    float xShift =  craterCenterPointDis(generator);
    float yShift =  craterCenterPointDis(generator);

    auto craterX = static_cast<unsigned>((baseCenterPoint.x * xShift + baseCenterPoint.x));
    auto craterY = static_cast<unsigned>((baseCenterPoint.y * yShift + baseCenterPoint.y));
    Point craterCenterPoint(craterX, craterY);

    VolcanoData volcanoData = VolcanoData();

    volcanoData.height = heightMeters;
    volcanoData.craterMaxHeight = heightMeters - craterFallMeters;
    volcanoData.craterMinHeight = craterMinHeightMeters;
    volcanoData.craterMinHeightRatio = craterMinHeightRatio;
    volcanoData.craterFall = craterFallMeters;
    volcanoData.craterFallRatio = craterFallRatio;
    volcanoData.baseLongAxisPixels = BA1Pixels;
    volcanoData.baseShortAxisPixels = BA2Pixels;
    volcanoData.craterLongAxisPixels = CA1Pixels;
    volcanoData.craterShortAxisPixels = CA2Pixels;
    volcanoData.baseCenter = baseCenterPoint;
    volcanoData.craterCenter = craterCenterPoint;

    // in case of underflow
    if(craterMinHeightRatio > underflowIndicator ||
       craterFallMeters > underflowIndicator     ||
       BA1Pixels > underflowIndicator            ||
       BA2Pixels > underflowIndicator            ||
       CA1Pixels > underflowIndicator            ||
       CA2Pixels > underflowIndicator            ||
       craterX > underflowIndicator              ||
       craterY > underflowIndicator)
    { return  generateVolcanoData (generator); }

    return volcanoData;
}

std::ostream &operator<<(std::ostream &os, const VolcanoData vd) {
    return os << "Height meters: "           << vd.height                <<
              "\nCrater fall: "              << vd.craterFall            <<
              "\nCrater fall ratio: "        << vd.craterFallRatio       <<
              "\nCrater Max height: "        << vd.craterMaxHeight       <<
              "\nCrater Min height: "        << vd.craterMinHeight       <<
              "\nCrater Min height ratio: "  << vd.craterMinHeightRatio  <<
              "\nBase Long axis pixels: "    << vd.baseLongAxisPixels    <<
              "\nBase Short axis pixels: "   << vd.baseShortAxisPixels   <<
              "\nCrater Long axis pixels: "  << vd.craterLongAxisPixels  <<
              "\nCrater Short axis pixels: " << vd.craterShortAxisPixels <<
              "\nBase center: "              << vd.baseCenter            <<
              "\nCrater center: "            << vd.craterCenter          <<
              endl;
}
//----------------------------------------------------------------------------------------------------------------------