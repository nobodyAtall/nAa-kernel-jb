#ifndef __ASM_ARCH_MFP_PXA25X_H
#define __ASM_ARCH_MFP_PXA25X_H

#include <mach/mfp.h>
#include <mach/mfp-pxa2xx.h>

/* GPIO */
#define GPIO2_GPIO		MFP_CFG_IN(GPIO2, AF0)
#define GPIO3_GPIO		MFP_CFG_IN(GPIO3, AF0)
#define GPIO4_GPIO		MFP_CFG_IN(GPIO4, AF0)
#define GPIO5_GPIO		MFP_CFG_IN(GPIO5, AF0)
#define GPIO6_GPIO		MFP_CFG_IN(GPIO6, AF0)
#define GPIO7_GPIO		MFP_CFG_IN(GPIO7, AF0)
#define GPIO8_GPIO		MFP_CFG_IN(GPIO8, AF0)

#define GPIO1_RST		MFP_CFG_IN(GPIO1, AF1)

/* Crystal and Clock Signals */
#define GPIO10_RTCCLK		MFP_CFG_OUT(GPIO10, AF1, DRIVE_LOW)
#define GPIO70_RTCCLK		MFP_CFG_OUT(GPIO70, AF1, DRIVE_LOW)
#define GPIO7_48MHz		MFP_CFG_OUT(GPIO7,  AF1, DRIVE_LOW)
#define GPIO11_3_6MHz		MFP_CFG_OUT(GPIO11, AF1, DRIVE_LOW)
#define GPIO71_3_6MHz		MFP_CFG_OUT(GPIO71, AF1, DRIVE_LOW)
#define GPIO12_32KHz		MFP_CFG_OUT(GPIO12, AF1, DRIVE_LOW)
#define GPIO72_32kHz		MFP_CFG_OUT(GPIO72, AF1, DRIVE_LOW)

/* SDRAM and Static Memory I/O Signals */
#define GPIO15_nCS_1		MFP_CFG_OUT(GPIO15, AF2, DRIVE_HIGH)
#define GPIO78_nCS_2		MFP_CFG_OUT(GPIO78, AF2, DRIVE_HIGH)
#define GPIO79_nCS_3		MFP_CFG_OUT(GPIO79, AF2, DRIVE_HIGH)
#define GPIO80_nCS_4		MFP_CFG_OUT(GPIO80, AF2, DRIVE_HIGH)
#define GPIO33_nCS_5		MFP_CFG_OUT(GPIO33, AF2, DRIVE_HIGH)

/* Miscellaneous I/O and DMA Signals */
#define GPIO18_RDY		MFP_CFG_IN(GPIO18, AF1)
#define GPIO20_DREQ_0		MFP_CFG_IN(GPIO20, AF1)
#define GPIO19_DREQ_1		MFP_CFG_IN(GPIO19, AF1)

/* Alternate Bus Master Mode I/O Signals */
#define GPIO13_MBGNT		MFP_CFG_OUT(GPIO13, AF2, DRIVE_LOW)
#define GPIO73_MBGNT		MFP_CFG_OUT(GPIO73, AF1, DRIVE_LOW)
#define GPIO14_MBREQ		MFP_CFG_IN(GPIO14, AF1)
#define GPIO66_MBREQ		MFP_CFG_IN(GPIO66, AF1)

/* PC CARD */
#define GPIO52_nPCE_1		MFP_CFG_OUT(GPIO52, AF2, DRIVE_HIGH)
#define GPIO53_nPCE_2		MFP_CFG_OUT(GPIO53, AF2, DRIVE_HIGH)
#define GPIO55_nPREG		MFP_CFG_OUT(GPIO55, AF2, DRIVE_HIGH)
#define GPIO50_nPIOR		MFP_CFG_OUT(GPIO50, AF2, DRIVE_HIGH)
#define GPIO51_nPIOW		MFP_CFG_OUT(GPIO51, AF2, DRIVE_HIGH)
#define GPIO49_nPWE		MFP_CFG_OUT(GPIO49, AF2, DRIVE_HIGH)
#define GPIO48_nPOE		MFP_CFG_OUT(GPIO48, AF2, DRIVE_HIGH)
#define GPIO57_nIOIS16		MFP_CFG_IN(GPIO57, AF1)
#define GPIO56_nPWAIT		MFP_CFG_IN(GPIO56, AF1)
#define GPIO54_nPSKTSEL		MFP_CFG_OUT(GPIO54, AF2, DRIVE_HIGH)

