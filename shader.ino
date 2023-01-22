#include "Adafruit_Arcada.h"
#include <math.h>

Adafruit_Arcada arcada;
uint16_t *framebuffer;
uint16_t counter = 0;
float speed = .1f;
float ratio = (float)ARCADA_TFT_WIDTH / ARCADA_TFT_HEIGHT;
float square = min(ARCADA_TFT_WIDTH, ARCADA_TFT_HEIGHT);
uint16_t mode = 0;
#define MODES 4

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

// Credits: Danilo Guanabara, Creation by Silexars, https://www.shadertoy.com/view/XsXXDn
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

// Credits: dynamite, Total Noob, https://www.shadertoy.com/view/XdlSDs
uint16_t render_noob(uint16_t x, uint16_t y, uint16_t t) {
  float px = (2.f * x - ARCADA_TFT_WIDTH) / ARCADA_TFT_HEIGHT;
  float py = (2.f * y - ARCADA_TFT_HEIGHT) / ARCADA_TFT_HEIGHT;
  float uv = atan2(py, px) / 6.283f;
  float m = uv - t / 30.f;
  float col = (m - floor(m)) * 3.f;
  float r = col < 1.f ? 1.25f - col : (col < 2.f ? .25f : col - 1.75f);
  float g = col < 1.f ? .25f + col : (col < 2.f ? 2.25f - col : 0.25f);
  float b = col < 1.f ? .25f : (col < 2.f ? col - .75f : 3.25f - col);
  float width = 32.f * (.7f + .5f * cosf((2.f * uv - 1.f) * 9.425f * min(max(floor(5.f + 10.f * cosf(t / 10.f)), 0.f), 10.f))) * abs(1.f / (45.f * sqrt(px * px + py * py) - 30.f));
  return (int)min(width * r, 31.f) << 11 | (int)min(width * g, 31.f) << 6 | (int)min(width * b, 31.f);
}

// Credits: nasana, glowingMarblingBlack, https://www.shadertoy.com/view/WtdXR8
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


// Credits: nightjar, cube ray trace, https://www.shadertoy.com/view/clsXzn
uint16_t render_cube(uint16_t x, uint16_t y, float ex, float ey, float ez, float ux, float uy, float uz, float vx, float vy, float vz) {
  float uvx = (float)x / ARCADA_TFT_WIDTH - .5f;
  float uvy = (float)y / ARCADA_TFT_WIDTH - .5f / ratio;
  float dx = ex + uvx * ux + uvy * vx;
  float dy = ey + uvx * uy + uvy * vy;
  float dz = ez + uvx * uz + uvy * vz;
  float ax = (ex + .2f) / dx;
  float ay = (ey + .2f) / dy;
  float az = (ez + .2f) / dz;
  float bx = (ex - .2f) / dx;
  float by = (ey - .2f) / dy;
  float bz = (ez - .2f) / dz;
  float mi = max(max(min(ax, bx), min(ay, by)), min(az, bz));
  float ma = min(min(max(ax, bx), max(ay, by)), max(az, bz));
  float tt = mi < ma ? mi : 100.f;
  float ix = ex - dx * tt;
  float iy = ey - dy * tt;
  float iz = ez - dz * tt;
  float r = ix < -.199f ? -dx : (ix < .199f ? 0 : dx);
  float g = iy < -.199f ? -dy : (iy < .199f ? 0 : dy);
  float b = iz < -.199f ? -dz : (iz < .199f ? 0 : dz);
  return min(max((int)(r * 32.f), 0), 31) << 11 | min(max((int)(g * 32.f), 0), 31) << 6 | min(max((int)(b * 32.f), 0), 31);
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
  } else if (mode == 3) {
    sensors_event_t event;
    arcada.accel->getEvent(&event);
    float length = sqrt(event.acceleration.x * event.acceleration.x + event.acceleration.y * event.acceleration.y + event.acceleration.z * event.acceleration.z);
    float ex = event.acceleration.x / length;
    float ey = event.acceleration.y / length;
    float ez = -event.acceleration.z / length;
    float ll = 1.f + ez;
    float ux = ex * ex / ll - 1.f;
    float uy = ey * ex / ll;
    float uz = ex;
    float vx = uy;
    float vy = -ux - ll;
    float vz = ey;
    for (uint16_t y = 0, i = 0; y < ARCADA_TFT_HEIGHT; y++) {
      for (uint16_t x = 0; x < ARCADA_TFT_WIDTH; x++, i++) {
        framebuffer[i] = render_cube(x, y, ex, ey, ez, ux, uy, uz, vx, vy, vz);
      }
    }
  }
  arcada.blitFrameBuffer(0, 0, false, false);
  counter++;
  uint32_t buttons = arcada.justPressedButtons();
  if (buttons & ARCADA_BUTTONMASK_RIGHT) {
    mode = (mode + 1) % MODES;
    counter = 0;
  } else if (buttons & ARCADA_BUTTONMASK_LEFT) {
    mode = (mode + MODES - 1) % MODES;
    counter = 0;
  } else if (buttons & ARCADA_BUTTONMASK_B) {
    Serial.println(1000.f / (millis() - start));
  }
}
