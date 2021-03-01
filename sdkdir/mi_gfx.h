/* Copyright (c) 2018-2019 Sigmastar Technology Corp.
 All rights reserved.

  Unless otherwise stipulated in writing, any and all information contained
 herein regardless in any format shall remain the sole proprietary of
 Sigmastar Technology Corp. and be kept in strict confidence
 (��Sigmastar Confidential Information��) by the recipient.
 Any unauthorized act including without limitation unauthorized disclosure,
 copying, use, reproduction, sale, distribution, modification, disassembling,
 reverse engineering and compiling of the contents of Sigmastar Confidential
 Information is unlawful and strictly prohibited. Sigmastar hereby reserves the
 rights to any and all damages, losses, costs and expenses resulting therefrom.
*/
#ifndef _MI_GFX_H_
#define _MI_GFX_H_
#ifdef __cplusplus
extern "C" {
#endif

#include "mi_common.h"
#include "mi_gfx_datatype.h"

#define GFX_MAJOR_VERSION 2
#define GFX_SUB_VERSION 4
#define MACRO_TO_STR(macro) #macro
#define GFX_VERSION_STR(major_version,sub_version) ({char *tmp = sub_version/100 ? \
                                    "mi_gfx_version_" MACRO_TO_STR(major_version)"." MACRO_TO_STR(sub_version) : sub_version/10 ? \
                                    "mi_gfx_version_" MACRO_TO_STR(major_version)".0" MACRO_TO_STR(sub_version) : \
                                    "mi_gfx_version_" MACRO_TO_STR(major_version)".00" MACRO_TO_STR(sub_version);tmp;})
#define MI_GFX_API_VERSION GFX_VERSION_STR(GFX_MAJOR_VERSION,GFX_SUB_VERSION)

/*-------------------------------------------------------------------------------------------
 * Global Functions
-------------------------------------------------------------------------------------------*/

MI_S32 MI_GFX_Open(void);
MI_S32 MI_GFX_Close(void);
MI_S32 MI_GFX_WaitAllDone(MI_BOOL bWaitAllDone, MI_U16 u16TargetFence);
MI_S32 MI_GFX_QuickFill(MI_GFX_Surface_t *pstDst, MI_GFX_Rect_t *pstDstRect,
    MI_U32 u32ColorVal, MI_U16 *pu16Fence);
MI_S32 MI_GFX_GetAlphaThresholdValue(MI_U8 *pu8ThresholdValue);
MI_S32 MI_GFX_SetAlphaThresholdValue(MI_U8 u8ThresholdValue);
MI_S32 MI_GFX_BitBlit(MI_GFX_Surface_t *pstSrc, MI_GFX_Rect_t *pstSrcRect,
    MI_GFX_Surface_t *pstDst,  MI_GFX_Rect_t *pstDstRect, MI_GFX_Opt_t *pstOpt, MI_U16 *pu16Fence);

#ifdef __cplusplus
}
#endif

#endif //_MI_GFX_H_
