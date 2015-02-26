#ifndef ENCODER_INFORMATION_H
#define ENCODER_INFORMATION_H

#include <QObject>
#include "encoder_package.h"
/**
 * @brief 信息打包类，生成结构化视频信息包
 *        邢大天 2014-12-07
 *        邢大天 2015-02-06
 *
 */
class encoder_information : public QObject
{
    Q_OBJECT
public:
    /**
     * @brief
     *
     * @param parent
     */
    explicit encoder_information(QObject *parent = 0);

signals:
    /**
     * @brief 信号
     *
     * @param data 需要udp发送的信息
     */
    void send_data_to_udp(QByteArray data);
private:
    QByteArray data_send; /**< 需要发送的信息 */
    QByteArray data_sensor; /**< 传感器信息 */
    int        num_of_sensor; /**< 传感器数量 */
public slots:

    /**
     * @brief 槽函数，接收编码后视频信息 融合传感器信息形成结构化信息
     *        格式：
     *          6字节包长度+1个字节传感器数量+6字节视频长度+视频信息+4字节传感器信息长度+传感器信息
     *
     * @param encoded_picture_information h.264编码结构体
     */
    void get_video_from_encoder(encoded_picture_information);



    /**
     * @brief 槽函数，接收传感器信息
     *
     * @param sensor_data  传感器信息
     * @param sensor_num   已采集传感器数量
     */
    void get_infor_from_sensor(QByteArray sensor_data,int sensor_num);
};

#endif // ENCODER_INFORMATION_H
