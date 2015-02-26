#include "encoder_udpsocket.h"

encoder_udpsocket::encoder_udpsocket(QObject *parent) :
    QUdpSocket(parent)
{

}

void encoder_udpsocket::get_data_from_information(QByteArray data)
{
    ///向服务器和解码器发送打包信息
    this->writeDatagram(data,host_address,7000);
    this->writeDatagram(data,decoder_address,7000);
}

void encoder_udpsocket::change_decoder_address(QString decoder_ip)
{
    ///设置解码器IP
    decoder_address.setAddress(decoder_ip);
}

void encoder_udpsocket::set_address()
{
    host_address.setAddress(device_setting_read.settings_hostip.data());
    decoder_address.setAddress(QByteArray("127.0.0.1").data());
}
