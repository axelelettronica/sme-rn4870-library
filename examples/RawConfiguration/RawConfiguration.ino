/*
 * RawConfiguration.ino
 *
 * Example that send every char received from the USB port to the ble.
 * Set the SerialMonitor to send every char without CR/LF.
 * To end the command the user shall type the '.' (dot) char
 * 
 * All the possible commands are visible on the following document wrote by Microchip.
 * User Guide = "http://ww1.microchip.com/downloads/en/DeviceDoc/50002466A.pdf"
 *
 *
 * Author: development@axelelettronica.it
 *
 */
#include <Arduino.h>
#include "rn4870.h"

bool ledLigth = false;
char bufferAnswer[100];
char bufferSend[100];
bool sendCmd = true;
char ptrData = 0;

static void ledOn(bool ligthOn) {
#ifdef ARDUINO_SAMD_SMARTEVERYTHING
    ledYellowOneLight(ligthOn);
#else
    pinMode(PIN_LED, OUTPUT);
    digitalWrite(PIN_LED, ligthOn);
#endif
}


void setup() {
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

    if (!ble_rn4870.enterConfigurationMode()) {
        SerialUSB.print("Cannot ");
    }
    SerialUSB.println("Start in configuration mode");

}

void loop() {
    char data;
    char readData = 0;

    if (sendCmd) {
        // compose the command to send
        if (SerialUSB.available()) {
            data = SerialUSB.read();
            if (data != '.'){
                bufferSend[ptrData++] = data;
            } else {
                ptrData = 0; // reset ptr for next command
                ble_rn4870.rawConfiguration(bufferSend);
                sendCmd = false;
            }
        }
    } else {
        // collect the Answer from BLE
        if (ble_rn4870.hasAnswer()==completeAnswer) {
            SerialUSB.write(ble_rn4870.getLastAnswer());

            // clear buffer
            memset(bufferSend, 0, sizeof(bufferSend));
            sendCmd = true;
        }
    }
}
