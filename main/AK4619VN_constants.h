#pragma once

// n bit wide mask shifted by x bits
#define BIT_MASK(n, x) ((1UL << (n)) - 1) << (x)

// *--------------------------------------------------------
// *
// * Power Management Register
// *
// *--------------------------------------------------------
#define REG_00_INIT 0x00

// ---- PMADx Constants ----
// Power management of ADCx

#define PMADx_POWER_DOWN 0b0 // Default
#define PMADx_POWER_UP   0b1

#define PMAD2_REG 0x00
#define PMAD2_WIDTH 1U
#define PMAD2_POS 5U
#define PMAD2_MASK BIT_MASK(PMAD2_WIDTH, PMAD2_POS)

#define PMAD1_REG 0x00
#define PMAD1_WIDTH 1U
#define PMAD1_POS 4U
#define PMAD1_MASK BIT_MASK(PMAD1_WIDTH, PMAD1_POS)

// ---- PMDAx Constants ----
// Power Management for DACx

#define PMDAX_POWER_DOWN 0b0 // Default
#define PMDAX_POWER_UP   0b1

#define PMDA2_REG 0x00
#define PMDA2_WIDTH 1U
#define PMDA2_POS 2U
#define PMDA2_MASK BIT_MASK(PMDA2_WIDTH, PMDA2_POS)

#define PMDA1_REG 0x00
#define PMDA1_WIDTH 1U
#define PMDA1_POS 1U
#define PMDA1_MASK BIT_MASK(PMDA1_WIDTH, PMDA1_POS)

// ---- RSTN Constants ----
// Internal timing reset

#define RSTN_RESET 0b0 // Default
#define RSTN_NORMAL 0b1

#define RSTN_REG 0x00
#define RSTN_WIDTH 1U
#define RSTN_POS 0U
#define RSTN_MASK BIT_MASK(RSTN_WIDTH, RSTN_POS)

// *--------------------------------------------------------
// *
// * AUDIO I/F Format
// *
// *--------------------------------------------------------
#define REG_01_INIT 0x0C

// ---- TDM Constants ----
// TDM Mode Control

#define TDM_MODE_OFF 0b0 // Default
#define TDM_MODE_ON  0b1

#define TDM_REG 0x01
#define TDM_WIDTH 1U
#define TDM_POS 7U
#define TDM_MASK BIT_MASK(TDM_WIDTH, TDM_POS)

// ---- DCF Constants ----

#define DCF_STEREO_MODE_I2S 0b000 // Default-
#define DCF_STEREO_MODE_MSB 0b101
#define DCF_STEREO_MODE_PCM_SHORT 0b110
#define DCF_STEREO_MODE_PCM_LONG 0b111
#define DCF_TDM_128_I2S 0b010
#define DCF_TDM_128_MSB 0b111
#define DCF_TDM_256_I2S 0b010
#define DCF_TDM_256_MSB 0b111

#define DCF_REG 0x01
#define DCF_WIDTH 3U
#define DCF_POS 4U
#define DCF_MASK BIT_MASK(DCF_WIDTH, DCF_POS)

// ---- DSL Constants ----
// Slot Length setting

#define DSL_24_BIT 0b00
#define DSL_20_BIT 0b01
#define DSL_16_BIT 0b10
#define DSL_32_BIT 0b11 // Default

#define DSL_REG 0x01
#define DSL_WIDTH 2U
#define DSL_POS 2U
#define DSL_MASK BIT_MASK(DSL_WIDTH, DSL_POS)


// ---- BCKP Constants ----
// BICK Edge Setting

#define BCKP_FALLING_EDGE 0b0 // Default
#define BCKP_RISING_EDGE 0b1

#define BCKP_REG 0x01
#define BCKP_WIDTH 1U
#define BCKP_POS 1U
#define BCKP_MASK BIT_MASK(BCKP_WIDTH, BCKP_POS)

// ---- SDOPH Constants ----
// Fast Mode Setting of SDOUT1/2 Output

#define SDOPH_SLOW_MODE 0b0 // Default
#define SDOPH_FAST_MODE 0b1

#define SDOPH_REG 0x01
#define SDOPH_WIDTH 1U
#define SDOPH_POS 0U
#define SDOPH_MASK BIT_MASK(SDOPH_WIDTH, SDOPH_POS)

// *--------------------------------------------------------
// *
// * Reset Control
// *
// *--------------------------------------------------------
#define REG_02_INIT 0x0C

// ---- SLOT Constants ----
// Start Position Setting for next Slot

#define SLOT_LRCK_EDGE_BASIS 0b0 // Default
#define SLOT_SLOT_LENGTH_BASIS 0b1

