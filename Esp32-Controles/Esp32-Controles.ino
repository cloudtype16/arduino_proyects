#include <Bluepad32.h>

ControllerPtr myControllers[BP32_MAX_GAMEPADS];

int CN = 2;

// Configuración inicial de Arduino.
void setup() {
  Serial.begin(115200);
  Serial.printf("Firmware: %s\n", BP32.firmwareVersion());
  const uint8_t* addr = BP32.localBdAddress();
  Serial.printf("BD Addr: %2X:%2X:%2X:%2X:%2X:%2X\n", addr[0], addr[1], addr[2], addr[3], addr[4], addr[5]);

  // Configura los callbacks de Bluepad32.
  BP32.setup(&onConnectedController, &onDisconnectedController);

  // Opcional: olvidando claves de Bluetooth.
  BP32.forgetBluetoothKeys();

  pinMode(CN, OUTPUT);
  digitalWrite(CN, LOW);
}

// Función para procesar el estado del Joy-Con.
void processJoyCon(ControllerPtr ctl) {

  Serial.printf(
    "idx=%d, dpad: 0x%02x, buttons: 0x%04x, axis L: %4d, %4d, axis R: %4d, %4d, brake: %4d, throttle: %4d, "
    "misc: 0x%02x, gyro x:%6d y:%6d z:%6d, accel x:%6d y:%6d z:%6d\n",
    ctl->index(),        // Controller Index
    ctl->dpad(),         // D-pad
    ctl->buttons(),      // bitmask of pressed buttons
    ctl->axisX(),        // (-511 - 512) left X Axis
    ctl->axisY(),        // (-511 - 512) left Y axis
    ctl->axisRX(),       // (-511 - 512) right X axis
    ctl->axisRY(),       // (-511 - 512) right Y axis
    ctl->brake(),        // (0 - 1023): brake button
    ctl->throttle(),     // (0 - 1023): throttle (AKA gas) button
    ctl->miscButtons(),  // bitmask of pressed "misc" buttons
    ctl->gyroX(),        // Gyro X
    ctl->gyroY(),        // Gyro Y
    ctl->gyroZ(),        // Gyro Z
    ctl->accelX(),       // Accelerometer X
    ctl->accelY(),       // Accelerometer Y
    ctl->accelZ()        // Accelerometer Z
  );
}

// Bucle principal de Arduino.
void loop() {
  if (BP32.update()) {
    processControllers();
  }

  // Agregar un pequeño retraso para evitar que el watchdog se dispare.
  delay(150);
}

// Callback para cuando se conecta un controlador.
void onConnectedController(ControllerPtr ctl) {
  for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
    if (myControllers[i] == nullptr) {
      digitalWrite(CN, HIGH);
      Serial.printf("Control conectado, índice=%d\n", i);
      myControllers[i] = ctl;
      break;
    }
  }
}

// Callback para cuando se desconecta un controlador.
void onDisconnectedController(ControllerPtr ctl) {
  for (int i = 0; i < BP32_MAX_GAMEPADS; i++) {
    if (myControllers[i] == ctl) {
      digitalWrite(CN, LOW);
      Serial.printf("Control desconectado del índice=%d\n", i);
      myControllers[i] = nullptr;
      break;
    }
  }
}

void processControllers() {
  for (auto myController : myControllers) {
    if (myController && myController->isConnected() && myController->hasData() && myController->isGamepad()) {
      processJoyCon(myController);
    }
  }
}
