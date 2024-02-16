/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
#include "string.h"
#include <stdio.h>
#include "stdint.h"
#include <string.h>
#include "DHT.h"
#include "RTC.h"
#include "ILI9225.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
I2C_HandleTypeDef hi2c2;

SPI_HandleTypeDef hspi1;

UART_HandleTypeDef huart1;

/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_I2C2_Init(void);
static void MX_SPI1_Init(void);
static void MX_USART1_UART_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
/* variable of temp and hum */
DHT_DataTypedef DHT11_Data;
extern uint8_t Temp_byte1;
extern uint8_t Temp_byte2;
extern uint8_t Rh_byte1;
extern uint8_t Rh_byte2;
float Temperature;
float Humidity;
float Temp1;
float Temp2;
float Hum1;
float Hum2;

float previous_temp1;
float previous_temp2;
float previous_hum1;
float previous_hum2;

uint8_t temperature_string[28];
uint8_t humidity_string[28];
uint8_t is_show_frame = 0;
uint8_t temp_nguyen_str[4];
uint8_t temp_thap_phan_str[4];
uint8_t  hum_phan_nguyen_str[4];
uint8_t	 hum_phan_thap_phan_str[8];
int flag;
/* variable of date and time */
DateTime_t datetime;
uint8_t second_present, min_present, hour_present, date_present, month_present, year_present;
int8_t second_temp, min_temp, hour_temp, date_temp, month_temp, year_temp;
int8_t second_temp1, min_temp1, hour_temp1, date_temp1, month_temp1, year_temp1;
uint8_t secondtemp, mintemp, hourtemp, datetemp, monthtemp, yeartemp;
uint8_t date_max;
char strmin[20];
char strhour[20];
char strdate[20];
char strtempmin1[20];
char strtemphour1[20];
char strtempdate1[20];
char str1[20]; /* to store string of time */
char str2[20]; /*to store string of date */
uint8_t is_morning = 0;
uint8_t is_evening = 0;
uint8_t is_session_change = 0;
uint8_t present = 1;
uint8_t is_done_test_1 = 0;

uint8_t present_session = 0;
uint8_t previous_session = 0;
uint8_t session_flag = 0;

/* variable for uart */
uint8_t Rx_Buf = 0;
uint8_t uart_buffer[7];
uint8_t uart_pointer = 0;
uint8_t uart_flag = 0;
uint32_t u32_timer = 0;
int isOK;
uint8_t data_string[19];
uint8_t data_string2[7];
uint8_t data_string3[12];
uint8_t live_flag = 0;
uint8_t living = 0;

typedef enum
{
	BINHTHUONG = 0,
	CHINHPHUT = 1,
	CHINHGIO = 2,
	CHINHNGAY = 3,
}Mode;

Mode mode; /* khai báo biến "mode" có kiểu dữ liệu "Mode" */

/* 0: not detected, 1: detected */
volatile uint8_t button0_detect = 0;
volatile uint8_t button00_detect = 0;
volatile uint32_t button0_detect_time = 0;
volatile uint8_t button1_detect = 0;
volatile uint8_t button11_detect = 0;
volatile uint32_t button1_detect_time = 0;
volatile uint8_t button22_detect = 0;
volatile uint8_t button2_detect = 0;
volatile uint32_t button2_detect_time = 0;

/* function */
/* Hàm đọc data từ DHT */
void Read_DataDHT(void)
{
	DHT_GetData(&DHT11_Data);
	Temperature = DHT11_Data.Temperature;
	Humidity = DHT11_Data.Humidity;
	Temp1 = DHT11_Data.Temp1;
	Temp2 = DHT11_Data.Temp2;
	Hum1 = DHT11_Data.Hum1;
	Hum2 = DHT11_Data.Hum2;
}
/* Hàm callback ngắt ngoài */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	if (GPIO_Pin == GPIO_PIN_4)
	{
		if (button0_detect == 0)
				{
					button00_detect = 1;
					button0_detect = 1;
					button0_detect_time = HAL_GetTick();
				}
	}
	else if (GPIO_Pin == GPIO_PIN_5)
	{
		button11_detect = 1;
		button1_detect = 1;
		button1_detect_time = HAL_GetTick();
	}
	else if (GPIO_Pin == GPIO_PIN_6)
	{
		button22_detect = 1;
		button2_detect = 1;
		button2_detect_time = HAL_GetTick();
	}
}

