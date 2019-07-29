#include "qthreadcamera.h"

qthreadcamera::qthreadcamera(QObject *parent) :
    QThread(parent)
{
}
#define CLEAR(x) memset(&(x), 0, sizeof(x))
extern "C" void camera(void);
extern "C" void errno_exit(const char *s);
extern "C"  int xioctl(int fd, int request, void *arg);
extern "C" inline int clip(int value, int min, int max);
extern "C"  void image_save(const void *p, int len);
extern "C"  void process_image(const void *p);
extern "C"  int  read_frame(void);
extern "C"  void run1(void);
extern "C"  void stop_capturing(void);
extern "C"  void start_capturing(void);
extern "C"  void uninit_device(void);
extern "C"  void init_mmap(void);
extern "C"  void init_device(void);
extern "C"  void close_device(void);
extern "C"  void open_device(void);

struct buffer
{
    void *start;
    size_t length;
}; //定义缓冲区结构体
//缓冲区用来临时存放视频的数据
//
static int  enpre =0;
static int  encap =0;
static const char *dev_name; // 设备名称，可根据设备名打开设备
static int fd = -1;
struct buffer *buffers = NULL;
static unsigned int n_buffers = 0;
static int time_in_sec_capture = 5;
static int fbfd = -1;
static struct fb_var_screeninfo vinfo;
static struct fb_fix_screeninfo finfo;
static char *fbp = NULL;
static long screensize = 0;
//

//
void qthreadcamera::setencapfunc(){
    encap = 1;
}
void qthreadcamera::setuncapfunc(){
    encap = 2;
}
void qthreadcamera::setenprefunc(){
    enpre =1;
}
void qthreadcamera::setunprefunc(){
    enpre =0;
}

void qthreadcamera::run()
{
    //connect(this,SIGNAL(setencap()),qc,);
    dev_name = "/dev/video0";

    open_device();

    init_device();

    start_capturing();

    run1();

    stop_capturing();

    uninit_device();

    close_device();

    exit(EXIT_SUCCESS);

    //return 0;
}

void errno_exit(const char *s) // 错误退出，并打印错误信息
{
    fprintf(stderr, "%s error %d, %s\n", s, errno, strerror(errno));
    exit(EXIT_FAILURE);
}

int xioctl(int fd, int request, void *arg) // 根据参数对设备进行控制
{
    int r;
    do
        r = ioctl(fd, request, arg);
    while (-1 == r && EINTR == errno);
    return r;
}
//控制ioctl函数使用不同的参数

