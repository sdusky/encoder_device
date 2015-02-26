#include <QCoreApplication>
#include "encoder_package.h"
#include "encoder_settings.h"
#include "encoder_tcpsocket.h"
#include "encoder_udpsocket.h"
#include "encoder_v4l2.h"
#include "encoder_encoder.h"
#include "encoder_information.h"
#include "encoder_sensor.h"

settings_info device_setting_read;
settings_info device_setting_write;
encoded_picture_information current_pic;
encoded_picture_information encoded_pic;
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    qDebug()<<"encoder started";
    upload_settings(&device_setting_read);
    ///todo 引导程序
    QThread                     tcp_thread;                 /**< tcp线程 */
    QThread                     udp_thread;                 /**< udp线程 */
    QThread                     v4l2_thread;                /**< v4l2采集线程 */
    QThread                     sensor_thread;              /**< 传感器采集线程 */
    QThread                     settings_thread;            /**< 配置接收线程 */
    QThread                     encoder_thread;             /**< 编码线程 */
    QTimer                      capture_timer;              /**< 采集定时器 */
    video_information           vd_info;                    /**< 采集信息 */
    encoder_settings            set_tcp(NULL,device_setting_read.settings_hostip,5000);
    encoder_tcpsocket           con_tcp(NULL,device_setting_read.settings_hostip,6000);
    encoder_udpsocket           udp;
    encoder_v4l2                v4l2;
    camera_information          cam_info;
    encoder_encoder             encoder;
    encoder_information         information;
    encoder_sensor              sensor;

    cam_info.cam_format = V4L2_PIX_FMT_YUYV;
    cam_info.cam_height = 480;
    cam_info.cam_width  = 640;
    QByteArray vid_name;
    vid_name.append("/dev/video0");
    v4l2.v4l2_check(vid_name.data(),&vd_info);
    v4l2.v4l2_init( vid_name.data(),&vd_info,cam_info);

    encoder.encoder_init(cam_info);
    encoder.encoder_encode_headers(&encoded_pic);

    capture_timer.setInterval(50);

    udp.moveToThread(&udp_thread);
    con_tcp.moveToThread(&tcp_thread);
    set_tcp.moveToThread(&settings_thread);
    v4l2.moveToThread(&v4l2_thread);
    sensor.moveToThread(&sensor_thread);
    encoder.moveToThread(&encoder_thread);
    ///信号与槽连接
    QObject::connect(&capture_timer,SIGNAL(timeout()),&v4l2,SLOT(v4l2_capture()),Qt::DirectConnection);
    QObject::connect(&v4l2,SIGNAL(need_to_encoder()),&encoder,SLOT(encode_one_frame()),Qt::DirectConnection);
    QObject::connect(&encoder,SIGNAL(need_to_merge(encoded_picture_information)),
                     &information,SLOT(get_video_from_encoder(encoded_picture_information)),Qt::DirectConnection);
    QObject::connect(&sensor,SIGNAL(need_to_merge(QByteArray,int)),
                     &information,SLOT(get_infor_from_sensor(QByteArray,int)),Qt::DirectConnection);
    QObject::connect(&information,SIGNAL(send_data_to_udp(QByteArray)),
                     &udp,SLOT(get_data_from_information(QByteArray)),Qt::DirectConnection);
    QObject::connect(&con_tcp,SIGNAL(change_decoder_ip(QString)),
                     &udp,SLOT(change_decoder_address(QString)),Qt::DirectConnection);
    QObject::connect(&con_tcp,SIGNAL(readyRead()),
                     &con_tcp,SLOT(get_data_from_host()));
    QObject::connect(&set_tcp,SIGNAL(readyRead()),
                     &set_tcp,SLOT(get_data_from_host()));

    QObject::connect(&tcp_thread,SIGNAL(started()),&con_tcp,SLOT(connect_to_host()));
    QObject::connect(&settings_thread,SIGNAL(started()),&set_tcp,SLOT(connect_to_host()));
    QObject::connect(&udp_thread,SIGNAL(started()),&udp,SLOT(set_address()));
    ///线程开始
    tcp_thread.start();
    udp_thread.start();
    settings_thread.start();
    sensor_thread.start();
    v4l2_thread.start();
    encoder_thread.start();

    ///定时器开启
    capture_timer.start();
    return a.exec();
}

void upload_settings(settings_info *info)
{
    QSettings ini_file("/settings/device_settings.ini",QSettings::IniFormat);

    info->settings_time    =
            QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss").toLatin1();
    info->settings_addr    =
            ini_file.value("/basic_information/address").toByteArray();
    info->settings_hostip  =
            ini_file.value("/net_information/hostip").toByteArray();
    info->settings_localip =
            ini_file.value("/net_information/localip").toByteArray();
    qDebug()<<"******************************************";
    qDebug()<<"HostIP:" << info->settings_hostip;
}
