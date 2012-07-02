#ifndef AES_INPUT_DEVICE_H
#define AES_INPUT_DEVICE_H

#include "aes2501_common.h"

#include <add-ons/input_server/InputServerDevice.h>
#include <USBKit.h>

#include <Entry.h>
#include <driver_settings.h>

#define PRINT_AES 1
#ifdef PRINT_AES
	inline void _iprint(const char* fmt, ...) {
		FILE* log = fopen("/var/log/aes2501.log", "a");
		va_list ap;
		va_start(ap, fmt);
		vfprintf(log, fmt, ap);
		va_end(ap);
		fflush(log);
		fclose(log);
       }
#	define PRINT(x...)	_iprint (x)
#else
#	define PRINT(x...) ;
#endif

extern "C" _EXPORT BInputServerDevice* instantiate_input_device();
extern "C" status_t aes_usb_exec(status_t (*bulk_transfer)(unsigned char *, size_t),
	status_t (*clear_stall)(), bool strict, const pairs *cmd, unsigned int num);

enum {
	AES_FIRST_BUTTON = 1,
	AES_SECOND_BUTTON,
	AES_THIRD_BUTTON
};
typedef struct {
	bool handle_click,
		 handle_scroll;
	int which_button;
} AesSettings;

class AesInputDevice;
static AesInputDevice *InputDevice;
class AesInputDevice: public BInputServerDevice {
	class AesUSBRoster *URoster;

	BUSBDevice *device;
	struct {
		BUSBEndpoint *pipe_in,
					 *pipe_out;
	} dev_data;
	AesSettings *settings;
public:
	AesInputDevice();
	virtual ~AesInputDevice();
private:
	void _ReadSettings();
	status_t aes_setup_pipes(const BUSBInterface *);
	status_t InitThread();
	static status_t InitThreadProxy(void *_this)
	{
		AesInputDevice *dev = (AesInputDevice *) _this;
		return dev->InitThread();
	}

	status_t bulk_transfer(unsigned char *, size_t);
	status_t clear_stall();
	static status_t g_bulk_transfer(unsigned char *d, size_t s)
	{
		return InputDevice->bulk_transfer(d, s);
	}
	static status_t g_clear_stall(void)
	{
		return InputDevice->clear_stall();
	}
public:
	/* BInputServerDevice */
	virtual status_t InitCheck();
	virtual status_t Start(const char*, void*);
	/* */

	friend class AesUSBRoster;
};

class AesUSBRoster: public BUSBRoster {
	virtual void DeviceRemoved(BUSBDevice *dev) {}; // no hotplug
public:
	virtual status_t DeviceAdded(BUSBDevice *dev);
};

/*
void c------------------------------() {}
*/

/* protos */
class finger_det_cmd {
public:
	const pairs *cmds;
	finger_det_cmd() {
		const pairs finger_det_cmd[] = {
	{ AES2501_REG_CTRL1, AES2501_CTRL1_MASTER_RESET },
	{ AES2501_REG_EXCITCTRL, 0x40 },
	{ AES2501_REG_DETCTRL,
		AES2501_DETCTRL_DRATE_CONTINUOUS | AES2501_DETCTRL_SDELAY_31_MS },
	{ AES2501_REG_COLSCAN, AES2501_COLSCAN_SRATE_128_US },
	{ AES2501_REG_MEASDRV, AES2501_MEASDRV_MDRIVE_0_325 | AES2501_MEASDRV_MEASURE_SQUARE },
	{ AES2501_REG_MEASFREQ, AES2501_MEASFREQ_2M },
	{ AES2501_REG_DEMODPHASE1, DEMODPHASE_NONE },
	{ AES2501_REG_DEMODPHASE2, DEMODPHASE_NONE },
	{ AES2501_REG_CHANGAIN,
		AES2501_CHANGAIN_STAGE2_4X | AES2501_CHANGAIN_STAGE1_16X },
	{ AES2501_REG_ADREFHI, 0x44 },
	{ AES2501_REG_ADREFLO, 0x34 },
	{ AES2501_REG_STRTCOL, 0x16 },
	{ AES2501_REG_ENDCOL, 0x16 },
	{ 0xff, 0x00 }, //{...image data format...},
	{ AES2501_REG_TREG1, 0x70 },
	{ 0xa2, 0x02 },
	{ 0xa7, 0x00 },
	{ AES2501_REG_TREGC, AES2501_TREGC_ENABLE },
	{ AES2501_REG_TREGD, 0x1a },
	{ 0, 0 },
	{ AES2501_REG_CTRL1, AES2501_CTRL1_REG_UPDATE },
	{ AES2501_REG_CTRL2, AES2501_CTRL2_SET_ONE_SHOT },
	{ AES2501_REG_LPONT, AES2501_LPONT_MIN_VALUE },
		};
		cmds = &finger_det_cmd[0];
	};
};
/* */

#endif // AES_INPUT_DEVICE_H