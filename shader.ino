#include "Adafruit_Arcada.h"
#include <math.h>

Adafruit_Arcada arcada;
uint16_t *framebuffer;
uint16_t counter = 0;
float speed = .1f;
float ratio = (float)ARCADA_TFT_WIDTH / ARCADA_TFT_HEIGHT;
float square = min(ARCADA_TFT_WIDTH, ARCADA_TFT_HEIGHT);
uint16_t mode = 0;

float cosf(float x) {
  x *= 0.159f;
  x -= .25f + floor(x + .25f);
  x *= 16.f * (abs(x) - .5f);
  return x;
}

float sinf(float x) {
  x *= 0.159f;
  x -= .5f + floor(x);
  x *= 16.f * (abs(x) - .5f);
  return x;
}

void setup(void) {
  Serial.begin(9600);
  if (!arcada.arcadaBegin()) {
    Serial.print("error arcada begin");
    while (1) {}
  }
  arcada.displayBegin();
  arcada.setBacklight(255);
  if (!arcada.createFrameBuffer(ARCADA_TFT_WIDTH, ARCADA_TFT_HEIGHT)) {
    Serial.print("error frame buffer");
    while (1) {}
  }
  framebuffer = arcada.getFrameBuffer();
}

// Credits: Danilo Guanabara, Creation by Silexars
uint16_t render_silexars(uint16_t x, uint16_t y, uint16_t t) {
  uint16_t c[3];
  float sx = (float)x / ARCADA_TFT_WIDTH;
  float sy = (float)y / ARCADA_TFT_HEIGHT;
  float px = (sx - .5f) * ratio;
  float py = sy - .5f;
  float z = t * speed + 4.f;
  float l = sqrt(px * px + py * py);
  for (int i = 0; i < 3; i++) {
    z += .07f;
    float f = (sinf(z) + 1.f) * abs(sinf(l * 9.f - z - z)) / l;
    float ux = sx + px * f;
    float uy = sy + py * f;
    float vx = ux - floor(ux) - .5f;
    float vy = uy - floor(uy) - .5f;
    c[i] = min(.32f / sqrt(vx * vx + vy * vy) / l, 31);
  }
  return c[0] << 11 | c[1] << 6 | c[2];
}

// Credits: dynamite, Total Noob
uint16_t render_noob(uint16_t x, uint16_t y, uint16_t t) {
  float px = (2.f * x - ARCADA_TFT_WIDTH) / ARCADA_TFT_HEIGHT;
  float py = (2.f * y - ARCADA_TFT_HEIGHT) / ARCADA_TFT_HEIGHT;
  float uv = atan2(py, px) / 6.283f;
  float m = uv - (t / 30.f);
  float col = (m - floor(m)) * 3.f;
  float r = .25f, g = .25f, b = .25f;
  if (col < 1.f) {
    r += 1.f - col;
    g += col;
  } else if (col < 2.f) {
    g += 2.f - col;
    b += col - 1.f;
  } else {
    b += 3.f - col;
    r += col - 2.f;
  }
  float width = 32.f * (.7f + .5f * cosf((2.f * uv - 1.f) * 9.425f * min(max(floor(5.f + 10.f * cosf(t / 10.f)), 0.f), 10.f))) * abs(1.f / (45.f * sqrt(px * px + py * py) - 30.f));
  return (int)min(width * r, 31.f) << 11 | (int)min(width * g, 31.f) << 6 | (int)min(width * b, 31.f);
}

// Credits: nasana, glowingMarblingBlack
uint16_t render_marbling(uint16_t x, uint16_t y, uint16_t t) {
  float uvx = (2.f * x - ARCADA_TFT_WIDTH) / square;
  float uvy = (2.f * y - ARCADA_TFT_HEIGHT) / square;
  float time = t * .1f;
  for (uint16_t i = 1; i < 5; i++) {
    uvx += .6f * cosf(i * 2.5f * uvy + time) / i;
    uvy += .6f * cosf(i * 1.5f * uvx + time) / i;
  }
  float v = 3.2f / abs(sinf(time - uvy - uvx));
  return (int)min(v, 31.f) << 11 | (int)min(v, 31.f) << 6 | (int)min(v, 31.f);
}

void loop() {
  uint32_t start = millis();
  arcada.readButtons();
  if (mode == 0) {
    for (uint16_t y = 0, i = 0; y < ARCADA_TFT_HEIGHT; y++) {
      for (uint16_t x = 0; x < ARCADA_TFT_WIDTH; x++, i++) {
        framebuffer[i] = render_silexars(x, y, counter);
      }
    }
  } else if (mode == 1) {
    for (uint16_t y = 0, i = 0; y < ARCADA_TFT_HEIGHT; y++) {
      for (uint16_t x = 0; x < ARCADA_TFT_WIDTH; x++, i++) {
        framebuffer[i] = render_noob(x, y, counter);
      }
    }
  } else if (mode == 2) {
    for (uint16_t y = 0, i = 0; y < ARCADA_TFT_HEIGHT; y++) {
      for (uint16_t x = 0; x < ARCADA_TFT_WIDTH; x++, i++) {
        framebuffer[i] = render_marbling(x, y, counter);
      }
    }
  }
  arcada.blitFrameBuffer(0, 0, false, false);
  counter++;
  uint32_t buttons = arcada.justPressedButtons();
  if (buttons & ARCADA_BUTTONMASK_A) {
    mode = (mode + 1) % 3;
    counter = 0;
  }
  if (buttons & ARCADA_BUTTONMASK_B) {
    Serial.println(1000.f / (millis() - start));
  }
}
