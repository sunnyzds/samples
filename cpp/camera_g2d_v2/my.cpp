/*
* Copyright (c) 2012 Freescale Semiconductor, Inc.
*/

/****************************************************************************
*
*    Copyright 2012 Vivante Corporation, Sunnyvale, California.
*    All Rights Reserved.
*
*    Permission is hereby granted, free of charge, to any person obtaining
*    a copy of this software and associated documentation files (the
*    'Software'), to deal in the Software without restriction, including
*    without limitation the rights to use, copy, modify, merge, publish,
*    distribute, sub license, and/or sell copies of the Software, and to
*    permit persons to whom the Software is furnished to do so, subject
*    to the following conditions:
*
*    The above copyright notice and this permission notice (including the
*    next paragraph) shall be included in all copies or substantial
*    portions of the Software.
*
*    THE SOFTWARE IS PROVIDED 'AS IS', WITHOUT WARRANTY OF ANY KIND,
*    EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
*    MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NON-INFRINGEMENT.
*    IN NO EVENT SHALL VIVANTE AND/OR ITS SUPPLIERS BE LIABLE FOR ANY
*    CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
*    TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
*    SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*
*****************************************************************************/


/*
* 
*
* Draws a simple triangle with basic vertex and pixel shaders.
*/
#ifndef GL_GLEXT_PROTOTYPES
#define GL_GLEXT_PROTOTYPES 1
#endif

#include <EGL/egl.h>
#include <GLES2/gl2.h>
#include <GLES2/gl2ext.h>
/*#include "FSL/fsl_egl.h"*/

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

#include <assert.h>
#include <math.h>
#include <signal.h>
#include <time.h>
#include <string.h>

#include <sys/time.h>
#include <errno.h>

#include <pthread.h>
#include <semaphore.h>


#include <unistd.h>
#include <sys/ioctl.h>
#include <stdint.h>
#include <asm/types.h>
#include <linux/videodev2.h>
#include <sys/mman.h>
#include <malloc.h>
#include "g2d.h"
#include "mxcfb.h"

#define TFAIL -1
#define TPASS 0
#define  CAMERA_NUM  1
#define G2D_CACHEABLE    0

#define Timespec_Double(t) ((double)((t)->tv_sec) + (1.e-9 * (double)((t)->tv_nsec)))

#define Timespec_Sub(r, a, b) \
  do { \
    if ((a)->tv_nsec < (b)->tv_nsec) { \
      (r)->tv_nsec = 1000000000 + (a)->tv_nsec - (b)->tv_nsec; \
      (r)->tv_sec = (a)->tv_sec - (b)->tv_sec - 1; \
    } else { \
      (r)->tv_nsec = (a)->tv_nsec - (b)->tv_nsec; \
      (r)->tv_sec  = (a)->tv_sec  - (b)->tv_sec; \
    } \
  } while (0)

void report_fps();
    int fd_fb0 = 0;
    unsigned short * fb0;
    int g_fb0_size;
    int g_fb0_phys;




int GST_COMMAND_TYPE[4] = { GL_VIV_UYVY,  GL_VIV_UYVY, GL_VIV_UYVY, GL_VIV_UYVY};
char fb_display_dev[100] = "/dev/fb0";
char fb_display_bg_dev[100] = "/dev/fb";

struct v4l2_mxc_offset {
    uint32_t u_offset;
    uint32_t v_offset;
};

#define TEST_BUFFER_NUM 4

struct videobuffer
{
        unsigned char *start;
        size_t offset;
        unsigned int length;
     int filled;    // 1: filled with camera data, 0:camera data has been glTexDirectMapped
//   int mapped; // 1: already direct mapped, 0:not directly mapped texture
     int invalid;  // 1: already swapped, 0:not swappedto texture
};

struct g2d_buf  *g2d_buffers[4][TEST_BUFFER_NUM];
struct g2d_buf  *g2d_buffers_usrp[TEST_BUFFER_NUM];


void *g2dHandle;

struct videobuffer buffers[4][TEST_BUFFER_NUM];
int g_in_width [4]={640, 1280, 1280, 1280};
int g_in_height[4] ={480, 8000, 8000, 800};
int g_out_width[4] = {640, 480, 480,480};
int g_out_height[4] = {480, 270, 270, 270};
int g_input = 0;
int g_in_fmt = V4L2_PIX_FMT_UYVY;
//int g_in_fmt = V4L2_PIX_FMT_YUYV;

//int g_out_width = 640;
//int g_out_height = 480;
int g_cap_fmt = V4L2_PIX_FMT_YUYV;
int g_capture_mode = 0;
int g_timeout = 10;
int g_camera_framerate = 30;    /* 30 fps */
int g_loop = 0;
int g_mem_type = V4L2_MEMORY_USERPTR;  //V4L2_MEMORY_MMAP;
int g_frame_size;
char g_v4l_device[100] = "/dev/video0";


