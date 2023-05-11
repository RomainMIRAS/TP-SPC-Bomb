#include <stdio.h>
#include <math.h>
#include <string.h>
#include "sys/cm4.h"
#include "sys/devices.h"
#include "sys/init.h"
#include "sys/clock.h"

static volatile char c=0;

void tempo_500ms(){
	volatile uint32_t duree;
	/* estimation, suppose que le compilateur n'optimise pas trop... */
	for (duree = 0; duree < 5600000 ; duree++){
		;
	}

}

void init_LD2(){
	/* on positionne ce qu'il faut dans les différents
	   registres concernés */
	RCC.AHB1ENR |= 0x01;
	GPIOA.MODER = (GPIOA.MODER & 0xFFFFF3FF) | 0x00000400;
	GPIOA.OTYPER &= ~(0x1<<5);
	GPIOA.OSPEEDR |= 0x03<<10;
	GPIOA.PUPDR &= 0xFFFFF3FF;
}

void init_PB(){
	/* GPIOC.MODER = ... */
	GPIOC.MODER = (GPIOC.MODER & ~(0x3<<26));
}

int is_button_pressed(){
	/* renvoie 1 si button pressed (IDR13 = 0) */
	return (GPIOC.IDR & (0x1<<13)) == 0;

}

/**
 * Allume la LED LD2 Pour la première partie de la question 1
 */
void allumer_LED_infini(){
	/* 
		init_L2()
		while vrai
			si bouton push (voir avec IDR) alors
				allumer LED
	*/
	while(1){
		if(is_button_pressed()){
			GPIOA.ODR = GPIOA.ODR |(0x0020);
		}
		else {
			GPIOA.ODR = GPIOA.ODR & ~(0x0020);
		}
	}
}

void clignoter_LED(){
	while(1){
		GPIOA.ODR = GPIOA.ODR |(0x0020);
		tempo_500ms();
		GPIOA.ODR = GPIOA.ODR & ~(0x0020);
		tempo_500ms();
	}
}

void allumer_clignoter_LED(){
	while(1){
		if(is_button_pressed()){
			GPIOA.ODR = GPIOA.ODR |(0x0020);
		}
		else {
			GPIOA.ODR = GPIOA.ODR & ~(0x0020);
			for(int i=0; i<4; i++){
				tempo_500ms();
			}
			GPIOA.ODR = GPIOA.ODR |(0x0020);
			for(int i=0; i<4; i++){
				tempo_500ms();
			}
		}
	}
}

/* Initialisation du timer système (systick) */
void systick_init(uint32_t freq){
	uint32_t p = get_SYSCLK()/freq;
	SysTick.LOAD = (p-1) & 0x00FFFFFF;
	SysTick.VAL = 0;
	SysTick.CTRL |= 7;
}

int main() {
  
	printf("\e[2J\e[1;1H\r\n");
	printf("\e[01;32m*** Welcome to Nucleo F446 ! ***\e[00m\r\n");

	printf("\e[01;31m\t%08lx-%08lx-%08lx\e[00m\r\n",
	       U_ID[0],U_ID[1],U_ID[2]);
	printf("SYSCLK = %9lu Hz\r\n",get_SYSCLK());
	printf("AHBCLK = %9lu Hz\r\n",get_AHBCLK());
	printf("APB1CLK= %9lu Hz\r\n",get_APB1CLK());
	printf("APB2CLK= %9lu Hz\r\n",get_APB2CLK());
	printf("\r\n");
	
	init_LD2();
	init_PB();

	while (1){
		// Pour la première partie de la question 1
		//allumer_LED_infini();

		// Pour la troisième partie de la question 1
		allumer_clignoter_LED();
	}
	
	return 0;
}