void btn_adjust_mode(void)
{
				if (mode == BINHTHUONG)
		  		{
		  			mode = CHINHPHUT;  			/* gán mode = 1 */
		  			if ((button00_detect == 1) ||(button11_detect == 1) || (button22_detect == 1))
		  			{
		  			button00_detect = 0;
		  			button22_detect = 0;
		  			button11_detect = 0;
		  			sprintf(strtempmin1, "%02d", min_temp1);
		  			draw_string(120, 85, COLOR_GREEN, 1, strtempmin1);
		  			}
		  		}
		  		else if (mode == CHINHPHUT)
		  		{
		  			datetime.min = min_temp1;
		  			RTC_WriteTime(&datetime);
		  			mode = CHINHGIO;			/* gán mode = 2 */
		  			fill_rectangle(120, 85, 140, 95, COLOR_WHITE);
		  			if ((button00_detect == 1) ||(button11_detect == 1) || (button22_detect == 1))
		  			{
		  			button00_detect =0;
		  			button22_detect = 0;
		  			button11_detect = 0;

		  			sprintf(strtemphour1, "%02d", hour_temp1);
		  			draw_string(95, 85, COLOR_GREEN, 1, strtemphour1);
		  			}
		  		}
		  		else if (mode == CHINHGIO)		/* mode = 2 */
		  		{
		  			datetime.hour = hour_temp1;
		  			RTC_WriteTime(&datetime);
		  			mode = CHINHNGAY; 			/* gán mode = 3 */
		  			fill_rectangle(95, 85, 115, 95, COLOR_WHITE);
		  			if ((button00_detect == 1) ||(button11_detect == 1) || (button22_detect == 1))
		  			{
		  			button00_detect =0;
		  			button22_detect = 0;
		  			button11_detect = 0;
		  			sprintf(strtempdate1, "%02d", date_temp1);
		  			draw_string(95, 145, COLOR_GREEN, 1, strtempdate1);
		  			}
		  		}
		  		else if (mode == CHINHNGAY)		/* mode = 3 */
		  		{
		  			fill_rectangle(95, 145, 115, 155, COLOR_WHITE);
		  			if (datetime.date != date_temp1)
		  			{
						datetime.date = date_temp;
						RTC_WriteTime(&datetime);
		  			}
		  			mode = BINHTHUONG;			/* gán mode = 0 */
		  		}
}
	/* Tạo hàm sẽ chạy khi nhấn button_up */
void btn_adjust_up (void)
{
     	if (mode == CHINHPHUT)
     	{	
			++min_temp;
			if (min_temp == 60)
			{
				min_temp = 0;
			}
		  	if (min_temp1 != min_temp)
			{
				min_temp1 = min_temp;
				fill_rectangle(120, 85, 140, 95, COLOR_WHITE);
			}
			sprintf(strtempmin1, "%02d", min_temp1);
			draw_string(120, 85, COLOR_GREEN, 1, strtempmin1);
     	}
     	else if (mode == CHINHGIO)
		{	
			++hour_temp;
			if (hour_temp >= 24)
			{
				hour_temp = 0;
			}
		  	if (hour_temp1 != hour_temp)
			{
				hour_temp1 = hour_temp;
				fill_rectangle(95, 85, 115, 95, COLOR_WHITE);
			}
		  	sprintf(strtemphour1, "%02d", hour_temp1);
			draw_string(95, 85, COLOR_GREEN, 1, strtemphour1);
		}
     	else if (mode == CHINHNGAY)
		{		
			++date_temp;
			switch (datetime.month)   			/* xét xem tháng hiện tại có tổng bao nhiêu ngày */
			{
				case 1:
				case 3:
				case 5:
				case 7:
				case 8:
				case 10:
				case 12:
					date_max = 31 + 1;
					break;
				case 4:
				case 6:
				case 9:
				case 11:
					date_max = 30 + 1;
				case 2:
					if ((datetime.year - 2014) % 4 == 0)
					{
						date_max = 29 + 1;
					}
					else
					{
						date_max = 28 + 1;
					}
					break;
				default:
					break;
			}
			if (date_temp >= date_max)
			{
				date_temp = 1;
			}
			if (date_temp1 != date_temp)
			{
				date_temp1 = date_temp;
				fill_rectangle(95, 145, 115, 155, COLOR_WHITE);
			}
			sprintf(strtempdate1, "%02d", date_temp1);
			draw_string(95, 145, COLOR_GREEN, 1, strtempdate1);
		}
}
	/* Tạo hàm sẽ chạy khi nhấn button_down */
