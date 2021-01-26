/**
  ******************************************************************************
  * @file    main.c
  * @author  Vsion yang
  * @version V1.0.0
  * @date    2019.8.5
  * @brief   
  ******************************************************************************
  * @attention
  *
  * THE PRESENT FIRMWARE WHICH IS FOR GUIDANCE ONLY AIMS AT PROVIDING CUSTOMERS
  * WITH CODING INFORMATION REGARDING THEIR PRODUCTS IN ORDER FOR THEM TO SAVE
  * TIME. AS A RESULT, STMICROELECTRONICS SHALL NOT BE HELD LIABLE FOR ANY
  * DIRECT, INDIRECT OR CONSEQUENTIAL DAMAGES WITH RESPECT TO ANY CLAIMS ARISING
  * FROM THE CONTENT OF SUCH FIRMWARE AND/OR THE USE MADE BY CUSTOMERS OF THE
  * CODING INFORMATION CONTAINED HEREIN IN CONNECTION WITH THEIR PRODUCTS.
  *
  * <h2><center>&copy; COPYRIGHT 2016 STMicroelectronics</center></h2>
  ******************************************************************************
 */
 
/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"
#include "main.h"
#include "gpio.h"
#include "usart.h"
#include "oled.h"
//#include "spi.h"
#include "spi.h"
#include "time.h"
#include "exit.h"
#include "key.h"
#include "can.h"
#include "display.h"
#include "flash.h"
#include "tle5012.h"
#include "iwdg.h"


#define CLOSED_LOOP
#define CLOSED_LOOP_CONFIG
//#define TEST_FLASH

//
int16_t kp=30;     
int16_t ki=10;  
int16_t kd=250; 

//
const uint8_t LPFA=125; 
const uint8_t LPFB=3;

int32_t s = 0;//
int32_t s_1 = 0;
int32_t s_sum = 0;//
int32_t r = 0;//
int32_t r_1 = 0;
bool positiveDir = true; // if the shaft is to be spun in a positive direction
int16_t y = 0;//
int16_t y_1 = 0;
int32_t yw = 0;//
int32_t yw_1 = 0;
int16_t advance = 0;//
int32_t wrap_count = 0;//
int32_t error = 0;//
int32_t iterm = 0;//
int32_t dterm = 0;//
int16_t u = 0;//
int32_t stepNum = 0;//
uint8_t stepangle=0;//

uint16_t hccount=0;//
bool closedLoopMode;//
uint8_t enmode=0;//

uint8_t Menu_Num = 0;
uint8_t Menu_Num_item=0;         //
uint8_t Menu_move_num=0;         //
volatile bool menuUpdateFlag = true;      //
volatile bool dataUpdateFlag = true; //
volatile uint16_t Data_update_Count =0; //

uint8_t CalibrateEncoder_finish_flag=0; //
uint8_t Second_Calibrate_flag=0;        //
uint8_t Second_Menu=0;                  //
uint8_t Menu_Num2_item=0;               //  
uint8_t Menu_Num3_item=0;               //
uint8_t Menu_Num4_item=0;               //  
uint8_t Menu_Num5_item=0;               // 
uint8_t Menu_Num6_item=0;               // 

int16_t Motor_speed = 0;
int16_t wap1=0;
int16_t wap2=0;
uint8_t Motor_speed_flag = 0; 

uint8_t Currents = 0;
uint8_t Currents_Set=0;
uint8_t Microstep_Set=0;            //->
uint8_t Microstep=0;                //->
uint8_t Dir_Enable=0x00;           // 
                                    //!ENABLE = 
                                    //ENABLE = 
bool positiveDirection = false;               //

bool motorEnabled = false; // If the motor should be enabled
volatile bool enableModeFlag = false; //
volatile uint8_t enter1_once_flag =1;    //
volatile uint8_t enter2_once_flag =0;

volatile bool dir1_once_flag = true;      //
volatile bool dir2_once_flag = false;

volatile bool flashStoreFlag = false;    //
uint16_t table1[14];                    //
volatile uint8_t resetStatusFlag = 0; // CANNOT be boolean

uint8_t Rx1_buff[9];
uint8_t Receive_Count=0;                //    
uint8_t Rx1_temp_num=0;                 //
uint8_t Receive_finish_flag=0;          //

