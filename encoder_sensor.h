/***************************************************************
Copyright  :邢大天
Author     :邢大天
Date       :2014-12-05
Description:encoder_sensor
            传感器信息采集类
****************************************************************/

#ifndef ENCODER_SENSOR_H
#define ENCODER_SENSOR_H

#include <QObject>
#include "encoder_encoder.h"

class encoder_sensor : public QObject
{
    Q_OBJECT
public:
    explicit encoder_sensor(QObject *parent = 0);

private:
    /*******************************************************************************
    Function:       void set_send_interval(int msec);
    Description:    设置传感器信息传递时间
    Calls:
    Input:          int msec 设置毫秒
    Output:
    Return:
    Others:         调用QTimer类成员函数
    ********************************************************************************/
    void set_send_interval(int msec);
    /*******************************************************************************
    Function:       void set_send_interval();
    Description:    获取各传感器标示长度
    Calls:
    Input:
    Output:
    Return:
    Others:
    ********************************************************************************/
    void get_name_length();
    sensor_label  sensors[SENSOR_NUM];       //传感器数量定义在encoder_package.h
    QTimer        sensor_timer;              //传感器采集定时器
signals:
    /*******************************************************************************
    Signals:        void need_to_merge(QByteArray sensor_data);
    Description:    向encoder_information类发送信号，触发其槽函数
    Calls:
    Input:          QByteArray sensor_data 需要融合的传感器信息
    Output:         无
    Return:
    Others:
    ********************************************************************************/
    void need_to_merge(QByteArray sensor_data);
public slots:
    /*******************************************************************************
    Function:       （SLOT）void send_data_to_information();
    Description:    将传感器信息发送给information
    Calls:
    Input:
    Output:
    Return:
    Others:         检查各传感器的更新标示若为1则组成数据包后通过信号发送给
                    encoder_information类进行叠加
                    其内容格式为：
                    1个字节传感器数量 + 4字节name_length + name + 4字节数据长度 + 数据 + ...
    ********************************************************************************/
    void send_data_to_information();
    /*******************************************************************************
    Function:       （SLOT）void set_sensor_data(QByteArray sensor_name,QByteArray sensor_data);
    Description:    传感器信息采集
    Calls:
    Input:          QByteArray sensor_name 传感器名称
                    QByteArray sensor_data 传感器信息
    Output:
    Return:
    Others:         采集后将更新标示置为1
    ********************************************************************************/
    void set_sensor_data(QByteArray sensor_name,QByteArray sensor_data);
};

#endif // ENCODER_SENSOR_H