int g_capture_num_buffers=TEST_BUFFER_NUM;

//char g_v4l_device[4][100] ={ "/dev/video0", "/dev/video1", "/dev/video2", "/dev/video3"};
int g_fill_buffer_inx[4] = {-1,-1,-1,-1};

int g_g2d_frame_size[4];
int g_g2d_fmt[4]={G2D_UYVY, G2D_UYVY, G2D_UYVY, G2D_UYVY} ;
int  dst_fmt = G2D_RGBA8888;
int g_display_num_buffers =1;


 int fd_v4l[4]={-1,-1, -1, -1};
 static struct v4l2_buffer capture_buf[4][TEST_BUFFER_NUM];

unsigned char * g_fb_display = NULL; 
int fd_fb_display = 0;
int g_display_base_phy;;
int g_display_size;
int g_display_fg = 1;
int g_display_id = 1;
int g_display_top = 0;
int g_display_left = 0;
int g_display_width = 640;
int g_display_height = 480;
struct fb_var_screeninfo g_screen_info;



volatile sig_atomic_t quit = 0;
int frames = 0;
int new_buffer = 0;
static int renderFrames=0;

/* GL_VIV_direct_texture */
#ifndef GL_VIV_direct_texture
#define GL_VIV_YV12                     0x8FC0
#define GL_VIV_NV12                     0x8FC1
#define GL_VIV_YUY2                     0x8FC2
#define GL_VIV_UYVY                     0x8FC3
#define GL_VIV_NV21                     0x8FC4
#endif

typedef void (GL_APIENTRY *PFNGLTEXDIRECTVIVMAP) (GLenum Target, GLsizei Width, GLsizei Height, GLenum Format, GLvoid ** Pixels, const GLuint * Physical);
typedef void (GL_APIENTRY *PFNGLTEXDIRECTINVALIDATEVIV) (GLenum Target);
static PFNGLTEXDIRECTVIVMAP pFNglTexDirectVIVMap = NULL;
static PFNGLTEXDIRECTINVALIDATEVIV pFNglTexDirectInvalidateVIV = NULL;




pthread_mutex_t gst_th_mutex[4] = {PTHREAD_MUTEX_INITIALIZER,PTHREAD_MUTEX_INITIALIZER,PTHREAD_MUTEX_INITIALIZER, PTHREAD_MUTEX_INITIALIZER};
pthread_cond_t gstCON = PTHREAD_COND_INITIALIZER;

static int exit_flag[4] = {0,0,0,0};

//thread functions
static void* GstLoop1(void* thArg); 
static void* GstLoop2(void* thArg); 
static void* GstLoop3(void* thArg); 
static void* GstLoop4(void* thArg); 


static void print_pixelformat(char *prefix, int val)
{
    printf("%s: %c%c%c%c\n", prefix ? prefix : "pixelformat",
                    val & 0xff,
                    (val >> 8) & 0xff,
                    (val >> 16) & 0xff,
                    (val >> 24) & 0xff);
}


static void* GstLoop1(void* thArg)
{
    int i = 0;
    int camera = 0;
    while (!exit_flag[0]) {
    
    if(g_fill_buffer_inx[camera] != -1)
    {
         i = g_fill_buffer_inx[camera]+1;
         if(i  == TEST_BUFFER_NUM) i = 0;
     }
    else i = 0;
     pthread_mutex_lock(&gst_th_mutex[camera]); 
      buffers[camera][i].filled = 0;
      buffers[camera][i].invalid = 0;        
     pthread_mutex_unlock(&gst_th_mutex[camera]);   
    if(i != TEST_BUFFER_NUM){
        memset(&capture_buf[camera][i], 0, sizeof(capture_buf[camera][i]));  
        capture_buf[camera][i].type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        capture_buf[camera][i].memory = V4L2_MEMORY_MMAP;
        if (ioctl (fd_v4l[camera], VIDIOC_DQBUF, &capture_buf[camera][i]) < 0)  {
              printf("VIDIOC_DQBUF failed for camera 1.\n");
            continue;
         }
        
         pthread_mutex_lock(&gst_th_mutex[camera]);
         buffers[camera][i].filled = 1;
         g_fill_buffer_inx[camera] =i;      
        //pthread_cond_signal(&gstCON);
        pthread_mutex_unlock(&gst_th_mutex[camera]);
        //printf(" data from v4l2 camera 1 %d\n", capture_buf[i].index);
     }
      
         {
            {                   
              if (ioctl (fd_v4l[camera], VIDIOC_QBUF, &capture_buf[camera][i]) < 0) {
                                printf("VIDIOC_QBUF failed  buff %d for camera %d\n", i, camera);
                                                       
            }
            //  printf("QBUF for camera 0 %d\n", i);

              }
          }
   
    
 

    }
    return 0;
}


