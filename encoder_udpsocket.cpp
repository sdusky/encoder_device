#include "encoder_udpsocket.h"

encoder_udpsocket::encoder_udpsocket(QObject *parent) :
    QUdpSocket(parent)
{
    address.setAddress(device_setting_read.settings_hostip.data());
    qDebug()<<device_setting_read.settings_hostip;
}

void encoder_udpsocket::get_data_from_information(QByteArray data)
{
    this->writeDatagram(data,address,7000);
}
