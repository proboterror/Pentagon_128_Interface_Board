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

uint8_t table[128] = {127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,0,127,127,32,48,127,127,127,1,17,16,33,49,127,
127,3,2,18,34,51,50,127,127,112,4,19,36,35,52,127,127,115,116,100,20,84,68,127,127,127,114,99,83,67,66,127,
127,127,98,82,81,64,65,127,127,127,127,97,127,80,127,127,127,127,127,127,127,127,127,127,127,113,96,127,127,127,127,127,
127,127,127,127,127,127,64,127,127,127,127,52,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127,127};

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
  
  SetAddr(table[0x12]);
  SetKey(LOW);
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
      case 0x66:              //если считался код 0x66 [BS]
        SetAddr(table[0x12]);
        SetKey(d);
        SetAddr(table[0x45]);
        SetKey(d);
        d = HIGH;
        break;
      case 0x6B:              //если считался код 0x6B [Left]
        SetAddr(table[0x12]);
        SetKey(d);
        SetAddr(table[0x2E]);
        SetKey(d);
        d = HIGH;
        break;
      case 0x72:              //если считался код 0x72 [Down]
        SetAddr(table[0x12]);
        SetKey(d);
        SetAddr(table[0x36]);
        SetKey(d);
        d = HIGH;
        break;
      case 0x75:              //если считался код 0x75 [Up]
        SetAddr(table[0x12]);
        SetKey(d);
        SetAddr(table[0x3D]);
        SetKey(d);
        d = HIGH;
        break;
      case 0x74:              //если считался код 0x74 [Right]
        SetAddr(table[0x12]);
        SetKey(d);
        SetAddr(table[0x3E]);
        SetKey(d);
        d = HIGH;
        break;
      case 0x76:              //если считался код 0x76 [Edit]
        SetAddr(table[0x12]);
        SetKey(d);
        SetAddr(table[0x16]);
        SetKey(d);
        d = HIGH;
        break;
      case 0x58:              //если считался код 0x58 [Caps lock]
        SetAddr(table[0x12]);
        SetKey(d);
        SetAddr(table[0x1E]);
        SetKey(d);
        d = HIGH;
        break;
      case 0x0D:              //если считался код 0x0D [Ext mode]
        SetAddr(table[0x12]);
        SetKey(d);
        SetAddr(table[0x59]);
        SetKey(d);
        d = HIGH;
        break;
      case 0x41:              //если считался код 0x41 [,]
        SetAddr(table[0x59]);
        SetKey(d);
        SetAddr(table[0x31]);
        SetKey(d);
        d = HIGH;
        break;
      case 0x49:              //если считался код 0x49 [.]
        SetAddr(table[0x59]);
        SetKey(d);
        SetAddr(table[0x3A]);
        SetKey(d);
        d = HIGH;
        break;
      case 0x4C:              //если считался код 0x4C [;]
        SetAddr(table[0x59]);
        SetKey(d);
        SetAddr(table[0x44]);
        SetKey(d);
        d = HIGH;
        break;
      case 0x52:              //если считался код 0x52 ["]
        SetAddr(table[0x59]);
        SetKey(d);
        SetAddr(table[0x4D]);
        SetKey(d);
        d = HIGH;
        break;
      default:  
        SetAddr(table[c]);
        SetKey(d);
        d = HIGH;
    }
  }
}