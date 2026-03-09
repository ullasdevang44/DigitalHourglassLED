#include <LedControl.h>

// ---------------- MATRIX INDICES ----------------
#define MATRIX_TOP     1
#define MATRIX_BOTTOM  0

// ---------------- PIN DEFINITIONS ----------------
#define PIN_DATAIN 3
#define PIN_CLK    5
#define PIN_LOAD   4
#define PIN_BUZZER 6
#define PIN_TILT   2

// ---------------- TIMING ----------------
#define DELAY_FRAME  50
#define DELAY_DROP   600
#define ROTATION_OFFSET 90

LedControl lc(PIN_DATAIN, PIN_CLK, PIN_LOAD, 2);

bool ledState[2][8][8] = {{{false}}};
int currentGravity = 0;
int current_rotation = 0;

struct coord { byte x, y; };

// ---------------- SOUND ----------------
inline void playTone(int f, int d) {
  tone(PIN_BUZZER, f, d);
}

void playFlowStartSound() {
  playTone(600, 40); delay(50);
  playTone(800, 40); delay(50);
  playTone(1000, 80);
}

void playCompletionSound() {
  playTone(1000, 150); delay(180);
  playTone(800, 150);  delay(180);
  playTone(1200, 350);
}

// ---------------- ROTATION ----------------
coord getPhysicalCoord(int rot, byte x, byte y) {
  switch (rot % 360) {
    case 0:   return {x, y};
    case 90:  return {y, (byte)(7 - x)};
    case 180: return {(byte)(7 - x), (byte)(7 - y)};
    case 270: return {(byte)(7 - y), x};
  }
  return {x, y};
}

coord getLogicalCoord(int rot, byte x, byte y) {
  return getPhysicalCoord((360 - rot) % 360, x, y);
}

// ---------------- LED CONTROL ----------------
void setLed(int d, int x, int y, bool v) {
  coord p = getPhysicalCoord(current_rotation, x, y);
  lc.setLed(d, p.y, p.x, v);
  ledState[d][x][y] = v;
}

bool getLed(int d, int x, int y) {
  return ledState[d][x][y];
}

// ---------------- REMAP ----------------
void remapState(int oldR, int newR) {
  bool tmp[2][8][8] = {{{false}}};

  for (int d = 0; d < 2; d++)
    for (byte x = 0; x < 8; x++)
      for (byte y = 0; y < 8; y++) {
        coord p = getPhysicalCoord(newR, x, y);
        coord o = getLogicalCoord(oldR, p.x, p.y);
        tmp[d][x][y] = ledState[d][o.x][o.y];
      }

  memcpy(ledState, tmp, sizeof(tmp));

  for (int d = 0; d < 2; d++)
    for (byte x = 0; x < 8; x++)
      for (byte y = 0; y < 8; y++) {
        coord p = getPhysicalCoord(newR, x, y);
        lc.setLed(d, p.y, p.x, tmp[d][x][y]);
      }
}

// ---------------- PARTICLES ----------------
coord down (int x, int y) { return {(byte)(x - 1), (byte)(y + 1)}; }
coord left (int x, int y) { return {(byte)(x - 1), y}; }
coord right(int x, int y) { return {x, (byte)(y + 1)}; }

bool canDown (int d, int x, int y) { return x > 0 && y < 7 && !getLed(d, x - 1, y + 1); }
bool canLeft (int d, int x, int y) { return x > 0 && !getLed(d, x - 1, y); }
bool canRight(int d, int x, int y) { return y < 7 && !getLed(d, x, y + 1); }

