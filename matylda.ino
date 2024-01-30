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
Servo Lram;
Servo Hrud;
Servo Hlava;

int i = 0;                                                     // Integer pro proměný úhel serv
int y = 0;                                                     // Druhý integer pro proměný úhel serv
int pTrig = 40;                                                // Integer trigger pinu pro ultazvukový senzor
int pEcho = 41;                                                // Integer echo pinu pro ultazvukový senzor
int pica = A0;                                                 // Integer pinu infračerveného senzoru mezi nahama
int x = 0;                                                     // Integer určený k omezení opakování loopu bliz()
int vytah = 0;                                                 // Integer určený k omezení opakování loopu idle()
int Rpryc;                                                     // Integer pro uložení hodnoty randomizéru pro zvukovou stopu "Pryč"
int Rbliz;                                                     // Integer pro uložení hodnoty randomizéru pro zvukovou stopu "Blíž"
long odezva, vzdalenost;                                       // Dlouhá proměná pro hodnoty výpočtu vzdálenosti ultrazvukovým senzorem
int last;                                                      // Integer pro uložení hodnoty poslední pehrané skladby

int HlavaP = 90;                                               // Integery pro defaltní polohu serv
int RrukP = 160;
int LrukP = 20;
int RramP = 90;
int RnohP = 100;
int RkycP = 100;
int RkolP = 90;
int LramP = 80;
int HrudP = 90;

int d1 = 0;                                                    // Inteegery pro polohu DIP přepínače d1 - d10
int d2 = 0;
int d3 = 0;
int d4 = 0;
int d5 = 0;
int d6 = 0;
int d7 = 0;
int d8 = 0;
int d9 = 0;
int d10 = 0;

int vol = 15;                                                  // Inteeger pro míru hlasitosti

void setup() {
  //Osmička v piči
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

                                                              // Nastavení pinů DIP přepínače A1 - A10
  pinMode(A1, INPUT_PULLUP);                                  // Změna hlasitosti 15/20
  pinMode(A2, INPUT_PULLUP);
  pinMode(A3, INPUT_PULLUP);
  pinMode(A4, INPUT_PULLUP);
  pinMode(A5, INPUT_PULLUP);
  pinMode(A6, INPUT_PULLUP);
  pinMode(A7, INPUT_PULLUP);
  pinMode(A8, INPUT_PULLUP);                                   // Vypnutí výtahové hudby
  pinMode(A9, INPUT_PULLUP);                                   // Vypnutí pohybu
  pinMode(A10, INPUT_PULLUP);                                  // Mute přepínač 

  Rruk.write(160);                                             // Zamezení škubání pravé ruky při startu
  mySoftwareSerial.begin(9600);                                // Nastavení řenosové rychlosti pro zvukový modul
  Serial.begin(19200);                                         // Nastavení řenosové rychlosti pro PC

  pinMode(pTrig, OUTPUT);                                      // Nastavení pinů pro ultrazvukový senzor
  pinMode(pEcho, INPUT);

  Serial.println(F("Zvukový modul"));                          // Troubleshoot zvukového modulu
  Serial.println(F("Inicializace zvukového modulu ... (Může trvat 3~5 sekund)"));
  if (!myDFPlayer.begin(mySoftwareSerial)) {
    Serial.println(F("Nepodařilo se navázat seriovou komunikaci:"));
    Serial.println(F("1. Zkontroluj připojení!"));
    Serial.println(F("2. Vlož SD kartu!"));
  while (true);
  }
  Serial.println(F("DFPlayer Mini online."));
}

void loop() {

  d1 = digitalRead(A1);  
  d2 = digitalRead(A2);
  d3 = digitalRead(A3);
  d4 = digitalRead(A4);
  d5 = digitalRead(A5);
  d6 = digitalRead(A6);
  d7 = digitalRead(A7);
  d8 = digitalRead(A8);
  d9 = digitalRead(A9);
  d10 = digitalRead(A10);
  
  Serial.print("klávesnice: ");

  Serial.print(d1);
  Serial.print(d2);
  Serial.print(d3);
  Serial.print(d4);
  Serial.print(d5);
  Serial.print(d6);
  Serial.print(d7);
  Serial.print(d8);
  Serial.print(d9);
  Serial.println(d10);

if (d10 == 0) {                                                // Změna hlasitosti zvukového modulu pomocí DIP přepínače
  vol = 0;
} else if (d1 == 0) {
  vol = 15;
} else {
  vol = 25;
}


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
Serial.print("Last = ");
Serial.println(last);



myDFPlayer.volume(vol);                                          // Nastavení hlasitosti zvukového modulu (25)

if (vytah > 100 && vzdalenost > 200 && last != 1 && d8 == 1) { idle(); }  // Spuštění loopu idle()
if (vzdalenost > 91 && vzdalenost < 170 && x < 5) { bliz(); }    // Spuštění loopu bliz()
if (vzdalenost< 90 && vzdalenost > 25 && last /* != 1 */) { ahoj(); }// Spuštění loopu ahoj()
if (vzdalenost < 25) { pryc(); }                                 // Spuštění loopu pryc()
if (kunda == 1) { slimak(); } 	                                 // Spuštění loopu slimak()

}

