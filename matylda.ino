#include "Arduino.h"
#include "SoftwareSerial.h"
#include "DFRobotDFPlayerMini.h"
#include <Servo.h>

SoftwareSerial mySoftwareSerial(11, 10);                       // Vytvoření sériové komunikace se zvukovým modulem (RX, TX)
DFRobotDFPlayerMini myDFPlayer;                                // Vytvoření objektu zvukového modulu

Servo Rruk;                                                    // Vytvoření objektů serv
Servo Lruk;
Servo Rram;
Servo Rnoh;
Servo Rkyc;
Servo Rkol;
Servo g;
Servo Lram;
Servo Hrud;
Servo Hlava;
Servo k;

int i = 0;                                                     // Integer pro proměný úhel serv
int pTrig = 40;                                                // Integer trigger pinu pro ultazvukový senzor
int pEcho = 41;                                                // Integer echo pinu pro ultazvukový senzor
int pica = A0;                                                 // Integer pinu infračerveného senzoru mezi nahama
int x = 0;                                                     // Integer určený k omezení opakování loopu bliz()
int vytah = 0;                                                 // Integer určený k omezení opakování loopu idle()
int Rpryc;                                                     // Integer pro uložení hodnoty randomizéru pro zvukovou stopu "Pryč"
int Rbliz;                                                     // Integer pro uložení hodnoty randomizéru pro zvukovou stopu "Blíž"
long odezva, vzdalenost;                                       // Dlouhá proměná pro hodnoty výpočtu vzdálenosti ultrazvukovým senzorem
int last;                                                      // Integer pro uložení hodnoty poslední pehrané skladby


void setup() { 

  // Osmička v piči
  //Rruk.attach(2);
  //Lruk.attach(3);
  //Rram.attach(4);
  //Rnoh.attach(5);
  //Rkyc.attach(6);
  //Rkol.attach(7);
  //močka g.attach(8);
  //Lram.attach(9);
  //Hrud.attach(13);
  //Hlava.attach(12);
  
   

  mySoftwareSerial.begin(9600);                                // Nastavení řenosové rychlosti pro zvukový modul
  Serial.begin(19200);                                         // Nastavení řenosové rychlosti pro PC

  pinMode(pTrig, OUTPUT);                                      // Nastavení pinů pro ultrazvukový senzor
  pinMode(pEcho, INPUT);

  Serial.println(F("DFRobot DFPlayer Mini Demo"));             // Troubleshoot zvukového modulu
  Serial.println(F("Initializing DFPlayer ... (May take 3~5 seconds)"));
  if (!myDFPlayer.begin(mySoftwareSerial)) {
    Serial.println(F("Unable to begin:"));
    Serial.println(F("1.Please recheck the connection!"));
    Serial.println(F("2.Please insert the SD card!"));
    while (true);
  }
 Serial.println(F("DFPlayer Mini online."));
  }


void loop() {

digitalWrite(pTrig, LOW);                                      // Proces měření vzdálenosti ultrazvukovým senzorem
delayMicroseconds(10);
digitalWrite(pTrig, HIGH);
delayMicroseconds(10);
digitalWrite(pTrig, LOW);

odezva = pulseIn(pEcho, HIGH);                                 // Výpočet vzdálenosti ultrazvukového senzoru
vzdalenost = odezva / 58.31;

int kunda = digitalRead(pica);                                 // Měření vzdálenosti infračerveným senzorem mezi nohama
last = myDFPlayer.readCurrentFileNumber();                     // Uložení hodnoty poslední přehrané skladby
vytah = vytah + 1;                                             // Zvětšování omezující hodnoty pr spuštění loopu idle()

Serial.print("Vzdalenost je ");                                // Zobrazení hodnoty naměřené ultrazvukovým senzorem
Serial.print(vzdalenost);
Serial.println(" cm.");
myDFPlayer.volume(25);                                          // Nastavení hlasitosti zvukového modulu



if (vytah > 25 && vzdalenost > 200 && last != 2) { idle(); }   // Spuštění loopu idle()
if (vzdalenost > 91 && vzdalenost < 200 && x < 5) { bliz(); }  // Spuštění loopu bliz()
if (vzdalenost< 90 && vzdalenost > 25 && last != 1) { ahoj(); }// Spuštění loopu ahoj()
if (vzdalenost < 25) { pryc(); }                               // Spuštění loopu pryc()
if (kunda == 1) { slimak(); } 	                               // Spuštění loopu slimak()
}

void idle() {
  Serial.println("idle");
  myDFPlayer.playMp3Folder(2);                                 // Přehrání zvukové stopy MP3/0002.mp3
  vytah = 0;                                                   // Resetování hodnoty pro stupštění loopu idle()
  delay(500);

}

void bliz() {
  int Rbliz = random(3, 6);                                    // Randomizování čtyř možných skladeb
  Serial.println("bliz");
  myDFPlayer.playMp3Folder(Rbliz);                             // Přehrání randomizované skladby ze složky MP3

  Lruk.attach(3);                                              // Iniciování serva 3 - Levá ruka
  Rruk.attach(2);                                              // Iniciování serva 2 - Pravá ruka
  for (i = 20; i < 40; i++) {                                  // Výpočet postupného pohybu serva, 20° - 40°
    Lruk.write(i);                     
    delay(25);                                                 // Prodleva mezi jedním ° serva
  }
  delay(500);
  for (i = 40; i > 20; i--) {
    Lruk.write(i);
    delay(25);
  }
  delay(500);
  for (i = 160; i > 140; i--) {
    Rruk.write(i);
    delay(25);
  }
  delay(25);
  for (i = 140; i < 160; i++) {
    Rruk.write(i);                     
    delay(25);                      
  }
  delay(2250);
  vytah = 0;                                                   // Resetování hodnoty pro stupštění loopu idle()
  Lruk.detach();                                               // Odpojení serva 3 - Levá ruka
  Rruk.detach();                                               // Odpojení serva 2 - Pravá ruka
  delay(100);

  x = x + 1;                                                   // Navýšení omezující hodnoty pro spuštění loopu bliz()

}

