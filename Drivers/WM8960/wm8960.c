/*
 * Copyright (c) 2016, Freescale Semiconductor, Inc.
 * Copyright 2016-2019 NXP
 * All rights reserved.
 *
 * SPDX-License-Identifier: BSD-3-Clause
 */
#include "wm8960.h"
#include "LOG.h"
#include "i2c.h"

/*******************************************************************************
 * Definitations
 ******************************************************************************/
#define LEFT_INPUT_VOLUME 0X00
#define RIGHT_INPUT_VOLUME 0X01
#define LOUT1_VOLUME 0X02
#define ROUT1_VOLUME 0X03
#define CLOCKING_1 0X04
#define ADC_AND_DAC_CONTROL_1 0X05
#define ADC_AND_DAC_CONTROL_2 0X06
#define DIGITAL_AUDIO_INTERFACE 0X07
#define CLOCKING_2 0X08
#define AUDIO_INTERFACE 0X09
#define LEFT_DAC_VOLUME 0X0A
#define RIGHT_DAC_VOLUME 0X0B
#define RESET 0X0F
#define _3D_CONTROL 0X10
#define ALC1 0X11
#define ALC2 0X12
#define ALC3 0X13
#define NOISE_GATE 0X14
#define LEFT_ADC_VOLUME 0X15
#define RIGHT_ADC_VOLUME 0X16
#define ADDITIONAL_CONTROL_1 0X17
#define ADDITIONAL_CONTROL_2 0X18
#define POWER_MANAGEMENT_1 0X19
#define POWER_MANAGEMENT_2 0X1A
#define ADDITIONAL_CONTROL_3 0X1B
#define ANTI_POP_1 0X1C
#define ANTI_POP_2 0X1D
#define ADCL_SIGNAL_PATH  0X20
#define ADCR_SIGNAL_PATH  0X21
#define LEFT_OUT_MIX 0X22
#define RIGHT_OUT_MIX 0X25
#define MONO_OUT_MIX_1 0X26
#define MONO_OUT_MIX_2 0X27
#define LOUT2_VOLUME 0X28
#define ROUT2_VOLUME 0X29
#define MONOOUT_VOLUME 0X2A
#define INPUT_BOOST_MIXER_1 0X2B
#define INPUT_BOOST_MIXER_2 0X2C
#define BYPASS_1 0X2D
#define BYPASS_2 0X2E
#define POWER_MANAGEMENT_3 0X2F
#define ADDITIONAL_CONTROL_4 0X30
#define CLASS_D_CONTROL_1 0X31
#define CLASS_D_CONTROL_3 0X33
#define PLL_N 0X34
#define PLL_K_1 0X35
#define PLL_K_2 0X36
#define PLL_K_3 0X37
/*******************************************************************************
 * Prototypes
 ******************************************************************************/

/*******************************************************************************
 * Variables
 ******************************************************************************/
/*
 * wm8960 register cache
 * We can't read the WM8960 register space when we are
 * using 2 wire for device control, so we cache them instead.
 */
static const uint16_t wm8960_reg[56] = {
    0x0097, 0x0097, 0x0000, 0x0000, 0x0000, 0x0008, 0x0000, 0x000a, 0x01c0, 0x0000, 0x00ff, 0x00ff, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x007b, 0x0100, 0x0032, 0x0000, 0x00c3, 0x00c3, 0x01c0, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0000, 0x0000, 0x0000, 0x0000, 0x0100, 0x0100, 0x0050, 0x0050, 0x0050, 0x0050, 0x0000, 0x0000, 0x0000, 0x0000,
    0x0040, 0x0000, 0x0000, 0x0050, 0x0050, 0x0000, 0x0002, 0x0037, 0x004d, 0x0080, 0x0008, 0x0031, 0x0026, 0x00e9,
};

static uint16_t reg_cache[56];

/*******************************************************************************
 * Code
 ******************************************************************************/
