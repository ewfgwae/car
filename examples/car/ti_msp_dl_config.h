/*
 * Copyright (c) 2023, Texas Instruments Incorporated - http://www.ti.com
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * *  Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *
 * *  Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * *  Neither the name of Texas Instruments Incorporated nor the names of
 *    its contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE,
 * EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

/*
 *  ============ ti_msp_dl_config.h =============
 *  Configured MSPM0 DriverLib module declarations
 *
 *  DO NOT EDIT - This file is generated for the LP_MSPM0G3507
 *  by the SysConfig tool.
 */
#ifndef ti_msp_dl_config_h
#define ti_msp_dl_config_h

#define CONFIG_LP_MSPM0G3507
#define CONFIG_MSPM0G3507

#if defined(__ti_version__) || defined(__TI_COMPILER_VERSION__)
#define SYSCONFIG_WEAK __attribute__((weak))
#elif defined(__IAR_SYSTEMS_ICC__)
#define SYSCONFIG_WEAK __weak
#elif defined(__GNUC__)
#define SYSCONFIG_WEAK __attribute__((weak))
#endif

#include <ti/devices/msp/msp.h>
#include <ti/driverlib/driverlib.h>
#include <ti/driverlib/m0p/dl_core.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  ======== SYSCFG_DL_init ========
 *  Perform all required MSP DL initialization
 *
 *  This function should be called once at a point before any use of
 *  MSP DL.
 */


/* clang-format off */

#define POWER_STARTUP_DELAY                                                (16)



#define CPUCLK_FREQ                                                     32000000



/* Defines for PWM_0 */
#define PWM_0_INST                                                         TIMA0
#define PWM_0_INST_IRQHandler                                   TIMA0_IRQHandler
#define PWM_0_INST_INT_IRQN                                     (TIMA0_INT_IRQn)
#define PWM_0_INST_CLK_FREQ                                             32000000
/* GPIO defines for channel 0 */
#define GPIO_PWM_0_C0_PORT                                                 GPIOA
#define GPIO_PWM_0_C0_PIN                                          DL_GPIO_PIN_0
#define GPIO_PWM_0_C0_IOMUX                                       (IOMUX_PINCM1)
#define GPIO_PWM_0_C0_IOMUX_FUNC                      IOMUX_PINCM1_PF_TIMA0_CCP0
#define GPIO_PWM_0_C0_IDX                                    DL_TIMER_CC_0_INDEX
/* GPIO defines for channel 1 */
#define GPIO_PWM_0_C1_PORT                                                 GPIOA
#define GPIO_PWM_0_C1_PIN                                          DL_GPIO_PIN_1
#define GPIO_PWM_0_C1_IOMUX                                       (IOMUX_PINCM2)
#define GPIO_PWM_0_C1_IOMUX_FUNC                      IOMUX_PINCM2_PF_TIMA0_CCP1
#define GPIO_PWM_0_C1_IDX                                    DL_TIMER_CC_1_INDEX



/* Defines for COMPARE_0 */
#define COMPARE_0_INST                                                   (TIMG6)
#define COMPARE_0_INST_IRQHandler                               TIMG6_IRQHandler
#define COMPARE_0_INST_INT_IRQN                                 (TIMG6_INT_IRQn)
/* GPIO defines for channel 0 */
#define GPIO_COMPARE_0_C0_PORT                                             GPIOA
#define GPIO_COMPARE_0_C0_PIN                                     DL_GPIO_PIN_21
#define GPIO_COMPARE_0_C0_IOMUX                                  (IOMUX_PINCM46)
#define GPIO_COMPARE_0_C0_IOMUX_FUNC                 IOMUX_PINCM46_PF_TIMG6_CCP0

