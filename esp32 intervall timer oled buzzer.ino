// *****************************************************************
// Projekt: Intervalltimer für Fitnesstraining
// created 04.11.2023 by micronano (micronano@gmx-topmail.de)
// 
// Beschreibung: 3 Timer mit je 2 Intervallen einstellbar
//               Timereinstellungen werden intern gespeichert und 
//               gehen somit nicht verloren
// Intervallzeiten bis 245sek getrennt einstellbar 
// nach Ablauf von Intervall1 erfolgt 1 Piep, nach Pausezeit 2 Piepse.
// nach Ablauf von Intervall2 erfolgen 2 Piepser.
// Nach Ablauf des Timers erfolgen 5 Piepser.
// Zweck: zB 1min trainieren, 20s Pause, 1min trainieren, 20s Pause ... 
//        Pausezeiten reduzieren den Trainigstimer nicht
// Taste rot: Timer start/stop und Werte im Einstellungsmodus erhöhen
// Taste blau: Werte im Einstellungsmodus reduzieren
// Taste grün: Timer wählen / länger gedrückt: Timer einstellen
// Einstellung bleiben nach Stromunterbrechungen erhalten
// Stromverbrauch: ca 30mA
// verwendete Pins beim ESP12-F:
// int btn_rt          = 12; 
// int btn_bl          = 13;  
// int btn_set         = 14;  
// int buzz            = 15;
// https://wokwi.com/projects/380378934614223873
// *******************************************************************

#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include<EEPROM.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Declaration for SSD1306 display connected using I2C
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

int btn_rt          = 15; 
int btn_bl          = 2;  
int btn_set         = 4;  
int buzz            = 0;

int akt_timer       = 0;
int hrs             = 0;
int Min             = 0;
int sek             = 0;
int piep1           = 0;
int piep2           = 0;

int piep1cnt = 0;
int piep2cnt = 0;

unsigned int check_val = 50;

int add_chk         = 0;    // EEPROM Speicherstelle
int add_akt_timer   = 1;
int add_hrs         = 2;
int add_min         = 3;
int add_sek         = 4;
int add_piep1       = 5;
int add_piep2       = 6;

bool RUN              = true;

bool set_timer_flag   = true;
bool timer_flag       = true;
bool sek_flag         = true;
bool min_flag         = true;
bool hrs_flag         = true;
bool piep1_flag       = true;
bool piep2_flag       = true;

int val;
int bnt_set_val = 0;


void INIT()
{
  //Serial.println("INIT()");
  akt_timer = EEPROM.read(add_akt_timer);
  hrs = EEPROM.read(akt_timer * 5);
  Min = EEPROM.read(akt_timer * 5 + 1);
  sek = EEPROM.read(akt_timer * 5 + 2);
  piep1 = EEPROM.read(akt_timer * 5 + 3);
  piep2 = EEPROM.read(akt_timer * 5 + 4);

  display.clearDisplay();
  // Display Text
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(23,0); // x, y
  display.print("TIMER:");
  display.setCursor(93,0);
  display.print(akt_timer);

  display.setTextSize(2);
  display.setCursor(15,20);
  if (hrs <= 9)
  {
    display.print('0');
  }
  display.print(hrs);
  display.print(':');
  if (Min <= 9)
  {
    display.print('0');
  }
  display.print(Min);
  display.print(':');
  if (sek <= 9)
  {
    display.print('0');
  }
  display.print(sek);

  display.setTextSize(1);
  display.setCursor(0,40);
  display.print("Piep:");
  display.setTextSize(2);
  display.print(piep1);
  display.print("/");
  display.print(piep2);

  piep1cnt = 0;
  piep2cnt = 0;

  set_timer_flag = false;
  timer_flag = true;
  sek_flag = true;
  min_flag = true;
  hrs_flag = true;
  piep1_flag = true;
  piep2_flag = true;
  //delay(500);
  display.display();
  //delay(2000);
  display.clearDisplay();
}



