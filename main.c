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

int init_wav_header(wav_header* header) {

    if(header == NULL){
        fprintf(stderr, "Error: header pointer is NULL\n");
        return 1;
    }

    strncpy(header->riff, "RIFF", 4);
    strncpy(header->wave, "WAVE", 4);
    strncpy(header->fmt, "fmt ", 4);
    strncpy(header->data, "data", 4);

    header->chunk_size = 16;
    header->format_tag = 1;
    header->num_channels = NUM_CHANNELS;
    header->sample_rate = SAMPLE_RATE;
    header->bits_per_sample = BIT_DEPTH;
    header->bytes_per_sample = (header->bits_per_sample) / 8 * header->num_channels;
    header->bytes_per_second = header->sample_rate * header->bytes_per_sample;
    
    const int buffer_size = header->sample_rate * DATA_LENGTH_SECONDS;

    header->d_length = buffer_size * header->bytes_per_sample;
    header->f_length = header->d_length + WAVE_HEADER_LENGTH - 8;

    return 0;
}


int main (){
    wav_header header;
    
    if (init_wav_header(&header) != 0) {
        fprintf(stderr, "Failed to initialize WAV header\n");
        return 1;
    }

    int buffer_size = SAMPLE_RATE * DATA_LENGTH_SECONDS;
    float audio_buffer[buffer_size][NUM_CHANNELS];

    const float frequency = 440.0f;
    const float amplitude = 0.4f;
    const float p_increment = TWO_PI * frequency * INVERSE_SAMPLE_RATE;
    
    float phase = 0;
    float sample = 0;

    for(size_t i = 0; i < NUM_CHANNELS; ++i) {
        for(size_t j = 0; j < buffer_size; ++j){
            sample = sin(phase) * amplitude;
            audio_buffer[i][j] = sample;
            phase += p_increment;
            if(phase >= TWO_PI) phase -= TWO_PI;
        }
    }

    int16_t converted_buffer[buffer_size][NUM_CHANNELS];
    //Convert float samples to int16_t PCM data
    for(size_t i = 0; i < NUM_CHANNELS; ++i) {
        for(size_t j = 0; j < buffer_size; ++j){
            sample = audio_buffer[i][j];
            float clamped = (sample < -1.0f) ? -1.0f : (sample > 1.0f) ? 1.0f : sample;
            converted_buffer[i][j] = (int16_t)(clamped * 32767.0f);
        }
    }

    FILE* file = fopen("out.wav", "wb");
    if(!file) { perror("fopen"); return 1; }

    size_t wrote = fwrite(&header, sizeof(header), 1, file);
    if (wrote != 1) { perror("fwrite"); return 1; }

    wrote = fwrite(&converted_buffer, sizeof(converted_buffer), 1, file);
    if (wrote != 1) { perror("fwrite"); return 1; }
    if (fclose(file) != 0) { perror("fclose"); return 1; }

    return 0;
}