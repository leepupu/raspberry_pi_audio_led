#include <wiringPi.h>
#include <stdlib.h>
#include <stdio.h>

#define CUBE_OE 4         //wiringPi pin 1 is BCM_GPIO 18.11
#define CUBE_CK 5
#define CUBE_DT 6
#define LED_L_1 2
#define LED_L_2 3
#define LED_L_3 0
#define LED_L_4 1
#define LED_L_5 7

int cube_shape[5][25]={ {0,1,0,1,0, 1,0,1,0,1, 0,1,0,1,0, 1,0,1,0,1 ,0,1,0,1,0},
			{1,0,1,0,1, 0,1,0,1,0, 1,0,1,0,1, 0,1,0,1,0 ,1,0,1,0,1}, 
			{0,1,0,1,0, 1,0,1,0,1, 0,1,0,1,0, 1,0,1,0,1 ,0,1,0,1,0},
                        {1,0,1,0,1, 0,1,0,1,0, 1,0,1,0,1, 0,1,0,1,0 ,1,0,1,0,1},
			{0,1,0,1,0, 1,0,1,0,1, 0,1,0,1,0, 1,0,1,0,1 ,0,1,0,1,0}};

void st_pin(void){
	pinMode(LED_L_1,OUTPUT);
	pinMode(LED_L_2,OUTPUT);
	pinMode(LED_L_3,OUTPUT);
	pinMode(LED_L_4,OUTPUT);
	pinMode(LED_L_5,OUTPUT);
	pinMode(CUBE_OE,OUTPUT);
	pinMode(CUBE_CK,OUTPUT);
	pinMode(CUBE_DT,OUTPUT);
	digitalWrite(CUBE_OE,0);
	digitalWrite(CUBE_CK,1);
	digitalWrite(CUBE_DT,0);
	digitalWrite(LED_L_1,1);
	digitalWrite(LED_L_2,1);
	digitalWrite(LED_L_3,1);
	digitalWrite(LED_L_4,1);
	digitalWrite(LED_L_5,1);
}

//
void Layer_Clean(void){
	digitalWrite(LED_L_1,1);
        digitalWrite(LED_L_2,1);
        digitalWrite(LED_L_3,1);
        digitalWrite(LED_L_4,1);
        digitalWrite(LED_L_5,1);
}


void cube_output(int the_shape[][25])
{
	int i, j;
	for(i=0;i<5;i++)
	{
		for(j=0;j<24;j++)
		{
			digitalWrite(CUBE_CK, 0);
			digitalWrite(CUBE_DT,the_shape[i][j]);
//			delayMicroseconds(1);
			digitalWrite(CUBE_CK, 1);
//			delayMicroseconds(1);
			digitalWrite(CUBE_CK, 0);
		}
		digitalWrite(CUBE_DT,the_shape[i][24]);
		if(i==0)digitalWrite(LED_L_1,0);
	        if(i==1)digitalWrite(LED_L_2,0);
	        if(i==2)digitalWrite(LED_L_3,0);
	        if(i==3)digitalWrite(LED_L_4,0);
        	if(i==4)digitalWrite(LED_L_5,0);
		delay(2);
//		delayMicroseconds(3100);
		Layer_Clean();
	}
	
}


//int main(void){
//	wiringPiSetup();
//	printf("asdf");
//	int i=0;
//	st_pin();
//	for(;;)
//	{
//		cube_output(cube_shape);
//	}
//}
