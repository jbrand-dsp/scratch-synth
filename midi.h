#include <CoreMIDI/CoreMIDI.h>
#include <stdint.h>

#define MIDI_HEADER_SIZE 14
#define MIDI_TRACK_HEADER_SIZE 8

typedef uint8_t MidiStatus;

typedef struct {
  char MThd[4]; // Chunk ID
  uint32_t header_length;
  uint16_t format;
  uint16_t num_tracks;
  uint16_t division;
} midi_header;

typedef struct {
  char MTrk[4]; // Chunk ID
  uint32_t track_length;
} track_header;

uint16_t swap16(uint16_t val) { return (val >> 8) | (val << 8); }

uint32_t swap32(uint32_t val) {
  return (val >> 24) | ((val >> 8) & 0x0000FF00) | ((val << 8) & 0x00FF0000) |
         (val << 24);
}

uint16_t read_uint16_be(const uint8_t *buf) {
  return ((uint16_t)buf[0] << 8) | ((uint16_t)buf[1]);
}

uint32_t read_uint32_be(const uint8_t *buf) {
  return ((uint32_t)buf[0] << 24) | ((uint32_t)buf[1] << 16) |
         ((uint32_t)buf[2] << 8) | ((uint32_t)buf[3]);
}

typedef enum {
  MIDI_VLQ_DATA_MASK = 0x7F,   // lower 7 bits contain the value
  MIDI_VLQ_CONTINUATION = 0x80 // MSB = 1 means more bytes follow
} midi_vlq_flags_t;

typedef struct {
  uint8_t event_type;
  uint8_t midi_channel;
  uint8_t param_1;
  uint8_t param_2;
} midi_event;

typedef enum {
  MIDI_NOTE_OFF = 0x80,
  MIDI_NOTE_ON = 0x90,
  MIDI_POLY_PRESSURE = 0xA0,
  MIDI_CONTROL_CHANGE = 0xB0,
  MIDI_PROGRAM_CHANGE = 0xC0,
  MIDI_CHANNEL_PRESS = 0xD0,
  MIDI_PITCH_BEND = 0xE0,
} midi_event_type;

