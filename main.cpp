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
int main(int argc, char *argv[])
{
    QCoreApplication a(argc, argv);
    qDebug()<<"encoder started";
    upload_settings(&device_setting_read);
    QThread                     tcp_thread;
    QThread                     udp_thread;
    QThread                     v4l2_thread;
    QThread                     sensor_thread;
    QThread                     settings_thread;
    QThread                     encoder_thread;
    QTimer                      capture_timer;
    video_information           vd_info;
    encoded_picture_information current_pic;
    encoder_settings            set_tcp(NULL,device_setting_read.settings_hostip,5000);
    encoder_tcpsocket           con_tcp(NULL,device_setting_read.settings_hostip,6000);
    encoder_udpsocket           udp;
    encoder_v4l2                v4l2;
    camera_information          cam_info;
    encoder_encoder             encoder;
    encoder_information         information;
    encoder_sensor              sensor;

    cam_info.cam_format = V4L2_PIX_FMT_YUYV;
    cam_info.cam_height = 360;
    cam_info.cam_width  = 480;
    v4l2.v4l2_check("/dev/video0",&vd_info);
    v4l2.v4l2_init("/dev/video0",&vd_info,cam_info,&current_pic);
    encoder.encoder_init(cam_info);

    encoder.encoder_encode_headers(&current_pic);
    capture_timer.setInterval(50);

    udp.moveToThread(&udp_thread);
    con_tcp.moveToThread(&tcp_thread);
    set_tcp.moveToThread(&settings_thread);
    v4l2.moveToThread(&v4l2_thread);
    sensor.moveToThread(&sensor_thread);
    encoder.moveToThread(&encoder_thread);

    QObject::connect(&capture_timer,SIGNAL(timeout()),&v4l2,SLOT(v4l2_capture()),Qt::DirectConnection);
    QObject::connect(&v4l2,SIGNAL(need_to_encoder()),&encoder,SLOT(encode_one_frame()),Qt::DirectConnection);
    QObject::connect(&encoder,SIGNAL(need_to_merge(encoded_picture_information)),
                     &information,SLOT(get_video_from_encoder(encoded_picture_information)),Qt::DirectConnection);
    QObject::connect(&sensor,SIGNAL(need_to_merge(QByteArray)),
                     &information,SLOT(get_infor_from_sensor(QByteArray)),Qt::DirectConnection);
    QObject::connect(&information,SIGNAL(send_data_to_udp(QByteArray)),
                     &udp,SLOT(get_data_from_information(QByteArray)),Qt::DirectConnection);

    tcp_thread.start();
    udp_thread.start();
    settings_thread.start();
    sensor_thread.start();
    v4l2_thread.start();
    encoder_thread.start();

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
