/***************************************************************
Copyright  :邢大天
Author     :邢大天
Date       :2014-12-05
Description:encoder_tcpsocket
            传输类
****************************************************************/
#ifndef ENCODER_UDPSOCKET_H
#define ENCODER_UDPSOCKET_H

#include <QUdpSocket>
#include <QHostAddress>
#include "encoder_package.h"
class encoder_udpsocket : public QUdpSocket
{
    Q_OBJECT
public:
    explicit encoder_udpsocket(QObject *parent = 0);
    QHostAddress address;
signals:

public slots:
    /*******************************************************************************
    Function:       （SLOT）get_data_from_information(QByteArray data);
    Description:    获取information信息并发送
    Calls:          QByteArray data要发送的信息
    Input:
    Output:
    Return:
    Others:
    ********************************************************************************/
    void get_data_from_information(QByteArray data);
};

#endif // ENCODER_UDPSOCKET_H
