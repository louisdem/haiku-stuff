#ifndef AES_INPUT_DEVICE_H
#define AES_INPUT_DEVICE_H

#include "aes2501_common.h"

#include <add-ons/input_server/InputServerDevice.h>
#include <USBKit.h>

#include <Entry.h>
#include <driver_settings.h>

#define POLL_INTERVAL 1000

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
extern "C" status_t aes_usb_exec(status_t (*bulk_transfer)(int, unsigned char *, size_t),
	status_t (*clear_stall)(), bool strict, const pairs *cmd, unsigned int num);

enum {
	AES_FIRST_BUTTON = 1,
	AES_SECOND_BUTTON,
	AES_THIRD_BUTTON
};
enum {
	AES_DETECT_FINGER,
	AES_BREAK_LOOP
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
	thread_id EmulatedInterruptId;
public:
	AesInputDevice();
	virtual ~AesInputDevice();
private:
	void _ReadSettings();
	status_t InitThread();
	status_t EmulatedInterrupt();
	static status_t InitThreadProxy(void *_this)
	{
		AesInputDevice *dev = (AesInputDevice *) _this;
		return dev->InitThread();
	}
	static status_t EmulatedInterruptProxy(void *_this)
	{
		AesInputDevice *dev = (AesInputDevice *) _this;
		return dev->EmulatedInterrupt();
	}
	status_t aes_setup_pipes(const BUSBInterface *);
	status_t aes_usb_read(unsigned char* const, size_t);

	status_t bulk_transfer(int, unsigned char *, size_t);
	status_t clear_stall();
	static status_t g_bulk_transfer(int dir, unsigned char *dat, size_t s)
	{
		return InputDevice->bulk_transfer(dir, dat, s);
	}
	static status_t g_clear_stall(void)
	{
		return InputDevice->clear_stall();
	}
public:
	/* BInputServerDevice */
	virtual status_t InitCheck();
	virtual status_t Start(const char*, void*);
	virtual status_t Stop(const char*, void*);
	/* */

	friend class AesUSBRoster;
};

class AesUSBRoster: public BUSBRoster {
	virtual void DeviceRemoved(BUSBDevice *dev) {}; // no hotplug
public:
	virtual status_t DeviceAdded(BUSBDevice *dev);
};

#endif // AES_INPUT_DEVICE_H