#include "mainwindow.h"
#include "ui_mainwindow.h"
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
#include <others.h>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

im_state image_state; //定义全局状态结构体

// 刷新软件全局的值
void MainWindow::get_global_state()
{
//    Qimclear(ui->image);
//    Qimclear(ui->newimage);
    // 清空字符打印窗体
    ui->label_print_text->setText(" ");

    // 刷新阈值相关的值
    image_state.threshold.value = ui->cBox_twovalue_threshold_value->currentText().mid(3).toInt();
    image_state.threshold.max = ui->cBox_twovalue_threshold_max->currentText().mid(4).toInt();
    image_state.threshold.mode = ui->cBox_twovalue_threshold_mode->currentIndex();

    // 刷新文件保存相关的值
    image_state.save_dir_name = ui->lineEdit_dir_filename->text().toStdString();
    image_state.save_dirs_name = ui->lineEdit_dirs_filename->text().toStdString();

    // 刷新 resize 相关的值
    image_state.resize.hight = std::stoi(ui->lineEdit_resize_hight->text().toStdString());
    image_state.resize.width = std::stoi(ui->lineEdit_resize_width->text().toStdString());
    image_state.resize.mode = ui->cBox_resize_mode->currentIndex();

    // 刷新 erode 相关的值
    image_state.erode.kernel_hight = std::stoi(ui->lineEdit_erode_kernel_hight->text().toStdString());
    image_state.erode.kernel_width = std::stoi(ui->lineEdit_erode_kernel_width->text().toStdString());
    image_state.erode.StructuringElement = ui->cBox_erode_Structuring->currentIndex();

    // 刷新 dilate 相关的值
    image_state.dilate.kernel_hight = std::stoi(ui->lineEdit_dilate_kernel_hight->text().toStdString());
    image_state.dilate.kernel_width = std::stoi(ui->lineEdit_dilate_kernel_width->text().toStdString());
    image_state.dilate.StructuringElement = ui->cBox_dilate_Structuring->currentIndex();
}

// 利用界面 label 显示打印信息
void MainWindow::setprint(QString str)
{
    ui->label_print_text->setText(str);
}

// 显示图片信息
void MainWindow::Image_info_show()
{
    QString text_str;
    if(!image_state.img.empty()){
        text_str.sprintf("原图：  高：%d  宽：%d  %d通道", \
                              image_state.img.rows,image_state.img.cols,image_state.img.channels());
        ui->label_image_text->setText(text_str);
    }
    else{
        text_str.sprintf("当前数据为空");
        ui->label_image_text->setText(text_str);
    }

    if(!image_state.new_img.empty())
    {
        text_str.sprintf("处理后图：  高：%d  宽：%d  %d通道", \
                              image_state.new_img.rows,image_state.new_img.cols,image_state.new_img.channels());
        ui->label_newimage_text->setText(text_str);
    }
    else{
        text_str.sprintf("当前数据为空");
        ui->label_newimage_text->setText(text_str);
    }
}

// 打开单个图片文件
void MainWindow::on_pB_dir_load_clicked()
{
    get_global_state();

    MainWindow::filename = QFileDialog::getOpenFileName(this, QStringLiteral("选择图片文件"), "D:",\
                                            QStringLiteral("类型(*png *jpg *tif *gif)"));

    if (!filename.isEmpty()) {
        if(isReasonablefile(filename.toStdString()) == true){
            image_state.isDirectorys = false;
            image_state.process_mode = mode_set::show;

            image_state.img = Image_read(filename.toStdString());
            image_state.new_img = Image_mode(image_state.img, image_state.process_mode);

            QImage Qimg = Mat2QImage(image_state.img);
            Qimshow(ui->image, Qimg);

            QImage Qnewimg = Mat2QImage(image_state.new_img);
            Qimshow(ui->newimage, Qnewimg);

            Image_info_show();  // 显示图片尺寸，通道信息
        } else setprint("不能识别该路径或文件格式  " + filename);
    } else setprint("未选择图片文件");
}

