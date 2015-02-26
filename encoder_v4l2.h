/************************命令标志符********************************
        VIDIOC_REQBUFS  ：  分配内存
        VIDIOC_QUERYBUF ：  把VIDIOC_REQBUFS中分配的数据缓存转换成物理地址
        VIDIOC_QUERYCAP ：  查询驱动功能
        VIDIOC_ENUM_FMT ：  获取当前驱动支持的视频格式
        VIDIOC_S_FMT    ：  设置当前驱动的频捕获格式
        VIDIOC_G_FMT    ：  读取当前驱动的频捕获格式
        VIDIOC_TRY_FMT  ：  验证当前驱动的显示格式
        VIDIOC_CROPCAP  ：  查询驱动的修剪能力
        VIDIOC_S_CROP   ：  设置视频信号的边框
        VIDIOC_G_CROP   ：  读取视频信号的边框
        VIDIOC_QBUF     ：  把数据放回缓存队列
        VIDIOC_DQBUF    ：  把数据从缓存中读取出来
        VIDIOC_STREAMON ：  开始视频显示函数
        VIDIOC_STREAMOFF：  结束视频显示函数
        VIDIOC_QUERYSTD ：  检查当前视频设备支持的标准，例如PAL或NTSC。
**********************************************************************/
/*********************************操作流程*****************************
1. 打开设备文件。
2. 取得设备的capability，看看设备具有什么功能，比如是否具有视频输入,或者音频输入输出等。VIDIOC_QUERYCAP,struct v4l2_capability
3. 选择视频输入，一个视频设备可以有多个视频输入。VIDIOC_S_INPUT,struct v4l2_input(可不要)
4. 设置视频的制式和帧格式，制式包括PAL，NTSC，帧的格式个包括宽度和高度等。
5. 向驱动申请帧缓冲，一般不超过5个。struct v4l2_requestbuffers
6. 申请物理内存,将申请到的帧缓冲映射到用户空间。
   将申请到的帧缓冲全部入队列，以便存放采集到的数据.VIDIOC_QBUF,struct v4l2_buffer
7. 开始视频的采集。
8. 出队列以取得已采集数据的帧缓冲，取得原始采集数据。VIDIOC_DQBUF,
   将缓冲重新入队列尾,这样可以循环采集。VIDIOC_QBUF
9. 停止视频的采集。VIDIOC_STREAMOFF
10. 关闭视频设备。close(fd);
*********************************************************************/
#ifndef ENCODER_V4L2_H
#define ENCODER_V4L2_H

#include <QObject>
#include <iostream>
#include "encoder_package.h"

using namespace std;

/**
 * @brief Video For Linux 2 摄像头采集封装 v4l2_api
 *        邢大天 2014-06-06
 *
 */
class encoder_v4l2 : public QObject
{
    Q_OBJECT
public:
    explicit encoder_v4l2(QObject *parent = 0);

    /**
     * @brief 检查摄像头状态、功能及格式
     * @see   1.int v4l2_get_capability(struct video_information *vd_info);
     *        2.int v4l2_get_format(struct video_information *vd_info);
     * @other  步骤：1.阻塞方式打开设备
     *              2.调用v4l2_get_capability获取V4L2设备信息
     *              3.调用v4l2_get_format获取摄像头支持格式
     *              4.关闭摄像头，返回
     * @param dev_name 摄像头名称
     * @param vd_info  采集信息结构体
     * @return int     返回码
     */
    int v4l2_check(char *dev_name,struct video_information *vd_info);

    /**
     * @brief 摄像头采集初始化
     * @other步骤：       1.设置摄像头输出信息 高、宽、采集格式
     *                   2.根据格式分配内存：（1）MJPEG frame_buffer解码 tmp_buffer
     *                                    （2）YUYV  tmp_buffer
     *                   3.阻塞打开获取capability
     *                   4.设置输出格式
     *                   5.设置流参数 包括帧数
     *                   6.申请缓冲区                VIDIOC_REQBUFS
     *                   7.将缓冲区映射至应用程序内存   mmap()
     *                   8.缓冲帧放入缓冲队列
     *                   9.启动流
     * @see              1.int v4l2_set_format(struct video_information *vd_info,\
     *                          struct camera_infomation cam_info);
     * @see              2.int v4l2_set_streamparam(struct video_information *vd_info);
     * @see              3.int v4l2_on(struct video_information *vd_info);
     * @param dev_name   设备名称
     * @param vd_info    采集信息
     * @param cam_info   摄像头参数
     * @return int       返回码
     */
    int v4l2_init(char* dev_name,struct video_information *vd_info, \
                  struct camera_information cam_info);

