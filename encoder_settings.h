#ifndef ENCODER_SETTINGS_H
#define ENCODER_SETTINGS_H

#include <QTcpSocket>
#include <QHostAddress>
#include "encoder_package.h"

/**
 * @brief 设备配置类，继承自QTcpSocket类
 *
 *        邢大天 2014-12-06
 *
 */
class encoder_settings : public QTcpSocket
{
    Q_OBJECT
public:

    /**
     * @brief 构造函数，连接配置服务器
     *
     * @param parent
     * @param hostip    服务器IP
     * @param hostport  服务器端口
     */
    explicit encoder_settings(QObject *parent , QString hostip , int hostport);


signals:

    /**
     * @brief 信号，连接错误信息
     *
     * @param error_no 错误号
     */
    void connect_error(int error_no);
public slots:

    /**
     * @brief 槽函数，接收服务器设置信息
     *
     */
    void get_data_from_host();

    /**
     * @brief 槽函数，连接到服务器
     *
     */
    void connect_to_host();
private:

    /**
     * @brief 信息解析函数
     *
     * @param data 要解析的信息
     *        信息格式为
     *              4字节长度 + 4字节操作码 + 操作数
     *              操作码：
     *              SETTING_CODE_ADDRESS       设置安置地点
     *              SETTING_CODE_HOSTIP        设置服务器IP
     *              SETTING_CODE_LOCALIP       设置本机IP
     *              SETTING_CODE_TYPE          设置设备类型(encoder or decoder)
     *              SETTING_CODE_FILENAME      发送文件名
     *              SETTING_CODE_FILEEND       发送文件结束
     *              SETTING_CODE_LOCALMAC      设置本机
     *
     */
    void data_parse(QByteArray data);

    /**
     * @brief 将配置信息写入配置文件
     *
     * @param op_code 配置信息操作码
     * @param op_data 配置信息数据
     */
    void write_settings_to_file(QByteArray op_code,QByteArray op_data);

    QByteArray data_from_host;  /**< 来自服务器的信息 */
    QString    host_ip;         /**< 服务器IP */
    int        host_port;       /**< 服务器端口号 */
};

#endif // ENCODER_SETTINGS_H
