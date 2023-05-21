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

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

    QString filename;  // 加载单张图片名
    QVector<QString> image_list;   // 文件夹路径列表

    void get_global_state();
    void Image_info_show();
    void setprint(QString str);

private slots:
    void on_pB_dirs_next_clicked();


    void on_pB_opencv_show_clicked();
    void on_pB_dir_load_clicked();
    void on_pB_dirs_load_clicked();

    void on_pB_dirs_pre_clicked();

    void on_pushButton_twovalue_clicked();

    void on_pushButton_twovalue_gray_clicked();

    void on_pB_dir_save_clicked();

    void on_pB_dirs_save_all_clicked();

    void on_pB_resize_clicked();

    void on_pB_turn_channels_there_clicked();

    void on_pB_erode_clicked();

    void on_pB_turn_image_clicked();

    void on_pB_dilate_clicked();

    void on_pB_turn_channels_b_clicked();

    void on_pB_turn_channels_g_clicked();

    void on_pB_turn_channels_r_clicked();

    void on_pushButton_twovalue_otsu_clicked();

    void on_pB_equalizehist_clicked();

    void on_pushButton_twovalue_triangle_clicked();

    void on_pB_flip_horizontal_clicked();

    void on_pB_flip_vertical_clicked();

    void on_pB_gray2color_clicked();

    void on_pB_padding_clicked();

    void on_pB_log_clicked();

    void on_pB_gamma_clicked();

    void on_pB_blur_clicked();

    void on_pB_boxfilter_clicked();

    void on_pB_gaussianblur_clicked();

    void on_pB_medianblur_clicked();

    void on_pB_bilateralfilter_clicked();

    void on_pB_adaptivethreshold_clicked();

    void on_pB_canny_clicked();

private:
    Ui::MainWindow *ui;
};

#endif // MAINWINDOW_H
