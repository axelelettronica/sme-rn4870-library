/*
 * ConfiguratioinExample.ino
 *
 * Example that shows some API used to retrieve the BLE configuration
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
char bufferParam[100];
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
        SerialUSB.print("Cannot enter in Configuration mode");
        while(1){};
    }

    SerialUSB.println("-----------------------------------------");
    SerialUSB.println("  ---   Configure Your BLE RN4870  ---   ");
    SerialUSB.println("-----------------------------------------");
    SerialUSB.println("1) Set BLE Name");
    SerialUSB.println("2) Set Prompt command <pre>,<post> (default is %,%)");

    /*
    00 921600
    01 460800
    02 230400
    03 115200
    04 57600
    05 38400
    06 28800
    07 19200
    08 14400
    09 9600
    0A 4800
    0B 2400
     */
    SerialUSB.println("3) Set micro to micro uart baud-rate");
    SerialUSB.println("4) Set Power Save mode (1=enable; 2=disable)");
    SerialUSB.println("5) Get Fw, Hw, SW version & Serial Number");
    SerialUSB.println("6) Get Manufacture");
    SerialUSB.println("7) Get Name");
    SerialUSB.println("8) Get uart baud-rate");
    SerialUSB.println("9) Get Power Save mode");
    SerialUSB.println("A) Factory Reset !! USE IT CAREFULLY !! ");
    SerialUSB.println("-----------------------------------------");


}


static boolean composeParam(void) {
    char data;
    if (SerialUSB.available()) {
        data = SerialUSB.read();
        if (data != '.'){
            bufferParam[ptrData++] = data;
            return false;
        } else {
            bufferParam[ptrData++] = 0; //end the string
            return true;
        }
    }
    return false;
}


void loop() {
    char data;
    char readData = 0;
    bool paramCompleted = false;
    boolean activated=false;


    if (SerialUSB.available()) {
        data = SerialUSB.read();

        switch (data) {
        case '1':
            do {
                paramCompleted = composeParam();
            } while (!paramCompleted);

            // function to set the name          
            if (ble_rn4870.setName(bufferParam, strlen(bufferParam))) {
                SerialUSB.print("Name ");
                SerialUSB.print(bufferParam);
                SerialUSB.println(" changed correctly");
            } else {
                SerialUSB.println("Error in setName ");
            }
            break;

        case '2':
            break;

        case '3':
            do {
                paramCompleted = composeParam();
            } while (!paramCompleted);

            // function to set the name
            if (ble_rn4870.setBaudRate(bufferParam[0])) {
                SerialUSB.print("baud-rate set to ");
                SerialUSB.println(bufferParam);
            } else {
                SerialUSB.println("Error in baud-rate setting ");
            }
            break;

        case '4':
            do {
                paramCompleted = composeParam();
            } while (!paramCompleted);

            // function to set the name     
            switch (bufferParam[0]) {
            case '0':
                activated = ble_rn4870.setPowerSave(false);
                break;

            case '1':
                activated = ble_rn4870.setPowerSave(true);
                break;

            default:
                SerialUSB.println("Wrong param ");
                break;
            }     
            if (activated) {                
                SerialUSB.println("set new Power Save");
            } else {
                SerialUSB.println("Error in setPowerSave ");
            }
            break;

            case '5':
                if (ble_rn4870.getFwVersion(bufferParam)) {
                    SerialUSB.print("FW = ");
                    SerialUSB.println(ble_rn4870.getLastAnswer());
                } else {
                    SerialUSB.println("Error in getFW ");
                }
                if (ble_rn4870.getHwVersion(bufferParam)) {
                    SerialUSB.print("Hw = ");
                    SerialUSB.println(ble_rn4870.getLastAnswer());
                } else {
                    SerialUSB.println("Error in getHw ");
                }
                if (ble_rn4870.getSwVersion(bufferParam)) {
                    SerialUSB.print("Sw = ");
                    SerialUSB.println(ble_rn4870.getLastAnswer());
                } else {
                    SerialUSB.println("Error in getSw ");
                }
                if (ble_rn4870.getSN(bufferParam)) {
                    SerialUSB.print("SN = ");
                    SerialUSB.println(ble_rn4870.getLastAnswer());
                } else {
                    SerialUSB.println("Error in getSw ");
                }
                break;

            case '6':
                break;

            case '7':
                if (ble_rn4870.getName(bufferParam)) {
                    SerialUSB.print("Name is ");
                    SerialUSB.println(bufferParam);
                } else {
                    SerialUSB.println("Error in getName ");
                }
                break;

            case '8':
                if (ble_rn4870.getBaudRate(bufferParam)) {
                    SerialUSB.print("baud-rate is ");
                    SerialUSB.println(ble_rn4870.getLastAnswer());
                } else {
                    SerialUSB.println("Error in baud-rate ");
                }
                break;

            case '9':
                boolean power;
                if (ble_rn4870.getPowerSave(&power)) {
                    SerialUSB.print("BLE is ");
                    if (!power)
                        SerialUSB.print("NOT ");
                    SerialUSB.println("in Power Save");
                } else {
                    SerialUSB.println("Error in Power Save");
                }
                break;

            case'A':
                if (ble_rn4870.factoryReset()) {
                    SerialUSB.print("BLE resetting..... ");
                } else {
                    SerialUSB.println("Error in factoryReset");
                }
                break;
        }                                
    }

    // collect the Answer from BLE
    if (ble_rn4870.hasAnswer()==completeAnswer) {
        SerialUSB.write(ble_rn4870.getLastAnswer());

        // clear buffer
        memset(bufferParam, 0, sizeof(bufferParam));
        sendCmd = true;
    }
}

