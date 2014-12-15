encoder_device
==============
程序介绍：
该程序为结构化视频信息系统编码器部分，其功能为采集视频信息和传感器信息，将其融合在一起通过链路一起传递到服务器存储（解码端显示）。
程序分为：
encoder_alarm类，报警设置
encoder_v4l2类，采集摄像头（视频）信息
encoder_tcpsocket类，连接服务器，用于服务器控制
encoder_udpsocket类，传递综合信息
encoder_settings类，接收服务器配置信息
encoder_sensor类，采集传感器信息
encoder_information类，传感器信息和视频信息（编码后）融合生成传递报文
encoder_encoder类，使用S5PV210的MFC进行硬件h.264编码


对象间通过Qt的信号与槽机制进行信息传递和调用，降低了类外部视图过度开放造成的耦合问题，更好的隔离了类的外部视图与内部实现。