//camera set up
int start_capturing(int fd_v4l)
{
    unsigned int i;
    struct v4l2_buffer buf;
    enum v4l2_buf_type type;
    struct v4l2_requestbuffers req;
    int camera=0;

    memset(&req, 0, sizeof (req));
    req.count = TEST_BUFFER_NUM;
    req.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    req.memory = g_mem_type;

    if (ioctl(fd_v4l, VIDIOC_REQBUFS, &req) < 0) {
        printf("VIDIOC_REQBUFS failed\n");
        return -1;
    }

    if (g_mem_type == V4L2_MEMORY_MMAP) {
        for (i = 0; i < TEST_BUFFER_NUM; i++) {
            memset(&buf, 0, sizeof (buf));
            buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
            buf.memory = g_mem_type;
            buf.index = i;

            if (ioctl(fd_v4l, VIDIOC_QUERYBUF, &buf) < 0) {
                printf("VIDIOC_QUERYBUF error\n");
                return -1;
            }

            buffers[camera][i].length = buf.length;
            buffers[camera][i].offset = (size_t) buf.m.offset;
            buffers[camera][i].start = (unsigned char*)mmap(NULL, buffers[camera][i].length,
                PROT_READ | PROT_WRITE, MAP_SHARED,
                fd_v4l, buffers[camera][i].offset);
            memset(buffers[camera][i].start, 0xFF, buffers[camera][i].length);
        }
    }

    for (i = 0; i < TEST_BUFFER_NUM; i++)
    {
        memset(&buf, 0, sizeof (buf));
        buf.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
        buf.memory = g_mem_type;
        buf.index = i;
        buf.length = buffers[camera][i].length;
        if (g_mem_type == V4L2_MEMORY_USERPTR)
            buf.m.userptr = (unsigned long) buffers[camera][i].start;
        else
            buf.m.offset = buffers[camera][i].offset;

        if (ioctl (fd_v4l, VIDIOC_QBUF, &buf) < 0) {
            printf("VIDIOC_QBUF error\n");
            return -1;
        }
    }

    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (ioctl (fd_v4l, VIDIOC_STREAMON, &type) < 0) {
        printf("VIDIOC_STREAMON error\n");
        return -1;
    }

    return 0;
}

int stop_capturing(int fd_v4l)
{
    enum v4l2_buf_type type;

    type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    return ioctl (fd_v4l, VIDIOC_STREAMOFF, &type);
}

static int open_video_device(void)
{
    struct v4l2_capability cap;
    int fd_v4l;

    if ((fd_v4l = open(g_v4l_device, O_RDWR, 0)) < 0) {
        printf("unable to open %s for capture device.\n", g_v4l_device);
    }
    if (ioctl(fd_v4l, VIDIOC_QUERYCAP, &cap) == 0) {
        if (cap.capabilities & V4L2_CAP_VIDEO_CAPTURE) {
            printf("Found v4l2 capture device %s.\n", g_v4l_device);
            return fd_v4l;
        }
    } else
        close(fd_v4l);

    return fd_v4l;
}

int memalloc(int buf_size, int buf_cnt)
{
       int camera = 0;
       int i;
        struct g2d_buf *buf ;     
    for (i = 0; i < buf_cnt; i++) {
          g2d_buffers_usrp[i] = g2d_alloc(g_g2d_frame_size[camera], 0);//alloc physical contiguous memory for source image data
         if(!g2d_buffers_usrp[i]) {
             printf("Fail to allocate output physical memory for image buffer!\n");
             return 1;
              }

        buf= g2d_buffers_usrp[i];    

        buffers[camera][i].length =  buf_size;
        
        buffers[camera][i].offset =buf->buf_paddr;
        buffers[camera][i].start = (unsigned char *)(buf->buf_vaddr);

     //   v4l2_info("%s, buf_size=0x%x\n", __func__, buf_size);
       // v4l2_info("USRP: alloc bufs va=%p, pa=0x%x, size %d\n",
       //         buffers[i].start, buffers[i].offset, buf_size);
    }

    return 0;
}

