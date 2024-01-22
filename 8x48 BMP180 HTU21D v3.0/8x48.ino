// #include <EEPROM.h>
#include "MAX7219Driver.h"
// #include <Adafruit_Sensor.h>
// #include <Adafruit_BME280.h>
#include <Arduino.h>
#include <RTClib.h>
#include <Bounce2.h>
#include <BMP180I2C.h>
#include <GyverHTU21D.h>
// #include <Ticker.h>

//----------------------------------Д а т ч и к и-------------------------------------------------------------------
#define I2C_ADDRESS 0x77
BMP180I2C bmp180(I2C_ADDRESS);//Подключать к SDA и SCL, питание +5.0в //Датчик давления и температуры
GyverHTU21D htu; //Подключать к SDA и SCL, питание +3.3в //Датчик влажности

// #include "oclock.h"
#include "alarm.h"
#include "screen.h"

// зуммер
const uint8_t Buzzer_Pin       = 8; //D8 Пассивный пьезозуммер, подключенный к +5 В.

// контакт фоторезистора
const uint8_t Photo_Pin        = A3; //Фоторезистор подключен к +5В с 10КОМ к -В

// Количество сегментов 8x8, которые вы соединяете
const int LEDMATRIX_SEGMENTS   = 6;//шт
const int LEDMATRIX_WIDTH      = LEDMATRIX_SEGMENTS * 8;

// экранные буферы
byte screen_buffer[LEDMATRIX_WIDTH];

// Подключить DIN->D11 CLK->D13
const uint8_t LEDMATRIX_CS_PIN = 9; //D9

// Объект отображения
LEDMatrixDriver lmd(LEDMATRIX_SEGMENTS, LEDMATRIX_CS_PIN);

// Объект часов
// Подключите SDA->A4 SCL->A5
RTC_DS3231 rtc; // I2C

const uint8_t KeyMode_Pin      = 4; //D4 //Ключи, подключенные к -V
const uint8_t KeyPlus_Pin      = 5; //D5 -//-
const uint8_t KeyMinus_Pin     = 6; //D6 -//- (опция)

// Ключевые объекты
Bounce KeyMode  = Bounce();
Bounce KeyPlus  = Bounce();
Bounce KeyMinus = Bounce();

// int EepromSize = EEPROM.length();

//////////////////////////////////////////////////////////////////////////////
// мелодия будильника в формате RTTTL
const char melody1[] PROGMEM = "Aha:d=4,o=4,b=160:8f#5,8f#5,8f#5,8d5,8p,8b,8p,8e5,8p,8e5,8p,8e5,8g#5,8g#5,8a5,8b5,8a5,8a5,8a5,8e5,8p,8d5,8p,8f#5,8p,8f#5,8p,8f#5,8e5,8e5,8f#5,8e5,8f#5,8f#5,8f#5,8d5,8p,8b,8p,8e5,8p,8e5,8p,8e5,8g#5,8g#5,8a5,8b5,8a5,8a5,8a5,8e5,8p,8d5,8p,8f#5,8p,8f#5,8p,8f#5,8e5,8e5";
const char melody2[] PROGMEM = "Europe:d=4,o=5,b=125:p,8p,16b,16a,b,e,p,8p,16c6,16b,8c6,8b,a,p,8p,16c6,16b,c6,e,p,8p,16a,16g,8a,8g,8f#,8a,g.,16f#,16g,a.,16g,16a,8b,8a,8g,8f#,e,c6,2b.,16b,16c6,16b,16a,1b";
const char melody3[] PROGMEM = "IndianaJ:d=4,o=5,b=250:e,8p,8f,8g,8p,1c6,8p.,d,8p,8e,1f,p.,g,8p,8a,8b,8p,1f6,p,a,8p,8b,2c6,2d6,2e6,e,8p,8f,8g,8p,1c6,p,d6,8p,8e6,1f.6,g,8p,8g,e.6,8p,d6,8p,8g,e.6,8p,d6,8p,8g,f.6,8p,e6,8p,8d6,2c6";
const char melody4[] PROGMEM = "Popcorn:d=4,o=5,b=125:8c6,8a#,8c6,8g,8d#,8g,c,8c6,8a#,8c6,8g,8d#,8g,c,8c6,8d6,8d#6,16c6,8d#6,16c6,8d#6,8d6,16a#,8d6,16a#,8d6,8c6,8a#,8g,8a#,c6";
const char melody5[] PROGMEM = "Axel-F:d=4,o=5,b=125:g,8a#.,16g,16p,16g,8c6,8g,8f,g,8d.6,16g,16p,16g,8d#6,8d6,8a#,8g,8d6,8g6,16g,16f,16p,16f,8d,8a#,2g,p,16f6,8d6,8c6,8a#,g,8a#.,16g,16p,16g,8c6,8g,8f,g,8d.6,16g,16p,16g,8d#6,8d6,8a#,8g,8d6,8g6,16g,16f,16p,16f,8d,8a#,2g";

