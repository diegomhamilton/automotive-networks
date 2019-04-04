/**********************************************************************************************
* Exercício PID/OBD2
* Autor: Diego Maia Hamilton - 04 de Março, 2019.
* Disciplina: IF747 - TOP. AVANC. REDES DE COMPUTADORES - Redes Automotivas
* 
* O código permite escolher os parâmetros a serem lidos da rede de acordo com o identificador.
* A exibição dos dados é feita utilizando a porta serial do Arduino.
* 
**********************************************************************************************/

#include <SPI.h>
#include "src/CAN_master/mcp_can.h"

/* User definitions */
#define SPI_CS_PIN      10              // Arduino CS Pin

#define REQUEST_ID      0x7DF
#define RESPONSE_ID     0x7E8
#define BUS_TIMEOUT     500
#define CAN_FREQUENCY   CAN_500KBPS
#define PID_TEMP        0x05
#define PID_RPM         0x0C
#define PID_SPEED       0x0D
#define PID_THROTTLE    0x11
#define PID_FUEL_L      0x2F


MCP_CAN CAN(SPI_CS_PIN);

uint8_t new_ID = PID_RPM;
bool new_rx_data = false, timeout = false;

void printMenu();                       // print menu for choosing parameter to be requested
void sendPID(uint16_t pid);             // send request of PID in the CAN bus
void receiveMessage();                  // check and proccess received data
void printParameter(uint8_t* msg);      // print parameter according to message PID

void setup()
{
    Serial.begin(115200);
    while (CAN.begin(CAN_FREQUENCY) != CAN_OK)
    {
        Serial.println("Connecting...");
        delay(500);
    }
    printMenu();
}

void loop()
{
    receiveMessage();                   // check if a CAN message was received

    if (new_rx_data)
    {
        sendPID(new_ID);
        new_rx_data = false;
    }
}

void printMenu()
{
    Serial.println("\n\nRequest messages over CAN bus:");
    Serial.println("\t(1: Engine coolant temperature)");
    Serial.println("\t(2: Engine RPM)");
    Serial.println("\t(3: Vehicle speed)");
    Serial.println("\t(4: Throttle position)");
    Serial.println("\t(5: Fuel level)");
}

void sendPID(uint8_t pid)
{
    uint8_t msg[8] = {0};
    msg[0] = 0x02;                      // Two data bytes in message
    msg[1] = 0x01;                      // Mode 1 (show current data)
    msg[2] = pid;                       // Parameter identifier
    CAN.sendMsgBuf(REQUEST_ID, 0, 8, msg);
}

void receiveMessage()
{
    uint16_t nodeID = 0x000;
    uint8_t msg[8] = {0};
    uint8_t length = 0;

    if (CAN.checkReceive() == CAN_MSGAVAIL)
    {
        CAN.readMsgBuf(&length, msg);
        nodeID = CAN.getCanId();
        Serial.println("\t Message received from node: 0x" + String(nodeID, HEX));
        
        if (nodeID == RESPONSE_ID)
        {
            printParameter(msg);
        }
        printMenu();
    }
}

void printParameter(uint8_t* msg)
{
    uint8_t pid = msg[2];

    switch(pid)
    {
        case PID_TEMP:
            int16_t temperature;
            temperature = msg[3] - 40;
            Serial.println("- Engine coolant temperature: " + String(temperature));
            break;
        case PID_RPM:
            uint16_t rpm;
            rpm = (float) (256*msg[3] + msg[4])/4.0;
            Serial.println("- Engine rpm: " + String(rpm));
            break;
        case PID_SPEED:
            uint8_t speed;
            speed = msg[3];
            Serial.println("- Vehicle speed: " + String(speed));
            break;
        case PID_THROTTLE:
            uint8_t throttle_pos;
            throttle_pos = 100 * (float) msg[3]/255.0;
            Serial.println("- Throttle position: " + String(throttle_pos));
            break;
        case PID_FUEL_L:
            uint8_t fuel_level;
            fuel_level = 100 * (float) msg[3]/255.0;
            Serial.println("- Fuel level: " + String(fuel_level));
            break;
        default:
            Serial.print("Unknown PID");
            break;
    }
}

void serialEvent()
{
    if(Serial.available())
    {
        char c = Serial.read();

        switch(c)
        {
            case '1':
                Serial.print("Engine coolant temperature");
                new_ID = PID_TEMP;
                break;
            case '2':
                Serial.print("Engine rpm");
                new_ID = PID_RPM;
                break;
            case '3':
                Serial.print("Vehicle speed");
                new_ID = PID_SPEED;
                break;
            case '4':
                Serial.print("Throttle position");
                new_ID = PID_THROTTLE;
                break;
            case '5':
                Serial.print("Fuel level");
                new_ID = PID_FUEL_L;
                break;
            default:
                Serial.print("Unknown message");
                break;
        }
        Serial.println(" requested");
        // SET TIMEOUT!!!
        new_rx_data = true;
    }
}