void ahoj() {
  Serial.println("ahoj");
  myDFPlayer.playMp3Folder(1);                                // Přehrání zvukové stopy MP3/0001.mp3
/*
  Rruk.attach(3);
  Lruk.attach(2);
  Hrud.attach(13);

  for (pos = 20; pos <= 90; pos += 1) { // goes from 0 degrees to 180 degrees
    Lruk.write(pos);              // tell servo to go to position in variable 'pos'
    delay(25);                       // waits 15 ms for the servo to reach the position
  }
  for (pos = 20; pos <= 90; pos += 1) { // goes from 0 degrees to 180 degrees
    Rruk.write(pos);              // tell servo to go to position in variable 'pos'
    delay(25);                       // waits 15 ms for the servo to reach the position
  }

  for (pos = 90; pos <= 100; pos += 1) { // goes from 0 degrees to 180 degrees
    Hrud.write(pos);              // tell servo to go to position in variable 'pos'
    delay(25);                       // waits 15 ms for the servo to reach the position
  }
  Serial.println("kokot");
  for (pos = 100; pos >= 80; pos -= 1) { // goes from 180 degrees to 0 degrees
    Hrud.write(pos);              // tell servo to go to position in variable 'pos'
    delay(25);                       // waits 15 ms for the servo to reach the position
  }
  for (pos = 80; pos <= 100; pos += 1) { // goes from 0 degrees to 180 degrees
    Hrud.write(pos);              // tell servo to go to position in variable 'pos'
    delay(25);                       // waits 15 ms for the servo to reach the position
  }
  for (pos = 100; pos >= 90; pos -= 1) { // goes from 180 degrees to 0 degrees
    Hrud.write(pos);              // tell servo to go to position in variable 'pos'
    delay(25);    
  }
  for (pos = 90; pos >= 160; pos -= 1) { // goes from 180 degrees to 0 degrees
    Lruk.write(pos);              // tell servo to go to position in variable 'pos'
    delay(25);    
  }
  for (pos = 90; pos >= 20; pos -= 1) { // goes from 180 degrees to 0 degrees
    Rruk.write(pos);              // tell servo to go to position in variable 'pos'
    delay(25);    
  }

  Rruk.detach();
  Lruk.detach();
  Hrud.detach();
*/
  delay(3250);
  x = 0;
  vytah = 0;
}

void pryc() {
  int Rpryc = random(7, 9);                                   // Randomizování tří možných skladeb
  Serial.println("pryc");
  myDFPlayer.playMp3Folder(Rpryc);                            // Přehrání randomizované skladby ze složky MP3
  Serial.println(Rpryc);
  delay(1250);
  
  vytah = 0;
}

void slimak() {
  Serial.println("slimak");
  myDFPlayer.playMp3Folder(10);                              // Přehrání  skladby MP3/0010.mp3
  delay(3250);
  
  vytah = 0;
}














// void frnda() {
//   Rruk.attach(2);
//   Rram.attach(4);
//   Rram.write(180);
// }
// 
// 
// void asdad() {
//   // hruď
//   Hrud.attach(10);
//   for (i = 80; i < 100; i++) {
//     Hrud.write(i);                     
//     delay(30);                      
//   }
//   for (i = 100; i > 80; i--) {
//     Hrud.write(i);
//     delay(30);
//   }
// }


/*
void loop() {
  Lram.write(80);
  Rram.write(95);
val = digitalRead(7);
if (val == HIGH) {
    Serial.println("cs");
    for (i = 20; i < 40; i++) { // L ruka
      Lruk.write(i);                     
      delay(25);                      
    }
    for (i = 40; i > 20; i--) { // L ruka
      Lruk.write(i);
      delay(25);
    }
    for (i = 160; i > 140; i--) { // R ruka
      Rruk.write(i);
      delay(25);
    }
    for (i = 140; i < 160; i++) { // R ruka
      Rruk.write(i);                     
      delay(25);                      
    }
}
*/







// pohyby

/*
  for (i = 20; i < 40; i++) { // L ruka
      Lruk.write(i);                     
      delay(25);                      
  }
  delay(100);

  for (i = 20; i < 40; i++) { // L rameno
      Lram.write(i);                     
      delay(25);                      
  }
  delay(100);
  for (i = 170; i > 150; i--) { // R ruka
    Rruk.write(i);
    delay(25);
  }
  for (i = 150; i > 130; i--) { // R rameno
    Rram.write(i);
    delay(25);
  }
  delay(100);
  //------------------------------------------ jeden pohyb
  //------------------------------------------ druhej pohyb
  for (i = 40; i > 20; i--) { // L ruka
    Lruk.write(i);
    delay(25);
  }
  delay(100);
  for (i = 40; i > 20; i--) { // L rameno
      Lram.write(i);                     
      delay(25);                      
  }
  delay(100);
  for (i = 150; i < 170; i++) { // R ruka
      Rruk.write(i);                     
      delay(25);                      
  }
  delay(100);
  for (i = 130; i < 150; i++) { // R rameno
    Rram.write(i);
    delay(25);
  }
  delay(100);
  */



