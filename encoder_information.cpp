#include "encoder_information.h"

encoder_information::encoder_information(QObject *parent) :
    QObject(parent)
{
}

void encoder_information::get_video_from_encoder(encoded_picture_information video_data)
{
    qDebug()<<"merge"<<video_data.length;
    data_send.clear();
    QByteArray data_length = QString::number(video_data.length + data_sensor.size() + 6 + 6 + 4).toLatin1();
    while(data_length.size() < 6)
        data_length.insert(0,'0');
    QByteArray video_length = QString::number(video_data.length).toLatin1();
    while(video_length.size() < 6)
        video_length.insert(0,'0');
    QByteArray sensor_length = QString::number(data_sensor.size()).toLatin1();
    while(sensor_length.size() < 4)
        sensor_length.insert(0,'0');
    data_send.append(data_length);
    data_send.append(video_length);
    data_send.append((const char *)video_data.buffer,video_data.length);
    data_send.append(sensor_length);
    data_send.append(data_sensor);

    emit send_data_to_udp(data_send);

    data_sensor.clear();
}

void encoder_information::get_infor_from_sensor(QByteArray sensor_data)
{
    data_sensor.clear();
    data_sensor = sensor_data;
}
