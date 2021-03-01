/**
 * @file fbdev.c
 *
 */

/*********************
 *      INCLUDES
 *********************/
#define USE_SSD20X


#include "fbdev.h"

#if USE_FBDEV || USE_BSD_FBDEV

#include <stdlib.h>
#include <unistd.h>
#include <stddef.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

#if USE_BSD_FBDEV
#include <sys/fcntl.h>
#include <sys/time.h>
#include <sys/consio.h>
#include <sys/fbio.h>
#else  /* USE_BSD_FBDEV */
#include <linux/fb.h>
#include   <pthread.h>
#include <sys/epoll.h>
#endif /* USE_BSD_FBDEV */
#ifdef USE_SSD20X
#include <mi_common.h>
#include <mi_sys_datatype.h>
#include <mi_sys.h>
#include "mstarFb.h"
#include "sstardisp.h"
#include <mi_gfx.h>
#include <mi_gfx_datatype.h>
#include "verify_gfx_type.h"
#include "verify_gfx.h"
#include "blitutil.h"
#endif

#if (LV_MONITOR_ROTATE == 2)
#include <string.h>
#include "mi_gfx.h"
#include "mi_gfx_datatype.h"
#include "mi_sys.h"


#endif

/*********************
 *      DEFINES
 *********************/
#ifndef FBDEV_PATH
#define FBDEV_PATH  "/dev/fb0"
#endif

char toggle = 0;

#if (LV_MONITOR_ROTATE == 2)
MI_PHY phyAddr = NULL;; // For example: Fill a rect to Fb. (u32PhyAddr = Fb Start Addr)
MI_U64 u64VirAddr;
#endif


/**********************
 *      TYPEDEFS
 **********************/

/**********************
 *      STRUCTURES
 **********************/

struct bsd_fb_var_info{
    uint32_t xoffset;
    uint32_t yoffset;
    uint32_t xres;
    uint32_t yres;
    int bits_per_pixel;
 };

struct bsd_fb_fix_info{
    long int line_length;
    long int smem_len;
};

/**********************
 *  STATIC PROTOTYPES
 **********************/

/**********************
 *  STATIC VARIABLES
 **********************/
#if USE_BSD_FBDEV
static struct bsd_fb_var_info vinfo;
static struct bsd_fb_fix_info finfo;
#else
static struct fb_var_screeninfo vinfo;
static struct fb_fix_screeninfo finfo;
#endif /* USE_BSD_FBDEV */
static char *fbp = 0,*fbpbase = 0;
static long int half_screensize = 0;
static int fbfd = 0;

#ifdef USE_SSD20X

MI_FB_DisplayLayerAttr_t g_stLayerInfo = {0};
#endif 
/**********************
 *      MACROS
 **********************/

/**********************
 *   GLOBAL FUNCTIONS
 **********************/

#define ExecFunc(_func_, _ret_) \
    if (_func_ != _ret_)\
    {\
        printf("[%s][%d]exec function failed\n", __FUNCTION__, __LINE__);\
        return ;\
    }\
    else\
    {\
        printf("[%s][%d]exec function pass\n", __FUNCTION__, __LINE__);\
    }