// 打开文件夹
void MainWindow::on_pB_dirs_load_clicked()
{
    get_global_state();

    QString directory = QFileDialog::getExistingDirectory(this, tr("选择文件夹"));

    if (!directory.isEmpty()) {
        MainWindow::image_list = get_directory_list(directory);
        if(!image_list.isEmpty()) {
            MainWindow::filename = image_list[0];
            if(isReasonablefile(filename.toStdString()) == true) {
                image_state.isDirectorys = true;
                image_state.process_mode = mode_set::show;

                image_state.img = Image_read(filename.toStdString());
                image_state.new_img = Image_mode(image_state.img, image_state.process_mode);

                QImage Qimg = Mat2QImage(image_state.img);
                Qimshow(ui->image, Qimg);

                QImage Qnewimg = Mat2QImage(image_state.new_img);
                Qimshow(ui->newimage, Qnewimg);

                Image_info_show();  // 显示图片尺寸，通道信息
            } else setprint("不能识别该路径或文件格式  " + filename);
        } else setprint("未找到图片");
    } else setprint("未选择文件夹");
}

// 文件夹的下一张
void MainWindow::on_pB_dirs_next_clicked()
{
    get_global_state();
    if(!image_list.isEmpty()) {
        int index = image_list.indexOf(filename);
        if ( 0 <= index && index < image_list.size()-1) index++;
        filename = image_list[index];

        if(isReasonablefile(filename.toStdString()) == true) {

            image_state.img = Image_read(filename.toStdString());
            image_state.new_img = Image_mode(image_state.img, image_state.process_mode);

            QImage Qimg = Mat2QImage(image_state.img);
            Qimshow(ui->image, Qimg);

            QImage Qnewimg = Mat2QImage(image_state.new_img);
            Qimshow(ui->newimage, Qnewimg);

            Image_info_show();  // 显示图片尺寸，通道信息

        } else setprint("不能识别该路径或文件格式  " + filename);
    } else setprint("文件夹列表为空");
}

// 文件夹上一张
void MainWindow::on_pB_dirs_pre_clicked()
{
    get_global_state();
    if(!image_list.isEmpty()) {
        int index = image_list.indexOf(filename);
        if ( 0 < index && index < image_list.size()) index--;
        filename = image_list[index];

        if(isReasonablefile(filename.toStdString()) == true) {

            image_state.img = Image_read(filename.toStdString());
            image_state.new_img = Image_mode(image_state.img, image_state.process_mode);

            QImage Qimg = Mat2QImage(image_state.img);
            Qimshow(ui->image, Qimg);

            QImage Qnewimg = Mat2QImage(image_state.new_img);
            Qimshow(ui->newimage, Qnewimg);

            Image_info_show();  // 显示图片尺寸，通道信息

        } else setprint("不能识别该路径或文件格式  " + filename);
    } else setprint("文件夹列表为空");
}

// 保存单张图片
void MainWindow::on_pB_dir_save_clicked()
{
    get_global_state();  // 获取全局设置状态

    if(!image_state.new_img.empty())  // 不能保存空MAT
    {
        QString directory = QFileDialog::getExistingDirectory(this, tr("选择文件夹"));
        if (!directory.isEmpty()) {  // 判断保存路径
            cv::String save_path = directory.toStdString() + "/" + image_state.save_dir_name;

            if(isReasonablefile(image_state.save_dir_name) == true) {
                imwrite(save_path, image_state.new_img);
                setprint("图片保存成功  "  + QString::fromStdString(save_path));
            } else setprint("不支持该路径或者文件格式的保存" + QString::fromStdString(save_path));
        } else setprint("未选择保存路径");
    } else setprint("不能保存空MAT");
}

// 保存整个文件夹的图片
void MainWindow::on_pB_dirs_save_all_clicked()
{
    get_global_state();  // 获取全局设置状态

    cv::String head,end;  // 文件名的头部尾部
    int number,number_width;
    cv::String cv_filennme; //最终保存的文件名
    // 判断设置的文件名是否合理
    if(isReasonablefile(image_state.save_dirs_name) == true) {
        find_num(image_state.save_dirs_name, head, number, number_width, end);
        // 判断是否有源图片列表
        if(!image_list.isEmpty()) {
            QString directory = QFileDialog::getExistingDirectory(this, tr("选择保存文件夹"));
            // 判断是否选择保存文件夹
            if (!directory.isEmpty()) {
                for(int i = number; i < number+image_list.length();i++){

                    MainWindow::filename = image_list[i-number];

                    if(isReasonablefile(filename.toStdString()) == true) {
                        image_state.img = Image_read(filename.toStdString());
                        image_state.new_img = Image_mode(image_state.img, image_state.process_mode);

                        QImage Qimg = Mat2QImage(image_state.img);
                        Qimshow(ui->image, Qimg);

                        QImage Qnewimg = Mat2QImage(image_state.new_img);
                        Qimshow(ui->newimage, Qnewimg);

                        Image_info_show();  // 显示图片尺寸，通道信息

                        if(int(std::to_string(number).length()) != number_width) {
                            // 需要补全 “0056” -> 56
                            cv_filennme = directory.toStdString() + '/' + head + zfill(std::to_string(i), number_width) + end;
                        }
                        else cv_filennme = directory.toStdString() + '/' + head + std::to_string(i) + end;

                        if(!image_state.new_img.empty())  {
                            cv::imwrite(cv_filennme, image_state.new_img);
                            setprint("图片保存成功  "  + QString::fromStdString(cv_filennme));
                        } else setprint("不能保存空MAT");
                    } else setprint("不能识别该路径或文件格式" + directory + "/" + filename);
                }
            } else setprint("未选择保存文件夹");
        } else setprint("请打开要处理的文件夹");
    } else setprint("不支持该路径或者文件格式的保存");
}

