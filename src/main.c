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

static volatile int frequenceCliniotementLedBleu = 1000; // Représente la vitesse de clignotement de la LED bleu durant le jeu (1 fois par seconde au début)

static volatile int compteurMiliseconde = 0; // Est incrémenté de 1 à chaque milliseconde

static volatile int secondCounter = 0; // Est incrémenté de 1 à chaque seconde

static volatile int freqBuzzer = 2; // Représente le nombre de millisecond entre chaque activation du buzzer 2 par défaut (500Hz)

static volatile int switchBuzzer = 2; // Représente le temps écouler depuis le dernier changement d'état du buzzer

static volatile int etatJeu = WAITING; // Représente l'état du jeu (WIN, LOSE, PLAYING, WAITING)

static volatile int makeSound = 0; //Lorsque cette variable vaut 1, le buzzer emet du son

static volatile int secondStartWinningSong = 0; //La seconde à laquel le joueur à gagner (utile pour lancer le son de victoire)


/*
	Initialise toutes les couleurs de la LED tricolore
*/
void initLedTricolore(){
	RCC.AHB1ENR |= 0x01; //On selectionne le GPIO modifié (le A)

	/*
		Initialisation des LED séparément
	*/

	/*//LED ROUGE
	GPIOA.MODER = (GPIOA.MODER & 0xFFFCFFFF) | 0x00010000;
	GPIOA.PUPDR &= 0xFFFCFFFF;

	// LED VERT
	GPIOA.MODER = (GPIOA.MODER & 0xFFF3FFFF) | 0x00040000;
	GPIOA.PUPDR &= 0xFFF3FFFF;

	// LED BLEU
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
	switchBuzzer--;
	if (switchBuzzer == 0){
		inverserEtatBuzz();
		switchBuzzer = freqBuzzer;
	}
}

void initLevers(){
	RCC.AHB1ENR |= 0x02;  //On selectionne le GPIO modifié (le B)

	/*
		Initialisation des leviers séparément
	*/
	/*
	GPIOB.MODER = (GPIOB.MODER & ~(0x3<<12)) | 0x1<<12; // Init lever 1
	GPIOB.MODER = (GPIOB.MODER & ~(0x3<<10)) | 0x1<<10; // Init lever 2
	GPIOB.MODER = (GPIOB.MODER & ~(0x3<<8)) | 0x1<<8; // Init lever 3
	GPIOB.MODER = (GPIOB.MODER & ~(0x3<<6)) | 0x1<<6; // Init lever 4
	*/

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
void __attribute__((interrupt)) SysTick_Handler() {
	compteurMiliseconde++;
	if (etatJeu != WAITING) {

		if (compteurMiliseconde%1000 == 0) { // Chaque seconde
			secondCounter++;

			if (secondCounter >= 30 && etatJeu == PLAYING) { // Si ca fait plus de 30 secondes le joueur à perdu
				etatJeu = LOSE;
			}
			
			if (secondCounter % 5 == 0) { // Toutes les 5 secondes la vitesse de clignotement est divisé par 2
				frequenceCliniotementLedBleu = frequenceCliniotementLedBleu / 2;
			}
		}
		int freqSoundBeforeBomb = 0;


		if (etatJeu == PLAYING) {

			/*
			Gère le son de défaite
			*/
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
					freqSoundBeforeBomb = 0; //Quand on ne veut pas jouer de sons
					break;
			}
		}

		if (etatJeu == WIN) {
			/*
			Gère le son de victoire
			*/
			if (secondCounter == secondStartWinningSong + 1) {
				if (compteurMiliseconde % 1000 == 0 || compteurMiliseconde % 1000 == 220 || compteurMiliseconde % 1000 == 440 || compteurMiliseconde % 1000 == 690) {
					freqBuzzer = 1;
					makeSound = 1;
				} else if (compteurMiliseconde % 1000 == 200 || compteurMiliseconde % 1000 == 420 || compteurMiliseconde % 1000 == 640 || compteurMiliseconde % 1000 == 890) {
					makeSound = 0;
				} else if (compteurMiliseconde % 1000 == 990) {
					freqBuzzer = 10;
					makeSound = 1;
				}
			} else if (secondCounter == secondStartWinningSong + 2) {
				if (compteurMiliseconde % 1000 == 490) {
					freqBuzzer = 10;
					makeSound = 1;
				} else if (compteurMiliseconde % 1000 == 340 || compteurMiliseconde % 1000 == 890) {
					makeSound = 0;
				}
			} else if (secondCounter == secondStartWinningSong + 3) {
				if (compteurMiliseconde % 1000 == 140) {
					freqBuzzer = 2;
					makeSound = 1;
				} else if (compteurMiliseconde % 1000 == 340 || compteurMiliseconde % 1000 == 490) {
					makeSound = 0;
				} else if (compteurMiliseconde % 1000 == 390) {
					freqBuzzer = 10;
					makeSound = 1;
				} else if ( compteurMiliseconde % 1000 == 540) {
					freqBuzzer = 3;
					makeSound = 1;
				}
			} else if (secondCounter == secondStartWinningSong + 4) {
				if (compteurMiliseconde % 1000 == 240) {
					makeSound = 0;
				}
			}
		}

		if (freqSoundBeforeBomb != 0 && compteurMiliseconde % freqSoundBeforeBomb == 0) { //On fait "clignoter" le son
			makeSound = !makeSound;
		}
	}

	if (compteurMiliseconde % frequenceCliniotementLedBleu == 0 && etatJeu == PLAYING) { //On gère le clignotement de la LED bleu
			inverserEtatLedBleu();
	}

	if (etatJeu == LOSE || makeSound) { // Si on a perdu ou qu'il faut jouer du son on active le son
		buzz();
		freqBuzzer = 2;
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
	do { // Tant que toutes les LED sont allumé on génère une cobinaison aléatoire (pour éviter une victoire instantané)
		
		/*
		Chaque LED a 1 chance sur 2 d'être allumé
		*/
		if (rand() % 2) {
			allumerLedBasDroite();
		} else {
			eteindreLedBasDroite();
		}

		if (rand() % 2) {
			allumerLedBasGauche();
		} else {
			eteindreLedBasGauche();
		}

		if (rand() % 2) {
			allumerLedHautDroite();
		} else {
			eteindreLedHautDroite();
		}

		if (rand() % 2) {
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
	frequenceCliniotementLedBleu = 1000;
	compteurMiliseconde = 0;
	secondCounter = 0;
	etatJeu = PLAYING;
	makeSound = false;
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
	switchBuzzer = freqBuzzer;
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
		switch (etatJeu) {
		case WAITING: // Attente que le joueur lace le jeu
			if (isButtonPressed()) { // Quand on lance le jeu, on initialise le générateur aléatoire
				startGame();
			}
			break;

		case PLAYING: // On actualise les LED en fonction des leviers que le joueur active
			if (bombeOff()) { //Si toutes les LED sont allumé
				etatJeu = WIN;
				secondStartWinningSong = secondCounter;
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
			break;

		case WIN: // Si on a gagner la LED verte s'allume
			eteindreLedBleu();
			allumerLedVert();
			break;
		
		case LOSE: //Si on a perdu la LED rouge s'allume
			eteindreLedBleu();
			allumerLedRouge();
			break;
		
		default:
			break;
		}

		if (etatJeu == WIN || etatJeu == LOSE) { // Quand la partie est finit, pour en relancer une
			if (isButtonPressed()) {
				restart();
			}
		}
	}
	
	return 0;
}

