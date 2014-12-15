/***************************************************************
Copyright  :邢大天
Author     :邢大天
Date       :2014-12-05
Description:encoder_tcpsocket
            服务器远程控制类
****************************************************************/
#ifndef ENCODER_TCPSOCKET_H
#define ENCODER_TCPSOCKET_H

#include <QTcpSocket>
#include "encoder_package.h"

class encoder_tcpsocket : public QTcpSocket
{
    Q_OBJECT
public:
    explicit encoder_tcpsocket(QObject *parent , QString hostip,int hostport);
signals:
    /*******************************************************************************
    Signals:        void state_changed();
    Description:    decoder设备专属，向UI发送信号影响显示
    Calls:
    Input:
    Output:
    Return:
    Others:
    ********************************************************************************/
    void state_changed();
    void connect_error(int error_no);
    /*******************************************************************************
    Signals:        void change_decoder_ip(QString decoder_ip);
    Description:    encoder设备专属，发送解码器ip
    Calls:
    Input:          QString decoder_ip解码器ip
    Output:
    Return:
    Others:
    ********************************************************************************/
    void change_decoder_ip(QString decoder_ip);
private:
    /*******************************************************************************
    Function:       void send_info_to_host(QString op_code,QString op_data);
    Description:    ACK
    Calls:          操作码
    Input:
    Output:
    Return:
    Others:         返回ACK： 4字节长度 + 操作码 + 操作数
    ********************************************************************************/
    void send_info_to_host(QString op_code,QString op_data);
    /*******************************************************************************
    Function:       void data_parse(QByteArray data);
    Description:    配置信息解析，并写入文件
    Calls:          void send_info_to_host(QString op_code,QString op_data);
    Input:
    Output:
    Return:
    Others:         信息格式为
                    4字节长度 + 4字节操作码 + 操作数
                    操作码：
                    GET_DEVICE_TIME           获取设备时间
                    GET_DEVICE_ADDRESS        获取设备安装地点
                    GET_DEVICE_STATE          获取设备状态
                    GET_DEVICE_HEART          心跳包
                    SET_DEVICE_TIME           设置设备时间
                    SET_DEVICE_DECODERIP      设置解码器IP
                    SET_DEVICE_EXIT           重启设备
                    SET_DEVICE_STATE          设置decoder状态
    ********************************************************************************/
    void data_parse(QByteArray data);
    QByteArray data_from_host;
    QByteArray data_to_host;

private slots:
    /*******************************************************************************
    Function:       （SLOT）void get_data_from_host();
    Description:    获取配置信息
    Calls:          void data_parse(QByteArray data);
    Input:
    Output:
    Return:
    Others:
    ********************************************************************************/
    void get_data_from_host();

};

#endif // ENCODER_TCPSOCKET_H
