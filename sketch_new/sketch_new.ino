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
bool dawnAlarmEnabled = false;

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
    if (menuState > 4) {
      menuState = 0;
    }
    delay(200);
  }

  if (digitalRead(buttonNext) == LOW) {
    switch (menuState) {
            case 1: // Ustaw godzinę
        hour++;
        if (hour > 23) {
          hour = 0;
        }
        break;
      case 2: // Ustaw minutę
        minute++;
        if (minute > 59) {
          minute = 0;
        }
        break;
      case 3: // Ustaw budzik
        alarmHour++;
        if (alarmHour > 23) {
          alarmHour = 0;
        }
        break;
      case 4: // Włącz budzik "skoro świt"
        dawnAlarmEnabled = !dawnAlarmEnabled;
        break;
    }
    delay(200);
  }

  if (digitalRead(buttonSelect) == LOW) {
    switch (menuState) {
      case 3: // Ustaw budzik
        alarmMinute++;
        if (alarmMinute > 59) {
          alarmMinute = 0;
        }
        break;
      case 4: // Włącz/wyłącz budzik
        alarmEnabled = !alarmEnabled;
        break;
    }
    delay(200);
  }
}

void checkAlarm() {
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
      if (alarmEnabled) {
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
      } else {
        lcd.print("Budzik: wyl.");
      }
      break;
    case 1:
      lcd.setCursor(0, 1);
      lcd.print("Ustaw godzine");
      break;
    case 2:
      lcd.setCursor(0, 1);
      lcd.print("Ustaw minute");
      break;
    case 3:
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
    case 4:
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
}

void loop() {
  updateClock();
  displayTime();
  handleMenu();
  checkAlarm();
  delay(100);
}


