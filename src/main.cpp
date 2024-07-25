#include <M5AtomS3.h>
#include "USB.h"
#include "USBHIDConsumerControl.h"

#define GPIO_BUTTON 41
#define POLLING_INTERVAL_MS 20 // 50Hz
#define CHATTERING_INTERVAL_MS 1000
USBHIDConsumerControl ConsumerControl;

volatile bool g_irq0 = false;
void setIRQ0() {
  g_irq0 = true;
}

void setup() {
  AtomS3.begin(true);
  ConsumerControl.begin();
  USB.begin();
  
  pinMode(GPIO_BUTTON, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(GPIO_BUTTON), setIRQ0, FALLING);
  AtomS3.dis.drawpix(CRGB::Black);
  AtomS3.dis.show();
}

void loop() {
  static int prev_ms = 0;

  delay(POLLING_INTERVAL_MS);

  if(!g_irq0){
    return;
  }

  const int curr_ms = millis();
  const int spent_ms = curr_ms >= prev_ms ? curr_ms - prev_ms : INT_MAX - prev_ms + curr_ms;
  if(spent_ms < CHATTERING_INTERVAL_MS){
    return;
  }

  ConsumerControl.press(CONSUMER_CONTROL_VOLUME_INCREMENT);
  ConsumerControl.release();

  prev_ms = curr_ms;
  g_irq0 = false;
  return;
}
