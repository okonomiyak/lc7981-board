#include <U8g2lib.h>
#include <string.h>
#include "timetable.h"

U8G2_LC7981_240X64_F_6800 u8g2(U8G2_R0,
  /* d0-d7 = */ 13, 14, 27, 26, 25, 33, 32, 19,
  /* enable=*/ 5,
  /* cs=   */ 17,
  /* dc=   */ 18,
  /* reset=*/ 16);

// ===== 仮想時計(Wi-Fi/NTPなし。3秒で1分進む) =====
int virtualMinutes = 5 * 60 + 20; // 始発前から開始
unsigned long lastTick = 0;
const unsigned long tickIntervalMs = 3000; // 3秒 = 1分

void getNextTwo(int nowMinutes, int& i1, int& i2, int& m1, int& m2) {
  int bestDiff1 = 100000, bestDiff2 = 100000;
  i1 = -1; i2 = -1;
  for (int i = 0; i < numTrains; i++) {
    int dept = timetable[i].hour * 60 + timetable[i].minute;
    int diff = dept - nowMinutes;
    if (diff < 0) diff += 24 * 60; // 日をまたいだら翌日扱い
    if (diff < bestDiff1) {
      bestDiff2 = bestDiff1; i2 = i1;
      bestDiff1 = diff; i1 = i;
    } else if (diff < bestDiff2) {
      bestDiff2 = diff; i2 = i;
    }
  }
  m1 = bestDiff1;
  m2 = bestDiff2;
}

// 種別から両数を判定: 特急・洛楽・快急は8両、それ以外は7両
int getCars(const char* type) {
  if (strcmp(type, "特急") == 0 || strcmp(type, "洛楽") == 0 || strcmp(type, "快急") == 0) {
    return 8;
  }
  return 7;
}

// Kintetsu/京阪風デザイン案を単色240x64に翻訳したもの。
// 種別バッジは元デザインの「色付き背景+白文字」を、白反転バッジ(黒文字)として再現。
// 発順ラベル(先発/次発)は上下の行位置そのものが順番を表すので省略。
void drawRow(int y, const Train& t) {
  // 種別バッジ(反転: 白背景+黒文字)
  u8g2.setDrawColor(1);
  u8g2.drawBox(0, y, 54, 20);
  u8g2.setDrawColor(0);
  u8g2.drawUTF8(3, y + 16, t.type);
  u8g2.setDrawColor(1);

  // 時刻
  char timeBuf[8];
  snprintf(timeBuf, sizeof(timeBuf), "%d:%02d", t.hour, t.minute);
  u8g2.drawUTF8(60, y + 16, timeBuf);

  // 行先
  u8g2.drawUTF8(112, y + 16, t.dest);

  // 両数
  char carsBuf[8];
  snprintf(carsBuf, sizeof(carsBuf), "%d両", getCars(t.type));
  u8g2.drawUTF8(202, y + 16, carsBuf);
}

void setup() {
  u8g2.begin();
  u8g2.setFont(u8g2_font_unifont_t_japanese3);
  lastTick = millis();
}

void loop() {
  // 3秒ごとに仮想時計を1分進める
  if (millis() - lastTick >= tickIntervalMs) {
    lastTick += tickIntervalMs;
    virtualMinutes = (virtualMinutes + 1) % (24 * 60);
  }

  int i1, i2, m1, m2;
  getNextTwo(virtualMinutes, i1, i2, m1, m2);

  u8g2.clearBuffer();
  if (i1 >= 0) drawRow(2, timetable[i1]);
  u8g2.drawHLine(0, 32, 240);
  if (i2 >= 0) drawRow(34, timetable[i2]);
  u8g2.sendBuffer();

  delay(200);
}