void setup() {
  Serial.begin(9600);

  EEPROM.begin(100);       //Initialasing EEPROM
  delay(10);

  display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS);
  
  // // Clear the buffer.
  display.clearDisplay();

  pinMode(btn_rt, INPUT_PULLUP);
  pinMode(btn_bl, INPUT_PULLUP);
  pinMode(btn_set, INPUT_PULLUP);
  pinMode(buzz, OUTPUT);
  digitalWrite(buzz, LOW);

  if (EEPROM.read(add_chk) != check_val)
  {
    EEPROM.write(add_chk, check_val);
    EEPROM.write(add_akt_timer, 1);
    EEPROM.write(5, 0); // hrs
    EEPROM.write(6, 3); // Min
    EEPROM.write(7, 30); // sek
    EEPROM.write(8, 60);// piep1 max bis 254 da Integer
    EEPROM.write(9, 20);// piep2 max bis 254 da Integer

    EEPROM.write(10, 1); // hrs
    EEPROM.write(11, 0); // Min
    EEPROM.write(12, 0); // sek
    EEPROM.write(13, 60);// piep1 max bis 254 da Integer
    EEPROM.write(14, 30);// piep2 max bis 254 da Integer

    EEPROM.write(15, 1); // hrs
    EEPROM.write(16, 0); // Min
    EEPROM.write(17, 0); // sek
    EEPROM.write(18, 60);// piep1 max bis 254 da Integer
    EEPROM.write(19, 0);// piep2 max bis 254 da Integer

    EEPROM.commit();
  }


  else
  {
    akt_timer = EEPROM.read   (add_akt_timer);
    hrs       = EEPROM.read   (add_akt_timer * 5);
    Min       = EEPROM.read   (add_akt_timer * 5 + 1);
    sek       = EEPROM.read   (add_akt_timer * 5 + 2);
    piep1     = EEPROM.read   (add_akt_timer * 5 + 3);
    piep2     = EEPROM.read   (add_akt_timer * 5 + 4);
  }
  //delay(1500);
  INIT();

}


