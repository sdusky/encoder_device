/***************************************************************
Copyright  :邢大天
Author     :邢大天
Date       :2014-12-05
Description:encoder_information
            视频信息与传感器信息打包类，生成结构化视频信息
****************************************************************/
#ifndef ENCODER_INFORMATION_H
#define ENCODER_INFORMATION_H

#include <QObject>
#include "encoder_package.h"
class encoder_information : public QObject
{
    Q_OBJECT
public:
    explicit encoder_information(QObject *parent = 0);

signals:
    /*******************************************************************************
    Signals:        void send_data_to_udp(QByteArray data);
    Description:    向encoder_udpsocket类发送信号，触发其槽函数
    Calls:
    Input:          QByteArray data 需要通过udp发送的信息
    Output:         无
    Return:
    Others:
    ********************************************************************************/
    void send_data_to_udp(QByteArray data);
private:
    QByteArray data_send;
    QByteArray data_sensor;
public slots:
    /*******************************************************************************
    Function:       （SLOTS）void get_video_from_encoder(encoded_picture_information);
    Description:    接收来自encoder_encoder类的图像信息并与encoder_sensor类传来的
                    传感器信息进行融合
    Calls:          发送send_data_to_udp(QByteArray data)信号；
    Input:          encoded_picture_information
    Output:         无
    Return:
    Others:         组成udp数据包其格式为
                    6字节包长度+6字节视频长度+视频信息+4字节传感器信息长度+传感器信息
                    调用方式为encoder_encoder类的need_to_merge信号
    ********************************************************************************/
    void get_video_from_encoder(encoded_picture_information);
    /*******************************************************************************
    Function:       （SLOTS）void get_infor_from_sensor(QByteArray sensor_data);
    Description:    接收来自encoder_sensor类传来的传感器信息
    Calls:
    Input:          QByteArray sensor_data 传感器信息
    Output:         无
    Return:
    Others:         调用方式为encoder_encoder类的need_to_merge信号
    ********************************************************************************/
    void get_infor_from_sensor(QByteArray sensor_data);
};

#endif // ENCODER_INFORMATION_H