/* Defines for COMPARE_1 */
#define COMPARE_1_INST                                                   (TIMG0)
#define COMPARE_1_INST_IRQHandler                               TIMG0_IRQHandler
#define COMPARE_1_INST_INT_IRQN                                 (TIMG0_INT_IRQn)
/* GPIO defines for channel 0 */
#define GPIO_COMPARE_1_C0_PORT                                             GPIOA
#define GPIO_COMPARE_1_C0_PIN                                     DL_GPIO_PIN_12
#define GPIO_COMPARE_1_C0_IOMUX                                  (IOMUX_PINCM34)
#define GPIO_COMPARE_1_C0_IOMUX_FUNC                 IOMUX_PINCM34_PF_TIMG0_CCP0




/* Defines for TIMER_ENCODER_READ */
#define TIMER_ENCODER_READ_INST                                          (TIMA1)
#define TIMER_ENCODER_READ_INST_IRQHandler                        TIMA1_IRQHandler
#define TIMER_ENCODER_READ_INST_INT_IRQN                        (TIMA1_INT_IRQn)
#define TIMER_ENCODER_READ_INST_LOAD_VALUE                                 (4999U)




/* Defines for I2C_0 */
#define I2C_0_INST                                                          I2C1
#define I2C_0_INST_IRQHandler                                    I2C1_IRQHandler
#define I2C_0_INST_INT_IRQN                                        I2C1_INT_IRQn
#define GPIO_I2C_0_SDA_PORT                                                GPIOA
#define GPIO_I2C_0_SDA_PIN                                        DL_GPIO_PIN_16
#define GPIO_I2C_0_IOMUX_SDA                                     (IOMUX_PINCM38)
#define GPIO_I2C_0_IOMUX_SDA_FUNC                      IOMUX_PINCM38_PF_I2C1_SDA
#define GPIO_I2C_0_SCL_PORT                                                GPIOA
#define GPIO_I2C_0_SCL_PIN                                        DL_GPIO_PIN_11
#define GPIO_I2C_0_IOMUX_SCL                                     (IOMUX_PINCM22)
#define GPIO_I2C_0_IOMUX_SCL_FUNC                      IOMUX_PINCM22_PF_I2C1_SCL


/* Defines for UART_0 */
#define UART_0_INST                                                        UART0
#define UART_0_INST_FREQUENCY                                           32000000
#define UART_0_INST_IRQHandler                                  UART0_IRQHandler
#define UART_0_INST_INT_IRQN                                      UART0_INT_IRQn
#define GPIO_UART_0_RX_PORT                                                GPIOA
#define GPIO_UART_0_TX_PORT                                                GPIOA
#define GPIO_UART_0_RX_PIN                                        DL_GPIO_PIN_31
#define GPIO_UART_0_TX_PIN                                        DL_GPIO_PIN_28
#define GPIO_UART_0_IOMUX_RX                                      (IOMUX_PINCM6)
#define GPIO_UART_0_IOMUX_TX                                      (IOMUX_PINCM3)
#define GPIO_UART_0_IOMUX_RX_FUNC                       IOMUX_PINCM6_PF_UART0_RX
#define GPIO_UART_0_IOMUX_TX_FUNC                       IOMUX_PINCM3_PF_UART0_TX
#define UART_0_BAUD_RATE                                                (115200)
#define UART_0_IBRD_32_MHZ_115200_BAUD                                      (17)
#define UART_0_FBRD_32_MHZ_115200_BAUD                                      (23)





/* Port definition for Pin Group LED */
#define LED_PORT                                                         (GPIOB)

/* Defines for LED1: GPIOB.9 with pinCMx 26 on package pin 61 */
#define LED_LED1_PIN                                             (DL_GPIO_PIN_9)
#define LED_LED1_IOMUX                                           (IOMUX_PINCM26)
/* Port definition for Pin Group BUZZER */
#define BUZZER_PORT                                                      (GPIOB)

/* Defines for EN: GPIOB.13 with pinCMx 30 on package pin 1 */
#define BUZZER_EN_PIN                                           (DL_GPIO_PIN_13)
#define BUZZER_EN_IOMUX                                          (IOMUX_PINCM30)
/* Port definition for Pin Group MOTOR */
#define MOTOR_PORT                                                       (GPIOA)