inline int clip(int value, int min, int max)
{
    return (value > max ? max : value < min ? min : value);
}
//进行转换时候的函数
void image_save(const void *p, int len)
{
    static int unclose = 1;
    static int count = 0;
    static FILE *file;

    //
    if (encap ==2){
        count=0;
    }
    //
    if (encap ==1&&count == 0)
    {
        printf("file wile open");
        file = fopen("camout.yuv", "wb");
        printf("file is open ");
        //emit();
    }
    if (encap ==1&&count < 500)
    {
        fwrite(p, 1, len, file);
        count++;
    }
    if (encap ==1&&count == 500)
    {
        if (unclose != 0){
         printf("sava completed");
         unclose = fclose(file);
         //encap == 2
        // emit
         //count =0;
        }
    }
    if (encap ==2){ //count != 0;
        if (unclose != 0){
        printf("abort cap");
        unclose = fclose(file);
        //count = 0
        //emit
        }
    }
}
/*
    不管预览在哪里，反正点击开始采集，就执行一系列初始化
    把保存与不保存的功能单独的摘出来   加上500M的功能
    把获得命令行的函数给改了
    不获取命令行了   直接设置设备名称
    不设置具体的时间，时间无限    用一个全局变量的修改行为来响应停止预览
*/
void process_image(const void *p)
{   // 图像处理

    //ConvertYUVToRGB32

    //unsigned
    char *in = (char *)p;
    int width = 320;
    int height = 240;
    int istride = width * 2;
    int x, y, j;
    int y0, u, y1, v, r, g, b, c, d, e;
    long location = 0;
    for (y = 10; y < height + 10; ++y)
    {
        for (j = 0, x = 10; j < width * 2; j += 4, x += 2)
        {
            location = (x + vinfo.xoffset) * (vinfo.bits_per_pixel / 8) +
                (y + vinfo.yoffset) * finfo.line_length;

            y0 = in[j] - 16;
            u = in[j + 1] - 128;
            y1 = in[j + 2] - 16;
            v = in[j + 3] - 128;
            c = y0;
            d = u;
            e = v;

            r = clip((298 * c + 409 * e + 128) >> 8, 0, 255);
            g = clip((298 * c - 100 * d - 208 * e + 128) >> 8, 0, 255);
            b = clip((298 * c + 516 * d + 128) >> 8, 0, 255);

            fbp[location + 0] = b;
            fbp[location + 1] = g;
            fbp[location + 2] = r;
            fbp[location + 3] = 255;

            c = y1;

            r = clip((298 * c + 409 * e + 128) >> 8, 0, 255);
            g = clip((298 * c - 100 * d - 208 * e + 128) >> 8, 0, 255);
            b = clip((298 * c + 516 * d + 128) >> 8, 0, 255);

            fbp[location + 4] = b;
            fbp[location + 5] = g;
            fbp[location + 6] = r;
            fbp[location + 7] = 255;
        }
        in += istride;
    }
}

int read_frame(void) // 对图像帧进行读取
{
    struct v4l2_buffer buf; //这是视频接口V4L2中相关文件定义的，在videodev2.h中
    //unsigned int i;

    CLEAR(buf);
    buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;  // 获取格式
    buf.memory = V4L2_MEMORY_MMAP;//内存映射的地址

    if (-1 == xioctl(fd, VIDIOC_DQBUF, &buf))
    {
        switch (errno)
        {
        case EAGAIN:
            return 0;
        case EIO:

        default:
            errno_exit("VIDIOC_DQBUF");
        }
    }

    assert(buf.index < n_buffers);
    //  printf("v4l2_pix_format->field(%d)\n", buf.field);
    //assert (buf.field ==V4L2_FIELD_NONE);
   if (enpre==1){
    process_image(buffers[buf.index].start);
   }
    if (encap==1||encap==2){
    image_save(buffers[buf.index].start, buffers[buf.index].length);
    }
    if (-1 == xioctl(fd, VIDIOC_QBUF, &buf))
        errno_exit("VIDIOC_QBUF");
    return 1;
}
//将视频帧进行转换 YUVtoRGB32
//将buffers的数据进行保存

void run1(void) //循环采集
{
   // if (encap==0){
   // }
    //unsigned int count;
    int frames;
    frames = 30 * time_in_sec_capture;

    while (frames-- > 0)
    {
        for (;;)
        {
            fd_set fds;
            struct timeval tv; //时间结构体
            int r;
            FD_ZERO(&fds); //清零
            FD_SET(fd, &fds);

            tv.tv_sec = 2;
            tv.tv_usec = 0;

            r = select(fd + 1, &fds, NULL, NULL, &tv); //等待一帧图像采集完成，超时定为2秒

            if (-1 == r)
            {
                if (EINTR == errno)
                    continue;
                errno_exit("select");
            }

            if (0 == r)
            {
                fprintf(stderr, "select timeout\n");
                exit(EXIT_FAILURE);
            }

            if (read_frame()) //读取一帧图像
                break;
            /*再循环*/
        }
      frames++;  //for循环，如果不成功就继续循环但是要有超时的判断
    }
}

void stop_capturing(void) //终止采集
{
    enum v4l2_buf_type type;

    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (-1 == xioctl(fd, VIDIOC_STREAMOFF, &type))
        errno_exit("VIDIOC_STREAMOFF");
}

