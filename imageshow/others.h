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
    equalizehist
};

// 定义阈值相关参数结构体
struct ThreShold {
    uint8_t value = 150;
    uint8_t max = 255;
    uint8_t mode;
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
};
extern im_state image_state;

extern cv::Mat Image_read(cv::String file_path);
extern QImage Mat2QImage(const cv::Mat& mat);
extern void Qimshow(QLabel *label, QImage Qimg);
extern void Qimclear(QLabel *label);
extern QVector<QString> get_directory_list(QString path);
extern cv::Mat Image_mode(cv::Mat mat, uint8_t mode);
extern cv::String make_savename(cv::String str);
extern std::string zfill(std::string str, size_t width);
extern void find_num(std::string str, std::string& str_head, int& number, int& number_width, std::string& str_end);
extern bool isReasonablefile(cv::String filename);

#endif // OTHERS_H

