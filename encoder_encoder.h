
#ifndef ENCODER_ENCODER_H
#define ENCODER_ENCODER_H

#include <QObject>
#include "encoder_package.h"
#include "MFC_API/mfc_interface.h"
#include "MFC_API/SsbSipMfcApi.h"

/**
 * @brief 硬件编码类，封装三星MFC_API函数
 *        邢大天 2014-12-06
 *        邢大天 2015-02-01
 *
 */
class encoder_encoder : public QObject
{
    Q_OBJECT
public:
    explicit encoder_encoder(QObject *parent = 0);
    /**
     * @brief 编码器初始化
     * @other 步骤： 1.encoder参数设置     encoder_param_init
     *              2.打开MFC            SsbSipMfcDecOpen
     *              3.获取输入缓存         SsbSipMfcDecGetInBuf
     *              4.初始化时间戳
     * @see         void encoder_param_init(int width, int height);
     * @see         void *SsbSipMfcDecOpen(void *value);
     * @see         SSBSIP_MFC_ERROR_CODE SsbSipMfcDecInit(void *openHandle, SSBSIP_MFC_CODEC_TYPE codec_type, int Frameleng);
     * @see         void *SsbSipMfcDecGetInBuf(void *openHandle, void **phyInBuf, int inputBufferSize);
     * @see         SSBSIP_MFC_ERROR_CODE SsbSipMfcDecClose(void *openHandle);
     * @see         void encoder_param_init(int width, int height);
     * @param cam_info 采集摄像头数据
     * @return int     返回码 成功：1 失败：负数
     */
    int encoder_init(struct camera_information cam_info);

    /**
     * @brief 编码头部，生成sps pps信息
     * @other 步骤：1.获取输出缓存        SsbSipMfcEncGetOutBuf
     *             2.设置头部长度
     *             3.设置输出虚拟地址
     * @see   SSBSIP_MFC_ERROR_CODE SsbSipMfcEncGetOutBuf(void *openHandle, SSBSIP_MFC_ENC_OUTPUT_INFO *output_info)；
     * @param headers_info 要编码的信息
     * @return int         返回码
     */
    int encoder_encode_headers(struct encoded_picture_information *headers_info);

    /**
     * @brief 编码一帧图像
     * @other步骤：       1.计算当前图像时间戳
     *                   2.设置编码器变量       SsbSipMfcEncSetConfig
     *                   3.YUV420转NV12       encoder_yuv420_to_nv12
     *                   4.设置编码前输入地址    SsbSipMfcEncSetInBuf
     *                   5.编码视频流           SsbSipMfcEncExe
     *                   6.获取编码后输出地址    SsbSipMfcEncGetOutBuf
     *                   7.获取编码器变量        SsbSipMfcEncGetConfig
     *                   8.设置编码后流的时间戳
     *                   9.设置编码后帧类型     I P B
     * @see              1.SSBSIP_MFC_ERROR_CODE SsbSipMfcEncSetConfig(void *openHandle, SSBSIP_MFC_ENC_CONF conf_type, void *value);
     * @see              2.void encoder_yuv420_to_nv12(const unsigned char *yuv420,unsigned char *nv12,
     *                                            int csize);
     * @see              3.SSBSIP_MFC_ERROR_CODE SsbSipMfcEncSetInBuf(void *openHandle, SSBSIP_MFC_ENC_INPUT_INFO *input_info);
     * @see              4.SSBSIP_MFC_ERROR_CODE SsbSipMfcEncExe(void *openHandle);
     * @see              5.SSBSIP_MFC_ERROR_CODE SsbSipMfcEncGetOutBuf(void *openHandle, SSBSIP_MFC_ENC_OUTPUT_INFO *output_info);
     * @see              6.SSBSIP_MFC_ERROR_CODE SsbSipMfcEncGetConfig(void *openHandle, SSBSIP_MFC_ENC_CONF conf_type, void *value);
     * @param out_pic_info 编码后图像信息
     * @param raw_pic_info 原始图像信息
     * @return int         返回码
     */
    int encoder_encode_frame(struct encoded_picture_information *out_pic_info,struct encoded_picture_information raw_pic_info);
private:
    /**
     * @brief 编码器参数设置
     *
     * @param width  图像宽
     * @param height 图像高
     */
    void encoder_param_init(int width, int height);

    /**
     * @brief 色彩空间转换（软件转换）
     *
     * @param yuv420 yuv420指针
     * @param nv12   nv12指针
     * @param csize  长度
     */
    void encoder_yuv420_to_nv12(const unsigned char *yuv420,unsigned char *nv12,
                                int csize);

    SSBSIP_MFC_ENC_H264_PARAM     h264_param; /**< h.264参数 */
    SSBSIP_MFC_ENC_INPUT_INFO     input_info; /**< 编码器输入 */
    SSBSIP_MFC_ENC_OUTPUT_INFO    output_info; /**< 编码器输出 */
    void                          *mfc_handle;  /**< 编码器句柄 */
    unsigned long                 start_timestamp; /**< 时间戳 */
    int                           y_size; /**< Ysize */
    int                           c_size; /**< UVsize */

signals:
    /**
     * @brief 信号，需要融合的图像信息（编码后）
     *
     * @param pic_info
     */
    void need_to_merge(encoded_picture_information pic_info);
public slots:

    /**
     * @brief 槽函数，编码图像
     *
     */
    void encode_one_frame();
};

#endif // ENCODER_ENCODER_H
