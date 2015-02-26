#include "encoder_v4l2.h"

encoder_v4l2::encoder_v4l2(QObject *parent) :
    QObject(parent)
{
}

int encoder_v4l2::v4l2_check(char *dev_name, struct video_information *vd_info)
{
    vd_info->fd = open(dev_name,O_RDWR,0);
    if(vd_info->fd < 0)
    {
        perror("Class V4L2: Open device error!");
        return -1;
    }
    if(v4l2_get_capability(vd_info) < 0)
    {
        perror("Class V4L2: Get capability error!");
        return -1;
    }
    if(v4l2_get_format(vd_info) < 0)
    {
        perror("Class V4L2: Get format error!");
        return -1;
    }

    vd_info->frame_buffer = NULL;
    v4l2_close(vd_info);
    return 1;
}

int encoder_v4l2::v4l2_init(char *dev_name, struct video_information *vd_info, camera_information cam_info)
{
    video_info = vd_info;
    camera_info.cam_format = cam_info.cam_format;
    camera_info.cam_height = cam_info.cam_height;
    camera_info.cam_width  = cam_info.cam_width;
    current_pic.buffer = NULL;
    current_pic.height = cam_info.cam_height;
    current_pic.width = cam_info.cam_width;
    current_pic.buffer = (unsigned char *)malloc(
                cam_info.cam_height * cam_info.cam_width * 3 / 2);

    if(current_pic.buffer != NULL)
        qDebug()<<cam_info.cam_height<<"," << cam_info.cam_width;

    int i;
    ///initialize the given data*
    vd_info->format = cam_info.cam_format;
    vd_info->width = cam_info.cam_width;
    vd_info->height = cam_info.cam_height;
    vd_info->is_quit = 1;
    vd_info->frame_size_in = (vd_info->width * vd_info->height << 1);   //这个參数不清楚

    ///分配内存,因为YUYV是一种原始数据,可以直接显示,不需要编解码,而mjpeg格式的,需要解码,所以要分配两个缓冲区*/
    switch(vd_info->format)
    {
    case V4L2_PIX_FMT_MJPEG:
        vd_info->tmp_buffer = (unsigned char *)calloc(1,(size_t)vd_info->frame_size_in);
        if(vd_info->tmp_buffer == NULL)
        {
            perror("Class V4L2: calloc tmp_buffer:");
            return -1;
        }
        vd_info->frame_buffer = (unsigned char *)calloc(1,(size_t)vd_info->width * (vd_info->height + 8)*2);//??这里分配的大小不懂
        if(vd_info->frame_buffer == NULL)
        {
            perror("Class V4L2: calloc frame_buffer_mjpeg:");
            return -1;
        }
        break;
    case V4L2_PIX_FMT_YUYV:
        /*vd_info->frame_buffer = (unsigned char *)calloc(1,(size_t)vd_info->frame_size_in);
        if(vd_info->frame_buffer == NULL)
        {
            perror("Class V4L2: calloc frame_buffer_yuyv");
            return -1;
        }*/
        ///分配原始帧所需内存
        vd_info->tmp_buffer = (unsigned char *)calloc(1,(size_t)vd_info->frame_size_in);
        if(vd_info->tmp_buffer == NULL)
        {
            perror("Class V4L2: calloc tmp_buffer:");
            return -1;
        }
        break;
    default:
        return -1;
        break;
    }
    ///打开,以阻塞方式打开.

    vd_info->fd = open(dev_name,O_RDWR,0);
    if(vd_info->fd < 0)
    {
        perror("Class V4L2: Open device error!");
        return -1;
    }
    //
    /*VIDIOC_QUERYCAP.It is used to identify kernel devices compatible with this specification
    and to obtain information about driver and hardware capabilities.The ioctl takes a pointer
    to a struct v4l2_capability which is filled by the driver*/
    if(ioctl(vd_info->fd,VIDIOC_QUERYCAP,&vd_info->cap))
    {
        perror("Class V4L2: query camera failed:");
        return -1;
    }
    if((vd_info->cap.capabilities) & (V4L2_CAP_VIDEO_CAPTURE == 0))
    {
        perror("Class V4L2: video capture not supported:");
        return -1;
    }
    ///set format
    v4l2_set_format(vd_info,cam_info);
    ///set param
    v4l2_set_streamparam(vd_info);
    ///request buffers
    memset(&vd_info->rb,0,sizeof(struct v4l2_requestbuffers));
    vd_info->rb.count = NB_BUFFER;
    vd_info->rb.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    vd_info->rb.memory = V4L2_MEMORY_MMAP;
    ///VIDIOC_REQBUFS：获取内存
    if(-1 == ioctl(vd_info->fd,VIDIOC_REQBUFS,&vd_info->rb))
    {
        perror("Class V4L2: request_buffer:");
        return -1;
    }
    ///map the buffers(4 buffer)映射到用戶空间内存
    for(i = 0;i< NB_BUFFER;i++)
    {
        memset(&vd_info->buf,0,sizeof(struct v4l2_buffer));
        vd_info->buf.index = i;
        vd_info->buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        vd_info->buf.memory = V4L2_MEMORY_MMAP;
        /*mmap()系统调用使得进程之间通过映射同一个普通文件实现共享内存。普通文件
        被映射到进程地址空间后，进程可以像访问普通内存一样对文件进行访问，不必在调
        用read()，write()等操作。两个不同进程A、B共享内存的意思是，同一块物理内
        存被映射到进程A、B各自的进程地址空间。进程A可以即时访问进程B对共享内存中
        数据的更新，反之亦然。*/
        /*VIDIOC_QUERYBUF：把VIDIOC_REQBUFS中分配的数据缓存转换成物理地址*/
        if(-1 == ioctl(vd_info->fd,VIDIOC_QUERYBUF,&vd_info->buf))
        {
            perror("Class V4L2: QUERYBUF:");
            return -1;
        }
        /*map it,0 means anywhere把摄像头图像数据映射到进程内存中，
        也就是只要使用vd->mem指针就可以使用采集到的图像数据*/
        vd_info->mem[i] = mmap(0,vd_info->buf.length,PROT_READ,MAP_SHARED,
                               vd_info->fd,vd_info->buf.m.offset);
        /*MAP_FAILED */
        if(MAP_FAILED == vd_info->mem[i])
        {
            perror("Class V4L2: mmap:");
            return -1;
        }
    }
    ///queue the buffers 进入队列
    for(i = 0;i < NB_BUFFER;i++)
    {
        memset(&vd_info->buf,0,sizeof(struct v4l2_buffer));
        vd_info->buf.index = i;
        vd_info->buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        vd_info->buf.memory = V4L2_MEMORY_MMAP;
        /*把数据从缓存中读取出来*/
        if(-1 == ioctl(vd_info->fd,VIDIOC_QBUF,&vd_info->buf))
        {
            perror("Class V4L2: queue buffer:");
            return -1;
        }
    }
    v4l2_on(vd_info);
    return 1;
}

