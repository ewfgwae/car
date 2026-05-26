#include "delay.h"
#include "ti_msp_dl_config.h"

#include "stdio.h"
#include "string.h"


void delay_ms(uint32_t ms)
{
    DL_Common_delayCycles(CPUCLK_FREQ/1000*ms);
}

void delay_us(uint32_t us)
{
    DL_Common_delayCycles(CPUCLK_FREQ/1000000*us);
}

void led_on()
{
    DL_GPIO_setPins(LED_PORT,  LED_LED1_PIN);
}
void led_off()
{
    DL_GPIO_clearPins(LED_PORT,  LED_LED1_PIN);
}

void led_toggle()
{
    DL_GPIO_togglePins(LED_PORT,  LED_LED1_PIN);
}

void p_s(char *string,  ...)
{
    char buffer[100] = {0};
    va_list arg;
    int re = 0;
    char *p;

    va_start(arg,  string);
    re = vsnprintf(buffer,  100,  string,  arg);
    va_end(arg);

    if(re < 0){
        strcpy(buffer,  "p_s :\r\n");
    }

    p = buffer;
    while (*p)
    {
        DL_UART_transmitDataBlocking(UART_0_INST,  *p);
        p++;
    }
    
}

void buzzer_off()
{
    DL_GPIO_setPins(BUZZER_PORT,  BUZZER_EN_PIN);
}

void buzzer_on()
{
    DL_GPIO_clearPins(BUZZER_PORT,  BUZZER_EN_PIN);
}

void buzzer_toggle()
{
    DL_GPIO_togglePins(BUZZER_PORT,  BUZZER_EN_PIN);
}