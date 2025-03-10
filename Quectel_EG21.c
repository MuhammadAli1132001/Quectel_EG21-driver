#include "stm32g0xx_hal.h"
#include "Quectel_EG21.h"
#include <string.h>
#include <stdio.h>

//character array for sending AT command
char ATcommand[200];
//buffer for receiving the data
uint8_t buffer[200] = { 0 };

/* log the data on serial port
 * @param character array
 */
void _printf(char *strg) {
	HAL_UART_Transmit(&huart2, (uint8_t*) strg, strlen(strg), 50); //send the data on UART2
	//UART 2 is used for debugging purpose
}

uint8_t Check_OkResponse() {
	if (strstr((char*) buffer, "OK"))
		return 1;
	else
		return 0;
}
/* Transmit the SIM data via UART1
 * @param command
 * @param timeout
 * @retval none
 */
void SIMTransmit(char *cmd, uint16_t timeout) {
	memset(buffer, 0, sizeof(buffer));
	_printf(cmd);
	HAL_UART_Transmit(&huart1, (uint8_t*) cmd, strlen(cmd), timeout); //send data via UART1
	HAL_UART_Receive(&huart1, buffer, 197, timeout); //received data from UART1
	_printf((char*) buffer);

}

uint8_t checkAT() {
	SIMTransmit("AT\r\n", 1000);
	return Check_OkResponse();
}

uint8_t check_IM(){

	SIMTransmit("AT+CGSN", 1000);

	if(Check_OkResponse()){
	printf("IM checked");
	}

	return Check_OkResponse();
}

uint8_t changeIMEI(char * NewIMEI){

	sprint(ATcommand, "AT+EGMR=1,7, %s", NewIMEI);
	SIMTransmit(ATcommand, 1000);

	if(Check_OkResponse()){
	printf("IMEI Changed");
	}

	return Check_OkResponse();

}
/*check RSSI signals
 * @retval Service Quality value
 */
uint16_t checkRSSI() {

	uint16_t temp = 0;
	SIMTransmit("AT+CSQ\r\n", 2000);                  //AT command for checking Quality of service
	uint8_t index = 0, flag1 = 0;
	if (strstr((char*) buffer, "+CSQ:") == NULL) {
		return 0;
	}
	                                                 //extract the service quality value from the response
	while (buffer[index] != ',') {
		if (buffer[index] == ':') {
			index += 2;
			flag1 = 1;
		}
		if (flag1 == 1) {
			temp *= 10;
			temp += buffer[index] - 48;
		}
		index++;
	}
	return temp;
}


/* configure MQTT keep alive time
 * @retval status
 */
uint8_t configMQTT() {
	SIMTransmit("AT+QMTCFG=\"keepalive\",1,60\r\n", 2000); // configure the MQTT
	return Check_OkResponse();
}

uint8_t OpenMQTTclient(char *Host, int Port) {
	sprintf(ATcommand, "AT+QMTOPEN=0,\"%s\",%d\r\n", Host, Port); // open the host on the specified port
	SIMTransmit(ATcommand, 4000);

	return Check_OkResponse();
}


uint8_t ConnectMQTTServer(char *cid, char *Username, char *Password) {
	sprintf(ATcommand, "AT+QMTCONN=0,\"%s\",\"%s\",\"%s\"\r\n", cid, Username, Password); //connect the client with server credentials
	SIMTransmit(ATcommand, 5000);
	if (strstr((char*) buffer, "+QMTCONN: 0,0,0")) //modify the code according to client index
		return 2;
	if (strstr((char*) buffer, "+QMTCONN: 1,1"))
		return 1;
	else
		return 0;
}

/*	Check whether client is connected with the server
 * @retval status
 */
uint8_t checkMQTTserver() {
	SIMTransmit("AT+QMTOPEN?\r\n", 4000); // check which server is open
	if (strstr((char*) buffer, "+QMTOPEN: 0,\"test.mosquitto.org")) //modify the client index
		return 1;
	else
		return 0;
}

/*Publish the MQTT data on the topic
 * @param MQTT topic name
 * @param data for publish
 * @retval status
 */

