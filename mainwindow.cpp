#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <dirent.h>
#include <opencv2/opencv.hpp>
#include <string>
#include <iostream>
#include <fstream>
#include <QThread>
#include <algorithm>
#include <iterator>
#include <QFileDialog>

using namespace cv;
using namespace std;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_pushButton_exit_clicked()
{
    this->close();
}

vector<string> MainWindow::listFile(char folder_name[])
{
    DIR *pDIR;
    struct dirent *entry;
    vector<string> files;
    if( pDIR=opendir(folder_name) ){
            while(entry = readdir(pDIR)){
                    if( strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0 )
                   // cout << entry->d_name << "\n";

                    files.push_back(entry->d_name);
            }
            closedir(pDIR);
    }
    return files;
}

void MainWindow::on_pushButton_colour_clicked()
{
    int kernel_size = 3;
    int scale = 1;
    int delta = 1;
    int ddepth = CV_16S;

    QString str = QFileDialog::getExistingDirectory();
    QByteArray ba = str.toLocal8Bit();
    char *c_str = ba.data();
    string slash = "/";

    vector<string> img_name;

    img_name = listFile(c_str);

    for(int i = 0; i < img_name.size(); i++)                         //size of the img_name
    {

     string file_name = c_str + slash + img_name[i];

     Mat img = imread(file_name);
     Mat img_filter;
     pyrMeanShiftFiltering(img, img_filter, 21, 3, 2);

     Mat img_ori;
     cv::GaussianBlur(img_filter, img_ori, cv::Size(0, 0), 3);
     cv::addWeighted(img_filter, 1.5, img_ori, -0.5, 0, img_ori);

     Mat img_rgb;
    // cvtColor(img_ori, img_rgb, CV_RGB2HSV);
     vector<Mat> rgb;
     split(img_ori, rgb);


     Mat img_lap, abs_dst;
     Laplacian(rgb[1], img_lap, ddepth, kernel_size, scale, delta, BORDER_DEFAULT );
     convertScaleAbs( img_lap, abs_dst );

     Mat thresh;
     //adaptiveThreshold(abs_dst, thresh, 255, ADAPTIVE_THRESH_GAUSSIAN_C, THRESH_BINARY, 47, -3);
     threshold(abs_dst, thresh, 55, 255, CV_THRESH_BINARY);

      Mat dst;
      Mat kernel1 = Mat::ones(3, 3, CV_8UC1);
     // dilate(thresh, dst, kernel1);
    //  erode(thresh, dst, kernel1);

      dst = thresh.clone();
    // medianBlur (dst, dst, 3);
    // convertScaleAbs(dst, dst);


     Mat thresh_copy = dst.clone();
     vector<vector<Point> > contours;
     vector<Vec4i> hierarchy;
     findContours(thresh_copy, contours, hierarchy, CV_RETR_CCOMP, CV_CHAIN_APPROX_NONE);


     for (vector<vector<Point> >::iterator it = contours.begin(); it!=contours.end(); )
     {
         if ( it->size() > 10)
             it=contours.erase(it);
         else
             ++it;
     }


     drawContours(dst, contours, -1, Scalar(0), CV_FILLED);



     for (int i = 0; i < thresh.rows; i++)
     {
         for (int j = 0; j < thresh.cols; j++)
         {
             if (dst.at<uchar>(i,j) == 255)
             {
                 img.at<Vec3b>(i,j)[0] = 0;
                 img.at<Vec3b>(i,j)[1] = 0;
                 img.at<Vec3b>(i,j)[2] = 255;
             }
         }
     }


     imshow("ori", thresh);
     imshow("meanshift", img_filter);
     imshow("display", img_ori);
     waitKey(0);
    }
}

void MainWindow::on_pushButton_kmeans_clicked()
{

    int kernel_size = 3;
    int scale = 1;
    int delta = 0;
    int ddepth = CV_16S;

    QString str = QFileDialog::getExistingDirectory();
    QByteArray ba = str.toLocal8Bit();
    char *c_str = ba.data();
    string slash = "/";

    vector<string> img_name;

    img_name = listFile(c_str);

    for(int i = 0; i < img_name.size(); i++)                         //size of the img_namefdsfafdsfdsaf dfdfdsfsafs
    {

     string file_name = c_str + slash + img_name[i];

     Mat img_ori = imread(file_name);

     Mat img_rgb;
     pyrMeanShiftFiltering(img_ori, img_rgb, 21, 3, 2);

     vector<Mat> rgb;
     split(img_rgb, rgb);

     Mat img_lap, abs_r, abs_g, abs_b;
     Laplacian(rgb[0], img_lap, ddepth, kernel_size, scale, delta, BORDER_DEFAULT );
     convertScaleAbs( img_lap, abs_r );

     Laplacian(rgb[1], img_lap, ddepth, kernel_size, scale, delta, BORDER_DEFAULT );
     convertScaleAbs( img_lap, abs_g );

     Laplacian(rgb[2], img_lap, ddepth, kernel_size, scale, delta, BORDER_DEFAULT );
     convertScaleAbs( img_lap, abs_b );

     Mat img_new;
     vector<Mat> rgb_new;
     rgb_new.push_back(abs_r);
     rgb_new.push_back(abs_g);
     rgb_new.push_back(abs_b);
     merge(rgb_new, img_new);


     int k = 4;
     int n = img_ori.rows *img_ori.cols;
     cv::Mat img3xN(n,3,CV_8U);
     for(int i=0;i!=3;++i)
     {
       rgb_new[i].reshape(1,n).copyTo(img3xN.col(i));
     }

     img3xN.convertTo(img3xN,CV_32F);
     cv::Mat bestLables;
     cv::kmeans(img3xN,k,bestLables,cv::TermCriteria(),10,cv::KMEANS_RANDOM_CENTERS );
     bestLables= bestLables.reshape(0, img_ori.rows);



     Mat dst;
     Mat kernel1 = Mat::ones(3, 3, CV_8UC1);
    // dilate(bestLables, dst, kernel1);
   //  erode(thresh, dst, kernel1);

   //  cv::convertScaleAbs(bestLables,bestLables,int(255/k));

     for (int i = 0; i < bestLables.rows; i++)
     {
         for (int j = 0; j < bestLables.cols; j++)
         {
             if (bestLables.at<int>(i,j) == 0 || bestLables.at<int>(i,j) == 2)
             {
                 img_ori.at<Vec3b>(i,j)[0] = 0;
                 img_ori.at<Vec3b>(i,j)[1] = 0;
                 img_ori.at<Vec3b>(i,j)[2] = 255;

                 cout<<bestLables.at<int>(i,j)<<" ";
             }
         }
     }

     imshow("result",img_ori);


     Mat cm_img;
     applyColorMap(bestLables, cm_img, COLORMAP_JET);
     imshow("display", img_rgb);
     imshow("colour", cm_img);
     cv::waitKey(0);

    }
}
