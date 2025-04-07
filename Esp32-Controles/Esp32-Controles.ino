#include <Bluepad32.h>
#include <uni.h>
#include <U8g2lib.h>

U8G2_SSD1306_128X32_UNIVISION_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);

static const char* controller_addr_string = "3C:FA:06:13:2B:B9";
GamepadPtr control = nullptr;

int cn_led = 2;
int zona_muerta = 50;

void onConnectedController(ControllerPtr ctl) {
  digitalWrite(cn_led, HIGH);
  Serial.println("Control conectado");
  control = ctl;

}

void onDisconnectedController(ControllerPtr ctl) {
  digitalWrite(cn_led, LOW);
  Serial.println("Control desconectado");
  control = nullptr;

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
  pinMode(cn_led, OUTPUT);

  u8g2.begin();

}

void loop() {
  BP32.update();
  u8g2.clearBuffer();
  u8g2.setFont(u8g_font_ncenB08);
  u8g2_uint_t ancho_texto;
  u8g2_uint_t alto_fuente = u8g2.getFontAscent() - u8g2.getFontDescent();
  u8g2_uint_t x;
  u8g2_uint_t y = (32 - alto_fuente) / 2 + u8g2.getFontAscent();
  char buffer[30];
  const char* texto = "Sin conexion";

  if (control && control->isConnected()) {
    int gatillo_lt = control->brake();
    int gatillo_rt = control->throttle();
    int joystick_lx = control->axisX();
    int joystick_ly = control->axisY();
    int joystick_rx = control->axisRX();
    int joystick_ry = control->axisRY();
    int btns = control->buttons();
    int dpad = control->dpad();
    int menu = control->miscButtons();

    if (btns == 1) texto = "Boton A";
    else if (btns == 2) texto = "Boton B";
    else if (btns == 8) texto = "Boton Y";
    else if (btns == 4) texto = "Boton X";
    else if (btns == 512) texto = "Boton CR";
    else if (btns == 256) texto = "Boton CL";
    else if (btns == 16) texto = "Boton LB";
    else if (btns == 32) texto = "Boton RB";
    else if (dpad == 2) texto = "Abajo";
    else if (dpad == 4) texto = "Derecha";
    else if (dpad == 1) texto = "Arriba";
    else if (dpad == 8) texto = "Izquierda";
    else if (menu == 1) texto = "Xbox";
    else if (menu == 2) texto = "Cuadros";
    else if (menu == 4) texto = "Rayas";
    else if (gatillo_lt > 0) {
      sprintf(buffer, "Boton LT: %d", map(gatillo_lt, 0, 1023, 0, 255));
      texto = buffer;

    } else if (gatillo_rt > 0) {
      sprintf(buffer, "Boton RT: %d", map(gatillo_rt, 0, 1023, 0, 255));
      texto = buffer;

    } else if (joystick_lx > zona_muerta) {
      sprintf(buffer, "Joystick L X: %d", map(joystick_lx, zona_muerta, 511, 0, 255));
      texto = buffer;

    } else if (joystick_lx < -(zona_muerta)) {
      sprintf(buffer, "Joystick L -X: %d", map(joystick_lx, -(zona_muerta), -512, 0, 255));
      texto = buffer;

    } else if (joystick_ly > zona_muerta) {
      sprintf(buffer, "Joystick L Y: %d", map(joystick_ly, zona_muerta, 511, 0, 255));
      texto = buffer;

    } else if (joystick_ly < -(zona_muerta)) {
      sprintf(buffer, "Joystick L -Y: %d", map(joystick_ly, -(zona_muerta), -512, 0, 255));
      texto = buffer;

    } else if (joystick_rx > zona_muerta) {
      sprintf(buffer, "Joystick R X: %d", map(joystick_rx, zona_muerta, 511, 0, 255));
      texto = buffer;

    } else if (joystick_rx < -(zona_muerta)) {
      sprintf(buffer, "Joystick R -X: %d", map(joystick_rx, -(zona_muerta), -512, 0, 255));
      texto = buffer;

    } else if (joystick_ry > zona_muerta) {
      sprintf(buffer, "Joystick R Y: %d", map(joystick_ry, zona_muerta, 511, 0, 255));
      texto = buffer;

    } else if (joystick_ry < -(zona_muerta)) {
      sprintf(buffer, "Joystick R -Y: %d", map(joystick_ry, -(zona_muerta), -512, 0, 255));
      texto = buffer;

    } 
    else texto = "Ninguno";

  }

  ancho_texto = u8g2.getStrWidth(texto);
  x = (128 - ancho_texto) / 2;
  u8g2.drawStr(x, y, texto);
  u8g2.sendBuffer();

  delay(50);

}