void btn_adjust_down(void)
{
		if (mode == CHINHPHUT)
		{	
			--min_temp;
			if (min_temp <= -1)
			{
				min_temp = 59;
			}
		  	if (min_temp1 != min_temp)
			{
				min_temp1 = min_temp;
				fill_rectangle(120, 85, 140, 95, COLOR_WHITE);
			}
		  	sprintf(strtempmin1, "%02d", min_temp1);
		  	draw_string(120, 85, COLOR_GREEN, 1, strtempmin1);
		}
		else if (mode == CHINHGIO)
		{	
			--hour_temp;
			if (hour_temp <= -1)
			{
				hour_temp = 23;
			}
		  	if (hour_temp1 != hour_temp)
			{
			  	hour_temp1 = hour_temp;
				fill_rectangle(95, 85, 115, 95, COLOR_WHITE);
			}
		  	sprintf(strtemphour1, "%02d", hour_temp1);
			draw_string(95, 85, COLOR_GREEN, 1, strtemphour1);

		}
		else if (mode == CHINHNGAY)
		{
			--date_temp;
			switch (datetime.month)						/* xét xem tháng hiện tại có tổng bao nhiêu ngày */
			{
				case 1:
				case 3:
				case 5:
				case 7:
				case 8:
				case 10:
				case 12:
					date_max = 31 + 1;
					break;
				case 4:
				case 6:
				case 9:
				case 11:
					date_max = 30 + 1;
				case 2:
					if ((datetime.year - 2014) % 4 == 0)	/* Nếu năm hiện tại là năm nhuận thì số tổng số ngày của tháng 2 ở năm hiện tại là 29, còn không thì là 28*/
					{
						date_max = 29 + 1;
					}
					else
					{
						date_max = 28 + 1;
					}
					break;
				default:
					break;
			}
			if (date_temp == 0)
			{
				date_temp = date_max - 1;
			}
		  	if (date_temp1 != date_temp)
			{
				date_temp1 = date_temp;
				fill_rectangle(95, 145, 115, 155, COLOR_WHITE);
			}
		  	sprintf(strtempdate1, "%02d", date_temp1);
			draw_string(95, 145, COLOR_GREEN, 1, strtempdate1);
		}
}
	/* Xu li nhan data qua UART */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
	if (huart->Instance == huart1.Instance)
	{
		if (Rx_Buf != '\n')
		{
			uart_buffer[uart_pointer] = Rx_Buf;
			uart_pointer++;
		}
		else
		{
			uart_buffer[uart_pointer] = '\0';
			uart_flag = 1; 						/* biểu thị rằng 1 dòng data hoàn chỉnh đã được nhận từ ngoại vi qua giao tiếp UART */
		}
	HAL_UART_Receive_IT(&huart1, &Rx_Buf, 1);
	}
}