/* FFUART */
#define GPIO34_FFUART_RXD	MFP_CFG_IN(GPIO34, AF1)
#define GPIO35_FFUART_CTS	MFP_CFG_IN(GPIO35, AF1)
#define GPIO36_FFUART_DCD	MFP_CFG_IN(GPIO36, AF1)
#define GPIO37_FFUART_DSR	MFP_CFG_IN(GPIO37, AF1)
#define GPIO38_FFUART_RI	MFP_CFG_IN(GPIO38, AF1)
#define GPIO39_FFUART_TXD	MFP_CFG_OUT(GPIO39, AF2, DRIVE_HIGH)
#define GPIO40_FFUART_DTR	MFP_CFG_OUT(GPIO40, AF2, DRIVE_HIGH)
#define GPIO41_FFUART_RTS	MFP_CFG_OUT(GPIO41, AF2, DRIVE_HIGH)

/* BTUART */
#define GPIO42_BTUART_RXD	MFP_CFG_IN(GPIO42, AF1)
#define GPIO43_BTUART_TXD	MFP_CFG_OUT(GPIO43, AF2, DRIVE_HIGH)
#define GPIO44_BTUART_CTS	MFP_CFG_IN(GPIO44, AF1)
#define GPIO45_BTUART_RTS	MFP_CFG_OUT(GPIO45, AF2, DRIVE_HIGH)

/* STUART */
#define GPIO46_STUART_RXD	MFP_CFG_IN(GPIO46, AF2)
#define GPIO47_STUART_TXD	MFP_CFG_OUT(GPIO47, AF1, DRIVE_HIGH)

/* HWUART */
#define GPIO42_HWUART_RXD	MFP_CFG_IN(GPIO42, AF3)
#define GPIO43_HWUART_TXD	MFP_CFG_OUT(GPIO43, AF3, DRIVE_HIGH)
#define GPIO44_HWUART_CTS	MFP_CFG_IN(GPIO44, AF3)
#define GPIO45_HWUART_RTS	MFP_CFG_OUT(GPIO45, AF3, DRIVE_HIGH)
#define GPIO48_HWUART_TXD	MFP_CFG_OUT(GPIO48, AF1, DRIVE_HIGH)
#define GPIO49_HWUART_RXD	MFP_CFG_IN(GPIO49, AF1)
#define GPIO50_HWUART_CTS	MFP_CFG_IN(GPIO50, AF1)
#define GPIO51_HWUART_RTS	MFP_CFG_OUT(GPIO51, AF1, DRIVE_HIGH)

/* FICP */
#define GPIO46_FICP_RXD		MFP_CFG_IN(GPIO46, AF1)
#define GPIO47_FICP_TXD		MFP_CFG_OUT(GPIO47, AF2, DRIVE_HIGH)

/* PWM 0/1 */
#define GPIO16_PWM0_OUT		MFP_CFG_OUT(GPIO16, AF2, DRIVE_LOW)
#define GPIO17_PWM1_OUT		MFP_CFG_OUT(GPIO17, AF2, DRIVE_LOW)

/* AC97 */
#define GPIO28_AC97_BITCLK	MFP_CFG_IN(GPIO28, AF1)
#define GPIO29_AC97_SDATA_IN_0	MFP_CFG_IN(GPIO29, AF1)
#define GPIO30_AC97_SDATA_OUT	MFP_CFG_OUT(GPIO30, AF2, DRIVE_LOW)
#define GPIO31_AC97_SYNC	MFP_CFG_OUT(GPIO31, AF2, DRIVE_LOW)
#define GPIO32_AC97_SDATA_IN_1	MFP_CFG_IN(GPIO32, AF1)

