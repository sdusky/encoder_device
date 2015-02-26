#ifndef ENCODER_UDPSOCKET_H
#define ENCODER_UDPSOCKET_H

#include <QUdpSocket>
#include <QHostAddress>
#include "encoder_package.h"

/**
 * @brief 信息发送类，继承自QUdpSocket
 *        邢大天 2014-12-06
 *
 */
class encoder_udpsocket : public QUdpSocket
{
    Q_OBJECT
public:

    /**
     * @brief 构造函数 设置服务器IP
     *
     * @param parent
     */
    explicit encoder_udpsocket(QObject *parent = 0);

    QHostAddress host_address; /**< 服务器地址 */
    QHostAddress decoder_address; /**< 解码器地址 */

signals:

public slots:

    /**
     * @brief 获取要发送的数据
     *
     * @param data 要发送的数据
     */
    void get_data_from_information(QByteArray data);

    /**
     * @brief 更改解码器IP进行实时显示或停止
     *
     * @param decoder_ip 解码器IP
     */
    void change_decoder_address(QString decoder_ip);

    void set_address();
};

#endif // ENCODER_UDPSOCKET_H