int encoder_v4l2::v4l2_grab()
{
#define HEADFRAME1 0xaf
    //static int count = 0;

    if(!video_info->is_streaming)  //is stream is off,start it
    {
        if(v4l2_on(video_info) != 0)   //failed
        {
            goto err;
        }
    }

    memset(&video_info->buf,0,sizeof(struct v4l2_buffer));
    video_info->buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    video_info->buf.memory = V4L2_MEMORY_MMAP;
    /*get data from buffers*/
    /*出队列以取得已采集数据的帧缓冲，取得原始采集数据。VIDIOC_DQBUF*/
    if(-1 == ioctl(video_info->fd,VIDIOC_DQBUF,&video_info->buf))
    {
        perror("Class V4L2: get data from buffers:");
        goto err;
    }

    if(video_info->buf.bytesused <= HEADFRAME1)
    {
        return 1;
    }

    memcpy(video_info->tmp_buffer,video_info->mem[video_info->buf.index],video_info->buf.bytesused);
    if(-1 == ioctl(video_info->fd,VIDIOC_QBUF,&video_info->buf))
    {
        perror("Class V4L2: queue buffer again:");
        goto err;
    }

    return 1;
err:
    video_info->is_quit = 0;
    return -1;
}

int encoder_v4l2::v4l2_close(struct video_information *vd_info)
{
    if(vd_info->fd < 0)
    {
        return -1;
    }
    else
    {
        close(vd_info->fd);
        return 1;
    }
}

int encoder_v4l2::v4l2_get_capability(video_information *vd_info)
{
    memset(&vd_info->cap,0,sizeof(struct v4l2_capability));
    if(ioctl(vd_info->fd,VIDIOC_QUERYCAP,&(vd_info->cap)))
    {
        return -1;
    }
    printf("***********************V4L2 Capability**********************\n");
    printf("Driver Name   :%s\n",vd_info->cap.driver);
    printf("Card   Name   :%s\n",vd_info->cap.card);
    printf("Bus    info   :%s\n",vd_info->cap.bus_info);
    printf("Driver Version:%u.%u.%u\n",(vd_info->cap.version>>16)&0XFF, (vd_info->cap.version>>8)&0XFF,vd_info->cap.version&0XFF);
    return 1;
}

