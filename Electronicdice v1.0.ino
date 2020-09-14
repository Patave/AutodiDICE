
//------------------------------
// Dé électronique sur ATtiny85
// (C) 2020, Patrick Aversenq
//------------------------------
// Version "1.0", september 2020

#include <avr/io.h>
#include <avr/sleep.h>
#include <avr/interrupt.h>

// ----- DECLARATION DES VARIABLES ------



// Ports utilisée pour les groupes de LED:
#define PortLeds PORTB
#define BitUn PB3   // UN  (1 led centrale)
#define BitDeux PB2 // DEUX (2 leds en coin)
#define BitTrois PB0 // TROIS (2 leds en coin opposés)
#define BitSix PB1 // SIX (2 leds milieu)

#define PortButton PINB
#define buttonPin PB4  //Capteur d'inclinaison

// Combinaisons de leds pour obtenir les chiffres:
#define Un (1<<BitUn)
#define Deux (1<<BitDeux)
#define Trois ((1<<BitUn)|(1<<BitDeux))
#define Quatre ((1<<BitDeux)|(1<<BitTrois))
#define Cinq ((1<<BitUn)|(1<<BitDeux)|(1<<BitTrois))
#define Six ((1<<BitDeux)|(1<<BitTrois)|(1<<BitSix))

// autres combinaisons:
#define Zero ~((1<<BitUn)|(1<<BitDeux)|(1<<BitTrois)|(1<<BitSix))
#define Sept ((1<<BitUn)|(1<<BitDeux)|(1<<BitTrois)|(1<<BitSix))

#define Special6 ((1<<BitUn)|(1<<BitSix))
#define Special3 ((1<<BitUn)|(1<<BitTrois))
#define Special2 ((1<<BitUn)|(1<<BitDeux))
#define Special63 ((1<<BitUn)|(1<<BitSix)|(1<<BitTrois))
#define Special62 ((1<<BitUn)|(1<<BitSix)|(1<<BitDeux))

#define Bit6 (1<<BitSix)
#define Bit3 (1<<BitTrois)
#define Bit2 (1<<BitDeux)
#define Bit1 (1<<BitUn)

// Déclarations

int buttonState; //Etat du capteur d'inclinaison
int buttonPreviousState; //Etat capteur inclinaison précédent

// Variable pour le random du résultat final
long ran;

unsigned long previousMillis = 0; //Time for activity

// Nombre de msseconde avant le sleep (Time Before Sleep)
#define Sleepdelay 7000

// Procédure pour afficher la valeur;
void Display (char valeur) {
 PortLeds &= Zero;
 switch (valeur) {
    case 0:
      //Serial.println ("Select 0);
      //PortLeds &= Zero; // °°°/°/°°°
      break;
    case 1:
      //Serial.println ("Select 1);
      PortLeds |= Un; // °°°/./°°°
      break;
    case 2:
      //Serial.println ("Select 2);
      PortLeds |= Deux; // .°°/°/.°°
      break;
    case 3:
      //Serial.println ("Select 3);
      PortLeds |= Trois; // °°././°°.
      break;
    case 4:
      //Serial.println ("Select 4);
      PortLeds |= Quatre; // .°./°/.°.
      break;
    case 5:
      //Serial.println ("Select 5);
      PortLeds |= Cinq; // .°././.°.
      break;
    case 6:
      //Serial.println ("Select 6);
      PortLeds |= Six; // .../°/...
      break;
    case 7:
      //Serial.println ("Select 7);
      PortLeds |= Sept; // ..././...
      break;
    case 8:
      //Serial.println ("Select 8);
      PortLeds |= Special6; // °.°/./°.°
      break;
    case 9:
      //Serial.println ("Select 9);
      PortLeds |= Special3; // °°././°°.
      break;
    case 10:
      //Serial.println ("Select 10);
      PortLeds |= Special2; // .°°/./.°°
      break;
    case 21:
      //Serial.println ("Select 21");
      PortLeds |= Bit1; // °°°/./°°°
      break;
    case 22:
      //Serial.println ("Select 22");
      PortLeds |= Bit2; // .°°/°/.°°
      break;
    case 23:
      //Serial.println ("Select 23");
      PortLeds |= Bit3; // °°./°/°°.
      break;
    case 26:
      //Serial.println ("Select 26");
      PortLeds |= Bit6; // °.°/°/°.°
      break;
    case 62:
      //Serial.println ("Select 62");
      PortLeds |= Special62; // ..°/./..°
      break;
    case 63:
      //Serial.println ("Select 63");
      PortLeds |= Special63; // °.././°..
      break;
    default:
      //Serial.println ("Select 0);
      //PortLeds &= Zero; // °°°/°/°°°
      break;
  }
}

// Procédure de reveil (interruption)
ISR (PCINT0_vect) { // l'interruption termine le mode sleep
 sleep_disable();
 buttonPreviousState = LOW;
}


// Procédure de sommeil
void Sleep() {
 Display(0); //Eteind tout
 
 PCMSK|=(1<<buttonPin); // Pin Change sur PB3
 GIMSK|=(1<<PCIE); // Pin Change Interupt Inable
 sei();
 set_sleep_mode(SLEEP_MODE_PWR_DOWN); // mieux: 3v à 2.5v en 10 sec.
 sleep_enable();
 sleep_cpu(); // on passe en mode sleep.
 // l'interruption le réveillera
}


// ----- DECLARATION ENTREES ET SORTIES ------
void setup ()
{
  Serial.begin(9600);
  Serial.println("Setup ...");
  
  //On indique que les LED sont des sorties  
  DDRB = (1<<BitUn)|(1<<BitDeux)|(1<<BitTrois)|(1<<BitSix); // sorties
  // On indique que le detecteur d'inclinaison est une entrée.
  PORTB=(1<<buttonPin); // entrée, avec Pull-up  pinMode (buttonPin, INPUT_PULLUP);

  randomSeed(analogRead(0)); // on initialise le fait de pouvoir lancer des randoms
  Sleep(); //mode sleep after setup
}

// ----- BOUCLE ------
void loop()
{ 
  unsigned long currentMillis = millis();
  buttonState = digitalRead(buttonPin);

  if ((buttonState!=buttonPreviousState) && (buttonState == HIGH)) { // Si on bouge le dé (detection front)
   //SMALL ANIMATION FOR SUSPENS !!!  
   for (int i=0; i <= 32; i++){ // Animation avec 100 ms entre chaque et acceleration (trait en rotation)
      switch (i%3) {
        case 0: 
          Display(10); //Special 2
          break;
        case 1: 
          Display(8);  //Special 6
          break;
        case 2: 
          Display(9);  //Spécial 3
          break;
      }
     delay(100-2*i); // Pause entre l'affichage de l'animation avec acceleration
    }

    //RESET DISPLAY
    Display(0); //Efface tout
    delay(200);
   
    // RANDOM DU RESULTAT FINAL
    ran = random(1, 7); //Random value between >=1 and <7 (1 to 6)
    Display (ran); //Affiche le tirage
    
    //réinitialise le timer d'activité
    currentMillis = millis();
    previousMillis = currentMillis;
    buttonPreviousState=buttonState;
  }

  //SLEEP MODE DETECTION
  if (currentMillis - previousMillis >= Sleepdelay) { //Time of Activity closed
       // save the last time you measure
      previousMillis = currentMillis;
      Sleep(); // Endormissement
   }

}
