#include <stdio.h>
#include <math.h>
#include <string.h>
#include "sys/cm4.h"
#include "sys/devices.h"
#include "sys/init.h"
#include "sys/clock.h"

#define WIN 2
#define PLAYING 0
#define LOSE 1

static volatile char c=0;

static volatile int timer_second = 0;

static volatile int frequence_clinio = 1000;

static volatile int frequence_timer = 0;

static volatile int second_passed = 0;

static volatile int game_state = PLAYING;

void tempo_500ms();

void init_LD2(){
	/* on positionne ce qu'il faut dans les différents
	   registres concernés */
	RCC.AHB1ENR |= 0x01;
	GPIOA.MODER = (GPIOA.MODER & 0xFFFFF3FF) | 0x00000400;
	GPIOA.OTYPER &= ~(0x1<<5);
	GPIOA.OSPEEDR |= 0x03<<10;
	GPIOA.PUPDR &= 0xFFFFF3FF;
}

void init_Tricolor_Led(){
	/* on positionne ce qu'il faut dans les différents
	   registres concernés */
	RCC.AHB1ENR |= 0x01;

	GPIOA.OTYPER &= ~(0x1<<8);
	GPIOA.OSPEEDR |= 0x03<<10;

	/*
	//PARTIE ROUGE
	GPIOA.MODER = (GPIOA.MODER & 0xFFFCFFFF) | 0x00010000;
	GPIOA.PUPDR &= 0xFFFCFFFF;

	// PARTIE VERT
	GPIOA.MODER = (GPIOA.MODER & 0xFFF3FFFF) | 0x00040000;
	GPIOA.PUPDR &= 0xFFF3FFFF;

	// PARTIE BLEU
	GPIOA.MODER = (GPIOA.MODER & 0xFFCFFFFF) | 0x00100000;
	GPIOA.PUPDR &= 0xFFCFFFFF;
	*/

	// PARTIE ALL
	GPIOA.MODER = (GPIOA.MODER & 0xFFC0FFFF) | 0x00150000;
	GPIOA.PUPDR &= 0xFFC0FFFF;

}

void init_PB(){
	/* GPIOC.MODER = ... */
	GPIOC.MODER = (GPIOC.MODER & ~(0x3<<26));

}

int is_button_pressed(){
	/* renvoie 1 si button pressed (IDR13 = 0) */
	return (GPIOC.IDR & (0x1<<13)) == 0;

}

void allumer_LED_infini(){
	/* 
		init_L2()
		while vrai
			si bouton push (voir avec IDR) alors
				allumer LED
	*/
	
	init_LD2();
	init_PB();

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
	init_LD2();
	while(1){
		GPIOA.ODR = GPIOA.ODR |(0x0020);
		tempo_500ms();
		GPIOA.ODR = GPIOA.ODR & ~(0x0020);
		tempo_500ms();
	}
}

void allumer_clignoter_LED(){
	init_LD2();
	init_PB();

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

void tempo_500ms(){
	volatile uint32_t duree;
	/* estimation, suppose que le compilateur n'optimise pas trop... */
	for (duree = 0; duree < 5600000 ; duree++){
		;
	}

}

void init_USART(){
	GPIOA.MODER = (GPIOA.MODER & 0xFFFFFF0F) | 0x000000A0;
	GPIOA.AFRL = (GPIOA.AFRL & 0xFFFF00FF) | 0x00007700;
	USART2.BRR = get_APB1CLK()/9600;
	USART2.CR3 = 0;
	USART2.CR2 = 0;
}

void _putc(const char c){
	while( (USART2.SR & 0x80) == 0);  
	USART2.DR = c;
}

void _puts(const char *c){
	int len = strlen(c);
	for (int i=0;i<len;i++){
		_putc(c[i]);
	}
}

char _getc(){
	/* À compléter */
	char c;
	scanf("%c",&c);	
	return c;
}


void affichage_clavier(){	
	while(1){
		_putc(_getc());	
	}
}


void ecrire_carac(){
	_putc('c');
	_putc(0x0A);
	_putc('o');
	//_putc(0x0D);
	_putc('u');
	//_putc(0x0A);
	_putc('c');
	_putc(0x0A);
	_putc(0x0D);
	_putc('o');
}

void ecrire_carac_puts(){
	_puts("coucou");
}

/* Initialisation du timer système (systick) */
void systick_init(uint32_t freq){
	uint32_t p = get_SYSCLK()/freq;
	SysTick.LOAD = (p-1) & 0x00FFFFFF;
	SysTick.VAL = 0;
	SysTick.CTRL |= 7;
}

void __attribute__((interrupt)) SysTick_Handler(){
	/* Le fait de définir cette fonction suffit pour
	 * qu'elle soit utilisée comme traitant,
	 * cf les fichiers de compilation et d'édition de lien
	 * pour plus de détails.
	 */
	/* ... */

	/*
	GPIOA.ODR = GPIOA.ODR |(0x0100); // ROUGE
	GPIOA.ODR = GPIOA.ODR |(0x0200); // VERT
	GPIOA.ODR = GPIOA.ODR |(0x0400); // BLEU */

	timer_second++;
	frequence_timer++;
	if (timer_second >= 1000){ // Chaque seconde
		timer_second = 0;
		second_passed++;
		if (second_passed >= 10) {
			GPIOA.ODR = GPIOA.ODR |(0x0100); // ALLUMER ROUGE
			GPIOA.ODR = GPIOA.ODR & ~(0x0400);  // STOP BLEU
			game_state = LOSE;
		} else if (second_passed % 2 == 0) { 
			frequence_clinio = frequence_clinio / 2;		
		}
	}


	if (frequence_timer >= frequence_clinio && game_state == PLAYING){
		frequence_timer= 0;
		GPIOA.ODR = GPIOA.ODR ^ (0x0400); // BLEU
	}

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
	init_Tricolor_Led();
	systick_init(1000); // Traitant toutes les millisecondes

	while (1){
		
	}
	
	return 0;
}

