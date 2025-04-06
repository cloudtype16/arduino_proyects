#include <Bluepad32.h>
#include <uni.h>

static const char* controller_addr_string = "3C:FA:06:13:2B:B9";

GamepadPtr myGamepad = nullptr;

void onConnectedController(ControllerPtr ctl) {
  Serial.println("Gamepad conectado");
  myGamepad = ctl;
}

void onDisconnectedController(ControllerPtr ctl) {
  Serial.println("Gamepad desconectado");
  myGamepad = nullptr;
}

void setup() {
  Serial.begin(115200);
  Serial.println("Iniciando Bluepad32...");

  BP32.setup(&onConnectedController, &onDisconnectedController);
  BP32.enableNewBluetoothConnections(true); 

  bd_addr_t controller_addr;

  sscanf_bd_addr(controller_addr_string, controller_addr);

  uni_bt_allowlist_add_addr(controller_addr);

  uni_bt_allowlist_set_enabled(true);
}

void loop() {

  BP32.update();

  if (myGamepad && myGamepad->isConnected()) {
    // Leer los valores de botones
    if (myGamepad->a()) Serial.println("Botón A presionado");
    if (myGamepad->b()) Serial.println("Botón B presionado");
    if (myGamepad->x()) Serial.println("Botón X presionado");
    if (myGamepad->y()) Serial.println("Botón Y presionado");

    Serial.print("Joystick izquierdo: X=");
    Serial.print(myGamepad->axisX());
    Serial.print(" Y=");
    Serial.println(myGamepad->axisY());

    delay(100);  
  }
}