#define SLOT_REG 0x02
#define SLOT_WIDTH 1U
#define SLOT_POS 4U
#define SLOT_MASK BIT_MASK(SLOT_WIDTH, SLOT_POS)

// ---- DIDL Constants ----
// SDIN1/2 Word Length Setting

#define DIDL_24_BIT 0b00
#define DIDL_20_BIT 0b01
#define DIDL_16_BIT 0b10
#define DIDL_32_BIT 0b11 // Default

#define DIDL_REG 0x02
#define DIDL_WIDTH 2U
#define DIDL_POS 2U
#define DIDL_MASK BIT_MASK(DIDL_WIDTH, DIDL_POS)

// ---- DODL Constants ----
// SDOUT1/2 Word Length Setting

#define DODL_24_BIT 0b00 // Default
#define DODL_20_BIT 0b01
#define DODL_16_BIT 0b10
// 0b11 not used 

#define DODL_REG 0x02
#define DODL_WIDTH 2U
#define DODL_POS 0U
#define DODL_MASK BIT_MASK(DODL_WIDTH, DODL_POS)

// *--------------------------------------------------------
// *
// * System Clock Setting
// *
// *--------------------------------------------------------
#define REG_03_INIT 0x00

// ---- FS Constants ----
// MCLK/BICK and Sampling Frequency Range Setting

// There's two 256 modes, meaning two different constants:
//     - 8 kHz ≦ fs ≦ 48 kHz
//     - fs = 96 kHz
#define FS_256_MCLK_48KHZ 0b000 // Default
#define FS_256_MCLK_96KHZ 0b001
#define FS_384_MCLK 0b010
#define FS_512_MCLK 0b011
#define FS_128_MCLK 0b100

#define FS_REG 0x03
#define FS_WIDTH 3U
#define FS_POS 0U
#define FS_MASK BIT_MASK(FS_WIDTH, FS_POS)

// *--------------------------------------------------------
// *
// * MIC AMP Gain
// *
// *--------------------------------------------------------
#define REG_04_INIT 0x22
#define REG_05_INIT 0x22

// ---- MGNxx Constants ----
// MIC Gain AMP Setting

#define MGN_MINUS_6_DB 0b0000
#define MGN_MINUS_3_DB 0b0001
#define MGN_0_DB 0b0010 // Default
#define MGN_3_DB 0b0011
#define MGN_6_DB 0b0100
#define MGN_9_DB 0b0101
#define MGN_12_DB 0b0110
#define MGN_15_DB 0b0111
#define MGN_18_DB 0b1000
#define MGN_21_DB 0b1001
#define MGN_24_DB 0b1010
#define MGN_27_DB 0b1011
// 0b11xx not used

#define MGN1L_REG 0x04
#define MGN1L_WIDTH 4U
#define MGN1L_POS 4U
#define MGN1L_MASK BIT_MASK(MGN1L_WIDTH, MGN1L_POS)

#define MGN1R_REG 0x04
#define MGN1R_WIDTH 4U
#define MGN1R_POS 0U
#define MGN1R_MASK BIT_MASK(MGN1R_WIDTH, MGN1R_POS)

#define MGN2L_REG 0x05
#define MGN2L_WIDTH 4U
#define MGN2L_POS 4U
#define MGN2L_MASK BIT_MASK(MGN2L_WIDTH, MGN2L_POS)

#define MGN2R_REG 0x05
#define MGN2R_WIDTH 4U
#define MGN2R_POS 0U
#define MGN2R_MASK BIT_MASK(MGN2R_WIDTH, MGN2R_POS)

// *--------------------------------------------------------
// *
// * ADCx xch Digital Volume
// *
// *--------------------------------------------------------
#define REG_06_INIT 0x30
#define REG_07_INIT 0x30
#define REG_08_INIT 0x30
#define REG_09_INIT 0x30

// ---- VOLADxx Constants ----
// ADC Digital Volume Setting

// 00h is +24 db, FEh is -103 db, FFh is -∞ db
// Step size is 0.5 dB

// This takes in a float and returns a register value
// Clamps input to valid range and handles 0.5 dB steps properly
#define VOLADXX_VOLUME(x) ((x) <= -103.0f ? 0xFEU : \
                          (x) >= 24.0f ? 0x00U : \
                          (0xFEU - (unsigned int)(((x) + 103.0f) * 2.0f + 0.5f)))

#define VOLADXX_VOLUME_MINUS_INFINITY 0xFF // -∞ dB

#define VOLADXX_VOLUME_MAX 24.0f // +24 dB
#define VOLADXX_VOLUME_MIN -103.0f // -103 dB
#define VOLADXX_VOLUME_STEP 0.5f // dB