#ifdef USE_SSD20X
void init_ssd20x_disp(void)
{

    MI_DISP_PubAttr_t stDispPubAttr = {0};
    struct timeval tv1 = {0, 0};
    struct timeval tv2 = {0, 0};

    memset(&finfo, 0, sizeof(struct fb_fix_screeninfo));
    memset(&vinfo, 0, sizeof(struct fb_var_screeninfo));
    memset(&g_stLayerInfo, 0, sizeof(MI_FB_DisplayLayerAttr_t));

    gettimeofday(&tv1, NULL);

    stDispPubAttr.eIntfType = E_MI_DISP_INTF_LCD;
    stDispPubAttr.eIntfSync = E_MI_DISP_OUTPUT_USER;

    //LOG();
    sstar_disp_init(&stDispPubAttr);

    gettimeofday(&tv2, NULL);
    printf("%s %d %ld\n", __FUNCTION__, __LINE__, (tv2.tv_usec - tv1.tv_usec) / 1000);

    /* Open the file for reading and writing */
    fbfd = open(FBDEV_PATH, O_RDWR);

    if(fbfd == -1)
    {
        perror("Error: cannot open framebuffer device");
        exit(1);
    }

    //get fb_fix_screeninfo
    if(ioctl(fbfd, FBIOGET_FSCREENINFO, &finfo) == -1)
    {
        perror("Error reading fixed information");
        exit(2);
    }

    //get fb_var_screeninfo
    if(ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo) == -1)
    {
        perror("Error reading variable information");
        exit(3);
    }
    //get FBIOGET_DISPLAYLAYER_ATTRIBUTES
    if(ioctl(fbfd, FBIOGET_DISPLAYLAYER_ATTRIBUTES, &g_stLayerInfo) == -1)
    {
        perror("3Error reading variable information");
        exit(3);
    }

    printf("xres: %d,yres: %d,line_length: %d,eFbColorFmt: %d  finfo.smem_start = %d finfo.smem_len = %d \n",\
    vinfo.xres,vinfo.yres,finfo.line_length,g_stLayerInfo.eFbColorFmt,finfo.smem_start,finfo.smem_len);


    half_screensize =  finfo.smem_len >> 1;

    fbp = (char *) mmap(0, finfo.smem_len, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, 0);
    fbpbase =  fbp;
    if(fbp == MAP_FAILED)
    {
        perror("Error: Failed to map framebuffer device to memory");
        exit(4);
    }
}
#endif

void fbdev_init(void)
{
#ifdef USE_SSD20X
    init_ssd20x_disp();
#else
    // Open the file for reading and writing
    fbfd = open(FBDEV_PATH, O_RDWR);
    if(fbfd == -1) {
        perror("Error: cannot open framebuffer device");
        return;
    }
    printf("The framebuffer device was opened successfully.\n");

#if USE_BSD_FBDEV
    struct fbtype fb;
    unsigned line_length;

    //Get fb type
    if (ioctl(fbfd, FBIOGTYPE, &fb) != 0) {
        perror("ioctl(FBIOGTYPE)");
        return;
    }

    //Get screen width
    if (ioctl(fbfd, FBIO_GETLINEWIDTH, &line_length) != 0) {
        perror("ioctl(FBIO_GETLINEWIDTH)");
        return;
    }

    vinfo.xres = (unsigned) fb.fb_width;
    vinfo.yres = (unsigned) fb.fb_height;
    vinfo.bits_per_pixel = fb.fb_depth + 8;
    vinfo.xoffset = 0;
    vinfo.yoffset = 0;
    finfo.line_length = line_length;
    finfo.smem_len = finfo.line_length * vinfo.yres;
#else /* USE_BSD_FBDEV */

    // Get fixed screen information
    if(ioctl(fbfd, FBIOGET_FSCREENINFO, &finfo) == -1) {
        perror("Error reading fixed information");
        return;
    }

    // Get variable screen information
    if(ioctl(fbfd, FBIOGET_VSCREENINFO, &vinfo) == -1) {
        perror("Error reading variable information");
        return;
    }
#endif /* USE_BSD_FBDEV */

    printf("%dx%d, %dbpp finfo.smem_len = %d \n", vinfo.xres, vinfo.yres, vinfo.bits_per_pixel,finfo.smem_len);

    // Figure out the size of the screen in bytes

    half_screensize = finfo.smem_len >> 1;
    // Map the device to memory
    fbp = (char *)mmap(0, finfo.smem_len;, PROT_READ | PROT_WRITE, MAP_SHARED, fbfd, 0);
	  fbpbase =  fbp;
    if((intptr_t)fbp == -1) {
        perror("Error: failed to map framebuffer device to memory");
        return;
    }
    memset(fbp, 0, finfo.smem_len);

    printf("The framebuffer device was mapped to memory successfully.\n");
#endif

}