    /**
     * @brief 色彩空间转换YUV422->RGB888
     * @other       步骤：1.将YUV图像分为4个字节1组Y0 U Y1 V
     *                   2.将Y0 U V与 Y1 U V分别送入v4l2_yuv422_to_rgb888_pixel函数
     *                   3.得到RGB图像
     * @see  int v4l2_yuv422_to_rgb888_pixel(int y,int u,int v);
     * @param yuv422   YUV422空间 YUYV YUYV      内存 w * h * 2
     * @param rgb888   RGB888空间 RGB  RGB       内存 w * h * 3
     * @param cam_info 摄像头参数（宽，高）
     */
    void v4l2_yuv422_to_rgb888(unsigned char *yuv422,unsigned char *rgb888,\
                              struct camera_information cam_info);

    /**
     * @brief 色彩空间转换YUV422->RGB888
     *
     * @param yuv422 YUV422空间 YUYV YUYV      内存 w * h * 2
     * @param yuv420 YUV420空间 YUV420数据YYYY YYYY UU VV 内存 w * h * 3 / 2
     * @param cam_info
     */
    void v4l2_yuv422_to_yuv420(unsigned char *yuv422,unsigned char *yuv420,\
                               struct camera_information cam_info);

    /**
     * @brief 关闭摄像头
     *
     * @param vd_info 采集信息
     * @return int    返回码
     */
    int v4l2_close(struct video_information *vd_info);

private:

    /**
     * @brief 获取V4L2设备信息
     * @see   IOCTL    VIDIOC_QUERYCAP
     * @param vd_info  采集信息
     * @return int     返回码
     */
    int v4l2_get_capability(struct video_information *vd_info);

    /**
     * @brief 获取摄像头支持格式并输出
     * @see   IOCTL    查询并显示所有支持的格式  VIDIOC_ENUM_FMT
     * @see   IOCTL    查看当前格式            VIDIOC_G_FMT
     * @param vd_info  采集信息
     * @return int     返回码
     */
    int v4l2_get_format(struct video_information *vd_info);

    /**
     * @brief 设置摄像头输出格式
     * @see   IOCTL    设置当前格式            VIDIOC_S_FMT
     * @param vd_info  采集信息
     * @param cam_info 摄像头信息
     * @return int     返回码
     */
    int v4l2_set_format(struct video_information *vd_info,\
                        struct camera_information cam_info);

    /**
     * @brief 开始捕捉
     * @see   IOCTL    开始视频的采集           VIDIOC_STREAMON
     * @param vd_info  采集信息
     * @return int     返回码
     */
    int v4l2_on(struct video_information *vd_info);

    /**
     * @brief 关闭捕捉
     * @see  IOCTL    停止视频的采集           VIDIOC_STREAMOFF
     * @param vd_info 采集信息
     * @return int    返回码
     */
    int v4l2_off(struct video_information *vd_info);

    /**
     * @brief 设置帧率
     * @see   IOCTL    设置采集帧率            VIDIOC_S_PARM
     * @param vd_info 采集信息
     * @return int    返回码
     */
    int v4l2_set_streamparam(struct video_information *vd_info);

    /**
     * @brief yuv转rgb
     * @other       公式：  R = Y + 1.4075 *（V-128）
     *                     G = Y – 0.3455 *（U –128） – 0.7169 *（V –128）
     *                     B = Y + 1.779 *（U – 128）
     * @param y
     * @param u
     * @param v
     * @return int
     */
    int v4l2_yuv422_to_rgb888_pixel(int y,int u,int v);

    /**
     * @brief 获取缓冲帧
     * @other       步骤：1.判断是否流开启
     *                   2.出队列以取得已采集数据的帧缓冲，取得原始采集数据。VIDIOC_DQBUF
     *                   3.将数据放入tmp_buffer
     * @return int  返回码
     */
    int v4l2_grab();
    video_information           *video_info;
    camera_information          camera_info;

public slots:

    /**
     * @brief 槽函数，采集
     *
     */
    void v4l2_capture();
signals:

    /**
     * @brief 信号，编码当前帧
     *
     */
    void need_to_encoder();
};

#endif // ENCODER_V4L2_H
