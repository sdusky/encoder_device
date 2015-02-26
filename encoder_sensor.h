#ifndef ENCODER_SENSOR_H
#define ENCODER_SENSOR_H

#include <QObject>
#include "encoder_encoder.h"

/**
 * @brief 传感器信息采集类
 *        邢大天 2014-12-06
 *        邢大天 2015-02-07
 *
 */
class encoder_sensor : public QObject
{
    Q_OBJECT
public:

    /**
     * @brief 设置sensors数组信息
     *
     * @param parent
     */
    explicit encoder_sensor(QObject *parent = 0);
    void setNum(int num);
private:

    /**
     * @brief 设置传感器采集速度
     *
     * @param msec 传感器采集速度 毫秒
     */
    void set_send_interval(int msec);

    /**
     * @brief 获取传感器标识长度
     *
     */
    void get_name_length();

    sensor_label  sensors[MAX_SENSOR_NUM]; /**< 传感器数组 */
    int sensor_number;                  /**< 传感器数量 */
    QTimer        sensor_timer;        /**< 传感器信息主动传输定时器 */
signals:
    /**
     * @brief 信号，传递传感器信息及已采集数量
     *
     * @param sensor_data 传感器信息
     * @param sensor_num  传感器数量
     */
    void need_to_merge(QByteArray sensor_data,int sensor_num);
public slots:
    /**
     * @brief 槽函数，定时器触发
     *        传递可融合的传感器信息
     *
     */
    void send_data_to_information();

    /**
     * @brief 槽函数，接收采集到的传感器信息
     *
     * @param sensor_name 传感器标识、名称
     * @param sensor_data 传感器数据
     */
    void set_sensor_data(QByteArray sensor_name,QByteArray sensor_data);

};

#endif // ENCODER_SENSOR_H
