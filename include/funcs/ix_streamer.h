/* ix/streamer.cpp */
int ix_streamer_init(void);                                           /* 0x00442750 */
void ix_streamer_destroy(void);                                      /* 0x0044286F */
void ix_streamer_configure(int option, void *value);                  /* 0x0044291E */
int ix_streamer_open_stream_file(char *path);                         /* 0x004429B6 */
void ix_streamer_close_stream_file(void);                             /* 0x0044307A */
void ix_streamer_audio_play(void);                                    /* 0x004431F3 */
void ix_streamer_audio_stop(void);                                    /* 0x00443253 */
void ix_streamer_set_intensity(unsigned char intensity);              /* 0x0044336B */
void ix_streamer_set_trigger(char trigger);                           /* 0x004433C1 */
char ix_streamer_get_trigger(void);                                   /* 0x00443419 */
void ix_streamer_force_trigger(char trigger);                         /* 0x0044342E */
void ix_streamer_set_volume(unsigned short volume);                   /* 0x004435BE */
unsigned short ix_streamer_get_volume(void);                          /* 0x0044363B */
