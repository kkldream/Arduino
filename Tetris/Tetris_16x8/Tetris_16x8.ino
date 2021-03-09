#include "LedControl.h" //MAX7219函式庫
LedControl lc = LedControl(13, 11, 12, 3);  //MAX7219腳位 = (DIN, CLK, CS, 連接螢幕數量)
const byte SW_Pin[] = {A0, A1, A2, A3, A4}, RGB_Pin[] = {10, 9, 6}, tone_Pin[] = {5, 7};  //按鍵腳位, LED腳位, 蜂鳴器腳位
const byte num[] = {  //七段數字解碼
  0B01111110,
  0B00110000,
  0B01101101,
  0B01111001,
  0B00110011,
  0B01011011,
  0B01011111,
  0B01110000,
  0B01111111,
  0B01110011
};
bool Gameover, Pause;
bool lcd[19][8]; //螢幕畫面暫存，lcd[18][0]就是畫面左下角，lcd[18][7]就是畫面右下角，lcd[3][0]就是畫面左上角，lcd[3][7]就是畫面右上角
bool music_oe = 1, bee_oe = 1;  //音樂、音效開關
byte y, mode, Score, Tetris;
byte tetris_tone[38] = {  //音樂旋律
  6, 2, 3, 4, 3, 2, 1, 1, 3, 5, 4, 3, 2, 2, 3, 4, 5, 3, 1, 1,
  5, 6, 7, 6, 5, 4, 3, 4, 3, 2, 1, 1, 2, 3, 4, 2, 0, 0
};
byte tetris_beat[38] = {  //音樂節奏
  4, 2, 2, 4, 2, 2, 4, 2, 2, 4, 2, 2, 4, 2, 2, 4, 4, 4, 4, 8,
  6, 2, 4, 2, 2, 6, 2, 4, 2, 2, 4, 2, 2, 4, 4, 4, 4, 8
};
int tone_table[14] = {262, 294, 330, 349, 392, 440, 494, 523, 587, 659, 698, 784, 880, 988};  //常用的音高頻率
int x, speed_time = 1000;
void setup() {
  randomSeed(analogRead(A7)); //設定亂數種子
  random(7);  //避免第一個亂數重複，先使用掉
  for (byte a = 0; a < 3; a++) {
    lc.shutdown(a, 0);  //開啟螢幕
    lc.clearDisplay(a); //清除畫面
    lc.setIntensity(a, 15); //螢幕亮度調整
  }
  lc.setRow(2, 0, num[0]);  //七段個位顯示0
  lc.setRow(2, 1, num[0]);  //七段十位顯示0
  for (byte a = 0; a < 5; a++) {
    pinMode(SW_Pin[a], OUTPUT);
    digitalWrite(SW_Pin[a], LOW);
  }
  for (byte a = 0; a < 3; a++) {
    pinMode(RGB_Pin[a], OUTPUT);
    digitalWrite(RGB_Pin[a], HIGH);
  }
  pinMode(tone_Pin[0], OUTPUT);
  re(); //開始第一個方塊
}