void parseMidiFIle(FILE *file) {
  uint8_t buffer[MIDI_HEADER_SIZE];
  fread(buffer, 1, 14, file);

  // MIDI header is encoded in big endian hence the conversion to little endian
  midi_header h;
  memcpy(h.MThd, buffer, 4);
  h.header_length = read_uint32_be(buffer + 4);
  h.format = read_uint16_be(buffer + 8);
  h.num_tracks = read_uint16_be(buffer + 10);
  h.division = read_uint16_be(buffer + 12);

  // Parse first track header
  track_header t;
  uint8_t track_buffer[MIDI_TRACK_HEADER_SIZE];
  uint8_t stream[1];

  for (int i = 0; i < h.num_tracks; ++i) {
    fread(track_buffer, 1, 8, file);
    memcpy(t.MTrk, track_buffer, 4);
    t.track_length = read_uint32_be(track_buffer + 4);

    int bytes_remaining = t.track_length;
    int num_bytes_consumed_by_delta_time = 0;
    int num_bytes_consumed_by_event = 0;
    while (bytes_remaining != 0) {

      // read first bite
      fread(stream, 1, 1, file);
      int n = 0; // TODO: check if n always 0
      int d_time = 0;
      int value = 0;
      int byte = stream[n];
      value = byte & MIDI_VLQ_DATA_MASK;
      d_time = (d_time << 7) | value; // (d_time * 128) + value
      num_bytes_consumed_by_delta_time++;

      // continue reading if MSB == 1
      while (byte & MIDI_VLQ_CONTINUATION) {
        fread(stream, 1, 1, file);
        byte = stream[n];
        value = byte & MIDI_VLQ_DATA_MASK;
        d_time = (d_time << 7) | value; // (d_time * 128) + value
        num_bytes_consumed_by_delta_time++;
      }

      /**
      typedef struct {
        uint8_t event_type;
        uint8_t midi_channel;
        uint8_t param_1;
        uint8_t param_2;
      } midi_event;
       */

      // parse midi
      fread(stream, 1, 1, file);
      num_bytes_consumed_by_event++;
      byte = stream[0];
      uint8_t captured_midi_event = byte;
      uint8_t midi_channel = 0;
      uint8_t MIDI_CHANNEL_MASK = 0x0F;
      midi_event m;
      m.midi_channel = 0;
      m.param_1 = 0;
      m.param_2 = 0;

      if (captured_midi_event & MIDI_NOTE_OFF) {
        m.midi_channel = byte & MIDI_CHANNEL_MASK;
        fread(stream, 1, 1, file);
        num_bytes_consumed_by_event++;
        m.param_1 = stream[0];
        fread(stream, 1, 1, file);
        num_bytes_consumed_by_event++;
        m.param_2 = stream[0];
      }

      if (captured_midi_event & MIDI_NOTE_ON) {
        m.midi_channel = byte & MIDI_CHANNEL_MASK;
        fread(stream, 1, 1, file);
        num_bytes_consumed_by_event++;
        m.param_1 = stream[0];
        fread(stream, 1, 1, file);
        num_bytes_consumed_by_event++;
        m.param_2 = stream[0];
        break;
      }

      if (captured_midi_event & MIDI_POLY_PRESSURE) {
        m.midi_channel = byte & MIDI_CHANNEL_MASK;
        fread(stream, 1, 1, file);
        num_bytes_consumed_by_event++;
        m.param_1 = stream[0];
        fread(stream, 1, 1, file);
        num_bytes_consumed_by_event++;
        m.param_2 = stream[0];
        break;
      }

      if (captured_midi_event & MIDI_CONTROL_CHANGE) {
        m.midi_channel = byte & MIDI_CHANNEL_MASK;
        fread(stream, 1, 1, file);
        num_bytes_consumed_by_event++;
        m.param_1 = stream[0];
        fread(stream, 1, 1, file);
        num_bytes_consumed_by_event++;
        m.param_2 = stream[0];
        break;
      }

      if (captured_midi_event & MIDI_PROGRAM_CHANGE) {
        m.midi_channel = byte & MIDI_CHANNEL_MASK;
        fread(stream, 1, 1, file);
        num_bytes_consumed_by_event++;
        m.param_1 = stream[0];
        fread(stream, 1, 1, file);
        num_bytes_consumed_by_event++;
        m.param_2 = stream[0];
        break;
      }

      if (captured_midi_event & MIDI_CHANNEL_PRESS) {
        m.midi_channel = byte & MIDI_CHANNEL_MASK;
        fread(stream, 1, 1, file);
        num_bytes_consumed_by_event++;
        m.param_1 = stream[0];
        fread(stream, 1, 1, file);
        num_bytes_consumed_by_event++;
        m.param_2 = stream[0];
        break;
      }

      if (captured_midi_event & MIDI_PITCH_BEND) {
        m.midi_channel = byte & MIDI_CHANNEL_MASK;
        fread(stream, 1, 1, file);
        num_bytes_consumed_by_event++;
        m.param_1 = stream[0];
        fread(stream, 1, 1, file);
        num_bytes_consumed_by_event++;
        m.param_2 = stream[0];
        break;
      }

      bytes_remaining -= num_bytes_consumed_by_delta_time;
      bytes_remaining -= num_bytes_consumed_by_event;
    }
  }
}

/**
 *
 * parseFile(){
 *
 * -stream
 *
 * lamda
 * -swap byte order
 *
 * lamda
 * -readstring
 *
 * lamda
 * -readvalue
 *  check most significant bit while MSB set
 *
 * header:
 *
 *
 *  while not eof
 *
 *  for each track log event types
 *
 *
 * }
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 *
 * **/