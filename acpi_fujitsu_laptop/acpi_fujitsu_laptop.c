#include <KernelExport.h>
#include <Drivers.h>
#include <Errors.h>
#include <string.h>

#include <stdio.h>
#include <stdlib.h>

#include <ACPI.h>

#define TRACE_FJL 1
#ifdef TRACE_FJL
#       define TRACE(x) dprintf("acpi_fujitsu_laptop: "x)
#else
#       define TRACE(x) ;
#endif

#define ACPI_FJL_MODULE_NAME "drivers/power/acpi_fujitsu_laptop/driver_v1"
#define ACPI_FJL_DEVICE_MODULE_NAME "drivers/power/acpi_fujitsu_laptop/device_v1"

/* Base Namespace devices are published to */
#define ACPI_FJL_BASENAME "power/acpi_fujitsu_laptop/%d"

// name of pnp generator of path ids
#define ACPI_FJL_PATHID_GENERATOR "acpi_fujitsu_laptop/path_id"

static device_manager_info *sDeviceManager;

typedef struct acpi_ns_device_info {
	device_node *node;
	acpi_device_module_info *acpi;
	acpi_device acpi_cookie;
} acpi_fjl_device_info;

/* bypass device/driver separation */
struct {
	acpi_module_info *acpi;
	const char *full_path;
	char *what;
} acpi_fujitsu;

static bool fjl_hw_set_lcd_level(int);

//	#pragma mark - device module API


static status_t
acpi_fjl_init_device(void *_cookie, void **cookie)
{
	device_node *node = (device_node *)_cookie;
	acpi_fjl_device_info *device;
	device_node *parent;

	device = (acpi_fjl_device_info *)calloc(1, sizeof(*device));
	if (device == NULL)
		return B_NO_MEMORY;

	device->node = node;

	parent = sDeviceManager->get_parent_node(node);
	sDeviceManager->get_driver(parent, (driver_module_info **)&device->acpi,
		(void **)&device->acpi_cookie);
	sDeviceManager->put_node(parent);

	*cookie = device;
	return B_OK;
}


static void
acpi_fjl_uninit_device(void *_cookie)
{
	acpi_fjl_device_info *device = (acpi_fjl_device_info *)_cookie;
	free(device);
}


static status_t
acpi_fjl_open(void *_cookie, const char *path, int flags, void** cookie)
{
	acpi_fjl_device_info *device = (acpi_fjl_device_info *)_cookie;
	*cookie = device;
	return B_OK;
}


static status_t
acpi_fjl_read(void* _cookie, off_t position, void *buf, size_t* num_bytes)
{
	return B_ERROR;
}


static status_t
acpi_fjl_write(void* cookie, off_t position, const void* buffer, size_t* num_bytes)
{
	return B_ERROR;
}


static status_t
acpi_fjl_control(void* _cookie, uint32 op, void* arg, size_t len)
{
	//acpi_fjl_device_info* device = (acpi_fjl_device_info*)_cookie;

	return B_ERROR;
}


static status_t
acpi_fjl_close (void* cookie)
{
	return B_OK;
}


static status_t
acpi_fjl_free (void* cookie)
{
	return B_OK;
}


//	#pragma mark - driver module API


static float
acpi_fjl_support(device_node *parent)
{
	const char *bus;
	uint32 device_type;
	const char *path;

	// make sure parent is really the ACPI bus manager
	if (sDeviceManager->get_attr_string(parent, B_DEVICE_BUS, &bus, false))
		return -1;

	if (strcmp(bus, "acpi"))
		return 0.0;

	// check whether it's really a device
	if (sDeviceManager->get_attr_uint32(parent, ACPI_DEVICE_TYPE_ITEM, &device_type, false) != B_OK
		|| device_type != ACPI_TYPE_DEVICE) {
		return 0.0;
	}

	// check whether it's our device
	if (sDeviceManager->get_attr_string(parent, /*ACPI_DEVICE_HID_ITEM*/ ACPI_DEVICE_PATH_ITEM
		, &path, false) == B_OK
			// parsing of string-valued HID is broken on Haiku, at least with my laptop :(
			/*|| strcmp(hid, "FUJ02B1")*/) {
		//dprintf("acpi_fujitsu_laptop: ACPI path: %s\n", path);

		if (strstr(path, "FJEX")) {
			TRACE("Found supported device\n");

			acpi_fujitsu.full_path = path;
			return 0.6;
		}
	}

	return 0.0;
}