#define VOLADXX_VOLUME_MIN_REG 0xFE // -103 dB
#define VOLADXX_VOLUME_MAX_REG 0x00 // +24 dB
#define VOLADXX_VOLUME_DEFAULT 0x30 // 0 dB

#define VOLAD1L_REG 0x06
#define VOLAD1L_WIDTH 8U
#define VOLAD1L_POS 0U
#define VOLAD1L_MASK BIT_MASK(VOLAD1L_WIDTH, VOLAD1L_POS)

#define VOLAD1R_REG 0x07
#define VOLAD1R_WIDTH 8U
#define VOLAD1R_POS 0U
#define VOLAD1R_MASK BIT_MASK(VOLAD1R_WIDTH, VOLAD1R_POS)

#define VOLAD2L_REG 0x08
#define VOLAD2L_WIDTH 8U
#define VOLAD2L_POS 0U
#define VOLAD2L_MASK BIT_MASK(VOLAD2L_WIDTH, VOLAD2L_POS)

#define VOLAD2R_REG 0x09
#define VOLAD2R_WIDTH 8U
#define VOLAD2R_POS 0U
#define VOLAD2R_MASK BIT_MASK(VOLAD2R_WIDTH, VOLAD2R_POS)

// *--------------------------------------------------------
// *
// * ADC Digital Filter Setting
// *
// *--------------------------------------------------------
#define REG_0A_INIT 0x00

// ---- ADxVO Constants ----
// ADCx Voice Filter Control

#define ADXVO_VOICE_FILTER_OFF 0b0 // Default
#define ADXVO_VOICE_FILTER_ON  0b1

#define AD2VO_REG 0x0A
#define AD2VO_WIDTH 1U
#define AD2VO_POS 6U
#define AD2VO_MASK BIT_MASK(AD2VO_WIDTH, AD2VO_POS)

#define AD1VO_REG 0x0A
#define AD1VO_WIDTH 1U
#define AD1VO_POS 2U
#define AD1VO_MASK BIT_MASK(AD1VO_WIDTH, AD1VO_POS)

// ---- ADxSD Constants ----
// ADCx Digital Filter Selection (Short Delay on vs off)

#define ADXSD_SHORT_DELAY_OFF 0b0 // Default
#define ADXSD_SHORT_DELAY_ON  0b1

#define AD2SD_REG 0x0A
#define AD2SD_WIDTH 1U
#define AD2SD_POS 5U
#define AD2SD_MASK BIT_MASK(AD2SD_WIDTH, AD2SD_POS)

#define AD1SD_REG 0x0A
#define AD1SD_WIDTH 1U
#define AD1SD_POS 1U
#define AD1SD_MASK BIT_MASK(AD1SD_WIDTH, AD1SD_POS)

// ---- ADxSL Constants ----
// ADCx Digital Filter Selection (Sharp Roll-off vs Slow Roll-off)

#define ADXSL_SHARP_ROLL_OFF 0b0 // Default
#define ADXSL_SLOW_ROLL_OFF  0b1

#define AD2SL_REG 0x0A
#define AD2SL_WIDTH 1U
#define AD2SL_POS 4U
#define AD2SL_MASK BIT_MASK(AD2SL_WIDTH, AD2SL_POS)

#define AD1SL_REG 0x0A
#define AD1SL_WIDTH 1U
#define AD1SL_POS 0U
#define AD1SL_MASK BIT_MASK(AD1SL_WIDTH, AD1SL_POS)

// *--------------------------------------------------------
// *
// * ADC Analog Input Setting
// *
// *--------------------------------------------------------
#define REG_0B_INIT 0x00

// ---- ADxxSEL Constants ----

#define ADXXSEL_DIFFERENTIAL 0b00 // Default
#define ADXXSEL_SINGLE_ENDED1 0b01
#define ADXXSEL_SINGLE_ENDED2 0b10
#define ADXXSEL_PSEUDO_DIFFERENTIAL 0b11

#define AD1LSEL_REG 0x0B
#define AD1LSEL_WIDTH 2U
#define AD1LSEL_POS 6U
#define AD1LSEL_MASK BIT_MASK(AD1LSEL_WIDTH, AD1LSEL_POS)

#define AD1RSEL_REG 0x0B
#define AD1RSEL_WIDTH 2U
#define AD1RSEL_POS 4U
#define AD1RSEL_MASK BIT_MASK(AD1RSEL_WIDTH, AD1RSEL_POS)