int encoder_v4l2::v4l2_get_format(struct video_information *vd_info)
{
    struct v4l2_fmtdesc fmtdesc;
    fmtdesc.index=0;
    fmtdesc.type=V4L2_BUF_TYPE_VIDEO_CAPTURE;
    printf("***********************V4L2 Format*************************\n");
    while(ioctl(vd_info->fd, VIDIOC_ENUM_FMT, &fmtdesc) != -1)
    {
        printf("\t%d.%s\n",fmtdesc.index+1,fmtdesc.description);
        fmtdesc.index++;
    }

    memset(&vd_info->fmt,0,sizeof(struct v4l2_format));
    vd_info->fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if(ioctl(vd_info->fd,VIDIOC_G_FMT,&(vd_info->fmt)))
    {
        perror("get_format:");
        return -1;
    }
    vd_info->format = vd_info->fmt.fmt.pix.pixelformat;

    vd_info->width = vd_info->fmt.fmt.pix.width;
    vd_info->height = vd_info->fmt.fmt.pix.height;
    vd_info->field = (v4l2_field)vd_info->fmt.fmt.pix.field;
    vd_info->bytes_per_line = vd_info->fmt.fmt.pix.bytesperline;
    vd_info->size_image = vd_info->fmt.fmt.pix.sizeimage;
    vd_info->color_space = (v4l2_colorspace)vd_info->fmt.fmt.pix.colorspace;
    vd_info->priv = vd_info->fmt.fmt.pix.priv;

    return 1;
}

int encoder_v4l2::v4l2_set_format(struct video_information *vd_info, camera_information cam_info)
{
    memset(&vd_info->fmt,0,sizeof(struct v4l2_format));
    vd_info->fmt.type                   = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    vd_info->fmt.fmt.pix.width          = cam_info.cam_width;
    vd_info->fmt.fmt.pix.height         = cam_info.cam_height;
    vd_info->fmt.fmt.pix.pixelformat    = cam_info.cam_format;
    vd_info->fmt.fmt.pix.field          = V4L2_FIELD_NONE;

    v4l2_std_id std;
    int ret;
    do
    {
        ret=ioctl(vd_info->fd,VIDIOC_QUERYSTD,&std);

    }while(ret==-1 && errno==EAGAIN);

    /*设置视频帧格式.*/
    if(ioctl(vd_info->fd,VIDIOC_S_FMT,&(vd_info->fmt)))
    {
        perror("Class V4L2: set_format\n");
        return -1;
    }
    return 1;
}

int encoder_v4l2::v4l2_on(struct video_information *vd_info)
{
    vd_info->type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    ///VIDIOC_STREAMON 开始视频的采集*
    if(-1 == ioctl(vd_info->fd,VIDIOC_STREAMON,&vd_info->type))
    {
        perror("Class V4L2: v4l2_on\n");
        return -1;
    }
    vd_info->is_streaming = 1;
    cout<<"Class V4L2: Start streaming!"<<endl;
    return 1;
}

int encoder_v4l2::v4l2_off(video_information *vd_info)
{
    vd_info->type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    ///结束视频显示函数  VIDIOC_STREAMOFF
    if(-1 == ioctl(vd_info->fd,VIDIOC_STREAMOFF,&vd_info->type))
    {
        perror("Class V4L2: v4l2_off:");
        return -1;
    }
    vd_info->is_streaming = 0;
    cout<<"Class V4L2: Stop streaming!"<<endl;
    return 1;
}

int encoder_v4l2::v4l2_set_streamparam(struct video_information *vd_info)
{
    memset(&vd_info->parm,0,sizeof(struct v4l2_streamparm));
    vd_info->parm.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    vd_info->parm.parm.capture.timeperframe.denominator = 30;
    vd_info->parm.parm.capture.timeperframe.numerator = 1;
    vd_info->parm.parm.output.timeperframe.denominator = 15;
    vd_info->parm.parm.output.timeperframe.numerator = 1;
    if(-1 == ioctl(vd_info->fd,VIDIOC_S_PARM,&vd_info->parm))
    {
        perror("Class V4L2: set param\n");
        return -1;
    }
    printf("the frame rate:%d\n",vd_info->parm.parm.output.timeperframe.denominator);
    return 1;
}