void start_capturing(void) //开始采集
{
    unsigned int i;
    enum v4l2_buf_type type;

    for (i = 0; i < n_buffers; ++i)
    {
        struct v4l2_buffer buf;
        CLEAR(buf);

        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = i;

        if (-1 == xioctl(fd, VIDIOC_QBUF, &buf))
            errno_exit("VIDIOC_QBUF");
    }
    //缓冲队列

    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    if (-1 == xioctl(fd, VIDIOC_STREAMON, &type))
        errno_exit("VIDIOC_STREAMON");
}

void uninit_device(void) //卸载设备
{
    unsigned int i;

    for (i = 0; i < n_buffers; ++i)
        if (-1 == munmap(buffers[i].start, buffers[i].length))
            errno_exit("munmap");

    if (-1 == munmap(fbp, screensize))
    {
        printf(" Error: framebuffer device munmap() failed.\n");
        exit(EXIT_FAILURE);
    }
    free(buffers);
}

void init_mmap(void) //初始化内存映射
{
    struct v4l2_requestbuffers req;
    //申请帧缓冲

    //mmap framebuffer
    fbp = (char *)mmap(NULL, screensize, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, 0);
    //   int fbpint =fbp;
    //    if (fbpint == -1)
    //    {
    //        printf("Error: failed to map framebuffer device to memory.\n"); //错误：无法映射到存储设备
    //        exit(EXIT_FAILURE);
    //    }
    memset(fbp, 0, screensize);
    CLEAR(req);

    req.count = 4;
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = V4L2_MEMORY_MMAP;

    if (-1 == xioctl(fd, VIDIOC_REQBUFS, &req))
    {
        if (EINVAL == errno)
        {
            fprintf(stderr, "%s does not support memory mapping\n", dev_name); //...不支持内存映射
            exit(EXIT_FAILURE);
        }
        else
        {
            errno_exit("VIDIOC_REQBUFS");
        }
    }

    if (req.count < 4)
    {                                                                    //if (req.count < 2)
        fprintf(stderr, "Insufficient buffer memory on %s\n", dev_name); //缓冲内存不足
        exit(EXIT_FAILURE);
    }

    buffers = (buffer *)calloc(req.count, sizeof(*buffers));
    //c convert to c++

    if (!buffers)
    {
        fprintf(stderr, "Out of memory\n"); //内存不足
        exit(EXIT_FAILURE);
    }
    //将申请到的帧缓冲映射到用户空间,这样就可以直接操作采集到的帧了,而不必去复制
    for (n_buffers = 0; n_buffers < req.count; ++n_buffers)
    {
        struct v4l2_buffer buf;

        CLEAR(buf);

        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = V4L2_MEMORY_MMAP;
        buf.index = n_buffers;

        if (-1 == xioctl(fd, VIDIOC_QUERYBUF, &buf))
            errno_exit("VIDIOC_QUERYBUF");

        buffers[n_buffers].length = buf.length;
        //转换成相对地址:内核-->用户空间.offset:没帧相对及地址的偏移
        buffers[n_buffers].start = mmap(NULL, buf.length, PROT_READ | PROT_WRITE, MAP_SHARED, fd, buf.m.offset);

        if (MAP_FAILED == buffers[n_buffers].start)
            errno_exit("mmap");
    }
}

