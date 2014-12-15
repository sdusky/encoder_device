/***************************************************************
Copyright  :邢大天
Author     :邢大天
Date       :2014-12-05
Description:encoder_encoder
            通过S5PV210的MFC硬件编码
****************************************************************/
#ifndef ENCODER_ENCODER_H
#define ENCODER_ENCODER_H

#include <QObject>
#include "encoder_package.h"
#include "MFC_API/mfc_interface.h"
#include "MFC_API/SsbSipMfcApi.h"
class encoder_encoder : public QObject
{
    Q_OBJECT
public:
    explicit encoder_encoder(QObject *parent = 0);
    /*******************************************************************************
    Function:       int encoder_init(struct camera_information cam_info);
    Description:    编码器初始化
    Calls:          1.void encoder_param_init(int width, int height);
                    2.void *SsbSipMfcDecOpen(void *value);
                    3.SSBSIP_MFC_ERROR_CODE SsbSipMfcDecInit(void *openHandle, SSBSIP_MFC_CODEC_TYPE codec_type, int Frameleng);
                    4.void *SsbSipMfcDecGetInBuf(void *openHandle, void **phyInBuf, int inputBufferSize);
                    5.SSBSIP_MFC_ERROR_CODE SsbSipMfcDecClose(void *openHandle);
    Input:          struct camera_information cam_info 结构体 定义位于common.h
    Output:         int
    Return:         错误返回 -1   调用SSBSIP_MFC_ERROR_CODE SsbSipMfcDecClose(void *openHandle);关闭MFC
                    正确返回  1
    Others:         步骤：1.encoder参数设置     encoder_param_init
                         2.打开MFC            SsbSipMfcDecOpen
                         3.获取输入缓存         SsbSipMfcDecGetInBuf
                         4.初始化时间戳
    ********************************************************************************/

    int encoder_init(struct camera_information cam_info);
    /*******************************************************************************
    Function:       int encoder_encode_headers(struct encoded_picture_information *headers_info);
    Description:    编码头部
    Calls:          1.SSBSIP_MFC_ERROR_CODE SsbSipMfcEncGetOutBuf(void *openHandle, SSBSIP_MFC_ENC_OUTPUT_INFO *output_info)；
    Input:          struct encoded_picture_information *headers_info 结构体 定义位于common.h
    Output:         int
    Return:         错误返回 -1   关闭MFC SsbSipMfcDecClose
                    正确返回  1
    Others:         步骤：1.获取输出缓存        SsbSipMfcEncGetOutBuf
                         2.设置头部长度
                         3.设置输出虚拟地址
    ********************************************************************************/

    int encoder_encode_headers(struct encoded_picture_information *headers_info);
    /*******************************************************************************
    Function:       int encoder_encode_frame(struct encoded_picture_information raw_pic_info,
                                             struct encoded_picture_information *out_pic_info);
    Description:    编码帧
    Calls:          1.SSBSIP_MFC_ERROR_CODE SsbSipMfcEncSetConfig(void *openHandle, SSBSIP_MFC_ENC_CONF conf_type, void *value);
                    2.void encoder_yuv420_to_nv12(const unsigned char *yuv420,unsigned char *nv12,
                                                  int csize);
                    3.SSBSIP_MFC_ERROR_CODE SsbSipMfcEncSetInBuf(void *openHandle, SSBSIP_MFC_ENC_INPUT_INFO *input_info);
                    4.SSBSIP_MFC_ERROR_CODE SsbSipMfcEncExe(void *openHandle);
                    5.SSBSIP_MFC_ERROR_CODE SsbSipMfcEncGetOutBuf(void *openHandle, SSBSIP_MFC_ENC_OUTPUT_INFO *output_info);
                    6.SSBSIP_MFC_ERROR_CODE SsbSipMfcEncGetConfig(void *openHandle, SSBSIP_MFC_ENC_CONF conf_type, void *value);
    Input:          struct encoded_picture_information raw_pic_info 原始图片 结构体 定义位于common.h
                    struct encoded_picture_information *out_pic_info 编码后图片
    Output:         int
    Return:         错误返回 -1
                    正确返回  1
    Others:         步骤：1.计算当前图像时间戳
                         2.设置编码器变量       SsbSipMfcEncSetConfig
                         3.YUV420转NV12       encoder_yuv420_to_nv12
                         4.设置编码前输入地址    SsbSipMfcEncSetInBuf
                         5.编码视频流           SsbSipMfcEncExe
                         6.获取编码后输出地址    SsbSipMfcEncGetOutBuf
                         7.获取编码器变量        SsbSipMfcEncGetConfig
                         8.设置编码后流的时间戳
                         9.设置编码后帧类型     I P B
    ********************************************************************************/
    int encoder_encode_frame(struct encoded_picture_information *out_pic_info,struct encoded_picture_information raw_pic_info);
private:
    /*******************************************************************************
    Function:       void encoder_param_init(int width, int height);
    Description:    编码器参数初始化
    Calls:          无
    Input:          int width;      图像宽
                    int height;     图像高
    Output:         无
    Return:
    Others:
    ********************************************************************************/
    void encoder_param_init(int width, int height);

    /*******************************************************************************
    Function:       void encoder_yuv420_to_nv12(const unsigned char *yuv420,unsigned char *nv12,
                                int csize);
    Description:    编码帧
    Calls:
    Input:          const unsigned char *yuv420 原始yuv图像
                    unsigned char *nv12         转换后图像
                    int csize                   原始尺寸（h*w/2）
    Output:         无
    Return:
    Others:         YYYY UVUV
    ********************************************************************************/
    void encoder_yuv420_to_nv12(const unsigned char *yuv420,unsigned char *nv12,
                                int csize);

    SSBSIP_MFC_ENC_H264_PARAM     h264_param;
    SSBSIP_MFC_ENC_INPUT_INFO     input_info;
    SSBSIP_MFC_ENC_OUTPUT_INFO    output_info;
    void                          *mfc_handle;
    unsigned long                 start_timestamp;
    int                           y_size;
    int                           c_size;
    encoded_picture_information   *current_pic;   //编码前信息
    encoded_picture_information   encoded_pic;    //编码后信息
signals:
    /*******************************************************************************
    Signals:        void need_to_merge(encoded_picture_information pic_info);
    Description:    向encoder_information类发送信号，触发其槽函数
    Calls:
    Input:          encoded_picture_information pic_info编码后信息
    Output:         无
    Return:
    Others:
    ********************************************************************************/
    void need_to_merge(encoded_picture_information pic_info);
public slots:
    /*******************************************************************************
    Function:       （SLOTS）void encode_one_frame();
    Description:    开始编码帧信息
    Calls:
    Input:
    Output:         无
    Return:
    Others:         调用方式为encoder_v4l2类的need_to_encoder信号
    ********************************************************************************/
    void encode_one_frame();
};

#endif // ENCODER_ENCODER_H
