#ifndef __MAIN_H
#define __MAIN_H

#define USB_INFO 1 // info(usb_printf) control

#define USE_CALIBRATION (DS_SENSOR == 132 || DS_SENSOR == 133 || DS_SENSOR == 138 || DS_SENSOR == 144) // calibration
#define USE_BLINK (DS_SENSOR == 133 || DS_SENSOR == 132 || DS_SENSOR == 144)                           // led blink

void startup(void); // cpu running fast

#endif