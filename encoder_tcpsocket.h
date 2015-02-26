#ifndef ENCODER_TCPSOCKET_H
#define ENCODER_TCPSOCKET_H

#include <QTcpSocket>
#include "encoder_package.h"

/**
 * @brief 服务器连接类，继承自QTcpSocket
 *        邢大天 2014-12-06
 *
 */
class encoder_tcpsocket : public QTcpSocket
{
    Q_OBJECT
public:
    /**
     * @brief 构造函数，连接服务器
     *
     * @param parent
     * @param hostip 服务器IP
     * @param hostport 服务器端口
     */
    explicit encoder_tcpsocket(QObject *parent , QString hostip,int hostport);


signals:

    /**
     * @brief 信号，连接错误信息
     *
     * @param error_no 错误号
     */
    void connect_error(int error_no);

    /**
     * @brief 设置解码器IP
     *
     * @param decoder_ip 解码器IP
     */
    void change_decoder_ip(QString decoder_ip);
private:

    /**
     * @brief 向服务器发送信息
     *
     * @param op_code 操作码
     * @param op_data 操作数
     */
    void send_info_to_host(QString op_code,QString op_data);

    /**
     * @brief 信息解析函数
     *
     * @param data 要解析的信息
     *              信息格式为
     *              4字节长度 + 4字节操作码 + 操作数
     *              操作码：
     *              GET_DEVICE_TIME           获取设备时间
     *              GET_DEVICE_ADDRESS        获取设备安装地点
     *              GET_DEVICE_STATE          获取设备状态
     *              GET_DEVICE_HEART          心跳包
     *              SET_DEVICE_TIME           设置设备时间
     *              SET_DEVICE_DECODERIP      设置解码器IP
     *              SET_DEVICE_EXIT           重启设备
     *              SET_DEVICE_STATE          设置decoder状态
     */
    void data_parse(QByteArray data);

    QByteArray data_from_host;  /**< 来自服务器的信息 */
    QByteArray data_to_host;    /**< 向服务器传递的信息 */
    QString    host_ip;         /**< 服务器IP */
    int        host_port;       /**< 服务器端口号 */

public slots:

    /**
     * @brief 接收服务信息
     *
     */
    void get_data_from_host();

    /**
     * @brief 槽函数，连接服务器
     *
     */
    void connect_to_host();

};

#endif // ENCODER_TCPSOCKET_H
