#include "utils.h"

void printMatAligned(Mat m)
{
    for(int i = 0; i < m.rows; i++)
    {
        for(int j = 0; j < m.cols; j++)
        {
            cout << setprecision(8) << fixed << setw(12) << m.at<float>(i, j) << " ";
        }
        cout << endl;
    }
    cout << endl;
}

VolcanoData getTestData()
{
    VolcanoData test = VolcanoData();
    test.height = 20;
    test.craterMaxHeight = 16;
    test.craterMinHeight = 10;
    test.craterFall = 4;
    test.baseLongAxisPixels = 40;
    test.baseShortAxisPixels = 40;
    test.craterLongAxisPixels = 5;
    test.craterShortAxisPixels = 5;
    test.baseCenter = Point(400, 400);
    test.craterCenter = Point(400, 400);

    return test;
}

float perlinNoise (Point p, unsigned height, unsigned width, PerlinNoise pn)
{
    float denominatorCols = width == 0.0 ? 1.0 : (float)width;
    float denominatorRows = height == 0.0 ? 1.0 : (float)height;
    float n1 = 5*(float)p.x/(denominatorCols);
    float n2 = 5*(float)p.y/(denominatorRows);
    float noise = pn.noise(n1, n2, 0.5)
                  + 0.5 *pn.noise(2*n1, 2*n2, 0.5)
                  + 0.25*pn.noise(4*n1, 4*n2, 0.5);

    return noise;
}

void extrapolate_mat(cv::Mat &mat, int kernel_size)
{
    float factor = 0, val=0;

    cv::Mat kernel(5, 5, CV_32FC1, cv::Scalar(-1));
    kernel.at<float>(0,0)=1; kernel.at<float>(0,1)=4;  kernel.at<float>(0,2)=7;  kernel.at<float>(0,3)=4;  kernel.at<float>(0,4)=1;
    kernel.at<float>(1,0)=4; kernel.at<float>(1,1)=16; kernel.at<float>(1,2)=26; kernel.at<float>(1,3)=16; kernel.at<float>(1,4)=4;
    kernel.at<float>(2,0)=7; kernel.at<float>(2,1)=26; kernel.at<float>(2,2)=41; kernel.at<float>(2,3)=26; kernel.at<float>(2,4)=7;
    kernel.at<float>(3,0)=4; kernel.at<float>(3,1)=16; kernel.at<float>(3,2)=26; kernel.at<float>(3,3)=16; kernel.at<float>(3,4)=4;
    kernel.at<float>(4,0)=1; kernel.at<float>(4,1)=4;  kernel.at<float>(4,2)=7;  kernel.at<float>(4,3)=4;  kernel.at<float>(4,4)=1;

    for (int row=0; row<mat.rows; row++)
    {
        for (int col=0; col<mat.cols; col++)
        {
            if(mat.at<float>(row,col) != -1) continue;

            for (int ker_row=0; ker_row<kernel_size; ker_row++)
            {
                for (int ker_col = 0; ker_col<kernel_size; ker_col++)
                {
                    int kernel2mat_y = row-kernel_size/2+ker_row;
                    int kernel2mat_x = col-kernel_size/2+ker_col;

                    if(kernel2mat_y < 0 || kernel2mat_y > mat.rows || kernel2mat_x < 0 || kernel2mat_x > mat.cols) continue;

                    if(isnan(mat.at<float>(kernel2mat_y, kernel2mat_x))) continue;
                    val += mat.at<float>(kernel2mat_y, kernel2mat_x) * kernel.at<float>(ker_row, ker_col);
                    factor += kernel.at<float>(ker_row, ker_col);
                }
            }
            mat.at<float>(row, col) = factor == 0 ? 0 : val*(1/factor);
            val=0;
            factor = 0;
        }
    }
}

cv::Mat gradients(cv::Mat &mat)
{
    cv::Mat normal_grad = Mat(mat.rows, mat.cols, CV_32FC3, 0.0);
//    for (int y = 0; y < mat.rows; y++)
//    {
//        for (int x = 0; x < mat.cols; x++)
//        {
//            Point p(x, y);
//
//            float dzdx = (mat.at<float>(y, x + 1) - mat.at<float>(y, x - 1)) / 2.0;
//            float dzdy = (mat.at<float>(y + 1, x) - mat.at<float>(y - 1, x)) / 2.0;
//
//            float hyp = sqrt(pow(dzdx, 2) + pow(dzdy, 2) + 0);
//            dzdx /= hyp;
//            dzdy /= hyp;
//            Vec3f norm(-dzdx, -dzdy, 0);
//
//            normal_grad.at<cv::Vec3f>(y, x) = norm;
//        }
//    }

    for (int y = 0; y < mat.rows; y++)
    {
        for (int x = 0; x < mat.cols; x++)
        {
            Point p(x, y);

            float dzdx = (mat.at<float>(y, x + 1) - mat.at<float>(y, x - 1)) / 2.0;
            float dzdy = (mat.at<float>(y + 1, x) - mat.at<float>(y - 1, x)) / 2.0;

            float hyp = sqrt(pow(dzdx, 2) + pow(dzdy, 2) + 1);
            dzdx /= hyp;
            dzdy /= hyp;
            Vec3f norm(-dzdx, -dzdy, 0);

            normal_grad.at<cv::Vec3f>(y, x) = norm;
        }
    }

    return normal_grad;
}