void loop() {
  if ( (digitalRead(btn_rt) == LOW) && (digitalRead(btn_bl) == HIGH) )
  {
    Serial.println("btn_rt = start");
    display.clearDisplay();
    delay(250);
    RUN = true;
    while (RUN)
    {
      if ( (digitalRead(btn_rt) == LOW) && (digitalRead(btn_bl) == HIGH) )
      {
        Serial.println("btn_rt = stopp");
        delay(1000);
        if ( (digitalRead(btn_rt) == LOW) && (digitalRead(btn_bl) == HIGH) )
        {
          Serial.println("btn_rt = stopp");
          display.setTextSize(2);
          display.setTextColor(WHITE);
          display.setCursor(4,25);
          display.print("TIMER STOP");
          display.display();
          display.clearDisplay();
          delay(2000);
          RUN = false;
          INIT();
          break;
        }
      }       // wenn nichts gedrückt dann weiter mit herunterzählen


      // COUNTDOWN ------------------------------
      if (piep2cnt == 0) {
        sek = sek - 1; // es sollen nur die trainierten Sekunden herunterzählen
      }
      delay(1000);  // hier ist die Verzögerung
      if (sek == -1)
      {
        sek = 59;
        Min = Min - 1;
      }
      if (Min == -1)
      {
        Min = 59;
        hrs = hrs - 1;
      }
      if (hrs == -1) hrs = 0;
      // ----------------------------- COUNTDOWN


      if (piep1 > 0 && piep1cnt < piep1) 
      { 
        piep1cnt++;
        if (piep1 == piep1cnt) 
        {
          digitalWrite(buzz, HIGH);
          delay(400);
          digitalWrite(buzz, LOW);
          delay(400);
          Serial.println("PIEP1");
        }        
      }

      if (piep2 > 0 && piep1 == piep1cnt) 
      {
        piep2cnt++;
        if (piep2 == piep2cnt) 
        {
          digitalWrite(buzz, HIGH);
          delay(200);
          digitalWrite(buzz, LOW);
          delay(200);
          Serial.println("PIEP2");
          digitalWrite(buzz, HIGH);
          delay(200);
          digitalWrite(buzz, LOW);
          delay(200);
          Serial.println("PIEP2");
          piep1cnt = 0;
          piep2cnt = 0;
        }
      } else {
        if (piep1 == piep1cnt) piep1cnt = 0;
      }

      // Timer am Display ausgeben -----------------------------
      display.setTextSize(2);
      display.setTextColor(WHITE);
      display.setCursor(23,0); // x, y
      display.print("TIMER:");
      display.setCursor(93,0);
      display.print(akt_timer);

      display.setCursor(15,20);
      if (hrs <= 9)
      {
        display.print('0');
      }
      display.print(hrs);
      //display.setTextSize(2);
      display.print(':');
      //display.setTextSize(3);
      if (Min <= 9)
      {
        display.print('0');
      }
      display.print(Min);
      //display.setTextSize(2);
      display.print(':');
      //display.setTextSize(3);
      if (sek <= 9)
      {
        display.print('0');
      }
      display.print(sek);

      display.setTextSize(1);
      display.setCursor(0,40);
      display.print("Piep:");
      display.setTextSize(2);
      if (piep1-piep1cnt <= 9)
      {
        display.print('0');
      }
      display.print(piep1-piep1cnt);
      display.print("/");
      if (piep2-piep2cnt <= 9)
      {
        display.print('0');
      }
      display.print(piep2-piep2cnt);

      // ----------------------------------Timer am Display ausgeben

      
      if (hrs == 0 && Min == 0 && sek == 0) // Wenn alles heruntergezählt wurde, dann piepsen
      {
        display.setCursor(20,42); 
        RUN = false; // While Schleife beenden

        // piepsen
        for (int i = 0; i < 5; i++)
        {
          digitalWrite(buzz, HIGH);
          delay(250);
          digitalWrite(buzz, LOW);
          delay(250);
        }
        INIT(); // auf Startzustand zurückkehren
        break;
      }
      display.display();
      display.clearDisplay();
    } // ENDE While(RUN)
  } // ENDE if ( (digitalRead(btn_rt) == LOW) && (digitalRead(btn_bl) == HIGH) )

// #################################################################################
// #################################################################################
// #################################################################################


  if  (digitalRead(btn_bl) == LOW ) // blauer Knopf - anderen Timer einstellen
  {
    delay(1000);
     if  (digitalRead(btn_bl) == LOW) 
      {
            akt_timer = akt_timer + 1;
            if (akt_timer >=4) akt_timer = 1;
            delay(100);
            EEPROM.write(add_akt_timer, akt_timer);
            EEPROM.commit();
            delay(500);
            INIT();
      }
  }

  if  (digitalRead(btn_set) == LOW) // grünen Knopf gedrückt
  {
      akt_timer = EEPROM.read(add_akt_timer);
      delay(500);

    while (sek_flag)
    {
      // Timer am Display ausgeben -----------------------------
      display.clearDisplay();
      display.setTextSize(2);
      display.setTextColor(WHITE);
      display.setCursor(23,0); // x, y
      display.print("TIMER:");
      display.setCursor(93,0);
      display.print(akt_timer);

      display.setCursor(5,30);
      display.print("Sek: ");
      if (sek <= 9)
      {
        display.print('0');
      }
      display.print(sek);
      
      delay(100);
      if (digitalRead(btn_rt) == LOW)
      {
        sek = sek + 1;
        if (sek >= 60) sek = 0;
        delay(100);
      }
      if (digitalRead(btn_bl) == LOW)
      {
        sek = sek - 1;
        if (sek <= -1) sek = 0;
        delay(100);
      }
      if  (digitalRead(btn_set) == LOW) 
      {
        sek_flag = false;
        delay(250);
      }
      display.display();
      display.clearDisplay();
    }
    while (min_flag)
    {
      // Timer am Display ausgeben -----------------------------
      display.clearDisplay();
      display.setTextSize(2);
      display.setTextColor(WHITE);
      display.setCursor(23,0); // x, y
      display.print("TIMER:");
      display.setCursor(93,0);
      display.print(akt_timer);

      display.setCursor(5,30);
      display.print("Min: ");
      if (Min <= 9)
      {
        display.print('0');
      }
      display.print(Min);
      
      delay(100);
      if (digitalRead(btn_rt) == LOW)
      {
        Min = Min + 1;
        if (Min >= 60) Min = 0;
        delay(100);
      }
      if (digitalRead(btn_bl) == LOW)
      {
        Min = Min - 1;
        if (Min <= -1) Min = 0;
        delay(100);
      }
      if  (digitalRead(btn_set) == LOW) 
      {
        min_flag = false;
        delay(250);
      }
      display.display();
      display.clearDisplay();
    }
    while (hrs_flag)
    {
      // Timer am Display ausgeben -----------------------------
      display.clearDisplay();
      display.setTextSize(2);
      display.setTextColor(WHITE);
      display.setCursor(23,0); // x, y
      display.print("TIMER:");
      display.setCursor(93,0);
      display.print(akt_timer);

      display.setCursor(5,30);
      display.print("Std: ");
      if (hrs <= 9)
      {
        display.print('0');
      }
      display.print(hrs);

      delay(100);
      if (digitalRead(btn_rt) == LOW)
      {
        hrs = hrs + 1;
        if (hrs > 1000) hrs = 0;//edit max jam
        delay(100);
      }
      if (digitalRead(btn_bl) == LOW)
      {
        hrs = hrs - 1;
        if (hrs <= -1) hrs = 0;
        delay(100);
      }
      if  (digitalRead(btn_set) == LOW) 
      {
        hrs_flag = false;
        delay(250);
      }
      display.display();
      display.clearDisplay();
    }
    while (piep1_flag)
    {
      // Timer am Display ausgeben -----------------------------
      display.clearDisplay();
      display.setTextSize(2);
      display.setTextColor(WHITE);
      display.setCursor(23,0); // x, y
      display.print("TIMER:");
      display.setCursor(93,0);
      display.print(akt_timer);

      display.setCursor(5,30);
      display.print("Piep1: ");
      display.print(piep1);
      
      delay(100);
      if (digitalRead(btn_rt) == LOW)
      {
        piep1 = piep1 + 1;
        if (piep1 >= 255) piep1 = 0;
        delay(100);
      }
      if (digitalRead(btn_bl) == LOW)
      {
        piep1 = piep1 - 1;
        if (piep1 <= -1) piep1 = 254;
        delay(100);
      }
      if  (digitalRead(btn_set) == LOW) 
      {
        piep1_flag = false;
        delay(250);
      }
      display.display();
      display.clearDisplay();
    }
    while (piep2_flag)
    {
      // Timer am Display ausgeben -----------------------------
      display.clearDisplay();
      display.setTextSize(2);
      display.setTextColor(WHITE);
      display.setCursor(23,0); // x, y
      display.print("TIMER:");
      display.setCursor(93,0);
      display.print(akt_timer);

      display.setCursor(5,30);
      display.print("Piep2: ");
      display.print(piep2);
      
      delay(100);
      if (digitalRead(btn_rt) == LOW)
      {
        piep2 = piep2 + 1;
        if (piep2 >= 255) piep2 = 0;
        delay(100);
      }
      if (digitalRead(btn_bl) == LOW)
      {
        piep2 = piep2 - 1;
        if (piep2 <= -1) piep2 = 254;
        delay(100);
      }
      if  (digitalRead(btn_set) == LOW) 
      {
        piep2_flag = false;
        delay(250);
      }
      display.display();
      display.clearDisplay();
    }

    if (hrs == 0 && Min == 0 && sek == 0)
    {
      // Timer am Display ausgeben -----------------------------
      display.clearDisplay();
      display.setTextSize(2);
      display.setTextColor(WHITE);
      display.setCursor(23,0); // x, y
      display.print("TIMER:");
      display.setCursor(93,0);
      display.print(akt_timer);

      display.setCursor(15,25);
      display.print("falsche");
      display.setCursor(15,45);
      display.print("Eingabe");
      display.display();
      display.clearDisplay();
      delay(2000);
    }
    else
    {
      //EEPROM.write(add_akt_timer, akt_timer);
      EEPROM.write(akt_timer * 5, hrs); // hrs
      EEPROM.write(akt_timer * 5 + 1, Min); // Min
      EEPROM.write(akt_timer * 5 + 2, sek); // sek
      EEPROM.write(akt_timer * 5 + 3, piep1);// piep1 max bis 254 da Integer
      EEPROM.write(akt_timer * 5 + 4, piep2);// piep2 max bis 254 da Integer
      EEPROM.commit();
      delay(500);
    }
    INIT(); 
  }




}