uint8_t publishtopicMQTT(char *Topic, char *data) {
//	uint8_t endMarker = 0x1A;
	printf("pub topic: %s\n", Topic);

	sprintf(ATcommand, "AT+QMTPUBEX=0,0,0,0,\"%s\",%d\r\n", Topic, strlen(data)); // Set the topic for publish message
	SIMTransmit(ATcommand, 2000);
	if (strstr((char*) buffer, ">")) {
		sprintf(ATcommand, "%s\r\n", data); //publish the message
		SIMTransmit(ATcommand, 2000);
//        HAL_UART_Transmit(&huart1, &endMarker , 2, 2000);                     //send data via UART1

		return Check_OkResponse();
	} else
		return 0;
}

uint8_t subtopic(char *topic, uint8_t QoS) {

	printf("sub topic: %s\n", topic);
	sprintf(ATcommand, "AT+QMTSUB=0,1,\"%s\",%d\r\n", topic, QoS); // setting up the subscribe topic
	SIMTransmit(ATcommand, 2000);
	return Check_OkResponse();
}

/* Disconnect from MQTT server
 * @retval status
 */
uint8_t MQTTdiconnectserver() {
	SIMTransmit("AT+QMTDISC=0\r\n", 2000); // Disconnect from Server
	return Check_OkResponse();
}

/* Start the GPS
 * @retval status
 */
uint8_t GPSHOTstart() {
	SIMTransmit("AT+QGPS=1\r\n", 1500); // Start GPS
	return Check_OkResponse();
}

/*stop the GPS
 * @retval status
 */
uint8_t GPSstop() {
	SIMTransmit("AT+QGPSEND\r\n", 1000); // Stop GPS
	return Check_OkResponse();
}

/* Get the GPS information sentence
 * @retval GPS_response
 */
char* GPSINFO() {
	SIMTransmit("AT+QGPSGNMEA=\"RMC\"\r\n", 1000); //Get GPS info
	if (strstr((char*) buffer, "+QGPSGNMEA: $GPRMC,,") != NULL)
		return "@@";
	else if (strstr((char*) buffer, "+CME ERROR: 505") != NULL)
		return "#1#";
	else if (strstr((char*) buffer, "+QGPSGNMEA: $GPRMC") != NULL)
		return (char*) buffer;
	else
		return "##";
}

/* check network registration status
 * @retval registration status
 */
uint8_t Network_Reg() {
	SIMTransmit("AT+CREG?\r\n", 1000); // Read Network Registration status
	if (strstr((char*) buffer, "OK")) {
		if (strstr((char*) buffer, "+CREG: 0,0"))
			return 1;
		else if (strstr((char*) buffer, "+CREG: 0,1"))
			return 2;
		else if (strstr((char*) buffer, "+CREG: 0,2"))
			return 3;
		else
			return 4;
	} else
		return 0;
}

/*Reset the Quectel modem
 * @retval status
 */
uint8_t Resetmodem() {
	SIMTransmit("AT+CFUN=1,1\r\n", 1000); // Reset the modem
	return Check_OkResponse();
}

/* set the quectel module to full functionality
 * @retval status
 */
uint8_t SetFULLFun() {
	SIMTransmit("AT+CFUN=1\r\n", 1000); // set the modem to full functionality
	return Check_OkResponse();
}

/*set the Quectel module to minimum functionality
 * @retval status
 */
uint8_t SetMinFun() {
	SIMTransmit("AT+CFUN=0\r\n", 1000); // set the modem to minimum functionality
	return Check_OkResponse();
}

/*clear the GSM buffer
 */
void SIMbufferclear() {
	memset(buffer, 0, sizeof(buffer));
	HAL_UART_Receive(&huart1, buffer, 150, 200);
	_printf((char*) buffer);

}
uint8_t Quectel_Power_down() {
	SIMTransmit("AT+QPOWD=1\r\n", 1000); // Push the modem into normal power down
	return Check_OkResponse();
}
uint8_t Sleep_Quectel() {
	SIMTransmit("AT+QSCLK=1\r\n", 1000); // Push the modem into sleep mode
	return Check_OkResponse();
}
uint8_t Config_ApReady() {
	SIMTransmit("AT+QCFG=\"apready\",1\r\n", 1000); //configure AP_READY
	return Check_OkResponse();
}
uint8_t Config_URC() {
	SIMTransmit(" AT+QURCCFG=\"urcport\",\"uart1\"\r\n", 1000); //Configure URC
	return Check_OkResponse();
}


