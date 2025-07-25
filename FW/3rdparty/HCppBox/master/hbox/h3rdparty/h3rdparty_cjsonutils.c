/***************************************************************
 * Name:      h3rdparty_cjsonutils.c
 * Purpose:   引入第三方源代码文件
 * Author:    HYH (hyhsystem.cn)
 * Created:   2024-09-27
 * Copyright: HYH (hyhsystem.cn)
 * License:   MIT
 **************************************************************/
#include "hdefaults.h"
#include "h3rdparty.h"

//定义内存分配函数
#define malloc hmalloc
#define free   hfree
#define calloc hcalloc
#define realloc hrealloc

#ifndef  H3RDPARTY_USING_SYSTEM_CJSON
#include "3rdparty/cJSON/cJSON_Utils.c"
#endif // H3RDPARTY_USING_SYSTEM_CJSON


