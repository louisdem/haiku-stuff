#ifndef _ACPI_THINKPAD_P_H
#define _ACPI_THINKPAD_P_H

#include <Drivers.h>

// io controls
enum {
        GET_BACKLIGHT_LIMIT = B_DEVICE_OP_CODES_END + 20001,
        GET_BACKLIGHT_LEVEL,
        SET_BACKLIGHT_LEVEL
};

#endif