void loop() {
  unsigned long Old_millis = millis();
  while (millis() - Old_millis < speed_time || Pause) { //控制下降間隔時間並同時執行其他函式
    lcd_dis();  //螢幕刷新
    SW(); //按鍵偵測
    music();  //音樂播放
    RGB();  //呼吸燈動作
    Bee(0, 0, 0); //音效靜音(防止雜音)
  }
  if (down()) { //執行下降的動作並判斷如果停止下降就執行下列動作
    for (byte a = 0; a < 8; a++) if (lcd[2][a]) Gameover = 1; //判斷方塊是否超出上緣(遊戲結束)
    if (Gameover) { //如果遊戲結束執行下列動作
      random_7(1);  //重置方塊順序
      //遊戲結束動畫
      for (byte b = 0; b < 8; b++) {
        for (byte a = 0; a < 8; a++) {
          lcd[2 * a + 3][b] = 1;
          lcd[2 * a + 4][7 - b] = 1;
        }
        lcd_dis();
        loop_delay(100);
      }
      for (byte b = 0; b < 8; b++) {
        for (byte a = 0; a < 8; a++) {
          lcd[2 * a + 3][b] = 0;
          lcd[2 * a + 4][7 - b] = 0;
        }
        for (byte b = 0; b < 3; b++) for (byte a = 0; a < 8; a++) lcd[b][a] = 0;
        lcd_dis();
        loop_delay(100);
      }
      //----------------
      Score = 0;  //重置遊戲分數
    }
    else {  //如果遊戲沒有結束執行下列動作
      bool clr[16] = {1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1};
      for (byte b = 0; b < 16; b++) for (byte a = 0; a < 8; a++) if (!lcd[b + 3][a]) clr[b] = 0;  //判斷哪些橫行需被清除
      //消行動畫
      for (byte b = 0; b < 16; b++) for (byte a = 0; a < 8; a++) if (clr[b]) lcd[b + 3][a] = 0;
      lcd_dis();
      if (bee_oe) Bee(1, 1000, 200);
      loop_delay(100);
      for (byte b = 0; b < 16; b++) for (byte a = 0; a < 8; a++) if (clr[b]) lcd[b + 3][a] = 1;
      lcd_dis();
      if (bee_oe) Bee(1, 1000, 200);
      loop_delay(100);
      for (byte b = 0; b < 16; b++) for (byte a = 0; a < 8; a++) if (clr[b]) lcd[b + 3][a] = 0;
      lcd_dis();
      //----------
      byte Score_var; //此變數為清除的行數
      //計算清除橫行並將清除的空間填入
      for (byte b = 3; b < 19; b++) {
        if (clr[b - 3]) {
          for (byte c = 0; c < b - 2; c++) for (byte a = 0; a < 8; a++) lcd[b - c][a] = lcd[b - 1 - c][a];  //
          Score_var++;
        }
      }
      //--------------------------------
      if (Score_var == 4) Score += 6; //"Tetris"加6分
      else if (Score_var == 3) Score += 5;  //"Triple"加5分
      else Score += Score_var;  //其餘都加1分
    }
    score_dis(Score); //分數畫面刷新
    re(); //呼叫下一個方塊
  }
}
//按鍵偵測函式
unsigned long down_millis, left_right_millis, rt_millis;
bool left_right_press, up_press, down_press, rt_press, score_press;
void SW() {
  if (digitalRead(SW_Pin[4])) {
    if (!Pause)score_press = 0;
    Pause = 1;
    if (!digitalRead(SW_Pin[0])) up_press = 0;
    else if (up_press == 0) {
      score_press = 1;
      music_oe = music_oe == 0 ? 1 : 0;
      up_press = 1;
    }
    if (!digitalRead(SW_Pin[3])) down_press = 0;
    else if (down_press == 0) {
      score_press = 1;
      bee_oe = bee_oe == 0 ? 1 : 0;
      down_press = 1;
    }
    if (!digitalRead(SW_Pin[1]) || !digitalRead(SW_Pin[2])) {
      if (digitalRead(SW_Pin[1]) && Score != 99) {
        score_press = 1;
        if (digitalRead(SW_Pin[1]) &&  long(millis() - rt_millis) > 50) {
          score_dis(++Score);
          if (!rt_press) rt_millis = millis() + 100;
          else rt_millis = millis();
          rt_press = 1;
        }
        else if (!digitalRead(SW_Pin[1]))rt_press = 0;
      }
      else if (digitalRead(SW_Pin[2]) && Score != 0) {
        score_press = 1;
        if (digitalRead(SW_Pin[2]) &&  long(millis() - rt_millis) > 50) {
          score_dis(--Score);
          if (!rt_press) rt_millis = millis() + 100;
          else rt_millis = millis();
          rt_press = 1;
        }
        else if (!digitalRead(SW_Pin[2]))rt_press = 0;
      }
    }
  }
  else {
    if (Pause && !score_press) {
      Gameover = 1;
      speed_time = 0;
    }
    Pause = 0;
    if (digitalRead(SW_Pin[2]) && millis() - down_millis > 50) {
      down();
      down_millis = millis();
    }
    if (!digitalRead(SW_Pin[0]) || !digitalRead(SW_Pin[3])) {
      if (digitalRead(SW_Pin[3])) {
        if (digitalRead(SW_Pin[3]) &&  long(millis() - left_right_millis) > 50) {
          right();
          if (!left_right_press) left_right_millis = millis() + 100;
          else left_right_millis = millis();
          left_right_press = 1;
        }
        else if (!digitalRead(SW_Pin[3]))left_right_press = 0;
      }
      else {
        if (digitalRead(SW_Pin[0]) &&  long(millis() - left_right_millis) > 50) {
          left();
          if (!left_right_press) left_right_millis = millis() + 100;
          else left_right_millis = millis();
          left_right_press = 1;
        }
        else if (!digitalRead(SW_Pin[0]))left_right_press = 0;
      }
    }
    if (!digitalRead(SW_Pin[1])) up_press = 0;
    else if (up_press == 0) {
      up();
      up_press = 1;
    }
  }
}
//延遲函式，並讓必要的函式持續執行
void loop_delay(int delay_time) {
  unsigned long time_var = millis();
  while (millis() - time_var <= delay_time) {
    music();
    RGB();
    Bee(0, 0, 0);
  }
}
//音樂播放函式，用函式tone(腳位, 頻率, 持續時間)來寫
unsigned long beat_millis;
byte toneNum, overtoneNum;
void music() {
  if (millis() - beat_millis >= 200 && Gameover && music_oe) {
    beat_millis = millis();
    tone(tone_Pin[1], 100, 100);
    toneNum = 38;
  }
  else if (millis() - beat_millis >= tetris_beat[toneNum - 1] * 120 && music_oe) {
    beat_millis = millis();
    tone(tone_Pin[1], tone_table[tetris_tone[toneNum] % 10], tetris_beat[toneNum] * 100);
    toneNum = toneNum == 38 ? 0 : ++toneNum;
  }
}
//音效播放函式，因為1顆atmega328p只能用一個tone()，所以音效只能用digitalWrite()硬寫
unsigned long Bee_start_millis, Bee_cycle_micros;
bool Bee_status;
int delay_time, duration;
void Bee(bool start, int a, int b) {
  if (start) {
    Bee_start_millis = millis();
    delay_time = a;
    duration = b;
  }
  else {
    if (micros() - Bee_cycle_micros >= delay_time && millis() - Bee_start_millis <= duration) {
      Bee_status = Bee_status == 0 ? 1 : 0;
      digitalWrite(tone_Pin[0], Bee_status);
      Bee_cycle_micros = micros();
    }
    else digitalWrite(tone_Pin[0], LOW);
  }
}
//依照模式變換呼吸燈顏色
unsigned long RGB_millis;
byte RGB_color, RGB_brightness = 250, RGB_var = -1;
void RGB() {
  if (Gameover) { //若Gameover亮紅燈
    RGB_color = 0;
    digitalWrite(RGB_Pin[1], HIGH); //LED是共陽，所以給HIGH是暗，給LOW是亮
    digitalWrite(RGB_Pin[2], HIGH);
  }
  else if (Pause) { //若按下功能鍵亮藍燈
    RGB_color = 2;
    digitalWrite(RGB_Pin[0], HIGH);
    digitalWrite(RGB_Pin[1], HIGH);
  }
  else {  //遊戲中亮綠燈
    RGB_color = 1;
    digitalWrite(RGB_Pin[0], HIGH);
    digitalWrite(RGB_Pin[2], HIGH);
  }
  if (millis() - RGB_millis >= 20) {  //呼吸燈動作
    RGB_millis = millis();
    if (RGB_brightness == 225) RGB_var *= -1;
    else if (RGB_brightness == 255) RGB_var *= -1;
    RGB_brightness += RGB_var;
    analogWrite(RGB_Pin[RGB_color], RGB_brightness);
  }
}
//把lcd[][]變數解碼給MAX7219做顯示
void lcd_dis() {
  for (byte b = 0; b < 8; b++) {
    byte c = 0; bool d = 0;
    for (byte a = 0; a < 8; a++) if (lcd[a + 3][7 - b]) c |= 0B10000000 >> a;
    if (c & 0B00000001) d = 1;
    c >>= 1;
    if (d) c |= 0B10000000;
    lc.setRow(0, b, c);
    c = 0; d = 0;
    for (byte a = 0; a < 8; a++) if (lcd[a + 11][7 - b]) c |= 0B10000000 >> a;
    if (c & 0B00000001) d = 1;
    c >>= 1;
    if (d) c |= 0B10000000;
    lc.setRow(1, b, c);
  }
}
//七段顯示分數並控制遊戲難度
void score_dis(byte score) {
  if (score > 80) speed_time = 100;
  else speed_time = map(score, 0, 80, 1000, 100);
  if (score > 99) score = 99;
  lc.setRow(2, 0, num[score / 10]);
  lc.setRow(2, 1, num[score % 10]);
}
//模擬Tetris的方塊落下的順序方式，方塊隨機出現，一組7種沒輪完不會重複
byte n = 6, var[7];
byte random_7(bool clean) {
  bool k;
  if (clean) n = 6;
  else {
    if (n == 6) {
      for (byte a = 0; a < 7; a++) {
        do {
          var[a] = random(7);
          k = 0;
          for (byte b = 0; b < a; b++) if (var[b] == var[a]) k = 1;
        } while (k);
      }
      n = 0;
    }
    else n++;
    return var[n];
  }
}
//呼叫下一個方塊
void re() {
  mode = 0; //初始方塊方向
  x = 3; y = 2; //初始方塊位置
  Gameover = 0;
  Tetris = random_7(0); //取得下一個方塊編號
  if (Tetris == 0) I_open();
  else if (Tetris == 1) O_open();
  else if (Tetris == 2) S_open();
  else if (Tetris == 3) Z_open();
  else if (Tetris == 4) J_open();
  else if (Tetris == 5) L_open();
  else if (Tetris == 6) T_open();
  down();
}
//呼叫方塊下降並回傳是否碰底
bool down() {
  if (Gameover) return 1;
  else if (Tetris == 0) return I_down();
  else if (Tetris == 1) return O_down();
  else if (Tetris == 2) return S_down();
  else if (Tetris == 3) return Z_down();
  else if (Tetris == 4) return J_down();
  else if (Tetris == 5) return L_down();
  else if (Tetris == 6) return T_down();
}
//呼叫方塊左移
void left() {
  if (Tetris == 0) I_left();
  else if (Tetris == 1) O_left();
  else if (Tetris == 2) S_left();
  else if (Tetris == 3) Z_left();
  else if (Tetris == 4) J_left();
  else if (Tetris == 5) L_left();
  else if (Tetris == 6) T_left();
}
//呼叫方塊右移
void right() {
  if (Tetris == 0) I_right();
  else if (Tetris == 1) O_right();
  else if (Tetris == 2) S_right();
  else if (Tetris == 3) Z_right();
  else if (Tetris == 4) J_right();
  else if (Tetris == 5) L_right();
  else if (Tetris == 6) T_right();
}
//呼叫方塊轉向
void up() {
  if (Tetris == 0) I_up();
  else if (Tetris == 2) S_up();
  else if (Tetris == 3) Z_up();
  else if (Tetris == 4) J_up();
  else if (Tetris == 5) L_up();
  else if (Tetris == 6) T_up();
}
//以下都是方塊的移動方式和轉向方式和判斷是否可轉或可移動
void I_open() {
  lcd[y][x - 1] = 1;
  lcd[y][x] = 1;
  lcd[y][x + 1] = 1;
  lcd[y][x + 2] = 1;
}
bool I_down() {
  if (!mode) {
    if (!(lcd[y + 1][x - 1] || lcd[y + 1][x] || lcd[y + 1][x + 1] || lcd[y + 1][x + 2]) && y != 18) {
      lcd[y + 1][x - 1] = 1;
      lcd[y + 1][x] = 1;
      lcd[y + 1][x + 1] = 1;
      lcd[y + 1][x + 2] = 1;
      lcd[y][x - 1] = 0;
      lcd[y][x] = 0;
      lcd[y][x + 1] = 0;
      lcd[y][x + 2] = 0;
      y++;
      if (bee_oe) Bee(1, 1000, 200);
      return 0;
    }
    else return 1;
  }
  else {
    if (!(lcd[y + 2][x]) && y != 17) {
      lcd[y + 2][x] = 1;
      lcd[y - 2][x] = 0;
      y++;
      if (bee_oe) Bee(1, 1000, 200);
      return 0;
    }
    else return 1;
  }
}
void I_right() {
  if (!mode) {
    if (!(lcd[y][x + 3]) && x != 5) {
      lcd[y][x + 3] = 1;
      lcd[y][x - 1] = 0;
      x++;
    }
  }
  else {
    if (!(lcd[y - 2][x + 1] || lcd[y - 1][x + 1] || lcd[y][x + 1] || lcd[y + 1][x + 1]) && x != 7) {
      lcd[y - 2][x + 1] = 1;
      lcd[y - 1][x + 1] = 1;
      lcd[y][x + 1] = 1;
      lcd[y + 1][x + 1] = 1;
      lcd[y - 2][x] = 0;
      lcd[y - 1][x] = 0;
      lcd[y][x] = 0;
      lcd[y + 1][x] = 0;
      x++;
    }
  }
}
void I_left() {
  if (!mode) {
    if (!(lcd[y][x - 2]) && x != 1) {
      lcd[y][x - 2] = 1;
      lcd[y][x + 2] = 0;
      x--;
    }
  }
  else {
    if (!(lcd[y - 2][x - 1] || lcd[y - 1][x - 1] || lcd[y ][x - 1] || lcd[y + 1][x - 1]) && x != 0) {
      lcd[y - 2][x - 1] = 1;
      lcd[y - 1][x - 1] = 1;
      lcd[y][x - 1] = 1;
      lcd[y + 1][x - 1] = 1;
      lcd[y - 2][x] = 0;
      lcd[y - 1][x] = 0;
      lcd[y][x] = 0;
      lcd[y + 1][x] = 0;
      x--;
    }
  }
}
void I_up() {
  if (!mode) {
    if (!(lcd[y - 2][x] || lcd[y - 1][x] || lcd[y + 1][x]) && y != 18) {
      lcd[y - 2][x] = 1;
      lcd[y - 1][x] = 1;
      lcd[y + 1][x] = 1;
      lcd[y][x - 1] = 0;
      lcd[y][x + 1] = 0;
      lcd[y][x + 2] = 0;
      mode = 1;
    }
  }
  else {
    if (!(lcd[y][x - 1] || lcd[y][x + 1] || lcd[y][x + 2]) && x > 0 && x < 6) {
      lcd[y][x - 1] = 1;
      lcd[y][x + 1] = 1;
      lcd[y][x + 2] = 1;
      lcd[y - 2][x] = 0;
      lcd[y - 1][x] = 0;
      lcd[y + 1][x] = 0;
      mode = 0;
    }
    else if (!(lcd[y][x + 1] || lcd[y][x + 2] || lcd[y][x + 3]) && x < 5) {
      lcd[y][x + 1] = 1;
      lcd[y][x + 2] = 1;
      lcd[y][x + 3] = 1;
      lcd[y - 2][x] = 0;
      lcd[y - 1][x] = 0;
      lcd[y + 1][x] = 0;
      x++;
      mode = 0;
    }
    else if (!(lcd[y][x + 1] || lcd[y][x - 1] || lcd[y][x - 2]) && x > 1 && x < 7) {
      lcd[y][x + 1] = 1;
      lcd[y][x - 1] = 1;
      lcd[y][x - 2] = 1;
      lcd[y - 2][x] = 0;
      lcd[y - 1][x] = 0;
      lcd[y + 1][x] = 0;
      x--;
      mode = 0;
    }
    else if (!(lcd[y][x - 1] || lcd[y][x - 2] || lcd[y][x - 3]) && x > 2) {
      lcd[y][x - 1] = 1;
      lcd[y][x - 2] = 1;
      lcd[y][x - 3] = 1;
      lcd[y - 2][x] = 0;
      lcd[y - 1][x] = 0;
      lcd[y + 1][x] = 0;
      x -= 2;
      mode = 0;
    }
  }
}
void O_open() {
  lcd[y][x] = 1;
  lcd[y][x + 1] = 1;
  lcd[y - 1][x] = 1;
  lcd[y - 1][x + 1] = 1;
}
bool O_down() {
  if (!(lcd[y + 1][x] || lcd[y + 1][x + 1]) && y != 18) {
    lcd[y + 1][x] = 1;
    lcd[y + 1][x + 1] = 1;
    lcd[y - 1][x] = 0;
    lcd[y - 1][x + 1] = 0;
    y++;
    if (bee_oe) Bee(1, 1000, 200);
    return 0;
  }
  else return 1;
}
void O_left() {
  if (!(lcd[y][x - 1] || lcd[y - 1][x - 1]) && x != 0) {
    lcd[y][x - 1] = 1;
    lcd[y - 1][x - 1] = 1;
    lcd[y][x + 1] = 0;
    lcd[y - 1][x + 1] = 0;
    x--;
  }
}
void O_right() {
  if (!(lcd[y][x + 2] || lcd[y - 1][x + 2]) && x != 6) {
    lcd[y][x + 2] = 1;
    lcd[y - 1][x + 2] = 1;
    lcd[y][x] = 0;
    lcd[y - 1][x] = 0;
    x++;
  }
}

