#include "encoder_settings.h"

encoder_settings::encoder_settings(QObject *parent, QString hostip, int hostport) :
    QTcpSocket(parent)
{
    host_ip = hostip;
    host_port = hostport;
}

void encoder_settings::connect_to_host()
{
    ///若连接不到，则持续连接
    this->connectToHost("192.168.1.2",5000);
    while(!this->waitForConnected(30000))
    {
         this->connectToHost("192.168.1.2",5000);
        qDebug()<<"Trying";
    }
    qDebug()<<"Connected";
}

void encoder_settings::get_data_from_host()
{
    data_from_host = this->readAll(); ///接收来自服务器信息
    data_parse(data_from_host);
}

void encoder_settings::data_parse(QByteArray data)
{
    int length = data.left(4).toInt(NULL,10);
    qDebug()<<data;
    if(length <= 0)
        return;

    QByteArray code = data.mid(4,4); ///判断配置操作码
    qDebug()<<code;
    ///若配置地点、服务器IP、自身IP等信息，写入文件
    if(code.operator ==(SETTING_CODE_ADDRESS) ||
            code.operator ==(SETTING_CODE_HOSTIP) ||
            code.operator ==(SETTING_CODE_LOCALIP) ||
            code.operator ==(SETTING_CODE_TYPE) ||
            code.operator ==(SETTING_CODE_LOCALMAC))
    {
        write_settings_to_file(code,
                               data.mid(8,length - 8));
    }
    ///接收来自服务器的文件
    else if(code.operator ==(SETTING_CODE_FILENAME))
    {
        ///TODO
    }
    else if(code.operator ==(SETTING_CODE_FILEEND))
    {
        ///TODO
    }
    else if(code.operator ==(SETTING_CODE_RESET))
    {
        QByteArray data_to_host;
        data_to_host.append(SETTING_CODE_RESET);
        data_to_host.append("0000");
        this->write(data_to_host);
        this->close();
        system("reboot");
    }


}

void encoder_settings::write_settings_to_file(QByteArray op_code, QByteArray op_data)
{
    QSettings settings_file("/settings/device_settings.ini",QSettings::IniFormat);
    if(op_code.operator ==(SETTING_CODE_ADDRESS))
    {
        settings_file.setValue("/basic_information/address",op_data);
    }
    else if(op_code.operator ==(SETTING_CODE_HOSTIP))
    {
        settings_file.setValue("/net_information/hostip",op_data);
    }
    else if(op_code.operator ==(SETTING_CODE_LOCALIP))
    {
        settings_file.setValue("/net_information/localip",op_data);
    }
    else if(op_code.operator ==(SETTING_CODE_TYPE))
    {
        settings_file.setValue("/basic_information/devce_type",op_data);
    }
    else if(op_code.operator ==(SETTING_CODE_LOCALMAC))
    {
        settings_file.setValue("/net_information/localmac",op_data);
    }
    ///写入信息正确，返回正确信息，操作数为0000
    QByteArray data_to_host;
    data_to_host.append(op_code);
    data_to_host.append("0000");
    this->write(data_to_host);
}
