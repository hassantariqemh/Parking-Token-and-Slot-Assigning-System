#include "stm32f10x.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_gpio.h"
#include "delay.c"
#include "lcd16x2.c"

void button_init();
void configureLEDpins(void);
void configureKeyPadpins(void);
void Display_On_7_Segment(void);
void ms_Delay(uint32_t mstime);

void SetColumnsLow(void);
void SetColumnsHigh(void);
void SetColumn_1(void);
void SetColumn_2(void);
void SetColumn_3(void);
void SetColumn_4(void);

char KeyPad_Scan(void);

void BuzzerBeep(void);
void configureBuzzer(void);

int find_free_slot(void);
void LCD_Show(int token, int slot_no);
void configure_7segment(void);
void display_C_7segment(void);
void display_O_7segment(void);
void display_dash_7segment(void);



int Col_1=0;
int Col_2=0;
int Col_3=0;
int Col_4=0,i;

#define TOTAL_SLOTS 9
int slot[TOTAL_SLOTS] = {0};   // 0 = free, 1 = occupied
int token_no = 0;
char key;
int free_slot;
	
GPIO_InitTypeDef myLED;
GPIO_InitTypeDef BuzzerGPIO;

int main(void)
{	
	lcd16x2_init(LCD16X2_DISPLAY_ON_CURSOR_ON_BLINK_OFF);
		configureKeyPadpins();
	  configureBuzzer();
	  configure_7segment();
	button_init();
	lcd16x2_clrscr();
	lcd16x2_puts("Parking System");
	BuzzerBeep();     // Beep
	display_dash_7segment();
	DelayMs(10);
	SetColumnsLow();
	SetColumnsHigh();
	while(1){	
    		/* -------- Scan Keypad Column 4 -------- */
    		SetColumn_4();
    		DelayMs(1);
    		key = KeyPad_Scan();

    		/* -------- A : Assign Token -------- */
		if(key == 'A'){			
 	       		lcd16x2_clrscr();
   	     		lcd16x2_puts("Confirm #");
			lcd16x2_gotoxy(0,2);
			lcd16x2_puts("Cancel *");
			// Wait for # key
    			while(1){
    				SetColumn_3();
    				DelayMs(100);
    				key = KeyPad_Scan();
				if(key=='#'){
					// Assign token
    					token_no++;
    					free_slot = find_free_slot();
    					if(free_slot != -1){
       		 			slot[free_slot] = 1;
        				LCD_Show(token_no, free_slot);  // Show token & slot on LCD
        				BuzzerBeep();
    					}
    					else{
        				lcd16x2_clrscr();
        				lcd16x2_puts("Parking Full");
        				BuzzerBeep();
					}
    					DelayMs(2000); 	
					lcd16x2_clrscr();
					lcd16x2_puts("Parking System");
					BuzzerBeep();
					while(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_10)!=0);
					display_O_7segment();
					DelayMs(1000);
					display_C_7segment();
					break;
				}
				SetColumn_1();
    				DelayMs(100);
    				key = KeyPad_Scan();
				if(key=='*'){
					lcd16x2_clrscr();
					lcd16x2_puts("Parking System");
					break;
				}
			}
		}
    		key = KeyPad_Scan();

    		/* -------- B : Reset Token -------- */
    		if(key == 'B')
    		{
			lcd16x2_clrscr();
        		lcd16x2_puts("Confirm #");
			lcd16x2_gotoxy(0,2);
			lcd16x2_puts("Cancel *");
			while(1){
				SetColumn_3();
    				DelayMs(1);
    				key = KeyPad_Scan();
				if(key=='#'){
					token_no = 0;
 					lcd16x2_clrscr();
        				lcd16x2_puts("Token Reset");
   					DelayMs(2000); 
					lcd16x2_clrscr();
					lcd16x2_puts("Parking System");
        				BuzzerBeep();
					break;
				}
				SetColumn_1();
   				DelayMs(100);
    				key = KeyPad_Scan();
				if(key=='*'){
					lcd16x2_clrscr();
					lcd16x2_puts("Parking System");
					break;
				}
        			DelayMs(10);
   			}
		}	
    		key = KeyPad_Scan();
			/* -------- C : Reset Slots -------- */
    		if(key == 'C'){
			lcd16x2_clrscr();
        		lcd16x2_puts("Confirm #");
			lcd16x2_gotoxy(0,2);
			lcd16x2_puts("Cancel *");
			while(1){
				SetColumn_3();
    				DelayMs(1);
    				key = KeyPad_Scan();
				if(key=='#'){
					for(i=0;i<9;i++){
						slot[i] = 0;
					}
        			lcd16x2_clrscr();
        			lcd16x2_puts("All Slot Free");
        			BuzzerBeep();
				DelayMs(2000);
				lcd16x2_clrscr();
				lcd16x2_puts("Parking System");
        			BuzzerBeep();
				break;
				}
				SetColumn_1();
    				DelayMs(100);
    				key = KeyPad_Scan();
				if(key=='*'){
					lcd16x2_clrscr();
					lcd16x2_puts("Parking System");
					break;
				}

    			}  
    		}
    		/* -------- D : Free Slot -------- */
    		if(key == 'D'){
			char s;
			lcd16x2_clrscr();
        		lcd16x2_puts("Confirm #");
			lcd16x2_gotoxy(0,2);
			lcd16x2_puts("Cancel *");
			while(1){
				SetColumn_3();
    				DelayMs(1);
    				key = KeyPad_Scan();
				if(key=='#'){
					lcd16x2_clrscr();
        				lcd16x2_puts("Free Slot:");
        				while(1){
            					SetColumn_1();
            					DelayMs(10);
            					s = KeyPad_Scan();
						if(s=='1'||s=='4'||s=='7'){
							break;
						}
            					SetColumn_2();
           	 				DelayMs(10);
            					s = KeyPad_Scan();
						if(s=='2'||s=='5'||s=='8'){
							break;
						}
           					SetColumn_3();
            					DelayMs(10);
            					s = KeyPad_Scan();
						if(s=='3'||s=='6'||s=='9'){
							break;
						}
					}
        				slot[s - '1'] = 0;
        				lcd16x2_clrscr();
        				lcd16x2_puts("Slot Freed");
				        BuzzerBeep();
					DelayMs(2000);
					lcd16x2_clrscr();
        				BuzzerBeep();
					lcd16x2_puts("Parking System");
					while(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_10)!=0);
					display_O_7segment();
					DelayMs(1000);
					display_C_7segment();
					break;
				}
				SetColumn_1();
    				DelayMs(100);
    				key = KeyPad_Scan();
				if(key=='*'){
					lcd16x2_clrscr();
					lcd16x2_puts("Parking System");
					break;
				}
			}	
    		}
	}
}