void encoder_v4l2::v4l2_yuv422_to_rgb888(unsigned char *yuv422, unsigned char *rgb888, struct camera_information cam_info)
{
    unsigned int in, out = 0;
    unsigned int pixel_16;
    unsigned char pixel_24[3];
    unsigned int pixel32;
    int y0, u, y1, v;
    for(in = 0; in < cam_info.cam_width * cam_info.cam_height * 2; in += 4) {
        pixel_16 =  yuv422[in + 3] << 24 |  yuv422[in + 2] << 16 |  yuv422[in + 1] <<  8 |  yuv422[in + 0];
        y0 = (pixel_16 & 0x000000ff);
        u  = (pixel_16 & 0x0000ff00) >>  8;
        y1 = (pixel_16 & 0x00ff0000) >> 16;
        v  = (pixel_16 & 0xff000000) >> 24;
        pixel32 = v4l2_yuv422_to_rgb888_pixel(y0, u, v);
        pixel_24[0] = (pixel32 & 0x000000ff);
        pixel_24[1] = (pixel32 & 0x0000ff00) >> 8;
        pixel_24[2] = (pixel32 & 0x00ff0000) >> 16;
        rgb888[out++] = pixel_24[0];
        rgb888[out++] = pixel_24[1];
        rgb888[out++] = pixel_24[2];
        pixel32 = v4l2_yuv422_to_rgb888_pixel(y1, u, v);
        pixel_24[0] = (pixel32 & 0x000000ff);
        pixel_24[1] = (pixel32 & 0x0000ff00) >> 8;
        pixel_24[2] = (pixel32 & 0x00ff0000) >> 16;
        rgb888[out++] = pixel_24[0];
        rgb888[out++] = pixel_24[1];
        rgb888[out++] = pixel_24[2];
    }
}

int encoder_v4l2::v4l2_yuv422_to_rgb888_pixel(int y, int u, int v)
{
    unsigned int pixel32 = 0;
    unsigned char *pixel = (unsigned char*)&pixel32;
    int r, g, b;
    r = y + ((1437289 * (v - 128)) >> 20);
    g = y - (((731907  * (v - 128)) + (354034 * (u - 128))) >> 20);
    b = y + ((1816601 * (u - 128)) >> 20);
    if(r > 255) r = 255;
    if(g > 255) g = 255;
    if(b > 255) b = 255;
    if(r < 0) r = 0;
    if(g < 0) g = 0;
    if(b < 0) b = 0;
    pixel[0] = (r * 220) >> 8;
    pixel[1] = (g * 220) >> 8;
    pixel[2] = (b * 220) >> 8;
    return pixel32;
}

void encoder_v4l2::v4l2_yuv422_to_yuv420(unsigned char *yuv422, unsigned char *yuv420, struct camera_information cam_info)
{
    int i,j;
    unsigned char *pY = yuv420;
    unsigned char *pU = yuv420 + cam_info.cam_width * cam_info.cam_height;
    unsigned char *pV = pU + (cam_info.cam_width * cam_info.cam_height) / 4;

    unsigned char *pYUVTemp = yuv422;
    unsigned char *pYUVTempNext = yuv422 + cam_info.cam_width * 2;

    for(i = 0; i < (int)cam_info.cam_height; i+=2)
    {
        for(j = 0; j < (int)cam_info.cam_width; j+=2)
        {

            pY[j] = *pYUVTemp++;

            pY[j + cam_info.cam_width] = *pYUVTempNext++;
            //qDebug()<<"Convert i:"<<i<<",j:"<<j;
            pU[j/2] =(*(pYUVTemp) + *(pYUVTempNext)) / 2;

            pYUVTemp++;
            pYUVTempNext++;

            pY[j + 1] = *pYUVTemp++;
            pY[j + 1 + cam_info.cam_width] = *pYUVTempNext++;

            pV[j / 2] =(*(pYUVTemp) + *(pYUVTempNext)) / 2;
            pYUVTemp++;
            pYUVTempNext++;
         }
         pYUVTemp+=cam_info.cam_width * 2;
         pYUVTempNext+=cam_info.cam_width * 2;
         pY+=cam_info.cam_width * 2;
         pU+=cam_info.cam_width / 2;
         pV+=cam_info.cam_width / 2;
    }
}

void encoder_v4l2::v4l2_capture()
{
    if( v4l2_grab() == 1)
    { 
        v4l2_yuv422_to_yuv420(video_info->tmp_buffer,current_pic.buffer,camera_info);
        current_pic.timestamp.tv_sec = video_info->buf.timestamp.tv_sec;
        current_pic.timestamp.tv_usec = video_info->buf.timestamp.tv_usec;

        emit need_to_encoder();
    }
}