volatile uint8_t Communications_Process_flag=0;     //
volatile uint8_t uartCRCFlag=0;                   //  
volatile uint8_t frameErrorFlag=0;                //
volatile uint8_t uartCRCCorrectFlag=0;           //
volatile uint8_t info_report_flag=0;
//uint8_t Receive_statu=0x00;
volatile int16_t temp=0;
uint8_t receivePulse =0;

/* Private function prototypes -----------------------------------------------*/
uint16_t table[]={'1','2','3','4','5','6','7','8','9','A','B','C','D','E','F','0'};

/* Private functions ---------------------------------------------------------*/
void flash_test(void);
void restart_init(void);

/**
  * @brief  main program
  * @param  None
  * @retval None
  */
int main(void)
{
	static uint8_t res = 0;
    uint8_t canbufTx[]="12345678";
    
    // Setup the system clock (includes overclocking)
    System_Clock_Init();

    // Initialize the delay timer
    delayInit();

    // Set the priority of execution
    NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
  
    // Initialize the LED and OLED
	LED_Init();
	OLED_Init();

    // Give the user feeback that OLED starting has been successful
    OLED_ShowString(0,0,"Oled Init...OK");

    // Wait for .1 seconds so everything can boot
    delayMs(100);

    // Initialize the buttons (for menu)
    Key_init();
    
    // Test the flash if specified
    #ifdef TEST_FLASH
        flash_test(); 
    #endif

    // Setup the closed loop information if it is enabled 
    #ifdef CLOSED_LOOP_CONFIG

        // Clear the display, then write that we're using the closed loop model
        OLED_Clear();
        OLED_ShowString(0,0,"Close Loop Model");

        // Initialize the encoder, motor, and the PWM timer
        encoderInit();
        motorInit();
        TIM3_Init();
        
        if(isCalibrated()){
            OLED_ShowString(0,25,"  Calibration  ");
            OLED_ShowString(40,45,"  OK!");
            delayMs(500);
            //
            OLED_Clear();   //
            OLED_ShowString(0,2,"Simp:  0000 RPM");//
            OLED_ShowString(0,22," Err:  000.00 ");
            OLED_ShowString(0,42," Deg:  0000.0");
            Menu_Num_item=2;                                        //
            Second_Menu=1;                                          //
            menuUpdateFlag = false;                                     //
            Menu_Num=1;                                             //   
            resetStatusFlag=1;                                    //
    //        Menu_Num2_item=8;                                       //
            //Motor_Enable = enmode;                                //
    /*****************************************************/

            flashRead(DATA_STORE_ADDRESS, table1, 14);            //
            SetModeCheck();                 //

            Currents=table1[1];             //
            Menu_Num2_item =table1[2];
            
            stepangle=table1[3];             //
            Menu_Num3_item=table1[4];        //
            
            enableModeFlag = table1[5];
            Menu_Num4_item= table1[6];  
            
            positiveDirection =table1[7];            //
            Menu_Num5_item =table1[8];
            
    //        Motor_Dir =table1[9];           //
    //        Menu_Num5_item =table1[10];

            kp = table1[11];                //Kp
            ki = table1[12];                //Ki
            kd = table1[13];                //Kd
        }
        else{//
            OLED_ShowString(48,16,"NOT");
            OLED_ShowString(16,32,"Calibration");
            OLED_ShowString(0,48,"Please calibrate");
            delayMs(500);
            //
            OLED_Clear();
            OLED_ShowString(0,0,"->");

            while(1){
                KeyScan();
                Oled_display();
            }            
        }

        EXTIX_Init();                       //
        NVIC_EnableIRQ(EXTI1_IRQn);         //
        UART_Configuration(USART1, UART1_DEFAULT_BAUDRATE);     //
        CAN1_Mode_Init(CAN_SJW_1tq,CAN_BS2_6tq,CAN_BS1_5tq,6,CAN_Mode_LoopBack);// 
        res=CAN1_Send_Msg(canbufTx,8);                      //
        delayMs(100);
        if(res){                                            //
            printf("CAN1 Transport fail!\r\n");
        }
        else{
            printf("CAN1 Transport OK!\r\n");
        }
        TIM2_Cap_Init(0xFFFF,0);          //
        TIM4_Init(7200-1, 0);             //
    #endif    
//    IWDG_Init(4,625);                 //
 	while(1) {
        #ifdef CLOSED_LOOP 
            if(enableModeFlag){
                if(getEnablePin()) {
                    restart_init();              
                } 
                else{
                    resetStatusFlag++;
                    enmode = 0;
                }    
            }
            else {
                if(!getEnablePin()){
                    restart_init();
                }
                else{
                    resetStatusFlag++;
                    enmode=0; //
                }
            }
            if(resetStatusFlag == 1){       //
                enmode = 0;
                resetStatusFlag++;           //1++            
                PID_Cal_value_init();           //
                    
                wap1 = 0;
                wap2 = 0;
                dataUpdateFlag = true;
            }
            else{
                if(resetStatusFlag > 3)
                    resetStatusFlag--;
            }
            
            usart_Receive_Process();                        //
            data_Process();
            test_uart();
            
            if(frameErrorFlag){
                frameErrorFlag =0;
                USART1_SendStr("Frame Err!\r\n");   
            }
            if(flashStoreFlag){
                flashStoreFlag = false;

                //USART_Cmd(USART1, DISABLE);
                USART_ITConfig(USART1, USART_IT_RXNE, DISABLE);
                USART_ITConfig(USART1, USART_IT_IDLE, DISABLE);
                
                flashWrite(DATA_STORE_ADDRESS, table1, 14);//
                
                //USART_Cmd(USART1, ENABLE);
                USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);
                USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);
                
                //Reset_status_flag=1;
                //restart_init();
            }
            KeyScan();                                      //
            Oled_display();                                 //
            Motor_data_dis();                               //
    #endif
