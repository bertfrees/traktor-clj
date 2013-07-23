#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <vector>
#include <rtmidi/RtError.h>
#include <rtmidi/RtMidi.h>
#include <traktor/midi/utils.h>

/* ====
 * MIDI
 * ==== */

unsigned char midi_status(unsigned char status_byte) {
	unsigned char high_nibble = (status_byte & 0xF0);
	return (high_nibble == 0xF0 ? status_byte : high_nibble);
}

MidiShortMessage midi_parse_message(std::vector<unsigned char>* message_bytes) {
	MidiShortMessage msg = {};
	msg.status = midi_status((*message_bytes)[0]);
	if (msg.status < 0xF0)
		msg.channel = ((*message_bytes)[0] & 0x0F);
	for (int i = 1; i < message_bytes->size(); i++)
		msg.data[i-1] = (*message_bytes)[i];
	return msg;
}

RtMidiIn *traktor_midi_in() {
	try {
		RtMidiIn *midi_in = new RtMidiIn();
		try {
			midi_in->ignoreTypes(false, false, false);
			for (unsigned int i = 0; i < midi_in->getPortCount(); i++) {
				if (midi_in->getPortName(i) == "Traktor Virtual Output") {
					midi_in->openPort(i);
					return midi_in; }}}
		catch (RtError &error) {}
		delete midi_in; }
	catch (RtError &error) {}
	return NULL;
}

/* ========
 * CLOSURES
 * ======== */

void **alloc_user_data(int n_var, ...) {
	va_list variables;
	void **user_data = (void **)malloc(sizeof(void *) * n_var);
	va_start(variables, n_var);
	for (int i = 0; i < n_var; i++)
		user_data[i] = va_arg(variables, void *);
	va_end(variables);
	return user_data;
}