void UART_Handle()
{
	/* send temperature and humidity to another bluetooth device */
	if (live_flag == 1 && living == 1)
	{
		snprintf(data_string3, 11, "*%02g%02g%02g%02g#", Temp1, Temp2, Hum1, Hum2);
		HAL_UART_Transmit(&huart1, (uint8_t*)data_string3, 11, 200);
	}
	/* cut string */
	if (uart_flag == 1)
	{
		char *arg_list[10];//mảng có 10 ô, mỗi ô chứa tối đa 8 bits
		uint8_t arg_num = 0; // số thứ n trong mảng
		char *temp_token = strtok((char *)uart_buffer, " ");	/* Chỉ gán char trừ khoảng trắng cho con tro temp_token */
		while (temp_token != NULL) 				/* vòng lặp chạy mãi cho tới khi temp_token = null */
		{
			arg_list[arg_num] = temp_token; 	/* byte của temp_token sẽ được lưu vào mảng arg_list tại arg_num */
			arg_num++; 							/* để bảo đảm là mỗi token đã được qua thực thi rồi */
			temp_token = strtok(NULL, " ");
		}
	/* handle */
		if (strstr(arg_list[0], "*") != NULL)
		{
			for (int j = 1; j <=6; j++){
				if (uart_buffer[j] == '*')
				{
					for (int i = 0; i<=6; i++)
					{
						uart_buffer[i] = '\0';
						uart_pointer = 0;
					}
				}
			}
			if (strstr(arg_list[1], "live") != NULL)
			{
				live_flag = 1;
				isOK = 1;
				living++;
				if (living == 2) 
					living = 0;
				for (int i = 0; i<=6; i++)
				{
					uart_buffer[i] = '\0';
					uart_pointer = 0;
				}
			}
		}
		else
		{
	    	for (int i = 0; i<=6; i++)
		    {
				uart_buffer[i] = '\0';
				uart_pointer = 0;
		    }
	    	uart_pointer = 0;
			uart_flag = 0;
			isOK = 3;
	    }
	}
}

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

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
  MX_I2C2_Init();
  MX_SPI1_Init();
  MX_USART1_UART_Init();
  /* USER CODE BEGIN 2 */
  mode = 0;
  DHT_Start();
  HAL_UART_Receive_IT(&huart1, &Rx_Buf, 1); // Nhận Data
  lcd_init();
  fill_rectangle(0, 0, WIDTH, HEIGHT, COLOR_WHITE);
  draw_string(10, 10, COLOR_BLUEVIOLET, 2, "HELLO");
  fill_rectangle(0, 0, WIDTH, HEIGHT, COLOR_WHITE);

  /*setting time*/
  datetime.second = 50;
  datetime.min = 58;
  datetime.hour = 17;
  datetime.date = 5;
  datetime.month = 12;
  datetime.year = 23;
  RTC_WriteTime(&datetime);

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
	  /* Hàm xử lí UART */
	  UART_Handle();
	  /* button detect */
	  /* button mode */
	  if ((button0_detect == 1) && (HAL_GetTick() - button0_detect_time >= 200))
	  {
		  button0_detect = 0;
		  btn_adjust_mode();
	  }
	  /* button up */
	  if ((button1_detect == 1) && (HAL_GetTick() - button1_detect_time >= 200))
	  {
		  button1_detect = 0;
		  btn_adjust_up();
	  }
	  /* button down */
	  if ((button2_detect == 1) && (HAL_GetTick() - button2_detect_time >= 200))
	  {
		  button2_detect = 0;
		  btn_adjust_down();
	  }
	  /*show frame of screen once*/
	  if (is_show_frame == 0)
	  {
		 	/*draw title*/
		 	draw_string(40, 5, COLOR_DARKGREEN, 2, "QMD C24");
		 	/*draw for temp and hum*/
			draw_string(20, 45, COLOR_GOLD, 1, "TEMP :");
		    draw_string(20, 65, COLOR_GOLD, 1, "HUM  :");
		    draw_pic_8bit(170, 39, ICON_TEMP, COLOR_RED, 2);
			draw_pic_8bit(170, 59, ICON_HUM, COLOR_BLUE, 2);
			/*draw icon calendar and clock*/
		    draw_pic_8bit(15, 115, ICON_CALENDAR, COLOR_ORANGE, 2);
			draw_pic_8bit(15, 95, ICON_CLOCK, COLOR_BROWN, 2);
			is_show_frame = 1;
	  }

	 /*part of show date and time*/
	 RTC_ReadTime(&datetime);
	 secondtemp = datetime.second;
	 mintemp = datetime.min;
	 hourtemp = datetime.hour;
	 datetemp = datetime.date;
	 monthtemp = datetime.month;
	 yeartemp = datetime.year;
	 sprintf(str1, "Time: %02d:%02d:%02d", datetime.hour, datetime.min, datetime.second);
	 sprintf(strmin, "%02d", datetime.min);
	 sprintf(strhour, "%02d", datetime.hour);
	 sprintf(strdate, "%02d", datetime.date);

	 if(secondtemp != second_present)
	 {
		 fill_rectangle(145, 100, 170, 110, COLOR_WHITE);
		 second_present = secondtemp;
	 }
	 draw_string(40, 100, COLOR_BLACK, 1, str1);

	 if(mintemp != min_present)
	 {
		 fill_rectangle(120, 100, 140, 110, COLOR_WHITE);
		 min_present = mintemp;
	 }
	 draw_string(40, 100, COLOR_BLACK, 1, str1);

	 if(hourtemp != hour_present)
	 {
		 fill_rectangle(95, 100, 115, 110, COLOR_WHITE);
		 hour_present = hourtemp;
	 }
	 draw_string(40, 100, COLOR_BLACK, 1, str1);

	 sprintf(str2,"Date: %02d/%02d/20%02d",datetime.date,datetime.month,datetime.year );
	 if(datetemp != date_present)
	 {
		  fill_rectangle(95, 115, 115, 135, COLOR_WHITE);
		  date_present = datetemp;
	 }
	  	  draw_string(40, 120, COLOR_BLACK, 1, str2);

	 if (monthtemp != month_present)
	 {
		 fill_rectangle(120, 115, 140, 135, COLOR_WHITE);
		 month_present = monthtemp;
	 }
	 draw_string(40, 120, COLOR_BLACK, 1, str2);

	 if (yeartemp != year_present)
	 {
		 fill_rectangle(145, 115, 190, 135, COLOR_WHITE);
		 year_present = yeartemp;
	 }
	 draw_string(40, 120, COLOR_BLACK, 1, str2);

	 /*part of display moon and sun*/
	 if (datetime.hour > 6 && datetime.hour < 18)
	 {
		 present_session = 1;
		 session_flag = 0;
	 }

	 if((datetime.hour <= 6 || datetime.hour >= 18) && session_flag == 0)
	 {
		 present_session = 2;
		 session_flag = 1;
	 }
	 if(present_session != previous_session && session_flag == 1)
	 {
		 fill_rectangle(182, 5, 220, 45, COLOR_WHITE);
		 draw_pic_16bit(182, 5, MOON, COLOR_GRAY, 2); /* display moon */
		 previous_session = present_session;
	 }
	 else if(present_session != previous_session && session_flag == 0)
	 {
		 fill_rectangle(182, 5, 220, 45, COLOR_WHITE);
		 draw_pic_16bit(182, 5, SUN, COLOR_SUN, 2); /* display sun */
		 previous_session = present_session;
	 }

	 /* part of show temp and hum */
	 Read_DataDHT();
	 /* display temp and hum */
	 if (Temp1 != previous_temp1)
	 {
		 sprintf((char *)temp_nguyen_str, "%02d",(uint16_t)Temp1);
		 fill_rectangle(100, 45, 120, 55, COLOR_WHITE);
		 draw_string(100, 45, COLOR_BLUE, 1, (char *)temp_nguyen_str);
		 previous_temp1 = Temp1;
	 }
	 if (Temp2 != previous_temp2)
	 {
		 sprintf((char *)temp_thap_phan_str,".%02d",(uint16_t)Temp2);
		 fill_rectangle(118, 45, 148, 55, COLOR_WHITE);
		 draw_string(118, 45, COLOR_BLUE, 1, (char *)temp_thap_phan_str );
		 previous_temp2 = Temp2;
	 }
	 if (Hum1 != previous_hum1)
	 {
		 sprintf((char *)hum_phan_nguyen_str, "%02d",(uint16_t)Hum1);
		 fill_rectangle(100, 65, 120, 75, COLOR_WHITE);
		 draw_string(100, 65, COLOR_BLUE, 1, (char *)hum_phan_nguyen_str);
		 previous_hum1 = Hum1;
	 }
	 sprintf((char *)hum_phan_thap_phan_str, ".%02d %%",(uint16_t)Hum2);
	 draw_string(118, 65, COLOR_BLUE, 1, (char *)hum_phan_thap_phan_str);
	 HAL_Delay(600);
  }

  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL9;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV2;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_2) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief I2C2 Initialization Function
  * @param None
  * @retval None
  */