bool moveParticle(int d, int x, int y) {
  if (!getLed(d, x, y)) return false;

  if (canDown(d, x, y)) {
    setLed(d, x, y, false);
    coord p = down(x, y);
    setLed(d, p.x, p.y, true);
  }
  else if (canLeft(d, x, y) && !canRight(d, x, y)) {
    setLed(d, x, y, false);
    setLed(d, x - 1, y, true);
  }
  else if (canRight(d, x, y) && !canLeft(d, x, y)) {
    setLed(d, x, y, false);
    setLed(d, x, y + 1, true);
  }
  else if (canLeft(d, x, y) && canRight(d, x, y)) {
    random(2) ? setLed(d, x - 1, y, true) : setLed(d, x, y + 1, true);
    setLed(d, x, y, false);
  }
  else return false;

  return true;
}

// ---------------- MATRIX UPDATE ----------------
bool updateMatrix() {
  bool moved = false;

  for (int s = 0; s < 15; s++) {
    bool dir = random(2);
    int z = s < 8 ? 0 : s - 7;

    for (int j = z; j <= s - z; j++) {
      byte x = dir ? (s - j) : j;
      byte y = dir ? (7 - j) : (7 - (s - j));
      moved |= moveParticle(MATRIX_TOP, x, y);
      moved |= moveParticle(MATRIX_BOTTOM, x, y);
    }
  }
  return moved;
}

// ---------------- DROP ----------------
bool dropParticle() {
  static unsigned long lastDrop = 0;
  static bool flowStarted = false;

  if (millis() - lastDrop < DELAY_DROP) return false;
  lastDrop = millis();

  coord a = getLogicalCoord(current_rotation, 0, 0);
  coord b = getLogicalCoord(current_rotation, 7, 7);

  bool a_on = getLed(MATRIX_TOP, a.x, a.y);
  bool b_on = getLed(MATRIX_BOTTOM, b.x, b.y);

  if (a_on != b_on) {
    if (!flowStarted) {
      playFlowStartSound();
      flowStarted = true;
    }
    setLed(MATRIX_TOP, a.x, a.y, !a_on);
    setLed(MATRIX_BOTTOM, b.x, b.y, !b_on);
    playTone(600, 18);  // droplet
    return true;
  }

  flowStarted = false;
  return false;
}

// ---------------- UTIL ----------------
int getGravity() {
  return digitalRead(PIN_TILT) == LOW ? 0 : 180;
}

int getTopMatrix() {
  return currentGravity == 0 ? MATRIX_TOP : MATRIX_BOTTOM;
}

int countParticles(int d) {
  int c = 0;
  for (byte x = 0; x < 8; x++)
    for (byte y = 0; y < 8; y++)
      if (getLed(d, x, y)) c++;
  return c;
}

void fill(int d, int n) {
  int c = 0;
  for (int s = 0; s < 15; s++) {
    int z = s < 8 ? 0 : s - 7;
    for (int j = z; j <= s - z; j++)
      if (++c <= n)
        setLed(d, s - j, 7 - j, true);
  }
}

// ---------------- SETUP ----------------
void setup() {
  pinMode(PIN_TILT, INPUT_PULLUP);
  randomSeed(analogRead(A0));

  for (byte i = 0; i < 2; i++) {
    lc.shutdown(i, false);
    lc.setIntensity(i, 4);
    lc.clearDisplay(i);
  }

  fill(MATRIX_TOP, 63);
}

// ---------------- LOOP ----------------
void loop() {
  static unsigned long lastFrame = 0;
  static bool completionPlayed[2] = {false, false};

  int oldGravity = currentGravity;
  currentGravity = getGravity();
  current_rotation = (ROTATION_OFFSET + currentGravity) % 360;

  if (oldGravity != currentGravity) {
    remapState((ROTATION_OFFSET + oldGravity) % 360, current_rotation);
    completionPlayed[0] = completionPlayed[1] = false;
  }

  if (millis() - lastFrame >= DELAY_FRAME) {
    lastFrame = millis();

    dropParticle();
    updateMatrix();

    int top = getTopMatrix();
    if (countParticles(top) == 63 && !completionPlayed[top]) {
      playCompletionSound();
      completionPlayed[top] = true;
    }
  }
}
