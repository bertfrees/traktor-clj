#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <list>
#include <vector>
#include <rtmidi/RtError.h>
#include <rtmidi/RtMidi.h>
#include <traktor/midi.h>
#include <traktor/midi/utils.h>

#define NUMBER_OF_DECKS 4

/* ==================
 * TRACK TITLE STREAM
 * ================== */

static RtMidiIn *midi_in_title[NUMBER_OF_DECKS];
static std::list<void (*)(char)> callbacks_title[NUMBER_OF_DECKS];

static void _track_title(double delta_time, std::vector<unsigned char> *message, void *user_data) {
	static unsigned char position[NUMBER_OF_DECKS];
	static char segment[NUMBER_OF_DECKS];
	static std::list<void (*)(char)>::iterator it;
	MidiShortMessage msg = midi_parse_message(message);
	if ((MidiShortMessageStatus)msg.status == CONTROL_CHANGE) {
		void **data = (void **)user_data;
		unsigned char deck = *((unsigned char *)data[0]);
		if (deck == msg.channel) {
			if (msg.data[0] <= 0x0D || msg.data[0] >= 0x0E && msg.data[0] <= 0x19) {
				if (msg.data[0] < position[deck]) {
					for (it = callbacks_title[deck].begin(); it != callbacks_title[deck].end(); it++)
						(*it)(segment[deck]); }
				position[deck] = (char)msg.data[0]; }
			switch (msg.data[0]) {
			case 0x01:
				segment[deck] = msg.data[1] << 4;
				break;
			case 0x21:
				segment[deck] += msg.data[1]; }}}
}

static void _track_title_cancel(ffi_cif *cif, void *ret, void **args, void *user_data) {
	void **data = (void **)user_data;
	unsigned char deck = *((unsigned char *)data[0]);
	void (*callback)(char) = (void (*)(char))data[1];
	callbacks_title[deck].remove(callback);
	if (callbacks_title[deck].size() == 0 && midi_in_title[deck] != NULL) {
		midi_in_title[deck]->cancelCallback();
		delete midi_in_title[deck];
		midi_in_title[deck] = NULL; }
}

void (*track_title(unsigned char deck, void (*callback)(char)))() {
	if (deck >= NUMBER_OF_DECKS)
		return NULL;
	callbacks_title[deck].push_back(callback);
	if (midi_in_title[deck] == NULL) {
		midi_in_title[deck] = traktor_midi_in();
		if (midi_in_title[deck] != NULL) {
			try {
				midi_in_title[deck]->setCallback(&_track_title,
				                                 alloc_user_data(1, new unsigned char(deck))); }
			catch (RtError &error) {
				delete midi_in_title[deck];
				midi_in_title[deck] = NULL; }}}
	ALLOC_CLOSURE(cancel, _track_title_cancel, 2, new unsigned char(deck), callback)
	return cancel;
}

/* ===================
 * TRACK ARTIST STREAM
 * =================== */

static RtMidiIn *midi_in_artist[NUMBER_OF_DECKS];
static std::list<void (*)(char)> callbacks_artist[NUMBER_OF_DECKS];

static void _track_artist(double delta_time, std::vector<unsigned char> *message, void *user_data) {
	static unsigned char position[NUMBER_OF_DECKS];
	static char segment[NUMBER_OF_DECKS];
	static std::list<void (*)(char)>::iterator it;
	MidiShortMessage msg = midi_parse_message(message);
	if ((MidiShortMessageStatus)msg.status == CONTROL_CHANGE) {
		void **data = (void **)user_data;
		unsigned char deck = *((unsigned char *)data[0]);
		if (deck == msg.channel) {
			if (msg.data[0] <= 0x0D || msg.data[0] >= 0x0E && msg.data[0] <= 0x19) {
				if (msg.data[0] < position[deck]) {
					for (it = callbacks_artist[deck].begin(); it != callbacks_artist[deck].end(); it++)
						(*it)(segment[deck]); }
				position[deck] = (char)msg.data[0]; }
			switch (msg.data[0]) {
			case 0x0E:
				segment[deck] = msg.data[1] << 4;
				break;
			case 0x2E:
				segment[deck] += msg.data[1]; }}}
}

static void _track_artist_cancel(ffi_cif *cif, void *ret, void **args, void *user_data) {
	void **data = (void **)user_data;
	unsigned char deck = *((unsigned char *)data[0]);
	void (*callback)(char) = (void (*)(char))data[1];
	callbacks_artist[deck].remove(callback);
	if (callbacks_artist[deck].size() == 0 && midi_in_artist[deck] != NULL) {
		midi_in_artist[deck]->cancelCallback();
		delete midi_in_artist[deck];
		midi_in_artist[deck] = NULL; }
}

void (*track_artist(unsigned char deck, void (*callback)(char)))() {
	if (deck >= NUMBER_OF_DECKS)
		return NULL;
	callbacks_artist[deck].push_back(callback);
	if (midi_in_artist[deck] == NULL) {
		midi_in_artist[deck] = traktor_midi_in();
		if (midi_in_artist[deck] != NULL) {
			try {
				midi_in_artist[deck]->setCallback(&_track_artist,
				                                  alloc_user_data(1, new unsigned char(deck))); }
			catch (RtError &error) {
				delete midi_in_artist[deck];
				midi_in_artist[deck] = NULL; }}}
	ALLOC_CLOSURE(cancel, _track_artist_cancel, 2, new unsigned char(deck), callback)
	return cancel;
}