int v4l_capture_setup(void)
{
    struct v4l2_format fmt;
    struct v4l2_streamparm parm;
    struct v4l2_fmtdesc fmtdesc;
    struct v4l2_frmsizeenum frmsize;
    int fd_v4l = 0;
         int camera = 0;
    if ((fd_v4l = open_video_device()) < 0)
    {
        printf("Unable to open v4l2 capture device.\n");
        return -1;
    }

    parm.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    parm.parm.capture.capturemode = g_capture_mode;
    parm.parm.capture.timeperframe.denominator = g_camera_framerate;
    parm.parm.capture.timeperframe.numerator = 1;
    if (ioctl(fd_v4l, VIDIOC_S_PARM, &parm) < 0)
    {
        printf("VIDIOC_S_PARM failed\n");
        goto fail;
    }

    fmtdesc.index = 0;
    fmtdesc.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;

    if (ioctl(fd_v4l, VIDIOC_ENUM_FMT, &fmtdesc) < 0) {
        printf("VIDIOC ENUM FMT failed \n");
        goto fail;
    }
    print_pixelformat("pixelformat (output by camera)",
            fmtdesc.pixelformat);
    g_cap_fmt = fmtdesc.pixelformat;

    frmsize.pixel_format = fmtdesc.pixelformat;
    frmsize.index = g_capture_mode;
    if (ioctl(fd_v4l, VIDIOC_ENUM_FRAMESIZES, &frmsize) < 0) {
        printf("get capture mode %d framesize failed\n", g_capture_mode);
        goto fail;
    }

    g_in_width[camera] = frmsize.discrete.width;
    g_in_height[camera] = frmsize.discrete.height;
    printf("g_in_width = %d, g_in_height = %d.\r\n", g_in_width[camera], g_in_height[camera]);

         switch (g_cap_fmt) {
             case V4L2_PIX_FMT_RGB565:
                 g_frame_size = g_in_width[camera] * g_in_height[camera] * 2;
                 g_g2d_fmt[camera] = G2D_RGB565;
                 break;
         
             case V4L2_PIX_FMT_UYVY:
                 g_frame_size = g_in_width[camera] * g_in_height[camera] * 2;
                 g_g2d_fmt[camera] = G2D_UYVY;
                 break;
         
             case V4L2_PIX_FMT_YUYV:
                 g_frame_size = g_in_width[camera] * g_in_height[camera] * 2;
                 g_g2d_fmt[camera] = G2D_YUYV;
                 break;
         
             case V4L2_PIX_FMT_YUV420:
                 g_frame_size = g_in_width[camera] * g_in_height[camera] * 3 / 2;
                 g_g2d_fmt[camera] = G2D_I420;
                 break;
         
             case V4L2_PIX_FMT_NV12:
                 g_frame_size = g_in_width [camera]* g_in_height[camera] * 3 / 2;
                 g_g2d_fmt[camera] = G2D_NV12;
                 break;
         
             default:
                 printf("Unsupported format.\n");
                 return TFAIL;
         }

     g_g2d_frame_size[camera] = g_out_height[camera]*g_out_width[camera]*4;
    memset(&fmt, 0, sizeof(fmt));
    fmt.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    fmt.fmt.pix.pixelformat = g_cap_fmt;
    fmt.fmt.pix.width = g_in_width[camera] ;
    fmt.fmt.pix.height = g_in_height[camera];
    if (ioctl(fd_v4l, VIDIOC_S_FMT, &fmt) < 0)
    {
        printf("set format failed\n");
        goto fail;
    }

    if (ioctl(fd_v4l, VIDIOC_G_FMT, &fmt) < 0)
    {
        printf("get format failed\n");
        goto fail;
    }

    memset(&parm, 0, sizeof(parm));
    parm.type = V4L2_BUF_TYPE_VIDEO_CAPTURE;
    if (ioctl(fd_v4l, VIDIOC_G_PARM, &parm) < 0)
    {
        printf("VIDIOC_G_PARM failed\n");
        parm.parm.capture.timeperframe.denominator = g_camera_framerate;
    }

  printf("\t WxH@fps = %dx%d@%d", fmt.fmt.pix.width,
          fmt.fmt.pix.height, parm.parm.capture.timeperframe.denominator);
  printf("\t Image size = %d\n", fmt.fmt.pix.sizeimage);

    g_frame_size = fmt.fmt.pix.sizeimage;

    return fd_v4l;

fail:
    close(fd_v4l);
    return -1;
}




/*This is the output buffer from g2d, g2d can convert the frame from 4 camera capture to any format, size and rotation you want
   just set the dst_format,  and g_out_with and g_out_height */
 int prepare_g2d_buffers(void)
{
    int i;
    int camera;

  if (g2d_open(&g2dHandle) == -1 || g2dHandle == NULL) {
      printf("Fail to open g2d device!\n");
     
      return  1;
  }

  for (camera=0; camera<CAMERA_NUM; camera++)
 {
    for (i = 0; i < g_capture_num_buffers; i++) {
#if G2D_CACHEABLE
        g2d_buffers[camera][i] = g2d_alloc(g_frame_size , 1);//alloc physical contiguous memory for source image data with cacheable attribute
#else
        g2d_buffers[camera][i] = g2d_alloc(g_frame_size , 0);//alloc physical contiguous memory for source image data
#endif
        if(!g2d_buffers[camera][i]) {
            printf("Fail to allocate physical memory for image buffer!\n");
            return TFAIL;
        }
    }
 }
    return 0;
}


