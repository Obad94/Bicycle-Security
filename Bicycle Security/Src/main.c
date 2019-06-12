
	/**
	******************************************************************************
	* @file           : main.c
	* @brief          : Main program body
	******************************************************************************
	* This notice applies to any and all portions of this file
	* that are not between comment pairs USER CODE BEGIN and
	* USER CODE END. Other portions of this file, whether 
	* inserted by the user or by software development tools
	* are owned by their respective copyright owners.
	*
	* Copyright (c) 2019 STMicroelectronics International N.V. 
	* All rights reserved.
	*
	* Redistribution and use in source and binary forms, with or without 
	* modification, are permitted, provided that the following conditions are met:
	*
	* 1. Redistribution of source code must retain the above copyright notice, 
	*    this list of conditions and the following disclaimer.
	* 2. Redistributions in binary form must reproduce the above copyright notice,
	*    this list of conditions and the following disclaimer in the documentation
	*    and/or other materials provided with the distribution.
	* 3. Neither the name of STMicroelectronics nor the names of other 
	*    contributors to this software may be used to endorse or promote products 
	*    derived from this software without specific written permission.
	* 4. This software, including modifications and/or derivative works of this 
	*    software, must execute solely and exclusively on microcontroller or
	*    microprocessor devices manufactured by or for STMicroelectronics.
	* 5. Redistribution and use of this software other than as permitted under 
	*    this license is void and will automatically terminate your rights under 
	*    this license. 
	*
	* THIS SOFTWARE IS PROVIDED BY STMICROELECTRONICS AND CONTRIBUTORS "AS IS" 
	* AND ANY EXPRESS, IMPLIED OR STATUTORY WARRANTIES, INCLUDING, BUT NOT 
	* LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY, FITNESS FOR A 
	* PARTICULAR PURPOSE AND NON-INFRINGEMENT OF THIRD PARTY INTELLECTUAL PROPERTY
	* RIGHTS ARE DISCLAIMED TO THE FULLEST EXTENT PERMITTED BY LAW. IN NO EVENT 
	* SHALL STMICROELECTRONICS OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
	* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
	* LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, 
	* OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF 
	* LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING 
	* NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
	* EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
	*
	******************************************************************************
	*/
	/* Includes ------------------------------------------------------------------*/
	#include "main.h"
	#include "stm32l4xx_hal.h"
	#include "cmsis_os.h"
	#include "stdbool.h"
	#include <string.h>
	#include <stdio.h>
	/* USER CODE BEGIN Includes */
	#define DEBUG false //true
	/* USER CODE END Includes */

	/* Private variables ---------------------------------------------------------*/
	UART_HandleTypeDef huart1;
	UART_HandleTypeDef huart2;

	osThreadId defaultTaskHandle;
	osThreadId myTask02Handle;
	osMutexId uartMutexHandle;

	char buffer[300];
	char * OK_reply = "";
	uint8_t reply[300];
	//char phone_no[] = "+923323567799"; // replace with your phone no.
	char *state, *timegps, *latitude, *longitude;
	uint8_t p1[ 20 ] = "Priority Thread 1\n";
	uint8_t p2[ 20 ] = "Priority Thread 2\n";
	uint8_t data[200];
	uint8_t ledOn[ 20 ] = "LED IS ON\n";
	uint8_t ledOff[ 20 ] = "LED IS OFF\n";
	uint8_t print[] = "HELLO WORLD\n";
	uint8_t print2[] = "SLEEPING WORLD\n";
	uint8_t msg[ 40 ] = "Message Recieved\n";
	uint8_t rx_data = 0;
	uint8_t rx_buffer[20];
	volatile bool check = false;
	volatile bool check2 = false;

	/* USER CODE BEGIN PV */
	char str[90];
	const char DEFAULT1[]     = "AT&F0\r\n";
	const char ECHO[]         = "ATE0\r\n";
	const char SAVE[]         = "AT&W\r\n";
	const char CMEE[]         = "AT+CMEE=2\r\n";
	const char AT[]           = "AT\r\n";
	const char CREG[]         = "AT+CREG?\r\n";
	const char CSQ[]          = "AT+CSQ\r\n";
	const char COPS[]         = "AT+COPS?\r\n";
	const char SAPBR3_1[]     = "AT+SAPBR=3,1,\"CONTYPE\",\"GPRS\"\r\n";
	const char APN[]          = "AT+SAPBR=3,1,\"APN\",\"internet\"\r\n"; 
	const char USER[]         = "AT+SAPBR=3,1,\"USER\",\"Telenor\"\r\n";
	const char PWD[]          = "AT+SAPBR=3,1,\"PWD\",\"Telenor\"\r\n";
	const char SAPBR1_1[]     = "AT+SAPBR=1,1\r\n";
	const char SAPBR0_1[]     = "AT+SAPBR=0,1\r\n";
	const char SAPBR2_1[]     = "AT+SAPBR=2,1\r\n";   //AT+SAPBR=0,1\r\n to slose bearier 
	const char HTTPINIT[]     = "AT+HTTPINIT\r\n";
	const char HTTPPARACID[]  = "AT+HTTPPARA=\"CID\",1\r\n";
	//const char HTTPSTATUS[] = "AT+HTTPSTATUS?\r\n";
	const char HTTPACTION[]   = "AT+HTTPACTION=0\r\n";
	const char HTTPREAD[]     = "AT+HTTPREAD\r\n";
	const char HTTPTERM[]     = "AT+HTTPTERM\r\n";
	const char NewLine[]      = "\r\n";
	char* OK_REPLY;


	/* USER CODE END PV */

	/* Private function prototypes -----------------------------------------------*/
	void SystemClock_Config(void);
	static void MX_GPIO_Init(void);
	static void MX_USART1_UART_Init(void);
	static void MX_USART2_UART_Init(void);
	void Thread1(void const * argument);
	void Thread2(void const * argument);
	void setup(void);
	void loop(void);
	void sendMSG(char*, char*);
	void updateSerial(void);
	void gprs(void);
	char* Check_OK(char []);

	/* USER CODE BEGIN PFP */
	/* Private function prototypes -----------------------------------------------*/

	/* USER CODE END PFP */

	/* USER CODE BEGIN 0 */
	//void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
	//{
	//	HAL_UART_Transmit ( &huart2, print2 , sizeof( print2 ), 100 );
	//	if(huart->Instance == USART2){
	//		HAL_UART_Receive_IT ( &huart2, &rx_data , 1);
	//	  HAL_UART_Transmit ( &huart2, &rx_data , 1, 100 );
	//		check = true;
	//	}
	//	if(huart->Instance == USART1){
	//		HAL_UART_Receive_IT ( &huart1, &rx_data , 1);
	//	  HAL_UART_Transmit ( &huart2, &rx_data , 1, 100 );
	//		check2 = true;
	//	}
	//}
	/* USER CODE END 0 */

	/**
	* @brief  The application entry point.
	*
	* @retval None
	*/
	int main(void)
	{
	/* USER CODE BEGIN 1 */

	/* USER CODE END 1 */

	/* MCU Configuration----------------------------------------------------------*/

	/* Reset of all peripherals, Initializes the Flash interface and the Systick. */
	HAL_Init();

	/* USER CODE BEGIN Init */

	/* USER CODE END Init */

	/* Configure the system clock */
	SystemClock_Config();

	/* USER CODE BEGIN SysInit */

	/* USER CODE END SysInit */

	/* Initialize all configured peripherals */
	MX_GPIO_Init();
	MX_USART1_UART_Init();
	MX_USART2_UART_Init();
	/* USER CODE BEGIN 2 */
	setup();
	//	HAL_UART_Receive_IT(&huart2, &rx_data, 1);
	//	HAL_UART_Receive_IT(&huart1, &rx_data, 1);
	/* USER CODE END 2 */

	/* Create the mutex(es) */
	/* definition and creation of uartMutex */
	osMutexDef(uartMutex);
	uartMutexHandle = osMutexCreate(osMutex(uartMutex));

	/* USER CODE BEGIN RTOS_MUTEX */
	/* add mutexes, ... */
	/* USER CODE END RTOS_MUTEX */

	/* USER CODE BEGIN RTOS_SEMAPHORES */
	/* add semaphores, ... */
	/* USER CODE END RTOS_SEMAPHORES */

	/* USER CODE BEGIN RTOS_TIMERS */
	/* start timers, add new ones, ... */
	/* USER CODE END RTOS_TIMERS */

	/* Create the thread(s) */
	/* definition and creation of defaultTask */
	osThreadDef(defaultTask, Thread1, osPriorityRealtime, 0, 128);
	defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

	/* definition and creation of myTask02 */
	osThreadDef(myTask02, Thread2, osPriorityHigh, 0, 128);
	myTask02Handle = osThreadCreate(osThread(myTask02), NULL);

	/* USER CODE BEGIN RTOS_THREADS */
	/* add threads, ... */
	/* USER CODE END RTOS_THREADS */

	/* USER CODE BEGIN RTOS_QUEUES */
	/* add queues, ... */
	/* USER CODE END RTOS_QUEUES */


	/* Start scheduler */
	osKernelStart();

	/* We should never get here as control is now taken by the scheduler */

	/* Infinite loop */
	/* USER CODE BEGIN WHILE */
	setvbuf(stdin, NULL, _IONBF, 0);
	while (1)
	{

	}
	/* USER CODE END 3 */

	}

	/**
	* @brief System Clock Configuration
	* @retval None
	*/


	void sendMSG(char* latitude, char* longitude){
	char nl[] = "\r\n";
	uint8_t CMGS[] = "AT+CMGS=\"+923323567799""\"\r\n"; 
	char URL[100];
	char foo[] = { 26 };
	char *Latitude , *Longitude;
	Latitude = latitude;
	Longitude = longitude;
	memset(URL , '\0', strlen(URL));
	strcpy(URL , "http://maps.google.com/maps?q=loc:");
	strcat (URL , Latitude);
	strcat(URL , ",");
	strcat (URL , Longitude);
	if(DEBUG){
	xSemaphoreTake(uartMutexHandle ,portMAX_DELAY);
	HAL_UART_Transmit ( &huart2, (uint8_t*)nl, sizeof( nl ), 1000 );
	xSemaphoreGive(uartMutexHandle);
	}
	memset( reply, '\0', sizeof( reply ) );
	xSemaphoreTake(uartMutexHandle ,portMAX_DELAY);
	HAL_UART_Transmit (&huart1, (uint8_t*)CMGS, strlen( CMGS ), 1000 );
	HAL_Delay(30);
	HAL_UART_Transmit ( &huart1, (uint8_t*)URL, strlen( URL ), 1000 );
	HAL_UART_Transmit( &huart1, (uint8_t*)foo, strlen( foo ), 1000);
	HAL_UART_Transmit (&huart1, (uint8_t*)nl, sizeof( nl ), 1000 );
	HAL_UART_Transmit (&huart1, (uint8_t*)nl, sizeof( nl ), 1000 );
	HAL_UART_Receive  (&huart1, (uint8_t*)reply, sizeof(reply), 1000);
	xSemaphoreGive(uartMutexHandle);
	if(DEBUG){
	xSemaphoreTake(uartMutexHandle ,portMAX_DELAY);
	HAL_UART_Transmit (&huart2, (uint8_t*)reply, sizeof( reply ), 1000 );
	xSemaphoreGive(uartMutexHandle);
	}
	}

	void setup(void){
	HAL_GPIO_WritePin(Led_GPIO_Port, Led_Pin, GPIO_PIN_RESET);
	HAL_Delay(3000);
	HAL_GPIO_WritePin(Led_GPIO_Port, Led_Pin, GPIO_PIN_SET);
	HAL_Delay(3000);
	uint8_t initial[ 20 ] = "Initializing...\n";
	uint8_t factoryDefault[] = "AT&F\r\n";
	uint8_t AT[] = "AT\r\n";
	uint8_t CSMP[] = "AT+CSMP=17,167,0,0\r\n";	
	uint8_t CMGF[] = "AT+CMGF=1\r\n";		
	uint8_t CNMI[] = "AT+CNMI=1,2,0,0,0\r\n";						
	uint8_t CGNSPWR[] = "AT+CGNSPWR=1\r\n";	
	uint8_t CGNSSEQ[] = "AT+CGNSSEQ=RMC\r\n";				
	uint8_t buffer[150];
		if(DEBUG){
	HAL_UART_Transmit ( &huart2, initial, sizeof( initial ), 10 );
		}
	memset(buffer,0,sizeof(buffer));
	HAL_UART_Transmit (&huart1, factoryDefault, sizeof( factoryDefault ), 10 );
	HAL_UART_Receive(&huart1, buffer, sizeof(buffer), 10);
		if(DEBUG){
	HAL_UART_Transmit ( &huart2, buffer, sizeof( buffer ), 10 );
		}
	memset(buffer,0,sizeof(buffer));
	HAL_UART_Transmit (&huart1, AT, sizeof( AT ), 10 );
	HAL_UART_Receive(&huart1, buffer, sizeof(buffer), 10);
		if(DEBUG){
	HAL_UART_Transmit ( &huart2, buffer, sizeof( buffer ), 10 );
		}
	memset(buffer,0,sizeof(buffer));
	HAL_UART_Transmit (&huart1, CSMP, sizeof( CSMP), 10 );
	HAL_UART_Receive(&huart1, buffer, sizeof(buffer), 10);
		if(DEBUG){
	HAL_UART_Transmit ( &huart2, buffer, sizeof( buffer ), 10 );
		}
	memset(buffer,0,sizeof(buffer));
	HAL_UART_Transmit (&huart1, CGNSPWR, sizeof( CGNSPWR ), 10 );
	HAL_UART_Receive(&huart1, buffer, sizeof(buffer), 10);
		if(DEBUG){
	HAL_UART_Transmit ( &huart2, buffer, sizeof( buffer ), 10 );
		}
	memset(buffer,0,sizeof(buffer));
	HAL_UART_Transmit (&huart1, CGNSSEQ, sizeof( CGNSSEQ ), 10 );
	HAL_UART_Receive(&huart1, buffer, sizeof(buffer), 10);
		if(DEBUG){
	HAL_UART_Transmit ( &huart2, buffer, sizeof( buffer ), 10 );
		}
	memset(buffer,0,sizeof(buffer));
	HAL_UART_Transmit (&huart1, CMGF, sizeof( CMGF ), 10 );
	HAL_UART_Receive(&huart1, buffer, sizeof(buffer), 10);
		if(DEBUG){
	HAL_UART_Transmit ( &huart2, buffer, sizeof( buffer ), 10 );
		}
	memset(buffer,0,sizeof(buffer));
	HAL_UART_Transmit (&huart1, CNMI, sizeof( CNMI ), 10 );
	HAL_UART_Receive(&huart1, buffer, sizeof(buffer), 10);
		if(DEBUG){
	HAL_UART_Transmit ( &huart2, buffer, sizeof( buffer ), 10 );
		}
	}


	void updateSerial(void){

	memset( data, '\0', sizeof( data ) );
	xSemaphoreTake(uartMutexHandle ,portMAX_DELAY);
	HAL_UART_Receive(&huart1, data, sizeof(data), 1000);
	xSemaphoreGive(uartMutexHandle);
	if ((strstr(data, "On")) || (strstr(data, "Off")))
	{
	if (strstr(data, "On")) {
	HAL_GPIO_WritePin(Led_GPIO_Port, Led_Pin, GPIO_PIN_SET);
		if(DEBUG){
	xSemaphoreTake(uartMutexHandle ,portMAX_DELAY);
	HAL_UART_Transmit ( &huart2, ledOn, sizeof(ledOn), 10 );
	xSemaphoreGive(uartMutexHandle);
		}
	}
	else if (strstr(data, "Off")) {
	HAL_GPIO_WritePin(Led_GPIO_Port, Led_Pin, GPIO_PIN_RESET);
		if(DEBUG){
	xSemaphoreTake(uartMutexHandle ,portMAX_DELAY);
	HAL_UART_Transmit ( &huart2, ledOff, sizeof(ledOff), 10 );
	xSemaphoreGive(uartMutexHandle);
		}
	}
	if(DEBUG){
	xSemaphoreTake(uartMutexHandle ,portMAX_DELAY);
	HAL_UART_Transmit ( &huart2,p1, sizeof( p1 ), 10 );
	HAL_UART_Transmit(&huart2, data, sizeof(data), 10 ); 
	xSemaphoreGive(uartMutexHandle);
	}
	}
	}

	void loop(void){
	uint8_t comma_count = 0;
	char delim[] = ",";
	char dataGSM[100];
	char *token;
	uint8_t newline[] = "\n";
	uint8_t CGNSINF[] = "AT+CGNSINF\r";
	uint8_t varState[] = "\n\nState  : ";
	uint8_t varfTime[] = "    Time  : ";
	uint8_t varLatitude[] = "    Latitude  : ";
	uint8_t varLongitude[] = "    Longitude  : ";
	uint8_t initGPS[] = "\nGPS Initializing…\n";
	memset(dataGSM,'\0',sizeof(dataGSM));
	xSemaphoreTake(uartMutexHandle ,portMAX_DELAY);
	HAL_UART_Transmit (&huart1, (uint8_t*)CGNSINF, sizeof( CGNSINF ), 5 );
	HAL_UART_Receive(&huart1, (uint8_t*)dataGSM, sizeof(dataGSM), 10);
	xSemaphoreGive(uartMutexHandle);
		if(DEBUG){
	xSemaphoreTake(uartMutexHandle ,portMAX_DELAY);
	HAL_UART_Transmit ( &huart2, (uint8_t*)dataGSM, sizeof( dataGSM), 10 );
	xSemaphoreGive(uartMutexHandle);
		}
	for(int i = 0; i< 15;i++){
	dataGSM[i] = ',';
	}
	token = strtok (dataGSM,delim);
	while (comma_count <= 3)
	{
	token = strtok(NULL, delim);
	comma_count++;
	switch (comma_count) {
	case 1 :
	state = token;	
	break;
	case 2 :
	timegps = token;
	break;
	case 3 :
	latitude = token;
	break;
	case 4:
	longitude = token;
	break;
	}
	}
	if (*state != 0) {
	gprs();
	//sendMSG(latitude,longitude);
		if(DEBUG){
	xSemaphoreTake(uartMutexHandle ,portMAX_DELAY);
	HAL_UART_Transmit ( &huart2, varState, sizeof( varState ), 10 );
	xSemaphoreGive(uartMutexHandle);
	xSemaphoreTake(uartMutexHandle ,portMAX_DELAY);
	HAL_UART_Transmit ( &huart2, (uint8_t*)state, strlen( state ), 10);
	xSemaphoreGive(uartMutexHandle);
	xSemaphoreTake(uartMutexHandle ,portMAX_DELAY);
	HAL_UART_Transmit ( &huart2, varfTime, sizeof( varfTime ), 10 );
	xSemaphoreGive(uartMutexHandle);
	xSemaphoreTake(uartMutexHandle ,portMAX_DELAY);
	HAL_UART_Transmit ( &huart2, (uint8_t*)timegps , strlen( timegps ), 10 );
	xSemaphoreGive(uartMutexHandle);
	xSemaphoreTake(uartMutexHandle ,portMAX_DELAY);
	HAL_UART_Transmit ( &huart2, varLatitude, sizeof( varLatitude ), 10 );
	xSemaphoreGive(uartMutexHandle);
	xSemaphoreTake(uartMutexHandle ,portMAX_DELAY);
	HAL_UART_Transmit ( &huart2, (uint8_t*)latitude, strlen( latitude ), 10 );
	xSemaphoreGive(uartMutexHandle);
	xSemaphoreTake(uartMutexHandle ,portMAX_DELAY);
	HAL_UART_Transmit ( &huart2, varLongitude, sizeof( varLongitude ), 10 );
	xSemaphoreGive(uartMutexHandle);
	xSemaphoreTake(uartMutexHandle ,portMAX_DELAY);
	HAL_UART_Transmit ( &huart2, (uint8_t*)longitude , strlen( longitude ), 10 );
	xSemaphoreGive(uartMutexHandle);
	xSemaphoreTake(uartMutexHandle ,portMAX_DELAY);
	HAL_UART_Transmit (&huart2, (uint8_t*)newline, sizeof( newline ), 10 );
	xSemaphoreGive(uartMutexHandle);
	xSemaphoreTake(uartMutexHandle ,portMAX_DELAY);
	HAL_UART_Transmit (&huart2, (uint8_t*)newline, sizeof( newline ), 10 );
	xSemaphoreGive(uartMutexHandle);
}
	}
	else {
		if(DEBUG){
	xSemaphoreTake(uartMutexHandle ,portMAX_DELAY);
	HAL_UART_Transmit ( &huart2, (uint8_t*)initGPS, sizeof( initGPS ), 10 );
	xSemaphoreGive(uartMutexHandle);
		}
	}
	}

	void gprs(void){
			char *state = "ON";
	strcpy(str, "AT+HTTPPARA=\"""URL""\",\"http://zafarobad.000webhostapp.com/sensor1.php?api=XjUyT9K0&&temp=");
	strcat (str, state);
	strcat (str, "\"\r\n");
		if(DEBUG){
	xSemaphoreTake(uartMutexHandle ,portMAX_DELAY);
	HAL_UART_Transmit ( &huart2, (uint8_t*)str, sizeof( str ), 20 );
	xSemaphoreGive(uartMutexHandle);
		}
	for(int i = 0; i < 10 ; i++){
	memset(buffer,'\0',sizeof(buffer));
	xSemaphoreTake(uartMutexHandle ,portMAX_DELAY);
	HAL_UART_Transmit (&huart1,(uint8_t*) AT, sizeof( AT ), 10 );
	HAL_UART_Receive(&huart1, (uint8_t*) buffer, sizeof(buffer), 10);
		if(DEBUG){
	HAL_UART_Transmit ( &huart2, (uint8_t*)buffer, sizeof( buffer ), 10 );
		}
	xSemaphoreGive(uartMutexHandle);
	OK_REPLY =	Check_OK(buffer);
	if(strncmp(OK_REPLY,"true",5) == 0){
	break;
	}
	}
	for(int i = 0; i < 10 ; i++){
	memset(buffer,'\0',sizeof(buffer));
	xSemaphoreTake(uartMutexHandle ,portMAX_DELAY);
	HAL_UART_Transmit (&huart1, (uint8_t*)COPS, sizeof( COPS ), 10 );
	HAL_UART_Receive(&huart1, (uint8_t*)buffer, sizeof(buffer), 10);
		if(DEBUG){
	HAL_UART_Transmit ( &huart2, (uint8_t*)buffer, sizeof( buffer ), 10 );
		}
	xSemaphoreGive(uartMutexHandle);
	OK_REPLY =	Check_OK(buffer);
	if(strncmp(OK_REPLY,"true",5) == 0){
	break;
	}
	}
	for(int i = 0; i < 10 ; i++){
	memset(buffer,'\0',sizeof(buffer));
	xSemaphoreTake(uartMutexHandle ,portMAX_DELAY);
	HAL_UART_Transmit (&huart1, (uint8_t*)SAPBR3_1, sizeof( SAPBR3_1 ), 10 );
	HAL_UART_Receive(&huart1, (uint8_t*)buffer, sizeof(buffer), 10);
		if(DEBUG){
	HAL_UART_Transmit ( &huart2, (uint8_t*)buffer, sizeof( buffer ), 10 );
		}
	xSemaphoreGive(uartMutexHandle);
	OK_REPLY =	Check_OK(buffer);
	if(strncmp(OK_REPLY,"true",5) == 0){
	break;
	}
	}
		for(int i = 0; i < 10 ; i++){
	memset(buffer,'\0',sizeof(buffer));
	xSemaphoreTake(uartMutexHandle ,portMAX_DELAY);
	HAL_UART_Transmit (&huart1, (uint8_t*)APN , sizeof( USER ), 10 );
	HAL_UART_Receive(&huart1, (uint8_t*)buffer, sizeof(buffer), 10);
		if(DEBUG){
	HAL_UART_Transmit ( &huart2, (uint8_t*)buffer, sizeof( buffer ), 10 );
		}
	xSemaphoreGive(uartMutexHandle);
	OK_REPLY =	Check_OK(buffer);
	if(strncmp(OK_REPLY,"true",5) == 0){
	break;
	}
	}
			for(int i = 0; i < 10 ; i++){
	memset(buffer,'\0',sizeof(buffer));
	xSemaphoreTake(uartMutexHandle ,portMAX_DELAY);
	HAL_UART_Transmit (&huart1, (uint8_t*)USER, sizeof( USER ), 10 );
	HAL_UART_Receive(&huart1, (uint8_t*)buffer, sizeof(buffer), 10);
		if(DEBUG){
	HAL_UART_Transmit ( &huart2, (uint8_t*)buffer, sizeof( buffer ), 10 );
		}
	xSemaphoreGive(uartMutexHandle);
	OK_REPLY =	Check_OK(buffer);
	if(strncmp(OK_REPLY,"true",5) == 0){
	break;
	}
	}
				for(int i = 0; i < 10 ; i++){
	memset(buffer,'\0',sizeof(buffer));
	xSemaphoreTake(uartMutexHandle ,portMAX_DELAY);
	HAL_UART_Transmit (&huart1, (uint8_t*)PWD, sizeof( PWD ), 10 );
	HAL_UART_Receive(&huart1, (uint8_t*)buffer, sizeof(buffer), 10);
		if(DEBUG){
	HAL_UART_Transmit ( &huart2, (uint8_t*)buffer, sizeof( buffer ), 10 );
		}
	xSemaphoreGive(uartMutexHandle);
	OK_REPLY =	Check_OK(buffer);
	if(strncmp(OK_REPLY,"true",5) == 0){
	break;
	}
	}
	for(int i = 0; i < 10 ; i++){
	memset(buffer,'\0',sizeof(buffer));
	xSemaphoreTake(uartMutexHandle ,portMAX_DELAY);
	HAL_UART_Transmit (&huart1, (uint8_t*)SAPBR1_1, sizeof( SAPBR1_1 ), 10 );
	HAL_UART_Receive(&huart1, (uint8_t*)buffer, sizeof(buffer), 1000);
	HAL_UART_Transmit ( &huart2, (uint8_t*)buffer, sizeof( buffer ), 10 );
	xSemaphoreGive(uartMutexHandle);
	OK_REPLY =	Check_OK(buffer);
	if(strncmp(OK_REPLY,"true",5) == 0){
	break;
	}
	}
		for(int i = 0; i < 10 ; i++){
	memset(buffer,'\0',sizeof(buffer));
	xSemaphoreTake(uartMutexHandle ,portMAX_DELAY);
	HAL_UART_Transmit (&huart1, (uint8_t*)SAPBR2_1, sizeof( SAPBR2_1 ), 10 );
	HAL_UART_Receive(&huart1, (uint8_t*)buffer, sizeof(buffer), 1000);
	HAL_UART_Transmit ( &huart2, (uint8_t*)buffer, sizeof( buffer ), 10 );
	xSemaphoreGive(uartMutexHandle);
	OK_REPLY =	Check_OK(buffer);
	if(strncmp(OK_REPLY,"true",5) == 0){
	break;
	}
	}
	for(int i = 0; i < 10 ; i++){
	memset(buffer,'\0',sizeof(buffer));
	xSemaphoreTake(uartMutexHandle ,portMAX_DELAY);
	HAL_UART_Transmit (&huart1,(uint8_t*) HTTPINIT, sizeof( HTTPINIT ), 5 );
	HAL_UART_Receive(&huart1, (uint8_t*)buffer, sizeof(buffer), 5);
		if(DEBUG){
	HAL_UART_Transmit ( &huart2,(uint8_t*) buffer, sizeof( buffer ), 5 );
		}
	xSemaphoreGive(uartMutexHandle);
	OK_REPLY =	Check_OK(buffer);
	if(strncmp(OK_REPLY,"true",5) == 0){
	break;
	}
	}
	for(int i = 0; i < 10 ; i++){
	memset(buffer,'\0',sizeof(buffer));
	xSemaphoreTake(uartMutexHandle ,portMAX_DELAY);
	HAL_UART_Transmit (&huart1, (uint8_t*)HTTPPARACID, sizeof( HTTPPARACID ), 5 );
	HAL_UART_Receive(&huart1, (uint8_t*)buffer, sizeof(buffer), 5);
		if(DEBUG){
	HAL_UART_Transmit ( &huart2, (uint8_t*)buffer, sizeof( buffer ), 5 );
		}
	xSemaphoreGive(uartMutexHandle);
	OK_REPLY =	Check_OK(buffer);
	if(strncmp(OK_REPLY,"true",5) == 0){
	break;
	}
	}
	for(int i = 0; i < 10 ; i++){
	memset(buffer,'\0',sizeof(buffer));
	xSemaphoreTake(uartMutexHandle ,portMAX_DELAY);
	HAL_UART_Transmit (&huart1, (uint8_t*)str, sizeof( str ), 20 );
	HAL_UART_Receive(&huart1, (uint8_t*)buffer, sizeof(buffer), 20);
		if(DEBUG){
	HAL_UART_Transmit ( &huart2, (uint8_t*)buffer, sizeof( buffer ), 20 );
		}
	xSemaphoreGive(uartMutexHandle);
	OK_REPLY =	Check_OK(buffer);
	if(strncmp(OK_REPLY,"true",5) == 0){
	break;
	}
	}
	for(int i = 0; i < 10 ; i++){
	memset(buffer,'\0',sizeof(buffer));
	xSemaphoreTake(uartMutexHandle ,portMAX_DELAY);
	HAL_UART_Transmit (&huart1, (uint8_t*)HTTPACTION, sizeof( HTTPACTION ), 5 );
	HAL_UART_Receive(&huart1, (uint8_t*)buffer, sizeof(buffer), 5);
		if(DEBUG){
	HAL_UART_Transmit ( &huart2, (uint8_t*)buffer, sizeof( buffer ), 5 );
		}
	xSemaphoreGive(uartMutexHandle);
	OK_REPLY =	Check_OK(buffer);
	if(strncmp(OK_REPLY,"true",5) == 0){
	break;
	}
	}
	for(int i = 0; i < 10 ; i++){
	memset(buffer,'\0',sizeof(buffer));
	xSemaphoreTake(uartMutexHandle ,portMAX_DELAY);
	HAL_UART_Transmit (&huart1, (uint8_t*)HTTPREAD, sizeof( HTTPREAD), 5 );
	HAL_UART_Receive(&huart1, (uint8_t*)buffer, sizeof(buffer), 5);
		if(DEBUG){
	HAL_UART_Transmit ( &huart2, (uint8_t*)buffer, sizeof( buffer ), 5 );
		}
	xSemaphoreGive(uartMutexHandle);
	OK_REPLY =	Check_OK(buffer);
	if(strncmp(OK_REPLY,"true",5) == 0){
	break;
	}
	}
	for(int i = 0; i < 10 ; i++){
	memset(buffer,'\0',sizeof(buffer));
	xSemaphoreTake(uartMutexHandle ,portMAX_DELAY);
	HAL_UART_Transmit (&huart1, (uint8_t*)HTTPTERM, sizeof( HTTPTERM ), 5);
	HAL_UART_Receive(&huart1, (uint8_t*)buffer, sizeof(buffer), 50);
		if(DEBUG){
	HAL_UART_Transmit ( &huart2, (uint8_t*)buffer, sizeof( buffer ), 5 );
		}
	xSemaphoreGive(uartMutexHandle);
	OK_REPLY =	Check_OK(buffer);
	if(strncmp(OK_REPLY,"true",5) == 0){
	break;
	}
	}
		for(int i = 0; i < 10 ; i++){
	memset(buffer,'\0',sizeof(buffer));
	xSemaphoreTake(uartMutexHandle ,portMAX_DELAY);
	HAL_UART_Transmit (&huart1, (uint8_t*)SAPBR0_1, sizeof( SAPBR0_1 ), 10 );
	HAL_UART_Receive(&huart1, (uint8_t*)buffer, sizeof(buffer), 1000);
	HAL_UART_Transmit ( &huart2, (uint8_t*)buffer, sizeof( buffer ), 10 );
	xSemaphoreGive(uartMutexHandle);
	OK_REPLY =	Check_OK(buffer);
	if(strncmp(OK_REPLY,"true",5) == 0){
	break;
	}
	}
	}

	char* Check_OK(char buffer[]){
	const char OK[]           = "\nOK Found\n";
	const char NO_OK[]        = "\nOK Not Found\n";
	for(int i = 0; i < 300; i++){
	if((buffer[i] == 'O') && (buffer[i+1] == 'K')) {
		if(DEBUG){
	xSemaphoreTake(uartMutexHandle ,portMAX_DELAY);
	HAL_UART_Transmit ( &huart2, (uint8_t*)OK, sizeof( OK ), 5 );
	xSemaphoreGive(uartMutexHandle);
		}
	OK_reply = "true";
	break;
	}
	else{
	//HAL_UART_Transmit ( &huart2, (uint8_t*)NO_OK, sizeof( NO_OK ), 10 );
	OK_reply = "false";
	}
	}
	return OK_reply;
	}



	void SystemClock_Config(void)
	{

	RCC_OscInitTypeDef RCC_OscInitStruct;
	RCC_ClkInitTypeDef RCC_ClkInitStruct;
	RCC_PeriphCLKInitTypeDef PeriphClkInit;

	/**Configure the main internal regulator output voltage 
	*/
	if (HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1) != HAL_OK)
	{
	_Error_Handler(__FILE__, __LINE__);
	}

	/**Initializes the CPU, AHB and APB busses clocks 
	*/
	RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_MSI;
	RCC_OscInitStruct.MSIState = RCC_MSI_ON;
	RCC_OscInitStruct.MSICalibrationValue = 0;
	RCC_OscInitStruct.MSIClockRange = RCC_MSIRANGE_6;
	RCC_OscInitStruct.PLL.PLLState = RCC_PLL_NONE;
	if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
	{
	_Error_Handler(__FILE__, __LINE__);
	}

	/**Initializes the CPU, AHB and APB busses clocks 
	*/
	RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
			|RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
	RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_MSI;
	RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
	RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
	RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

	if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
	{
	_Error_Handler(__FILE__, __LINE__);
	}

	PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_USART1|RCC_PERIPHCLK_USART2;
	PeriphClkInit.Usart1ClockSelection = RCC_USART1CLKSOURCE_PCLK2;
	PeriphClkInit.Usart2ClockSelection = RCC_USART2CLKSOURCE_PCLK1;
	if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
	{
	_Error_Handler(__FILE__, __LINE__);
	}

	/**Configure the Systick interrupt time 
	*/
	HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/1000);

	/**Configure the Systick 
	*/
	HAL_SYSTICK_CLKSourceConfig(SYSTICK_CLKSOURCE_HCLK);

	/* SysTick_IRQn interrupt configuration */
	HAL_NVIC_SetPriority(SysTick_IRQn, 15, 0);
	}

	/* USART1 init function */
	static void MX_USART1_UART_Init(void)
	{

	huart1.Instance = USART1;
	huart1.Init.BaudRate = 115200;
	huart1.Init.WordLength = UART_WORDLENGTH_8B;
	huart1.Init.StopBits = UART_STOPBITS_1;
	huart1.Init.Parity = UART_PARITY_NONE;
	huart1.Init.Mode = UART_MODE_TX_RX;
	huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart1.Init.OverSampling = UART_OVERSAMPLING_16;
	huart1.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
	huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
	if (HAL_UART_Init(&huart1) != HAL_OK)
	{
	_Error_Handler(__FILE__, __LINE__);
	}

	}

	/* USART2 init function */
	static void MX_USART2_UART_Init(void)
	{

	huart2.Instance = USART2;
	huart2.Init.BaudRate = 115200;
	huart2.Init.WordLength = UART_WORDLENGTH_8B;
	huart2.Init.StopBits = UART_STOPBITS_1;
	huart2.Init.Parity = UART_PARITY_NONE;
	huart2.Init.Mode = UART_MODE_TX_RX;
	huart2.Init.HwFlowCtl = UART_HWCONTROL_NONE;
	huart2.Init.OverSampling = UART_OVERSAMPLING_16;
	huart2.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
	huart2.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
	if (HAL_UART_Init(&huart2) != HAL_OK)
	{
	_Error_Handler(__FILE__, __LINE__);
	}

	}

	/** Configure pins as 
	* Analog 
	* Input 
	* Output
	* EVENT_OUT
	* EXTI
	*/
	static void MX_GPIO_Init(void)
	{

	GPIO_InitTypeDef GPIO_InitStruct;

	/* GPIO Ports Clock Enable */
	__HAL_RCC_GPIOA_CLK_ENABLE();
	__HAL_RCC_GPIOB_CLK_ENABLE();

	/*Configure GPIO pin Output Level */
	HAL_GPIO_WritePin(Led_GPIO_Port, Led_Pin, GPIO_PIN_RESET);

	/*Configure GPIO pin : Led_Pin */
	GPIO_InitStruct.Pin = Led_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(Led_GPIO_Port, &GPIO_InitStruct);

	}

	/* USER CODE BEGIN 4 */

	/* USER CODE END 4 */

	/* USER CODE BEGIN Header_Thread1 */
	/**
	* @brief  Function implementing the defaultTask thread.
	* @param  argument: Not used 
	* @retval None
	*/
	/* USER CODE END Header_Thread1 */
	void Thread1(void const * argument)
	{

	/* USER CODE BEGIN 5 */
	/* Infinite loop */
	for(;;)
	{
	MX_USART1_UART_Init();
	updateSerial();
	osDelay(10);
	}
	/* USER CODE END 5 */ 
	}

	/* USER CODE BEGIN Header_Thread2 */
	/**
	* @brief Function implementing the myTask02 thread.
	* @param argument: Not used
	* @retval None
	*/
	/* USER CODE END Header_Thread2 */
	void Thread2(void const * argument)
	{
	/* USER CODE BEGIN Thread2 */
	/* Infinite loop */
	for(;;)
	{
	MX_USART1_UART_Init();
	loop();
	osDelay(10);
	}
	/* USER CODE END Thread2 */
	}

	/**
	* @brief  Period elapsed callback in non blocking mode
	* @note   This function is called  when TIM16 interrupt took place, inside
	* HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
	* a global variable "uwTick" used as application time base.
	* @param  htim : TIM handle
	* @retval None
	*/
	void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
	{
	/* USER CODE BEGIN Callback 0 */

	/* USER CODE END Callback 0 */
	if (htim->Instance == TIM16) {
	HAL_IncTick();
	}
	/* USER CODE BEGIN Callback 1 */

	/* USER CODE END Callback 1 */
	}

	/**
	* @brief  This function is executed in case of error occurrence.
	* @param  file: The file name as string.
	* @param  line: The line in file as a number.
	* @retval None
	*/
	void _Error_Handler(char *file, int line)
	{
	/* USER CODE BEGIN Error_Handler_Debug */
	/* User can add his own implementation to report the HAL error return state */
	while(1)
	{
	}
	/* USER CODE END Error_Handler_Debug */
	}

	#ifdef  USE_FULL_ASSERT
	/**
	* @brief  Reports the name of the source file and the source line number
	*         where the assert_param error has occurred.
	* @param  file: pointer to the source file name
	* @param  line: assert_param error line source number
	* @retval None
	*/
	void assert_failed(uint8_t* file, uint32_t line)
	{ 
	/* USER CODE BEGIN 6 */
	/* User can add his own implementation to report the file name and line number,
	tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
	/* USER CODE END 6 */
	}
	#endif /* USE_FULL_ASSERT */

	/**
	* @}
	*/

	/**
	* @}
	*/

	/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
