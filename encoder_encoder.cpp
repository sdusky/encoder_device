#include "encoder_encoder.h"

encoder_encoder::encoder_encoder(QObject *parent) :
    QObject(parent)
{

}

int encoder_encoder::encoder_init(struct camera_information cam_info)
{
    SSBIP_MFC_BUFFER_TYPE buf_type = CACHE;

    encoder_param_init(cam_info.cam_width, cam_info.cam_height);
    y_size = cam_info.cam_width * cam_info.cam_height;
    c_size = y_size / 2;
    mfc_handle = SsbSipMfcEncOpen(&buf_type);
    if(!mfc_handle){
        fprintf(stderr, "SsbSipMfcEncOpen failed\n");
        return 0;
    }
    if(SsbSipMfcEncInit(mfc_handle, &h264_param) != MFC_RET_OK){
        fprintf(stderr, "SsbSipMfcEncInit failed\n");
        goto error;
    }

    if (SsbSipMfcEncGetInBuf(mfc_handle, &input_info) != MFC_RET_OK) {
        fprintf(stderr, "SsbSipMfcEncGetInBuf failed\n");
        goto error;
    }
    printf("input_info.YVirAddr=0x%08x, input_info.CVirAddr=0x%08x\n",
        (unsigned int)input_info.YVirAddr, (unsigned int)input_info.CVirAddr);

    start_timestamp = -1;
    return 1;

error:
    SsbSipMfcEncClose(mfc_handle);
    return -1;
}

void encoder_encoder::encoder_param_init(int width, int height)
{
    h264_param.codecType                = H264_ENC;                 //编码格式
    h264_param.SourceWidth              = width;                    //原始图像宽度 (64~1920)
    h264_param.SourceHeight             = height;                   //原始图像高度 (64~1080)
    h264_param.IDRPeriod                = 100;                      //I帧间隔   0:all frame is P frame 1:all frame is I frame 2：IPIP 3：IPPIPPIPP
    h264_param.SliceMode                = 0;                        //分片参数  0：disabled  1：固定宏块数量 2：固定字节数量 3：可变宏块数量
    h264_param.RandomIntraMBRefresh     = 0;                        //循环帧内刷新 0 ~  MBCnt(一副图像中的宏块数目)
    h264_param.EnableFRMRateControl     = 1;                        //基于帧的码率控制使能 0:disable 1 :enable
    h264_param.Bitrate                  = 512000;                   //码率 1 ~

    h264_param.FrameQp                  = 15;                       //帧量化参数（1 ~ 51）
    h264_param.FrameQp_P                = 15;                       //P帧量化参数（1 ~ 51）
    h264_param.FrameQp_B                = 20;                       //B帧量化参数（1 ~ 51）

    h264_param.QSCodeMax                = 30;                       //最大量化值（1 ~ 51）
    h264_param.QSCodeMin                = 10;                       //最小量化值（1 ~ 51）

    h264_param.CBRPeriodRf              = 120;                      //码率控制反应系数 CBR（固定码率）:2 ~ 10  VBR（动态码率）:100~1000

    h264_param.PadControlOn             = 0;                        //填充控制使能 0:使用边界像素   1:使用下面设置值
    h264_param.LumaPadVal               = 0;                        //Y亮度象素值用于填充填充区域   0~255
    h264_param.CbPadVal                 = 0;                        //Cb蓝色像素值用于填充填充区域  0~255
    h264_param.CrPadVal                 = 0;                        //Cr红色像素值用于填充填充区域  0~255

    h264_param.ProfileIDC               = 1;                        //视频压缩特性描述 0:H264 Main Profile    主流画质 支持I/P/B帧 支持Progressive 和 Interlaced 支持CAVLC和CABAC
                                                                    //              1：H264 High Profile    高级画质 在main的基础上增加了8*8内部预测、自定义量化、无损视频编码和更多的YUV格式
                                                                    //              2：H264 Baseline Profile 基本画质 支持I/P帧 支持Progressive和CAVLC
    h264_param.LevelIDC                 = 22;                       //视频本身特性描述 1（level1.0）～40（level4.0）
    h264_param.FrameRate                = DEFAULT_FPS * 1000;       //帧率 1～
    h264_param.SliceArgument            = 0;                        //片宏块数或字节数 MB:1~MBcnt-1 byte:1900~
    h264_param.NumberBFrames            = 0;                        //连续插入B帧数量 0（not used）～ 2
    h264_param.NumberReferenceFrames    = 2;                        //参考图像数量 1 ～ 2
    h264_param.NumberRefForPframes      = 2;                        //用于编码P帧的参考帧数量 1 ～ 2
    // h264_param.LoopFilterDisable     = 0;                        //
    // h264_param.LoopFilterAlphaC0Offset = 2;                      //
    // h264_param.LoopFilterBetaOffset    = 1;
    h264_param.LoopFilterDisable        = 1;                        //环路滤波器 1: Loop Filter Disable, 0: Filter Enable
    h264_param.LoopFilterAlphaC0Offset  = 0;                        //Alpha & C0 Offset for H264 loop filter -6 ~ +6
    h264_param.LoopFilterBetaOffset     = 0;                        //Beta offset for H264 loop filter -6 ~ +6
    h264_param.SymbolMode               = 1;                        //熵编码模式：0: CAVLC基于上下文的自适应变长编码  1: CABAC前文参考之适应性二元算数编码
    h264_param.PictureInterlace         = 0;                        //0:frame picture帧图像 1:field picture场图像
    h264_param.Transform8x8Mode         = 1;                        //帧内预测块选择       0:disable 4*4 1:enable 8*8
    h264_param.EnableMBRateControl      = 0;                        //宏块码率控制使能     0: Disable, 1:MB level RC
    h264_param.DarkDisable              = 0;                        //暗区自适应码率控制   0:enable 1:disable
    h264_param.SmoothDisable            = 0;                        //平滑区自适应码率控制 0:enable 1:disable
    h264_param.StaticDisable            = 0;                        //静态区域自适应码率控制 0:enable 1:disable
    h264_param.ActivityDisable          = 0;                        //活跃区域自适应码率控制 0:enable 1:disable
    // h264_param.DarkDisable     = 1;
    // h264_param.SmoothDisable   = 1;
    // h264_param.StaticDisable   = 1;
    // h264_param.ActivityDisable = 1;
    h264_param.FrameMap                 = NV12_LINEAR;              //
}