static void map_frame_to_g2d_buffer( int camera, int capture_buf_index)
{
    int i = capture_buf_index;
    int width = g_out_width[camera];
    int height = g_out_height[camera];
    struct g2d_buf *buf = g2d_buffers[camera][i];
    struct g2d_surface src, dst;
   // void *g2dHandle;

#if G2D_CACHEABLE
        g2d_cache_op(buf, G2D_CACHE_FLUSH);
#endif

int phy;
phy = *( (int*)(&(buffers[camera][i].start)));

   // catpured frame is UYVY
    src.planes[0] = buffers[camera][i].offset;
    src.left = 0;
    src.top = 0;
    src.right =g_in_width[camera];
    src.bottom = g_in_height[camera];
    src.stride = g_in_width[camera];
    src.width = g_in_width[camera];
    src.height = g_in_height[camera];
    src.rot =  G2D_ROTATION_0;
    src.format = (g2d_format)(g_g2d_fmt[camera]);



    dst.format = (g2d_format)dst_fmt;
;
    switch (dst.format) {
    case G2D_RGB565:
    case G2D_RGBA8888:
    case G2D_RGBX8888:
    case G2D_BGRA8888:
    case G2D_BGRX8888:
    case G2D_BGR565:
    case G2D_YUYV:
    case G2D_UYVY:
        dst.planes[0] = buf->buf_paddr;
        break;
    case G2D_NV12:
        dst.planes[0] = buf->buf_paddr;
        dst.planes[1] = buf->buf_paddr + width * height;
        break;
    case G2D_I420:
        dst.planes[0] = buf->buf_paddr;
        dst.planes[1] = buf->buf_paddr + width * height;
        dst.planes[2] = dst.planes[1]  + width * height / 4;
        break;
    case G2D_YV12:
        dst.planes[0] = buf->buf_paddr;
        dst.planes[2] = buf->buf_paddr + width * height;
        dst.planes[1] = dst.planes[2]  + width * height / 4;
        break;
    case G2D_NV16:
        dst.planes[0] = buf->buf_paddr;
        dst.planes[1] = buf->buf_paddr + width * height;
                break;
    default:
        printf("Unsupport image format in the example code\n");
        return;
    }

    dst.left = 0;
    dst.top = 0;
    dst.right = width;
    dst.bottom = height;
    dst.stride = width;
    dst.width  = width;
    dst.height = height;
    dst.rot  = G2D_ROTATION_0;

    g2d_blit(g2dHandle, &src, &dst);
    g2d_finish(g2dHandle);

   // g2d_close(g2dHandle);
   

}

bool RenderInit()
{
    int camera = 0;
    int s_prod = 0;  
    int i = 0;
    pthread_t gst_data_process_th1;
   

 

     for (camera=0; camera<CAMERA_NUM; camera++)
    {
       if (start_capturing(fd_v4l[camera]) < 0)
        {
               printf("start_capturing failed %d\n", camera);
               return false;
               
        }
    }
     if(CAMERA_NUM >0)pthread_create(&gst_data_process_th1, NULL, GstLoop1, NULL);
   
   return true;
}
static void draw_image_to_framebuffer(struct g2d_buf *buf, int img_width, int img_height, int img_format, 
         struct fb_var_screeninfo *screen_info, int left, int top, int to_width, int to_height, int set_alpha, int rotation)
{
    int i;
    struct g2d_surface src, dst;
   // void *g2dHandle;

    if (((left+to_width) > (int)screen_info->xres) || ((top+to_height) > (int)screen_info->yres)) {
        printf("Bad display image dimensions!\n");
        return;
    }

#if G2D_CACHEABLE
        g2d_cache_op(buf, G2D_CACHE_FLUSH);
#endif


/*
    NOTE: in this example, all the test image data meet with the alignment requirement.
    Thus, in your code, you need to pay attention on that.

    Pixel buffer address alignment requirement,
    RGB/BGR:  pixel data in planes [0] with 16bytes alignment,
    NV12/NV16:  Y in planes [0], UV in planes [1], with 64bytes alignment,
    I420:    Y in planes [0], U in planes [1], V in planes [2], with 64 bytes alignment,
    YV12:  Y in planes [0], V in planes [1], U in planes [2], with 64 bytes alignment,
    NV21/NV61:  Y in planes [0], VU in planes [1], with 64bytes alignment,
    YUYV/YVYU/UYVY/VYUY:  in planes[0], buffer address is with 16bytes alignment.
*/

