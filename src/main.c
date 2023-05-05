#include <stdio.h>
#include <math.h>
#include <string.h>
#include "sys/cm4.h"
#include "sys/devices.h"
#include "sys/init.h"
#include "sys/clock.h"
#include <stdbool.h>
#include <stdlib.h>

#define WIN 2
#define PLAYING 0
#define WAITING -1
#define LOSE 1

static volatile int frequenceCliniotement = 1000;

static volatile int compteurTick = 0;

static volatile int secondCounter = 0;

static volatile int etatJeu = WAITING;

void tempo_500ms();

void initLedTricolor(){
	RCC.AHB1ENR |= 0x01; //On modifie le GPIOA

	/*
		Initialisation des LED séparément
	*/

	/*//PARTIE ROUGE
	GPIOA.MODER = (GPIOA.MODER & 0xFFFCFFFF) | 0x00010000;
	GPIOA.PUPDR &= 0xFFFCFFFF;

	// PARTIE VERT
	GPIOA.MODER = (GPIOA.MODER & 0xFFF3FFFF) | 0x00040000;
	GPIOA.PUPDR &= 0xFFF3FFFF;

	// PARTIE BLEU
	GPIOA.MODER = (GPIOA.MODER & 0xFFCFFFFF) | 0x00100000;
	GPIOA.PUPDR &= 0xFFCFFFFF;*/
	
	/*
		Initialisation de toutes les LED en même temps
	*/

	// PARTIE ALL
	GPIOA.MODER = (GPIOA.MODER & 0xFFC0FFFF) | 0x00150000;
	GPIOA.PUPDR &= 0xFFC0FFFF;
}

void initBarre4Led(){
	
	RCC.AHB1ENR |= 0x01; //On modifie le GPIOA

	GPIOA.MODER = (GPIOA.MODER & 0xFFFF03CF) | 0x00005500;
	GPIOA.PUPDR &= 0xFFFF03CF;
}

void initLevers(){
	RCC.AHB1ENR |= 0x02; //On modifie le GPIOB

	/*
		Initialisation des leviers séparément
	*/
	/*GPIOB.MODER = (GPIOB.MODER & ~(0x3<<12)) | 0x1<<12; // Init lever 1
	GPIOB.MODER = (GPIOB.MODER & ~(0x3<<10)) | 0x1<<10; // Init lever 2
	GPIOB.MODER = (GPIOB.MODER & ~(0x3<<8)) | 0x1<<8; // Init lever 3
	GPIOB.MODER = (GPIOB.MODER & ~(0x3<<6)) | 0x1<<6; // Init lever 4*/

	/*
		Initialisation de tous les leviers en même temps
	*/
	GPIOB.MODER = (GPIOB.MODER & 0xFFFFC03F) | 0x00001540;
}

int isLever1On(){
	return !((GPIOB.IDR & (0x1<<6)) == 0);
}

int isLever2On(){
	return !((GPIOB.IDR & (0x1<<5)) == 0);
}

int isLever3On(){
	return !((GPIOB.IDR & (0x1<<4)) == 0);
}

int isLever4On(){
	return !((GPIOB.IDR & (0x1<<3)) == 0);
}

void initButton(){
	RCC.AHB1ENR |= 0x02;
	GPIOB.MODER = (GPIOB.MODER & ~(0x3<<16));
}

int isButtonPressed(){
	return (GPIOB.IDR & (0x1<<8)) == 0;

}

void inverserEtatLedRouge () {
	GPIOA.ODR = GPIOA.ODR ^(0x0100);
}

void inverserEtatLedVert () {
	GPIOA.ODR = GPIOA.ODR ^(0x0200);
}

void inverserEtatLedBleu () {
	GPIOA.ODR = GPIOA.ODR ^(0x0400);
}

void allumerLedRouge () {
	GPIOA.ODR = GPIOA.ODR |(0x0100);
}

void allumerLedVert () {
	GPIOA.ODR = GPIOA.ODR |(0x0200);
}

void allumerLedBleu () {
	GPIOA.ODR = GPIOA.ODR |(0x0400);
}

void eteindreLedRouge () {
	GPIOA.ODR = GPIOA.ODR &~(0x0100);
}

void eteindreLedVert () {
	GPIOA.ODR = GPIOA.ODR &~(0x0200);
}

void eteindreLedBleu () {
	GPIOA.ODR = GPIOA.ODR &~(0x0400);
}

void allumerLedHautGauche () {
	GPIOA.ODR = GPIOA.ODR | (0x0080);
}

int ledHautGaucheAllumer () {
	return (GPIOA.ODR & 0x0080) != 0 ? 1 : 0; // LED haut gauche allumer ?
}

void allumerLedHautDroite () {
	GPIOA.ODR = GPIOA.ODR | (0x0040);
}

int ledHautDroiteAllumer () {
	return (GPIOA.ODR & 0x0040) != 0 ? 1 : 0; // LED haut droite allumer ?
}

void allumerLedBasDroite () {
	GPIOA.ODR = GPIOA.ODR | (0x0020);
}

