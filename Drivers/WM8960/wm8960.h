
/*
 * Copyright (c) 2015, Freescale Semiconductor, Inc.
 * Copyright 2016-2019 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */

#ifndef _FSL_WM8960_H_
#define _FSL_WM8960_H_

#include "i2c.h"
#include "LOG.h"
#include "stdbool.h"


#if defined(__cplusplus)
extern "C" {
#endif


uint8_t WM8960_Init();

uint8_t WM8960_WriteReg(uint8_t reg, uint16_t val);



#if defined(__cplusplus)
}
#endif

/*! @} */

#endif /* _FSL_WM8960_H_ */

/*******************************************************************************
 * API
 ******************************************************************************/