    src.format = (g2d_format)img_format;
    switch (src.format) {
    case G2D_RGB565:
    case G2D_RGBA8888:
    case G2D_RGBX8888:
    case G2D_BGRA8888:
    case G2D_BGRX8888:
    case G2D_BGR565:
    case G2D_YUYV:
    case G2D_UYVY:
        src.planes[0] = buf->buf_paddr;
        break;
    case G2D_NV12:
        src.planes[0] = buf->buf_paddr;
        src.planes[1] = buf->buf_paddr + img_width * img_height;
        break;
    case G2D_I420:
        src.planes[0] = buf->buf_paddr;
        src.planes[1] = buf->buf_paddr + img_width * img_height;
        src.planes[2] = src.planes[1]  + img_width * img_height / 4;
        break;
    case G2D_YV12:
        src.planes[0] = buf->buf_paddr;
        src.planes[2] = buf->buf_paddr + img_width * img_height;
        src.planes[1] = src.planes[2]  + img_width * img_height / 4;
        break;
    case G2D_NV16:
        src.planes[0] = buf->buf_paddr;
        src.planes[1] = buf->buf_paddr + img_width * img_height;
                break;
    default:
        printf("Unsupport image format in the example code\n");
        return;
    }

    src.left = 0;
    src.top = 0;
    src.right = img_width;
    src.bottom = img_height;
    src.stride = img_width;
    src.width  = img_width;
    src.height = img_height;
    src.rot  = G2D_ROTATION_0;

    dst.planes[0] =  g_fb0_phys;  //g_display_base_phy;
    dst.left = left;
    dst.top = top;
    dst.right = left + to_width;
    dst.bottom = top + to_height;
    dst.stride = screen_info->xres;
    dst.width = screen_info->xres;
    dst.height = screen_info->yres;
    dst.rot = (g2d_rotation)rotation;
    dst.format = screen_info->bits_per_pixel == 16 ? G2D_RGB565 : (screen_info->red.offset == 0 ? G2D_RGBA8888 : G2D_BGRA8888);

    if (set_alpha) {
        src.blendfunc = G2D_ONE;
        dst.blendfunc = G2D_ONE_MINUS_SRC_ALPHA;
    
        src.global_alpha = 0x80;
        dst.global_alpha = 0xff;
    
        g2d_enable(g2dHandle, G2D_BLEND);
        g2d_enable(g2dHandle, G2D_GLOBAL_ALPHA);
    }

    g2d_blit(g2dHandle, &src, &dst);
    g2d_finish(g2dHandle);

    if (set_alpha) {
        g2d_disable(g2dHandle, G2D_GLOBAL_ALPHA);
        g2d_disable(g2dHandle, G2D_BLEND);
    }

   // g2d_close(g2dHandle);
}

// Actual rendering here.
void Render()
{
    int camera= 0;
    int i;
 
for (camera= 0; camera < CAMERA_NUM; camera++)
{
    pthread_mutex_lock(&gst_th_mutex[camera]);
   // pthread_cond_wait(&gstCON, &gst_th_mutex[camera]);        
    if(g_fill_buffer_inx[camera] == -1) i = 0;
    else  i = g_fill_buffer_inx[camera];
   {
          // for (i = 0; i < TEST_BUFFER_NUM; i++)
        {
            if( !buffers[camera][i].invalid  && (buffers[camera][i].filled))
            {
               //glBindTexture(GL_TEXTURE_2D, gTexObj[camera]);
               map_frame_to_g2d_buffer( camera, i);
               //glTexDirectInvalidateVIV(GL_TEXTURE_2D);
               buffers[camera][i].invalid = 1;
             
            }
         }
   }

   pthread_mutex_unlock(&gst_th_mutex[camera]);
}
 /* The section is to draw the output from g2d convert to display frame buffer on the screen for testing purpose to verify the conversion by g2d*/
   if(CAMERA_NUM >0)
   {   
      draw_image_to_framebuffer(g2d_buffers[0][i], g_out_width[0], g_out_height[0], dst_fmt, &g_screen_info, g_display_left, g_display_top, g_display_width, g_display_height, 0, G2D_ROTATION_0);
   }

   if(CAMERA_NUM >1)
   {   
    draw_image_to_framebuffer(g2d_buffers[1][i], g_out_width[1], g_out_height[1], dst_fmt, &g_screen_info, g_display_left+480, g_display_top, g_display_width, g_display_height, 0, G2D_ROTATION_0);
    }

   if(CAMERA_NUM >2)
   {   
     draw_image_to_framebuffer(g2d_buffers[2][i], g_out_width[2], g_out_height[2], dst_fmt, &g_screen_info, g_display_left, g_display_top+270, g_display_width, g_display_height, 0, G2D_ROTATION_0);
   }

   if(CAMERA_NUM >3)
   {   
    draw_image_to_framebuffer(g2d_buffers[3][i], g_out_width[3], g_out_height[3], dst_fmt, &g_screen_info, g_display_left+480, g_display_top+270, g_display_width, g_display_height, 0, G2D_ROTATION_0);
   }
 //report_fps();

    // flush all commands.
    //glFlush ();

}