void S_open() {
  lcd[y][x] = 1;
  lcd[y][x + 1] = 1;
  lcd[y - 1][x + 1] = 1;
  lcd[y - 1][x + 2] = 1;
}
bool S_down() {
  if (mode == 0) {
    if (!(lcd[y + 1][x] || lcd[y + 1][x + 1] || lcd[y][x + 2]) && y != 18) {
      lcd[y + 1][x] = 1;
      lcd[y + 1][x + 1] = 1;
      lcd[y][x + 2] = 1;
      lcd[y][x] = 0;
      lcd[y - 1][x + 1] = 0;
      lcd[y - 1][x + 2] = 0;
      y++;
      if (bee_oe) Bee(1, 1000, 200);
      return 0;
    }
    else return 1;
  }
  else {
    if (!(lcd[y][x] || lcd[y + 1][x + 1]) && y != 18) {
      lcd[y][x] = 1;
      lcd[y + 1][x + 1] = 1;
      lcd[y - 2][x] = 0;
      lcd[y - 1][x + 1] = 0;
      y++;
      if (bee_oe) Bee(1, 1000, 200);
      return 0;
    }
    else return 1;
  }
}
void S_left() {
  if (mode == 0) {
    if (!(lcd[y][x - 1] || lcd[y - 1][x]) && x > 0) {
      lcd[y][x - 1] = 1;
      lcd[y - 1][x] = 1;
      lcd[y][x + 1] = 0;
      lcd[y - 1][x + 2] = 0;
      x--;
    }
  }
  else {
    if (!(lcd[y][x] || lcd[y - 1][x - 1] || lcd[y - 2][x - 1]) && x > 0) {
      lcd[y][x] = 1;
      lcd[y - 1][x - 1] = 1;
      lcd[y - 2][x - 1] = 1;
      lcd[y][x + 1] = 0;
      lcd[y - 1][x + 1] = 0;
      lcd[y - 2][x] = 0;
      x--;
    }
  }
}
void S_right() {
  if (mode == 0) {
    if (!(lcd[y][x + 2] || lcd[y - 1][x + 3]) && x != 5) {
      lcd[y][x + 2] = 1;
      lcd[y - 1][x + 3] = 1;
      lcd[y][x] = 0;
      lcd[y - 1][x + 1] = 0;
      x++;
    }
  }
  else {
    if (!(lcd[y][x + 2] || lcd[y - 1][x + 2] || lcd[y - 2][x + 1]) && x != 6) {
      lcd[y][x + 2] = 1;
      lcd[y - 1][x + 2] = 1;
      lcd[y - 2][x + 1] = 1;
      lcd[y][x + 1] = 0;
      lcd[y - 1][x] = 0;
      lcd[y - 2][x] = 0;
      x++;
    }
  }
}
void S_up() {
  if (!mode) {
    if (!(lcd[y - 1][x] || lcd[y - 2][x])) {
      lcd[y - 1][x] = 1;
      lcd[y - 2][x] = 1;
      lcd[y][x] = 0;
      lcd[y - 1][x + 2] = 0;
      mode = 1;
    }
  }
  else {
    if (!(lcd[y][x] || lcd[y - 1][x + 2]) && x != 6) {
      lcd[y][x] = 1;
      lcd[y - 1][x + 2] = 1;
      lcd[y - 1][x] = 0;
      lcd[y - 2][x] = 0;
      mode = 0;
    }
    else if (!(lcd[y][x - 1] || lcd[y][x]) && x != 0) {
      lcd[y][x - 1] = 1;
      lcd[y][x] = 1;
      lcd[y - 2][x] = 0;
      lcd[y][x + 1] = 0;
      x--;
      mode = 0;
    }
  }
}