#define AD2LSEL_REG 0x0B
#define AD2LSEL_WIDTH 2U
#define AD2LSEL_POS 2U
#define AD2LSEL_MASK BIT_MASK(AD2LSEL_WIDTH, AD2LSEL_POS)

#define AD2RSEL_REG 0x0B
#define AD2RSEL_WIDTH 2U
#define AD2RSEL_POS 0U
#define AD2RSEL_MASK BIT_MASK(AD2RSEL_WIDTH, AD2RSEL_POS)

// *--------------------------------------------------------
// *
// * Reserved Register
// *
// *--------------------------------------------------------
#define REG_0C_INIT 0x00 // Reserved register, no fields defined

// *--------------------------------------------------------
// *
// * ADC Mute & HPF Control
// *
// *--------------------------------------------------------
#define REG_0D_INIT 0x00

// ---- ATSPAD Constants ----
//  ADC Volume Level Transition Time

#define ATSPAD_ATT_4 0b0 // Default
#define ATSPAD_ATT_16 0b1

#define ATSPAD_REG 0x0D
#define ATSPAD_WIDTH 1U
#define ATSPAD_POS 7U
#define ATSPAD_MASK BIT_MASK(ATSPAD_WIDTH, ATSPAD_POS)

// ---- ADxMUTE Constants ----
// ADCx Soft Mute Enable

#define ADXMUTE_DISABLE 0b0 // Default
#define ADXMUTE_ENABLE  0b1

#define AD2MUTE_REG 0x0D
#define AD2MUTE_WIDTH 1U
#define AD2MUTE_POS 6U
#define AD2MUTE_MASK BIT_MASK(AD2MUTE_WIDTH, AD2MUTE_POS)

#define AD1MUTE_REG 0x0D
#define AD1MUTE_WIDTH 1U
#define AD1MUTE_POS 5U
#define AD1MUTE_MASK BIT_MASK(AD1MUTE_WIDTH, AD1MUTE_POS)

// ---- ADxHPFN Constants ----
// ADCx DC Offset Cancel HPF Enable

#define ADXHPF_ENABLE 0b0 // Default
#define ADXHPF_DISABLE 0b1

#define AD2HPFN_REG 0x0D
#define AD2HPFN_WIDTH 1U
#define AD2HPFN_POS 2U
#define AD2HPFN_MASK BIT_MASK(AD2HPFN_WIDTH, AD2HPFN_POS)

#define AD1HPFN_REG 0x0D
#define AD1HPFN_WIDTH 1U
#define AD1HPFN_POS 1U
#define AD1HPFN_MASK BIT_MASK(AD1HPFN_WIDTH, AD1HPFN_POS)

// *--------------------------------------------------------
// *
// * DACx xch Digital Volume
// *
// *--------------------------------------------------------
#define REG_0E_INIT 0x18
#define REG_0F_INIT 0x18
#define REG_10_INIT 0x18
#define REG_11_INIT 0x18

// ---- VOLDAxx Constants ----
// DAC Digital Volume Setting

// 00h is +12 db, FEh is -115 db, FFh is -∞ db
// Step size is 0.5 dB

#define VOLDAXX_VOLUME(x) ((x) <= -115.0f ? 0xFEU : \
                          (x) >= 12.0f ? 0x00U : \
                          (0xFEU - (unsigned int)(((x) + 115.0f) * 2.0f + 0.5f)))

#define VOLDAXX_VOLUME_MINUS_INFINITY 0xFF // -∞ dB

#define VOLDAXX_VOLUME_MAX 12.0f // +12 dB
#define VOLDAXX_VOLUME_MIN -115.0f // -115 dB
#define VOLDAXX_VOLUME_STEP 0.5f // dB

#define VOLDAXX_VOLUME_MIN_REG 0xFE // -115 dB
#define VOLDAXX_VOLUME_MAX_REG 0x00 // +12 dB
#define VOLDAXX_VOLUME_DEFAULT 0x18 // 0 dB

#define VOLDA1L_REG 0x0E
#define VOLDA1L_WIDTH 8U
#define VOLDA1L_POS 0U
#define VOLDA1L_MASK BIT_MASK(VOLDA1L_WIDTH, VOLDA1L_POS)

#define VOLDA1R_REG 0x0F
#define VOLDA1R_WIDTH 8U
#define VOLDA1R_POS 0U
#define VOLDA1R_MASK BIT_MASK(VOLDA1R_WIDTH, VOLDA1R_POS)

#define VOLDA2L_REG 0x10
#define VOLDA2L_WIDTH 8U
#define VOLDA2L_POS 0U
#define VOLDA2L_MASK BIT_MASK(VOLDA2L_WIDTH, VOLDA2L_POS)

