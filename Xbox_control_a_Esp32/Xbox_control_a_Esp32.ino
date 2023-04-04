#include <XboxSeriesXControllerESP32_asukiaaa.hpp>

XboxSeriesXControllerESP32_asukiaaa::Core xboxController;

int CN = 2;

int LED = 15;

void setup() {

  Serial.begin(115200);
  xboxController.begin();

  pinMode(CN, OUTPUT);
  digitalWrite(CN, LOW);
}

void loop() {
  xboxController.onLoop();
  // Serial.println("battery " + String(xboxController.battery) + "%");

  if (xboxController.isConnected()) {
    digitalWrite(CN, HIGH);

    if(xboxController.xboxNotif.btnA){
      digitalWrite(LED, HIGH);
    } else {
      digitalWrite(LED, LOW);
    }
  } else {
    digitalWrite(CN, LOW);
    Serial.println("not connected");

    if (xboxController.getCountFailedConnection() > 2) {
      ESP.restart();
    }
  }
}
