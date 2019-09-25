#ifndef __IMAGE_H__
#define __IMAGE_H__

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "typedefs.h"

#ifdef __cplusplus
extern "C" {
#endif

void drawRectangle ( unsigned char Data[IMAGE_HEIGHT][IMAGE_WIDTH], 
                     MyRect r);

int writePgm(const char *fileName, unsigned char Data[IMAGE_HEIGHT][IMAGE_WIDTH] );

#ifdef __cplusplus
}
#endif

#endif
