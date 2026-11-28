/* ix/system.cpp */
int ix_system_init(void);                                              /* 0x00447200 */
void ix_system_service_sounds(void);                                     /* 0x004472A7 */
void ix_system_shutdown(void);                                            /* 0x004473F3 */
void ix_system_configure(int option, void *value);                     /* 0x00447426 */
void ix_system_set_master_volume(unsigned short volume);              /* 0x0044745B */
void ix_system_set_voice_count(int voiceCount);                        /* 0x0044748C */
IxSample *ix_system_new_sample(void);                                  /* 0x004474E3 */
void ix_system_delete_all_samples(void);                                  /* 0x00447548 */
IxSound *ix_system_new_sound(IxSample *sample);                        /* 0x0044758D */
void ix_system_delete_all_sounds(void);                                   /* 0x00447610 */
void ix_system_delete_sound(IxSound *sound);                              /* 0x004475C7 */
