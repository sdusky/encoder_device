#include "encoder_information.h"

encoder_information::encoder_information(QObject *parent) :
    QObject(parent)
{
    num_of_sensor = 0;
    data_send.clear();
    data_sensor.clear();
}

void encoder_information::get_video_from_encoder(encoded_picture_information video_data)
{
    data_send.clear();

    QByteArray data_length = /**< 包总长度*/
            QString::number(video_data.length + data_sensor.size() + 6 + 6 + 4 + 1).toLatin1();

    while(data_length.size() < 6)
        data_length.insert(0,'0');

    QByteArray video_length = QString::number(video_data.length).toLatin1(); /**< 视频信息长度 */
    while(video_length.size() < 6)
        video_length.insert(0,'0');

    QByteArray sensor_length = QString::number(data_sensor.size()).toLatin1(); /**< 传感器信息长度 */
    while(sensor_length.size() < 4)
        sensor_length.insert(0,'0');

    data_send.append(data_length); ///添加长度 6byte

    data_send.append(QString::number(num_of_sensor)); ///添加传感器数量 1byte

    data_send.append(video_length); ///添加视频长度 6byte

    data_send.append((const char *)video_data.buffer,video_data.length); ///添加视频信息

    data_send.append(sensor_length); ///添加传感器信息长度 4byte

    data_send.append(data_sensor);  ///添加传感器信息

    emit send_data_to_udp(data_send);
//    qDebug()<<"PKT_LENGTH:"<<data_length.toInt();
//    qDebug()<<"VID_LENGTH:"<<video_length.toInt();
//    qDebug()<<"SEN_LENGTH:"<<sensor_length.toInt();
    ///用完传感器信息后 传感器信息失效
    data_sensor.clear();
    num_of_sensor = 0;
}

void encoder_information::get_infor_from_sensor(QByteArray sensor_data,int sensor_num)
{
    data_sensor.clear();
    data_sensor = sensor_data;
    num_of_sensor = sensor_num;
}