/* I2S */
#define GPIO28_I2S_BITCLK_IN	MFP_CFG_IN(GPIO28, AF2)
#define GPIO28_I2S_BITCLK_OUT	MFP_CFG_OUT(GPIO28, AF1, DRIVE_LOW)
#define GPIO29_I2S_SDATA_IN	MFP_CFG_IN(GPIO29, AF2)
#define GPIO30_I2S_SDATA_OUT	MFP_CFG_OUT(GPIO30, AF1, DRIVE_LOW)
#define GPIO31_I2S_SYNC		MFP_CFG_OUT(GPIO31, AF1, DRIVE_LOW)
#define GPIO32_I2S_SYSCLK	MFP_CFG_OUT(GPIO32, AF1, DRIVE_LOW)

/* SSP 1 */
#define GPIO23_SSP1_SCLK	MFP_CFG_OUT(GPIO23, AF2, DRIVE_LOW)
#define GPIO24_SSP1_SFRM	MFP_CFG_OUT(GPIO24, AF2, DRIVE_LOW)
#define GPIO25_SSP1_TXD		MFP_CFG_OUT(GPIO25, AF2, DRIVE_LOW)
#define GPIO26_SSP1_RXD		MFP_CFG_IN(GPIO26, AF1)
#define GPIO27_SSP1_EXTCLK	MFP_CFG_IN(GPIO27, AF1)

/* SSP 2 - NSSP */
#define GPIO81_SSP2_CLK_OUT 	MFP_CFG_OUT(GPIO81, AF1, DRIVE_LOW)
#define GPIO81_SSP2_CLK_IN  	MFP_CFG_IN(GPIO81, AF1)
#define GPIO82_SSP2_FRM_OUT 	MFP_CFG_OUT(GPIO82, AF1, DRIVE_LOW)
#define GPIO82_SSP2_FRM_IN  	MFP_CFG_IN(GPIO82, AF1)
#define GPIO83_SSP2_TXD      	MFP_CFG_OUT(GPIO83, AF1, DRIVE_LOW)
#define GPIO83_SSP2_RXD      	MFP_CFG_IN(GPIO83, AF2)
#define GPIO84_SSP2_TXD      	MFP_CFG_OUT(GPIO84, AF1, DRIVE_LOW)
#define GPIO84_SSP2_RXD      	MFP_CFG_IN(GPIO84, AF2)

/* MMC */
#define GPIO6_MMC_CLK		MFP_CFG_OUT(GPIO6, AF1, DRIVE_LOW)
#define GPIO8_MMC_CS0		MFP_CFG_OUT(GPIO8, AF1, DRIVE_LOW)
#define GPIO9_MMC_CS1		MFP_CFG_OUT(GPIO9, AF1, DRIVE_LOW)
#define GPIO34_MMC_CS0		MFP_CFG_OUT(GPIO34, AF2, DRIVE_LOW)
#define GPIO39_MMC_CS1		MFP_CFG_OUT(GPIO39, AF1, DRIVE_LOW)
#define GPIO53_MMC_CLK		MFP_CFG_OUT(GPIO53, AF1, DRIVE_LOW)
#define GPIO54_MMC_CLK		MFP_CFG_OUT(GPIO54, AF1, DRIVE_LOW)
#define GPIO69_MMC_CLK		MFP_CFG_OUT(GPIO69, AF1, DRIVE_LOW)
#define GPIO67_MMC_CS0		MFP_CFG_OUT(GPIO67, AF1, DRIVE_LOW)
#define GPIO68_MMC_CS1		MFP_CFG_OUT(GPIO68, AF1, DRIVE_LOW)

