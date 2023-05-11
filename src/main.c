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
#define LOSE 1
#define PLAYING 0
#define WAITING -1

static volatile int frequenceCliniotement = 1000;

static volatile int compteurMiliseconde = 0;

static volatile int secondCounter = 0;

static const int freq_buzzer = 2;

static volatile int switch_buzzer = freq_buzzer;

static volatile int etatJeu = WAITING;

static volatile int makeSound = 0;


/*
	Initialise toutes les couleurs de la LED tricolore
*/
void initLedTricolore(){
	RCC.AHB1ENR |= 0x01; //On selectionne le GPIO modifié (le A)

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
	GPIOA.MODER = (GPIOA.MODER & 0xFFC0FFFF) | 0x00150000;
	GPIOA.PUPDR &= 0xFFC0FFFF;
}

/*
	Initialise toutes les LED de la barre de 4 LED
*/
void initBarre4Led(){
	
	RCC.AHB1ENR |= 0x01; //On selectionne le GPIO modifié (le A)

	/*
		Initialisation de toutes les LED en même temps
	*/
	GPIOA.MODER = (GPIOA.MODER & 0xFFFF03CF) | 0x00005500;
	GPIOA.PUPDR &= 0xFFFF03CF;
}

void initBuzzer(){
	RCC.AHB1ENR |= 0x02; //On selectionne le GPIO modifié (le B)

	GPIOB.OTYPER &= ~(0x1<<9); //Output push-pull
	GPIOB.OSPEEDR |= 0x03<<18; //High speed
	GPIOB.MODER = (GPIOB.MODER & 0xFFF3FFFF) | 0x00040000; //Output function
	GPIOB.PUPDR &= 0xFFF3FFFF;// No pull-up, no pull-down
}

/*
	Inverse l'état du Buzzer (allumé => éteint, éteint => allumé)
*/
void inverserEtatBuzz () {
	GPIOB.ODR = GPIOB.ODR ^(0x0200);
}

/*
Make a sound
*/
void buzz() {
	switch_buzzer--;
	if (switch_buzzer == 0){
		inverserEtatBuzz();
		switch_buzzer = freq_buzzer;
	}
}

void initLevers(){
	RCC.AHB1ENR |= 0x02;  //On selectionne le GPIO modifié (le B)

	/*
		Initialisation des leviers séparément
	*///On modifie le GPIOB
	/*GPIOB.MODER = (GPIOB.MODER & ~(0x3<<12)) | 0x1<<12; // Init lever 1
	GPIOB.MODER = (GPIOB.MODER & ~(0x3<<10)) | 0x1<<10; // Init lever 2
	GPIOB.MODER = (GPIOB.MODER & ~(0x3<<8)) | 0x1<<8; // Init lever 3
	GPIOB.MODER = (GPIOB.MODER & ~(0x3<<6)) | 0x1<<6; // Init lever 4*/

	/*
		Initialisation de tous les leviers en même temps
	*/
	GPIOB.MODER = (GPIOB.MODER & 0xFFFFC03F) | 0x00001540;
}

/*
	Renvoie 1 si le levier 1 est sur la position ON et 0 sinon
*/
int isLever1On(){
	return !((GPIOB.IDR & (0x1<<6)) == 0);
}

/*
	Renvoie 1 si le levier 2 est sur la position ON et 0 sinon
*/
int isLever2On(){
	return !((GPIOB.IDR & (0x1<<5)) == 0);
}

/*
	Renvoie 1 si le levier 3 est sur la position ON et 0 sinon
*/
int isLever3On(){
	return !((GPIOB.IDR & (0x1<<4)) == 0);
}

/*
	Renvoie 1 si le levier 4 est sur la position ON et 0 sinon
*/
int isLever4On(){
	return !((GPIOB.IDR & (0x1<<3)) == 0);
}

/*
	Initialise le bouton de la carte fille
*/
void initButton(){
	RCC.AHB1ENR |= 0x02; //On selectionne le GPIO modifié (le B)
	GPIOB.MODER = (GPIOB.MODER & ~(0x3<<16));
}

/*
	Renvoie 1 si le bouton de la carte fille est enfoncé et 0 sinon
*/
int isButtonPressed(){
	return (GPIOB.IDR & (0x1<<8)) == 0;
}

