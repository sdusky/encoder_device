#include "encoder_sensor.h"

encoder_sensor::encoder_sensor(QObject *parent) :
    QObject(parent)
{
    setNum(3);
    sensors[0].sensor_name = "Tem";
    sensors[0].has_data    = 0;
    sensors[1].sensor_name = "Hum";
    sensors[0].has_data    = 0;
    sensors[2].sensor_name = "Ill";
    sensors[0].has_data    = 0;

    get_name_length();
    set_send_interval(3000);
    connect(&sensor_timer,SIGNAL(timeout()),this,SLOT(send_data_to_information()));
    sensor_timer.start();
}

void encoder_sensor::setNum(int num)
{
    sensor_number = num;
}

void encoder_sensor::set_sensor_data(QByteArray sensor_name, QByteArray sensor_data)
{
    int i;
    for(i = 0; i < sensor_number; i++)
    {
        if(sensors[i].sensor_name.operator ==(sensor_name))
        {
            sensors[i].sensor_data.clear();
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
    for(i = 0; i < sensor_number; i++)
    {
        if(sensors[i].has_data == 1)
        {
            send_data.append(sensors[i].sensor_name_length); ///添加传感器名称长度 4byte

            send_data.append(sensors[i].sensor_name);        ///添加传感器名称
            QByteArray data_length;                          /**< 信息长度  */

            data_length = QString::number(sensors[i].sensor_data.size()).toLatin1();
            while(data_length.size() < 4)
                data_length.insert(0,'0');

            send_data.append(data_length);                   ///添加信息长度 4byte
            send_data.append(sensors[i].sensor_data);        ///添加传感器信息
            j += 1;
        }
        ///传感器信息用完失效
        sensors[i].has_data = 0;
    }

    emit need_to_merge(send_data,j);

}

void encoder_sensor::get_name_length()
{
    int i;
    for(i = 0; i < sensor_number; i++)
    {
        sensors[i].sensor_name_length =
                QString::number(sensors[i].sensor_name.size()).toLatin1();
        while(sensors[i].sensor_name_length.size() < 4)
        {
            sensors[i].sensor_name_length.insert(0,'0');
        }
    }
}