const char* const MelodyArray[] PROGMEM = {melody1, melody2, melody3, melody4, melody5};
const int MelodiesCount = sizeof(MelodyArray) / sizeof(const char*);

Alarm alarm(Buzzer_Pin, MelodyArray, MelodiesCount);

// void tochka();
byte dots = 0;
// Ticker flipper(tochka, 500, 0, MILLIS);

//////////////////////////////////////////////////////////////////////////////
// Текущее время
DateTime CurTime;
int CurHours = -1;//не инициирован
int CurMins  = 0;
int CurSecs  = 0;

#define MY_BME280_ADDRESS (0x76)
#define SEALEVELPRESSURE_HPA (1013.25)

// Подключите SDA->A4 SCL->A5
// Adafruit_BME280 bme; // I2C

bool fBME280present = false;

// //O'Clock version 11.06.2020
#define VERSION "1.1"


//////////////////////////////////////////////////////////////////////////////
// O'Clock main loop() with Clock Menu code

//задержка в режиме автоматического повтора ключа
//0-without delay
#define KEY_DELAY 1 //если 0 то каждую минуту меняются показатели (давление, температура и дата ) SHOW_PRESSURE_ON_SECS 15 //секунды, чтобы показать давление
                                                                                                //SHOW_TEMP_ON_SECS     30 //секунды, чтобы показать температуру
                                                                                                //SHOW_DATE_ON_SECS     45 //секунды, чтобы показать дату
                    //если 1 то SWITCH_MODE_PERIOD     2 //n минут - показ периода давления/температуры/даты

//показать давление в мм рт.ст.
#define PRESSURE_IN_MM

//автоматически показывать давление, температуру и дату  
#define SWITCH_MODE_PERIOD     2 //n минут - показ периода давления/температуры/даты
#define AUTO_SHOW_PRESSURE     2 //0-Выкл., иначе продолжительность в секундах
#define AUTO_SHOW_TEMP         2 //0-Выкл., иначе продолжительность в секундах
#define AUTO_SHOW_DATE         2 //0-Выкл., иначе продолжительность в секундах

//в это время отображался параметр секунд
#define SHOW_PRESSURE_ON_SECS 15 //секунды, чтобы показать давление
#define SHOW_TEMP_ON_SECS     30 //секунды, чтобы показать температуру
#define SHOW_DATE_ON_SECS     45 //секунды, чтобы показать дату

//////////////////////////////////////////////////////////////////////////////
#define BLINK_LOG   8 /* 8 -> 500ms */
#define IS_BLINK() (((millis() >> (BLINK_LOG + 1)) & 1) == 0) 

void Beep(int n = 1);
void BeepError();
//////////////////////////////////////////////////////////////////////////////
//пиктограммы
//первый байт - размер
const byte AlarmPic[] = 
  {6, 0x20, 0x3e, 0xbf, 0xbf, 0x3e, 0x20};
  //{6, 0x20, 0x3e, 0xa3, 0xa3, 0x3e, 0x20};
const byte GradC[] = 
  {5, 0x02, 0x00, 0x1c, 0x22, 0x22};

//////////////////////////////////////////////////////////////////////////////
// Конфигурация часов
bool fConfigChanged = false;
struct ClockConfig 
{
  //тревога
  uint8_t days;
  uint8_t hour;
  uint8_t mins;
  uint8_t melody;
} config;

