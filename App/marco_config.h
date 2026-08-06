#define USB_INFO 1 // info(usb_printf) control

#define USE_BLINK (DS_SENSOR == 132 || DS_SENSOR == 133 || DS_SENSOR == 138 || DS_SENSOR == 144 || DS_SENSOR == 165) // calibration

#if USB_INFO
#include "stc32_stc8_usb.h"
#endif