/* Defines for A_1: GPIOA.7 with pinCMx 14 on package pin 49 */
#define MOTOR_A_1_PIN                                            (DL_GPIO_PIN_7)
#define MOTOR_A_1_IOMUX                                          (IOMUX_PINCM14)
/* Defines for A_2: GPIOA.8 with pinCMx 19 on package pin 54 */
#define MOTOR_A_2_PIN                                            (DL_GPIO_PIN_8)
#define MOTOR_A_2_IOMUX                                          (IOMUX_PINCM19)
/* Defines for B_1: GPIOA.9 with pinCMx 20 on package pin 55 */
#define MOTOR_B_1_PIN                                            (DL_GPIO_PIN_9)
#define MOTOR_B_1_IOMUX                                          (IOMUX_PINCM20)
/* Defines for B_2: GPIOA.10 with pinCMx 21 on package pin 56 */
#define MOTOR_B_2_PIN                                           (DL_GPIO_PIN_10)
#define MOTOR_B_2_IOMUX                                          (IOMUX_PINCM21)
/* Port definition for Pin Group SENSOR */
#define SENSOR_PORT                                                      (GPIOB)

/* Defines for PIN_0: GPIOB.0 with pinCMx 12 on package pin 47 */
#define SENSOR_PIN_0_PIN                                         (DL_GPIO_PIN_0)
#define SENSOR_PIN_0_IOMUX                                       (IOMUX_PINCM12)
/* Defines for PIN_1: GPIOB.1 with pinCMx 13 on package pin 48 */
#define SENSOR_PIN_1_PIN                                         (DL_GPIO_PIN_1)
#define SENSOR_PIN_1_IOMUX                                       (IOMUX_PINCM13)
/* Defines for PIN_2: GPIOB.2 with pinCMx 15 on package pin 50 */
#define SENSOR_PIN_2_PIN                                         (DL_GPIO_PIN_2)
#define SENSOR_PIN_2_IOMUX                                       (IOMUX_PINCM15)
/* Defines for PIN_3: GPIOB.3 with pinCMx 16 on package pin 51 */
#define SENSOR_PIN_3_PIN                                         (DL_GPIO_PIN_3)
#define SENSOR_PIN_3_IOMUX                                       (IOMUX_PINCM16)
/* Defines for PIN_4: GPIOB.4 with pinCMx 17 on package pin 52 */
#define SENSOR_PIN_4_PIN                                         (DL_GPIO_PIN_4)
#define SENSOR_PIN_4_IOMUX                                       (IOMUX_PINCM17)
/* Port definition for Pin Group TURN_NUM */
#define TURN_NUM_PORT                                                    (GPIOB)

/* Defines for _1: GPIOB.20 with pinCMx 48 on package pin 19 */
#define TURN_NUM__1_PIN                                         (DL_GPIO_PIN_20)
#define TURN_NUM__1_IOMUX                                        (IOMUX_PINCM48)
/* Defines for _2: GPIOB.19 with pinCMx 45 on package pin 16 */
#define TURN_NUM__2_PIN                                         (DL_GPIO_PIN_19)
#define TURN_NUM__2_IOMUX                                        (IOMUX_PINCM45)
/* Defines for _3: GPIOB.18 with pinCMx 44 on package pin 15 */
#define TURN_NUM__3_PIN                                         (DL_GPIO_PIN_18)
#define TURN_NUM__3_IOMUX                                        (IOMUX_PINCM44)
/* Defines for _4: GPIOB.17 with pinCMx 43 on package pin 14 */
#define TURN_NUM__4_PIN                                         (DL_GPIO_PIN_17)
#define TURN_NUM__4_IOMUX                                        (IOMUX_PINCM43)
/* Defines for _5: GPIOB.16 with pinCMx 33 on package pin 4 */
#define TURN_NUM__5_PIN                                         (DL_GPIO_PIN_16)
#define TURN_NUM__5_IOMUX                                        (IOMUX_PINCM33)
/* Defines for _START: GPIOB.21 with pinCMx 49 on package pin 20 */
#define TURN_NUM__START_PIN                                     (DL_GPIO_PIN_21)
#define TURN_NUM__START_IOMUX                                    (IOMUX_PINCM49)
/* Port definition for Pin Group ENCODERA */
#define ENCODERA_PORT                                                    (GPIOA)