////////////////////////////////////////////////////////////////////////////////////////////
void tochka()
{
//      dots = !dots; 
// }
static int cikl = 0;
  cikl++;
  if(cikl > 80)
  {
    // tochka();
    dots = !dots; 
    cikl = 0;
  }
} 
/////////////////////////////////////////////////////////////////////////////////////////////////
float getPressureBMP180()
{ 
  if (!bmp180.measurePressure())
  {
  }     
  do
  {
    delay(10);
  } while (!bmp180.hasValue());
     float value3 = (bmp180.getPressure()/133.3);
//    float value3 = bmp180.getPressure();
     return value3;
}

/////////////////////////////////////////////////////////////////////////////////////////////
float getTemperaturBMP180()
{ 
  if (!bmp180.measureTemperature())
  {
  }     
  do
  {
    delay(10);
  } while (!bmp180.hasValue());
     float value4 = bmp180.getTemperature();
     return value4;
} 

/////////////////////////////////////////////////////////////////////////////////////////////////
// void LoadConfig()
// {
//   Serial.println(F("LoadConfig"));
//   EEPROM.get(0, config);
//   if(config.days != 0xff) //не инициализирован EEPROM
//     alarm.SetAlarm(config.days, config.hour, config.mins, config.melody);
// }

/////////////////////////////////////////////////////////////////////////////////////////////////
// void SaveConfig()
// {
//   if(fConfigChanged)
//   {
//     Serial.println(F("SaveConfig"));
//     fConfigChanged = false;
//     alarm.GetAlarm(config.days, config.hour, config.mins, config.melody);
//     EEPROM.put(0, config);
//   }
// }

////////////////////////////////////////////////////////////////////////////////////////////////
void ConfigChanged()
{
  fConfigChanged = true;
}

//////////////////////////////////////////////////////////////////////////////////////////////
int GetCurTime()
{
  int h = CurHours;
  int m = CurMins;
  int s = CurSecs;

  CurTime  = rtc.now();
  CurHours = CurTime.hour();
  CurMins  = CurTime.minute();
  CurSecs  = CurTime.second();

  if(h == -1)
  {
    // первый звонок
    return CHANGED_ALL;
  }
  // проверить измененные цифры
  int mod = CHANGED_NOTHING;
  if(h / 10 != CurHours / 10)
     mod = CHANGED_HOUR10;
    else if(h % 10 != CurHours % 10)
           mod = CHANGED_HOUR1;
         else if(m / 10 != CurMins / 10)
                mod = CHANGED_MIN10;
              else if(m % 10 != CurMins % 10)
                     mod = CHANGED_MIN1;
                   else if(s / 10 != CurSecs / 10) 
                          {mod = CHANGED_SEC10; /*tochka();*/}
                         else if(s % 10 != CurSecs % 10)  
                                {mod = CHANGED_SEC1; tochka();}

                  //  else if(s != CurSecs)
                    else  {mod = CHANGED_SEC; tochka();}

  return mod;
}

//////////////////////////////////////////////////////////////////////////////
void SetTime(const DateTime& dt)
{
  rtc.adjust(dt);
}

//////////////////////////////////////////////////////////////////////////////
void AdjustBright()
{
  // текущий уровень яркости
  static int CurBrightLevel = 0;

  // логарифмический уровень
  byte level[] = {0,0,0,0,0,0,1,2,4,8}; // 0 = низкий, 15 = высокий
  //максимальное аналоговое значение
  const int MaxAnalogValue = 1023;

  int v = analogRead(Photo_Pin);
  //Serial.print(F("Light level=")); Serial.println(v, DEC);

#if 0
  //выключайте светодиод в полной темноте
  if(v == 0)
    lmd.setEnabled(false);
  else
    lmd.setEnabled(true);
#endif

  // расчетный линейный уровень
  int l = (v * sizeof(level)) / MaxAnalogValue;
  // получить логарифмический уровень
  l = level[l];

  if(CurBrightLevel != l)
  {
    CurBrightLevel = l;
    lmd.setIntensity(CurBrightLevel);
  }
}

//////////////////////////////////////////////////////////////////////////////
void Beep(int n)
{
  if(n < 1 || n > 5)
    n = 1;
  while(n--)
  {
    tone(Buzzer_Pin, 500, 10);
    if(n)
      delay(150);
    else
      delay(10);
  }
  noTone(Buzzer_Pin);
}

