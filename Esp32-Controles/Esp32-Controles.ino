#include <Bluepad32.h>
#include <uni.h>

static const char * controller_addr_string = "3C:FA:06:13:2B:B9";

ControllerPtr myControllers[1];

void onConnectedController(ControllerPtr ctl) {
    bool foundEmptySlot = false;
    for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
        if (myControllers[i] == nullptr) {
            Serial.printf("CALLBACK: Controller is connected, index=%d\n", i);
            ControllerProperties properties = ctl->getProperties();
            Serial.printf("Controller model: %s, VID=0x%04x, PID=0x%04x\n", ctl->getModelName().c_str(), properties.vendor_id,
                           properties.product_id);
            myControllers[i] = ctl;
            foundEmptySlot = true;
            break;
        }
    }
    if (!foundEmptySlot) {
        Serial.println("CALLBACK: Controller connected, but could not found empty slot");
    }
}

void onDisconnectedController(ControllerPtr ctl) {
    bool foundController = false;

    for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
        if (myControllers[i] == ctl) {
            Serial.printf("CALLBACK: Controller disconnected from index=%d\n", i);
            myControllers[i] = nullptr;
            foundController = true;
            break;
        }
    }

    if (!foundController) {
        Serial.println("CALLBACK: Controller disconnected, but not found in myControllers");
    }
}

void dumpGamepad(ControllerPtr ctl) {
    // Serial.printf(
    //     "idx=%d, dpad: 0x%02x, buttons: 0x%04x, axis L: %4d, %4d, axis R: %4d, %4d, brake: %4d, throttle: %4d, "
    //     "misc: 0x%02x, gyro x:%6d y:%6d z:%6d, accel x:%6d y:%6d z:%6d\n",
    //     ctl->index(),        // Controller Index
    //     ctl->dpad(),         // D-pad
    //     ctl->buttons(),      // bitmask of pressed buttons
    //     ctl->axisX(),        // (-511 - 512) left X Axis
    //     ctl->axisY(),        // (-511 - 512) left Y axis
    //     ctl->axisRX(),       // (-511 - 512) right X axis
    //     ctl->axisRY(),       // (-511 - 512) right Y axis
    //     ctl->brake(),        // (0 - 1023): brake button
    //     ctl->throttle(),     // (0 - 1023): throttle (AKA gas) button
    //     ctl->miscButtons(),  // bitmask of pressed "misc" buttons
    //     ctl->gyroX(),        // Gyro X
    //     ctl->gyroY(),        // Gyro Y
    //     ctl->gyroZ(),        // Gyro Z
    //     ctl->accelX(),       // Accelerometer X
    //     ctl->accelY(),       // Accelerometer Y
    //     ctl->accelZ()        // Accelerometer Z
    // );
}

void processGamepad(ControllerPtr ctl) {

    Serial.println(ctl->a());
  
    if (ctl->a()) {
        static int colorIdx = 0;
        switch (colorIdx % 3) {
            case 0:
                // Red
                ctl->setColorLED(255, 0, 0);
                break;
            case 1:
                // Green
                ctl->setColorLED(0, 255, 0);
                break;
            case 2:
                // Blue
                ctl->setColorLED(0, 0, 255);
                break;
        }
        colorIdx++;
    }

    if (ctl->b()) {
        static int led = 0;
        led++;
        ctl->setPlayerLEDs(led & 0x0f);
    }

    if (ctl->x()) {
        ctl->playDualRumble(0 /* delayedStartMs */, 250 /* durationMs */, 0x80 /* weakMagnitude */,
                            0x40 /* strongMagnitude */);
    }

    dumpGamepad(ctl);
}

void processControllers() {
    for (auto myController : myControllers) {
        if (myController && myController->isConnected() && myController->hasData()) {
            if (myController->isGamepad()) {
                processGamepad(myController);
            } else {
                Serial.println("Unsupported controller");
            }
        }
    }
}

void setup() {
    Serial.begin(115200);
    Serial.printf("Firmware: %s\n", BP32.firmwareVersion());
    const uint8_t* addr = BP32.localBdAddress();
    Serial.printf("BD Addr: %2X:%2X:%2X:%2X:%2X:%2X\n", addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);

    BP32.setup(&onConnectedController, &onDisconnectedController);

    BP32.forgetBluetoothKeys();

    BP32.enableVirtualDevice(false);

    bd_addr_t controller_addr;

    sscanf_bd_addr(controller_addr_string, controller_addr);

    uni_bt_allowlist_add_addr(controller_addr);

    uni_bt_allowlist_set_enabled(true);
}

void loop() {

    bool dataUpdated = BP32.update();
    if (dataUpdated)
        processControllers();

    delay(50);
}