// 对图片进行二值处理
void MainWindow::on_pushButton_twovalue_clicked()
{
    get_global_state();  // 获取全局设置状态
    if (!filename.isEmpty()) {
        image_state.process_mode = mode_set::twovalue;

        image_state.new_img = Image_mode(image_state.img, image_state.process_mode);

        QImage Qimg = Mat2QImage(image_state.img);
        Qimshow(ui->image, Qimg);

        QImage Qnewimg = Mat2QImage(image_state.new_img);
        Qimshow(ui->newimage, Qnewimg);

        Image_info_show();  // 显示图片尺寸，通道信息
    } else setprint("未读取到图片文件");
}

// 对图片进行灰度处理
void MainWindow::on_pushButton_twovalue_gray_clicked()
{
    get_global_state();  // 获取全局设置状态
    if (!filename.isEmpty()) {

        image_state.process_mode = mode_set::gray;

        image_state.new_img = Image_mode(image_state.img, image_state.process_mode);

        QImage Qimg = Mat2QImage(image_state.img);
        Qimshow(ui->image, Qimg);

        QImage Qnewimg = Mat2QImage(image_state.new_img);
        Qimshow(ui->newimage, Qnewimg);

        Image_info_show();  // 显示图片尺寸，通道信息
    } else setprint("未读取到图片文件");
}

// 对图片进行重设大小处理
void MainWindow::on_pB_resize_clicked()
{
    get_global_state();  // 获取全局设置状态

    if (!filename.isEmpty()) {

        image_state.process_mode = mode_set::resize;

        image_state.new_img = Image_mode(image_state.img, image_state.process_mode);

        QImage Qimg = Mat2QImage(image_state.img);
        Qimshow(ui->image, Qimg);

        QImage Qnewimg = Mat2QImage(image_state.new_img);
        Qimshow(ui->newimage, Qnewimg);

        Image_info_show();  // 显示图片尺寸，通道信息
    } else setprint("未读取到图片文件");
}

// 将图片转化为三通道图片
void MainWindow::on_pB_turn_channels_there_clicked()
{
    get_global_state();  // 获取全局设置状态

    if (!filename.isEmpty()) {

        image_state.process_mode = mode_set::turn_channels_there;

        image_state.new_img = Image_mode(image_state.img, image_state.process_mode);

        QImage Qimg = Mat2QImage(image_state.img);
        Qimshow(ui->image, Qimg);

        QImage Qnewimg = Mat2QImage(image_state.new_img);
        Qimshow(ui->newimage, Qnewimg);

        Image_info_show();  // 显示图片尺寸，通道信息
    } else setprint("未读取到图片文件");
}

// 对图片进行腐蚀处理
void MainWindow::on_pB_erode_clicked()
{
    get_global_state();  // 获取全局设置状态

    if (!filename.isEmpty()) {

        image_state.process_mode = mode_set::erode;

        image_state.new_img = Image_mode(image_state.img, image_state.process_mode);

        QImage Qimg = Mat2QImage(image_state.img);
        Qimshow(ui->image, Qimg);

        QImage Qnewimg = Mat2QImage(image_state.new_img);
        Qimshow(ui->newimage, Qnewimg);

        Image_info_show();  // 显示图片尺寸，通道信息
    } else setprint("未读取到图片文件");
}