#define VOLDA2R_REG 0x11
#define VOLDA2R_WIDTH 8U
#define VOLDA2R_POS 0U
#define VOLDA2R_MASK BIT_MASK(VOLDA2R_WIDTH, VOLDA2R_POS)

// *--------------------------------------------------------
// *
// * DAC Input Select Setting
// *
// *--------------------------------------------------------
#define REG_12_INIT 0x04

// ---- DACxSEL Constants ----
// DACx source multiplexer setting

#define DACXSEL_SDIN1 0b00 // Default for DAC1
#define DACXSEL_SDIN2 0b01 // Default for DAC2
#define DACXSEL_SDOUT1 0b10
#define DACXSEL_SDOUT2 0b11

#define DAC2SEL_REG 0x12
#define DAC2SEL_WIDTH 2U
#define DAC2SEL_POS 2U
#define DAC2SEL_MASK BIT_MASK(DAC2SEL_WIDTH, DAC2SEL_POS)

#define DAC1SEL_REG 0x12
#define DAC1SEL_WIDTH 2U
#define DAC1SEL_POS 0U
#define DAC1SEL_MASK BIT_MASK(DAC1SEL_WIDTH, DAC1SEL_POS)

// *--------------------------------------------------------
// *
// * DAC De-Emphasis Setting
// *
// *--------------------------------------------------------
#define REG_13_INIT 0x05

// ---- DEMx Constants ----
// De-emphasis Filter Control

#define DEMX_44_1KHZ 0b00
#define DEMX_OFF 0b01 // Default
#define DEMX_48KHZ 0b10
#define DEMX_32KHZ 0b11

#define DEM2_REG 0x13
#define DEM2_WIDTH 2U
#define DEM2_POS 2U
#define DEM2_MASK BIT_MASK(DEM2_WIDTH, DEM2_POS)

#define DEM1_REG 0x13
#define DEM1_WIDTH 2U
#define DEM1_POS 0U
#define DEM1_MASK BIT_MASK(DEM1_WIDTH, DEM1_POS)

// *--------------------------------------------------------
// *
// * DAC Mute & Filter Setting
// *
// *--------------------------------------------------------
#define REG_14_INIT 0x00

// ---- ATSPDA Constants ----

#define ATSPDA_ATT_4 0b0 // Default
#define ATSPDA_ATT_16 0b1

#define ATSPDA_REG 0x14
#define ATSPDA_WIDTH 1U
#define ATSPDA_POS 7U
#define ATSPDA_MASK BIT_MASK(ATSPDA_WIDTH, ATSPDA_POS)

// ---- DAxMUTE Constants ----
// DACx Soft Mute Enable

#define DAXMUTE_DISABLE 0b0 // Default
#define DAXMUTE_ENABLE  0b1

#define DA2MUTE_REG 0x0E
#define DA2MUTE_WIDTH 1U
#define DA2MUTE_POS 5U
#define DA2MUTE_MASK BIT_MASK(DA2MUTE_WIDTH, DA2MUTE_POS)

#define DA1MUTE_REG 0x0E
#define DA1MUTE_WIDTH 1U
#define DA1MUTE_POS 4U
#define DA1MUTE_MASK BIT_MASK(DA1MUTE_WIDTH, DA1MUTE_POS)

// ---- DAxSD Constants ----
// DACX Digital Filter Selection (Short Delay on vs off)

#define DAXSD_SHORT_DELAY_OFF 0b0
#define DAXSD_SHORT_DELAY_ON  0b1  // Default

#define DA2SD_REG 0x14
#define DA2SD_WIDTH 1U
#define DA2SD_POS 3U
#define DA2SD_MASK BIT_MASK(DA2SD_WIDTH, DA2SD_POS)

#define DA1SD_REG 0x14
#define DA1SD_WIDTH 1U
#define DA1SD_POS 1U
#define DA1SD_MASK BIT_MASK(DA1SD_WIDTH, DA1SD_POS)

// ---- DAxSL Constants ----
// DACX Digital Filter Selection (Sharp Roll-off vs Slow Roll-off)

#define DAXSL_SHARP_ROLL_OFF 0b0  // Default
#define DAXSL_SLOW_ROLL_OFF 0b1

#define DA2SL_REG 0x14
#define DA2SL_WIDTH 1U
#define DA2SL_POS 2U
#define DA2SL_MASK BIT_MASK(DA2SL_WIDTH, DA2SL_POS)

#define DA1SL_REG 0x14
#define DA1SL_WIDTH 1U
#define DA1SL_POS 0U
#define DA1SL_MASK BIT_MASK(DA1SL_WIDTH, DA1SL_POS)
