/*****************************************************************************
* | File      	:   EPD_7in3e_test.c
* | Author      :   Waveshare team
* | Function    :   7.3inch e-Paper (F) Demo
* | Info        :
*----------------
* |	This version:   V1.0
* | Date        :   2023-03-13
* | Info        :
* -----------------------------------------------------------------------------
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documnetation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to  whom the Software is
# furished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS OR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
#
******************************************************************************/
#include "EPD_Test.h"
#include "ImageData.h"
#include "run_File.h"
#include "EPD_7in3e.h"
#include "GUI_Paint.h"
#include "GUI_BMPfile.h"

#include <stdlib.h> // malloc() free()
#include <string.h>

int EPD_7in3e_display_BMP(const char *path, float vol)
{
    printf("e-Paper Init and Clear...\r\n");
    EPD_7IN3E_Init();

    //Create a new image cache
    UBYTE *BlackImage;
    UDOUBLE Imagesize = ((EPD_7IN3E_WIDTH % 2 == 0)? (EPD_7IN3E_WIDTH / 2 ): (EPD_7IN3E_WIDTH / 2 + 1)) * EPD_7IN3E_HEIGHT;
    if((BlackImage = (UBYTE *)malloc(Imagesize)) == NULL) {
        printf("Failed to apply for black memory...\r\n");
        return -1;
    }
    printf("Paint_NewImage\r\n");
    Paint_NewImage(BlackImage, EPD_7IN3E_WIDTH, EPD_7IN3E_HEIGHT, 0, EPD_7IN3E_WHITE);
    Paint_SetScale(6);

#if 1
    run_mount();

    printf("Display BMP\r\n");
    Paint_SelectImage(BlackImage);
    Paint_Clear(EPD_7IN3E_WHITE);
    
    GUI_ReadBmp_RGB_6Color(path, 0, 0);

    if(Paint_GetRotate() == 90)
        Paint_SetRotate(270);
    else
        Paint_SetRotate(180);
    char strvol[21] = {0};
    sprintf(strvol, "%f V", vol);
    if(vol < 3.3) {
        Paint_DrawString_EN(10, 10, "Low voltage, please charge in time.", &Font16, EPD_7IN3E_BLACK, EPD_7IN3E_WHITE);
        Paint_DrawString_EN(10, 26, strvol, &Font16, EPD_7IN3E_BLACK, EPD_7IN3E_WHITE);
    }

    printf("EPD_Display\r\n");
    EPD_7IN3E_Display(BlackImage);

    run_unmount();
#endif
    printf("Update Path Index...\r\n");
    updatePathIndex();

    printf("Goto Sleep...\r\n\r\n");
    EPD_7IN3E_Sleep();
    free(BlackImage);
    BlackImage = NULL;

    return 0;
}

int EPD_7in3e_display(float vol)
{
    printf("e-Paper Init and Clear...\r\n");
    EPD_7IN3E_Init();

    //Create a new image cache
    UBYTE *BlackImage;
    UDOUBLE Imagesize = ((EPD_7IN3E_WIDTH % 2 == 0)? (EPD_7IN3E_WIDTH / 2 ): (EPD_7IN3E_WIDTH / 2 + 1)) * EPD_7IN3E_HEIGHT;
    if((BlackImage = (UBYTE *)malloc(Imagesize)) == NULL) {
        printf("Failed to apply for black memory...\r\n");
        return -1;
    }
    printf("Paint_NewImage\r\n");
    Paint_NewImage(BlackImage, EPD_7IN3E_WIDTH, EPD_7IN3E_HEIGHT, 0, EPD_7IN3E_WHITE);
    Paint_SetScale(6);

    printf("Display BMP\r\n");
    Paint_SelectImage(BlackImage);
    Paint_Clear(EPD_7IN3E_WHITE);
    
    Paint_DrawBitMap(Image6color);

    Paint_SetRotate(270);
    char strvol[21] = {0};
    sprintf(strvol, "%f V", vol);
    if(vol < 3.3) {
        Paint_DrawString_EN(10, 10, "Low voltage, please charge in time.", &Font16, EPD_7IN3E_BLACK, EPD_7IN3E_WHITE);
        Paint_DrawString_EN(10, 26, strvol, &Font16, EPD_7IN3E_BLACK, EPD_7IN3E_WHITE);
    }

    printf("EPD_Display\r\n");
    EPD_7IN3E_Display(BlackImage);

    printf("Goto Sleep...\r\n\r\n");
    EPD_7IN3E_Sleep();
    free(BlackImage);
    BlackImage = NULL;

    return 0;
}

