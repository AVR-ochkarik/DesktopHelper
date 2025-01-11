// Подключение библиотек
#include <GyverMAX7219.h>
#include <MQ135.h>
#include <ClosedCube_HDC1080.h>
#include <Wire.h>
#include <I2C_RTC.h>

// Обозначения пинов 
#define BUZZ 12
#define BUP 25
#define BDWN 14
#define BLFT 26
#define BRGHT 27
#define BOK 33
#define CS 5
#define CO2 32

// Объявление объектов библиотек
ClosedCube_HDC1080 temp;
MQ135 co2(CO2);
static DS1307 RTC;
MAX7219 < 4, 2, 5 > mtrx;

// Собранные данные
int hum[30] = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1};
int tem[30] = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1};
int co[30] = {-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1};

// Переменные
int mode = 0;
int arg = 0;
int cursor = 0;
int settingsOpened = 0;
int isTime = 1;
int last = 0;
int lastsec = 0;
int setCursor = 0;
int tresh = 0;

void setup() {

  // Инициализация объектов библиотек
  RTC.begin();
  temp.begin(0x40);
  mtrx.begin();
  mtrx.setBright(2);
  mtrx.setRotation(1);

  // Объявление пинов
  pinMode(BUP, INPUT_PULLUP);
  pinMode(BDWN, INPUT_PULLUP);
  pinMode(BLFT, INPUT_PULLUP);
  pinMode(BRGHT, INPUT_PULLUP);
  pinMode(BOK, INPUT_PULLUP);

  Serial.begin(921600); // Включение порта 
  last = RTC.getMinutes();
  hum[29] = temp.readHumidity();
  tem[29] = temp.readTemperature();
  co[29] = co2.getPPM();
}
// Иконки
void graphIcon(){
  mtrx.line(0, 15, 5, 5);
  mtrx.line(5, 5, 9, 9);
  mtrx.line(9, 9, 14, 1);
  mtrx.line(12, 0, 15, 0);
  mtrx.line(15, 0, 15, 3);
  mtrx.update();
}
void eyeIcon(){
  mtrx.line(0, 7, 7, 4);
  mtrx.line(8, 4, 15, 7);
  mtrx.line(0, 8, 7, 11);
  mtrx.line(8, 11, 15, 8);
  mtrx.roundRect(6, 6, 9, 9);
  mtrx.update();
}
void settingsIcon(){
  mtrx.line(0, 14, 12, 2);
  mtrx.line(1, 15, 13, 3);
  mtrx.dot(0, 15);
  mtrx.line(2, 13, 12, 3);
  mtrx.dot(13, 0);
  mtrx.dot(15, 2);
  mtrx.dot(12, 0);
  mtrx.dot(12, 1);
  mtrx.dot(15, 3);
  mtrx.dot(14, 3);
  mtrx.dot(14, 4);
  mtrx.dot(13, 4);
  mtrx.dot(11, 1);
  mtrx.dot(11, 2);
  mtrx.update();
}
void co2Icon(){
  mtrx.line(18, 4, 18, 7);
  mtrx.line(19, 3, 20, 3);
  mtrx.line(19, 8, 20, 8);
  mtrx.line(22, 4, 22, 7);
  mtrx.line(23, 3, 24, 3);
  mtrx.line(25, 4, 25, 7);
  mtrx.line(23, 8, 24, 8);
  mtrx.line(26, 9, 27, 8);
  mtrx.line(28, 8, 29, 9);
  mtrx.line(29, 10, 26, 13);
  mtrx.line(26, 13, 29, 13);
  mtrx.update();
}
void temIcon(){
  mtrx.line(24, 5, 27, 5);
  mtrx.line(23, 6, 23, 11);
  mtrx.line(24, 12, 27, 12);
  mtrx.dot(20, 4);
  mtrx.dot(21, 3);
  mtrx.dot(22, 4);
  mtrx.dot(21, 5);
  mtrx.update();
}
void humIcon(){
  mtrx.line(20, 11, 27, 4);
  mtrx.line(20, 4, 20, 7);
  mtrx.line(22, 4, 22, 7);
  mtrx.dot(21, 4);
  mtrx.dot(21, 7);
  mtrx.line(25, 8, 25, 11);
  mtrx.line(27, 8, 27, 11);
  mtrx.dot(26, 8);
  mtrx.dot(26, 11);
  mtrx.update();
}
void threshIcon(){
  mtrx.line(17, 12, 30, 12);
  mtrx.line(17, 12, 23, 1);
  mtrx.line(30, 12, 24, 1);
  mtrx.rect(23, 5, 24, 8);
  mtrx.line(23, 10, 24, 10);
  mtrx.update();
}
void timeIcon(){
  mtrx.line(19, 5, 22, 2);
  mtrx.line(22, 2, 26, 2);
  mtrx.line(26, 2, 29, 5);
  mtrx.line(29, 5, 29, 9);
  mtrx.line(29, 9, 26, 12);
  mtrx.line(26, 12, 22, 12);
  mtrx.line(22, 12, 19, 9);
  mtrx.line(19, 9, 19, 5);
  mtrx.line(24, 7, 24, 4);
  mtrx.line(24, 7, 25, 7);
  mtrx.update();
}
void dateIcon(){
  mtrx.line(18, 3, 29, 3);
  mtrx.line(29, 3, 29, 13);
  mtrx.line(29, 13, 18, 13);
  mtrx.line(18, 13, 18, 3);
  mtrx.line(18, 5, 29, 5);
  mtrx.line(20, 2, 20, 5);
  mtrx.line(27, 2, 27, 5);
  mtrx.rect(20, 7, 21, 8);
  mtrx.rect(20, 10, 21, 11);
  mtrx.rect(23, 7, 24, 8);
  mtrx.rect(23, 10, 24, 11);
  mtrx.rect(26, 7, 27, 8);
  mtrx.rect(26, 10, 27, 11);
  mtrx.update();
}
// Главные функции
void drawTime(){
  mtrx.clear();
  mtrx.setCursor(2, 4);
  if (RTC.getHours() < 10){
    mtrx.print(0);
  } 
  mtrx.print(RTC.getHours());
  if ( (millis() - millis() % 1000) % 2000 == 0){
  mtrx.print(":");} else {mtrx.print(" ");}
  if (RTC.getMinutes() < 10){
    mtrx.print(0);
  } 
  mtrx.print(RTC.getMinutes());
  mtrx.println();
  mtrx.println();
  mtrx.update();
  delay(100);
}
void drawGraph(int type){
  switch(type){
    case 0:
      for (int i = 0; i < 30; i ++){
        if (hum[i] == -1){continue;}else{
          if (hum[i] < 20){mtrx.dot((i+1), 15);}
          if (hum[i] >= 20 and hum[i] < 30){mtrx.dot((i+1), 14);}
          if (hum[i] >= 25 and hum[i] < 35){mtrx.dot((i+1), 13);}
          if (hum[i] >= 30 and hum[i] < 40){mtrx.dot((i+1), 12);}
          if (hum[i] >= 35 and hum[i] < 45){mtrx.dot((i+1), 11);}
          if (hum[i] >= 40 and hum[i] < 50){mtrx.dot((i+1), 10);}
          if (hum[i] >= 45 and hum[i] < 55){mtrx.dot((i+1), 9);}
          if (hum[i] >= 55 and hum[i] < 60){mtrx.dot((i+1), 8);}
          if (hum[i] >= 60 and hum[i] < 65){mtrx.dot((i+1), 7);}
          if (hum[i] >= 65 and hum[i] < 70){mtrx.dot((i+1), 6);}
          if (hum[i] >= 70 and hum[i] < 75){mtrx.dot((i+1), 5);}
          if (hum[i] >= 75 and hum[i] < 80){mtrx.dot((i+1), 4);}
          if (hum[i] >= 80 and hum[i] < 85){mtrx.dot((i+1), 3);}
          if (hum[i] >= 85 and hum[i] < 90){mtrx.dot((i+1), 2);}
          if (hum[i] >= 90){mtrx.dot((i+1), 1);}
        }
      }
      break;
    case 1:
      for (int i = 0; i < 30; i ++){
          if (tem[i] == -1){continue;}else{
          if (tem[i] <= 15){mtrx.dot((i+1), 14);}
          if (tem[i] == 16){mtrx.dot((i+1), 14);}
          if (tem[i] == 17){mtrx.dot((i+1), 13);}
          if (tem[i] == 18){mtrx.dot((i+1), 12);}
          if (tem[i] == 19){mtrx.dot((i+1), 11);}
          if (tem[i] == 20){mtrx.dot((i+1), 10);}
          if (tem[i] == 21){mtrx.dot((i+1), 9);}
          if (tem[i] == 22){mtrx.dot((i+1), 8);}
          if (tem[i] == 23){mtrx.dot((i+1), 7);}
          if (tem[i] == 24){mtrx.dot((i+1), 6);}
          if (tem[i] == 25){mtrx.dot((i+1), 5);}
          if (tem[i] == 26){mtrx.dot((i+1), 4);}
          if (tem[i] == 27){mtrx.dot((i+1), 3);}
          if (tem[i] == 28){mtrx.dot((i+1), 2);}
          if (tem[i] == 29){mtrx.dot((i+1), 1);}
          if (tem[i] >= 30){mtrx.dot((i+1), 0);}
        }
      }
      break;
    case 2:
      for (int i = 0; i < 30; i ++){
          if (co[i] == -1){continue;}else{
          if (co[i] < 350){mtrx.dot((i+1), 15);}
          if (co[i] >= 350 and co[i] < 400){mtrx.dot((i+1), 14);}
          if (co[i] >= 400 and co[i] < 450){mtrx.dot((i+1), 13);}
          if (co[i] >= 450 and co[i] < 500){mtrx.dot((i+1), 12);}
          if (co[i] >= 500 and co[i] < 550){mtrx.dot((i+1), 11);}
          if (co[i] >= 550 and co[i] < 600){mtrx.dot((i+1), 10);}
          if (co[i] >= 600 and co[i] < 650){mtrx.dot((i+1), 9);}
          if (co[i] >= 650 and co[i] < 700){mtrx.dot((i+1), 8);}
          if (co[i] >= 700 and co[i] < 750){mtrx.dot((i+1), 7);}
          if (co[i] >= 750 and co[i] < 800){mtrx.dot((i+1), 6);}
          if (co[i] >= 800 and co[i] < 850){mtrx.dot((i+1), 5);}
          if (co[i] >= 850 and co[i] < 900){mtrx.dot((i+1), 4);}
          if (co[i] >= 900 and co[i] < 950){mtrx.dot((i+1), 3);}
          if (co[i] >= 950 and co[i] < 1000){mtrx.dot((i+1), 2);}
          if (co[i] >= 1000 and co[i] < 1050){mtrx.dot((i+1), 1);}
          if (co[i] >= 1050){mtrx.dot((i+1), 0);}
        }
      }
      break;
  }
}

