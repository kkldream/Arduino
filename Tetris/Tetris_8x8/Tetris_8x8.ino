#include "LedControl.h"
LedControl lc = LedControl(12,11,10,1);  //Pins:DIN,CLK,CS,Display connected
const byte SW_Pin[] = {A0, A1, A2, A3};
bool lcd[11][8], gameover;
byte y, mode, Score, Tetris;
int x;

void setup() {
  randomSeed(analogRead(A7));
  random(7);
  lc.shutdown(0, 0);
  lc.setIntensity(0, 15);
  lc.clearDisplay(0);
  for (byte a = 0; a < 4; a++) {
    pinMode(SW_Pin[a], OUTPUT);
    digitalWrite(SW_Pin[a], LOW);
  }
  re();
}

void loop() {
  unsigned long Old_millis = millis();
  while (millis() - Old_millis < 1000) {
    lcd_dis();
    SW();
  }
  if (down()) {
    gameover = 0;
    for (byte a = 0; a < 8; a++) if (lcd[2][a]) gameover = 1;
    if (gameover) {
      random_7(1);
      for (byte b = 0; b < 8; b++) {
        for (byte a = 0; a < 8; a++) {
          lcd[2 * a + 3][b] = 1;
          lcd[2 * a + 4][7 - b] = 1;
        }
        lcd_dis();
        delay(100);
      }
      for (byte b = 0; b < 8; b++) {
        for (byte a = 0; a < 8; a++) {
          lcd[2 * a + 3][b] = 0;
          lcd[2 * a + 4][7 - b] = 0;
        }
        for (byte b = 0; b < 3; b++) for (byte a = 0; a < 8; a++) lcd[b][a] = 0;
        lcd_dis();
        delay(100);
      }
      Score = 0;
    }
    else {
      bool clr[8] = {1, 1, 1, 1, 1, 1, 1, 1};
      for (byte b = 0; b < 8; b++) for (byte a = 0; a < 8; a++) if (!lcd[b + 3][a]) clr[b] = 0;
      for (byte b = 0; b < 8; b++) for (byte a = 0; a < 8; a++) if (clr[b]) lcd[b + 3][a] = 0;
      lcd_dis();
      delay(100);
      for (byte b = 0; b < 8; b++) for (byte a = 0; a < 8; a++) if (clr[b]) lcd[b + 3][a] = 1;
      lcd_dis();
      delay(100);
      for (byte b = 0; b < 8; b++) for (byte a = 0; a < 8; a++) if (clr[b]) lcd[b + 3][a] = 0;
      lcd_dis();
      for (byte b = 3; b < 11; b++) {
        if (clr[b - 3]) {
          for (byte c = 0; c < b - 2; c++) for (byte a = 0; a < 8; a++) lcd[b - c][a] = lcd[b - 1 - c][a];
          Score++;
        }
      }
    }
    re();
  }
}

