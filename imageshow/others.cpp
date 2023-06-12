#include <others.h>


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
        if(mat.channels()==4){
            cv::Mat gray_mat;
            cv::Mat threshold_mat;
            cv::cvtColor(mat, gray_mat, cv::COLOR_BGRA2GRAY);
            cv::threshold(gray_mat, threshold_mat, image_state.threshold.value, image_state.threshold.max, image_state.threshold.mode);

            return threshold_mat;
        }
        else if(mat.channels()==3){
            cv::Mat gray_mat;
            cv::Mat threshold_mat;
            cv::cvtColor(mat, gray_mat, cv::COLOR_BGR2GRAY);
            cv::threshold(gray_mat, threshold_mat, image_state.threshold.value, image_state.threshold.max, image_state.threshold.mode);

            return threshold_mat;
        }
        else{ // 若为单通道，直接阈值处理
            cv::Mat threshold_mat;
            cv::threshold(mat, threshold_mat, image_state.threshold.value, image_state.threshold.max, image_state.threshold.mode);

            return threshold_mat;
        }
    }
    if(mode == mode_set::gray)  // 对图片进行灰度处理
    {
        cv::Mat bgr_mat;
        cv::Mat gray_mat;

        switch(mat.channels()){
            case 1:
            cv::cvtColor(mat, bgr_mat, cv::COLOR_GRAY2BGR);
            cv::cvtColor(bgr_mat, gray_mat, cv::COLOR_BGR2GRAY);
            break;

            case 3:
            cv::cvtColor(mat, gray_mat, cv::COLOR_BGR2GRAY);
            break;

            case 4:
            cv::cvtColor(mat, gray_mat, cv::COLOR_BGRA2GRAY);
            break;
        }

        return gray_mat;
    }
    if(mode == mode_set::otsu)  // 对图片进行OTSU处理
    {
        cv::Mat bgr_mat;
        cv::Mat gray_mat, otsu_mat;

        switch(mat.channels()){
            case 1:
            cv::cvtColor(mat, bgr_mat, cv::COLOR_GRAY2BGR);
            cv::cvtColor(bgr_mat, gray_mat, cv::COLOR_BGR2GRAY);
            image_state.threshold.computed = cv::threshold(gray_mat, otsu_mat, 0, 255, image_state.threshold.mode|cv::THRESH_OTSU);
            break;

            case 3:
            cv::cvtColor(mat, gray_mat, cv::COLOR_BGR2GRAY);
            image_state.threshold.computed = cv::threshold(gray_mat, otsu_mat, 0, 255, image_state.threshold.mode|cv::THRESH_OTSU);
            break;

            case 4:
            cv::cvtColor(mat, gray_mat, cv::COLOR_BGRA2GRAY);
            image_state.threshold.computed = cv::threshold(gray_mat, otsu_mat, 0, 255, image_state.threshold.mode|cv::THRESH_OTSU);
            break;
        }

        return otsu_mat;
    }
    if(mode == mode_set::triangle)  // 对图片进行triangle阈值处理
    {
        cv::Mat bgr_mat;
        cv::Mat gray_mat, triangle_mat;

        switch(mat.channels()){
            case 1:
            cv::cvtColor(mat, bgr_mat, cv::COLOR_GRAY2BGR);
            cv::cvtColor(bgr_mat, gray_mat, cv::COLOR_BGR2GRAY);
            image_state.threshold.computed = cv::threshold(gray_mat, triangle_mat, 0, 255, image_state.threshold.mode|cv::THRESH_TRIANGLE);
            break;

            case 3:
            cv::cvtColor(mat, gray_mat, cv::COLOR_BGR2GRAY);
            image_state.threshold.computed = cv::threshold(gray_mat, triangle_mat, 0, 255, image_state.threshold.mode|cv::THRESH_TRIANGLE);
            break;

            case 4:
            cv::cvtColor(mat, gray_mat, cv::COLOR_BGRA2GRAY);
            image_state.threshold.computed = cv::threshold(gray_mat, triangle_mat, 0, 255, image_state.threshold.mode|cv::THRESH_TRIANGLE);
            break;
        }

        return triangle_mat;
    }
    if(mode == mode_set::resize)  // 对图片进行resize
    {

        cv::Mat resize_mat;
        cv::Size outputSize(image_state.resize.width, image_state.resize.hight);

        // 对图像进行缩放
        cv::resize(mat, resize_mat, outputSize, 0, 0, image_state.resize.mode);

        return resize_mat;
    }
    if(mode == mode_set::turn_channels_there)  // 对图片进行三通道转换
    {
        // 若不为单通道，则先强制转化为单通道，再阈值处理
        if(mat.channels()==4){
            cv::Mat bgr_mat;
            cv::cvtColor(mat, bgr_mat, cv::COLOR_BGRA2BGR);
            return bgr_mat;
        }
        else if(mat.channels()==1){
            cv::Mat bgr_mat;
            cv::cvtColor(mat, bgr_mat, cv::COLOR_GRAY2BGR);

            return bgr_mat;
        }
        else return mat;

    }
    if(mode == mode_set::erode)  // 对图片进行腐蚀
    {
        cv::Mat gray_mat, bgr_mat;
        cv::Mat kernel,erode_mat;

        // 创建一个充满零的单通道图像，其大小与输入图像相同

        kernel = getStructuringElement(image_state.erode.StructuringElement, \
                                        cv::Size(image_state.erode.kernel_hight, \
                                                 image_state.erode.kernel_width));

        // 必须单通道才能腐蚀膨胀
        switch(mat.channels()){
            case 1:
            cv::cvtColor(mat, bgr_mat, cv::COLOR_GRAY2BGR);
            cv::cvtColor(bgr_mat, gray_mat, cv::COLOR_BGR2GRAY);
            cv::erode(gray_mat, erode_mat, kernel, cv::Point(-1,-1), image_state.erode.iterations);
            break;

            case 3:
            cv::cvtColor(mat, gray_mat, cv::COLOR_BGR2GRAY);
            cv::erode(gray_mat, erode_mat, kernel, cv::Point(-1,-1), image_state.erode.iterations);
            break;

            case 4:
            cv::cvtColor(mat, gray_mat, cv::COLOR_BGRA2GRAY);
            cv::erode(gray_mat, erode_mat, kernel, cv::Point(-1,-1), image_state.erode.iterations);
            break;
        }

        return erode_mat;
    }
    if(mode == mode_set::dilate)  // 对图片进行膨胀
    {
        cv::Mat gray_mat, bgr_mat;
        cv::Mat kernel,dilate_mat;

        // 创建一个充满零的单通道图像，其大小与输入图像相同
        kernel = getStructuringElement(image_state.erode.StructuringElement, \
                                        cv::Size(image_state.erode.kernel_hight, \
                                                 image_state.erode.kernel_width));

        // 必须单通道才能腐蚀膨胀
        switch(mat.channels()){
            case 1:
            cv::cvtColor(mat, bgr_mat, cv::COLOR_GRAY2BGR);
            cv::cvtColor(bgr_mat, gray_mat, cv::COLOR_BGR2GRAY);
            cv::dilate(gray_mat, dilate_mat, kernel, cv::Point(-1,-1), image_state.dilate.iterations);
            break;

            case 3:
            cv::cvtColor(mat, gray_mat, cv::COLOR_BGR2GRAY);
            cv::dilate(gray_mat, dilate_mat, kernel, cv::Point(-1,-1), image_state.dilate.iterations);
            break;

            case 4:
            cv::cvtColor(mat, gray_mat, cv::COLOR_BGRA2GRAY);
            cv::dilate(gray_mat, dilate_mat, kernel, cv::Point(-1,-1), image_state.dilate.iterations);
            break;
        }

        return dilate_mat;
    }
    if(mode == mode_set::one_channel_b)  // 提取出图片的 B 通道
    {
        cv::Mat gray_mat,bgr_mat;
        cv::Mat b_mat;
        std::vector<cv::Mat> channel;//图像数组

        switch(mat.channels()){
            case 1:
             // cv::extractChannel(mat, g_mat, 1);
            cv::cvtColor(mat, bgr_mat, cv::COLOR_GRAY2BGR);
            cv::split(bgr_mat, channel);
            channel[1] = 0;
            channel[2] = 0;
            cv::merge(channel, b_mat);
            break;

            case 3:
            cv::split(mat, channel);
            channel[1] = 0;
            channel[2] = 0;
            cv::merge(channel, b_mat);
            break;

            case 4:
            cv::cvtColor(mat, bgr_mat, cv::COLOR_BGRA2BGR);
            cv::split(mat, channel);
            channel[1] = 0;
            channel[2] = 0;
            cv::merge(channel, b_mat);
            break;
        }

        return b_mat;
    }
    if(mode == mode_set::one_channel_g)  // 提取出图片的 G 通道
    {
        cv::Mat gray_mat,bgr_mat;
        cv::Mat g_mat;
        std::vector<cv::Mat> channel;//图像数组

        switch(mat.channels()){
            case 1:
            cv::cvtColor(mat, bgr_mat, cv::COLOR_GRAY2BGR);
            cv::split(bgr_mat, channel);
            channel[0] = 0;
            channel[2] = 0;
            cv::merge(channel, g_mat);
            break;

            case 3:
            cv::split(mat, channel);
            channel[0] = 0;
            channel[2] = 0;
            cv::merge(channel, g_mat);
            break;

            case 4:
            cv::cvtColor(mat, bgr_mat, cv::COLOR_BGRA2BGR);
            cv::split(mat, channel);
            channel[0] = 0;
            channel[2] = 0;
            cv::merge(channel, g_mat);
            break;
        }

        return g_mat;
    }
    if(mode == mode_set::one_channel_r)  // 提取出图片的 R 通道
    {
        cv::Mat gray_mat,bgr_mat;
        cv::Mat r_mat;
        std::vector<cv::Mat> channel;//图像数组

        switch(mat.channels()){
            case 1:
            cv::cvtColor(mat, bgr_mat, cv::COLOR_GRAY2BGR);
            cv::split(bgr_mat, channel);
            channel[0] = 0;
            channel[1] = 0;
            cv::merge(channel, r_mat);
            break;

            case 3:
            cv::split(mat, channel);
            channel[0] = 0;
            channel[1] = 0;
            cv::merge(channel, r_mat);
            break;

            case 4:
            cv::cvtColor(mat, bgr_mat, cv::COLOR_BGRA2BGR);
            cv::split(mat, channel);
            channel[0] = 0;
            channel[1] = 0;
            cv::merge(channel, r_mat);
            break;
        }

        return r_mat;
    }
    if(mode == mode_set::equalizehist)  // 对图片进行直方图均衡处理
    {
        cv::Mat gray_mat, bgr_mat;
        cv::Mat hist_mat;

        switch(mat.channels()){
            case 1:
            cv::cvtColor(mat, bgr_mat, cv::COLOR_GRAY2BGR);
            cv::cvtColor(bgr_mat, gray_mat, cv::COLOR_BGR2GRAY);
            cv::equalizeHist(gray_mat, hist_mat);
            break;

            case 3:
            cv::cvtColor(mat, gray_mat, cv::COLOR_BGR2GRAY);
            cv::equalizeHist(gray_mat, hist_mat);
            break;

            case 4:
            cv::cvtColor(mat, gray_mat, cv::COLOR_BGRA2GRAY);
            cv::equalizeHist(gray_mat, hist_mat);
            break;
        }

        return hist_mat;
    }
    if(mode == mode_set::flip_horizontal) //对图片进行左右翻转
    {
        cv::Mat horizontal_mat;
        cv::flip(mat, horizontal_mat, 1);
        return horizontal_mat;
    }
    if(mode == mode_set::flip_vertical) //对图片进行上下翻转
    {
        cv::Mat vertical_mat;
        cv::flip(mat, vertical_mat, 0);
        return vertical_mat;
    }
    if(mode == mode_set::padding) //对图片进行padding填充
    {
        cv::Mat bgra_mat, padding_mat;

        switch (image_state.padding.bordermode) {
            case 0:
            case 1:
            case 2:
            case 3:
            case 4:
            cv::copyMakeBorder(mat, padding_mat, \
                               image_state.padding.top, image_state.padding.bottom, \
                               image_state.padding.left, image_state.padding.right, \
                               image_state.padding.bordermode);
            break;
            case 5:
            cv::copyMakeBorder(mat, padding_mat, \
                               image_state.padding.top, image_state.padding.bottom, \
                               image_state.padding.left, image_state.padding.right, \
                               cv::BORDER_ISOLATED);
            break;
        }

        return padding_mat;
    }
    if(mode == mode_set::gray2color) //对图片进行灰度转彩色
    {
        cv::Mat gray_mat, bgr_mat;
        cv::Mat color_mat;

        switch(mat.channels()){
            case 1:
            cv::cvtColor(mat, bgr_mat, cv::COLOR_GRAY2BGR);
            cv::cvtColor(bgr_mat, gray_mat, cv::COLOR_BGR2GRAY);
            cv::applyColorMap(gray_mat, color_mat, cv::COLORMAP_BONE);
            break;

            case 3:
            cv::cvtColor(mat, gray_mat, cv::COLOR_BGR2GRAY);
            cv::applyColorMap(gray_mat, color_mat, cv::COLORMAP_BONE);
            break;

            case 4:
            cv::cvtColor(mat, gray_mat, cv::COLOR_BGRA2GRAY);
            cv::applyColorMap(gray_mat, color_mat, cv::COLORMAP_BONE);
            break;
        }

        return color_mat;
    }
    if(mode == mode_set::log_turn)  // 对图片进行对数变换
    {
        cv::Mat gray_mat, bgr_mat;
        cv::Mat float_mat, log_mat;

        // 必须单通道才能转换
        switch(mat.channels()){
            case 1:
            cv::cvtColor(mat, bgr_mat, cv::COLOR_GRAY2BGR);
            cv::cvtColor(bgr_mat, gray_mat, cv::COLOR_BGR2GRAY);

            gray_mat.convertTo(float_mat, CV_32F);  // 转换为32位浮点型图像
            cv::log(1 + float_mat, log_mat);  // 对数变换
            log_mat = log_mat * image_state.log.C;
            log_mat.convertTo(log_mat, CV_8U);  // 转换为8位无符号整型图像
            break;

            case 3:
            cv::cvtColor(mat, gray_mat, cv::COLOR_BGR2GRAY);

            gray_mat.convertTo(float_mat, CV_32F);  // 转换为32位浮点型图像
            cv::log(1 + float_mat, log_mat);  // 对数变换
            log_mat = log_mat * image_state.log.C;
            log_mat.convertTo(log_mat, CV_8U);  // 转换为8位无符号整型图像
            break;

            case 4:
            cv::cvtColor(mat, gray_mat, cv::COLOR_BGRA2GRAY);

            gray_mat.convertTo(float_mat, CV_32F);  // 转换为32位浮点型图像
            cv::log(1 + float_mat, log_mat);  // 对数变换
            log_mat = log_mat * image_state.log.C;
            log_mat.convertTo(log_mat, CV_8U);  // 转换为8位无符号整型图像
            break;
        }

        return log_mat;
    }
    if(mode == mode_set::gamma_turn) // 对图片进行gamma变换
    {
        cv::Mat gray_mat, bgr_mat;
        cv::Mat float_mat, gamma_mat;

        // 必须单通道才能转换
        switch(mat.channels()){
            case 1:
            cv::cvtColor(mat, bgr_mat, cv::COLOR_GRAY2BGR);
            cv::cvtColor(bgr_mat, gray_mat, cv::COLOR_BGR2GRAY);

            gray_mat.convertTo(float_mat, CV_32F);  // 转换为32位浮点型图像
            cv::pow(float_mat / 255.0, image_state.gamma.value, gamma_mat);
            gamma_mat *= 255;
            gamma_mat.convertTo(gamma_mat, CV_8U);  // 转换为8位无符号整型图像
            break;

            case 3:
            cv::cvtColor(mat, gray_mat, cv::COLOR_BGR2GRAY);

            gray_mat.convertTo(float_mat, CV_32F);  // 转换为32位浮点型图像
            cv::pow(float_mat / 255.0, image_state.gamma.value, gamma_mat);
            gamma_mat *= 255;
            gamma_mat.convertTo(gamma_mat, CV_8U);  // 转换为8位无符号整型图像
            break;

            case 4:
            cv::cvtColor(mat, gray_mat, cv::COLOR_BGRA2GRAY);

            gray_mat.convertTo(float_mat, CV_32F);  // 转换为32位浮点型图像
            cv::pow(float_mat / 255.0, image_state.gamma.value, gamma_mat);
            gamma_mat *= 255;
            gamma_mat.convertTo(gamma_mat, CV_8U);  // 转换为8位无符号整型图像
            break;
        }

        return gamma_mat;
    }
    if(mode == mode_set::blur_turn) // 均值滤波
    {
        cv::Mat gray_mat, bgr_mat;
        cv::Mat blur_mat;

        // 必须单通道才能转换
        switch(mat.channels()){
            case 1:
            cv::cvtColor(mat, bgr_mat, cv::COLOR_GRAY2BGR);
            cv::cvtColor(bgr_mat, gray_mat, cv::COLOR_BGR2GRAY);

            cv::blur(gray_mat, blur_mat, \
                     cv::Size(image_state.blur.kernel_hight, \
                              image_state.blur.kernel_width));
            break;

            case 3:
            cv::cvtColor(mat, gray_mat, cv::COLOR_BGR2GRAY);

            cv::blur(gray_mat, blur_mat, \
                     cv::Size(image_state.blur.kernel_hight, \
                              image_state.blur.kernel_width));
            break;

            case 4:
            cv::cvtColor(mat, gray_mat, cv::COLOR_BGRA2GRAY);

            cv::blur(gray_mat, blur_mat, \
                     cv::Size(image_state.blur.kernel_hight, \
                              image_state.blur.kernel_width));
            break;
        }

        return blur_mat;
    }
    if(mode == mode_set::boxfilter) // 盒子滤波
    {
        cv::Mat gray_mat, bgr_mat;
        cv::Mat boxfilter_mat;

        // 必须单通道才能转换
        switch(mat.channels()){
            case 1:
            cv::cvtColor(mat, bgr_mat, cv::COLOR_GRAY2BGR);
            cv::cvtColor(bgr_mat, gray_mat, cv::COLOR_BGR2GRAY);

            cv::boxFilter(gray_mat, boxfilter_mat, -1,\
                          cv::Size(image_state.boxfilter.kernel_hight, \
                                   image_state.boxfilter.kernel_width));
            break;

            case 3:
            cv::cvtColor(mat, gray_mat, cv::COLOR_BGR2GRAY);

            cv::boxFilter(gray_mat, boxfilter_mat, -1,\
                          cv::Size(image_state.boxfilter.kernel_hight, \
                                   image_state.boxfilter.kernel_width));
            break;

            case 4:
            cv::cvtColor(mat, gray_mat, cv::COLOR_BGRA2GRAY);

            cv::boxFilter(gray_mat, boxfilter_mat, -1,\
                          cv::Size(image_state.boxfilter.kernel_hight, \
                                   image_state.boxfilter.kernel_width));
            break;
        }

        return boxfilter_mat;
    }
    if(mode == mode_set::gaussianblur) // 高斯滤波
    {
        cv::Mat gray_mat, bgr_mat;
        cv::Mat gaussianblur_mat;

        // 必须单通道才能转换
        switch(mat.channels()){
            case 1:
            cv::cvtColor(mat, bgr_mat, cv::COLOR_GRAY2BGR);
            cv::cvtColor(bgr_mat, gray_mat, cv::COLOR_BGR2GRAY);

            cv::GaussianBlur(gray_mat, gaussianblur_mat,\
                          cv::Size(image_state.boxfilter.kernel_hight, \
                                   image_state.boxfilter.kernel_width), \
                          image_state.gaussianblur.sigma);
            break;

            case 3:
            cv::cvtColor(mat, gray_mat, cv::COLOR_BGR2GRAY);

            cv::GaussianBlur(gray_mat, gaussianblur_mat,\
                          cv::Size(image_state.boxfilter.kernel_hight, \
                                   image_state.boxfilter.kernel_width), \
                          image_state.gaussianblur.sigma);
            break;

            case 4:
            cv::cvtColor(mat, gray_mat, cv::COLOR_BGRA2GRAY);

            cv::GaussianBlur(gray_mat, gaussianblur_mat,\
                          cv::Size(image_state.boxfilter.kernel_hight, \
                                   image_state.boxfilter.kernel_width), \
                          image_state.gaussianblur.sigma);
            break;
        }

        return gaussianblur_mat;
    }
    if(mode == mode_set::medianblur) // 中值滤波
    {
        cv::Mat gray_mat, bgr_mat;
        cv::Mat medianblur_mat;

        // 滤波核大小只能为奇数
        image_state.medianblur.kernel_hight = makeOdd(image_state.medianblur.kernel_hight);

        // 必须单通道才能转换
        switch(mat.channels()){
            case 1:
            cv::cvtColor(mat, bgr_mat, cv::COLOR_GRAY2BGR);
            cv::cvtColor(bgr_mat, gray_mat, cv::COLOR_BGR2GRAY);

            cv::medianBlur(gray_mat, medianblur_mat, image_state.medianblur.kernel_hight);
            break;

            case 3:
            cv::cvtColor(mat, gray_mat, cv::COLOR_BGR2GRAY);

            cv::medianBlur(gray_mat, medianblur_mat, image_state.medianblur.kernel_hight);
            break;

            case 4:
            cv::cvtColor(mat, gray_mat, cv::COLOR_BGRA2GRAY);

            cv::medianBlur(gray_mat, medianblur_mat, image_state.medianblur.kernel_hight);
            break;
        }

        return medianblur_mat;
    }
    if(mode == mode_set::bilateralfilter) // 双边滤波
    {
        cv::Mat gray_mat, bgr_mat;
        cv::Mat bilateralfilter_mat;

        // 必须单通道才能转换
        switch(mat.channels()){
            case 1:
            cv::cvtColor(mat, bgr_mat, cv::COLOR_GRAY2BGR);
            cv::cvtColor(bgr_mat, gray_mat, cv::COLOR_BGR2GRAY);

            cv::bilateralFilter(gray_mat, bilateralfilter_mat, \
                                image_state.bilateralfilter.diameter, \
                                image_state.bilateralfilter.sigmaColor, \
                                image_state.bilateralfilter.sigmaSpace);
            break;

            case 3:
            cv::cvtColor(mat, gray_mat, cv::COLOR_BGR2GRAY);

            cv::bilateralFilter(gray_mat, bilateralfilter_mat, \
                                image_state.bilateralfilter.diameter, \
                                image_state.bilateralfilter.sigmaColor, \
                                image_state.bilateralfilter.sigmaSpace);
            break;

            case 4:
            cv::cvtColor(mat, gray_mat, cv::COLOR_BGRA2GRAY);

            cv::bilateralFilter(gray_mat, bilateralfilter_mat, \
                                image_state.bilateralfilter.diameter, \
                                image_state.bilateralfilter.sigmaColor, \
                                image_state.bilateralfilter.sigmaSpace);
            break;
        }

        return bilateralfilter_mat;
    }
    if(mode == mode_set::adaptivethreshold) // 自适应阈值滤波
    {
        cv::Mat gray_mat, bgr_mat;
        cv::Mat adaptivethreshold_mat;

        // 必须单通道才能转换
        switch(mat.channels()){
            case 1:
            cv::cvtColor(mat, bgr_mat, cv::COLOR_GRAY2BGR);
            cv::cvtColor(bgr_mat, gray_mat, cv::COLOR_BGR2GRAY);
            cv::adaptiveThreshold(gray_mat, adaptivethreshold_mat, \
                                  255, cv::ADAPTIVE_THRESH_MEAN_C, cv::THRESH_BINARY, 11, 8);
            break;

            case 3:
            cv::cvtColor(mat, gray_mat, cv::COLOR_BGR2GRAY);
            cv::adaptiveThreshold(gray_mat, adaptivethreshold_mat, \
                                  255, cv::ADAPTIVE_THRESH_MEAN_C, cv::THRESH_BINARY, 11, 8);
            break;

            case 4:
            cv::cvtColor(mat, gray_mat, cv::COLOR_BGRA2GRAY);
            cv::adaptiveThreshold(gray_mat, adaptivethreshold_mat, \
                                  255, cv::ADAPTIVE_THRESH_MEAN_C, cv::THRESH_BINARY, 11, 8);
            break;
        }

        return adaptivethreshold_mat;
    }
    if(mode == mode_set::canny_turn)  // canny边缘检测器
    {
        cv::Mat gray_mat, bgr_mat;
        cv::Mat canny_mat;

        // 对 apertureSize 进行奇数判断
        image_state.canny.apertureSize = makeOdd(image_state.canny.apertureSize);
        // apertureSize 必须3-7之间
        image_state.canny.apertureSize = (image_state.canny.apertureSize < 3) ? 3 : \
                                         (image_state.canny.apertureSize > 7) ? 7 : \
                                          image_state.canny.apertureSize;

        // 必须单通道才能转换
        switch(mat.channels()){
            case 1:
            cv::cvtColor(mat, bgr_mat, cv::COLOR_GRAY2BGR);
            cv::cvtColor(bgr_mat, gray_mat, cv::COLOR_BGR2GRAY);
            cv::Canny(gray_mat, canny_mat, \
                      image_state.canny.threshold1, \
                      image_state.canny.threshold2, \
                      image_state.canny.apertureSize);
            break;

            case 3:
            cv::cvtColor(mat, gray_mat, cv::COLOR_BGR2GRAY);
            cv::Canny(gray_mat, canny_mat, \
                      image_state.canny.threshold1, \
                      image_state.canny.threshold2, \
                      image_state.canny.apertureSize);
            break;

            case 4:
            cv::cvtColor(mat, gray_mat, cv::COLOR_BGRA2GRAY);
            cv::Canny(gray_mat, canny_mat, \
                      image_state.canny.threshold1, \
                      image_state.canny.threshold2, \
                      image_state.canny.apertureSize);
            break;
        }

        return canny_mat;
    }
    if(mode == mode_set::brightened_turn)  // brightened亮度调节
    {
        cv::Mat brightened_mat;

        mat.convertTo(brightened_mat, -1, image_state.brightened.alpha, 0);

        return brightened_mat;
    }

    if(mode == mode_set::rotationMatrix2d_turn)  // 旋转仿射变换
    {
        cv::Mat transformMatrix, rotationMatrix2d_mat;


        // 定义仿射变换矩阵
        transformMatrix = cv::getRotationMatrix2D(\
                            cv::Point2f(image_state.rotationMatrix2d.center_cols, \
                                        image_state.rotationMatrix2d.center_rows), \
                            image_state.rotationMatrix2d.angle, \
                            image_state.rotationMatrix2d.scale);

        cv::warpAffine(mat, rotationMatrix2d_mat, transformMatrix, mat.size());

        return rotationMatrix2d_mat;
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

// 将偶数变为奇数
int makeOdd(int num)
{
    if (num % 2 == 0) {
        // 如果是偶数，加上1
        num++;
    }
    return num;
}