void configureKeyPadpins(void)
{
    RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);

    // Columns (output: PA4-PA7)
    myLED.GPIO_Speed = GPIO_Speed_2MHz;
    myLED.GPIO_Mode = GPIO_Mode_Out_PP;
    myLED.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5 | GPIO_Pin_6 | GPIO_Pin_7;
    GPIO_Init(GPIOA, &myLED);

    // Rows (input: PA0-PA3)
    myLED.GPIO_Speed = GPIO_Speed_2MHz;
    myLED.GPIO_Mode = GPIO_Mode_IPD;
    myLED.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 | GPIO_Pin_3;
    GPIO_Init(GPIOA, &myLED);
}

void SetColumnsHigh(void)
{
	GPIO_WriteBit(GPIOA, GPIO_Pin_4, 1);
    GPIO_WriteBit(GPIOA, GPIO_Pin_5, 1);
    GPIO_WriteBit(GPIOA, GPIO_Pin_6, 1);
    GPIO_WriteBit(GPIOA, GPIO_Pin_7, 1);
    
    Col_1 = 1;
    Col_2 = 1;
    Col_3 = 1;
    Col_4 = 1;
}

void SetColumnsLow(void)
{
		GPIO_WriteBit(GPIOA, GPIO_Pin_4, 0);
    GPIO_WriteBit(GPIOA, GPIO_Pin_5, 0);
    GPIO_WriteBit(GPIOA, GPIO_Pin_6, 0);
    GPIO_WriteBit(GPIOA, GPIO_Pin_7, 0);
    
    Col_1 = 0;
    Col_2 = 0;
    Col_3 = 0;
    Col_4 = 0;
}
void SetColumn_1(void)
{
	  GPIO_WriteBit(GPIOA, GPIO_Pin_4, 1);
    GPIO_WriteBit(GPIOA, GPIO_Pin_5, 0);
    GPIO_WriteBit(GPIOA, GPIO_Pin_6, 0);
    GPIO_WriteBit(GPIOA, GPIO_Pin_7, 0);
    
    Col_1 = 1;
    Col_2 = 0;
    Col_3 = 0;
    Col_4 = 0;
}
void SetColumn_2(void)
{
	  GPIO_WriteBit(GPIOA, GPIO_Pin_4, 0);
    GPIO_WriteBit(GPIOA, GPIO_Pin_5, 1);
    GPIO_WriteBit(GPIOA, GPIO_Pin_6, 0);
    GPIO_WriteBit(GPIOA, GPIO_Pin_7, 0);
    
    Col_1 = 0;
    Col_2 = 1;
    Col_3 = 0;
    Col_4 = 0;
}
void SetColumn_3(void)
{
	  GPIO_WriteBit(GPIOA, GPIO_Pin_4, 0);
    GPIO_WriteBit(GPIOA, GPIO_Pin_5, 0);
    GPIO_WriteBit(GPIOA, GPIO_Pin_6, 1);
    GPIO_WriteBit(GPIOA, GPIO_Pin_7, 0);
    
    Col_1 = 0;
    Col_2 = 0;
    Col_3 = 1;
    Col_4 = 0;
}

