/* Written by Artem Falcon <lomka@gero.in> */

#include "AesInputDevice.h"

const static char *kAesInputDirectory = "/dev/input/aes2501";
static AesInputDevice *InputDevice;

BInputServerDevice*
instantiate_input_device()
{
	return (InputDevice = new AesInputDevice());
}
AesInputDevice::AesInputDevice()
{
	this->device = NULL;

	this->URoster = NULL; settings = NULL;
}
AesInputDevice::~AesInputDevice()
{
	if (this->URoster)
		delete URoster;
	if (this->settings)
		free(settings);
}

status_t
AesInputDevice::InitCheck()
{
	BEntry entry(kAesInputDirectory);
	bigtime_t timeout = 1000, start;
	input_device_ref device = { "AuthenTec AES2501 USB", B_POINTING_DEVICE,
		(void *) this };
	input_device_ref *deviceList[] = { &device, NULL };

	// (f)open would be poor choice
	if (!entry.Exists()) {
		PRINT("Kernel-side driver didn't make device init for us\n");
		return B_ERROR;
	}

	this->URoster = new AesUSBRoster();
	URoster->Start();

	// as always :-!
	start = system_time();
	while(this->device == NULL) {
		if ((system_time() - start) > timeout) {
			debug_printf("aes2501: Supported device is missing. Unplugged?\n");

			return B_ERROR;
		}
		snooze(timeout);
	}

	if (!(this->settings = (AesSettings *) malloc(sizeof(AesSettings))))
		return B_ERROR;
	this->RegisterDevices(deviceList);
	return B_OK;
}
status_t
AesInputDevice::Start(const char *name, void *cookie)
{
	/* Would be nice to do:
		our_device->set_mouse_type(1); _/ */

	settings->which_button = AES_SECOND_BUTTON;
	this->_ReadSettings();

	/* \_ and:
		if (settings->handle_click == true) {
			map.button[0] = (settings->which_button == AES_SECOND_BUTTON) ?
				B_TERTIARY_MOUSE_BUTTON : to_mouseview_name(settings->which_button);
			our_device->set_mouse_map(&map);
		}
		but Haiku allows only global mouse settings */

	return B_OK; // 'll be ignored
}

status_t
AesUSBRoster::DeviceAdded(BUSBDevice *dev)
{
	if (dev->VendorID() == AES_VID && dev->ProductID() == AES_PID) {
		InputDevice->device = dev;
		return B_OK;
	}

	return B_ERROR;
}

/*
void c------------------------------() {}
*/

void AesInputDevice::_ReadSettings()
{
	void *handle;

	// Driver Settings API isn't right to be used here, but whatever...
	if ((handle = load_driver_settings("aes2501"))) {
		const char *str = get_driver_parameter(handle, "bind_to_which_button", NULL, NULL);
		if (str && (str[0] == '1' || str[0] == '2' || str[0] == '3'))
			this->settings->which_button = atoi(str);
	}
	settings->handle_click = get_driver_boolean_parameter(handle, "handle_click", true, true);
	settings->handle_scroll = get_driver_boolean_parameter(handle, "handle_scroll", true, true);
	unload_driver_settings(handle);
}