void RenderCleanup()
{
    // cleanup
    int camera =0;
   
    for (camera=0; camera<CAMERA_NUM; camera++)
   {
      exit_flag[camera] = 1;
      if(fd_v4l[camera] >=0){   
        if (stop_capturing(fd_v4l[camera]) < 0)
        {
                printf("stop_capturing  %d failed\n", camera);
        }
       for (int i = 0; i < g_capture_num_buffers; i++) 
      {
            g2d_free(g2d_buffers[camera][i]);
            munmap(buffers[camera][i].start, buffers[camera][i].length); 
          
         if (g_mem_type == V4L2_MEMORY_USERPTR) 
            g2d_free(g2d_buffers_usrp[i]);
       }     
        close(fd_v4l[camera]);
        }
         fd_v4l[camera] = -1;
        
     }
  //  if (g_display_fg)
   //     ioctl(fd_fb_display, FBIOBLANK, FB_BLANK_NORMAL);
   //  close(fd_fb_display);
 
g2d_close(g2dHandle);

}




/***************************************************************************************
***************************************************************************************/





void sighandler(int signal)
{
    printf("Caught signal %d, setting flaq to quit.\n", signal);
    quit = 1;
}
int ParseCommandLine(int argc, char * argv[])
{
    int result = 1;
    int i =1;
    // Walk all command line arguments.
    for (i = 1; i < argc; ++i)
    {
        if (argv[i][0] == '-')
        {
            switch (argv[i][1])
            {

            case 'f':
                // f<count> for number of frames (defaults to 0).
                if (++i < argc)
                    frames = atoi(&argv[i][0]);
                else
                    result = 0;
                break;
            default:
                result = 0;
                break;
            }

            if (result == 0)
            {
                break;
            }
        }
        else
        {
            result = 0;
            break;
        }
    }

    return result;
}


/***************************************************************************************
***************************************************************************************/

void report_fps(void)
{
  static bool first_call = true;
  static unsigned int fps_count_limit = 100;
  static unsigned int fps_count = 0;
  static struct timespec t1 = { 0, 0 };
  static struct timespec t2 = { 0, 0 };

  struct timespec diff;
  double t;
  int ret;

  if (first_call) {
    ret = clock_gettime(CLOCK_REALTIME, &t1);
    if (ret != 0) {
      fprintf(stderr, "ERROR: clock_gettime(): error %i: %s\n", errno, strerror(errno));
      return ;
    }
    first_call = false;
  }

  if (fps_count >= fps_count_limit) {

    ret = clock_gettime(CLOCK_REALTIME, &t2);
    if (ret != 0) {
      fprintf(stderr, "ERROR: clock_gettime(): error %i: %s\n", errno, strerror(errno));
      return ;
    }

    Timespec_Sub(&diff, &t2, &t1);
    t = Timespec_Double(&diff);

    printf("Frame rate: %f fps\n", (double)fps_count / t);

    memcpy(&t1, &t2, sizeof (t1));

    fps_count = 0;
  }

  ++fps_count;
}