//////////////////////////////////////////////////////////////////////////////
void BeepError()
{
  tone(Buzzer_Pin, 500, 1000);
  delay(1000);
  noTone(Buzzer_Pin);
}


//////////////////////////////////////////////////////////////////////////////
void ShowBuffer(byte* buffer)
{
  if(buffer && screen_buffer != buffer)
    memcpy(screen_buffer, buffer, LEDMATRIX_WIDTH);

  for(byte row = 0; row < LEDMATRIX_WIDTH; row++)
    lmd.setColumn(row, screen_buffer[row]);
  lmd.display();
}

/////////////////////////////////////////////////////////////////////////////
// bool fBME280present = false; 
bool IsTPHpresent()
{
  return fBME280present;
}

///////////////////////////////////////////////////////////////////////////////
// bool ReadTPH(float& Temperature, float& Pressure, float& Humidity)
// {
//   if(!fBME280present)//false
//     return false;
//   bme.takeForcedMeasurement();
//   Temperature = bme.readTemperature();
//   Pressure    = bme.readPressure();
//   Humidity    = bme.readHumidity();
//   return true;
// }
bool ReadTPH(float& Temperature, float& Pressure, float& Humidity)
{
  Temperature = getTemperaturBMP180();
  Pressure    = getPressureBMP180();
  
  // функция опрашивает датчик по своему таймеру
  if (htu.readTick()) 
      {
        // можно забирать значения здесь или в другом месте программы
        Humidity = htu.getHumidity();
      }
  return true;
}
//////////////////////////////////////////////////////////////////////////////
// Коды меню часов
enum {
  //Показать меню
  MODE_SHOW_BEGIN,
    MODE_SHOW_CLOCK,
    MODE_SHOW_TEMP,
    MODE_SHOW_PRESSURE,
    MODE_SHOW_DATE,
    MODE_SHOW_ALARM,
  MODE_SHOW_END,

  //Настройки
  MODE_SET_BEGIN,
    MODE_SET_TIME,
    MODE_SET_ALARM,
    MODE_SHOW_VERSION,
  MODE_SET_END,

  //изменить меню времени/даты
  MODE_CH_BEGIN,
    MODE_CH_HOUR,
    MODE_CH_MIN,
    MODE_CH_SEC,
    MODE_CH_DAY,
    MODE_CH_MONTH,
    MODE_CH_YEAR,
  MODE_CH_END,

  //изменить меню будильника
  MODE_CH_ALARM_BEGIN,
    MODE_CH_ALARM_HOUR,
    MODE_CH_ALARM_MIN,
    MODE_CH_ALARM_DAY1,
    MODE_CH_ALARM_DAY2,
    MODE_CH_ALARM_DAY3,
    MODE_CH_ALARM_DAY4,
    MODE_CH_ALARM_DAY5,
    MODE_CH_ALARM_DAY6,
    MODE_CH_ALARM_DAY7,
    MODE_CH_ALARM_MELODY,
  MODE_CH_ALARM_END,
  
  MODE_UNUSED
  //переместить сюда неиспользуемый идентификатор меню
};

//////////////////////////////////////////////////////////////////////////////
const char DaysSymbol[] = "MTWRFSU";
const char Months[12][4] = {"Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"};

