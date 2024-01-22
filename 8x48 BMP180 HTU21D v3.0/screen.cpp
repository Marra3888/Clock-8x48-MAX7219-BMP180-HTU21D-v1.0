/*
FreeBSD License

Copyright (c) 2019,2020 vikonix: valeriy.kovalev.software@gmail.com
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice, this
   list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR
ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND
ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/


#include "screen.h"


//////////////////////////////////////////////////////////////////////////////
// цифровой 6x8 моно
#define BIG_COLON   0xa
#define BIG_COLON1  0xb
#define SPACE       0xc
const byte font_digit_6x8[] PROGMEM = 
{
  0x7e, 0xff, 0x81, 0x81, 0xff, 0x7e,  //'0' 0
  0x00, 0x82, 0xff, 0xff, 0x80, 0x00,  //'1' 1 
  0x82, 0xc1, 0xa1, 0x91, 0xcf, 0xc6,  //'2' 2 
  0x42, 0xc1, 0x89, 0x89, 0xff, 0x76,  //'3' 3
  0x38, 0x24, 0xa2, 0xff, 0xff, 0xa0,  //'4' 4
  0x4f, 0xcf, 0x89, 0x89, 0xf9, 0x71,  //'5' 5
  0x7c, 0xfe, 0x8b, 0x89, 0xf9, 0x70,  //'6' 6
  0x01, 0x81, 0xf1, 0xf9, 0x8f, 0x07,  //'7' 7
  0x76, 0xff, 0x89, 0x89, 0xff, 0x76,  //'8' 8
  0x0e, 0x9f, 0x91, 0xd1, 0x7f, 0x3e,  //'9' 9
  0x62, 0x62, 0x00, 0x00, 0x00, 0x00,  //':' 0xa
  0x46, 0x46, 0x00, 0x00, 0x00, 0x00,   //':' 0xb
  0, 0, 0, 0, 0, 0  //пробел 0x0с
};

// цифровой 4x5 моно
const byte font_digit_4x5[] PROGMEM = 
{
        0, 124, 68, 68, 124, 0,   // 0x30, 0
        0, 0, 8, 124, 0, 0,   // 0x31, 1
        0, 116, 84, 84, 92, 0,   // 0x32, 2
        0, 68, 84, 84, 124, 0,   // 0x33, 3
        0, 48, 40, 36, 124, 0,   // 0x34, 4
        0, 92, 84, 84, 116, 0,   // 0x35, 5
        0, 124, 84, 84, 116, 0,   // 0x36, 6
        0, 4, 4, 4, 124, 0,   // 0x37, 7
        0, 124, 84, 84, 124, 0,   // 0x38, 8
        0, 92, 84, 84, 124, 0,    // 0x39, 9
};
     
// цифровой 6x5 мини
const byte font_digit_6x5_mini[] PROGMEM = 
{
        124, 124, 68, 68, 124, 124,   // 0x30, 0
        0, 0, 8, 124, 124, 0,   // 0x31, 1
        116, 116, 84, 84, 92, 92,   // 0x32, 2
        68, 84, 84, 84, 124, 124,   // 0x33, 3
        // 32, 48, 40, 124, 124, 0,   // 0x34, 4
        32, 48, 40, 124, 124, 32, // 0x34, 4
        92, 92, 84, 84, 116, 116,   // 0x35, 5
        124, 124, 84, 84, 116, 116,   // 0x36, 6
        4, 4, 4, 4, 124, 124,   // 0x37, 7
        124, 124, 84, 84, 124, 124,   // 0x38, 8
        92, 92, 84, 84, 124, 124,    // 0x39, 9
        36, 36, 0, 0, 0, 0, // Двоеточие
        0, 0, 0, 0, 0, 0, // Пробел
};

// цифровой 6x6 мини
const byte font_digit_6x6_mini[] PROGMEM = 
{
        126, 126, 66, 66, 126, 126, 	// 0
        0, 4, 4, 126, 126, 0, 	// 1
        122, 122, 74, 82, 94, 94, 	// 2
        66, 74, 74, 74, 126, 118, 	// 3
        48, 56, 36, 34, 126, 126, 	// 4
        94, 94, 82, 74, 122, 122, 	// 5
        126, 126, 82, 74, 122, 122, 	// 6
        2, 2, 114, 122, 14, 6, 	// 7
        // 126, 126, 74, 74, 126, 126, 	// 8
        118, 126, 74, 74, 126, 118, //8
        94, 94, 82, 74, 126, 126, 	// 9
        36, 36, 0, 0, 0, 0, // Двоеточие
        0, 0, 0, 0, 0, 0, // Пробел
}; 

// цифровой 6x7 мини
const byte font_digit_6x7[] PROGMEM = 
{
  127, 127, 65, 65, 127, 127, 	// 0
	0, 2, 2, 127, 127, 0, 	// 1
	121, 121, 73, 65, 79, 79, 	// 2
	65, 65, 73, 73, 127, 119, 	// 3
	48, 56, 36, 34, 127, 127, 	// 4
	79, 79, 65, 73, 121, 121, 	// 5
	127, 127, 65, 73, 121, 121, 	// 6
	1, 1, 121, 125, 7, 3, 	// 7
	119, 127, 73, 73, 127, 119, 	// 8
	79, 79, 73, 65, 127, 127, 	// 9
	36, 36, 0, 0, 0, 0, 	// Двоеточие
  0, 0, 0, 0, 0, 0 // Пробел
};

// цифровой 6x8 
const byte font_digit_6x8m[] PROGMEM = 
{
  255, 255, 129, 129, 255, 255, 	// 0
	0, 4, 6, 255, 255, 0, 	// 1
	249, 249, 137, 145, 159, 159, 	// 2
	129, 137, 137, 137, 255, 247, 	// 3
	48, 56, 36, 34, 255, 255, 	// 4
	159, 159, 129, 137, 249, 249, 	// 5
	255, 255, 129, 137, 249, 249, 	// 6
	1, 1, 249, 253, 7, 3, 	// 7
	247, 255, 137, 137, 255, 247, 	// 8
	159, 159, 145, 129, 255, 255, 	// 9
	36, 36, 0, 0, 0, 0, 	// Двоеточие
  0, 0, 0, 0, 0, 0 // Пробел
};


// мелкий шрифт с моей небольшой модификацией
// Модификации Tom Thumb для улучшения читаемости предоставлены Роби Пойнтером,
// см.: http://robey.lag.net/2010/01/23/tiny-monospace-font.html.
#define MIN_TINY  0x1e
#define MAX_TINY  0x7f
const unsigned short PROGMEM font_3x5[] = {
  //пользовательские символы
  0b0000100000000000, //0x1e 'точка вверху'
  0b0000000100000000, //0x1f 'точка посередине'
  //стандартные символы
  0b0000000000000000, //' '
  0b0000010111000000, //'!'
  0b0000100000000010, //'"'
  0b1111101010111110, //'#'
  0b0101011111001010, //'$'
  0b0100100100100100, //'%'
  0b0101010101110100, //'&'
  0b0000000011000000, //'''
  0b0000001110100010, //'('
  0b1000101110000000, //')'
  0b0101000100010100, //'*'
  0b0010001110001000, //'+'
  0b1000001100000001, //','
  0b0010000100001000, //'-'
  0b0000010000000000, //'.'
  0b1100000100000110, //'/'
#if 0
  //округленные цифры
  0b1111010001011110, //'0'
  0b0001011111000000, //'1'
  0b1100110101100100, //'2'
  0b1000110101010100, //'3'
  0b0011100100111110, //'4'
  0b1011110101010010, //'5'
  0b1111010101111010, //'6'
  0b1100100101000110, //'7'
  0b1111110101111110, //'8'
  0b1011110101011110, //'9'
#else
  //квадратные цифры
  0b1111110001111110, //'0'
  0b0001011111000000, //'1'
  0b1110110101101110, //'2'
  0b1000110101111110, //'3'
  0b0011100100111110, //'4'
  0b1011110101111010, //'5'
  0b1111110101111010, //'6'
  0b0000100001111110, //'7'
  0b1111110101111110, //'8'
  0b1011110101111110, //'9'
#endif
  0b0000001010000000, //':'
  0b1000001010000000, //';'
  0b0010001010100010, //'<'
  0b0101001010010100, //'='
  0b1000101010001000, //'>'
  0b0000110101000110, //'?'
  0b0111010101101100, //'@'
  0b1111000101111100, //'A'
  0b1111110101010100, //'B'
  0b0111010001100010, //'C'
  0b1111110001011100, //'D'
  0b1111110101101010, //'E'
  0b1111100101001010, //'F'
  0b0111010101111010, //'G'
  0b1111100100111110, //'H'
  0b1000111111100010, //'I'
  0b0100010000011110, //'J'
  0b1111100100110110, //'K'
  0b1111110000100000, //'L'
  0b1111100110111110, //'M'
  0b1111101110111110, //'N'
  0b0111010001011100, //'O'
  0b1111100101000100, //'P'
  0b0111010001111100, //'Q'
  0b1111101101101100, //'R'
  0b1001010101010010, //'S'
  0b0000111111000010, //'T'
  0b0111110000111110, //'U'
  0b0011111000001110, //'V'
  0b1111101100111110, //'W'
  0b1101100100110110, //'X'
  0b0001111100000110, //'Y'
  0b1100110101100110, //'Z'
  0b0000011111100010, //'['
  0b0001100100110000, //'\'
  0b1000111111000000, //']'
  0b0001000001000100, //'^'
  0b1000010000100000, //'_'
  0b0000100010000000, //'`'
  0b1101010110111000, //'a'
  0b1111110010011000, //'b'
  0b0110010010100100, //'c'
  0b0110010010111110, //'d'
  0b0110011010101100, //'e'
  0b0010011110001010, //'f'
  0b0011010101011111, //'g'
  0b1111100010111000, //'h'
  0b0000011101000000, //'i'
  0b1000001101000000, //'j'
  0b1111101100100100, //'k'
  0b1000111111100000, //'l'
  0b1111001110111000, //'m'
  0b1111000010111000, //'n'
  0b0110010010011000, //'o'
  0b1111101001001101, //'p'
  0b0011001001111111, //'q'
  0b1110000010000100, //'r'
  0b1010011110010100, //'s'
  0b0001011111100100, //'t'
  0b0111010000111100, //'u'
  0b0111011000011100, //'v'
  0b0111011100111100, //'w'
  0b1001001100100100, //'x'
  0b0001110100011111, //'y'
  0b1101011110101100, //'z'
  0b0010011011100010, //'{'
  0b0000011011000000, //'|'
  0b1000111011001000, //'}'
  0b0001000011000010, //'~'
  0b1111111111111110  //other symbols
};

extern byte dots;
//////////////////////////////////////////////////////////////////////////////
byte GetColumnMask(int size)
{
  switch(size)
  {
    case 1:
      return B10000000;
    case 2:
      return B11000000;
    case 3:
      return B11100000;
    case 4:
      return B11110000;
    case 5:
      return B11111000;
    case 6:
      return B11111100;
    case 7:
      return B11111110;
    case 8:
      return B11111111;
  }

  return B11111111;
}


//////////////////////////////////////////////////////////////////////////////
void CopySymbol(byte* pBuffer, const byte* pFont, int Symbol, int x, int y, int fontx, int fonty, int sizex = 0)
{
  byte mask = GetColumnMask(fonty);

  if(sizex == 0)
    sizex = fontx;

  for(int i = 0; i < sizex; ++i)
  {
    byte column = pgm_read_byte(pFont + Symbol * fontx + i);
    pBuffer[x + i] = (pBuffer[x + i] & ~(mask << y)) | (column << y);
  }
}

//////////////////////////////////////////////////////////////////////////////
int CopyTinySymbol(byte* pBuffer, int Symbol, int x, int y, bool fixed)
{
  byte mask = B11111100;
  unsigned short s3 = pgm_read_word(font_3x5 + Symbol);
  unsigned short descender = s3 & 0x01;

  byte column1 = (byte)(((s3 >> 11) & 0b00011111) << descender);
  byte column2 = (byte)(((s3 >>  6) & 0b00011111) << descender);
  byte column3 = (byte)(((s3 >>  1) & 0b00011111) << descender);

  int n = 0;
  if(fixed || column1 != 0)
  {
    pBuffer[x+n] = (pBuffer[x+n] & ~(mask << y)) | (column1 << y);
    ++n;
  }
  if(fixed || column2 != 0 || (column1 != 0 && column3 != 0))
  {
    pBuffer[x+n] = (pBuffer[x+n] & ~(mask << y)) | (column2 << y);
    ++n;
  }
  if(fixed || column3 != 0)
  {
    pBuffer[x+n] = (pBuffer[x+n] & ~(mask << y)) | (column3 << y);
    ++n;
  }
  return n + 1;
}

//////////////////////////////////////////////////////////////////////////////
void ScrollVerticalOneRow(byte* buffer, byte from, byte to, boolean fUp)
{
  for(byte n = from; n <= to; ++n)
  {
    if(fUp)
    {
      screen_buffer[n] >>= 1;
      bitWrite(screen_buffer[n], 7, bitRead(buffer[n], 0));
      buffer[n] >>= 1;
    }
    else
    {
      screen_buffer[n] <<= 1;
      bitWrite(screen_buffer[n], 0, bitRead(buffer[n], 7));
      buffer[n] <<= 1;
    }
  }
}


//////////////////////////////////////////////////////////////////////////////
void ScrollVertical(byte* buffer, byte from, byte to, boolean fUp)
{
  for(byte i = 0; i < 8; i++)
  {
    ScrollVerticalOneRow(buffer, from, to, fUp);
    ShowBuffer();
    delay(50);
  }
}


//////////////////////////////////////////////////////////////////////////////
//позиции цифр часов
// #define Normal_font_hour_minute
// #define Normal_font_second
// #define Little_Font_Secs
// #define Little6x5_Font_Secs
// #define Little6x6_Font_Secs
#define All_Little6x6_Font

#ifdef  All_Little6x6_Font
#define POS_DIGIT1 1                //0 hour10
#define POS_DIGIT2 (POS_DIGIT1 + 7) //7 hour1
#define POS_COLON  (POS_DIGIT2 + 7) //14
#define POS_DIGIT3 (POS_COLON  + 3) //17 min10
#define POS_DIGIT4 (POS_DIGIT3 + 7) //24 min1
#define POS_DIGIT5 (POS_DIGIT4 + 9) //34 sec10
#define POS_DIGIT6 (POS_DIGIT5 + 7) //41 sec1 
#endif

#ifdef Little6x6_Font_Secs
#define POS_DIGIT5 (POS_DIGIT4 + 7) //34 sec10
#define POS_DIGIT6 (POS_DIGIT5 + 7) //41 sec1 
#endif

#ifdef Little6x5_Font_Secs
#define POS_DIGIT5 (POS_DIGIT4 + 7) //34 sec10
#define POS_DIGIT6 (POS_DIGIT5 + 7) //41 sec1 
#endif

#ifdef Little_Font_Secs
#define POS_DIGIT5 (POS_DIGIT4 + 7) //34 sec10
#define POS_DIGIT6 (POS_DIGIT5 + 5) //41 sec1 
#endif

#ifdef Normal_font_hour_minute
#define POS_DIGIT1 1                //0 hour10
#define POS_DIGIT2 (POS_DIGIT1 + 7) //7 hour1
#define POS_COLON  (POS_DIGIT2 + 7) //14
#define POS_DIGIT3 (POS_COLON  + 3) //17 min10
#define POS_DIGIT4 (POS_DIGIT3 + 7) //24 min1
#endif

#ifdef Normal_font_second
#define POS_COLON1 (POS_DIGIT4 + 7) //31
#define POS_DIGIT5 (POS_COLON1 + 3) //34 sec10
#define POS_DIGIT6 (POS_DIGIT5 + 7) //41 sec1
#endif

void DisplayTime(int hours, int mins, int secs, bool alarm, byte scroll_mode, boolean fUp)
{
  byte sprite_buffer[LEDMATRIX_WIDTH];
  memset(sprite_buffer, 0, LEDMATRIX_WIDTH);

//  if(hours / 10)
#ifdef All_Little6x6_Font
  CopySymbol(sprite_buffer, font_digit_6x7, hours / 10, POS_DIGIT1, 0, 6, 8);
  CopySymbol(sprite_buffer, font_digit_6x7, hours % 10, POS_DIGIT2, 0, 6, 8);
  CopySymbol(sprite_buffer, font_digit_6x7, BIG_COLON + (dots & 1), POS_COLON, 0, 6, 8, 2);// + (dots & 1)
  CopySymbol(sprite_buffer, font_digit_6x7, mins / 10, POS_DIGIT3, 0, 6, 8);
  CopySymbol(sprite_buffer, font_digit_6x7, mins % 10, POS_DIGIT4, 0, 6, 8);  
  CopySymbol(sprite_buffer, font_digit_6x6_mini, secs / 10, POS_DIGIT5, 0, 6, 8);
  CopySymbol(sprite_buffer, font_digit_6x6_mini, secs % 10, POS_DIGIT6, 0, 6, 8);
#endif 
// #ifdef Normal_font 

// #endif
#ifdef Little6x6_Font_Secs  
  CopySymbol(sprite_buffer, font_digit_6x6_mini, secs / 10, POS_DIGIT5, 0, 6, 8);
  CopySymbol(sprite_buffer, font_digit_6x6_mini, secs % 10, POS_DIGIT6, 0, 6, 8);
#endif  
#ifdef Little6x5_Font_Secs  
  CopySymbol(sprite_buffer, font_digit_6x5_mini, secs / 10, POS_DIGIT5, 0, 6, 8);
  CopySymbol(sprite_buffer, font_digit_6x5_mini, secs % 10, POS_DIGIT6, 0, 6, 8);
#endif  
#ifdef Little_Font_Secs 
  CopySymbol(sprite_buffer, font_digit_4x5, secs / 10, POS_DIGIT5, 0, 6, 8);
  CopySymbol(sprite_buffer, font_digit_4x5, secs % 10, POS_DIGIT6, 0, 6, 8);
#endif  
#ifdef Normal_font_hour_minute
  CopySymbol(sprite_buffer, font_digit_6x8, hours / 10, POS_DIGIT1, 0, 6, 8);
  CopySymbol(sprite_buffer, font_digit_6x8, hours % 10, POS_DIGIT2, 0, 6, 8);
  CopySymbol(sprite_buffer, font_digit_6x8, BIG_COLON + (secs & 1), POS_COLON, 0, 6, 8, 2);
  CopySymbol(sprite_buffer, font_digit_6x8, mins / 10, POS_DIGIT3, 0, 6, 8);
  CopySymbol(sprite_buffer, font_digit_6x8, mins % 10, POS_DIGIT4, 0, 6, 8);
#endif  
#ifdef Normal_font_second  
  CopySymbol(sprite_buffer, font_digit_6x8, SPACE, POS_COLON1, 0, 6, 8, 2);
  CopySymbol(sprite_buffer, font_digit_6x8, secs / 10, POS_DIGIT5, 0, 6, 8);
  CopySymbol(sprite_buffer, font_digit_6x8, secs % 10, POS_DIGIT6, 0, 6, 8);
#endif  
  if(alarm)
  {
    sprite_buffer[LEDMATRIX_WIDTH - 1] = 0b10000000;
  }

  switch(scroll_mode)
  {
    case CHANGED_ALL:
    // default:
      ShowBuffer(sprite_buffer);
      // ScrollVertical(sprite_buffer, POS_DIGIT6, POS_DIGIT6+5, fUp);
      // static int cikl = 0;
      // cikl++;
      // if(cikl > 100)
      // {
      //   // tochka();
      //   dots = !dots; 
      //   cikl = 0;
      // } 
      break;

    case CHANGED_SEC1:
      ScrollVertical(sprite_buffer, POS_DIGIT6, POS_DIGIT6+5, fUp);
      break;
    case CHANGED_SEC10:
      ScrollVertical(sprite_buffer, POS_DIGIT6, POS_DIGIT6+5, fUp);
      ScrollVertical(sprite_buffer, POS_DIGIT5, POS_DIGIT5+5, fUp);
      break;
    case CHANGED_MIN1:
      ScrollVertical(sprite_buffer, POS_DIGIT6, POS_DIGIT6+5, fUp);
      ScrollVertical(sprite_buffer, POS_DIGIT5, POS_DIGIT5+5, fUp);
      ScrollVertical(sprite_buffer, POS_DIGIT4, POS_DIGIT4+5, fUp);
      break;
    case CHANGED_MIN10:
      ScrollVertical(sprite_buffer, POS_DIGIT6, POS_DIGIT6+5, fUp);
      ScrollVertical(sprite_buffer, POS_DIGIT5, POS_DIGIT5+5, fUp);
      ScrollVertical(sprite_buffer, POS_DIGIT4, POS_DIGIT4+5, fUp);
      ScrollVertical(sprite_buffer, POS_DIGIT3, POS_DIGIT3+5, fUp);
      break;
    case CHANGED_HOUR1:
      ScrollVertical(sprite_buffer, POS_DIGIT6, POS_DIGIT6+5, fUp);
      ScrollVertical(sprite_buffer, POS_DIGIT5, POS_DIGIT5+5, fUp);
      ScrollVertical(sprite_buffer, POS_DIGIT4, POS_DIGIT4+5, fUp);
      ScrollVertical(sprite_buffer, POS_DIGIT3, POS_DIGIT3+5, fUp);
      ScrollVertical(sprite_buffer, POS_DIGIT2, POS_DIGIT2+5, fUp);
      break;
    case CHANGED_HOUR10:
      ScrollVertical(sprite_buffer, POS_DIGIT6, POS_DIGIT6+5, fUp);
      ScrollVertical(sprite_buffer, POS_DIGIT5, POS_DIGIT5+5, fUp);
      ScrollVertical(sprite_buffer, POS_DIGIT4, POS_DIGIT4+5, fUp);
      ScrollVertical(sprite_buffer, POS_DIGIT3, POS_DIGIT3+5, fUp);
      ScrollVertical(sprite_buffer, POS_DIGIT2, POS_DIGIT2+5, fUp);
      ScrollVertical(sprite_buffer, POS_DIGIT1, POS_DIGIT1+5, fUp);
      break;
    case SCROLL_ALL:
      ScrollVertical(sprite_buffer, 0, LEDMATRIX_WIDTH, fUp);
      break;
  }
}


//////////////////////////////////////////////////////////////////////////////
void PrintTinyString(const char* str, int x, int y, bool fixed)
{
  byte sprite_buffer[LEDMATRIX_WIDTH+3];
  memset(sprite_buffer, 0, sizeof(sprite_buffer));

  char c;
  while((c = *str++) != 0 && (unsigned int)x < sizeof(sprite_buffer))
  {
    if(c < MIN_TINY || c > MAX_TINY)
      c = MAX_TINY;
    c -= MIN_TINY;
    x += CopyTinySymbol(sprite_buffer, c, x, y, fixed);
  }
  memcpy(screen_buffer, sprite_buffer, LEDMATRIX_WIDTH);
}


/////////////////////////////////////////////////////////////////////////////
void InverseBlock(int x, int y, int sizex, int sizey)
{
  byte mask = GetColumnMask(sizey) >> (8 - y - sizey);
  for(int i = 0; i < sizex; ++i)
  {
    screen_buffer[x + i] ^= mask;
  }
}


/////////////////////////////////////////////////////////////////////////////
void PrintPictogram(int x, const byte* buff, int size)
{
  memcpy(screen_buffer + x, buff, size);
}