/* For testing the g2d conversion is correct, frame buffer needs to be set up.  in real opencv case, this may not be needed*/
int fb_display_setup(void)
{
    int fd_fb_bg = 0;
    struct mxcfb_gbl_alpha alpha;
    char node[8];
    int retval = TPASS;
    struct fb_fix_screeninfo fb_fix;
    struct mxcfb_pos pos;

    if (ioctl(fd_fb_display, FBIOGET_VSCREENINFO, &g_screen_info) < 0) {
        printf("fb_display_setup FBIOGET_VSCREENINFO failed\n");
        return TFAIL;
    }

    if (ioctl(fd_fb_display, FBIOGET_FSCREENINFO, &fb_fix) < 0) {
        printf("fb_display_setup FBIOGET_FSCREENINFO failed\n");
        return TFAIL;
    }

    printf("fb_fix.id = %s.\r\n", fb_fix.id);
    if ((strcmp(fb_fix.id, "DISP4 FG") == 0) || (strcmp(fb_fix.id, "DISP3 FG") == 0)) {
        g_display_fg = 1;
        {
            pos.x = 0;
            pos.y = 0;
        } 
        if (ioctl(fd_fb_display, MXCFB_SET_OVERLAY_POS, &pos) < 0) {
            printf("fb_display_setup MXCFB_SET_OVERLAY_POS failed\n");
            return TFAIL;
        }

        sprintf(node, "%d", g_display_id - 1);  //for iMX6
#ifdef BUILD_FOR_ANDROID
        strcpy(fb_display_bg_dev, "/dev/graphics/fb");
#else
        strcpy(fb_display_bg_dev, "/dev/fb");
#endif
        strcat(fb_display_bg_dev, node);
      //  if ((fd_fb_bg = open(fb_display_bg_dev, O_RDWR )) < 0) {
     //       printf("Unable to open bg frame buffer\n");
        //    return TFAIL;
      //  }

        /* Overlay setting */
     //   alpha.alpha = 1;
      //  alpha.enable = 1;
     //   if (ioctl(fd_fb_bg, MXCFB_SET_GBL_ALPHA, &alpha) < 0) {
      //      printf("Set global alpha failed\n");
     //       close(fd_fb_bg);
      //      return TFAIL;
       // }

         {
            ioctl(fd_fb_bg, FBIOGET_VSCREENINFO, &g_screen_info);

            g_screen_info.yres_virtual = g_screen_info.yres * g_display_num_buffers;
            if (ioctl(fd_fb_display, FBIOPUT_VSCREENINFO, &g_screen_info) < 0) {
                printf("fb_display_setup FBIOPUET_VSCREENINFO failed\n");
                return TFAIL;
            }
            ioctl(fd_fb_display, FBIOGET_FSCREENINFO, &fb_fix);
            ioctl(fd_fb_display, FBIOGET_VSCREENINFO, &g_screen_info);
        }
    } else {
        g_display_fg = 0;

    }

    ioctl(fd_fb_display, FBIOBLANK, FB_BLANK_UNBLANK);

    g_display_base_phy = fb_fix.smem_start;
    printf("fb: smem_start = 0x%x, smem_len = 0x%x.\r\n", (unsigned int)fb_fix.smem_start, (unsigned int)fb_fix.smem_len);

    g_display_size = g_screen_info.xres * g_screen_info.yres * g_screen_info.bits_per_pixel / 8;
    printf("fb: frame buffer size = 0x%x bytes.\r\n", g_display_size);

    printf("fb: g_screen_info.xres = %d, g_screen_info.yres = %d.\r\n", g_screen_info.xres, g_screen_info.yres);
    printf("fb: g_display_left = %d.\r\n", g_display_left);
    printf("fb: g_display_top = %d.\r\n", g_display_top);
    printf("fb: g_display_width = %d.\r\n", g_display_width);
    printf("fb: g_display_height = %d.\r\n", g_display_height);

    return TPASS;
}

// Program entry.
int main(int argc, char** argv)
{   
    int retval = TPASS;
     int camera = 0;
    int currentFrame = 0;
    signal(SIGINT, sighandler);
    signal(SIGTERM, sighandler);   
   // prepare_g2d_buffers();
       ParseCommandLine(argc, argv);
//to set up frame buffer for display, it is for testing purpose.

     // struct fb_var_screeninfo screen_info;
      struct fb_fix_screeninfo fb_info;

    if ((fd_fb0 = open("/dev/fb0", O_RDWR, 0)) < 0) {
        if ((fd_fb0 = open("/dev/graphics/fb0", O_RDWR, 0)) < 0) {
            printf("Unable to open fb0\n");
            retval = TFAIL;
            goto OnError;
        }
    }

    /* Get fix screen info. */
    retval = ioctl(fd_fb0, FBIOGET_FSCREENINFO, &fb_info);
    if(retval < 0) {
        goto OnError;
    }
    g_fb0_phys = fb_info.smem_start;

    /* Get variable screen info. */
    retval = ioctl(fd_fb0, FBIOGET_VSCREENINFO, &g_screen_info);
    if (retval < 0) {
        goto OnError;
    }

    g_fb0_phys += (g_screen_info.xres_virtual * g_screen_info.yoffset * g_screen_info.bits_per_pixel / 8);

    g_fb0_size = g_screen_info.xres_virtual * g_screen_info.yres_virtual * g_screen_info.bits_per_pixel / 8;

    fd_v4l[camera] = v4l_capture_setup();
    if (fd_v4l[camera] < 0)
        return -1;
    if (g_mem_type == V4L2_MEMORY_USERPTR){
        if (memalloc(g_frame_size, TEST_BUFFER_NUM) ) {
            close(fd_v4l[camera]);
             goto OnError;

        }
      }

    {
        if (!RenderInit())
        {
            goto OnError;
        }
       printf("RenderInit done\n");
      prepare_g2d_buffers();
        while (!quit)
        {
            {
                Render();
               
                report_fps();
               currentFrame++;
            }
                     if ((frames > 0) && (--frames == 0)) 
                     {
                           quit = 1;
                     }          
        }


       

        RenderCleanup();
    }

OnError:
    close(fd_fb0);

    return 0;
}
