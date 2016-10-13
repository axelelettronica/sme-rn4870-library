#include "../rn4870.h"
#include "rn4870Model.h"

void RN4870::begin(char *buffer, int bufferLen, Uart *_antenna, long baudrate) {
    setStatus(dataMode);
    uartBuffer = buffer;
    uartBufferLen = bufferLen;
    this->antenna = _antenna;
    antenna->begin(baudrate);
}

boolean RN4870::startBLE(char userRA[6]) {
    if (enterConfigurationMode() == false) {
        return false;
    }

    if (assignRandomAddress() == false) {
        return false;
    }

    if (ble_rn4870.enterDataMode() == false) {
        return false;
    }

    return true;
}


void RN4870::rawConfiguration(String stream) {
    answerLen =0; // reset Answer Counter
    memset(uartBuffer, 0, uartBufferLen);
    stream.concat("\r");
    antenna->print(stream);
}

volatile char  timeout=0;
boolean RN4870::enterConfigurationMode(void) {

    setStatus(enterConfmode);
    antenna->print(CONF_CHAR);
    delay(10);
    antenna->print(CONF_CHAR);
    delay(10);
    antenna->print(CONF_CHAR);
    delay(10);

    return answerOrTimeout();
}

boolean RN4870::enterDataMode(void) {

    setStatus(enterDatamode);
    antenna->print(EXIT_CONF);

    return answerOrTimeout();

}

AnswerE RN4870::hasAnswer(void) {
    char data;
    if (antenna->available()) {

        data = antenna->read();
        uartBuffer[answerLen] = data;

        // when in data mode just collect all the char till the end of the buffer
        if (status != dataMode) {
            // the answer ends with the \n or '>' char
            if (uartBuffer[answerLen] == endStreamChar) {
                validateAnswer();
                return completeAnswer;
            }
        } else {
            return dataAnswer;
        }
        answerLen++;

        // cannot receive more data than the allocated buffer
        if (answerLen > uartBufferLen)
            return partialAnswer;
    }

    // nothing ready at the moment
    return noAnswer;
}


boolean RN4870::factoryReset(void) {
    // if not in configuration mode enter immediately
    if (status != confMode) {
        if (!enterConfigurationMode())
            return false;
    }

    rawConfiguration(FACTORY_RESET);
    return true;
}

boolean RN4870::assignRandomAddress(char userRA[6]){
    if (status == confMode) {
        char timeout=0;
        if(NULL == userRA) {
            rawConfiguration(AUTO_RANDOM_ADDRESS);
        } else {
            //[TODO] - Need to add the user requested Mac
            rawConfiguration(AUTO_RANDOM_ADDRESS);
        }

        if (answerOrTimeout()==true) {
            setAddress(trim((char*)getLastAnswer()));
            return true;
        } else {
            return false;
        }

    } else {
        return false;
    }
}


void RN4870::sendData(char *stream, int streamLen) {
    answerLen =0; // reset Answer Counter
    antenna->write(stream, streamLen);
}


boolean RN4870::setName(char *newName, char nameLen){
    if (status != confMode)
        return false;

    char ptrData = strlen(SET_NAME);
    memcpy(uartBuffer, SET_NAME, ptrData);
    memcpy(&uartBuffer[ptrData], newName, nameLen);
    rawConfiguration(uartBuffer);

    return answerOrTimeout();
}


boolean RN4870::getName(char* name){ 

    if (status != confMode)
        return false;

    rawConfiguration(GET_NAME);
    if (answerOrTimeout()) {
        getCleanCommandAnswer((char*)getLastAnswer(), name);
        return true;
      } else {
      return false;
      }      
}


boolean RN4870::getFwVersion(char *bufferParam)
{
    if (status != confMode)
        return false;

    rawConfiguration(GET_FWVERSION);
    return answerOrTimeout();
}

boolean RN4870::getSwVersion(char *bufferParam)
{
    if (status != confMode)
        return false;

    rawConfiguration(GET_SWVERSION);
    return answerOrTimeout();
}

boolean RN4870::getHwVersion(char *bufferParam)
{
    if (status != confMode)
        return false;

    rawConfiguration(GET_HWVERSION);
    return answerOrTimeout();
}

boolean RN4870::setBaudRate(char bufferParam)
{
    if (status != confMode)
        return false;

    char ptrData = strlen(SET_BAUDRATE);
    memcpy(uartBuffer, SET_BAUDRATE, ptrData);
    uartBuffer[ptrData] = bufferParam;
    rawConfiguration(uartBuffer);

    return answerOrTimeout();
}

