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
#include <sys/mman.h>       //for map_shared
#include <sys/select.h>
#include <fcntl.h>
#include <errno.h>
#include <stdint.h>
#include <linux/videodev2.h>
#include <error.h>
#include <unistd.h>

typedef struct _settings_info settings_info;
struct _settings_info{
    QByteArray settings_time;
    QByteArray settings_addr;
    QByteArray settings_localip;
    QByteArray settings_hostip;
    int        settings_state;
};

void upload_settings(settings_info *info);

#define GET_DEVICE_HEART           "0000"
#define GET_DEVICE_TIME            "0001"
#define GET_DEVICE_ADDRESS         "0002"
#define GET_DEVICE_STATE           "0003"

#define SET_DEVICE_TIME            "0011"
#define SET_DEVICE_DECODERIP       "0012"
#define SET_DEVICE_STATE           "0013"
#define SET_DEVICE_EXIT            "0014"

#define SETTING_CODE_TYPE           "1001"
#define SETTING_CODE_ADDRESS        "1002"
#define SETTING_CODE_HOSTIP         "1003"
#define SETTING_CODE_LOCALIP        "1004"
#define SETTING_CODE_FILENAME       "1005"
#define SETTING_CODE_FILEEND        "1006"

#define ERROR_TCP_CONNECT           1
#define ERROR_SET_CONNECT           2

#define SENSOR_NUM                  3

extern settings_info device_setting_read;

/**********************************************************************/
#define NB_BUFFER 4
#define DEFAULT_FPS 25

enum h264_frame_type{
    FRAME_TYPE_I,
    FRAME_TYPE_P,
    FRAME_TYPE_B
};
struct sensor_label{
    QByteArray sensor_name_length;          //传感器标示长度
    QByteArray sensor_name;                 //传感器标示
    QByteArray sensor_data;                 //传感器信息
    int        has_data;                    //传感器信息更新标示
};

struct encoded_picture_information{
    unsigned char              *buffer;
    int                        length;
    unsigned long long         timepoint;                    //(us)
    enum h264_frame_type       frame_type;
    void                       *usr_def;
    struct timeval             timestamp;
    int                        height;
    int                        width;
};

struct camera_information{
    unsigned int               cam_width;
    unsigned int               cam_height;
    unsigned int               cam_format;
};


struct video_information
{
    int                        fd;                           //文件描述符
    struct v4l2_capability     cap;                          //显示V4L2设备信息
    struct v4l2_format         fmt;                          //视频的制式和帧格式
    struct v4l2_requestbuffers rb;                           //帧缓冲
    struct v4l2_buffer         buf;                          //物理内存
    enum   v4l2_buf_type       type;                         //数据流类型
    struct v4l2_streamparm     parm;                         //视频采集的帧率
    void                       *mem[NB_BUFFER];              //MAIN BUFFERS
    unsigned char              *tmp_buffer;                  //for mjpeg
    unsigned char              *frame_buffer;                //one frame buffer here
    unsigned int               frame_size_in;
    unsigned int               format;                       //YUYV or MJPEG
    int                        width;
    int                        height;
    int                        is_streaming;                 //start capture
    int                        is_quit;
    enum v4l2_field            field;                        //隔行或逐行
    unsigned int               bytes_per_line;
    unsigned int               size_image;
    enum v4l2_colorspace       color_space;                  //色域
    unsigned int               priv;
};

#endif // ENCODER_PACKAGE_H
