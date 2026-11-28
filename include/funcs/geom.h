/* geom.c */
short __stdcall MeasureTextPixelWidthClamped(const char *text);         /* 0x00418080 */
int __stdcall SeekPacketSection(PacketSectionHandle *handle, int offset,
                                short origin);                         /* 0x004180C0 */
unsigned short GetMusicDriverPresent(short mode);                              /* 0x00418130 */
short __stdcall CollectActivePaletteIndices(Viewport *viewport,
                                             unsigned char *indices,
                                             short capacity);          /* 0x00418140 */
short get_ship_max_velocity(short obj);                                /* 0x004181C0 */
short recalc_max_velocity(short ship);                                /* 0x00418210 */
void drain_fuel(short ship, short amount);                            /* 0x00418280 */
void damage_ion_drive(short ship, short amount,
                      short maximum);                                 /* 0x004182B0 */
int GetShipAccelerationRate(short ship);                          /* 0x004182F0 */
void point_at(short obj, FixedVector point);                      /* 0x00418330 */
void look_at(short obj);                                         /* 0x004183A0 */
void position_relative(FixedVector *position, FixedVector direction,
                       short distance);                          /* 0x004183D0 */
void position_relative_ijk(FixedVector *position, short obj,
                           short right, short up, short forward); /* 0x00418420 */
short FixedToShortSaturating(int value);                           /* 0x004184C0 */
int MinInt(int a, int b);                                        /* 0x004184E0 */
int MaxInt(int a, int b);                                        /* 0x004184F0 */
int AbsInt(int v);                                               /* 0x00418500 */
int intfract_sign(int sign, int magnitude);                       /* 0x00418510 */
unsigned short SignShort(short v);                                  /* 0x00418520 */
unsigned int SignFixed(int v);                                      /* 0x00418540 */
short WrapDegrees(short degrees);                                     /* 0x00418560 */
int equ_vector(const FixedVector *left, const FixedVector *right);     /* 0x00418590 */
int IsPairEqualityDifferentFromFlag(const unsigned int *values);      /* 0x004185C0 */
void zero_vector(FixedVector *vector);                                /* 0x004185F0 */
void negate_vector(FixedVector *vector);                              /* 0x00418600 */
void AddFixedVectors(const FixedVector *left, const FixedVector *right,
                     FixedVector *sum);                               /* 0x00418620 */
void SubtractFixedVectors(FixedVector *left, FixedVector *right,
                          FixedVector *difference);                    /* 0x00418650 */
void ComputeVectorDelta(const FixedVector *from, const FixedVector *to,
                        FixedVector *delta);                            /* 0x00418680 */
void ScaleFixedVector(const FixedVector *vector, int scale,
                      FixedVector *result);                            /* 0x004186B0 */
void divide_vector(FixedVector *vector, int divisor,
                   FixedVector *result);                              /* 0x00418700 */
short ChooseRandomSignedMagnitude(short minimum, short maximum,
                                  int allowNegative);                  /* 0x00418750 */
void MakeRandomVectorFixed(short minimum, short maximum,
                           FixedVector *vector);                       /* 0x00418780 */
void FillFixedVectorWithRandomComponents(short limit,
                                         FixedVector *vector);          /* 0x004187E0 */
void random_radial(const FixedVector *center, short radius,
                   FixedVector *position);                           /* 0x00418800 */
void MakeRandomNormalizedVector(FixedVector *vector);                 /* 0x00418840 */
void rectangular_to_spherical(const FixedVector *rectangular,
                              SphericalVector *spherical);       /* 0x00418890 */
void ConvertShortVectorToFixedVector(const ShortVector *source,
                                     FixedVector *destination);       /* 0x00418980 */
void ConvertFixedVectorToShortVector(const FixedVector *source,
                                     ShortVector *destination);       /* 0x004189B0 */
int dot_product(const FixedVector *left, const FixedVector *right);    /* 0x004189E0 */
short vector_angle(FixedVector left, FixedVector right);              /* 0x00418A30 */
void vector_cross_product(const FixedVector *left,
                          const FixedVector *right,
                          FixedVector *product);                 /* 0x00418A80 */
short NormalizeFixedVector(FixedVector *vector);                      /* 0x00418B10 */
int vector_length_in_dir(const FixedVector *vector,
                         const FixedVector *direction);          /* 0x00418B60 */
void vector_component_in_dir(const FixedVector *vector,
                             const FixedVector *direction,
                             FixedVector *component);            /* 0x00418BB0 */
