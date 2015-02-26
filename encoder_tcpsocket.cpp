#include "encoder_tcpsocket.h"

encoder_tcpsocket::encoder_tcpsocket(QObject *parent, QString hostip, int hostport)
    :    QTcpSocket(parent)
{
    host_ip = hostip;
    host_port = hostport;
}

void encoder_tcpsocket::get_data_from_host()
{
    data_from_host = this->readAll();
    if(data_from_host.operator ==("what are you"))
    {
        this->write("i am a encoder");
    }
    else
        data_parse(data_from_host);
}

void encoder_tcpsocket::connect_to_host()
{
    this->connectToHost(host_ip,host_port);
    data_from_host.clear();
    ///若连接不到，则持续连接
    while(!this->waitForConnected(30000))
    {
         this->connectToHost(host_ip,host_port);
    }
}

void encoder_tcpsocket::data_parse(QByteArray data)
{
    qDebug()<<data;
    int length = data.left(4).toInt(NULL,10);
    if(length <= 0)
        return;

    QByteArray code = data.mid(4,4); /**< 操作数 */
    qDebug()<<code;
    ///获取服务器时间
    if(code.operator ==(GET_DEVICE_TIME))
    {
        send_info_to_host(GET_DEVICE_TIME,
                          QDateTime::currentDateTime().toString("yyyy-MM-dd hh:mm:ss"));
    }
    ///获取设备地址
    else if(code.operator ==(GET_DEVICE_ADDRESS))
    {
        send_info_to_host(GET_DEVICE_ADDRESS,
                          device_setting_read.settings_addr);
    }
    ///获取设备状态，一般为解码器
    else if(code.operator ==(GET_DEVICE_STATE))
    {
        send_info_to_host(GET_DEVICE_STATE,
                          QString::number(device_setting_read.settings_state).toLatin1());
    }
    ///获取心跳
    else if(code.operator ==(GET_DEVICE_HEART))
    {
        send_info_to_host(GET_DEVICE_HEART,
                          "0000");
    }
    ///设置设备时间
    else if(code.operator ==(SET_DEVICE_TIME))
    {
        QByteArray current_time = data.mid(8,19);
        char set_time[50];
        sprintf(set_time,"time -s \" %s \"\n",current_time.data());
        system(set_time);
    }
    ///设置设备重启
    else if(code.operator ==(SET_DEVICE_EXIT))
    {
        this->close();
        system("reboot\n");
    }
    ///设置解码器IP
    else if(code.operator ==(SET_DEVICE_DECODERIP))
    {
        emit change_decoder_ip(data.mid(8,length - 8));
    }
    ///设置停止实时显示
    else if(code.operator ==(SET_DEVICE_STOP))
    {
        emit change_decoder_ip("127.0.0.1");
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
