#ifndef INTRINSICS_DEF
#define INTRINSICS_DEF

/*----------------m128 ps config----------------------*/

#define MM_SET_PS(const_part, inter_part) _mm_set_ps(const_part + 3 * (inter_part), \
                                                        const_part + 2 * (inter_part), \
                                                        const_part +     (inter_part), \
                                                        const_part)

#define MM_SET_PD(const_part, inter_part) _mm_set_pd (const_part + (inter_part), \
                                                        const_part)

/*----------------------------------------------------*/

/*----------------m256 ps config----------------------*/
#define MM256_SET_PS(const_part, inter_part) _mm256_set_ps(const_part + 7 * (inter_part), \
                                                           const_part + 6 * (inter_part), \
                                                           const_part + 5 * (inter_part), \
                                                           const_part + 4 * (inter_part), \
                                                           const_part + 3 * (inter_part), \
                                                           const_part + 2 * (inter_part), \
                                                           const_part +     (inter_part), \
                                                           const_part)

#define MM256_SET_PD(const_part, inter_part) _mm256_set_pd(const_part + 3 * (inter_part), \
                                                           const_part + 2 * (inter_part), \
                                                           const_part +     (inter_part), \
                                                           const_part)
/*----------------------------------------------------*/
#endif
