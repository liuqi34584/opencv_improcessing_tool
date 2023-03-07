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

/*添加功能步骤：
 * 1.在mainwindows中布局
 * 2.为布局中的按钮添加槽函数
 * 3.在mainwindows.h文件中为mode[10]添加描述
 * 4.为对应的按钮功能函数添加功能代码,批量模式同步添加功能
 * */


// 功能函数:参数1-显示图像的Label，参数2-要显示的Mat
void MainWindow::matToQimageLabelShow(QLabel *label, cv::Mat mat)
{
    cv::Mat Rgb;
    QImage Img;
    if (mat.channels() == 3) {  // RGB Img
        cv::cvtColor(mat, Rgb, cv::COLOR_BGR2RGB);  //颜色空间转换
        Img = QImage((const uchar*)(Rgb.data), Rgb.cols, Rgb.rows, Rgb.cols * Rgb.channels(), QImage::Format_RGB888);
    }
    else {  //Gray Img
        Img = QImage((const uchar*)(mat.data), mat.cols, mat.rows, mat.cols*mat.channels(), QImage::Format_Grayscale8);
    }
    Img = Img.scaled(label->size(),Qt::KeepAspectRatio);
    label->setPixmap(QPixmap::fromImage(Img));
}

// 功能函数:对应二值处理功能
cv::Mat MainWindow::gray_value(cv::Mat mat)
{
    cv::Mat gray_new_img;
    uint8_t gray_value = ui->twovalue_cB_value->currentText().mid(3).toInt();
    uint8_t gray_max_value = ui->twovalue_cB_Max->currentText().mid(4).toInt();
    uint8_t gray_type = ui->twovalue_cB_type->currentIndex();
    cv::threshold(mat, gray_new_img, gray_value, gray_max_value,gray_type);

    return gray_new_img;
}

// 功能函数:获取文件夹中的文件列表
int getImagePathList(cv::String folder, std::vector<cv::String> &imagePathList)
{
    //search all the image in a folder
    cv::glob(folder,imagePathList);
    return 0;
}

// 功能函数:保存图片信息到文件中
void MainWindow::save_image()
{
    cv::String str;

    if(mode[0]==0 || mode[0]== 2) { // 保存单张图片
        if (!new_img.data) { // Mat数据为空
            ui->newimage->setText("没有新的数据处理\n保存无效！！！");
        }
        else { // 单张图片直接保存new_img中的数据，不用关心处于何种模式
            str = save_dirname.toStdString();
            str = str + "/" + "newimg.png"; // 新文件的名字
            cv::imwrite(str,new_img);
        }
     }
    if(mode[0]==1) { // 批量图片要关心处理模式
        QString str_fillenum;
        std::vector<cv::String> imagePathList;
        getImagePathList(dirname.toStdString() + "//",imagePathList);

        for (uint i = 0; i < imagePathList.size(); i++) //找到文件数量，遍历处理
        {

            if(mode[1]==1){
                // 显示原图片
                MainWindow::img = cv::imread(imagePathList[i],-1);
                matToQimageLabelShow(ui->image, img);

                // 显示处理结果图片
                MainWindow::new_img = cv::imread(imagePathList[i],0);
                new_img = gray_value(new_img);
                matToQimageLabelShow(ui->newimage, new_img);
            }
            if(mode[2]==1){
                // 显示原图片
                MainWindow::img = cv::imread(imagePathList[i],-1);
                matToQimageLabelShow(ui->image, img);

                // 显示处理结果图片
                MainWindow::new_img = cv::imread(imagePathList[i],0);
                matToQimageLabelShow(ui->newimage,new_img);
            }
            if(mode[3]==1){
                // 显示原图片
                MainWindow::img = cv::imread(imagePathList[i],-1);
                matToQimageLabelShow(ui->image, img);

                // 显示处理结果图片
                uint gao = ui->lE_resize_gao->text().toUInt();
                uint kuan = ui->lE_resize_kuan->text().toUInt();
                uint8_t r_type = ui->cB_resize_type->currentIndex();
                cv::resize(img, new_img, cv::Size(gao, kuan), 0, 0, r_type);
                matToQimageLabelShow(ui->newimage,new_img);
            }
//            if(mode[2]==1){
//                 待开发模式
//            }

            //保存图片，一个循环保存一次
            str_fillenum.sprintf("/newimg_ %05d.png",i);
            cv::imwrite((save_dirname + str_fillenum).toStdString(),new_img);
        }
    }
}