void Z_open() {
  lcd[y][x] = 1;
  lcd[y][x + 1] = 1;
  lcd[y - 1][x - 1] = 1;
  lcd[y - 1][x ] = 1;
}
bool Z_down() {
  if (!mode) {
    if (!(lcd[y][x - 1] || lcd[y + 1][x] || lcd[y + 1][x + 1]) && y != 18) {
      lcd[y][x - 1] = 1;
      lcd[y + 1][x] = 1;
      lcd[y + 1][x + 1] = 1;
      lcd[y - 1][x - 1] = 0;
      lcd[y - 1][x] = 0;
      lcd[y][x + 1] = 0;
      y++;
      if (bee_oe) Bee(1, 1000, 200);
      return 0;
    }
    else return 1;
  }
  else {
    if (!(lcd[y + 1][x] || lcd[y][x + 1]) &&  y != 18) {
      lcd[y + 1][x] = 1;
      lcd[y][x + 1] = 1;
      lcd[y - 1][x] = 0;
      lcd[y - 2][x + 1] = 0;
      y++;
      if (bee_oe) Bee(1, 1000, 200);
      return 0;
    }
    else return 1;
  }
}
void Z_left() {
  if (mode == 0) {
    if (!(lcd[y][x - 1] || lcd[y - 1][x - 2]) && x != 1) {
      lcd[y][x - 1] = 1;
      lcd[y - 1][x - 2] = 1;
      lcd[y][x + 1] = 0;
      lcd[y - 1][x] = 0;
      x--;
    }
  }
  else {
    if (!(lcd[y][x - 1] || lcd[y - 1][x - 1] || lcd[y - 2][x]) && x != 0) {
      lcd[y][x - 1] = 1;
      lcd[y - 1][x - 1]  = 1;
      lcd[y - 2][x] = 1;
      lcd[y][x ] = 0;
      lcd[y - 1][x + 1] = 0;
      lcd[y - 2][x + 1] = 0;
      x--;
    }
  }
}
void Z_right() {
  if (mode == 0) {
    if (!(lcd[y][x + 2] || lcd[y - 1][x + 1]) && x != 6) {
      lcd[y][x + 2] = 1;
      lcd[y - 1][x + 1] = 1;
      lcd[y][x] = 0;
      lcd[y - 1][x - 1] = 0;
      x++;
    }
  }
  else {
    if (!(lcd[y][x + 1] || lcd[y - 1][x + 2] || lcd[y - 2][x + 2]) && x != 6) {
      lcd[y][x + 1] = 1;
      lcd[y - 1][x + 2] = 1;
      lcd[y - 2][x + 2] = 1;
      lcd[y][x] = 0;
      lcd[y - 1][x] = 0;
      lcd[y - 2][x + 1] = 0;
      x++;
    }
  }
}
void Z_up() {
  if (!mode) {
    if (!(lcd[y - 1][x + 1] || lcd[y - 2][x + 1])) {
      lcd[y - 1][x + 1] = 1;
      lcd[y - 2][x + 1] = 1;
      lcd[y - 1][x - 1] = 0;
      lcd[y ][x + 1] = 0;
      mode = 1;
    }
  }
  else {
    if (!(lcd[y - 1][x - 1] || lcd[y][x + 1]) && x != 0) {
      lcd[y - 1][x - 1] = 1;
      lcd[y][x + 1] = 1;
      lcd[y - 1][x + 1] = 0;
      lcd[y - 2][x + 1] = 0;
      mode = 0;
    }
    else if (!(lcd[y][x + 1] || lcd[y][x + 2]) && x != 6) {
      lcd[y][x + 1] = 1;
      lcd[y][x + 2] = 1;
      lcd[y][x] = 0;
      lcd[y - 2][x + 1] = 0;
      x++;
      mode = 0;
    }
  }
}