uint8_t WM8960_Init()
{
	 // Serial.println("WM8960 INIT...");
	  //Reset Device
	  WM8960_WriteReg(RESET, 0x0000);
	  HAL_Delay(100);
	  // Serial.println("WM8960 reset completed !!\r\n");

	  //Set Power Source
	  WM8960_WriteReg(POWER_MANAGEMENT_1, 1<<7 | 1<<6 | 1<<5 | 1<<4 | 1<<3 | 1<<2 | 1<<1);
	  WM8960_WriteReg(POWER_MANAGEMENT_2, 1<<8 | 1<<7 | 1<<6 | 1<<5 | 1<<4 | 1<<3 | 1<<0);
	  WM8960_WriteReg(POWER_MANAGEMENT_3, 1<<3 | 1<<2 | 1<<4 | 1<<5);

	  //Configure clock
	  //MCLK->div1->SYSCLK->DAC/ADC sample Freq = 24MHz(MCLK) / 2*256 = 46.875kHz
	  WM8960_WriteReg(CLOCKING_1, 0x0091);
	  WM8960_WriteReg(CLOCKING_2, 0x01ca);

	  //Configure ADC/DAC
	  WM8960_WriteReg(ADC_AND_DAC_CONTROL_1, 1 << 2 | 1 << 1);
	  //Configure audio interface
	  //I2S format 16 bits word length and set to master mode
	  // WM8960_WriteReg(DIGITAL_AUDIO_INTERFACE, 1<<1 | 1<<6);
	  WM8960_WriteReg(DIGITAL_AUDIO_INTERFACE, 1<<1);

	  //Configure HP_L and HP_R OUTPUTS
	  WM8960_WriteReg(LOUT1_VOLUME, 0x007F | 0x0100);  //LOUT1 Volume Set
	  WM8960_WriteReg(ROUT1_VOLUME, 0x007F | 0x0100);  //ROUT1 Volume Set

	  //Configure SPK_RP and SPK_RN
	  WM8960_WriteReg(LOUT2_VOLUME, 0x7F | 1<<8 ); //Left Speaker Volume
	  WM8960_WriteReg(ROUT2_VOLUME, 0x7F | 1<<8 ); //Right Speaker Volume

	  //Enable the OUTPUTS
	  WM8960_WriteReg(CLASS_D_CONTROL_1, 0x00F7); //Enable Class D Speaker Outputs
	  //Configure DAC volume
	  WM8960_WriteReg(LEFT_DAC_VOLUME, 0X00FF | 0x0100);
	  WM8960_WriteReg(RIGHT_DAC_VOLUME, 0x00FF | 0x0100);
	  //3D
	  //WM8960_WriteReg(0x10, 0x001F);

	  // enable Left DAC to Left Output Mixer
	  WM8960_WriteReg(LEFT_OUT_MIX, 1<<8);
	  WM8960_WriteReg(RIGHT_OUT_MIX, 1<<8);

	  // connect LINPUT1 to PGA and set PGA Boost Gain.
	  WM8960_WriteReg(ADCL_SIGNAL_PATH, 0X0020 | 1<<8 | 1<<3);
	  WM8960_WriteReg(ADCR_SIGNAL_PATH, 0X0020 | 1<<8 | 1<<3);

	  // set Input PGA Volume
	  WM8960_WriteReg(LEFT_INPUT_VOLUME, 0X0027 | 0X0100 );
	  WM8960_WriteReg(RIGHT_INPUT_VOLUME, 0X0027 | 0X0100 );

	  // set ADC Volume
	  WM8960_WriteReg(LEFT_ADC_VOLUME, 0X00c3 | 0X0100 );
	  WM8960_WriteReg(RIGHT_ADC_VOLUME,0X00c3 | 0X0100 );

	  // disable bypass switch
	  WM8960_WriteReg(BYPASS_1,0x0000);
	  WM8960_WriteReg(BYPASS_2,0x0000);

	  // enable bypass switch
	  // WM8960_WriteReg(BYPASS_1,0x0000 | 1<<7);
	  // WM8960_WriteReg(BYPASS_2,0x0000 | 1<<7);
	  //enable ALC
	  WM8960_WriteReg(ALC1, 0X007B);
	  WM8960_WriteReg(ALC2, 0X0100);
	  WM8960_WriteReg(ALC3, 0X0032);

	  //speaker select
	  WM8960_WriteReg(ADDITIONAL_CONTROL_2, 1<<6 | 1<<5); //0 speaker out 1 headphone out
	  //enable thermal shutdown
	  WM8960_WriteReg(ADDITIONAL_CONTROL_1, 0x01C3);
	  WM8960_WriteReg(ADDITIONAL_CONTROL_4, 0x0009 | 1<<6);//0x000D,0x0005
	  //config f clock to 44100 hz
	  WM8960_WriteReg(PLL_N, 0x0037);
	  WM8960_WriteReg(PLL_K_1, 0x0086);
	  WM8960_WriteReg(PLL_K_2, 0x00C2);
	  WM8960_WriteReg(PLL_K_3, 0x0027);

	  return 1;
}


uint8_t WM8960_WriteReg(uint8_t reg, uint16_t val)
{
	uint8_t data[2];

	data[0]  = (reg<<1)|((uint8_t)((val>>8)&0x0001));
	data[1] = ((uint8_t)(val&0x00FF));

	LOG("address: 0x%02x: 0x%02x 0x%02x\n\r", 0x1A , data[0], data[1]);

	if(HAL_I2C_Master_Transmit(&hi2c4, 0x1A << 1, (uint8_t *)&data, 2, 1000)==HAL_OK)
	{
		reg_cache[reg] = val;
		return 0;
	}

	return 1;
}

uint8_t WM8960_ReadReg(uint8_t reg, uint16_t *val)
{
    if (reg >= 56)
    {
        return 2;
    }

    *val = reg_cache[reg];

    return 0;
}

uint8_t WM8960_ModifyReg(uint8_t reg, uint16_t mask, uint16_t val)
{
    uint8_t retval   = 0;
    uint16_t reg_val = 0;
    retval           = WM8960_ReadReg(reg, &reg_val);
    if (retval != 0)
    {
        return 1;
    }
    reg_val &= (uint16_t)~mask;
    reg_val |= val;
    retval = WM8960_WriteReg(reg, reg_val);
    if (retval != 0)
    {
        return 1;
    }
    return 0;
}

