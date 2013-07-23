CC        := g++
LD        := g++
CFLAGS    := -c -Wall -g -fPIC
LDFLAGS   := -L/usr/local/opt/libffi/lib -lffi -framework CoreMIDI -framework CoreAudio -framework CoreFoundation
SRC_DIR   := src/main/c
TEST_DIR  := src/test/c
BUILD_DIR := target/native/macosx/x86_64
OBJ       := $(addprefix $(SRC_DIR)/, traktor/midi/tempo.o  \
                                      traktor/midi/utils.o  \
                                      rtmidi/RtMidi.o)
INCLUDES  := -I$(SRC_DIR) -I/usr/local/opt/libffi/lib/libffi-3.0.11/include

all: $(BUILD_DIR)/libtraktor_midi.dylib

$(BUILD_DIR)/libtraktor_midi.dylib: $(OBJ)
	mkdir -p $(BUILD_DIR)
	$(LD) $(LDFLAGS) -dynamiclib $^ -o $@

%.o: %.cpp
	$(CC) $(CFLAGS) $(INCLUDES) -D__MACOSX_CORE__ $< -o $@

clean:
	rm -rf $(SRC_DIR)/rtmidi/*.o         \
       $(SRC_DIR)/traktor/midi/*.o   \
       $(BUILD_DIR)