/*
	Inverse l'état de la LED rouge (allumé => éteint, éteint => allumé)
*/
void inverserEtatLedRouge () {
	GPIOA.ODR = GPIOA.ODR ^(0x0100);
}

/*
	Inverse l'état de la LED vert (allumé => éteint, éteint => allumé)
*/
void inverserEtatLedVert () {
	GPIOA.ODR = GPIOA.ODR ^(0x0200);
}

/*
	Inverse l'état de la LED bleu (allumé => éteint, éteint => allumé)
*/
void inverserEtatLedBleu () {
	GPIOA.ODR = GPIOA.ODR ^(0x0400);
}

/*
	Allume la LED rouge
*/
void allumerLedRouge () {
	GPIOA.ODR = GPIOA.ODR |(0x0100);
}

/*
	Allume la LED verte
*/
void allumerLedVert () {
	GPIOA.ODR = GPIOA.ODR |(0x0200);
}

/*
	Allume la LED bleu
*/
void allumerLedBleu () {
	GPIOA.ODR = GPIOA.ODR |(0x0400);
}

/*
	Eteins la LED rouge
*/
void eteindreLedRouge () {
	GPIOA.ODR = GPIOA.ODR &~(0x0100);
}

/*
	Eteins la LED verte
*/
void eteindreLedVert () {
	GPIOA.ODR = GPIOA.ODR &~(0x0200);
}

/*
	Eteins la LED bleu
*/
void eteindreLedBleu () {
	GPIOA.ODR = GPIOA.ODR &~(0x0400);
}

/*
	Allume la LED en haut à gauche sur la barre de 4 LED
*/
void allumerLedHautGauche () {
	GPIOA.ODR = GPIOA.ODR | (0x0080);
}

/*
	Renvoie 1 si la LED en haut à gauche de la barre de 4 LED est allumé et 0 sinon
*/
int isLedHautGaucheOn () {
	return (GPIOA.ODR & 0x0080) != 0 ? 1 : 0;
}

/*
	Allume la LED en haut à droite sur la barre de 4 LED
*/
void allumerLedHautDroite () {
	GPIOA.ODR = GPIOA.ODR | (0x0040);
}

/*
	Renvoie 1 si la LED en haut à droite de la barre de 4 LED est allumé et 0 sinon
*/
int isLedHautDroiteOn () {
	return (GPIOA.ODR & 0x0040) != 0 ? 1 : 0;
}

/*
	Allume la LED en bas à droite sur la barre de 4 LED
*/
void allumerLedBasDroite () {
	GPIOA.ODR = GPIOA.ODR | (0x0020);
}

/*
	Renvoie 1 si la LED en bas à droite de la barre de 4 LED est allumé et 0 sinon
*/
int isLedBasDroiteOn () {
	return (GPIOA.ODR & 0x0020) != 0 ? 1 : 0; // LED bas droite allumer ?
}

/*
	Allume la LED en bas à gauche sur la barre de 4 LED
*/
void allumerLedBasGauche () {
	GPIOA.ODR = GPIOA.ODR | (0x0010);
}

/*
	Renvoie 1 si la LED en bas à gauche de la barre de 4 LED est allumé et 0 sinon
*/
int isLedBasGaucheOn () {
	return (GPIOA.ODR & 0x0010) != 0 ? 1 : 0; // LED bas gauche allumer ?
}

/*
	Eteins la LED en haut à gauche sur la barre de 4 LED
*/
void eteindreLedHautGauche () {
	GPIOA.ODR = GPIOA.ODR &(0xFF7F);
}

/*
	Eteins la LED en haut à droite sur la barre de 4 LED
*/
void eteindreLedHautDroite () {
	GPIOA.ODR = GPIOA.ODR &(0xFFBF);
}

/*
	Eteins la LED en bas à droite sur la barre de 4 LED
*/
void eteindreLedBasDroite () {
	GPIOA.ODR = GPIOA.ODR &(0xFFDF);
}

/*
	Eteins la LED en bas à gauche sur la barre de 4 LED
*/
void eteindreLedBasGauche () {
	GPIOA.ODR = GPIOA.ODR &(0xFFEF);
}

