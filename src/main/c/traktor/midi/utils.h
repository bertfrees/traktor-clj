#include <ffi.h>

/* ====
 * MIDI
 * ==== */

typedef enum {
	NOTE_OFF =              0x80,
	NOTE_ON =               0x90,
	POLY_PRESSURE =         0xA0,
	CONTROL_CHANGE =        0xB0,
	PROGRAM_CHANGE =        0xC0,
	CHANNEL_PRESSURE =      0xD0,
	PITCH_BEND =            0xE0,
	MIDI_TIME_CODE =        0xF1,
	SONG_POSITION_POINTER = 0xF2,
	SONG_SELECT =           0xF3,
	TUNE_REQUEST =          0xF6,
	END_OF_EXCLUSIVE =      0xF7,
	TIMING_CLOCK =          0xF8,
	START =                 0xFA,
	CONTINUE =              0xFB,
	STOP =                  0xFC,
	ACTIVE_SENSING =        0xFE,
	SYSTEM_RESET =          0xFF
} MidiShortMessageStatus;

typedef struct {
	double timestamp;
	unsigned char status;
	unsigned char channel;
	unsigned char data[2];
} MidiShortMessage;

unsigned char midi_status(unsigned char status_byte);

MidiShortMessage midi_parse_message(std::vector<unsigned char>* message);

RtMidiIn *traktor_midi_in();

/* ========
 * CLOSURES
 * ======== */

void **alloc_user_data(int n_var, ...);

#define ALLOC_CLOSURE(name, func, n_var, ...)                                \
	static ffi_cif name##_cif;                                               \
	static bool name##_prep_cif_done = false;                                \
	if (!name##_prep_cif_done) {                                             \
		ffi_prep_cif(&name##_cif, FFI_DEFAULT_ABI, 0, &ffi_type_void, NULL); \
		name##_prep_cif_done = true; }                                       \
	void (*name)();                                                          \
	ffi_closure *name##_closure = (ffi_closure *)                            \
		ffi_closure_alloc(sizeof(ffi_closure), (void **)&name);              \
	ffi_prep_closure_loc(name##_closure, &name##_cif, func,                  \
	                     alloc_user_data(n_var, __VA_ARGS__ ), (void *)name);
