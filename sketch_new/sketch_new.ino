/* 
przyciski na płytce
+   - buttonSelect
++  - buttonNext, buttonMenu


case0 : ustaw czas(godzine/minute)
case1 : budzik wył./wł.
case2 : ustaw godzine lub minute alarmu
case3 : fotorezystor
*/

#include <LiquidCrystal.h>

// Inicjalizacja wyświetlacza LCD
LiquidCrystal lcd(12, 11, 5, 4, 3, 2);

// Stałe dla przycisków
const int buttonMenu = 9;
const int buttonNext = 10;
const int buttonSelect = 13;

// Stałe dla diod LED
const int ledRed = 6;
const int ledGreen = 7;

// Stałe dla buzzera i fotorezystora
const int buzzer = 8;
const int photoresistor = A0;

// Zmienne globalne
int hour = 0;
int minute = 0;
int second = 0;
int alarmHour = 0;
int alarmMinute = 0;
bool alarmEnabled = false;
bool alarmSounding = false;
bool dawnAlarmEnabled = false; // na światło

int menuState = 0;

unsigned long previousMillis = 0;
const long interval = 1000;

void updateClock() {
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    second++;
    if (second > 59) {
      second = 0;
      minute++;
      if (minute > 59) {
        minute = 0;
        hour++;
        if (hour > 23) {
          hour = 0;
        }
      }
    }
  }
}

void handleMenu() {
  if (digitalRead(buttonMenu) == LOW) {
    menuState++;
    if (menuState > 3) {
      menuState = 0;
    }
    delay(200);
  }

  if (digitalRead(buttonNext) == LOW) {
    switch (menuState) {
        case 0: // Ustaw godzinę
        hour++;
        Serial.println("Zmieniam godzine.");
        if (hour > 23) {
          hour = 0;
        }
        break;
      case 1: // włącz/wyłącz budzik
        alarmEnabled = !alarmEnabled;  
        // reset godziny i minuty alarmu
        alarmHour = 0;
        alarmMinute = 0;
        break;
      case 2: // Ustaw godzinę alarmu
        alarmHour++;
        if (alarmHour > 23) {
          alarmHour = 0;
        }
        break;
      case 3: // włącz/wyłącz "skoro świt"
        dawnAlarmEnabled = !dawnAlarmEnabled;   
        break;          
    }
    delay(200);
  }

  if (digitalRead(buttonSelect) == LOW) {
    switch (menuState) {
      case 0: // Ustaw minutę
        minute++;
        if (minute > 59) {
          minute = 0;
        }
        break;
      case 2: // Ustaw minutę alarmu
        alarmMinute++;
        if (alarmMinute > 59) {
          alarmMinute = 0;
        }
        break;
    }
    delay(200);
  }
}

void checkAlarm() {
  // zielona dioda dla włączonego alarmu
  if(alarmEnabled) {
    digitalWrite(ledGreen, HIGH);        
  } else {
    digitalWrite(ledGreen, LOW);
  }
  // czerwona dioda świeci gdy jest czas alarmu
  if ((alarmEnabled && hour == alarmHour && minute == alarmMinute) || (dawnAlarmEnabled && analogRead(photoresistor) > 512)) {
    alarmSounding = true;
    while (alarmSounding) {
      digitalWrite(ledRed, HIGH);
      digitalWrite(buzzer, HIGH);
      delay(500);
      digitalWrite(ledRed, LOW);
      digitalWrite(buzzer, LOW);
      delay(500);
      if (digitalRead(buttonMenu) == LOW) {
        alarmSounding = false;
        alarmEnabled = false;
        dawnAlarmEnabled = false;
      }
    }
  }
}

void displayTime() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Czas: ");
  if (hour < 10) {
    lcd.print("0");
  }
  lcd.print(hour);
  lcd.print(":");
  if (minute < 10) {
    lcd.print("0");
  }
  lcd.print(minute);
  lcd.print(":");
  if (second < 10) {
    lcd.print("0");
  }
  lcd.print(second);

  switch (menuState) {
    case 0:
      lcd.setCursor(0, 1);
      lcd.print("Ustaw czas");
      break;
    case 1:
      lcd.setCursor(0, 1);
      lcd.print("Budzik ");
      lcd.print(alarmEnabled ? ": WL." : ": WYL.");
      break;
    case 2: // wyświetl gozinę/minutę alarmu
      lcd.setCursor(0, 1);
      lcd.print("Budzik: ");
      if (alarmHour < 10) {
        lcd.print("0");
      }
      lcd.print(alarmHour);
      lcd.print(":");
      if (alarmMinute < 10) {
        lcd.print("0");
      }
      lcd.print(alarmMinute);
      break; 
    case 3:
      lcd.setCursor(0, 1);
      lcd.print("Skoro swit");
      lcd.print(dawnAlarmEnabled ? ": WL." : ": WYL.");
      break;
  }
}

void setup() {
  pinMode(buttonMenu, INPUT_PULLUP);
  pinMode(buttonNext, INPUT_PULLUP);
  pinMode(buttonSelect, INPUT_PULLUP);
  pinMode(ledRed, OUTPUT);
  pinMode(ledGreen, OUTPUT);
  pinMode(buzzer, OUTPUT);
  pinMode(photoresistor, INPUT);

  lcd.begin(16, 2);
  lcd.print("Budzik Arduino");
  delay(2000);
  lcd.clear();
  Serial.begin(9600);
  Serial.println("Działam");
}

void loop() {
  updateClock();
  displayTime();
  handleMenu();
  checkAlarm();
  delay(100);
}


