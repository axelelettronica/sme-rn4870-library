#ifndef RN4870_H
#define RN4870_H

#include <Arduino.h>
#include "ble\rn4870Model.h"


class RN4870 {
private:
    Uart *antenna;
    int answerLen;

private:
    boolean checkAnswer(const char *answer);
    char *uartBuffer;
    int uartBufferLen;
    RN4870StatusE status;
    char endStreamChar;
    char bleAddress[6];

public:
    RN4870(){status=dataMode;};
#ifdef ARDUINO_SAMD_SMARTEVERYTHING
    void begin(char *buffer, int bufferLen,Uart *antenna=&BLE, long baudrate=115200);
#elif defined (ASME3_REVISION)
    void begin(char *buffer, int bufferLen,Uart *antenna=&BLE, long baudrate=115200);
#else
    void begin(char *buffer, int bufferLen,Uart *antenna=&Serial1, long baudrate=115200);
#endif

    /*
     * The function set the address and move in data mode
     */
    boolean startBLE(char userRA[6]=NULL);

    /* 
     * Send a specific string of configuration
     */
    void rawConfiguration(String stream);

    /*
     * Send a streamLen of bytes over the air
     */
    void sendData(char *stream, int streamLen);

    /* 
     * function to enter in the BLE chip configuration
     */
    boolean enterConfigurationMode(void);

    /*
     * function to enter in the BLE data communication
     */
    boolean enterDataMode(void);

    AnswerE hasAnswer(void);

    /* 
     * Assign the internal buffer used for the answer of configuration command
     *  or for the received message
     */
    inline void setAnswerBuffer(char *buffer, int bufferLen){
        uartBuffer = buffer;
        uartBufferLen = bufferLen;
    }

    /*
     * generates and assigns a random address to the local device.
     * It accepts one input parameter which is a 6-byte random address.
     * This random address can be a static or a private address.
     *
     *  When the use does not assign a specific address it will be automatically
     *  found by the BLE, otherwise the user address will be assign.
     */
    boolean assignRandomAddress(char userRA[6]=NULL);

    /* 
     * Return the latest receive message from the configuration command or 
     *      from the air
     */
    inline const char* getLastAnswer(void) {return uartBuffer;};

    /*
     * This command resets the configurations into factory default.
     * 
     * The function works either in configuration or in data mode.
     */
    boolean factoryReset(void);

    /*
     * The function return the address used by the BLE 
     *
     */
    inline char* getAddress(void){return bleAddress;};

    /*
     * Set a new name
     *
     * The function works in configuration mode.
     */
    boolean setName(char *newName, char nameLen);

    /*
     * retrieve the configured name
     *
     * The function works in configuration mode.
     *
     */
    boolean getName(char *name);

    /*
     * Set the internal baud rate
     *
     * 00 921600
     * 01 460800
     * 02 230400
     * 03 115200
     * 04 57600
     * 05 38400
     * 06 28800
     * 07 19200
     * 08 14400
     * 09 9600
     * 0A 4800
     * 0B 2400
     *
     * The function works in configuration mode.
     */
    boolean setBaudRate(char bufferParam);

    /*
     * get the internal baud-rate
     *
     * The function works in configuration mode.
     */
    boolean getBaudRate(char *bufferParam);


    /*
     * get the FW version of the BLE
     *
     * The function works in configuration mode.
     */
    boolean getFwVersion(char *bufferParam);

    /*
     * get the SW version of the BLE
     *
     * The function works in configuration mode.
     */
    boolean getSwVersion(char *bufferParam);

    /*
     * get the HW version of the BLE
     *
     * The function works in configuration mode.
     */
    boolean getHwVersion(char *bufferParam);

    /*
     * get the Serial Number of the BLE
     *
     * The function works in configuration mode.
     */
    boolean getSN(char *bufferParam);


    /*
     * true  = Enables  low-power operation.
     *         RN4870 runs 32 kHz clock with much lower 
     *         power consumption.
     *
     * false = disables low-power operation.
     *         RN4870/71 runs 16 MHz clock all the time, 
     *         therefore, can operate UART all the time.     
     *
     *  When RN4870 runs on 32 kHz clock, UART is not operational.
     *  RN4870/71 restarts 16 MHz clock by pulling UART_RX_IND pin low. 
     *
     *  When UART_RX_IND pin is high, RN4870/71 runs 32 kHz clock.      
     *  When RN4870/71 runs on 32 kHz clock, a BLE connection can 
     *      still be maintained, but UART cannot receive data. 
     *  
     *  If the user sends input data to the UART, UART_RX_IND pin 
     *      must be pulled low to start 16MHz clock, 
     *      then wait for 5 ms before UART can be operated.
     *
     *
     *   The function works either in configuration or in data mode.
     */
    boolean setPowerSave(boolean powerSave);

    /*
     * get the internal baud-rate
     *
     * The function works in configuration mode.
     */
    boolean getPowerSave(boolean *powerSave);

private:
    boolean validateAnswer(void);
    void setStatus(RN4870StatusE newStatus);
    char* trim(char* input);
    boolean answerOrTimeout(void);
    void setAddress(const char *address);
    void getCleanCommandAnswer(char* stream, char* name);
};

// external variable used by the sketches
extern RN4870  ble_rn4870;
#endif
