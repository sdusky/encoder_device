#include "encoder_sensor.h"

encoder_sensor::encoder_sensor(QObject *parent) :
    QObject(parent)
{
    sensors[0].sensor_name = "Tem";
    sensors[0].has_data    = 0;
    sensors[1].sensor_name = "Hum";
    sensors[0].has_data    = 0;
    sensors[2].sensor_name = "Ill";
    sensors[0].has_data    = 0;
    set_send_interval(3000);
    connect(&sensor_timer,SIGNAL(timeout()),this,SLOT(send_data_to_information()));
    sensor_timer.start();
}

void encoder_sensor::set_sensor_data(QByteArray sensor_name, QByteArray sensor_data)
{
    int i;
    for(i = 0; i < SENSOR_NUM; i++)
    {
        if(sensors[i].sensor_name.operator ==(sensor_name))
        {
            sensors[i].sensor_data = sensor_data;
            sensors[i].has_data    = 1;
            break;
        }
    }
}

void encoder_sensor::set_send_interval(int msec)
{
    sensor_timer.setInterval(msec);
}

void encoder_sensor::send_data_to_information()
{
    QByteArray send_data;
    int i,j=0;
    sensors[2].sensor_data = QString::number(32).toLatin1();
    sensors[2].has_data = 1;
    for(i = 0; i < SENSOR_NUM; i++)
    {
        if(sensors[i].has_data == 1)
        {
            send_data.append(sensors[i].sensor_name_length);
            send_data.append(sensors[i].sensor_name);
            QByteArray data_length;
            data_length = QString::number(sensors[i].sensor_data.size()).toLatin1();
            while(data_length.size() < 4)
                data_length.insert(0,'0');
            j += 1;
        }
        sensors[i].has_data = 0;
    }
    send_data.insert(0,QString::number(j));
    qDebug()<<"sensor_data:"<<send_data;
    emit need_to_merge(send_data);

}

void encoder_sensor::get_name_length()
{
    int i;
    for(i = 0; i < SENSOR_NUM; i++)
    {
        sensors[i].sensor_name_length =
                QString::number(sensors[i].sensor_name.size()).toLatin1();
        while(sensors[i].sensor_name_length.size() < 4)
        {
            sensors[i].sensor_name_length.insert(0,'0');
        }
    }
}