// 功能函数:文件夹模式预显示图像
void MainWindow::dir_pre_show_image()
{
    cv::String str;

    if(mode[0]== 0 || mode[0]== 2) {
        if (!new_img.data) { // Mat数据为空
            ui->newimage->setText("没有新的数据处理\n保存无效！！！");
        }
     }
    if(mode[0]==1) { // 批量图片要关心处理模式
        QString str_fillenum;
        std::vector<cv::String> imagePathList;
        getImagePathList(dirname.toStdString() + "//",imagePathList);

        if(mode[1]==1){
            MainWindow::new_img = cv::imread(imagePathList[0], 0);
            new_img = gray_value(new_img);

            matToQimageLabelShow(ui->newimage, new_img); // 界面显示图片
            QString labeltext_str;
            labeltext_str.sprintf("高：%d  宽：%d  %d通道      高：%d  宽：%d  %d通道", \
                                  img.rows,img.cols,img.channels(), \
                                  new_img.rows,new_img.cols,new_img.channels());
            ui->labeltext->setText(labeltext_str);
        }
        if(mode[2]==1){
            MainWindow::new_img = cv::imread(imagePathList[0], 0);

            matToQimageLabelShow(ui->newimage, new_img); // 界面显示图片
            QString labeltext_str;
            labeltext_str.sprintf("高：%d  宽：%d  %d通道      高：%d  宽：%d  %d通道", \
                                  img.rows,img.cols,img.channels(), \
                                  new_img.rows,new_img.cols,new_img.channels());
            ui->labeltext->setText(labeltext_str);
        }
        if(mode[3]==1){
            MainWindow::new_img = cv::imread(imagePathList[0],-1);

            uint gao = ui->lE_resize_gao->text().toUInt();
            uint kuan = ui->lE_resize_kuan->text().toUInt();
            uint8_t r_type = ui->cB_resize_type->currentIndex();

            cv::resize(new_img, new_img, cv::Size(gao, kuan), 0, 0, r_type);
            matToQimageLabelShow(ui->newimage,new_img);

            matToQimageLabelShow(ui->newimage, new_img); // 界面显示图片
            QString labeltext_str;
            labeltext_str.sprintf("高：%d  宽：%d  %d通道      高：%d  宽：%d  %d通道", \
                                  img.rows,img.cols,img.channels(), \
                                  new_img.rows,new_img.cols,new_img.channels());
            ui->labeltext->setText(labeltext_str);
        }
//        if(mode[2]==1){
//             待开发模式
//        }

    }
}

//  按钮函数:加载单张图片，并且显示图片
void MainWindow::on_pB_load_one_clicked()
{
    QFile file;

    if(mode[0] == 2){
        file.setFileName(filename);
        file.remove();
    }

    filename = QFileDialog::getOpenFileName(this,\
                                            QStringLiteral("请选择图片文件"),\
                                            "D:",\
                                           QStringLiteral("图片文件(*png *jpg *tif *gif)"));

    img = cv::imread(filename.toStdString(),-1);
    QString labeltext_str;
    labeltext_str.sprintf("高：%d  宽：%d  %d通道",img.rows,img.cols,img.channels());
    ui->labeltext->setText(labeltext_str);
    matToQimageLabelShow(ui->image,img);
    mode[0]= 0;mode[1]= 0;mode[2]= 0;mode[3]= 0;mode[4]= 0;
    mode[5]= 0;mode[6]= 0;mode[7]= 0;mode[8]= 0;mode[9]= 0;
}

// 按钮函数:加载批量图片的文件夹
void MainWindow::on_pB_load_dir_clicked()
{
    QFile file;

    if(mode[0] == 2){
        file.setFileName(filename);
        file.remove();
    }

    dirname = QFileDialog::getExistingDirectory(); // 弹出选择文件夹选项

    std::vector<cv::String> imagePathList;
    getImagePathList(dirname.toStdString() + "/",imagePathList); // 得到文件夹下的文件列表
    matToQimageLabelShow(ui->image,cv::imread(imagePathList[0], -1)); // 显示文件夹的第一张图片

    mode[0]= 1;mode[1]= 0;mode[2]= 0;mode[3]= 0;mode[4]= 0;
    mode[5]= 0;mode[6]= 0;mode[7]= 0;mode[8]= 0;mode[9]= 0;
}

