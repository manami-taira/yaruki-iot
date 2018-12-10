#include <WioLTEforArduino.h>
#include <Ultrasonic.h>      // https://github.com/Seeed-Studio/Grove_Ultrasonic_Ranger
#include <stdio.h>
 
#define ULTRASONIC_PIN  (WIOLTE_D38)
#define INTERVAL  (1000)
 
Ultrasonic UltrasonicRanger(ULTRASONIC_PIN);
 
WioLTE Wio;
 
void setup() {
  delay(200);
 
  SerialUSB.println("");
  SerialUSB.println("___ START ___");
 
  SerialUSB.println("### I/O Initialize.");
  Wio.Init();
 
  SerialUSB.println("### Power supply ON.");
  Wio.PowerSupplyLTE(true);
  delay(5000);
 
  SerialUSB.println("### Turn on or reset.");
  if (!Wio.TurnOnOrReset()) {
    SerialUSB.println("### ERROR! ###");
    return;
  }
 
  SerialUSB.println("### Connecting to \"soracom.io\".");
  delay(5000);
  if (!Wio.Activate("soracom.io", "sora", "sora")) {
    SerialUSB.println("### ERROR! ###");
    return;
  }
}
 
void loop() {
  char mydata[100];
 
  long distance;
  distance = UltrasonicRanger.MeasureInCentimeters();
 
  SerialUSB.println("### Open.");
  int connectId = Wio.SocketOpen("harvest.soracom.io", 8514, WIOLTE_UDP);
  if (connectId < 0) {
    SerialUSB.println("### ERROR! ###");
    goto err;
  }
 
  SerialUSB.println("### Send.");
  sprintf(mydata, "{\"distance\":%ld}", distance);
  SerialUSB.print("Send:");
  SerialUSB.print(mydata);
  SerialUSB.println("");
  if (!Wio.SocketSend(connectId, mydata)) {
    SerialUSB.println("### ERROR! ###");
    goto err;
  }
 
  SerialUSB.println("### Receive.");
  int length;
  do {
    length = Wio.SocketReceive(connectId, mydata, sizeof (mydata));
    if (length < 0) {
      SerialUSB.println("### ERROR! ###");
      goto err;
    }
  } while (length == 0);
  SerialUSB.print("Receive:");
  SerialUSB.print(mydata);
  SerialUSB.println("");
 
  SerialUSB.println("### Close.");
  if (!Wio.SocketClose(connectId)) {
    SerialUSB.println("### ERROR! ###");
    goto err;
  }
 
err:
  delay(INTERVAL);
}
