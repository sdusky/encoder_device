#ifndef ENCODER_PACKAGE_H
#define ENCODER_PACKAGE_H
#include <QObject>
#include <QSettings>
#include <QDateTime>
#include <QDebug>
#include <QFile>
#include <QTimer>
#include <QThread>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <linux/types.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/select.h>
#include <fcntl.h>
#include <errno.h>
#include <stdint.h>
#include <linux/videodev2.h>
#include <error.h>
#include <unistd.h>

/**
 * @brief 配置信息结构体
 *
 */
typedef struct _settings_info settings_info;
struct _settings_info{
    QByteArray settings_time;                           /**< 设置本地时间 */
    QByteArray settings_addr;                           /**< 设置安装地点 */
    QByteArray settings_localip;                        /**< 设置本地IP */
    QByteArray settings_hostip;                         /**< 设置服务器IP */
    int        settings_state;
};

/**
 * @brief 读入配置信息
 *
 * @param info  配置信息
 */
void upload_settings(settings_info *info);

#define GET_DEVICE_HEART           "0000"
#define GET_DEVICE_TIME            "0001"
#define GET_DEVICE_ADDRESS         "0002"
#define GET_DEVICE_STATE           "0003"

#define SET_DEVICE_TIME            "0011"
#define SET_DEVICE_DECODERIP       "0012"
#define SET_DEVICE_STATE           "0013"
#define SET_DEVICE_EXIT            "0014"
#define SET_DEVICE_STOP            "0015"

#define SETTING_CODE_TYPE           "1001"
#define SETTING_CODE_ADDRESS        "1002"
#define SETTING_CODE_HOSTIP         "1003"
#define SETTING_CODE_LOCALIP        "1004"
#define SETTING_CODE_LOCALMAC       "1005"
#define SETTING_CODE_FILENAME       "1006"
#define SETTING_CODE_FILEEND        "1007"
#define SETTING_CODE_RESET          "1008"


#define ERROR_TCP_CONNECT           1
#define ERROR_SET_CONNECT           2

#define MAX_SENSOR_NUM                  3

extern settings_info device_setting_read;               /**< 本机配置 */

#define NB_BUFFER 4
#define DEFAULT_FPS 15

/**
 * @brief enum h264帧类型
 *
 */
enum h264_frame_type{
    FRAME_TYPE_I,
    FRAME_TYPE_P,
    FRAME_TYPE_B
};

/**
 * @brief 传感器结构体
 *
 */
struct sensor_label{
    QByteArray sensor_name_length;                     /**< 传感器名称长度 */
    QByteArray sensor_name;                            /**< 传感器名称 */
    QByteArray sensor_data;                            /**< 传感器信息 */
    int        has_data;                               /**< 是否有可以新信息 */
};

/**
 * @brief 编码图片结构体
 *
 */
struct encoded_picture_information{
    unsigned char              *buffer;                /**< 编码图片指针 */
    int                        length;                 /**< 指针内容长度 */
    unsigned long long         timepoint;              ///(us)
    enum h264_frame_type       frame_type;             /**< 帧类型 */
    void                       *usr_def;
    struct timeval             timestamp;              /**< 编码时间戳 */
    int                        height;                 /**< 图片高度 */
    int                        width;                  /**< 图片宽度 */
};
extern encoded_picture_information current_pic;        /**< 未编码图片信息 */
extern encoded_picture_information encoded_pic;        /**< 编码后图片信息 */

/**
 * @brief 摄像头配置信息结构体
 *
 */
struct camera_information{
    unsigned int               cam_width;              /**< 宽 */
    unsigned int               cam_height;             /**< 高 */
    unsigned int               cam_format;             /**< 配置采集格式：YUYV：YUV422 */
};


/**
 * @brief 视频采集设置结构体
 *
 */
struct video_information
{
    int                        fd;                      /**< 文件描述符  */
    struct v4l2_capability     cap;                     /**< 显示V4L2设备信息  */
    struct v4l2_format         fmt;                     /**< 视频的制式和帧格式 */
    struct v4l2_requestbuffers rb;                      /**< 帧缓冲  */
    struct v4l2_buffer         buf;                     /**< 物理内存 */
    enum   v4l2_buf_type       type;                    /**< 数据流类型 */
    struct v4l2_streamparm     parm;                    /**< 视频采集的帧率 */
    void                       *mem[NB_BUFFER];         /**< MAIN BUFFERS */
    unsigned char              *tmp_buffer;             /**< for mjpeg */
    unsigned char              *frame_buffer;           /**< TODO */
    unsigned int               frame_size_in;
    unsigned int               format;                  /**< YUYV or MJPEG */
    int                        width;
    int                        height;
    int                        is_streaming;
    int                        is_quit;
    enum v4l2_field            field;                    /**< 逐行 or 隔行 */
    unsigned int               bytes_per_line;
    unsigned int               size_image;
    enum v4l2_colorspace       color_space;              /**< 色域 */
    unsigned int               priv;
};

#endif // ENCODER_PACKAGE_H
