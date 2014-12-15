#include "encoder_settings.h"

encoder_settings::encoder_settings(QObject *parent, QString hostip, int hostport) :
    QTcpSocket(parent)
{
    this->connectToHost(hostip,hostport);
    if(this->waitForConnected(30000))
    {
         emit connect_error(ERROR_SET_CONNECT);
    }
}

void encoder_settings::get_data_from_host()
{
    data_from_host = this->readAll();
    data_parse(data_from_host);
}

void encoder_settings::data_parse(QByteArray data)
{
    int length = data.left(4).toInt(NULL,10);
    if(length <= 0)
        return;

    QByteArray code = data.mid(4,4);
    if(code.operator ==(SETTING_CODE_ADDRESS) ||
            code.operator ==(SETTING_CODE_HOSTIP) ||
            code.operator ==(SETTING_CODE_LOCALIP) ||
            code.operator ==(SETTING_CODE_TYPE))
    {
        write_settings_to_file(code,
                               data.mid(8,length - 8));
    }
    else if(code.operator ==(SETTING_CODE_FILENAME))
    {
        //start receive file
    }
    else if(code.operator ==(SETTING_CODE_FILEEND))
    {
        //stop receive file
    }


}

void encoder_settings::write_settings_to_file(QByteArray op_code, QByteArray op_data)
{
    QSettings settings_file;
    if(op_code.operator ==(SETTING_CODE_ADDRESS))
    {
        settings_file.setValue("/basic_information/address",op_data);
    }
    else if(op_code.operator ==(SETTING_CODE_HOSTIP))
    {
        settings_file.setValue("/net_information/address",op_data);
    }
    else if(op_code.operator ==(SETTING_CODE_LOCALIP))
    {
        settings_file.setValue("/net_information/address",op_data);
    }
    else if(op_code.operator ==(SETTING_CODE_TYPE))
    {
        settings_file.setValue("/basic_information/devce_type",op_data);
    }
    QByteArray data_to_host;
    data_to_host.append(op_code);
    data_to_host.append("0000");
    this->write(data_to_host);
}
