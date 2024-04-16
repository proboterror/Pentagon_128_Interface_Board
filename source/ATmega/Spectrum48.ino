#include <PS2KeyRaw.h>
#define AX0 4
#define AX1 5
#define AX2 6
#define AX3 7
#define AY0 8
#define AY1 9
#define AY2 10
#define RSTMT 14
#define CSMT  15
#define DATMT 16
#define STBMT 17
#define DATAPIN 2
#define IRQPIN  3

volatile boolean d = HIGH;
uint8_t table[128] = {127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,0,127,127,32,48,127,127,127  ,
1,17,16,33,49,127,127,3,2,18,34,51,50,127,127,112,4,19,36,35,52,127,127,115,116,100,20,84,68,127,127,127,114,99,83,67,66,
127,127,127,98,82,81,64,65,127,127,127,127,97,127,80,127,127,127,127,127,127,127,127,127,127,127,113,96,127,127,127,127,127,127,127,127,127,127,127,
127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127};

PS2KeyRaw keyboard;
void setup() {
  //Инициализация портов
  pinMode(AX0, OUTPUT);   //AX0
  pinMode(AX1, OUTPUT);   //AX1
  pinMode(AX2, OUTPUT);   //AX2
  pinMode(AX3, OUTPUT);   //AX3
  pinMode(AY0, OUTPUT);   //AY0
  pinMode(AY1, OUTPUT);   //AY1
  pinMode(AY2, OUTPUT);   //AY2
  pinMode(RSTMT, OUTPUT); //RES
  pinMode(CSMT, OUTPUT);  //CS
  pinMode(DATMT, OUTPUT); //DAT
  pinMode(STBMT, OUTPUT); //STB
  //Инициализация клавиатуры
  keyboard.begin( DATAPIN, IRQPIN );
  //Инициализация MT8816
  SetAddr(0);
  digitalWrite(RSTMT, LOW);
  digitalWrite(CSMT, LOW);
  digitalWrite(DATMT, LOW);
  digitalWrite(STBMT, LOW); //инициализация
  digitalWrite(CSMT, HIGH); //выбор чипа
  digitalWrite(RSTMT, HIGH);
  digitalWrite(RSTMT, LOW);  //сброс
  digitalWrite(CSMT, LOW);
}
void SetAddr(uint8_t addr) {
  digitalWrite(AX0,bitRead(addr,0));
  digitalWrite(AX1,bitRead(addr,1));
  digitalWrite(AX2,bitRead(addr,2));
  digitalWrite(AX3,bitRead(addr,3));
  digitalWrite(AY0,bitRead(addr,4));
  digitalWrite(AY1,bitRead(addr,5));
  digitalWrite(AY2,bitRead(addr,6));
}
void SetKey(boolean data){
   digitalWrite(CSMT, HIGH); //выбор чипа
   digitalWrite(STBMT, HIGH); //строб on
   digitalWrite(DATMT, data); //данные
   digitalWrite(STBMT, LOW); //строб off    
   digitalWrite(CSMT, LOW);   
}
void loop()
{
  if( keyboard.available() ){
    int c = keyboard.read(); //чтение кода
    switch (c) {
      case 0xE0:              //если считался префикс 0xE0
        break;
      case 0xF0:              //если считался префикс 0xF0 (отпускание клавиши)
        d = LOW;
        break;
      default:  
        SetAddr(table[c]);
        SetKey(d);
        d = HIGH;
    }
  }
}