//////////////////////////////////////////////////////////////////////////////
//изменить временной режим
void ChangeTime(int Mode, int Increment)
{
  int h = CurTime.hour();
  int m = CurTime.minute();
  int s = CurTime.second();
  int D = CurTime.day();
  int M = CurTime.month();
  int Y = CurTime.year();

  switch(Mode)
  {
    case MODE_CH_HOUR:
      h += Increment;
      if(h < 0)
        h = 23;
      else if(h > 23)
        h = 0;
      break;
    case MODE_CH_MIN:
      m += Increment;
      if(m < 0)
        m = 59;
      else if(m > 59)
        m = 0;
      break;
    case MODE_CH_SEC:
      s += Increment;
      if(s < 0)
        s = 59;
      else if(s > 59)
        s = 0;
      break;
    case MODE_CH_DAY:
      D += Increment;
      if(D < 1)
        D = 31;
      else if(D > 31)
        D = 1;
      break;
    case MODE_CH_MONTH:
      M += Increment;
      if(M < 1)
        M = 12;
      else if(M > 12)
        M = 1;
      break;
    case MODE_CH_YEAR:
      Y += Increment;
      if(Y < 2010)
        Y = 2099;
      else if(Y > 2099)
        Y = 2010;
      break;
  }

  char buff[16];
  if(Mode == MODE_CH_HOUR || Mode == MODE_CH_MIN || Mode == MODE_CH_SEC)
  {
    sprintf(buff, "%02d:%02d.%02d", h, m, s);
    PrintTinyString(buff, 0, 1, true);
    int x = 0;
    if(Mode == MODE_CH_MIN)
      x = 12;
    else if(Mode == MODE_CH_SEC)
      x = 24;
    if(IS_BLINK())
    { 
      InverseBlock(x, 7, 7, 1);
    }
  }
  else
  {
    sprintf(buff, "%02d  %s.%d", D, Months[M - 1], Y - 2000);
    PrintTinyString(buff, 0, 1);
    int x  = 0;
    int sx = 7;
    if(Mode == MODE_CH_MONTH)
    {
      x  = 10;
      sx = 10;
    }
    else if(Mode == MODE_CH_YEAR)
    {
      x  = 24;
      sx = 6;
    }
    if(IS_BLINK())
    {  
      InverseBlock(x, 7, sx, 1);
    }
  }

  if(Increment)
  {
    SetTime(DateTime(Y, M, D, h, m, s));
    GetCurTime();
  }
}

//////////////////////////////////////////////////////////////////////////////
//изменить режим будильника
void ChangeAlarm(int Mode, int Increment)
{
  uint8_t d, uh, um, umelody;
  alarm.GetAlarm(d, uh, um, umelody);

  //нам нужны числа со знаком
  int h = uh;
  int m = um;
  int melody = umelody;

  switch(Mode)
  {
    case MODE_CH_ALARM_HOUR:
      h += Increment;
      if(h < 0)
        h = 23;
      else if(h > 23)
        h = 0;
      break;
    case MODE_CH_ALARM_MIN:
      m += Increment;
      if(m < 0)
        m = 59;
      else if(m > 59)
        m = 0;
      break;
    case MODE_CH_ALARM_DAY1:  
    case MODE_CH_ALARM_DAY2:  
    case MODE_CH_ALARM_DAY3:  
    case MODE_CH_ALARM_DAY4:  
    case MODE_CH_ALARM_DAY5:  
    case MODE_CH_ALARM_DAY6:  
    case MODE_CH_ALARM_DAY7: 
    { 
      uint8_t day  = Mode - MODE_CH_ALARM_DAY1;
      uint8_t mask = 1 << day;;
      if(Increment != 0)
        d ^= mask;
      break;
    }
    case MODE_CH_ALARM_MELODY:
      melody += Increment;
      if(melody < 0)
        melody = MelodiesCount - 1;
      else if(melody >= MelodiesCount)  
        melody = 0;
      break;  
  }

  char buff[16];
  if(Mode == MODE_CH_ALARM_HOUR || Mode == MODE_CH_ALARM_MIN)
  {
    sprintf(buff, "%02d:%02d", h, m);
    PrintTinyString(buff, 8, 1, true);
    PrintPictogram(0, &AlarmPic[1], AlarmPic[0]);

    int x = 8;
    if(Mode == MODE_CH_ALARM_MIN)
      x = 20;
    if(IS_BLINK())
    {
      InverseBlock(x, 7, 7, 1);
    }
  }
  else if(Mode == MODE_CH_ALARM_MELODY)
  {
    sprintf(buff, "Melody %d", melody + 1);
    PrintTinyString(buff, 0, 1, true);
    if(IS_BLINK())
    {
      InverseBlock(28, 7, 3, 1);
    }
  }
  else
  {
    PrintTinyString(DaysSymbol, 0, 0, true);
    uint8_t mask = 1;
    for(int i = 0; i < 7; ++i)
    {
      if((d & mask) != 0)
        InverseBlock(i * 4, 6, 3, 1);
      mask <<= 1;  
    }
    if(IS_BLINK())
      InverseBlock((Mode - MODE_CH_ALARM_DAY1) * 4, 7, 3, 1);
  }
  
  if(Increment != 0)
  {
    ConfigChanged();
    alarm.SetAlarm(d, h, m, melody);
    if(Mode == MODE_CH_ALARM_MELODY)
      alarm.Play();
  }
}

