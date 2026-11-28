/* joycal.c */
void LoadJoystickCalibrationFile(short horizontalRange,
                                 short verticalRange,
                                 short horizontalDeadZone,
                                 short verticalDeadZone);              /* 0x00431A10 */
short ReadCalibratedJoystick(void);                                    /* 0x00431D20 */
void __stdcall UnionRectBounds(ShortRect *destination,
                               const ShortRect *first,
                               const ShortRect *second);               /* 0x00431EA0 */
