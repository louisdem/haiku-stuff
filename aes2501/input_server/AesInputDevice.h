#ifndef AES_INPUT_DEVICE_H
#define AES_INPUT_DEVICE_H

#include "aes2501_common.h"

#include <Entry.h>
#include <driver_settings.h>
#include <add-ons/input_server/InputServerDevice.h>
#include <USBKit.h>

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
#	define PRINT(x)	_iprint("aes2501: " x)
#else
#	define PRINT(x) ;
#endif

extern "C" _EXPORT BInputServerDevice* instantiate_input_device();

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
class AesInputDevice: public BInputServerDevice {
	class AesUSBRoster *URoster;

	BUSBDevice *device;
	AesSettings *settings;
public:
	AesInputDevice();
	virtual ~AesInputDevice();
private:
	void _ReadSettings();
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

#endif // AES_INPUT_DEVICE_H