void init_device(void) //初始化设备
{
    struct v4l2_capability cap;
    struct v4l2_cropcap cropcap;
    struct v4l2_crop crop;
    struct v4l2_format fmt;
    //unsigned int min;

    // Get fixed screen information
    //得到固定屏幕的信息
    if (-1 == xioctl(fbfd, FBIOGET_FSCREENINFO, &finfo))
    {
        printf("Error reading fixed information.\n");
        exit(EXIT_FAILURE);
    }

    // Get variable screen information
    //得到可变屏幕的信息
    if (-1 == xioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo))
    {
        printf("Error reading variable information.\n");
        exit(EXIT_FAILURE);
    }

    screensize = vinfo.xres * vinfo.yres * vinfo.bits_per_pixel / 8;
    //输出可变屏幕和固定屏幕的信息
    printf("vinfo: xoffset:%d  yoffset:%d bits_per_pixel:%d xres:%d yres:%d\n", vinfo.xoffset, vinfo.yoffset, vinfo.bits_per_pixel, vinfo.xres, vinfo.yres);
    //printf("finfo: line_length:%d  screensize:%d\n", finfo.line_length, screensize);

    //获取camere的信息，复制到cap中
    if (-1 == xioctl(fd, VIDIOC_QUERYCAP, &cap))
    {

        if (EINVAL == errno)
        {
            fprintf(stderr, "%s is no V4L2 device\n", dev_name);
            exit(EXIT_FAILURE);
        }
        else
        {
            errno_exit("VIDIOC_QUERYCAP");
        }
    }

    printf("cap.capabilities = %d\n", cap.capabilities);

    if (!(cap.capabilities & V4L2_CAP_VIDEO_CAPTURE))
    {
        fprintf(stderr, "%s is no video capture device\n", dev_name);
        exit(EXIT_FAILURE);
    }

    if (!(cap.capabilities & V4L2_CAP_STREAMING))
    {
        fprintf(stderr, "%s does not support streaming i/o\n", dev_name); //#define V4L2_CAP_STREAMING              0x04000000  /* streaming I/O ioctls 不支持流式输入输出*/
        exit(EXIT_FAILURE);
    }

    CLEAR(cropcap);

    cropcap.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    //VIDIOC_CROPCAP：查询驱动的修剪能力
    if (0 == xioctl(fd, VIDIOC_CROPCAP, &cropcap))
    {
        crop.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        crop.c = cropcap.defrect;

        if (-1 == xioctl(fd, VIDIOC_S_CROP, &crop))
        {
            switch (errno)
            {
            case EINVAL:
                break;
            default:
                break;
            }
        }
    }
    else
    {
    }

    CLEAR(fmt);
    /*采集信号的type width height 帧格式*/
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.width = 320;
    fmt.fmt.pix.height = 240;
    fmt.fmt.pix.pixelformat = V4L2_PIX_FMT_YUYV; //V4L2_PIX_FMT_JPEG;//V4L2_PIX_FMT_YUYV;

    //  fmt.fmt.pix.field = V4L2_FIELD_INTERLACED;

    //VIDIOC_S_FMT：设置当前驱动的频捕获格式
    if (-1 == xioctl(fd, VIDIOC_S_FMT, &fmt))
        errno_exit("VIDIOC_S_FMT");

    init_mmap();
}

void close_device(void) //关闭设备
{
    if (-1 == close(fd))
        errno_exit("close");
    fd = -1;
    close(fbfd);
}

void open_device(void) //打开设备
{
    struct stat st;

    if (-1 == stat(dev_name, &st))
    {
        fprintf(stderr, "Cannot identify '%s': %d, %s\n", dev_name, errno, strerror(errno));
        exit(EXIT_FAILURE); //无法识别
    }

    if (!S_ISCHR(st.st_mode))
    {
        fprintf(stderr, "%s is no device\n", dev_name);
        exit(EXIT_FAILURE);
    }

    //open framebuffer   帧缓冲区
    fbfd = open("/dev/fb0", O_RDWR);
    if (fbfd == -1)
    {
        printf("Error: cannot open framebuffer device.\n");
        exit(EXIT_FAILURE);
    }

    //open camera
    fd = open(dev_name, O_RDWR | O_NONBLOCK, 0);

    if (-1 == fd)
    {
        fprintf(stderr, "Cannot open '%s': %d, %s\n", dev_name, errno, strerror(errno));
        exit(EXIT_FAILURE);
    }
}