void loop() {
  if (tresh != 0){
    while (co2.getPPM() >= tresh){
      tone(BUZZ, 300, 100);
      delay(100);
      noTone(BUZZ);
      delay(100);
    }
  } 
  // Сбор данных
  if (RTC.getMinutes() - last == 1 and RTC.getSeconds() == lastsec){
    for (int i = 0; i < 29; i ++) {
      hum[i] = hum[i + 1];
      tem[i] = tem[i + 1];
      co[i] = co[i + 1];
    }
    hum[29] = temp.readHumidity();
    tem[29] = temp.readTemperature();
    co[29] = co2.getPPM();
    last = RTC.getMinutes();
    lastsec = RTC.getSeconds();
    delay(1500);
    
  }
  // Обработка кнопок
  if (!digitalRead(BUP)){
    if (settingsOpened == 1){
      if (cursor == 0){
        mode += 1;
        mode = mode % 3;
      } else {
        arg += 1;
        arg = arg % 3;
      }
      delay(300);
    }
  }
  if (!digitalRead(BDWN)){
    if (settingsOpened == 1){
        if (cursor == 0){switch(mode){case 0:mode=2;break;case 1:mode=0;break;case 2:mode=1;break;}}else{switch(arg){case 0:arg=2;break;case 1:arg=0;break;case 2:arg=1;break;}}
      delay(200);
    }
  }
  if (!digitalRead(BLFT)){
    cursor += 1;
    cursor = cursor % 2;
    delay(200);
  }
  if (!digitalRead(BRGHT)){
    cursor -= 1;
    cursor = cursor % -2;
    delay(200);
  }
  if (!digitalRead(BOK)){
    settingsOpened += 1;
    settingsOpened = settingsOpened % 2;
    delay(200);
  }
  // Обработка режимов
  // Экран настроек
  if (settingsOpened == 1){
    switch(mode){
      case 0:
        graphIcon();
        break;
      case 1:
        eyeIcon();
        break;
      case 2:
        settingsIcon();
        break;
    }
    switch(arg){
        case 0:
          if (mode == 2){timeIcon();}else{humIcon();}
          break;
        case 1:
          if (mode == 2){dateIcon();}else{temIcon();}
          break;
        case 2:
          if (mode == 2){threshIcon();}else{co2Icon();}
          break;
    }

    if (cursor == 0) {
      mtrx.clear();
      delay(20);
      mtrx.line(8, 15, 15, 15);
      mtrx.update();
    } else {
      mtrx.clear();
      delay(20);
      mtrx.line(24, 15, 31, 15);
      mtrx.update();
    }
  } else {
    // Часы
    if (mode != 2){
      if (!digitalRead(BDWN) or !digitalRead(BLFT) or !digitalRead(BRGHT) or !digitalRead(BUP)){
        delay(200);
        while (digitalRead(BDWN) and digitalRead(BLFT) and digitalRead(BRGHT) and digitalRead(BUP)){
          drawTime();
          delay(100);
        }
      }
    }
    // Главный экран
    switch(mode){
      case 0:
        if (tresh != 0){
          while (co2.getPPM() >= tresh){
            tone(BUZZ, 300, 100);
            delay(100);
            noTone(BUZZ);
            delay(100);
          }
        }
        mtrx.clear();
        drawGraph(arg);
        mtrx.update();
        break;
      case 1:
        if (tresh != 0){
          while (co2.getPPM() >= tresh){
            tone(BUZZ, 300, 100);
            delay(100);
            noTone(BUZZ);
            delay(100);
          }
        }
        switch(arg){
          case 0:
            mtrx.setCursor(7, 4);
            if (temp.readHumidity() < 10){mtrx.print(0);}
            mtrx.print(int(round(temp.readHumidity())));
            mtrx.setCursor(20, 4);
            mtrx.print("%");
            mtrx.update();
            break;
          case 1:
            mtrx.setCursor(7, 4);
            if (temp.readTemperature() < 10){mtrx.print(0);}
            mtrx.print(int(round(temp.readTemperature())));
            mtrx.setCursor(23, 4);
            mtrx.print("C");
            mtrx.dot(20, 3);
            mtrx.dot(21, 2);
            mtrx.dot(21, 4);
            mtrx.dot(22, 3);
            mtrx.update();
            break;
          case 2:
            mtrx.setCursor(0, 0);
            mtrx.print(co2.getPPM());
            mtrx.setCursor(0, 8);
            mtrx.print("ppm");
            mtrx.update();
            break;
        }
        break;
      case 2:
        if (tresh != 0){
          while (co2.getPPM() >= tresh){
            tone(BUZZ, 300, 100);
            delay(100);
            noTone(BUZZ);
            delay(100);
          }
        }
        switch(arg){
          case 0:
            mtrx.clear();
            if (!digitalRead(BRGHT)){
              if (setCursor < 3){setCursor++;}
            }
            if (!digitalRead(BLFT)){
              if (setCursor > 0){setCursor--;}
            }
            mtrx.setCursor(2, 4);
            if (RTC.getHours() < 10){
              mtrx.print(0);
            } 
            mtrx.print(RTC.getHours());
            mtrx.print(":");
            if (RTC.getMinutes() < 10){
              mtrx.print(0);
            } 
            mtrx.print(RTC.getMinutes());
            mtrx.println();
            mtrx.println();

            switch(setCursor){
              case 0:
                mtrx.line(2, 15, 6, 15);
                mtrx.update();
                break;
              case 1:
                mtrx.line(8, 15, 13, 15);
                mtrx.update();
                break;
              case 2:
                mtrx.line(20, 15, 24, 15);
                mtrx.update();
                break;
              case 3:
                mtrx.line(26, 15, 30, 15);
                mtrx.update();
                break;
            }

            if (!digitalRead(BUP)){
              switch(setCursor){
              case 0:
                if (RTC.getHours() + 10 < 24){RTC.setHours(RTC.getHours()+ 10);}
                break;
              case 1:
                if (RTC.getHours() + 1 < 24){RTC.setHours(RTC.getHours()+ 1);}
                break;
              case 2:
              if (RTC.getMinutes() + 10 < 60){RTC.setMinutes(RTC.getMinutes()+ 10);}
                break;
              case 3:
                if (RTC.getMinutes() + 1 < 60){RTC.setMinutes(RTC.getMinutes()+ 1);}
                break;
              mtrx.update();
              delay(200);
              }
            }
            if (!digitalRead(BDWN)){
              switch(setCursor){
              case 0:
                if (RTC.getHours() - 10 >= 0){RTC.setHours(RTC.getHours() - 10);}
                break;
              case 1:
                if (RTC.getHours() - 1 >= 0){RTC.setHours(RTC.getHours() - 1);}
                break;
              case 2:
              if (RTC.getMinutes() - 10 >= 0){RTC.setMinutes(RTC.getMinutes() - 10);}
                break;
              case 3:
                if (RTC.getMinutes() - 1 >= 0){RTC.setMinutes(RTC.getMinutes() - 1);}
                break;
              mtrx.update();
              delay(200);
              }
            }
            mtrx.update();
            break;
          case 1:
            mtrx.clear();
            if (!digitalRead(BRGHT)){
              if (setCursor < 3){setCursor++;}
            }
            if (!digitalRead(BLFT)){
              if (setCursor > 0){setCursor--;}
            }
            if (!digitalRead(BUP)){
              switch(setCursor){
              case 0:
                if (RTC.getDay() + 10 < 29){RTC.setDay(RTC.getDay() + 10);}
                break;
              case 1:
                if (RTC.getDay() + 1 < 32){RTC.setDay(RTC.getDay() + 1);}
                break;
              case 2:
              if (RTC.getMonth() + 10 < 13){RTC.setMonth(RTC.getMonth() + 10);}
                break;
              case 3:
                if (RTC.getMonth() + 1 < 13){RTC.setMonth(RTC.getMonth() + 1);}
                if (RTC.getMonth() == 12){RTC.setYear(RTC.getYear() + 1);RTC.setMonth(1);}
                break;
              mtrx.update();
              delay(200);
              }
            }
            if (!digitalRead(BDWN)){
              switch(setCursor){
              case 0:
                if (RTC.getDay() - 10 >= 0){RTC.setDay(RTC.getDay() - 10);}
                break;
              case 1:
                if (RTC.getDay() - 1 >= 0){RTC.setDay(RTC.getDay() - 1);}
                break;
              case 2:
              if (RTC.getMonth() - 10 >= 0){RTC.setMonth(RTC.getMonth() - 10);}
                break;
              case 3:
                if (RTC.getMonth() - 1 >= 0){RTC.setMonth(RTC.getMonth() - 1);}
                if (RTC.getMonth() == 1){RTC.setYear(RTC.getYear() - 1); RTC.setMonth(12);}
                break;
              mtrx.update();
              delay(200);
              }
            }
            mtrx.setCursor(2, 4);
            if (RTC.getDay() < 10){mtrx.print("0");}
            mtrx.print(RTC.getDay());
            mtrx.print(".");
            if (RTC.getMonth() < 10){mtrx.print("0");}
            mtrx.print(RTC.getMonth());
            switch(setCursor){
              case 0:
                mtrx.line(2, 15, 6, 15);
                mtrx.update();
                break;
              case 1:
                mtrx.line(8, 15, 13, 15);
                mtrx.update();
                break;
              case 2:
                mtrx.line(20, 15, 24, 15);
                mtrx.update();
                break;
              case 3:
                mtrx.line(26, 15, 30, 15);
                mtrx.update();
                break;
            }

            break;
          case 2:
            mtrx.clear();
            if (!digitalRead(BRGHT)){
              if (setCursor < 3){setCursor++;}
            }
            if (!digitalRead(BLFT)){
              if (setCursor > 0){setCursor--;}
            }
            if (!digitalRead(BUP)){
              switch(setCursor){
                case 0:
                  if (tresh + 1000 < 10000){tresh += 1000;}
                  break;
                case 1:
                  if (tresh + 100 < 10000){tresh += 100;}
                  break;
                case 2:
                  if (tresh + 10 < 10000){tresh += 10;}
                  break;
                case 3:
                  if (tresh + 1 < 10000){tresh += 1;}
                  break;
              }
            }
            if (!digitalRead(BDWN)){
              switch(setCursor){
                case 0:
                  if (tresh - 1000 >= 0){tresh -= 1000;}
                  break;
                case 1:
                  if (tresh - 100 >= 0){tresh -= 100;}
                  break;
                case 2:
                  if (tresh - 10 >= 0){tresh -= 10;}
                  break;
                case 3:
                  if (tresh - 1 >= 0){tresh -= 1;}
                  break;
              }
            }
            mtrx.setCursor(4, 4);
            if (tresh < 1000){mtrx.print("0");}
            if (tresh < 100){mtrx.print("0");}
            if (tresh < 10){mtrx.print("0");}
            mtrx.print(tresh);
            switch(setCursor){
              case 0:
                mtrx.line(4, 15, 8, 15);
                mtrx.update();
                break;
              case 1:
                mtrx.line(10, 15, 14, 15);
                mtrx.update();
                break;
              case 2:
                mtrx.line(16, 15, 20, 15);
                mtrx.update();
                break;
              case 3:
                mtrx.line(22, 15, 26, 15);
                mtrx.update();
                break;
            }
            mtrx.update();
            break;
        }
        break;
    }
    mtrx.update();
    delay(100);
    mtrx.clear();
  }
}