void SetColumn_4(void)     
{
    GPIO_WriteBit(GPIOA, GPIO_Pin_4, 0);
    GPIO_WriteBit(GPIOA, GPIO_Pin_5, 0);
    GPIO_WriteBit(GPIOA, GPIO_Pin_6, 0);
    GPIO_WriteBit(GPIOA, GPIO_Pin_7, 1);
    
    Col_1 = 0;
    Col_2 = 0;
    Col_3 = 0;
    Col_4 = 1;
}

char KeyPad_Scan(void)
{
    if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_0)==1) 
    {
        if(Col_1==1) return '1';
        if(Col_2==1) return '2';
        if(Col_3==1) return '3';
        if(Col_4==1) return 'A';
    }

    if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_1)==1) 
    {
        if(Col_1==1) return '4';
        if(Col_2==1) return '5';
        if(Col_3==1) return '6';
        if(Col_4==1) return 'B';
    }

    if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_2)==1) 
    {
        if(Col_1==1) return '7';
        if(Col_2==1) return '8';
        if(Col_3==1) return '9';
        if(Col_4==1) return 'C';
    }

    if(GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_3)==1) 
    {
        if(Col_1==1) return '*';
        if(Col_2==1) return '0';
        if(Col_3==1) return '#';
        if(Col_4==1) return 'D';
    }

    return 0;
}


void configureBuzzer(void)
{
     // Enable GPIOC clock
    RCC->APB2ENR |= (1 << 4);  // IOPCEN
    // PC13 Output Push-Pull, 2MHz
    GPIOC->CRH &= ~(0xF << ((13-8)*4));
    GPIOC->CRH |=  (0x1 << ((13-8)*4));  // Output push-pull, 2MHz
}

void BuzzerBeep(void)
{
    GPIOC->BSRR = (1 << 13);  // Turn ON 
    DelayMs(500);
    GPIOC->BRR  = (1 << 13);  // Turn OFF
}

/*------------------ Find Free Slot ------------------*/
int find_free_slot(void)
{
    int i;
    for(i = 0; i < TOTAL_SLOTS; i++)
        if(slot[i] == 0) return i;
    return -1;
}

/*------------------ Display Token & Slot on LCD ------------------*/
void LCD_Show(int token, int slot_no)
{
    lcd16x2_clrscr();
    lcd16x2_puts("Token:");
    lcd16x2_putc(token + '0');
    lcd16x2_gotoxy(0,1);
    lcd16x2_puts("Slot:");
    lcd16x2_putc(slot_no + '1');
}

/*------------------ 7-Segment Display ------------------*/

void configure_7segment(void)
{
	// Enable GPIOB clock
	RCC->APB2ENR |= (1<<3); // IOPBEN = 1
	GPIOB->CRL = 0x33333333;
	GPIOB->CRH = 0x33333433;
	GPIOB->ODR |= (1<<0)|(1<<1)|(1<<5)|(1<<6)|(1<<7)|(1<<8)|(1<<9); // all OFF
}

void display_C_7segment(void)
{		
	//Make C on 7 segment
	GPIOB->ODR |= (1<<1) | (1<<5) | (1<<9);   // Turn OFF B, C, G
	GPIOB->ODR &= ~((1<<0) | (1<<6) | (1<<7) | (1<<8)); // Turn ON A, D, E, F
}
void display_O_7segment(void)
{
	//Make O on 7 segment	
	// Turn ON A, B, C, D, E, F 
	GPIOB->ODR &= ~((1<<0) | (1<<1) | (1<<5) | (1<<6) | (1<<7) | (1<<8));
	// Turn OFF G
	GPIOB->ODR |= (1<<9);

}
void display_dash_7segment(void)
{
	//Make dash(-) on 7 segment
	// Turn OFF all segments except G
	GPIOB->ODR |= (1<<0) | (1<<1) | (1<<5) | (1<<6) | (1<<7) | (1<<8); // OFF A-F	
	GPIOB->ODR &= ~(1<<9); // ON G

}

void button_init(){
    // PB10
    myLED.GPIO_Speed = GPIO_Speed_2MHz;
    myLED.GPIO_Mode = GPIO_Mode_IPU;
    myLED.GPIO_Pin = GPIO_Pin_10;
    GPIO_Init(GPIOB, &myLED);
	
}
