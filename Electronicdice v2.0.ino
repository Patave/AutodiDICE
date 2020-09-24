
//------------------------------
// Dé électronique sur ATtiny13
// (C) 2020, Patrick Aversenq
//------------------------------
// Version "2.0", 24 september 2020

#include <avr/io.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>
#include <util/delay.h>

// ----- DECLARATION DES VARIABLES ------

// Ports utilisée pour les groupes de LED:
//#define PortLeds PORTB
//#define BitUn PB3   // UN  (1 led centrale)
//#define BitDeux PB2 // DEUX (2 leds en coin)
//#define BitTrois PB0 // TROIS (2 leds en coin opposés)
//#define BitSix PB1 // SIX (2 leds milieu)

//#define PortButton PINB
//#define buttonPin PB4  //Capteur d'inclinaison


// Déclarations

int buttonState; //Etat du capteur d'inclinaison
int buttonPreviousState; //Etat capteur inclinaison précédent

unsigned long previousMillis = 0; //Time for activity

// Nombre de msseconde avant le sleep (Time Before Sleep)
#define Sleepdelay 7000


// Procédure pour afficher la valeur;
void Display (volatile byte *port, long valeur) {
   //Reset Port
   *port &= ~(_BV(PB0)|_BV(PB1)|_BV(PB2)|_BV(PB3));
   
   //Bit 1 (if value not even)
   if (valeur%2 != 0) *port |= _BV(PB3);
   
   //Bit 2 (if value > 1)
   if (valeur > 1) *port |= _BV(PB2);
  
   //Bit 3 (if value > 3)
   if (valeur > 3) *port |= _BV(PB0);
   
   //Bit 6 (if value > 5)
   if (valeur > 5) *port |= _BV(PB1);
   
   //short delay
   _delay_ms(200);
}


// Procédure de reveil (interruption)
ISR (PCINT0_vect) { // l'interruption termine le mode sleep
 sleep_disable();
 buttonPreviousState = LOW;
}


// Procédure de sommeil
void Sleep() {
  Display(&PORTB,0); //Eteind tout
  PCMSK|= _BV(PB4); // Pin Change sur PB3
  GIMSK|= _BV(PCIE); // Pin Change Interupt Inable

  set_sleep_mode(SLEEP_MODE_PWR_DOWN); // mieux: 3v à 2.5v en 10 sec.
  sleep_enable();
  sei(); // Lance l'interruption
  sleep_cpu(); // on passe en mode sleep.  
   // l'interruption le réveillera
}


// ----- DECLARATION ENTREES ET SORTIES ------
void setup ()
{
  //On indique que les LED sont des sorties  
  DDRB = (_BV(PB0)|_BV(PB1)|_BV(PB2)|_BV(PB3)); // sorties
  // On indique que le detecteur d'inclinaison est une entrée.
  PORTB= _BV(PB4); // entrée, avec Pull-up  pinMode (buttonPin, INPUT_PULLUP);
  //mode sleep after setup
  Sleep(); 
}

// ----- BOUCLE ------
void loop()
{ 
  unsigned long currentMillis = millis();
  // Variable pour le random du résultat final
  long ran;

  buttonState = digitalRead(PB4);

  if ((buttonState!=buttonPreviousState) && (buttonState == HIGH)) { // Si on bouge le dé (detection front)
   //SMALL ANIMATION FOR SUSPENS !!!  
    for (int i = 6; i >= 0 ; i--) {
      Display(&PORTB,i);
    }
    _delay_ms(300);
   
    // RANDOM DU RESULTAT FINAL
    ran = random(1, 7); //Random value between >=1 and <7 (1 to 6)
    Display (&PORTB, ran); //Affiche le tirage
    
    
    //réinitialise le timer d'activité; 
    currentMillis = millis();
    previousMillis = currentMillis;
    buttonPreviousState = buttonState;
  }

  //SLEEP MODE DETECTION
  if (currentMillis - previousMillis >= Sleepdelay) { //Time of Activity closed
       // save the last time you measure
      previousMillis = currentMillis;
      Sleep(); // Endormissement
   }

}
