#include "synth.h"
#include "wav_writer.h"
#include "audio.h"
#include <math.h>
#include <raylib.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>



typedef struct {
  float _phase;
} Note;

static Note note;

void audio_callback(void *bufferData, unsigned int frames) {
  float *buffer = (float *)bufferData;

  const float frequency = 440.0f;
  const float amplitude = 0.4f;
  const float p_increment = TWO_PI * frequency * INVERSE_SAMPLE_RATE;

  for(unsigned int i = 0; i < frames; ++i) {
    float val = sinf(note._phase) * amplitude;
    buffer[i] = val;
    note._phase += p_increment;
    note._phase >= TWO_PI ? note._phase -= TWO_PI : note._phase; 
  }
}

int main() {
  note._phase = 0.f;

  InitWindow(800, 600, "Synth");
  InitAudioDevice();
  AudioStream stream = LoadAudioStream(44100, 32, 1);
  SetAudioStreamCallback(stream, audio_callback);
  PlayAudioStream(stream);
  SetTargetFPS(60);

  while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(BLACK);
    DrawText("Hello", 10, 10, 20, WHITE);
    EndDrawing();
  }

  UnloadAudioStream(stream);
  CloseAudioDevice();

  CloseWindow();

  return 0;
}