static status_t
acpi_fjl_register_device(device_node *node)
{
	device_attr attrs[] = {
		{ B_DEVICE_PRETTY_NAME, B_STRING_TYPE, { string: "ACPI Fujitsu-Siemens Laptop driver" }},
		{ NULL }
	};

	return sDeviceManager->register_node(node, ACPI_FJL_MODULE_NAME, attrs, NULL, NULL);
}


static status_t
acpi_fjl_init_driver(device_node *node, void **_driverCookie)
{
	char *prefix;
	TRACE("init_driver()\n");

	*_driverCookie = node;

	prefix = (char *) calloc(1, strlen(acpi_fujitsu.full_path) + 6);
	/* path prefix, required cause of lack of cookie?! */
	sprintf(prefix, "%s%s", acpi_fujitsu.full_path, ".ABCD");
	acpi_fujitsu.what = strstr(prefix, "ABCD");
	acpi_fujitsu.full_path = prefix;

	fjl_hw_set_lcd_level(255);

	return B_OK;
}


static void
acpi_fjl_uninit_driver(void *driverCookie)
{
}


static status_t
acpi_fjl_register_child_devices(void *_cookie)
{
	device_node *node = _cookie;
	int path_id;
	char name[128];

	TRACE("register_child_devices()\n");

	path_id = sDeviceManager->create_id(ACPI_FJL_PATHID_GENERATOR);
	if (path_id < 0) {
		TRACE("register_child_devices(): couldn't create a path_id\n");
		return B_ERROR;
	}

	snprintf(name, sizeof(name), ACPI_FJL_BASENAME, path_id);

	return sDeviceManager->publish_device(node, name, ACPI_FJL_DEVICE_MODULE_NAME);
}


module_dependency module_dependencies[] = {
	{ B_DEVICE_MANAGER_MODULE_NAME, (module_info **)&sDeviceManager },
	{ B_ACPI_MODULE_NAME, (module_info **)&acpi_fujitsu.acpi },
	{}
};


driver_module_info acpi_fjl_driver_module = {
	{
		ACPI_FJL_MODULE_NAME,
		0,
		NULL
	},

	acpi_fjl_support,
	acpi_fjl_register_device,
	acpi_fjl_init_driver,
	acpi_fjl_uninit_driver,
	acpi_fjl_register_child_devices,
	NULL,	// rescan
	NULL,	// removed
};


struct device_module_info acpi_fjl_device_module = {
	{
		ACPI_FJL_DEVICE_MODULE_NAME,
		0,
		NULL
	},

	acpi_fjl_init_device,
	acpi_fjl_uninit_device,
	NULL,

	acpi_fjl_open,
	acpi_fjl_close,
	acpi_fjl_free,
	acpi_fjl_read,
	acpi_fjl_write,
	NULL,
	acpi_fjl_control,

	NULL,
	NULL
};

module_info *modules[] = {
	(module_info *)&acpi_fjl_driver_module,
	(module_info *)&acpi_fjl_device_module,
	NULL
};

/*
void c------------------------------() {}
*/

static bool fjl_hw_set_lcd_level(int level)
{
	acpi_handle handle = NULL;
	acpi_object_type arg0;
	acpi_objects arg_list;
	arg0.object_type = ACPI_TYPE_INTEGER;
	arg0.data.integer = level;
	arg_list.count = 1;
	arg_list.pointer = &arg0;

	strcpy(acpi_fujitsu.what, "SBLL");
	dprintf("acpi_fujitsu_laptop: fjl_hw_set_lcd_level(level = %d"/*, path = %s*/")\n", level
		/*,acpi_fujitsu.full_path*/);

	if (acpi_fujitsu.acpi->get_handle(NULL, acpi_fujitsu.full_path, &handle) != B_OK) {
		TRACE("SBLL wasn't found\n");
		return false;
	}

	if (acpi_fujitsu.acpi->evaluate_method(handle, NULL, &arg_list, NULL) != B_OK)
		return false;

	return true;
}