/* Defines for E1A: GPIOA.17 with pinCMx 39 on package pin 10 */
// pins affected by this interrupt request:["E1A","E1B"]
#define ENCODERA_INT_IRQN                                       (GPIOA_INT_IRQn)
#define ENCODERA_INT_IIDX                       (DL_INTERRUPT_GROUP1_IIDX_GPIOA)
#define ENCODERA_E1A_IIDX                                   (DL_GPIO_IIDX_DIO17)
#define ENCODERA_E1A_PIN                                        (DL_GPIO_PIN_17)
#define ENCODERA_E1A_IOMUX                                       (IOMUX_PINCM39)
/* Defines for E1B: GPIOA.15 with pinCMx 37 on package pin 8 */
#define ENCODERA_E1B_IIDX                                   (DL_GPIO_IIDX_DIO15)
#define ENCODERA_E1B_PIN                                        (DL_GPIO_PIN_15)
#define ENCODERA_E1B_IOMUX                                       (IOMUX_PINCM37)
/* Port definition for Pin Group ENCODERB */
#define ENCODERB_PORT                                                    (GPIOB)

/* Defines for E2A: GPIOB.14 with pinCMx 31 on package pin 2 */
// pins affected by this interrupt request:["E2A","E2B"]
#define ENCODERB_INT_IRQN                                       (GPIOB_INT_IRQn)
#define ENCODERB_INT_IIDX                       (DL_INTERRUPT_GROUP1_IIDX_GPIOB)
#define ENCODERB_E2A_IIDX                                   (DL_GPIO_IIDX_DIO14)
#define ENCODERB_E2A_PIN                                        (DL_GPIO_PIN_14)
#define ENCODERB_E2A_IOMUX                                       (IOMUX_PINCM31)
/* Defines for E2B: GPIOB.12 with pinCMx 29 on package pin 64 */
#define ENCODERB_E2B_IIDX                                   (DL_GPIO_IIDX_DIO12)
#define ENCODERB_E2B_PIN                                        (DL_GPIO_PIN_12)
#define ENCODERB_E2B_IOMUX                                       (IOMUX_PINCM29)
/* Port definition for Pin Group I2C */
#define I2C_PORT                                                         (GPIOB)

/* Defines for SDA: GPIOB.6 with pinCMx 23 on package pin 58 */
#define I2C_SDA_PIN                                              (DL_GPIO_PIN_6)
#define I2C_SDA_IOMUX                                            (IOMUX_PINCM23)
/* Defines for SCL: GPIOB.7 with pinCMx 24 on package pin 59 */
#define I2C_SCL_PIN                                              (DL_GPIO_PIN_7)
#define I2C_SCL_IOMUX                                            (IOMUX_PINCM24)



/* clang-format on */

void SYSCFG_DL_init(void);
void SYSCFG_DL_initPower(void);
void SYSCFG_DL_GPIO_init(void);
void SYSCFG_DL_SYSCTL_init(void);
void SYSCFG_DL_PWM_0_init(void);
void SYSCFG_DL_COMPARE_0_init(void);
void SYSCFG_DL_COMPARE_1_init(void);
void SYSCFG_DL_TIMER_ENCODER_READ_init(void);
void SYSCFG_DL_I2C_0_init(void);
void SYSCFG_DL_UART_0_init(void);

void SYSCFG_DL_SYSTICK_init(void);

bool SYSCFG_DL_saveConfiguration(void);
bool SYSCFG_DL_restoreConfiguration(void);

#ifdef __cplusplus
}
#endif

#endif /* ti_msp_dl_config_h */
