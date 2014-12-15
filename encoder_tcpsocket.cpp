#include "encoder_tcpsocket.h"

encoder_tcpsocket::encoder_tcpsocket(QObject *parent, QString hostip, int hostport)
    :    QTcpSocket(parent)
{
    this->connectToHost(hostip,hostport);
    data_from_host.clear();
    if(this->waitForConnected(30000))
    {
         emit connect_error(ERROR_TCP_CONNECT);
    }
}

void encoder_tcpsocket::get_data_from_host()
{
    data_from_host = this->readAll();
    data_parse(data_from_host);
}

void encoder_tcpsocket::data_parse(QByteArray data)
{
    int length = data.left(4).toInt(NULL,10);
    if(length <= 0)
        return;

    QByteArray code = data.mid(4,4);
    if(code.operator ==(GET_DEVICE_TIME))
    {
        send_info_to_host(GET_DEVICE_TIME,
                          QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
    }
    else if(code.operator ==(GET_DEVICE_ADDRESS))
    {
        send_info_to_host(GET_DEVICE_ADDRESS,
                          device_setting_read.settings_addr);
    }
    else if(code.operator ==(GET_DEVICE_STATE))
    {
        send_info_to_host(GET_DEVICE_STATE,
                          QString::number(device_setting_read.settings_state).toLatin1());
    }
    else if(code.operator ==(GET_DEVICE_HEART))
    {
        send_info_to_host(GET_DEVICE_HEART,
                          "0000");
    }
    else if(code.operator ==(SET_DEVICE_TIME))
    {
        QByteArray current_time = data.mid(8,19);
        char set_time[50];
        sprintf(set_time,"time -s \" %s \"\n",current_time.data());
        system(set_time);
    }
    else if(code.operator ==(SET_DEVICE_EXIT))
    {
        this->close();
        system("reboot\n");
    }
    else if(code.operator ==(SET_DEVICE_DECODERIP))
    {
        emit change_decoder_ip(data.mid(8,length - 8));
    }
}

void encoder_tcpsocket::send_info_to_host(QString op_code, QString op_data)
{
    data_to_host.clear();
    data_to_host.append(op_code);
    data_to_host.append(op_data);

    QByteArray array_length;
    array_length = QString::number(data_to_host.size()).toLatin1();
    while(array_length.size() < 4)
        array_length.insert(0,'0');

    data_to_host.insert(0,array_length);
    this->write(data_to_host);
}
