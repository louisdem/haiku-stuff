--- /boot/home/src/src/add-ons/kernel/bus_managers/ps2/ps2_dev.cpp.orig	2012-05-31 11:10:11.697303040 +0400
+++ /boot/home/src/src/add-ons/kernel/bus_managers/ps2/ps2_dev.cpp	2012-05-31 11:12:51.128712704 +0400
@@ -22,6 +22,15 @@
 
 #include <string.h>
 
+//#undef TRACE(x...)
+//#define TRACE_PS2_DEV 1
+#ifdef TRACE_PS2_DEV
+#define TRACE(x...) dprintf(x)
+#else
+#define TRACE(x...)
+#endif
+
+#define SYNAPTICS_HACK 1
 
 ps2_dev ps2_device[PS2_DEVICE_COUNT];
 
@@ -76,6 +85,15 @@ ps2_dev_detect_pointing(ps2_dev *dev, de
 		goto dev_found;
 	}
 
+#ifdef SYNAPTICS_HACK
+	// Synaptics doesn't bear mess from probe_trackpoint()
+	status = ps2_reset_mouse(dev);
+	if (status != B_OK) {
+		INFO("ps2: reset failed\n");
+		return B_ERROR;
+	}
+#endif
+
 	status = probe_synaptics(dev);
 	if (status == B_OK) {
 		*hooks = &gSynapticsDeviceHooks;
 
