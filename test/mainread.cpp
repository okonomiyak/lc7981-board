#include <U8g2lib.h>
#include "wakayama_bmp.h"
 
U8G2_LC7981_240X64_F_6800 u8g2(U8G2_R0,
  /* d0-d7 = */ 13, 14, 27, 26, 25, 33, 32, 19,
  /* enable=*/ 5,
  /* cs=   */ 17,
  /* dc=   */ 18,
  /* reset=*/ 16);
 
void setup() {
  u8g2.begin();
}
 
void loop() {
  // フレーム1:種別(紀州路快速)
  u8g2.clearBuffer();
  u8g2.drawXBMP(0, 0, 240, 64, wakayama_bmp);
  u8g2.sendBuffer();
  delay(2000);
}
 