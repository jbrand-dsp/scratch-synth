#include <CoreMIDI/CoreMIDI.h>
#include <stdint.h>

#define MIDI_HEADER_SIZE 14

typedef uint8_t MidiStatus;

typedef struct {
  char MThd[4]; // Chunk ID
  uint32_t header_length;
  uint16_t format;
  uint16_t num_tracks;
  uint16_t division;
} midi_header;

uint16_t swap16(uint16_t val) { return (val >> 8) | (val << 8); }

uint32_t swap32(uint32_t val) {
  return (val >> 24) | ((val >> 8) & 0x0000FF00) | ((val << 8) & 0x00FF0000) |
         (val << 24);
}

midi_header parseMidiHeader(FILE *file) {
  uint8_t buffer[MIDI_HEADER_SIZE];
  fread(buffer, 1, 14, file);

  midi_header h;
  memcpy(h.MThd, buffer, 4);
  h.header_length = swap32(*(uint32_t *)(buffer + 4));
  h.format = swap16(*(uint16_t *)(buffer + 8));
  h.num_tracks = swap16(*(uint16_t *)(buffer + 10));
  h.division = swap16(*(uint16_t *)(buffer + 12));

  return h;
}

enum {
  MIDI_NOTE_OFF = 0x80,
  MIDI_NOTE_ON = 0x90,
  MIDI_POLY_PRESSURE = 0xA0,
  MIDI_CONTROL_CHANGE = 0xB0,
  MIDI_PROGRAM_CHANGE = 0xC0,
  MIDI_CHANNEL_PRESS = 0xD0,
  MIDI_PITCH_BEND = 0xE0,
};

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