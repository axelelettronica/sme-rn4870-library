/*
  SerialCommunication.ino

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

bool ledLigth = false;
char bufferAnswer[100];

static void ledOn(bool ligthOn) {
#ifdef ARDUINO_SAMD_SMARTEVERYTHING
    ledYellowOneLight(ligthOn);
#else
    pinMode(PIN_LED, OUTPUT);
    digitalWrite(PIN_LED, ligthOn);
#endif
}

const char *mac;
void setup() {
    int i =0;
    SerialUSB.begin(115200);

#ifndef ARDUINO_SAMD_SMARTEVERYTHING
    pinMode(PIN_LED, OUTPUT);
#endif

    ble_rn4870.begin(bufferAnswer, sizeof(bufferAnswer));

    delay(100);
    while (!SerialUSB) {
        ;
    }

    ledOn(ledLigth);
    ledLigth = !ledLigth;

    if (!ble_rn4870.startBLE()) {
        SerialUSB.println("Cannot Start the BLE");
    } else {
        SerialUSB.print("Start the BLE with ");
        mac = ble_rn4870.getAddress();
        for (i=0; i<5; i++){
            SerialUSB.print(mac[i], HEX);
            SerialUSB.print('-');
        }        
        SerialUSB.print(mac[5], HEX);

        SerialUSB.println(" address");

    }

}

void loop() {
    char data;
    char readData = 0;

    // compose the command to send
    if (SerialUSB.available()) {
        data = SerialUSB.read();
        ble_rn4870.sendData(&data, 1);
    }

    // collect the Answer from BLE
    if (ble_rn4870.hasAnswer()==dataAnswer) {
        SerialUSB.write(ble_rn4870.getLastAnswer());
    }
}