//////////////////////////////////////////////////////////////////////////////
#define KEY_TIMEOUT_PERIOD     5  //sec
#define LONG_PRESSED_PERIOD 2000  //msec
#define LOOP_SLEEP_TIME       50  //msec

//////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
void setup()
{
  // pinMode(Buzzer_Pin, OUTPUT); // Установить зуммер на выход
  // Beep();

  // инициализировать дисплей
  // lmd.clear();
  // lmd.setEnabled(true);
  // lmd.setIntensity(1); // 0 = low, 15 = high

  // Serial.begin(9600);
  // Serial.println(F("Vikonix O'Clock Version:" VERSION));
  // Serial.print(F("EEPROM size=")); Serial.println(EepromSize, DEC);
  // flipper.start();
  if(!rtc.begin())
  {
    // Serial.println(F("Rtc ERROR"));
    // BeepError();
  }

  // if(rtc.lostPower())
  // {
  //   rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  //   Beep(2);
  // }
  // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));

  KeyMode.attach(KeyMode_Pin, INPUT_PULLUP);
  KeyMode.interval(20);
  KeyPlus.attach(KeyPlus_Pin, INPUT_PULLUP);
  KeyPlus.interval(20);
  KeyMinus.attach(KeyMinus_Pin, INPUT_PULLUP);
  KeyMinus.interval(20);

  // LoadConfig();

  // if(!bme.begin(MY_BME280_ADDRESS)) 
  //   Serial.println(F("BME280 sensor not found"));
  // else
  // {
  //   // Serial.println(F("BME280 sensor Ok"));
  //   fBME280present = true;
  //   bme.setSampling(
  //     Adafruit_BME280::MODE_FORCED,
  //     Adafruit_BME280::SAMPLING_X1, // температура
  //     Adafruit_BME280::SAMPLING_X1, // pressure
  //     Adafruit_BME280::SAMPLING_X1, // влажность
  //     Adafruit_BME280::FILTER_OFF   
  //   );
  // }
        htu.begin();   // запустить датчик

        if (bmp180.begin()) // запустить датчик
          {
            fBME280present = true;
          }
        // bmp180.begin();
        //сбросить датчик к параметрам по умолчанию.
        bmp180.resetToDefaults();
        //включить режим сверхвысокого разрешения для измерения давления
        bmp180.setSamplingMode(BMP180MI::MODE_UHR);
}

