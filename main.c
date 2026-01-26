#include "synth.h"
#include "wav_writer.h"
#include <math.h>
#include <raylib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#define TWO_PI 6.283185f

int main() {

  InitWindow(800, 600, "Synth");
  SetTargetFPS(60);

  while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(BLACK);
    DrawText("Hello", 10, 10, 20, WHITE);
    EndDrawing();
  }

  CloseWindow();

  return 0;
}