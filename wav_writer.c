#include "wav_writer.h"

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
    
    header->d_length = BUFFER_SIZE * header->bytes_per_sample;
    header->f_length = header->d_length + WAVE_HEADER_LENGTH - 8;
    
    return 0;
}

int write_wav_file(FILE *file, float audio_buffer[][NUM_CHANNELS]){
    
    //Create and initialise the header.
    wav_header header;
    if (init_wav_header(&header) != 0) {
        fprintf(stderr, "Failed to initialize WAV header\n");
        return 1;
    }

    int16_t converted_buffer[BUFFER_SIZE][NUM_CHANNELS];
    //Convert float samples to int16_t PCM data
    for(size_t i = 0; i < NUM_CHANNELS; ++i) {
        for(size_t j = 0; j < BUFFER_SIZE; ++j){
            float sample = audio_buffer[i][j];
            float clamped = (sample < -1.0f) ? -1.0f : (sample > 1.0f) ? 1.0f : sample;
            converted_buffer[i][j] = (int16_t)(clamped * 32767.0f);
        }
    }

    size_t wrote = fwrite(&header, sizeof(header), 1, file);
    if (wrote != 1) { perror("fwrite"); return 1; }

    wrote = fwrite(&converted_buffer, sizeof(converted_buffer), 1, file);
    if (wrote != 1) { perror("fwrite"); return 1; }
    if (fclose(file) != 0) { perror("fclose"); return 1; }

    return 0;
}