int EPD_7in3e_test(void)
{
    printf("e-Paper Init and Clear...\r\n");
    EPD_7IN3E_Init();

    //Create a new image cache
    UBYTE *BlackImage;
    UDOUBLE Imagesize = ((EPD_7IN3E_WIDTH % 2 == 0)? (EPD_7IN3E_WIDTH / 2 ): (EPD_7IN3E_WIDTH / 2 + 1)) * EPD_7IN3E_HEIGHT;
    if((BlackImage = (UBYTE *)malloc(Imagesize)) == NULL) {
        printf("Failed to apply for black memory...\r\n");
        return -1;
    }
    printf("Paint_NewImage\r\n");
    Paint_NewImage(BlackImage, EPD_7IN3E_WIDTH, EPD_7IN3E_HEIGHT, 0, EPD_7IN3E_WHITE);
    Paint_SetScale(6);

#if 1   // Drawing on the image
    //1.Select Image
    printf("SelectImage:BlackImage\r\n");
    Paint_SelectImage(BlackImage);
    Paint_Clear(EPD_7IN3E_WHITE);

    int hNumber, hWidth, vNumber, vWidth;
    hNumber = 20;
	hWidth = EPD_7IN3E_HEIGHT/hNumber; // 800/20
    vNumber = 10;
	vWidth = EPD_7IN3E_WIDTH/vNumber; // 480/10
	
    // 2.Drawing on the image
    printf("Drawing:BlackImage\r\n");
	for(int i=0; i<vNumber; i++) {
		Paint_DrawRectangle(1, 1+i*vWidth, 800, vWidth*(i+1), EPD_7IN3E_GREEN + (i % 5), DOT_PIXEL_1X1, DRAW_FILL_FULL);
	}
	for(int i=0, j=0; i<hNumber; i++) {
		if(i%2) {
			j++;
			Paint_DrawRectangle(1+i*hWidth, 1, hWidth*(1+i), 480, j%2 ? EPD_7IN3E_BLACK : EPD_7IN3E_WHITE, DOT_PIXEL_1X1, DRAW_FILL_FULL);
		}
	}

    printf("EPD_Display\r\n");
    EPD_7IN3E_Display(BlackImage);
#endif

    printf("Goto Sleep...\r\n");
    EPD_7IN3E_Sleep();
    free(BlackImage);
    BlackImage = NULL;

    return 0;
}

int EPD_7in3e_display_low_battery(void)
{
    printf("e-Paper Init and Clear...\r\n");
    EPD_7IN3E_Init();

    //Create a new image cache
    UBYTE *BlackImage;
    UDOUBLE Imagesize = ((EPD_7IN3E_WIDTH % 2 == 0)? (EPD_7IN3E_WIDTH / 2 ): (EPD_7IN3E_WIDTH / 2 + 1)) * EPD_7IN3E_HEIGHT;
    if((BlackImage = (UBYTE *)malloc(Imagesize)) == NULL) {
        printf("Failed to apply for black memory...\r\n");
        return -1;
    }
    printf("Paint_NewImage\r\n");
    Paint_NewImage(BlackImage, EPD_7IN3E_WIDTH, EPD_7IN3E_HEIGHT, 0, EPD_7IN3E_WHITE);
    Paint_SetScale(6);

    printf("Display Low Battery Message\r\n");
    Paint_SelectImage(BlackImage);
    Paint_Clear(EPD_7IN3E_WHITE);
    
    // Display a battery icon (simple rectangle with + terminal)
    Paint_SetRotate(270);
    
    // Draw battery outline
    Paint_DrawRectangle(200, 150, 600, 350, EPD_7IN3E_BLACK, DOT_PIXEL_3X3, DRAW_FILL_EMPTY);
    Paint_DrawRectangle(600, 220, 650, 280, EPD_7IN3E_BLACK, DOT_PIXEL_3X3, DRAW_FILL_FULL);
    
    // Draw battery level (low)
    Paint_DrawRectangle(220, 170, 280, 330, EPD_7IN3E_RED, DOT_PIXEL_1X1, DRAW_FILL_FULL);
    
    // Draw warning message
    Paint_DrawString_EN(120, 400, "Please Charge the Battery", &Font24, EPD_7IN3E_RED, EPD_7IN3E_WHITE);

    printf("EPD_Display\r\n");
    EPD_7IN3E_Display(BlackImage);

    printf("Goto Sleep...\r\n\r\n");
    EPD_7IN3E_Sleep();
    free(BlackImage);
    BlackImage = NULL;

    return 0;
}

