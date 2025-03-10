#ifndef __QUECTEL_EC21_H
#define __QUECTEL_EC21_H


extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart2;

void _printf(char *strg);
void SIMTransmit(char *cmd,uint16_t timeout);
void SIMbufferclear();

uint8_t Check_OkResponse();
uint8_t checkAT();
uint16_t checkRSSI();
uint8_t check_IM();
uint8_t changeIMEI(char * NewIMEI);
//MQTT
uint8_t configMQTT();
uint8_t ConnectMQTTServer(char *cid, char *Username, char *Password);
uint8_t OpenMQTTclient(char *Host, int Port);
uint8_t checkMQTTserver();
uint8_t publishtopicMQTT(char *Topic, char *data);
uint8_t subtopic(char *topic, uint8_t QoS);
uint8_t MQTTdiconnectserver();

//GPS
uint8_t GPSHOTstart();
uint8_t GPSstop();
char* GPSINFO();

//network
uint8_t Network_Reg();
uint8_t Resetmodem();
uint8_t SetFULLFun();
uint8_t SetMinFun();

//power managment
uint8_t Quectel_Power_down();
uint8_t Sleep_Quectel();

uint8_t Config_ApReady();
uint8_t Config_URC();


#define WAKEUP_IN 1
#define AP_READY 2
#define MAIN_DTR 3
#define MAIN_RI 4
#endif
