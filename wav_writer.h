#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <math.h>

#define SAMPLE_RATE 44100
#define INVERSE_SAMPLE_RATE (1.0f / 44100.0f)
#define BIT_DEPTH 16
#define NUM_CHANNELS 1
#define DATA_LENGTH_SECONDS 10
#define WAVE_HEADER_LENGTH 44
#define TWO_PI 6.283185f
#define BUFFER_SIZE (SAMPLE_RATE * DATA_LENGTH_SECONDS)

typedef struct {
    char riff[4]; 
    int32_t f_length;
    char wave[4];
    char fmt[4];
    int32_t chunk_size;
    int16_t format_tag;
    int16_t num_channels;
    int32_t sample_rate;
    int32_t bytes_per_second;
    int16_t bytes_per_sample;
    int16_t bits_per_sample;
    char data[4];
    int32_t d_length;
} wav_header;

int init_wav_header(wav_header* header); 

int write_wav_file(FILE *file, float audio_buffer[][NUM_CHANNELS]);