/////////////////////////////////////////////////////////////////////////////////////
void loop()
{
  // flipper.update();
  // static int cikl = 0;
  // cikl++;
  // if(cikl > 300)
  // {
  //   // tochka();
  //   dots = !dots; 
  //   cikl = 0;
  // } 

  //текущий режим
  static int  Mode = MODE_SHOW_CLOCK;
  static uint32_t ModeTimeout = 0;
  
  bool fBlink = false;
  static unsigned long blinkt = 0;
  
  unsigned long ms = millis();
  if(ms - blinkt >= (1ul << BLINK_LOG))
  {
    blinkt = ms;
    fBlink = true;
  }

  char buff[16];

  //Обновить параметры
  int TimeChanged = GetCurTime();
  if(TimeChanged)
    AdjustBright();

  bool fAlarm = false;
  if(Mode == MODE_SHOW_CLOCK || Mode == MODE_CH_ALARM_MELODY)
    fAlarm = alarm.CheckTime(CurTime);

  KeyMode.update();
  KeyPlus.update();
  KeyMinus.update();

  // Проверьте нажатие клавиш
  // разрешен только один ключ во времени
  bool fEvent = false;
  int  increment = 0;

  if(KeyMode.fell())
    {
      // Serial.println(F("Key mode"));
      if(fAlarm)
        alarm.Reset();
      else  
      {
        fEvent = true;

        if(Mode < MODE_SHOW_END)
          {
            ++Mode;
            if(Mode == MODE_SHOW_TEMP && !IsTPHpresent())
              //BME280 не запущен
              ++Mode;
            if(Mode <= MODE_SHOW_BEGIN || Mode >= MODE_SHOW_END)
              Mode = MODE_SHOW_BEGIN + 1;
            if(Mode == MODE_SHOW_CLOCK)  
              TimeChanged = CHANGED_ALL;
          }
        else if(Mode < MODE_SET_END)
                {
                  if(Mode == MODE_SET_TIME)
                    Mode = MODE_CH_BEGIN + 1;
                  else if(Mode == MODE_SET_ALARM)
                    Mode = MODE_CH_ALARM_BEGIN + 1;
                  else if(Mode == MODE_SHOW_VERSION)
                        {
                          Mode = MODE_SHOW_CLOCK;
                          TimeChanged = CHANGED_ALL;
                        }
                }
              else if(Mode < MODE_CH_END)
                      {
                        if(++Mode >= MODE_CH_END)
                          Mode = MODE_SET_TIME;
                      }
                    else if(Mode < MODE_CH_ALARM_END)
                          {
                            if(Mode == MODE_CH_ALARM_MELODY)
                              alarm.Reset();
                            if(++Mode >= MODE_CH_ALARM_END)
                              Mode = MODE_SET_ALARM;
                          }
      }
    }//end KeyMode.fell()
  else if(KeyPlus.fell())
          {
            // Serial.println(F("Key plus"));
            if(fAlarm)
              alarm.Reset(&CurTime);
            else
                {
                  fEvent = true;

                  if(Mode < MODE_SET_END)
                    {
                      if(Mode <= MODE_SET_BEGIN || ++Mode >= MODE_SET_END)
                        Mode = MODE_SET_BEGIN + 1;
                    }
                  else
                    {
                      //изменить дату/время
                      ++increment;
                    }
                }
          }
        else if(KeyMinus.fell())
                {
                  // Serial.println(F("Key minus"));
                  if(fAlarm)
                    alarm.Reset(&CurTime);
                  else
                      {
                        fEvent = true;

                        if(Mode < MODE_SET_END)
                          {
                            if(--Mode <= MODE_SET_BEGIN)
                              Mode = MODE_SET_END - 1;
                          }
                        else
                            {
                              //изменить дату/время
                              --increment;
                            }
                      }
                }

  //проверь долгое нажатие клавиши
  if(Mode == MODE_CH_HOUR
  || Mode == MODE_CH_MIN
  || Mode == MODE_CH_SEC
  || Mode == MODE_CH_DAY
  || Mode == MODE_CH_YEAR
  || Mode == MODE_CH_ALARM_HOUR
  || Mode == MODE_CH_ALARM_MIN
  )
  {
    static int KeyDelay = 0;
    if(!fEvent && !KeyPlus.read())
      {
        if(KeyPlus.currentDuration() >= LONG_PRESSED_PERIOD)
          {
            //долгое нажатие
            if(++KeyDelay >= KEY_DELAY)
              {
                fEvent = true;
                increment = 1;
                KeyDelay = 0;
              }
          }
      }
    else if(!fEvent && !KeyMinus.read())
          {
            if(KeyMinus.currentDuration() >= LONG_PRESSED_PERIOD)
              {
                //долгое нажатие
                if(++KeyDelay >= KEY_DELAY)
                  {
                    fEvent = true;
                    increment = -1;
                    KeyDelay = 0;
                  }
              }
          }
          else
              {
                KeyDelay = 0;
              }
  }//end mode

  if(fEvent)
    {
      Beep();
      ModeTimeout = CurTime.secondstime() + KEY_TIMEOUT_PERIOD;
    }
  // Тайм-аут режима проверки
  else if(Mode != MODE_SHOW_CLOCK && CurTime.secondstime() > ModeTimeout)
          {
            // Serial.println(F("reset mode"));
            ModeTimeout = 0;
            TimeChanged = SCROLL_ALL;//CHANGED_ALL;
            if(Mode == MODE_CH_ALARM_MELODY)
              alarm.Reset();
              
            Mode = MODE_SHOW_CLOCK;
          }

  // Применить режим
  if(Mode == MODE_SHOW_CLOCK)
  {
    fBlink = false;
    if(TimeChanged)
    {
      uint8_t h, m, d, n;
      alarm.GetAlarm(d, h, m, n);
      bool alarm = (Alarm::DayOfTheWeek(CurTime.dayOfTheWeek()) & d) != 0;

      DisplayTime(CurHours, CurMins, CurSecs, alarm, TimeChanged, 0);
      // SaveConfig();

        #if AUTO_SHOW_PRESSURE != 0
              if(CurSecs == SHOW_PRESSURE_ON_SECS)
                {  
                  Mode   = MODE_SHOW_PRESSURE;
                  fEvent = true;
                  ModeTimeout = CurTime.secondstime() + AUTO_SHOW_TEMP;
                }
              else 
        #endif
        #if AUTO_SHOW_TEMP != 0
              if(CurSecs == SHOW_TEMP_ON_SECS)
                {  
                  Mode   = MODE_SHOW_TEMP;
                  fEvent = true;
                  ModeTimeout = CurTime.secondstime() + AUTO_SHOW_TEMP;
                }
              else 
        #endif
        #if AUTO_SHOW_DATE != 0
              if(CurSecs == SHOW_DATE_ON_SECS)
                {  
                  Mode   = MODE_SHOW_DATE;
                  fEvent = true;
                  ModeTimeout = CurTime.secondstime() + AUTO_SHOW_DATE;
                }
              else
        #endif      
          {
            fEvent = false;
          }

      if(fEvent && SWITCH_MODE_PERIOD && 0 != CurMins % SWITCH_MODE_PERIOD)
        {
            //пропустить SWITCH_MODE_PERIOD
            Mode = MODE_SHOW_CLOCK;
            fEvent = false;
        }
    }
  }
  
  if(fEvent || fBlink)
    {
      if(Mode == MODE_SHOW_DATE)
        {
          sprintf(buff, "%d  %s. %d", CurTime.day(), Months[CurTime.month() - 1], CurTime.year() - 2000);
          PrintTinyString(buff, 0, 1);
        }
      else if(Mode == MODE_SHOW_TEMP || Mode == MODE_SHOW_PRESSURE)
              {
                float t, p, h;
                if(ReadTPH(t, p, h))
                  {
                    if(Mode == MODE_SHOW_TEMP)
                      {
                        int temp = t;
                        int rh   = h;
                        sprintf(buff, "%2d  %2d%%H", temp, rh);
                        PrintTinyString(buff, 0, 1, true);
                        PrintPictogram(8, &GradC[1], GradC[0]);
                      }
                    else
                          {
                            #ifndef PRESSURE_IN_MM
                                      int hPa  = p / 100.0f;
                                      sprintf(buff, "%d  hPa", hPa);
                            #else
                                      int mmHg  = p;
                                      sprintf(buff, "%d   mmHg", mmHg);
                            #endif          
                            PrintTinyString(buff, 1, 1);
                          }
                  }
                else
                    {
                      sprintf(buff, "BME280 ?");
                      PrintTinyString(buff, 0, 1);
                    }  
              }
      else if(Mode == MODE_SHOW_ALARM)
              {  
                uint8_t h, m, d, n;
                alarm.GetAlarm(d, h, m, n);
                sprintf(buff, "%02d:%02d", h, m);
                PrintTinyString(buff, 13, 1, true);
                PrintPictogram(3, &AlarmPic[1], AlarmPic[0]);
                PrintPictogram(0, &d, 1);
                uint8_t wd = Alarm::DayOfTheWeek(CurTime.dayOfTheWeek());
                PrintPictogram(1, &wd, 1);
              }
            else if(Mode == MODE_SET_TIME)
                    PrintTinyString("Set>Time", 0, 1);
                  else if(Mode == MODE_SET_ALARM)
                          PrintTinyString("Set>Alrm", 0, 1);
                        else if(Mode > MODE_CH_BEGIN && Mode < MODE_CH_END)
                                {
                                  ChangeTime(Mode, increment);
                                  if(increment)
                                    {
                                      //время изменено: тайм-аут обновления
                                      ModeTimeout = CurTime.secondstime() + KEY_TIMEOUT_PERIOD;
                                    }
                                }
                              else if(Mode > MODE_CH_ALARM_BEGIN && Mode < MODE_CH_ALARM_END)
                                      ChangeAlarm(Mode, increment);
                                  else //if(Mode == MODE_SHOW_VERSION)
                                      PrintTinyString("O'Clk- " VERSION, 0, 1);
                            
      ShowBuffer();
    }//end fEvent || fBlink

  // delay(LOOP_SLEEP_TIME);
}