/* LCD */
#define GPIO58_LCD_LDD_0	MFP_CFG_OUT(GPIO58, AF2, DRIVE_LOW)
#define GPIO59_LCD_LDD_1	MFP_CFG_OUT(GPIO59, AF2, DRIVE_LOW)
#define GPIO60_LCD_LDD_2	MFP_CFG_OUT(GPIO60, AF2, DRIVE_LOW)
#define GPIO61_LCD_LDD_3	MFP_CFG_OUT(GPIO61, AF2, DRIVE_LOW)
#define GPIO62_LCD_LDD_4	MFP_CFG_OUT(GPIO62, AF2, DRIVE_LOW)
#define GPIO63_LCD_LDD_5	MFP_CFG_OUT(GPIO63, AF2, DRIVE_LOW)
#define GPIO64_LCD_LDD_6	MFP_CFG_OUT(GPIO64, AF2, DRIVE_LOW)
#define GPIO65_LCD_LDD_7	MFP_CFG_OUT(GPIO65, AF2, DRIVE_LOW)
#define GPIO66_LCD_LDD_8	MFP_CFG_OUT(GPIO66, AF2, DRIVE_LOW)
#define GPIO67_LCD_LDD_9	MFP_CFG_OUT(GPIO67, AF2, DRIVE_LOW)
#define GPIO68_LCD_LDD_10	MFP_CFG_OUT(GPIO68, AF2, DRIVE_LOW)
#define GPIO69_LCD_LDD_11	MFP_CFG_OUT(GPIO69, AF2, DRIVE_LOW)
#define GPIO70_LCD_LDD_12	MFP_CFG_OUT(GPIO70, AF2, DRIVE_LOW)
#define GPIO71_LCD_LDD_13	MFP_CFG_OUT(GPIO71, AF2, DRIVE_LOW)
#define GPIO72_LCD_LDD_14	MFP_CFG_OUT(GPIO72, AF2, DRIVE_LOW)
#define GPIO73_LCD_LDD_15	MFP_CFG_OUT(GPIO73, AF2, DRIVE_LOW)
#define GPIO74_LCD_FCLK		MFP_CFG_OUT(GPIO74, AF2, DRIVE_LOW)
#define GPIO75_LCD_LCLK		MFP_CFG_OUT(GPIO75, AF2, DRIVE_LOW)
#define GPIO76_LCD_PCLK		MFP_CFG_OUT(GPIO76, AF2, DRIVE_LOW)
#define GPIO77_LCD_BIAS		MFP_CFG_OUT(GPIO77, AF2, DRIVE_LOW)

#ifdef CONFIG_CPU_PXA26x
/* GPIO */
#define GPIO85_GPIO		MFP_CFG_IN(GPIO85, AF0)
#define GPIO86_GPIO		MFP_CFG_IN(GPIO86, AF1)
#define GPIO87_GPIO		MFP_CFG_IN(GPIO87, AF1)
#define GPIO88_GPIO		MFP_CFG_IN(GPIO88, AF1)
#define GPIO89_GPIO		MFP_CFG_IN(GPIO89, AF1)

/* SDRAM */
#define GPIO86_nSDCS2		MFP_CFG_OUT(GPIO86, AF0, DRIVE_HIGH)
#define GPIO87_nSDCS3		MFP_CFG_OUT(GPIO87, AF0, DRIVE_HIGH)
#define GPIO88_RDnWR		MFP_CFG_OUT(GPIO88, AF0, DRIVE_HIGH)
#define GPIO89_nACRESET		MFP_CFG_OUT(GPIO89, AF0, DRIVE_HIGH)

/* USB */
#define GPIO9_USB_RCV		MFP_CFG_IN(GPIO9, AF1)
#define GPIO32_USB_VP		MFP_CFG_IN(GPIO32, AF2)
#define GPIO34_USB_VM		MFP_CFG_IN(GPIO34, AF2)
#define GPIO39_USB_VPO		MFP_CFG_OUT(GPIO39, AF3, DRIVE_LOW)
#define GPIO56_USB_VMO		MFP_CFG_OUT(GPIO56, AF1, DRIVE_LOW)
#define GPIO57_USB_nOE		MFP_CFG_OUT(GPIO57, AF1, DRIVE_HIGH)

/* ASSP */
#define GPIO28_ASSP_BITCLK_IN	MFP_CFG_IN(GPIO28, AF3)
#define GPIO28_ASSP_BITCLK_OUT	MFP_CFG_OUT(GPIO28, AF3, DRIVE_LOW)
#define GPIO29_ASSP_RXD		MFP_CFG_IN(GPIO29, AF3)
#define GPIO30_ASSP_TXD		MFP_CFG_OUT(GPIO30, AF3, DRIVE_LOW)
#define GPIO31_ASSP_SFRM_IN	MFP_CFG_IN(GPIO31, AF1)
#define GPIO31_ASSP_SFRM_OUT	MFP_CFG_OUT(GPIO31, AF3, DRIVE_LOW)
#endif

#endif /* __ASM_ARCH_MFP_PXA25X_H */