#ifdef __cplusplus
extern "C" {
#endif

#define DECK_A 0
#define DECK_B 1
#define DECK_C 2
#define DECK_D 3

void (*master_tempo(void (*callback)(double)))();
void (*track_title(unsigned char deck, void (*callback)(char)))();
void (*track_artist(unsigned char deck, void (*callback)(char)))();

#ifdef __cplusplus
}
#endif

