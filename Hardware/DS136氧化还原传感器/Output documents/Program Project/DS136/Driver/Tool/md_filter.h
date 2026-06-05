/**
 * @file md_filter.h
 * @author wongsx
 * @brief
 * @version 0.1
 * @date 2025-11-6
 *
 * @copyright Copyright (c) 2025
 *
 */

#ifndef __MD_FILTER_H__
#define __MD_FILTER_H__

#include "type_def.h"

/*
#define NUM_BUF_MF 16
md_filter_t mf;
mf_data_t  buffer_mf[NUM_BUF_MF];
md_filter_init(&mf, buffer, NUM_BUF_MF);

y = md_filter_update(&mf, x);
*/

#if 0
typedef u16 mf_data_t;
typedef u16 mf_out_t;
#else
typedef float mf_data_t;
typedef float mf_out_t;
#endif

typedef struct
{
    mf_data_t *buffer;
    u16 len;
    u16 index;
    u16 valid_count;
} md_filter_t;

void md_filter_init(md_filter_t *f, mf_data_t *buf, u16 len);
mf_out_t md_filter_update(md_filter_t *f, mf_data_t new_sample);

#endif
