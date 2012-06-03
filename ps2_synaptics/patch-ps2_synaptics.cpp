--- ps2_synaptics.cpp.orig	2012-05-31 11:43:35.539492352 +0400
+++ ps2_synaptics.cpp	2012-06-03 17:07:26.320077824 +0400
@@ -29,6 +29,13 @@
 #define REAL_MAX_PRESSURE		100
 #define MAX_PRESSURE			200
 
+//#undef TRACE(x...)
+//#define TRACE_PS2_SYNAPTICS 1
+#ifdef TRACE_PS2_SYNAPTICS
+#define TRACE(x...) dprintf(x)
+#else
+#define TRACE(x...)
+#endif
 
 static hardware_specs gHardwareSpecs;
 
@@ -57,6 +64,8 @@ send_touchpad_arg_timeout(ps2_dev *dev, 
 {
 	int8 i;
 	uint8 val[8];
+
+	ps2_dev_command_timeout(dev, 0xE6, NULL, 0, NULL, 0, 500000);
 	for (i = 0; i < 4; i++) {
 		val[2 * i] = (arg >> (6 - 2 * i)) & 3;
 		val[2 * i + 1] = 0xE8;
@@ -262,7 +271,7 @@ passthrough_command(ps2_dev *dev, uint8 
 		in[i - 1] = passThroughIn[i * 6 + 1];
 	}
 
-finalize:	
+finalize:
 	status_t statusOfEnable = ps2_dev_command(dev->parent_dev, PS2_CMD_ENABLE,
 			NULL, 0, NULL, 0);
 	if (statusOfEnable != B_OK)
@@ -278,7 +287,11 @@ probe_synaptics(ps2_dev *dev)
 	uint8 val[3];
 	uint8 deviceId;
 	status_t status;
-	TRACE("SYNAPTICS: probe\n");
+
+	dprintf("SYNAPTICS: probe\n");
+
+	// Required delay
+	snooze(400000);
 
 	status = send_touchpad_arg(dev, 0x00);
 	if (status != B_OK)
@@ -294,14 +307,14 @@ probe_synaptics(ps2_dev *dev)
 		return B_ERROR;
 	}
 
-	TRACE("SYNAPTICS: Touchpad found id:l %2x\n", deviceId);
+	dprintf("SYNAPTICS: Touchpad found id:l %2x\n", deviceId);
 	sTouchpadInfo.majorVersion = val[2] & 0x0F;
-	TRACE("SYNAPTICS: version %d.%d\n", sTouchpadInfo.majorVersion,
+	dprintf("SYNAPTICS: version %d.%d\n", sTouchpadInfo.majorVersion,
 		sTouchpadInfo.minorVersion);
 	// version >= 4.0?
 	if (sTouchpadInfo.minorVersion <= 2
 		&& sTouchpadInfo.majorVersion <= 3) {
-		TRACE("SYNAPTICS: too old touchpad not supported\n");
+		dprintf("SYNAPTICS: too old touchpad not supported\n");
 		return B_ERROR;
 	}
 	dev->name = kSynapticsPath[dev->idx];