boolean RN4870::getBaudRate(char *bufferParam)
{
    if (status != confMode)
        return false;

    rawConfiguration(GET_BAUDRATE);
    return answerOrTimeout();
}



boolean RN4870::getSN(char *bufferParam)
{
    if (status != confMode)
        return false;

    rawConfiguration(GET_SERIALNUM);
    return answerOrTimeout();
}



boolean RN4870::setPowerSave(boolean powerSave){

    // if not in configuration mode enter immediately
    if (status != confMode) {
        if (!enterConfigurationMode())
            return false;
    }

    // write CMD in buffer
    char ptrData = strlen(SET_POWERSAVE);
    memcpy(uartBuffer, SET_POWERSAVE, ptrData);

    // write data in buffer
    if (powerSave){
        uartBuffer[ptrData] = POWERSAVE_ENABLE;
    } else {
        uartBuffer[ptrData] = POWERSAVE_DISABLE;
    }
    rawConfiguration(uartBuffer);

    return answerOrTimeout();
}

/*
 * get the internal baud-rate
 *
 */
boolean RN4870::getPowerSave(boolean *powerSave){
    if (status != confMode)
        return false;

    rawConfiguration(GET_POWERSAVE);
    if (answerOrTimeout()) {
        if (getLastAnswer()[0] == '1') {
            *powerSave= true;
        } else {
            powerSave = false;
        } 
        return true;           
    } else {
        return false;
    }
}

/* -----------------------------------------------------------------------------
 *                      PRIVATE SECTION
 * -----------------------------------------------------------------------------
 */
char* RN4870::trim(char* input) {
    int i,j;
    char* output=input;
    for (i = 0, j = 0; i<strlen(input); i++,j++)
    {
        if (input[i]!=' ')
            output[j]=input[i];
        else
            j--;
    }
    output[j]=0;
    return output;
}


boolean RN4870::validateAnswer(void) {
    switch(status) {
    case enterConfmode:
        if ((uartBuffer[0] == PROMPT_FIRST_CHAR) &&
                (uartBuffer[PROMPT_LEN-1] == PROMPT_LAST_CHAR))
            setStatus(confMode);
        break;

    case enterDatamode:
        uartBuffer = trim(uartBuffer);
        if ((uartBuffer[0] == PROMPT_END_FIST_CHAR) &&
                (uartBuffer[strlen(PROMPT_END)-1] == PROMPT_END_LAST_CHAR))
            setStatus(dataMode);
        break;

    default:
        // Do nothing remain in the current status
        break;
    }
}

void RN4870::setStatus(RN4870StatusE newStatus) {
    status = newStatus;

    // reset Answer Counter and buffer
    answerLen =0;
    memset(uartBuffer, 0, uartBufferLen);

    switch(status) {
    case enterConfmode:
    case confMode:
        endStreamChar = PROMPT_LAST_CHAR;
        break;

    case enterDatamode:
    case dataMode:
        memset(uartBuffer, 0, uartBufferLen);
        endStreamChar = DATA_LAST_CHAR;
        break;
    }
}

boolean RN4870::answerOrTimeout(void) {
    // wait till there is an Answer or timeout occurred.
    while ((this->hasAnswer()!=completeAnswer) && (INTERNAL_CMD_TIMEOUT>timeout)){
        delay(DELAY_INTERNAL_CMD);
        timeout++;
    }

    if (INTERNAL_CMD_TIMEOUT>timeout) {
        return true;
    } else {
        return false;
    }
}

void RN4870::setAddress(const char *address){
    //     int i=0,j=0;
    //     char tmp[3]= {0,0,0};
    //     for (j=0; j<6; j++) {
    //         tmp[0] = address[i++];
    //         tmp[1] = address[i++];
    //         if (sscanf(tmp, "%x", &bleAddress[j])) {
    //             SerialUSB.print("OK ");
    //             SerialUSB.print(tmp);
    //         } else {
    //             SerialUSB.print("error ");
    //             SerialUSB.print(tmp);
    //         }
    //
    //     }
    bleAddress[0]=0x59;
    bleAddress[1]=0xe4;
    bleAddress[2]=0x34;
    bleAddress[3]=0xfa;
    bleAddress[4]=0x6b;
    bleAddress[5]=0xcd;
};


void RN4870::getCleanCommandAnswer(char* stream, char* name)
{
    char* cleaned = trim(stream);
    int i,j;
    
    for (i = 0, j = 0; i<strlen(cleaned); i++,j++)
    {
        if (cleaned[i]!=0xd)
            name[j]=cleaned[i];
        else {
        name[j]=0; // null pointer to String
        return;    // exit
        }        
    }
    
}

RN4870 ble_rn4870;
