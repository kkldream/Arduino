int tone_table[14] = {
  262, 294, 330, 349, 392, 440, 494, 523, 587, 659, 698, 784, 880, 988
};
byte tetris_tone[38] = {
  6, 2, 3, 4, 3, 2, 1, 1, 3, 5, 4, 3, 2, 2, 3, 4, 5, 3, 1, 1,
  5, 6, 7, 6, 5, 4, 3, 4, 3, 2, 1, 1, 2, 3, 4, 2, 0, 0
}, tetris_beat[38] = {
  4, 2, 2, 4, 2, 2, 4, 2, 2, 4, 2, 2, 4, 2, 2, 4, 4, 4, 4, 8,
  6, 2, 4, 2, 2, 6, 2, 4, 2, 2, 4, 2, 2, 4, 4, 4, 4, 8
};
void setup() {
}
void loop() {
  Bee();
}
byte beat;
unsigned long beat_millis;
void Bee() {
  if (millis() - beat_millis > tetris_beat[beat - 1] * 120) {
    beat_millis = millis();
    tone(2, tone_table[tetris_tone[beat] % 10], tetris_beat[beat] * 100);
    beat = beat == 38 ? 0 : ++beat;
  }
}
