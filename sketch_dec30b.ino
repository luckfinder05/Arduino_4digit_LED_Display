//**************************************************************//
//  Name    : 4-digit 7-segment LED-display
//  Author  : Novitskiy Vladislav
//  Date    : 30 Dec, 2018
//  Modified:
//  Version : 1.0
//  Notes   : Программа выводит на 4-значный цифровой дисплей
//          : цифры и текст
//****************************************************************

//Пин подключен к ST_CP входу 74HC595
const int latchPinVCC = 8;
//Пин подключен к SH_CP входу 74HC595
const int  clockPinVCC = 12;
//Пин подключен к DS входу 74HC595
const int  dataPinVCC = 11;

//Пин подключен к ST_CP входу 74HC595
const int  latchPinGND = 7;
//Пин подключен к SH_CP входу 74HC595
const int  clockPinGND = 6;
//Пин подключен к DS входу 74HC595
const int  dataPinGND = 5;

//{DP, G, F, E, D, C, B, A}
byte seg[12] = {
  B00111111, //Цифра 0
  B00000110, //Цифра 1
  B01011011, //Цифра 2
  B01001111, //Цифра 3
  B01100110, //Цифра 4
  B01101101, //Цифра 5
  B01111101, //Цифра 6
  B00000111, //Цифра 7
  B01111111, //Цифра 8
  B01101111, //Цифра 9
  B00000010, //Знак -
  B00000000  //Пустой разряд
};

int err[4][8] = {
  {0, 0, 0, 0, 0, 0, 0, 0}, //Пусто
  {1, 0, 0, 1, 1, 1, 1, 0}, //E
  {0, 0, 0, 0, 1, 0, 1, 0}, //r
  {0, 0, 0, 0, 1, 0, 1, 0}  //r
};

byte LED_Array[8][8];


void setup() {
  //устанавливаем режим OUTPUT
  pinMode(latchPinVCC, OUTPUT);
  pinMode(clockPinVCC, OUTPUT);
  pinMode(dataPinVCC, OUTPUT);

  pinMode(latchPinGND, OUTPUT);
  pinMode(clockPinGND, OUTPUT);
  pinMode(dataPinGND, OUTPUT);

  Serial.begin(9600);

}

void loop() {


  byte valShow = B11110111;
  byte gndShow = B11101111;
  //    do {
  //      gndShow = gndShow >> 1;
  //      PushToShiftRegister (latchPinGND, clockPinGND, dataPinGND, gndShow);
  //      PushToShiftRegister (latchPinVCC, clockPinVCC, dataPinVCC, valShow);
  //
  //      delay(500);
  //    } while (gndShow != B00001110);


  randomSeed(analogRead(1));
  //  int RunTimeMinutes = random(0, 10000);
  unsigned long startMillis = millis();

  int RunTimeMinutes = startMillis /  60000;

  PrintNum4xLED(RunTimeMinutes, 1000);
}

//
//Фунция целочисленного возведения в степень
int intPower (int base, int exp) {
  int result = 1;
  if (base != 0) {
    for (int i = 0; i < exp; i++) {
      result = result * base;
    }
  }
  return result;
}

//Вывести число на 4-значный дисплей
//Первый аргумент функции - число для вывода на дисплей
//Второй аргумент - задержка в милисекундах. Важна, например, когда выводятся числа, изменяющие значение чаще чем необходимо
// чтобы исключить мерцание.
void PrintNum4xLED (int NumPrinted, int delayTime) {
  unsigned long startMillis = millis();

  int division[4];
  for (int i = 4; i >= 1; i--) {
    division[i - 1] = ((NumPrinted % intPower(10, i) - NumPrinted % intPower(10, i - 1)) / intPower(10, i - 1));
  }

  //Вычисляем количество разрядов в числе, чтобы не выводить нули перед числом
  int count = 4;
  if (NumPrinted < 10) {
    count = 1;
  }
  else if (NumPrinted < 100) {
    count = 2;
  }
  else if (NumPrinted < 1000) {
    count = 3;
  }

  //Выводим число
  do {
    for (int i = 1; i <= count; i++) {
      //Привязка начала числа к левому краю
      ActivateDigit(count - i + 1);

      //Привязка начала числа к правому краю
      //ActivateDigit(5-i);
      PrintSign(division[i - 1]);
      delay(6);

      //Чтобы избавиться от остаточного свечения гасим все светодиоды.
      PushToShiftRegister (latchPinGND, clockPinGND, dataPinGND, B11111111);
      PushToShiftRegister (latchPinVCC, clockPinVCC, dataPinVCC, B00000000);
    }
  } while ((millis() - startMillis) < delayTime);
}

//
//Вывести число на дисплей в активированную ячейку (ячейка активируется функцией ActivateDigit)
void PrintSign (int intNum) {
  PushToShiftRegister (latchPinVCC, clockPinVCC, dataPinVCC, seg[intNum]);
}

//
//Активация ячейки на дисплее. Выводится будет в выбранную ячейку. Номера ячеек от 1 до 4.
void ActivateDigit (int digitNum) {

  PushToShiftRegister (latchPinGND, clockPinGND, dataPinGND, B11101111 >> digitNum);
}


//Функция бегающего элемента вокруг активной цифры
void RunThroughDigit (int count, int valDelay) {
  int valShow = 0;

  do {
    for (int j = 0; j <= 5; j++) {
      valShow = B00000001 << j;
      PushToShiftRegister (latchPinVCC, clockPinVCC, dataPinVCC, valShow);
      delay(valDelay);
    }
    count--;
  } while (count > 0);
}

void PushToShiftRegister (int latchPin, int clockPin, int dataPin, byte n2Display) {

  // устанавливаем синхронизацию "защелки" на LOW
  digitalWrite(latchPin, LOW);
  // передаем последовательно на dataPin
  shiftOut(dataPin, clockPin, MSBFIRST, n2Display);
  //"защелкиваем" регистр, тем самым устанавливая значения на выходах
  digitalWrite(latchPin, HIGH);

}