void J_open() {
  lcd[y][x] = 1;
  lcd[y][x + 1] = 1;
  lcd[y - 1][x + 1] = 1;
  lcd[y - 2][x + 1] = 1;
}
bool J_down() {
  if (mode == 0) {
    if (!(lcd[y + 1][x] || lcd[y + 1][x + 1]) && y != 18) {
      lcd[y + 1][x] = 1;
      lcd[y + 1][x + 1] = 1;
      lcd[y][x] = 0;
      lcd[y - 2][x + 1] = 0;
      y++;
      if (bee_oe) Bee(1, 1000, 200);
      return 0;
    }
    else return 1;
  }
  else if (mode == 1) {
    if (!(lcd[y + 1][x - 1] || lcd[y + 1][x] || lcd[y + 1][x + 1]) &&  y != 18) {
      lcd[y + 1][x - 1] = 1;
      lcd[y + 1][x] = 1;
      lcd[y + 1][x + 1] = 1;
      lcd[y - 1][x - 1] = 0;
      lcd[y][x ] = 0;
      lcd[y][x + 1] = 0;
      y++;
      if (bee_oe) Bee(1, 1000, 200);
      return 0;
    }
    else return 1;
  }
  else if (mode == 2) {
    if (!(lcd[y + 1][x - 1] || lcd[y - 1][x]) &&  y != 18) {
      lcd[y + 1][x - 1] = 1;
      lcd[y - 1][x] = 1;
      lcd[y - 2][x - 1] = 0;
      lcd[y - 2][x ] = 0;
      y++;
      if (bee_oe) Bee(1, 1000, 200);
      return 0;
    }
    else return 1;
  }
  else if (mode == 3) {
    if (!(lcd[y][x + 1] || lcd[y - 1][x - 1] || lcd[y - 1][x]) &&  y != 19) {
      lcd[y][x + 1] = 1;
      lcd[y - 1][x - 1] = 1;
      lcd[y - 1][x] = 1;
      lcd[y - 2][x - 1] = 0;
      lcd[y - 2][x] = 0;
      lcd[y - 2][x + 1] = 0;
      y++;
      if (bee_oe) Bee(1, 1000, 200);
      return 0;
    }
    else return 1;
  }
}
void J_left() {
  if (mode == 0) {
    if (!(lcd[y][x - 1] || lcd[y - 1][x] || lcd[y - 2][x]) && x != 0) {
      lcd[y][x - 1] = 1;
      lcd[y - 1][x]  = 1;
      lcd[y - 2][x] = 1;
      lcd[y][x + 1] = 0;
      lcd[y - 1][x + 1] = 0;
      lcd[y - 2][x + 1] = 0;
      x--;
    }
  }
  else if (mode == 1) {
    if (!(lcd[y][x - 2] || lcd[y - 1][x - 2]) && x != 1) {
      lcd[y][x - 2] = 1;
      lcd[y - 1][x - 2]  = 1;
      lcd[y][x + 1] = 0;
      lcd[y - 1][x - 1] = 0;
      x--;
    }
  }
  else if (mode == 2) {
    if (!(lcd[y][x - 2] || lcd[y - 1][x - 2] || lcd[y - 2][x - 2]) && x != 1) {
      lcd[y][x - 2] = 1;
      lcd[y - 1][x - 2]  = 1;
      lcd[y - 2][x - 2] = 1;
      lcd[y][x - 1] = 0;
      lcd[y - 1][x - 1] = 0;
      lcd[y - 2][x] = 0;
      x--;
    }
  }
  else if (mode == 3) {
    if (!(lcd[y - 1][x] || lcd[y - 2][x - 2]) && x != 1) {
      lcd[y - 1][x] = 1;
      lcd[y - 2][x - 2]  = 1;
      lcd[y - 1][x + 1] = 0;
      lcd[y - 2][x + 1] = 0;
      x--;
    }
  }
}
void J_right() {
  if (mode == 0) {
    if (!(lcd[y][x + 2] || lcd[y - 1][x + 2] || lcd[y - 2][x + 2]) && x != 6) {
      lcd[y][x + 2] = 1;
      lcd[y - 1][x + 2] = 1;
      lcd[y - 2][x + 2] = 1;
      lcd[y][x] = 0;
      lcd[y - 1][x + 1] = 0;
      lcd[y - 2][x + 1] = 0;
      x++;
    }
  }
  else if (mode == 1) {
    if (!(lcd[y][x + 2] || lcd[y - 1][x]) && x != 6) {
      lcd[y][x + 2]  = 1;
      lcd[y - 1][x] = 1;
      lcd[y][x - 1] = 0;
      lcd[y - 1][x - 1] = 0;
      x++;
    }
  }
  else if (mode == 2) {
    if (!(lcd[y][x] || lcd[y - 1][x] || lcd[y - 2][x + 1]) && x != 7) {
      lcd[y][x] = 1;
      lcd[y - 1][x] = 1;
      lcd[y - 2][x + 1] = 1;
      lcd[y][x - 1] = 0;
      lcd[y - 1][x - 1] = 0;
      lcd[y - 2][x - 1] = 0;
      x++;
    }
  }
  else if (mode == 3) {
    if (!(lcd[y - 1][x + 2] || lcd[y - 2][x + 2]) && x != 6) {
      lcd[y - 1][x + 2]  = 1;
      lcd[y - 2][x + 2] = 1;
      lcd[y - 1][x + 1] = 0;
      lcd[y - 2][x - 1] = 0;
      x++;
    }
  }
}
void J_up() {
  if (mode == 0) {
    if (!(lcd[y][x - 1] || lcd[y - 1][x - 1]) && x != 0) {
      lcd[y][x - 1] = 1;
      lcd[y - 1][x - 1] = 1;
      lcd[y - 1][x + 1] = 0;
      lcd[y - 2][x + 1] = 0;
      mode = 1;
    }
    else if (!(lcd[y - 1][x] || lcd[y][x + 2]) && x != 6) {
      lcd[y - 1][x] = 1;
      lcd[y][x + 2] = 1;
      lcd[y - 1][x + 1] = 0;
      lcd[y - 2][x + 1] = 0;
      x++;
      mode = 1;
    }
  }
  else if (mode == 1) {
    if (!(lcd[y - 2][x - 1] || lcd[y - 2][x])) {
      lcd[y - 2][x - 1] = 1;
      lcd[y - 2][x] = 1;
      lcd[y][x] = 0;
      lcd[y][x + 1] = 0;
      mode = 2;
    }
  }
  else if (mode == 2) {
    if (!(lcd[y - 1][x + 1] || lcd[y - 2][x + 1]) && x != 7) {
      lcd[y - 1][x + 1] = 1;
      lcd[y - 2][x + 1] = 1;
      lcd[y][x - 1] = 0;
      lcd[y - 1][x - 1] = 0;
      mode = 3;
    }
    else if (!(lcd[y - 2][x - 2] || lcd[y - 1][x]) && x != 1) {
      lcd[y - 2][x - 2] = 1;
      lcd[y - 1][x] = 1;
      lcd[y][x - 1] = 0;
      lcd[y - 1][x - 1] = 0;
      x--;
      mode = 3;
    }
  }
  else if (mode == 3) {
    if (!(lcd[y][x] || lcd[y][x + 1]) && y != 19) {
      lcd[y][x] = 1;
      lcd[y][x + 1] = 1;
      lcd[y - 2][x - 1] = 0;
      lcd[y - 2][x] = 0;
      mode = 0;
    }
  }
}