int encoder_encoder::encoder_encode_headers(encoded_picture_information *headers_info)
{
    current_pic = headers_info;
    if (SsbSipMfcEncGetOutBuf(mfc_handle, &output_info) != MFC_RET_OK) {
        fprintf(stderr, "SsbSipMfcEncGetOutBuf failed\n");
        return -1;
    }

    printf("output_info.OutputVirAddr=0x%08x, output_info.HeaderSize=%d\n",
        (uint32_t)output_info.StrmVirAddr, (uint32_t)output_info.headerSize);

    headers_info->length = output_info.headerSize;
    headers_info->buffer = (unsigned char*)output_info.StrmVirAddr;

    return 1;
}

int encoder_encoder::encoder_encode_frame(encoded_picture_information *out_pic_info,encoded_picture_information raw_pic_info)
{
    int64_t timestamp;
    int timestamp_int;
    timestamp = raw_pic_info.timestamp.tv_sec*1000000ull+raw_pic_info.timestamp.tv_usec;
    if(start_timestamp == (unsigned long)-1){
        start_timestamp = timestamp;
    }

    timestamp_int = (timestamp-start_timestamp)/1000; // us -> ms
    if (SsbSipMfcEncSetConfig(mfc_handle, MFC_ENC_SETCONF_FRAME_TAG, &(timestamp_int)) != MFC_RET_OK){
        fprintf(stderr, "SsbSipMfcEncSetConfig failed\n");
        return -1;
    }
    memcpy(input_info.YVirAddr, raw_pic_info.buffer, y_size);
    encoder_yuv420_to_nv12(raw_pic_info.buffer+y_size, (unsigned char *)input_info.CVirAddr, c_size);

    if (SsbSipMfcEncSetInBuf(mfc_handle, &input_info) != MFC_RET_OK) {
        fprintf(stderr, "SsbSipMfcEncSetInBuf failed\n");
        return -1;
    }

    if (SsbSipMfcEncExe(mfc_handle) != MFC_RET_OK) {
        fprintf(stderr, "SsbSipMfcEncExe failed\n");
        return -1;
    }

    if (SsbSipMfcEncGetOutBuf(mfc_handle, &output_info) != MFC_RET_OK) {
        fprintf(stderr, "SsbSipMfcEncGetOutBuf failed\n");
        return -1;
    }

    out_pic_info->buffer = (unsigned char *)output_info.StrmVirAddr;
    out_pic_info->length = output_info.dataSize;

    if (SsbSipMfcEncGetConfig(mfc_handle, MFC_ENC_GETCONF_FRAME_TAG, &timestamp_int) != MFC_RET_OK){
        fprintf(stderr, "SsbSipMfcEncGetConfig failed\n");
        return -1;
    }
    out_pic_info->timepoint = timestamp_int*1000ll;

    if(output_info.frameType == MFC_FRAME_TYPE_P_FRAME)
    {
        out_pic_info->frame_type = FRAME_TYPE_P;
    }
    else if(output_info.frameType == MFC_FRAME_TYPE_I_FRAME)
    {
        out_pic_info->frame_type = FRAME_TYPE_I;
    }
    else
    {
        out_pic_info->frame_type = FRAME_TYPE_B;
    }

    return 1;
}

void encoder_encoder::encoder_yuv420_to_nv12(const unsigned char *yuv420, unsigned char *nv12, \
                                     int csize)
{
    int c_half= csize/2;
    const unsigned char *pcb = yuv420;
    const unsigned char *pcr = yuv420 + c_half;
    int j;
    for(j = 0; j < c_half; j++)
    {
        *nv12 = *pcb;
        nv12++;
        *nv12 = *pcr;
        nv12++;

        pcr++;
        pcb++;
    }
}

void encoder_encoder::encode_one_frame()
{
    encoder_encode_frame(&encoded_pic,*current_pic);
    emit need_to_merge(encoded_pic);
}