// 对图片进行膨胀处理
void MainWindow::on_pB_dilate_clicked()
{
    get_global_state();  // 获取全局设置状态

    if (!filename.isEmpty()) {

        image_state.process_mode = mode_set::dilate;

        image_state.new_img = Image_mode(image_state.img, image_state.process_mode);

        QImage Qimg = Mat2QImage(image_state.img);
        Qimshow(ui->image, Qimg);

        QImage Qnewimg = Mat2QImage(image_state.new_img);
        Qimshow(ui->newimage, Qnewimg);

        Image_info_show();  // 显示图片尺寸，通道信息
    } else setprint("未读取到图片文件");
}

// 提取出图片的单个 B 通道
void MainWindow::on_pB_turn_channels_b_clicked()
{
    get_global_state();  // 获取全局设置状态

    if (!filename.isEmpty()) {

        image_state.process_mode = mode_set::one_channel_b;

        image_state.new_img = Image_mode(image_state.img, image_state.process_mode);

        QImage Qimg = Mat2QImage(image_state.img);
        Qimshow(ui->image, Qimg);

        QImage Qnewimg = Mat2QImage(image_state.new_img);
        Qimshow(ui->newimage, Qnewimg);

        Image_info_show();  // 显示图片尺寸，通道信息
    } else setprint("未读取到图片文件");
}

void MainWindow::on_pB_turn_channels_g_clicked()
{
    get_global_state();  // 获取全局设置状态

    if (!filename.isEmpty()) {

        image_state.process_mode = mode_set::one_channel_g;

        image_state.new_img = Image_mode(image_state.img, image_state.process_mode);

        QImage Qimg = Mat2QImage(image_state.img);
        Qimshow(ui->image, Qimg);

        QImage Qnewimg = Mat2QImage(image_state.new_img);
        Qimshow(ui->newimage, Qnewimg);

        Image_info_show();  // 显示图片尺寸，通道信息
    } else setprint("未读取到图片文件");
}

void MainWindow::on_pB_turn_channels_r_clicked()
{
    get_global_state();  // 获取全局设置状态

    if (!filename.isEmpty()) {

        image_state.process_mode = mode_set::one_channel_r;

        image_state.new_img = Image_mode(image_state.img, image_state.process_mode);

        QImage Qimg = Mat2QImage(image_state.img);
        Qimshow(ui->image, Qimg);

        QImage Qnewimg = Mat2QImage(image_state.new_img);
        Qimshow(ui->newimage, Qnewimg);

        Image_info_show();  // 显示图片尺寸，通道信息
    } else setprint("未读取到图片文件");
}

void MainWindow::on_pB_equalizehist_clicked()
{
    get_global_state();  // 获取全局设置状态

    if (!filename.isEmpty()) {

        image_state.process_mode = mode_set::equalizehist;

        image_state.new_img = Image_mode(image_state.img, image_state.process_mode);

        QImage Qimg = Mat2QImage(image_state.img);
        Qimshow(ui->image, Qimg);

        QImage Qnewimg = Mat2QImage(image_state.new_img);
        Qimshow(ui->newimage, Qnewimg);

        Image_info_show();  // 显示图片尺寸，通道信息
    } else setprint("未读取到图片文件");
}


// 将 newimage 转化为 image
void MainWindow::on_pB_turn_image_clicked()
{
    get_global_state();  // 获取全局设置状态

    if(!image_state.new_img.empty())
    {
        image_state.img = image_state.new_img;

        QImage Qimg = Mat2QImage(image_state.img);
        Qimshow(ui->image, Qimg);

        QImage Qnewimg = Mat2QImage(image_state.new_img);
        Qimshow(ui->newimage, Qnewimg);

    } else setprint("未选择处理模式");
}

// opencv单独弹窗显示，方便查看像素点
void MainWindow::on_pB_opencv_show_clicked()
{
    QString filename = QFileDialog::getOpenFileName(this,\
                                            QStringLiteral("请选择图片文件"), "D:",\
                                            QStringLiteral("图片文件(*png *jpg *tif *bmp *tiff)"));

    // 表示当选择的文件夹路径不为空时，执行 if 语句块中的代码
    if (!filename.isEmpty()) {

        cv::Mat img = cv::imread(filename.toStdString(), -1);
        if(img.channels() == 4)  img = cv::imread(filename.toStdString(), 1);

        cv::imshow(filename.toStdString(),img);
        cv::waitKey(0);
    } else setprint("未选择图片文件");
}