static void MX_I2C2_Init(void)
{

  /* USER CODE BEGIN I2C2_Init 0 */

  /* USER CODE END I2C2_Init 0 */

  /* USER CODE BEGIN I2C2_Init 1 */

  /* USER CODE END I2C2_Init 1 */
  hi2c2.Instance = I2C2;
  hi2c2.Init.ClockSpeed = 100000;
  hi2c2.Init.DutyCycle = I2C_DUTYCYCLE_2;
  hi2c2.Init.OwnAddress1 = 0;
  hi2c2.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
  hi2c2.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
  hi2c2.Init.OwnAddress2 = 0;
  hi2c2.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
  hi2c2.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
  if (HAL_I2C_Init(&hi2c2) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN I2C2_Init 2 */

  /* USER CODE END I2C2_Init 2 */

}

/**
  * @brief SPI1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SPI1_Init(void)
{

  /* USER CODE BEGIN SPI1_Init 0 */

  /* USER CODE END SPI1_Init 0 */

  /* USER CODE BEGIN SPI1_Init 1 */

  /* USER CODE END SPI1_Init 1 */
  /* SPI1 parameter configuration*/
  hspi1.Instance = SPI1;
  hspi1.Init.Mode = SPI_MODE_MASTER;
  hspi1.Init.Direction = SPI_DIRECTION_2LINES;
  hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
  hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
  hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
  hspi1.Init.NSS = SPI_NSS_SOFT;
  hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;
  hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
  hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
  hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
  hspi1.Init.CRCPolynomial = 10;
  if (HAL_SPI_Init(&hspi1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN SPI1_Init 2 */

  /* USER CODE END SPI1_Init 2 */

}

/**
  * @brief USART1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART1_UART_Init(void)
{

  /* USER CODE BEGIN USART1_Init 0 */

  /* USER CODE END USART1_Init 0 */

  /* USER CODE BEGIN USART1_Init 1 */

  /* USER CODE END USART1_Init 1 */
  huart1.Instance = USART1;
  huart1.Init.BaudRate = 9600;
  huart1.Init.WordLength = UART_WORDLENGTH_8B;
  huart1.Init.StopBits = UART_STOPBITS_1;
  huart1.Init.Parity = UART_PARITY_NONE;
  huart1.Init.Mode = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
  if (HAL_UART_Init(&huart1) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART1_Init 2 */

  /* USER CODE END USART1_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOA_CLK_ENABLE();
  __HAL_RCC_GPIOB_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOA, TFT_RST_Pin|GPIO_PIN_8, GPIO_PIN_RESET);

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(GPIOB, TFT_RS_Pin|TFT_CS_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pins : TFT_RST_Pin PA8 */
  GPIO_InitStruct.Pin = TFT_RST_Pin|GPIO_PIN_8;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);

  /*Configure GPIO pins : TFT_RS_Pin TFT_CS_Pin */
  GPIO_InitStruct.Pin = TFT_RS_Pin|TFT_CS_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /*Configure GPIO pins : MODE_Pin UP_Pin DOWN_Pin */
  GPIO_InitStruct.Pin = MODE_Pin|UP_Pin|DOWN_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_IT_FALLING;
  GPIO_InitStruct.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);

  /* EXTI interrupt init*/
  HAL_NVIC_SetPriority(EXTI4_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI4_IRQn);

  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 0, 0);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);

}

/* USER CODE BEGIN 4 */

/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
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
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
