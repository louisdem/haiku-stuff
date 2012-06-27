#ifndef AES_COMMON_H
#define AES_COMMON_H

#include <stdio.h>
#include <stdlib.h>

#include <SupportDefs.h>

#define AES_VID 0x08ff
#define AES_PID 0x2500

typedef struct { int reg, value; } pairs;

enum aes2501_regs_common {
	AES2501_REG_CTRL1 = 0x80,
#define AES2501_CTRL1_MASTER_RESET	(1<<0)
#define AES2501_CTRL1_SCAN_RESET	(1<<1) /* stop + restart scan sequencer */
	AES2501_REG_EXCITCTRL = 0x82, /* excitation control */
};

#endif // AES_COMMON_H