#if 0
        if(KEY_Select==0){
			delay_ms(10);
			if(KEY_Select==0){
                if(k3_flag == 0){
                    k3_flag =1;
                    led1=!led1;			//
                    res=CAN1_Send_Msg(canbufTx,8);//
                    if(res)
                        printf("CAN1 Transport fail!\r\n");
                    else
                        printf("OK!\r\n");
                }
			}
		}
        else if(KEY_Confirm==0){
			delay_ms(10);
			if(KEY_Confirm==0){
                if(k4_flag == 0){
                    k4_flag =1; 
                    mode=!mode;
                    CAN1_Mode_Init(CAN_SJW_1tq,CAN_BS2_6tq,CAN_BS1_5tq,6,mode);	//
                     
                    if(mode==0)//
                    {
                        printf("Nnormal Mode \r\n");	    
                    }else //
                    {
                        printf("LoopBack Mode\r\n");
                    }                    
                    led1=1;			//off
                    delay_ms(200);
                    led1=0;			//on
                    delay_ms(200);
                    led1=1;			//off
                    delay_ms(200);
                    led1=0;			//on
                    delay_ms(200);
                }
            }
		}
        else{
            k3_flag = 0;
            k4_flag = 0;
        }
        key=CAN1_Receive_Msg(canbufRx);
		if(key)//
		{	
            printf("CAN Receive: ");	
 			for(i=0;i<key;i++){		
                printf("%x ",canbufRx[i]);
 			}
            printf("\r\n");
		}
        key=0;
#endif
	}
}
//
void restart_init(void)
{
    //
    if(resetStatusFlag !=0 ){
        CLEAR_BIT(TIM2->CR1, TIM_CR1_CEN); 
        PID_Cal_value_init();           //
        SET_BIT(TIM2->CR1, TIM_CR1_CEN);
    }
    enmode=1;
    resetStatusFlag=0;
}
#ifdef ENFLASH_TEST 
void flash_test(void)
{
    static char t=0;
    STMFLASH_Write(Data_Store_Arrdess,table,16);
    delay_ms(10);
    FLASH_Unlock();						//
    STMFLASH_Read(Data_Store_Arrdess,table1,16);
    FLASH_Lock();//
     printf("flash Read: ");	
    for(t=0;t<16;t++){		
        printf("%c ",table1[t]);
    }
    while(1);
}
#endif























/******************* (C) COPYRIGHT 2016 STMicroelectronics *****END OF FILE****/
