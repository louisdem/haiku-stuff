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
	const BUSBConfiguration *conf;
	bigtime_t timeout = 1000, start;
	input_device_ref dev = { "AuthenTec AES2501 USB", B_POINTING_DEVICE,
		(void *) this };
	input_device_ref *deviceList[] = { &dev, NULL };

	const pairs start_scan_cmd[] = {
	{ 0xb0, 0x27 },
	{ AES2501_REG_CTRL1, AES2501_CTRL1_MASTER_RESET },
	{ AES2501_REG_EXCITCTRL, 0x40 },
	{ 0xff, 0x00 },
	{ AES2501_REG_CTRL1, AES2501_CTRL1_MASTER_RESET },
	{ AES2501_REG_EXCITCTRL, 0x40 },
	{ AES2501_REG_CTRL1, AES2501_CTRL1_MASTER_RESET },
	{ AES2501_REG_EXCITCTRL, 0x40 },
	{ AES2501_REG_CTRL1, AES2501_CTRL1_MASTER_RESET },
	{ AES2501_REG_EXCITCTRL, 0x40 },
	{ AES2501_REG_CTRL1, AES2501_CTRL1_MASTER_RESET },
	{ AES2501_REG_EXCITCTRL, 0x40 },
	{ AES2501_REG_CTRL1, AES2501_CTRL1_MASTER_RESET },
	{ AES2501_REG_EXCITCTRL, 0x40 },
	{ AES2501_REG_CTRL1, AES2501_CTRL1_SCAN_RESET },
	{ AES2501_REG_CTRL1, AES2501_CTRL1_SCAN_RESET },
	};

	// (f)open would be poor choice
	if (!entry.Exists()) {
		PRINT("Kernel-side driver didn't make device init for us\n");
		return B_ERROR;
	}

	this->URoster = new AesUSBRoster();

	/* dupe: We're doing a duplicate of what kernel driver did in it's init phase,
	   because of two reasons: 1) someone may wish to adopt this project for
	   hot swapping support, 2) i don't want to mess with creating of shared memory
	   area region and passing parms from kernel driver to userspace one */
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

	if (this->device->SetConfiguration(conf = device->ConfigurationAt(0)) < B_OK) {
		PRINT("can't set configuration\n");
		return B_ERROR;
	}
	if (aes_setup_pipes(conf->InterfaceAt(0)) != B_OK) {
		PRINT("can't setup pipes\n");
		return B_ERROR;
	}
	// end of dupe

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

status_t AesInputDevice::aes_setup_pipes(const BUSBInterface *uii)
{
	size_t ep = 0;
	BUSBEndpoint *epts[] = { NULL, NULL };

	for (; ep < uii->CountEndpoints(); ep++) {
		BUSBEndpoint *ed = (BUSBEndpoint *) uii->EndpointAt(ep);
		if (ed->IsBulk())
			(ed->IsInput()) ? (epts[0] = ed) : (epts[1] = ed);
	}

	this->dev_data.pipe_in = epts[0];
	dev_data.pipe_out = epts[1];

	PRINT("endpoint is: %d %d\n", dev_data.pipe_in->Index(), dev_data.pipe_out->Index());

	return epts[0] && epts[1] ? B_OK : B_ENTRY_NOT_FOUND;
}