/* Initialisation du timer système (systick) */
void systick_init(uint32_t freq){
	uint32_t p = get_SYSCLK()/freq;
	SysTick.LOAD = (p-1) & 0x00FFFFFF;
	SysTick.VAL = 0;
	SysTick.CTRL |= 7;
}
/*
Traitant d'intéruption du timer (appelé toutes les miliseconde)
*/
void __attribute__((interrupt)) SysTick_Handler(){
	compteurMiliseconde++;
	if (etatJeu != WAITING) {
		
		if (compteurMiliseconde%1000 == 0){ // Chaque seconde
			secondCounter++;

			if (secondCounter >= 30 && etatJeu == PLAYING) { // Si ca fait plus de 30 secondes le joueur à perdu
				etatJeu = LOSE;
			} 
			
			if (secondCounter % 5 == 0) { // Toutes les 5 secondes la vitesse de clignotement est divisé par 2
				frequenceCliniotement = frequenceCliniotement / 2;
			}
		}

		int freqSoundBeforeBomb;

		switch (secondCounter) { // On gère la vitesse du "clignotment" du son (de plus en plus rapide quand on approche des 30 secondes)
		case 25:
		case 26:
			freqSoundBeforeBomb = 200;
			break;
		case 27:
		case 28:
			freqSoundBeforeBomb = 100;
			break;
		case 29:
			freqSoundBeforeBomb = 40;
			break;
		default:
			freqSoundBeforeBomb = 0;
			break;
		}

		if (freqSoundBeforeBomb != 0 && compteurMiliseconde % freqSoundBeforeBomb == 0) { //On fait "clignoter" le son
			makeSound = !makeSound;
		}
	}

	if (compteurMiliseconde % frequenceCliniotement == 0 && etatJeu == PLAYING){ //On gère le clignotement de la LED bleu
			inverserEtatLedBleu();
	}

	if (etatJeu == LOSE || makeSound) { // Si on a perdu ou qu'il faut jouer du son on active le son
		buzz();
	}
}

/*
	Renvoie 1 si toutes les LED sont allumé (la bombe est éteinte) et 0 sinon
*/
int bombeOff () {
	return isLedBasDroiteOn() && isLedBasGaucheOn() && isLedHautDroiteOn() && isLedHautGaucheOn();
}

/*
Initialise les 4 LED de la barre de lED de manière aléatoire avec au minimum 1 LED éteinte
*/
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
	} while (bombeOff());
}

/*
	Remet le jeu à 0 de manière à pouvoir refaire une partit
*/
void restart () {
	frequenceCliniotement = 1000;
	compteurMiliseconde = 0;
	secondCounter = 0;
	etatJeu = PLAYING;
	eteindreLedRouge();
	eteindreLedVert();
	initBombe();
}

/*
	Initialise tous les composant dont nous avons besoin
*/
void initAll () {
	systick_init(1000);	//Traitant du timer toutes les millisecondes
	initBarre4Led();	//Initialisation de la barre de LED
	initLedTricolore();	//Initialisation de la LED tricolore
	initLevers();		//Initialisation des leviers
	initButton();		//Initialisation du boutons de la carte fille
	initBuzzer();		//Initialisation du buzzer
}

/*
	Lance le début du jeu
*/
void startGame() {
	etatJeu = PLAYING;
	srand(compteurMiliseconde);
	compteurMiliseconde = 0;
	initBombe();
}

//Function AF1

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

	initAll(); // Initialisation de tous les composants
	/*
		Variable pour garder l'état précédent des leviers en mémoire
		=> Permet de se servir des leviers comme d'un bouton (à chaque changement => pression d'un bouton)
	*/
	int previousStateLever1 = isLever1On();
	int previousStateLever2 = isLever2On();
	int previousStateLever3 = isLever3On();
	int previousStateLever4 = isLever4On();

	while (1){
		if (etatJeu == WAITING) {
			if (isButtonPressed()) { // Quand on lance le jeu, on initialise le générateur aléatoire
				startGame();
			}
		} else if (etatJeu == PLAYING) {
			if (bombeOff()) { //Si toutes les LED sont allumé
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
		}

		if (etatJeu == WIN || etatJeu == LOSE) {
			if (isButtonPressed()) {
				restart();
			}
		}
	}
	
	return 0;
}

