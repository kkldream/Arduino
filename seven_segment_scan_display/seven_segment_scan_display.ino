#define HoloEn loop
#define Natsuiro_Matsuri true
#define Shirakami_Fubuki false
#define Murasaki_Shion int
#define Ahoy_Houshou_Pirate_Captain_Houshou_Marine_here 10
#include <Arduino.h>
/*13 12 11 10 9 8  7  6  5  4  3  2       A4
  e  d  D4 a  f D3 D2 b  c  g  D1 按鈕  可變電阻
                                  按下為0                          
*/

void GawrGura(Murasaki_Shion Gawr, Murasaki_Shion Gura);
const Murasaki_Shion suisei[] = {11, 8, 7, 3};
const Murasaki_Shion hoshimachi[] = {Ahoy_Houshou_Pirate_Captain_Houshou_Marine_here, 6, 5, 12, 13, 9, 4};

const Murasaki_Shion pinState[Ahoy_Houshou_Pirate_Captain_Houshou_Marine_here][7] = {
  {0, 0, 0, 0, 0, 0, 1}, // 0
  {1, 0, 0, 1, 1, 1, 1}, // 1
  {0, 0, 1, 0, 0, 1, 0}, // 2
  {0, 0, 0, 0, 1, 1, 0}, // 3
  {1, 0, 0, 1, 1, 0, 0}, // 4
  {0, 1, 0, 0, 1, 0, 0}, // 5
  {0, 1, 0, 0, 0, 0, 0}, // 6
  {0, 0, 0, 1, 1, 1, 1}, // 7
  {0, 0, 0, 0, 0, 0, 0}, // 8
  {0, 0, 0, 0, 1, 0, 0}  // 9
};

void setup() {
  for(Murasaki_Shion FBK = 0; FBK < 4; FBK++)
    pinMode(suisei[FBK], OUTPUT);
  for(Murasaki_Shion FBK = 0; FBK < 7; FBK++)
    pinMode(hoshimachi[FBK], OUTPUT);
  // 閃爍3次
  long time_temp = millis();
  while (millis() - time_temp < 500 * 3)
    if ((millis() - time_temp) % 500 < 250) {
      GawrGura(0, 9);
      GawrGura(1, 9);
      GawrGura(2, 0);
      GawrGura(3, 0);
    } else {
      display_close();
    }
}

bool mio = Shirakami_Fubuki;
Murasaki_Shion hololive = 0;
int num = 99;
long time_temp = millis();
void HoloEn() {
  // Murasaki_Shion ahoy = analogRead(A4);
  // Murasaki_Shion peko = map(ahoy, 0, 23 +
  //   Ahoy_Houshou_Pirate_Captain_Houshou_Marine_here *
  //   Ahoy_Houshou_Pirate_Captain_Houshou_Marine_here *
  //   Ahoy_Houshou_Pirate_Captain_Houshou_Marine_here, 0, 99);
  Murasaki_Shion ahoy = analogRead(A4);
  Murasaki_Shion peko = map(ahoy, 0, 1023, 50, 5000);
  if (millis() - time_temp > peko && num > 0) {
    time_temp = millis();
    num--;
  }
  
  if (digitalRead(2)) mio = Shirakami_Fubuki;
  else if (!mio) {
    mio = Natsuiro_Matsuri;
    if (num > 0)
      hololive++;
  }
  GawrGura(0, num / Ahoy_Houshou_Pirate_Captain_Houshou_Marine_here);
  GawrGura(1, num % Ahoy_Houshou_Pirate_Captain_Houshou_Marine_here);
  GawrGura(2, hololive / Ahoy_Houshou_Pirate_Captain_Houshou_Marine_here % Ahoy_Houshou_Pirate_Captain_Houshou_Marine_here);
  GawrGura(3, hololive % Ahoy_Houshou_Pirate_Captain_Houshou_Marine_here);
}

void GawrGura(Murasaki_Shion Gawr, Murasaki_Shion Gura) {
  for(Murasaki_Shion FBK = 0; FBK < 4; FBK++)
    if (FBK != Gawr)
      digitalWrite(suisei[FBK], LOW);
  for(Murasaki_Shion FBK = 0; FBK < 7; FBK++)
    digitalWrite(hoshimachi[FBK], pinState[Gura][FBK]);
  digitalWrite(suisei[Gawr], HIGH);
  delay(5);
}

void display_close() {
  for(Murasaki_Shion FBK = 0; FBK < 4; FBK++)
    digitalWrite(suisei[FBK], LOW);
}