int ledBasDroiteAllumer () {
	return (GPIOA.ODR & 0x0020) != 0 ? 1 : 0; // LED bas droite allumer ?
}

void allumerLedBasGauche () {
	GPIOA.ODR = GPIOA.ODR | (0x0010);
}

int ledBasGaucheAllumer () {
	return (GPIOA.ODR & 0x0010) != 0 ? 1 : 0; // LED bas gauche allumer ?
}

void eteindreLedHautGauche () {
	GPIOA.ODR = GPIOA.ODR &(0xFF7F);
}

void eteindreLedHautDroite () {
	GPIOA.ODR = GPIOA.ODR &(0xFFBF);
}

void eteindreLedBasDroite () {
	GPIOA.ODR = GPIOA.ODR &(0xFFDF);
}

void eteindreLedBasGauche () {
	GPIOA.ODR = GPIOA.ODR &(0xFFEF);
}

void tempo_500ms(){
	volatile uint32_t duree;
	/* estimation, suppose que le compilateur n'optimise pas trop... */
	for (duree = 0; duree < 5600000 ; duree++){
		;
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
	compteurTick++;

	if (etatJeu != WAITING) {
		
		if (compteurTick%1000 == 0){ // Chaque seconde
			secondCounter++;
			if (secondCounter >= 10 && etatJeu == PLAYING) { // Si ca fait plus de 10 secondes le joueur à perdu
				etatJeu = LOSE;
			} else if (secondCounter % 2 == 0) { // Toutes les 2 secondes la vitesse de clignotement est divisé par 2
				frequenceCliniotement = frequenceCliniotement / 2;		
			}
		}

		if (compteurTick % frequenceCliniotement == 0 && etatJeu == PLAYING){
			inverserEtatLedBleu();
		}
	}
}

void initBombe() {
	do {
		int ledOn = rand() % 2;
		if (ledOn) {
			allumerLedBasDroite();
		} else {
			eteindreLedBasDroite();
		}

		ledOn = rand() % 2;

		if (ledOn) {
			allumerLedBasGauche();
		} else {
			eteindreLedBasGauche();
		}

		ledOn = rand() % 2;

		if (ledOn) {
			allumerLedHautDroite();
		} else {
			eteindreLedHautDroite();
		}

		ledOn = rand() % 2;

		if (ledOn) {
			allumerLedHautGauche();
		} else {
			eteindreLedHautGauche();
		}
		
	} while (ledBasDroiteAllumer() && ledBasGaucheAllumer() && ledHautDroiteAllumer() && ledHautGaucheAllumer());
}

/*
Make a sound when the player loose
*/
void loosingSong () {

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
	
	/*
		Initialisation of all component needed
	*/
	systick_init(1000); // Traitant toutes les millisecondes
	initBarre4Led();
	initLedTricolor();
	initLevers();
	initButton();

	int previousStateLever1 = isLever1On();
	int previousStateLever2 = isLever2On();
	int previousStateLever3 = isLever3On();
	int previousStateLever4 = isLever4On();

	while (1){
		if (etatJeu == WAITING) {
			if (isButtonPressed()) { // Quand on lance le jeu, on initialise le générateur aléatoire
				etatJeu = PLAYING;
				srand(compteurTick);
				initBombe();
			}
		} else if (etatJeu == PLAYING) {
			if (ledBasDroiteAllumer() && ledBasGaucheAllumer() && ledHautDroiteAllumer() && ledHautGaucheAllumer()) { //Si toutes les LED sont allumé
				etatJeu = WIN;
			}

			int actualLeverState = isLever1On();
			if (previousStateLever1 != actualLeverState) {
				previousStateLever1 = actualLeverState;
				allumerLedBasDroite();
			}

			actualLeverState = isLever2On();
			if (previousStateLever2 != actualLeverState) {
				previousStateLever2 = actualLeverState;
				allumerLedBasGauche();
				eteindreLedBasDroite();
			}

			actualLeverState = isLever3On();
			if (previousStateLever3 != actualLeverState) {
				previousStateLever3 = actualLeverState;
				allumerLedHautDroite();
				eteindreLedBasGauche();
			}

			actualLeverState = isLever4On();
			if (previousStateLever4 != actualLeverState) {
				previousStateLever4 = actualLeverState;
				allumerLedHautGauche();
				eteindreLedHautDroite();
			}
		} else if (etatJeu == WIN) { // Si on a gagner la LED verte s'allume
			eteindreLedBleu();
			allumerLedVert();
		} else { //Si on a perdu la LED rouge s'allume
			eteindreLedBleu();
			allumerLedRouge();
			loosingSong();
		}

		if (etatJeu == WIN || etatJeu == LOSE) {
			if (isButtonPressed()) {
				frequenceCliniotement = 1000;
				compteurTick = 0;
				secondCounter = 0;
				etatJeu = PLAYING;
				eteindreLedRouge();
				eteindreLedVert();
				initBombe();
			}
		}
	}
	
	return 0;
}

