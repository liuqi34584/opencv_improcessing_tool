#include <others.h>

// opencv读取函数
cv::Mat Image_read(cv::String file_path)
{
    cv::Mat img = cv::imread(file_path, -1);
    if(img.channels() == 4) img = cv::imread(file_path, 1); //四通道图片转为三通道

    return img;
}


QImage Mat2QImage(const cv::Mat& mat)
{
    // 按照Qt帮助文档说明，像素buffer必须在QImage的生命周期内保持有效，且它默认是不会去释放像素buffer，QImage对象之间采用引用计数实现隐式共享底层数据
    if (mat.type() == CV_8UC1)                    // 单通道
    {
        QImage image(mat.cols, mat.rows, QImage::Format_Indexed8);
        image.setColorCount(256);                // 设置灰度级
        for (int i = 0; i < 256; i++)            // 设置颜色表
        {
            image.setColor(i, qRgb(i, i, i));
        }
        const uchar* pSrc = mat.data;
        for (int row = 0; row < mat.rows; row++)
        {
            uchar *pDest = image.scanLine(row);
            memcpy(pDest, pSrc, mat.cols);
            pSrc += mat.step;
        }

        return image;
    }
    else if (mat.type() == CV_8UC3)                // 三通道
    {
#if 0
        cv::Mat rgbMat;
        cv::cvtColor(mat, rgbMat, cv::COLOR_BGR2RGB);
        // 如果不返回副本，mat在QImage对象生命周期结束之前析构了，则使用QImage对象会崩溃
        return QImage(rgbMat.data, rgbMat.cols, rgbMat.rows, rgbMat.step, QImage::Format_RGB888).copy();
#endif

#if 1
        QImage image(mat.data, mat.cols, mat.rows, mat.cols*mat.channels(), QImage::Format_RGB888);

        // 因为Mat内部的颜色顺序默认是BGR，也可以在构造之前使用cv::cvtColor(mat, Rgb, CV_BGR2RGB);//颜色空间转换
        return image.rgbSwapped();// 返回转换之后的副本，不会改变原来的对象
#endif
    }
    else if (mat.type() == CV_8UC4)
    {
        // mat.step表示以字节为单位的矩阵的宽度，等价于mat.cols*mat.channels()
        QImage image(mat.data, mat.cols, mat.rows, mat.step, QImage::Format_ARGB32);
        return image.copy();// 深拷贝
    }
    else
    {
        return QImage();
    }
}


void Qimshow(QLabel *label, QImage Qimg)
{
    Qimg = Qimg.scaled(label->size(),Qt::KeepAspectRatio);
    label->setPixmap(QPixmap::fromImage(Qimg));
}

void Qimclear(QLabel *label)
{
    label->clear();
}

// 读取指定文件夹，返回该文件夹下的文件列表
QVector<QString> get_directory_list(QString path)
{
    // 声明一个空的字符串列表容器
    QVector<QString> imagePathList;

    QDir dir(path);

    QFileInfoList files = dir.entryInfoList(QDir::Files);
    foreach (QFileInfo file, files) imagePathList.append(file.absoluteFilePath());

    return imagePathList;
}

// 图像处理函数，传入图像原mat数据，以及要处理的模式
cv::Mat Image_mode(cv::Mat mat, uint8_t mode)
{
    if(mode == mode_set::show)  // 显示原图
    {
        return mat;
    }
    if(mode == mode_set::twovalue)  // 对图片进行二值处理
    {

        // 若不为单通道，则先强制转化为单通道，再阈值处理
        if(mat.channels()!=1){
            cv::Mat gray_mat;
            cv::Mat threshold_mat;
            cv::cvtColor(mat, gray_mat, cv::COLOR_BGR2GRAY);
            // 若为单通道，直接阈值处理
            cv::threshold(gray_mat, threshold_mat, image_state.threshold.value, image_state.threshold.max, image_state.threshold.mode);

            return threshold_mat;
        } else {
            cv::Mat threshold_mat;
            // 若为单通道，直接阈值处理
            cv::threshold(mat, threshold_mat, image_state.threshold.value, image_state.threshold.max, image_state.threshold.mode);

            return threshold_mat;
        }

    }
    if(mode == mode_set::gray)  // 对图片进行灰度处理
    {
        // 不为单通道，才有转灰度的概念
        if(mat.channels() != 1) {
            cv::Mat gray_mat;
            cv::cvtColor(mat, gray_mat, cv::COLOR_BGR2GRAY);

            return gray_mat;
        } else {
            cv::Mat bgr_mat;
            cv::Mat gray_mat;
            cv::cvtColor(mat, bgr_mat, cv::COLOR_GRAY2BGR);
            cv::cvtColor(bgr_mat, gray_mat, cv::COLOR_BGR2GRAY);

            return gray_mat;
        }
    }
    if(mode == mode_set::resize)  // 对图片进行resize
    {

        cv::Mat resize_mat;
        cv::Size outputSize(image_state.resize.width, image_state.resize.hight);

        // 对图像进行缩放
        cv::resize(mat, resize_mat, outputSize, 0, 0, image_state.resize.mode);

        return resize_mat;
    }

    return mat; //默认返回值
}

// 字符串补0函数
std::string zfill(std::string str, size_t width) {
    if (str.length() < width) {
        // 将字符串 str 前面填充 0 直到字符串长度达到 width
        str = std::string(width - str.length(), '0') + str;
    }
    return str;
}

// 找出字符串中的数字,注意这里引用,方便传回多个参数
void find_num(std::string str, std::string& str_head, int& number, int& number_width, std::string& str_end)
{
    qint32 num_start_index = -100;  // 记录数字在str的头索引值
    qint32 num_end_index = -100;  // 记录数字在str的尾索引值

    for (size_t i = 0; i < str.length(); i++) {
        // 获取数字在str中的头尾索引值
        if (isdigit(str[i])) {
            if(num_start_index == -100) num_start_index=i;
            num_end_index = i+1;
        }
    }

    if(num_start_index == -100){  // 判断字符串中是否存在数字
        number = 0;
        num_start_index = str.length() - 4;
        num_end_index = str.length() - 4;
    }
    else{
        // 取出字符中的数字段，将其转化为整形，“0056” -> 56
        std::string str_num = str.substr(num_start_index, num_end_index-num_start_index);
        number = std::stoi(str_num);
    }

    number_width = num_end_index-num_start_index;
    str_head = str.substr(0, num_start_index);
    str_end = str.substr(num_end_index);
}

// 识别字符串中的中文
bool containChinese(const std::string& str) {
    for (auto c : str) {
        if (static_cast<unsigned char>(c) > 127) {
            return true;
        }
    }
    return false;
}

// 判断文件名是否合理
bool isReasonablefile(cv::String filename)
{
    std::vector<std::string> file_type {".png", ".jpg", ".tif", ".gif"};

    // 先检查文件名后缀是否合理
    auto it = std::find(file_type.begin(), file_type.end(), filename.substr(filename.length() - 4));

    if (it == file_type.end() || containChinese(filename) == true) return false;
    else return true;
}








