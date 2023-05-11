#include <stdio.h>
#include <math.h>
#include <string.h>
#include "sys/cm4.h"
#include "sys/devices.h"
#include "sys/init.h"
#include "sys/clock.h"

static volatile char c=0;
static volatile int  traitant = 0;
static volatile int frequence_clinio = 500;

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

	// INTERRUPTIONS USART2
	USART2.CR1 |= 0x20;
	USART2.CR1 |= 0x0020;
	USART2.CR1 |= 0x0001;
}

void _putc(const char c){
	while( (USART2.SR & 0x80) == 0);  
	USART2.DR = c;
}

void _puts(const char *c){
	while(*c != '\0'){
		_putc(*c);
		c++;
	}
}

char _getc(){
	while( (USART2.SR & 0x20) == 0);  
	return USART2.DR;
}

void affichage_clavier(){	
	while(1){
		_putc(_getc());	
	}
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
	
	// Allumer la led dans le traitant
	traitant++;
	if (traitant >= frequence_clinio){
		traitant = 0;
		GPIOA.ODR = GPIOA.ODR ^ (0x0020);
	} 
}

// INTERRUPTIONS USART
void __attribute__((interrupt)) USART2_Handler(){
	/* Le fait de définir cette fonction suffit pour
	 * qu'elle soit utilisée comme traitant,
	 * cf les fichiers de compilation et d'édition de lien
	 * pour plus de détails.
	 */
	printf("1000\n");
	if (USART2.SR & 0x20){
		frequence_clinio = 1000;
		printf("1000\n");
	} else {
		frequence_clinio = 500;
	}
}

/* Fonction non bloquante envoyant une 	chaîne par l'UART */
int _async_puts(const char* s) {
	/* Cette fonction doit utiliser un traitant d'interruption
	 * pour gérer l'envoi de la chaîne s (qui doit rester
	 * valide pendant tout l'envoi). Elle doit donc être
	 * non bloquante (pas d'attente active non plus) et
	 * renvoyer 0.
	 *
	 * Si une chaîne est déjà en cours d'envoi, cette
	 * fonction doit renvoyer 1 (et ignorer la nouvelle
	 * chaîne).
	 *
	 * Si s est NULL, le code de retour permet de savoir
	 * si une chaîne est encore en cours d'envoi ou si
	 * une nouvelle chaîne peut être envoyée.
	 */
	/* À compléter */
	return 1;
	}

void init_all(){
	init_LD2();
	init_PB();
	init_USART();
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

	init_all();

	GPIOA.ODR = GPIOA.ODR | (0x0020); // Allumer la led

	systick_init(1000); // Toute les millisecondes


	while (1){

		// PARTIE BOUTON

		// if(is_button_pressed()){
		// 	frequence_clinio = 250;
		// }
		// else {
		// 	frequence_clinio = 500;
		// }

		// PARTIE USART SANS INTERRUPTION

		//  if(USART2.SR & 0x20){
		// 	frequence_clinio = 1000;
		//  }
		//  else {
		// 	frequence_clinio = 500;
		//  }
	}
			
	
	return 0;
}