void L_open() {
  lcd[y][x + 1] = 1;
  lcd[y][x] = 1;
  lcd[y - 1][x] = 1;
  lcd[y - 2][x] = 1;
}
bool L_down() {
  if (mode == 0) {
    if (!(lcd[y + 1][x + 1] || lcd[y + 1][x]) && y != 18) {
      lcd[y + 1][x + 1] = 1;
      lcd[y + 1][x ] = 1;
      lcd[y][x + 1] = 0;
      lcd[y - 2][x] = 0;
      y++;
      if (bee_oe) Bee(1, 1000, 200);
      return 0;
    }
    else return 1;
  }
  else if (mode == 1) {
    if (!(lcd[y][x ] || lcd[y - 1][x + 1] || lcd[y - 1][x + 2]) &&  y != 19) {
      lcd[y][x ] = 1;
      lcd[y - 1][x + 1] = 1;
      lcd[y - 1][x + 2] = 1;
      lcd[y - 2][x ] = 0;
      lcd[y - 2][x + 1] = 0;
      lcd[y - 2][x + 2] = 0;
      y++;
      if (bee_oe) Bee(1, 1000, 200);
      return 0;
    }
    else return 1;
  }
  else if (mode == 2) {
    if (!(lcd[y - 1][x + 1] || lcd[y + 1][x + 2]) &&  y != 18) {
      lcd[y - 1][x + 1] = 1;
      lcd[y + 1][x + 2] = 1;
      lcd[y - 2][x + 2] = 0;
      lcd[y - 2][x + 1] = 0;
      y++;
      if (bee_oe) Bee(1, 1000, 200);
      return 0;
    }
    else return 1;
  }
  else if (mode == 3) {
    if (!(lcd[y + 1][x] || lcd[y + 1][x + 1] || lcd[y + 1][x + 2]) &&  y != 18) {
      lcd[y + 1][x ] = 1;
      lcd[y + 1][x + 1] = 1;
      lcd[y + 1][x + 2] = 1;
      lcd[y][x ] = 0;
      lcd[y][x + 1] = 0;
      lcd[y - 1][x + 2] = 0;
      y++;
      if (bee_oe) Bee(1, 1000, 200);
      return 0;
    }
    else return 1;
  }
}
void L_left() {
  if (mode == 0) {
    if (!(lcd[y][x - 1] || lcd[y - 1][x - 1] || lcd[y - 2][x - 1]) && x != 0) {
      lcd[y][x - 1] = 1;
      lcd[y - 1][x - 1]  = 1;
      lcd[y - 2][x - 1] = 1;
      lcd[y][x + 1] = 0;
      lcd[y - 1][x] = 0;
      lcd[y - 2][x] = 0;
      x--;
    }
  }
  else if (mode == 1) {
    if (!(lcd[y - 1][x - 1] || lcd[y - 2][x - 1]) && x != 0) {
      lcd[y - 1][x - 1] = 1;
      lcd[y - 2][x - 1]  = 1;
      lcd[y - 1][x] = 0;
      lcd[y - 2][x + 2] = 0;
      x--;
    }
  }
  else if (mode == 2) {
    if (!(lcd[y][x + 1] || lcd[y - 1][x + 1] || lcd[y - 2][x]) && x != -1) {
      lcd[y][x + 1]  = 1;
      lcd[y - 1][x + 1]  = 1;
      lcd[y - 2][x] = 1;
      lcd[y][x + 2] = 0;
      lcd[y - 1][x + 2] = 0;
      lcd[y - 2][x + 2] = 0;
      x--;
    }
  }
  else if (mode == 3) {
    if (!(lcd[y][x - 1] || lcd[y - 1][x + 1]) && x != 0) {
      lcd[y][x - 1] = 1;
      lcd[y - 1][x + 1] = 1;
      lcd[y][x + 2] = 0;
      lcd[y - 1][x + 2] = 0;
      x--;
    }
  }
}
void L_right() {
  if (mode == 0) {
    if (!(lcd[y][x + 2] || lcd[y - 1][x + 1] || lcd[y - 2][x + 1]) && x != 6) {
      lcd[y][x + 2] = 1;
      lcd[y - 1][x + 1] = 1;
      lcd[y - 2][x + 1] = 1;
      lcd[y][x] = 0;
      lcd[y - 1][x] = 0;
      lcd[y - 2][x] = 0;
      x++;
    }
  }
  else if (mode == 1) {
    if (!(lcd[y - 2][x + 3] || lcd[y - 1][x + 1]) && x != 5) {
      lcd[y - 2][x + 3]  = 1;
      lcd[y - 1][x + 1] = 1;
      lcd[y - 2][x ] = 0;
      lcd[y - 1][x ] = 0;
      x++;
    }
  }
  else if (mode == 2) {
    if (!(lcd[y][x + 3] || lcd[y - 1][x + 3] || lcd[y - 2][x + 3]) && x != 5) {
      lcd[y][x + 3] = 1;
      lcd[y - 1][x + 3] = 1;
      lcd[y - 2][x + 3] = 1;
      lcd[y][x + 2] = 0;
      lcd[y - 1][x + 2] = 0;
      lcd[y - 2][x + 1] = 0;
      x++;
    }
  }
  else if (mode == 3) {
    if (!(lcd[y][x + 3] || lcd[y - 1][x + 3]) && x != 5) {
      lcd[y][x + 3] = 1;
      lcd[y - 1][x + 3] = 1;
      lcd[y][x] = 0;
      lcd[y - 1][x + 2] = 0;
      x++;
    }
  }
}
void L_up() {
  if (mode == 0) {
    if (!(lcd[y - 2][x + 1] || lcd[y - 2][x + 2]) && x != 6) {
      lcd[y - 2][x + 1] = 1;
      lcd[y - 2][x + 2] = 1;
      lcd[y][x ] = 0;
      lcd[y][x + 1] = 0;
      mode = 1;
    }
    else if (!(lcd[y - 2][x - 1] || lcd[y - 1][x - 1] || lcd[y - 2][x + 1]) && x != 0) {
      lcd[y - 2][x - 1] = 1;
      lcd[y - 1][x - 1] = 1;
      lcd[y - 2][x + 1] = 1;
      lcd[y - 1][x ] = 0;
      lcd[y][x] = 0;
      lcd[y][x + 1] = 0;
      x--;
      mode = 1;
    }
  }
  else if (mode == 1) {
    if (!(lcd[y][x + 2] || lcd[y - 1][x + 2]) && y != 19) {
      lcd[y][x + 2] = 1;
      lcd[y - 1][x + 2] = 1;
      lcd[y - 1][x] = 0;
      lcd[y - 2][x] = 0;
      mode = 2;
    }
  }
  else if (mode == 2) {
    if (!(lcd[y][x] || lcd[y][x + 1]) && x != -1) {
      lcd[y][x] = 1;
      lcd[y][x + 1] = 1;
      lcd[y - 2][x + 1] = 0;
      lcd[y - 2][x + 2] = 0;
      mode = 3;
    }
    else if (!(lcd[y][x + 1] || lcd[y][x + 3] || lcd[y - 1][x + 3]) && x != 5) {
      lcd[y][x + 1] = 1;
      lcd[y][x + 3] = 1;
      lcd[y - 1][x + 3] = 1;
      lcd[y - 2][x + 1] = 0;
      lcd[y - 2][x + 2] = 0;
      lcd[y - 1][x + 2] = 0;
      x++;
      mode = 3;
    }
  }
  else if (mode == 3) {
    if (!(lcd[y - 1][x] || lcd[y - 2][x]) && y != 19) {
      lcd[y - 1][x] = 1;
      lcd[y - 2][x ] = 1;
      lcd[y - 1][x + 2] = 0;
      lcd[y][x + 2] = 0;
      mode = 0;
    }
  }
}