void fbdev_exit(void)
{
    close(fbfd);
}

#if (LV_MONITOR_ROTATE == 1)
void get_col(const lv_color_t * src, const lv_area_t * area, lv_coord_t y, lv_color_t * dest)
{
    lv_coord_t w = lv_area_get_width(area);
    lv_coord_t h = lv_area_get_height(area);

    lv_coord_t i;
    for(i = 0; i < h; i++) {
        dest[i].full = src[y].full;
        src += w ;
    }
}
#endif


/**
 * Flush a buffer to the marked area
 * @param drv pointer to driver where this function belongs
 * @param area an area where to copy `color_p`
 * @param color_p an array of pixel to copy to the `area` part of the screen
 */
void fbdev_flush(lv_disp_drv_t * drv, const lv_area_t * area, lv_color_t * color_p)
{
    if(fbp == NULL ||
            area->x2 < 0 ||
            area->y2 < 0 ||
            area->x1 > LV_HOR_RES_MAX - 1 ||
            area->y1 > LV_VER_RES_MAX - 1) {
        lv_disp_flush_ready(drv);
        return;
    }
#if LV_DUAL_FRAMEBUFFER
    if(toggle == 0)
    {
        toggle = 1;
        vinfo.yoffset = 0;
        fbp =fbpbase;
    }
    else
    {
        toggle = 0;
        vinfo.yoffset = vinfo.yres;	
        fbp =  fbpbase + half_screensize;
    }	
#else
    fbp = fbpbase;
#endif
    /*Truncate the area to the screen*/
    int32_t act_x1 = area->x1 < 0 ? 0 : area->x1;
    int32_t act_y1 = area->y1 < 0 ? 0 : area->y1;
    int32_t act_x2 = area->x2 > LV_HOR_RES_MAX - 1 ? LV_HOR_RES_MAX- 1 : area->x2;
    int32_t act_y2 = area->y2 > LV_VER_RES_MAX - 1 ? LV_VER_RES_MAX - 1 : area->y2;


    lv_coord_t w = (act_x2 - act_x1 + 1);
    long int location = 0;

    /*32 or 24 bit per pixel*/
    if(vinfo.bits_per_pixel == 32 || vinfo.bits_per_pixel == 24) {
      #if (LV_MONITOR_ROTATE == 0)
            uint32_t * fbp32 = (uint32_t *)fbp;
            int32_t y;
            for(y = act_y1; y <= act_y2; y++) 
            {
                location = (act_x1 ) + (y ) * finfo.line_length / 4;
                memcpy(&fbp32[location], (uint32_t *)color_p, (act_x2 - act_x1 + 1) * 4);
                color_p += w;
            }
       #elif (LV_MONITOR_ROTATE == 1)
            lv_coord_t x2 = area->x2;
            if(x2 >= drv->ver_res) x2 = vinfo.yres - 1;
        
            //lv_color_t * dest = (lv_color_t *)monitor.tft_fb;
            uint32_t * fbp32 = (uint32_t *)fbp;
            
            fbp32 +=  vinfo.xres * (vinfo.yres - 1);
            fbp32 -= vinfo.xres * area->x1;
        
            lv_coord_t x;
            for(x = area->x1; x <= x2; x++) {
                  get_col(color_p, area, (x - area->x1), fbp32 + area->y1);
                  fbp32 -= vinfo.xres;
            }
       #else
   

            MI_U16 u16TargetFence;
            MI_GFX_Surface_t stDst;
            MI_GFX_Rect_t stDstRect;
            
            MI_GFX_Surface_t stSrc;
            MI_GFX_Rect_t stSrcRect;
            MI_GFX_Opt_t stOpt;

            memset(&stOpt, 0, sizeof(stOpt));
            ExecFunc(MI_SYS_Init(), MI_SUCCESS);

            if (phyAddr == NULL)
            {
                // Ethan add, 2021-03-01, wechat: icarxx
                printf("phyAddr == NULL \r\n");
                ExecFunc(MI_SYS_MMA_Alloc("mma_heap_name0", LV_HOR_RES_MAX * LV_VER_RES_MAX * 4, &phyAddr), MI_SUCCESS);
                
                ExecFunc(MI_GFX_Open(), MI_SUCCESS);
                ExecFunc(MI_SYS_Mmap(phyAddr, LV_HOR_RES_MAX * LV_VER_RES_MAX * 4, &u64VirAddr , FALSE), MI_SUCCESS);
                
                memset((void *)u64VirAddr, 0xFF, LV_HOR_RES_MAX * LV_VER_RES_MAX * 4);
            }
            
            int32_t y;
            for(y = act_y1; y <= act_y2; y++) 
            {
                location = (act_x1 ) + (y ) * LV_HOR_RES_MAX;
                memcpy((uint32_t *)u64VirAddr + location, (uint32_t *)color_p, (act_x2 - act_x1 + 1) * 4);
                color_p += w;
            }
            //TODO: Dosomething bitblit/fill

            //bitblit
            stSrc.eColorFmt = E_MI_GFX_FMT_ARGB8888;
            stSrc.u32Width = LV_HOR_RES_MAX;
            stSrc.u32Height = LV_VER_RES_MAX;
            stSrc.u32Stride = LV_HOR_RES_MAX * 4;
            stSrc.phyAddr = phyAddr;

            stSrcRect.s32Xpos = 0;
            stSrcRect.s32Ypos = 0;
            stSrcRect.u32Width = LV_HOR_RES_MAX;
            stSrcRect.u32Height = LV_VER_RES_MAX;

            //rotation
            stDst.eColorFmt = E_MI_GFX_FMT_ARGB8888;
            stDst.u32Width = LV_VER_RES_MAX;
            stDst.u32Height = LV_HOR_RES_MAX;
            stDst.u32Stride =  LV_VER_RES_MAX * 4;
            #if LV_DUAL_FRAMEBUFFER
                if(toggle == 1)
                    stDst.phyAddr = finfo.smem_start;
                else
                    stDst.phyAddr = finfo.smem_start + half_screensize;
            #else
                stDst.phyAddr = finfo.smem_start;
            #endif
            stDstRect.s32Xpos = 0;
            stDstRect.s32Ypos = 0;
            stDstRect.u32Width = LV_HOR_RES_MAX;
            stDstRect.u32Height = LV_VER_RES_MAX;
            
            stOpt.eRotate = E_MI_GFX_ROTATE_270;
            stOpt.u32GlobalSrcConstColor = 0xFF000000;
            stOpt.u32GlobalDstConstColor = 0xFF000000;
            stOpt.eMirror = E_MI_GFX_MIRROR_NONE;

            stOpt.eSrcDfbBldOp = E_MI_GFX_DFB_BLD_ONE;
            stOpt.eDstDfbBldOp = E_MI_GFX_DFB_BLD_ZERO;

            ExecFunc(MI_GFX_BitBlit(&stSrc, &stSrcRect, &stDst, &stDstRect, &stOpt, &u16TargetFence), MI_SUCCESS);
            ExecFunc(MI_GFX_WaitAllDone(FALSE, u16TargetFence), MI_SUCCESS);

        #endif
	}
  else {
        /*Not supported bit per pixel*/
    }

#if LV_DUAL_FRAMEBUFFER
	if (ioctl(fbfd, FBIOPAN_DISPLAY, &vinfo) < 0) {
		fprintf(stderr, "active fb swap failed\n");
	}
#endif

    //May be some direct update command is required
    //ret = ioctl(state->fd, FBIO_UPDATE, (unsigned long)((uintptr_t)rect));

    lv_disp_flush_ready(drv);
}

/**********************
 *   STATIC FUNCTIONS
 **********************/

#endif
