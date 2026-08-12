#include <stdio.h>
#include <stdint.h>
#include <time.h>

#include "lvgl/lvgl.h"


uint32_t custom_tick_get(void)
{
    struct timespec ts;

    clock_gettime(CLOCK_MONOTONIC, &ts);

    return (uint32_t)(
        ts.tv_sec * 1000ULL +
        ts.tv_nsec / 1000000ULL
    );
}


int main(void)
{
    printf("\n");
    printf("========================================\n");
    printf(" LVGL Shared Library Test\n");
    printf("========================================\n");

    printf("LVGL version: %d.%d.%d\n",
           LVGL_VERSION_MAJOR,
           LVGL_VERSION_MINOR,
           LVGL_VERSION_PATCH);

    /*
     * LVGL 初始化
     */
    lv_init();

    printf("lv_init() OK\n");

    /*
     * 获取当前 screen
     */
    lv_obj_t *scr = lv_scr_act();

    if (scr == NULL) {
        printf("ERROR: lv_scr_act() failed\n");
        return -1;
    }

    printf("lv_scr_act() OK\n");

    /*
     * 创建 Label
     */
    lv_obj_t *label = lv_label_create(scr, NULL);

    if (label == NULL) {
        printf("ERROR: lv_label_create() failed\n");
        return -1;
    }

    lv_label_set_text(label, "Hello LVGL SO!");

    printf("lv_label_create() OK\n");
    printf("lv_label_set_text() OK\n");

    printf("\n");
    printf("custom_tick_get() = %u ms\n", custom_tick_get());

    printf("========================================\n");
    printf(" LVGL shared library test PASS\n");
    printf("========================================\n");

    return 0;
}