void T_open() {
  lcd[y][x] = 1;
  lcd[y][x + 1] = 1;
  lcd[y][x + 2] = 1;
  lcd[y - 1][x + 1] = 1;
}
bool T_down() {
  if (mode == 0) {
    if (!(lcd[y + 1][x] || lcd[y + 1][x + 1] || lcd[y + 1][x + 2]) && y != 18) {
      lcd[y + 1][x] = 1;
      lcd[y + 1][x + 1] = 1;
      lcd[y + 1][x + 2] = 1;
      lcd[y][x] = 0;
      lcd[y - 1][x + 1] = 0;
      lcd[y][x + 2] = 0;
      y++;
      if (bee_oe) Bee(1, 1000, 200);
      return 0;
    }
    else return 1;
  }
  else if (mode == 1) {
    if (!(lcd[y + 2][x + 1] || lcd[y + 1][x + 2]) &&  y != 17) {
      lcd[y + 2][x + 1 ] = 1;
      lcd[y + 1][x + 2] = 1;
      lcd[y - 1][x + 1] = 0;
      lcd[y][x + 2] = 0;
      y++;
      if (bee_oe) Bee(1, 1000, 200);
      return 0;
    }
    else return 1;
  }
  else if (mode == 2) {
    if (!(lcd[y + 1][x ] || lcd[y + 2][x + 1] || lcd[y + 1][x + 2]) &&  y != 17) {
      lcd[y + 1][x ]  = 1;
      lcd[y + 2][x + 1] = 1;
      lcd[y + 1][x + 2] = 1;
      lcd[y][x] = 0;
      lcd[y][x + 1] = 0;
      lcd[y][x + 2] = 0;
      y++;
      if (bee_oe) Bee(1, 1000, 200);
      return 0;
    }
    else return 1;
  }
  else if (mode == 3) {
    if (!(lcd[y + 1][x] || lcd[y + 2][x + 1]) &&  y != 17) {
      lcd[y + 1][x ] = 1;
      lcd[y + 2][x + 1] = 1;
      lcd[y][x ] = 0;
      lcd[y - 1][x + 1] = 0;
      y++;
      if (bee_oe) Bee(1, 1000, 200);
      return 0;
    }
    else return 1;
  }
}
void T_left() {
  if (mode == 0) {
    if (!(lcd[y][x - 1] || lcd[y - 1][x]) && x != 0) {
      lcd[y][x - 1] = 1;
      lcd[y - 1][x]  = 1;
      lcd[y][x + 2] = 0;
      lcd[y - 1][x + 1] = 0;
      x--;
    }
  }
  else if (mode == 1) {
    if (!(lcd[y][x] || lcd[y - 1][x] || lcd[y + 1][x]) && x != -1) {
      lcd[y][x] = 1;
      lcd[y - 1][x]  = 1;
      lcd[y + 1][x]  = 1;
      lcd[y - 1][x + 1] = 0;
      lcd[y][x + 2] = 0;
      lcd[y + 1][x + 1] = 0;
      x--;
    }
  }
  else if (mode == 2) {
    if (!(lcd[y][x - 1] || lcd[y + 1][x]) && x != 0) {
      lcd[y][x - 1]  = 1;
      lcd[y + 1][x]  = 1;
      lcd[y][x + 2] = 0;
      lcd[y + 1][x + 1] = 0;
      x--;
    }
  }
  else if (mode == 3) {
    if (!(lcd[y - 1][x] || lcd[y][x - 1] || lcd[y + 1][x ]) && x != 0) {
      lcd[y - 1][x] = 1;
      lcd[y][x - 1] = 1;
      lcd[y + 1][x ] = 1;
      lcd[y - 1][x + 1] = 0;
      lcd[y][x + 1] = 0;
      lcd[y + 1][x + 1] = 0;
      x--;
    }
  }
}
void T_right() {
  if (mode == 0) {
    if (!(lcd[y][x + 3] || lcd[y - 1][x + 2]) && x != 5) {
      lcd[y][x + 3] = 1;
      lcd[y - 1][x + 2]  = 1;
      lcd[y][x] = 0;
      lcd[y - 1][x + 1] = 0;
      x++;
    }
  }
  else if (mode == 1) {
    if (!(lcd[y - 1][x + 2] || lcd[y][x + 3] || lcd[y + 1][x + 2]) && x != 5) {
      lcd[y - 1][x + 2] = 1;
      lcd[y][x + 3]  = 1;
      lcd[y + 1][x + 2]  = 1;
      lcd[y - 1][x + 1] = 0;
      lcd[y][x + 1] = 0;
      lcd[y + 1][x + 1] = 0;
      x++;
    }
  }
  else if (mode == 2) {
    if (!(lcd[y][x + 3] || lcd[y + 1][x + 2]) && x != 5) {
      lcd[y][x + 3]  = 1;
      lcd[y + 1][x + 2]  = 1;
      lcd[y][x] = 0;
      lcd[y + 1][x + 1] = 0;
      x++;
    }
  }
  else if (mode == 3) {
    if (!(lcd[y - 1][x + 2] || lcd[y][x + 2] || lcd[y + 1][x + 2 ]) && x != 6) {
      lcd[y - 1][x + 2] = 1;
      lcd[y][x + 2] = 1;
      lcd[y + 1][x + 2] = 1;
      lcd[y - 1][x + 1] = 0;
      lcd[y][x] = 0;
      lcd[y + 1][x + 1] = 0;
      x++;
    }
  }
}
void T_up() {
  if (mode == 0) {
    if (!(lcd[y + 1][x + 1]) && y != 18) {
      lcd[y + 1][x + 1] = 1;
      lcd[y][x] = 0;
      mode = 1;
    }
  }
  else if (mode == 1) {
    if (!(lcd[y][x]) && x != -1) {
      lcd[y][x] = 1;
      lcd[y - 1][x + 1] = 0;
      mode = 2;
    }
    else if (!(lcd[y][x + 3] || lcd[y + 1][x + 2]) && x != 5) {
      lcd[y][x + 3] = 1;
      lcd[y + 1][x + 2] = 1;
      lcd[y - 1][x + 1] = 0;
      lcd[y + 1][x + 1] = 0;
      x++;
      mode = 2;
    }
  }
  else if (mode == 2) {
    if (!(lcd[y - 1][x + 1]) ) {
      lcd[y - 1][x + 1] = 1;
      lcd[y][x + 2] = 0;
      mode = 3;
    }
  }
  else if (mode == 3) {
    if (!(lcd[y][x + 2]) && x != 6) {
      lcd[y][x + 2] = 1;
      lcd[y + 1][x + 1] = 0;
      mode = 0;
    }
    else if (!(lcd[y][x - 1] || lcd[y - 1][x]) && x != 0) {
      lcd[y][x - 1] = 1;
      lcd[y - 1][x] = 1;
      lcd[y - 1][x + 1] = 0;
      lcd[y + 1][x + 1] = 0;
      x--;
      mode = 0;
    }
  }
}