// 按钮函数：加载图的保存路径
void MainWindow::on_pB_save_dir_clicked()
{
    save_dirname = QFileDialog::getExistingDirectory();
    save_image();
}

// 按钮函数:处理二值图像并且展示
void MainWindow::on_pushButton_imtwovalue_clicked()
{
    if(mode[0]==0 || mode[0]== 2) { // 模式0 2表示：不必批量处理,处理生成单个new_img
        MainWindow::new_img = cv::imread(filename.toStdString(),0);
        new_img = gray_value(new_img);
        matToQimageLabelShow(ui->newimage,new_img);

        QString labeltext_str;
        labeltext_str.sprintf("高：%d  宽：%d  %d通道      高：%d  宽：%d  %d通道", \
                              img.rows,img.cols,img.channels(), \
                              new_img.rows,new_img.cols,new_img.channels());
        ui->labeltext->setText(labeltext_str);
        }
    if(mode[0]==1) { // 模式1 表示批量处理，生成多个new_img
        mode[1]=1;
        dir_pre_show_image();
    }
}

// 按钮函数：处理灰度图像并且显示
void MainWindow::on_pushButton_gray_value_clicked()
{
    if(mode[0]==0 || mode[0]== 2) { // 模式0 2表示：不必批量处理,处理生成单个new_img
        MainWindow::new_img = cv::imread(filename.toStdString(),0);
        matToQimageLabelShow(ui->newimage,new_img);

        QString labeltext_str;
        labeltext_str.sprintf("高：%d  宽：%d  %d通道      高：%d  宽：%d  %d通道", \
                              img.rows,img.cols,img.channels(), \
                              new_img.rows,new_img.cols,new_img.channels());
        ui->labeltext->setText(labeltext_str);
        }
    if(mode[0]==1) { // 模式1 表示批量处理，生成多个new_img
        mode[2]=1;
        dir_pre_show_image();
    }
}

// 按钮函数，对图片resize大小且显示
void MainWindow::on_pb_resize_clicked()
{
    if(mode[0]==0 || mode[0]== 2) { // 模式0 2表示：不必批量处理,处理生成单个new_img
        MainWindow::new_img = cv::imread(filename.toStdString(),-1);

        uint gao = ui->lE_resize_gao->text().toUInt();
        uint kuan = ui->lE_resize_kuan->text().toUInt();
        uint8_t r_type = ui->cB_resize_type->currentIndex();
        cv::resize(new_img, new_img, cv::Size(gao, kuan), 0, 0, r_type);
        matToQimageLabelShow(ui->newimage,new_img);

        QString labeltext_str;
        labeltext_str.sprintf("高：%d  宽：%d  %d通道      高：%d  宽：%d  %d通道", \
                              img.rows,img.cols,img.channels(), \
                              new_img.rows,new_img.cols,new_img.channels());
        ui->labeltext->setText(labeltext_str);
        }
    if(mode[0] == 1) { // 模式1 表示批量处理，生成多个new_img
        mode[3] = 1; //模式三的批量处理开关
        dir_pre_show_image();
    }
}


// 按钮函数：将右图变换到左边
void MainWindow::on_pB_turn_image_clicked()
{
    cv::String str;
    if(mode[0] == 1) {
        ui->newimage->setText("批量处理时，不可执行本操作！");
    }
    else {
        if (!new_img.data) { // Mat数据为空
            ui->newimage->setText("请先选择模式，再转换！！！");
        }
        else {
            // 先把右图作为文件保存
            str = filename.toStdString();
            cv::String::size_type iPos = str.find_last_of('/') + 1;
            str = str.substr(0, iPos) + "prosess.png";
            cv::imwrite(str,new_img);
            filename = QString::fromStdString(str);

            // 再把右图Mat数据转换到左图,并且显示
            img = cv::imread(str, -1);
            matToQimageLabelShow(ui->image,img);

            QString labeltext_str;
            labeltext_str.sprintf("高：%d  宽：%d  %d通道",img.rows,img.cols,img.channels());
            ui->labeltext->setText(labeltext_str);

            ui->newimage->setText("转换完成！！！");
            mode[0] = 2;
        }
    }
}

void MainWindow::closeEvent(QCloseEvent *event)
{
    QFile file;
    if(mode[0] == 2){
        file.setFileName(filename);
        file.remove();
    }
    event->accept();
}




