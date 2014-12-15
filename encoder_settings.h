/***************************************************************
Copyright  :邢大天
Author     :邢大天
Date       :2014-12-05
Description:encoder_settings
            服务器远程配置类
****************************************************************/
#ifndef ENCODER_SETTINGS_H
#define ENCODER_SETTINGS_H

#include <QTcpSocket>
#include "encoder_package.h"

class encoder_settings : public QTcpSocket
{
    Q_OBJECT
public:
    explicit encoder_settings(QObject *parent , QString hostip , int hostport);

signals:
    void connect_error(int error_no);
public slots:
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
private:
    /*******************************************************************************
    Function:       void data_parse(QByteArray data);
    Description:    配置信息解析，并写入文件
    Calls:          void write_settings_to_file(QByteArray op_code,QByteArray op_data);
    Input:
    Output:
    Return:
    Others:         信息格式为
                    4字节长度 + 4字节操作码 + 操作数
                    操作码：
                    SETTING_CODE_ADDRESS       设置安置地点
                    SETTING_CODE_HOSTIP        设置服务器IP
                    SETTING_CODE_LOCALIP       设置本机IP
                    SETTING_CODE_TYPE          设置设备类型(encoder or decoder)
                    SETTING_CODE_FILENAME      发送文件名
                    SETTING_CODE_FILEEND       发送文件结束
    ********************************************************************************/
    void data_parse(QByteArray data);
    /*******************************************************************************
    Function:       void write_settings_to_file(QByteArray op_code,QByteArray op_data);
    Description:
    Calls:
    Input:          QByteArray op_code 操作码
                    QByteArray op_data 操作数
    Output:
    Return:
    Others:         将配置内容写入配置文件
                    返回ACK： 4字节长度 + 操作码 + 0000
    ********************************************************************************/
    void write_settings_to_file(QByteArray op_code,QByteArray op_data);
    QByteArray data_from_host;
};

#endif // ENCODER_SETTINGS_H
