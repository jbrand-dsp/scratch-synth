// #define TWO_PI 6.283185f

// // Sin generation
//   float audio_buffer[BUFFER_SIZE][NUM_CHANNELS];

//   const float frequency = 440.0f;
//   const float amplitude = 0.4f;
//   const float p_increment = TWO_PI * frequency * INVERSE_SAMPLE_RATE;

//   float phase = 0;
//   float sample = 0;

//   for (size_t i = 0; i < NUM_CHANNELS; ++i) {
//     for (size_t j = 0; j < BUFFER_SIZE; ++j) {
//       sample = sin(phase) * amplitude;
//       audio_buffer[i][j] = sample;
//       phase += p_increment;
//       if (phase >= TWO_PI)
//         phase -= TWO_PI;
//     }