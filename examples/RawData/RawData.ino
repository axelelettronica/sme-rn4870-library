/*
  RawData.ino

  Example demonstrating a BLE Bidirectional communication between a PC/Smartphone App (Central BLE device)
  and the SmartEveryting USB console (Peripheral BLE)

  - Serial -> BLE : Any character edited on the USB Arduino console will be sent via BLE  
                    on the attribute 0xFFF4 to the central device.
                    NOTICE: The Host needs to subscribe to 0xFFF4 attribute to get notified.

  - BLE -> Serial : Any character written by the Central device, on the writable attribute 0xFFF3, 
                    is showed on the Arduino console.
                    NOTICE: The protocol to be used is <len> <char 1> <char 2> ... <char len>

  Created: 07/07/2016 10:32:11 PM

  Author: development@axelelettronica.it

 */
#include <Arduino.h>
#include "rn4870.h"

static void ledOn(char led, bool ligthOn) {
#ifdef ARDUINO_SAMD_SMARTEVERYTHING
    if (led)
        ledYellowOneLight(ligthOn);
    else
        ledYellowOneLight(ligthOn);
#else
    if (led) {
        digitalWrite(PIN_LED, ligthOn);
    }    
#endif
}

bool ledGLigth = false;
bool ledBLigth = false;
void setup() {
    SerialUSB.begin(115200);
    ble_rn4870.begin(bufferAnswer, sizeof(bufferAnswer));

#ifndef ARDUINO_SAMD_SMARTEVERYTHING
    pinMode(PIN_LED, OUTPUT);
#endif
    ledOn(0, ledGLigth);
    ledGLigth = !ledGLigth;

    ledOn(1, ledBLigth);
    ledBLigth = !ledBLigth;

    while (!SerialUSB) {
        ;
    }
    SerialUSB.println("Started!!");

    ble_rn4870.write('$');
    delay(20);
    ble_rn4870.write('$');
    delay(20);
    ble_rn4870.write('$');
}

void loop() {
    char readData;
    while (SerialUSB.available()) {
        readData = SerialUSB.read();
        ble_rn4870.write(readData);
        ledOn(0, ledGLigth);
        ledGLigth = !ledGLigth;
    }

    while (ble_rn4870.available()) {
        readData = Serial1.read();
        SerialUSB.write(readData);
        ledOn(1, ledBLigth);
        ledBLigth = !ledBLigth;
    }

}