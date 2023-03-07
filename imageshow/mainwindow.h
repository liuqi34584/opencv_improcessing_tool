#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

#include "QLabel"
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/opencv.hpp>

#include <iostream>
#include <stdio.h>
#include <math.h>

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT
protected:
     void closeEvent(QCloseEvent *event);

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void matToQimageLabelShow(QLabel *label, cv::Mat mat);
    cv::Mat gray_value(cv::Mat mat); // 二值化的相关函数
    void save_image(); // 图片保存函数
    void dir_pre_show_image(); // 文件夹模式预显示图像
    QString filename;  // 加载单张图片名
    QString dirname;   // 加载文件目录名字
    QString save_dirname;   // 文件保存路径
    cv::Mat img,new_img;    // 全局CV图像img

    uint8_t mode[10] = {0};    //后面功能全按照这个数组走
    //mode[0] : 0为加载单张，1为加载目录,2为转换
    //mode[1] : 0不使用二值处理，1为使用二值处理
    //mode[2] : 0不使用灰度处理，1为使用灰度处理
    //mode[3] : 0不resize大小，1为resize大小

private slots:
    void on_pB_load_one_clicked();
    void on_pB_load_dir_clicked();
    void on_pB_save_dir_clicked();
    void on_pushButton_imtwovalue_clicked();
    void on_pushButton_gray_value_clicked();
    void on_pB_turn_image_clicked();

    void on_pb_resize_clicked();

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