void rotate_about_i(short angle, FixedVector *j,
                    FixedVector *k);                            /* 0x00418BE0 */
void rotate_about_j(short angle, FixedVector *i,
                    FixedVector *k);                            /* 0x00418D00 */
void rotate_about_k(short angle, FixedVector *i,
                    FixedVector *j);                            /* 0x00418E40 */
void init_ijk(short obj);                                       /* 0x00418F60 */
void copy_frame(short source, short destination);                    /* 0x00418FD0 */
void fix_objects_ijk(short obj);                                 /* 0x00419050 */
void transform_to_objects_frame(const FixedVector *source,
                                FixedVector *destination,
                                short obj);                           /* 0x004190B0 */
void alter_pitch(short angle, short obj);                        /* 0x00419110 */
void alter_yaw(short angle, short obj);                          /* 0x00419150 */
void alter_roll(short angle, short obj);                         /* 0x00419190 */
short distance_between_points(const FixedVector *from,
                              const FixedVector *to);                  /* 0x004191D0 */
short distance_from_point(short obj, const FixedVector *point);       /* 0x00419210 */
short distance_from_object(short obj, short other);                   /* 0x00419260 */
void get_facing_range_from_point(short obj, const FixedVector *point);/* 0x00419290 */
void get_facing_range_from_object(short obj, short other);            /* 0x00419310 */
void ship_vs_point(short obj, const FixedVector *point);              /* 0x00419390 */
void ship_vs_ship(short obj, short other);                             /* 0x004193B0 */
short facing_to_object(short obj, FixedVector *point);                /* 0x004193D0 */
short match_roll_orientation(short obj, short reference);             /* 0x00419440 */
int set_ship_rotation_goals(short obj, short turnRate,
                            const FixedVector *direction,
                            short pointingMode,
                            short *yawGoal, short *pitchGoal);         /* 0x004194D0 */
void point_ship(short obj, short turnRate,
                const FixedVector *direction);                        /* 0x00419620 */
void point_ship_at_point(short obj, const FixedVector *point);        /* 0x00419660 */
void point_ship_at_object(short obj, short other);                    /* 0x004196A0 */
void point_capital_ship_at_object(short obj, short other);            /* 0x004196C0 */
void point_ship_behind_object(short obj, short other);                /* 0x00419710 */
void point_ship_below_object(short obj, short other);                 /* 0x00419790 */
void point_perpendicular_to_point(short obj,
                                 const FixedVector *point);           /* 0x00419810 */
void point_perpendicular(short obj, short other);                     /* 0x00419850 */
void point_parallel(short obj, short other);                           /* 0x00419870 */
void MoveObjectAlongDirection(short obj, const FixedVector *direction,
                              short distance);                        /* 0x004198A0 */
void NormalizeAndScaleVector(FixedVector *vector, int scale);         /* 0x00419950 */
void SetVectorFixedPoint(FixedVector *p, short v);                            /* 0x00419970 */
unsigned int IsPointWithinRange(const FixedVector *from, const FixedVector *to,
                                short range);                          /* 0x00419990 */
short check_for_collision(short obj);                                 /* 0x004199C0 */
void position_child(short parent, short hardpoint,
                    FixedVector *position);                       /* 0x00419A70 */
void child_object(short hardpoint, short child, short parent);    /* 0x00419B40 */
short get_ship_slot(void);                                        /* 0x00419B70 */
short find_vacant_3d_object(void);                                /* 0x00419BA0 */
void remove_object(short obj);                                        /* 0x00419BD0 */
void apply_force_to_objects_center(FixedVector *force, short obj);    /* 0x00419CC0 */
void apply_force_to_object(FixedVector *point, FixedVector *force,
                           short obj);                                /* 0x00419D10 */
void rotational_acceleration(FixedVector *point, FixedVector *force,
                             short obj);                              /* 0x00419F70 */
void ClampVectorTo30(short *p);                                           /* 0x0041A0F0 */
void ClampTo30(short *p);                                           /* 0x0041A110 */
unsigned short IsPointWithinEyeViewCone(const FixedVector *point);   /* 0x0041A130 */
void transform_objects_to_your_view(void);                             /* 0x0041A1D0 */
void set_background_objects_rotation(short obj,
                                     FixedVector *direction);    /* 0x0041A530 */
void get_right_shape(short obj, FixedVector *direction);          /* 0x0041A610 */
