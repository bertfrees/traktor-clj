#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <list>
#include <vector>
#include <sys/time.h>
#include <rtmidi/RtError.h>
#include <rtmidi/RtMidi.h>
#include <traktor/midi.h>
#include <traktor/midi/utils.h>

/* ============
 * MASTER TEMPO
 * ============ */

#define BUFFER_SIZE 100

static RtMidiIn *midi_in = NULL;
static std::list<void (*)(double)> callbacks;
static std::vector<timeval> buffer;
static unsigned int slot = 0;

static void _master_tempo(double delta_time, std::vector<unsigned char> *message, void *user_data) {
	static timeval timestamp;
	if ((MidiShortMessageStatus)midi_status((*message)[0]) == TIMING_CLOCK) {
		gettimeofday(&timestamp, NULL);
		if (slot >= buffer.size())
			buffer.push_back(timestamp);
		else
			buffer[slot] = timestamp;
		if (buffer.size() == BUFFER_SIZE) {
			timeval t1 = buffer[(slot + 1) % BUFFER_SIZE];
			timeval t2 = buffer[slot];
			double bpm = (t2.tv_sec - t1.tv_sec) * 1000.0;
			bpm += (t2.tv_usec - t1.tv_usec) / 1000.0;
			bpm = ((60000.0 / 24) * (BUFFER_SIZE - 1)) / bpm;
			for (std::list<void (*)(double)>::iterator it = callbacks.begin(); it != callbacks.end(); it++)
				(*it)(bpm); }
		slot = (slot + 1) % BUFFER_SIZE; }
}

static void _master_tempo_cancel(ffi_cif *cif, void *ret, void **args, void *user_data) {
	void **data = (void **)user_data;
	void (*callback)(double) = (void (*)(double))data[0];
	callbacks.remove(callback);
	if (callbacks.size() == 0 && midi_in != NULL) {
		midi_in->cancelCallback();
		delete midi_in;
		midi_in = NULL;
		buffer.clear();
		slot = 0; }
}

void (*master_tempo(void (*callback)(double)))() {
	callbacks.push_back(callback);
	if (midi_in == NULL) {
		midi_in = traktor_midi_in();
		if (midi_in != NULL) {
			try {
				midi_in->setCallback(&_master_tempo); }
			catch (RtError &error) {
				delete midi_in;
				midi_in = NULL; }}}
	ALLOC_CLOSURE(cancel, _master_tempo_cancel, 1, callback)
	return cancel;
}