unsigned long down_millis, left_right_millis;
bool left_right_press, up_press;
void SW() {
  if (digitalRead(SW_Pin[2]) && millis() - down_millis > 50) {
    down();
    down_millis = millis();
  }
  if (!digitalRead(SW_Pin[0]) || !digitalRead(SW_Pin[3])) {
    if (digitalRead(SW_Pin[3])) {
      if (digitalRead(SW_Pin[3]) &&  long(millis() - left_right_millis) > 50) {
        right();
        if (!left_right_press) left_right_millis = millis() + 200;
        else left_right_millis = millis();
        left_right_press = 1;
      }
      else if (!digitalRead(SW_Pin[3]))left_right_press = 0;
    }
    else {
      if (digitalRead(SW_Pin[0]) &&  long(millis() - left_right_millis) > 50) {
        left();
        if (!left_right_press) left_right_millis = millis() + 200;
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

void lcd_dis() {
  for (byte b = 3; b < 11; b++) {
    byte c = 0;
    for (byte a = 0; a < 8; a++) if (lcd[b][a]) c = c | (0B10000000 >> a);
    lc.setRow(0, b - 3, c);
  }
}

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

void re() {
  mode = 0; x = 3; y = 2;
  Tetris = random_7(0);
  if (Tetris == 0) I_open();
  else if (Tetris == 1) O_open();
  else if (Tetris == 2) S_open();
  else if (Tetris == 3) Z_open();
  else if (Tetris == 4) J_open();
  else if (Tetris == 5) L_open();
  else if (Tetris == 6) T_open();
  down();
}

bool down() {
  if (Tetris == 0) return I_down();
  else if (Tetris == 1) return O_down();
  else if (Tetris == 2) return S_down();
  else if (Tetris == 3) return Z_down();
  else if (Tetris == 4) return J_down();
  else if (Tetris == 5) return L_down();
  else if (Tetris == 6) return T_down();
}
void left() {
  if (Tetris == 0) I_left();
  else if (Tetris == 1) O_left();
  else if (Tetris == 2) S_left();
  else if (Tetris == 3) Z_left();
  else if (Tetris == 4) J_left();
  else if (Tetris == 5) L_left();
  else if (Tetris == 6) T_left();
}
void right() {
  if (Tetris == 0) I_right();
  else if (Tetris == 1) O_right();
  else if (Tetris == 2) S_right();
  else if (Tetris == 3) Z_right();
  else if (Tetris == 4) J_right();
  else if (Tetris == 5) L_right();
  else if (Tetris == 6) T_right();
}
void up() {
  if (Tetris == 0) I_up();
  else if (Tetris == 2) S_up();
  else if (Tetris == 3) Z_up();
  else if (Tetris == 4) J_up();
  else if (Tetris == 5) L_up();
  else if (Tetris == 6) T_up();
}

void I_open() {
  lcd[y][x - 1] = 1;
  lcd[y][x] = 1;
  lcd[y][x + 1] = 1;
  lcd[y][x + 2] = 1;
}
bool I_down() {
  if (!mode) {
    if (!(lcd[y + 1][x - 1] || lcd[y + 1][x] || lcd[y + 1][x + 1] || lcd[y + 1][x + 2]) && y != 10) {
      lcd[y + 1][x - 1] = 1;
      lcd[y + 1][x] = 1;
      lcd[y + 1][x + 1] = 1;
      lcd[y + 1][x + 2] = 1;
      lcd[y][x - 1] = 0;
      lcd[y][x] = 0;
      lcd[y][x + 1] = 0;
      lcd[y][x + 2] = 0;
      y++;
      return 0;
    }
    else return 1;
  }
  else {
    if (!(lcd[y + 2][x]) && y != 9) {
      lcd[y + 2][x] = 1;
      lcd[y - 2][x] = 0;
      y++;
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
    if (!(lcd[y - 2][x] || lcd[y - 1][x] || lcd[y + 1][x]) && y != 10) {
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
  if (!(lcd[y + 1][x] || lcd[y + 1][x + 1]) && y != 10) {
    lcd[y + 1][x] = 1;
    lcd[y + 1][x + 1] = 1;
    lcd[y - 1][x] = 0;
    lcd[y - 1][x + 1] = 0;
    y++;
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
    if (!(lcd[y + 1][x] || lcd[y + 1][x + 1] || lcd[y][x + 2]) && y != 10) {
      lcd[y + 1][x] = 1;
      lcd[y + 1][x + 1] = 1;
      lcd[y][x + 2] = 1;
      lcd[y][x] = 0;
      lcd[y - 1][x + 1] = 0;
      lcd[y - 1][x + 2] = 0;
      y++;
      return 0;
    }
    else return 1;
  }
  else {
    if (!(lcd[y][x] || lcd[y + 1][x + 1]) && y != 10) {
      lcd[y][x] = 1;
      lcd[y + 1][x + 1] = 1;
      lcd[y - 2][x] = 0;
      lcd[y - 1][x + 1] = 0;
      y++;
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
    if (!(lcd[y][x - 1] || lcd[y + 1][x] || lcd[y + 1][x + 1]) && y != 10) {
      lcd[y][x - 1] = 1;
      lcd[y + 1][x] = 1;
      lcd[y + 1][x + 1] = 1;
      lcd[y - 1][x - 1] = 0;
      lcd[y - 1][x] = 0;
      lcd[y][x + 1] = 0;
      y++;
      return 0;
    }
    else return 1;
  }
  else {
    if (!(lcd[y + 1][x] || lcd[y][x + 1]) &&  y != 10) {
      lcd[y + 1][x] = 1;
      lcd[y][x + 1] = 1;
      lcd[y - 1][x] = 0;
      lcd[y - 2][x + 1] = 0;
      y++;
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
    if (!(lcd[y + 1][x] || lcd[y + 1][x + 1]) && y != 10) {
      lcd[y + 1][x] = 1;
      lcd[y + 1][x + 1] = 1;
      lcd[y][x] = 0;
      lcd[y - 2][x + 1] = 0;
      y++;
      return 0;
    }
    else return 1;
  }
  else if (mode == 1) {
    if (!(lcd[y + 1][x - 1] || lcd[y + 1][x] || lcd[y + 1][x + 1]) &&  y != 10) {
      lcd[y + 1][x - 1] = 1;
      lcd[y + 1][x] = 1;
      lcd[y + 1][x + 1] = 1;
      lcd[y - 1][x - 1] = 0;
      lcd[y][x ] = 0;
      lcd[y][x + 1] = 0;
      y++;
      return 0;
    }
    else return 1;
  }
  else if (mode == 2) {
    if (!(lcd[y + 1][x - 1] || lcd[y - 1][x]) &&  y != 10) {
      lcd[y + 1][x - 1] = 1;
      lcd[y - 1][x] = 1;
      lcd[y - 2][x - 1] = 0;
      lcd[y - 2][x ] = 0;
      y++;
      return 0;
    }
    else return 1;
  }
  else if (mode == 3) {
    if (!(lcd[y][x + 1] || lcd[y - 1][x - 1] || lcd[y - 1][x]) &&  y != 11) {
      lcd[y][x + 1] = 1;
      lcd[y - 1][x - 1] = 1;
      lcd[y - 1][x] = 1;
      lcd[y - 2][x - 1] = 0;
      lcd[y - 2][x] = 0;
      lcd[y - 2][x + 1] = 0;
      y++;
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
    if (!(lcd[y][x] || lcd[y][x + 1]) && y != 11) {
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
    if (!(lcd[y + 1][x + 1] || lcd[y + 1][x]) && y != 10) {
      lcd[y + 1][x + 1] = 1;
      lcd[y + 1][x ] = 1;
      lcd[y][x + 1] = 0;
      lcd[y - 2][x] = 0;
      y++;
      return 0;
    }
    else return 1;
  }
  else if (mode == 1) {
    if (!(lcd[y][x ] || lcd[y - 1][x + 1] || lcd[y - 1][x + 2]) &&  y != 11) {
      lcd[y][x ] = 1;
      lcd[y - 1][x + 1] = 1;
      lcd[y - 1][x + 2] = 1;
      lcd[y - 2][x ] = 0;
      lcd[y - 2][x + 1] = 0;
      lcd[y - 2][x + 2] = 0;
      y++;
      return 0;
    }
    else return 1;
  }
  else if (mode == 2) {
    if (!(lcd[y - 1][x + 1] || lcd[y + 1][x + 2]) &&  y != 10) {
      lcd[y - 1][x + 1] = 1;
      lcd[y + 1][x + 2] = 1;
      lcd[y - 2][x + 2] = 0;
      lcd[y - 2][x + 1] = 0;
      y++;
      return 0;
    }
    else return 1;
  }
  else if (mode == 3) {
    if (!(lcd[y + 1][x] || lcd[y + 1][x + 1] || lcd[y + 1][x + 2]) &&  y != 10) {
      lcd[y + 1][x ] = 1;
      lcd[y + 1][x + 1] = 1;
      lcd[y + 1][x + 2] = 1;
      lcd[y][x ] = 0;
      lcd[y][x + 1] = 0;
      lcd[y - 1][x + 2] = 0;
      y++;
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
    if (!(lcd[y][x + 2] || lcd[y - 1][x + 2]) && y != 11) {
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
    if (!(lcd[y - 1][x] || lcd[y - 2][x]) && y != 11) {
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
    if (!(lcd[y + 1][x] || lcd[y + 1][x + 1] || lcd[y + 1][x + 2]) && y != 10) {
      lcd[y + 1][x] = 1;
      lcd[y + 1][x + 1] = 1;
      lcd[y + 1][x + 2] = 1;
      lcd[y][x] = 0;
      lcd[y - 1][x + 1] = 0;
      lcd[y][x + 2] = 0;
      y++;
      return 0;
    }
    else return 1;
  }
  else if (mode == 1) {
    if (!(lcd[y + 2][x + 1] || lcd[y + 1][x + 2]) &&  y != 9) {
      lcd[y + 2][x + 1 ] = 1;
      lcd[y + 1][x + 2] = 1;
      lcd[y - 1][x + 1] = 0;
      lcd[y][x + 2] = 0;
      y++;
      return 0;
    }
    else return 1;
  }
  else if (mode == 2) {
    if (!(lcd[y + 1][x ] || lcd[y + 2][x + 1] || lcd[y + 1][x + 2]) &&  y != 9) {
      lcd[y + 1][x ]  = 1;
      lcd[y + 2][x + 1] = 1;
      lcd[y + 1][x + 2] = 1;
      lcd[y][x] = 0;
      lcd[y][x + 1] = 0;
      lcd[y][x + 2] = 0;
      y++;
      return 0;
    }
    else return 1;
  }
  else if (mode == 3) {
    if (!(lcd[y + 1][x] || lcd[y + 2][x + 1]) &&  y != 9) {
      lcd[y + 1][x ] = 1;
      lcd[y + 2][x + 1] = 1;
      lcd[y][x ] = 0;
      lcd[y - 1][x + 1] = 0;
      y++;
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
    if (!(lcd[y + 1][x + 1]) && y != 10) {
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