void idle() {
  Serial.println("idle");

  myDFPlayer.reset();
  Serial.println(F("Resetování zvukového modulu ... (Může trvat 3~5 sekund)"));
  delay(1000);
    if (!myDFPlayer.begin(mySoftwareSerial)) {
    Serial.println(F("Nepodařilo se navázat seriovou komunikaci:"));
    Serial.println(F("1. Zkontroluj připojení!"));
    Serial.println(F("2. Vlož SD kartu!"));
  while (true);
  }
  Serial.println(F("DFPlayer Mini online."));
  delay(500);

  myDFPlayer.volume(vol);
  myDFPlayer.playMp3Folder(2);                                  // Přehrání zvukové stopy MP3/0002.mp3
  vytah = 0;                                                    // Resetování hodnoty pro stupštění loopu idle()
  delay(500);

}

void bliz() {
  int Rbliz = random(3, 6);                                     // Randomizování čtyř možných skladeb
  Serial.println("bliz");
  myDFPlayer.playMp3Folder(Rbliz);                              // Přehrání randomizované skladby ze složky MP3

if (d9 == 1) {                                                  // Moožnost vypnutí pohybu pomocí DIP přepínače
    Lruk.attach(3);                                             // Iniciování serva 3 - Levá ruka
    Rruk.attach(2);                                             // Iniciování serva 2 - Pravá ruka
}

  for (i = LrukP; i < 40; i++) {                                // Výpočet postupného pohybu serva, 20° - 40°
    Lruk.write(i);                     
    delay(25);                                                  // Prodleva mezi jedním ° serva
  }
  delay(500);
  for (i = 40; i > LrukP; i--) {
    Lruk.write(i);
    delay(25);
  }
  delay(500);
  for (i = RrukP; i > 140; i--) {
    Rruk.write(i);
    delay(25);
  }
  delay(25);
  for (i = 140; i < RrukP; i++) {
    Rruk.write(i);                     
    delay(25);                      
  }
  delay(900);

  Lruk.detach();                                               // Odpojení serva 3 - Levá ruka
  Rruk.detach();                                               // Odpojení serva 2 - Pravá ruka

  x = x + 1;                                                   // Navýšení omezující hodnoty pro spuštění loopu bliz()
  vytah = 0;                                                   // Resetování hodnoty pro stupštění loopu idle()
}

void ahoj() {
  Serial.println("ahoj");
  myDFPlayer.playMp3Folder(1);                                 // Přehrání zvukové stopy MP3/0001.mp3
  if (d9 == 1) {
  Hrud.attach(13);
  }
  for (i = HrudP; i < 100; i++) {                              // Výpočet postupného pohybu serva, 20° - 40°
    Hrud.write(i);                     
    delay(35);                                                 // Prodleva mezi jedním ° serva
  }
  
  for (i = 100; i > 80; i--) {
    Hrud.write(i);
    delay(35);
  }

  for (i = 80; i < HrudP; i++) {                                // Výpočet postupného pohybu serva, 20° - 40°
    Hrud.write(i);                     
    delay(35);                                                  // Prodleva mezi jedním ° serva
  }

  Hrud.detach(); 
  delay(3000);
  x = 0;
  vytah = 0;
}

void pryc() {
  int Rpryc = random(7, 9);                                     // Randomizování tří možných skladeb
  Serial.println("pryc");
  myDFPlayer.playMp3Folder(Rpryc);                              // Přehrání randomizované skladby ze složky MP3
  Serial.println(Rpryc);
  delay(1250);
  if (d9 == 1) {
  Rkol.attach(7);
  Rnoh.attach(5);
  }
  for (i = RkolP; i > 40; i--) {
    Rkol.write(i);
    delay(15);
  }

    for (i = RnohP; i > 80; i--) {
    Rnoh.write(i);
    delay(15);
  }

  for (i = 40; i < RkolP; i++) {
    Rkol.write(i);                     
    delay(15);                      
  }

    for (i = 80; i < RnohP; i++) {
    Rnoh.write(i);
    delay(15);
  }

  Rkol.detach();
  Rnoh.detach();

  vytah = 0;
}

void slimak() {
  Serial.println("slimak");
  myDFPlayer.playMp3Folder(11);

  if (d9 == 1) {
  Lruk.attach(3);
  Rruk.attach(2);
  }
  for (i = LrukP; i < 100; i++) {
    Lruk.write(i);                    
    delay(15);
  }
  
  for (i = 100; i > LrukP; i--) {
    Lruk.write(i);
    delay(15);
  }
  
  for (i = RrukP; i > 70; i--) {
    Rruk.write(i);
    delay(15);
  }
  
  for (i = 70; i < RrukP; i++) {
    Rruk.write(i);                     
    delay(15);                      
  }
  delay(20); 

  Lruk.detach();
  Rruk.detach();        

  vytah = 0;
}
