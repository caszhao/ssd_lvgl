欢迎来到caszhao的基于Sigmastar系列的 LVGL源码仓库！
Welcome to caszhao's LVGL git source of LVGL base on Sigmastar SSD Platform.
====

如何使用：
----
    举例说明，由于我采用的是MIPI屏(800x1280分辨率，竖屏），如果要变为横屏使用
    1、修改你项目中fbdev.ini为两倍framebuffer大小，  位于 project\board\i2m\SSC011A-S01A\config\fbdev.ini
    FB_BUFFER_LEN = 8000
    这个值是由800 * 1280 * 4 * 2 / 1024所得，请根据你自己的屏幕分辨率修改此值
  
    2、修改 lv_conf.h
    我的旋转90度举例如下：
      //这里是旋转前的GUI大小，旋转后以便在800 x 1280的屏幕上显示
      #define LV_HOR_RES_MAX          (1280)
      #define LV_VER_RES_MAX          (800)
   
      //不旋转为0，软件旋转设为1，硬件旋转设为2，默认不旋转
      /*
       * caszhao add, 2021-03-01
       * - 0:  NO ROTATION
       * - 1:  SOFT ROTATION
       * - 2:  GFX ROTATION
      */
      
      #define LV_MONITOR_ROTATE 0

      //双缓冲开关，0为不使能双缓冲，1为使能双缓冲，默认使能
      /*
       * caszhao add, 2021-03-01
       * - 0:  disable dual framebuffer
       * - 1:  enable dual framebuffer
      */
      
      #define LV_DUAL_FRAMEBUFFER 1
      
    3、修改屏参
    屏参修改位于ssd_lv_drivers/display/sstardisp.c, 修改SQ101AB4II405_800x1280_MIPI.h为你的屏参

如何编译：
----
    本项目基于Sigmastar 201/202D源码，把ssd_lvgl目录放置于和sigmastar 201/202D 源码根目录(project,apps,boot，kernel等同级目录）,编译指令.

    1、make clean
    2、make -j4 (-j4 是线程数，核多的，譬如我，使用make -j48)

    ssd_lvgl目录下生成demossd即为所需。

    双缓冲实现，基于ETTEETTE的实现小幅度修改，表示感谢，我主要增加了屏幕旋转功能，包括软转和硬转。支持90度，180度和270度旋转。后续更多的功能会陆续添加

    features总结:

    1、基于LVGL 7.10
    2、实现双缓冲，fbdev.c内直接初始化屏参（ETTEETTE）
    3、实现屏幕旋转(支持90度，180度和270度旋转)

    硬转通过芯片内置的Graphic Engine实现， 从属于bitblit的eRotate

    感谢ETTEETTE，感谢LVGL开源项目，感谢Sigmastar。

温馨提示：
----

    欢迎与我沟通讨论技术细节，不局限于嵌入式，Linux，Android，Web等，
    
    我写代码超过20年，曾经是一名Oier和初探门径的ACMer。 
    
    写代码是我的生活的一部分。也欢迎你加入此开源项目。  
    
    Contact :
    wechat : icarxx
    QQ: 497477
    

# 2021-03-01, By caszhao

    Porting LVGL to Sigmastar SSD Series platform.

    1、Base on Sigmastar SSD Series chips.
    2、Double framebuffer. (Thanks to ETTEETTE's contribution)
    3、Software rotation, just for 270` degree.
    4、Add Sigmastar Graphic Engine. To implement Bitblit with rotation. Support 90`  180` 270` 

    https://github.com/caszhao/ssd_lvgl

# LVGL for frame buffer device

    LVGL configured to work with /dev/fb0 on Linux.

    When cloning this repository, also make sure to download submodules (`git submodule update --init --recursive`) otherwise you will be missing key components.

    Check out this blog post for a step by step tutorial:
    https://blog.lvgl.io/2018-01-03/linux_fb
