#ifndef OTHERS_H
#define OTHERS_H

#include "QLabel"
#include "QPixmap"
#include "QFileDialog"
#include <QCloseEvent>

#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>

#include <string>
#include <QString>
#include <iostream>
#include <QDebug>

#include <QDir>
#include <QFileInfoList>

// 定义读图模式相关参数结构体
enum mode_set {
    show =0,
    twovalue,
    gray,
    resize,
    turn_channels_there,
    erode,
    dilate,
    one_channel_b,
    one_channel_g,
    one_channel_r,
    equalizehist,
    otsu,
    triangle,
    flip_horizontal,
    flip_vertical,
    padding,
    gray2color,
    log_turn,
    gamma_turn,
    blur_turn,
    boxfilter,
    gaussianblur,
    medianblur,
    bilateralfilter,
    adaptivethreshold,
    canny_turn
};

// 定义阈值相关参数结构体
struct ThreShold {
    uint8_t value = 150;
    uint8_t max = 255;
    uint8_t mode;
    // computed threshold value if Otsu's or Triangle methods used
    double computed = 0;
};

// 定义resize相关参数结构体
struct Resize {
    int hight = 512;
    int width = 512;
    uint8_t mode;
};

// 定义erode相关参数结构体
struct Erode {
    int kernel_hight = 3;
    int kernel_width = 3;
    uint8_t StructuringElement;
};

// 定义dilate相关参数结构体
struct Dilate {
    int kernel_hight = 3;
    int kernel_width = 3;
    int StructuringElement;
};

// 定义padding相关参数结构体
struct Padding {
    int top = 0;
    int bottom = 0;
    int left = 0;
    int right = 0;
    int bordermode = 0;
};

// 定义对数变换相关参数结构体
struct Log {
    int C = 0;  // 定义对数变换的值
};

// 定义伽马变换相关参数结构体
struct Gamma {
    double value = 0;  // 定义对数变换的值
};

// 定义 滤波 相关参数结构体
struct Blur {
    int kernel_hight = 3;
    int kernel_width = 3;
};

// 定义 盒子滤波 相关参数结构体
struct Boxfilter {
    int kernel_hight = 3;
    int kernel_width = 3;
};

// 定义 高斯滤波 相关参数结构体
struct Gaussianblur {
    int kernel_hight = 3;
    int kernel_width = 3;
    float sigma = 1.0;
};

// 定义 中值滤波 相关参数结构体
struct Medianblur {
    int kernel_hight = 3;
    int kernel_width = 3;
};

// 定义 双边滤波 相关参数结构体
struct Bilateralfilter {
    int diameter = 9;       // 双边滤波器直径
    double sigmaColor = 75; // 颜色空间的标准差
    double sigmaSpace = 75; // 坐标空间的标准差
};

// 定义 canny边缘检测 相关参数结构体
struct Canny {
    double threshold1 = 100;  // 第一个阈值
    double threshold2 = 200;  // 第二个阈值
    int apertureSize = 3;     // Sobel算子的孔径大小
};

struct im_state {
    cv::Mat img;
    cv::Mat new_img;    // 全局CV图像mat
    bool isDirectorys = false;  // 默认单文件模式
    cv::String save_dir_name;  // 单文件保存名
    cv::String save_dirs_name;  // 多文件保存名
    cv::String save_name;  // 最终保存文件名

    uint8_t process_mode = mode_set::show;

    ThreShold threshold;
    Resize resize;
    Erode erode;
    Dilate dilate;
    Padding padding;
    Log log;
    Gamma gamma;
    Blur blur;
    Boxfilter boxfilter;
    Gaussianblur gaussianblur;
    Medianblur medianblur;
    Bilateralfilter bilateralfilter;
    Canny canny;
};
extern im_state image_state;

extern QImage Mat2QImage(const cv::Mat& mat);
extern void Qimshow(QLabel *label, QImage Qimg);
extern void Qimclear(QLabel *label);
extern QVector<QString> get_directory_list(QString path);
extern cv::Mat Image_mode(cv::Mat mat, uint8_t mode);
extern cv::String make_savename(cv::String str);
extern std::string zfill(std::string str, size_t width);
extern void find_num(std::string str, std::string& str_head, int& number, int& number_width, std::string& str_end);
extern bool isReasonablefile(cv::String filename);
extern int makeOdd(int num);

#endif // OTHERS_H

