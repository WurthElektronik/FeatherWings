/**
 * \file
 * \brief Arduino timer drivers for Adafruit M0 feather.
 *
 * This code is abstraction of arduino peripheral drivers for Adafruit feather
 * MO board.
 *
 * \copyright (c) 2020 Würth Elektronik eiSos GmbH & Co. KG
 *
 * \page License
 *
 * THE SOFTWARE INCLUDING THE SOURCE CODE IS PROVIDED “AS IS”. YOU ACKNOWLEDGE
 * THAT WÜRTH ELEKTRONIK EISOS MAKES NO REPRESENTATIONS AND WARRANTIES OF ANY
 * KIND RELATED TO, BUT NOT LIMITED TO THE NON-INFRINGEMENT OF THIRD PARTIES’
 * INTELLECTUAL PROPERTY RIGHTS OR THE MERCHANTABILITY OR FITNESS FOR YOUR
 * INTENDED PURPOSE OR USAGE. WÜRTH ELEKTRONIK EISOS DOES NOT WARRANT OR
 * REPRESENT THAT ANY LICENSE, EITHER EXPRESS OR IMPLIED, IS GRANTED UNDER ANY
 * PATENT RIGHT, COPYRIGHT, MASK WORK RIGHT, OR OTHER INTELLECTUAL PROPERTY
 * RIGHT RELATING TO ANY COMBINATION, MACHINE, OR PROCESS IN WHICH THE PRODUCT
 * IS USED. INFORMATION PUBLISHED BY WÜRTH ELEKTRONIK EISOS REGARDING
 * THIRD-PARTY PRODUCTS OR SERVICES DOES NOT CONSTITUTE A LICENSE FROM WÜRTH
 * ELEKTRONIK EISOS TO USE SUCH PRODUCTS OR SERVICES OR A WARRANTY OR
 * ENDORSEMENT THEREOF
 *
 * THIS SOURCE CODE IS PROTECTED BY A LICENSE.
 * FOR MORE INFORMATION PLEASE CAREFULLY READ THE LICENSE AGREEMENT FILE LOCATED
 * IN THE ROOT DIRECTORY OF THIS PACKAGE
 */
#include <Arduino.h>
#include "ArduinoTimer.h"

#define MAX_PRESCALER_VALUE 1024
#define MAX_COUNT16_PERIOD 1398
#define UINT24_MAX (uint32_t)16777215

#define UsesTccHardware(x) ((Timer0 == (x)) || (Timer1 == (x)) || (Timer2 == (x)))
#define UsesTcHardware(x) ((Timer3 == (x)) || (Timer4 == (x)) || (Timer5 == (x)))

/* Store configuration of timer interrupts */
Timer_IRQ_Config timer_irq_config[TIMER_INSTANCES];

typedef union
{
  Tc tcTimer;
  Tcc tccTimer;
} HardwareTimer;

/* private function definition */
/* Basic functions to access timer hardware */
static bool Timer_WaitForSync(HardwareTimer *pTc, TimerInstance instance,
                              uint32_t syncFlag);
static bool Timer_enable(HardwareTimer *pTc, TimerInstance instance);
static bool Timer_disable(HardwareTimer *pTc, TimerInstance instance);
static bool Timer_isInUse(HardwareTimer *pTC, TimerInstance instance);
bool Timer_reset(HardwareTimer *pTc, TimerInstance instance);
static bool Timer_setupClock(TimerInstance instance);

/* Functions to setup hardware */
static bool Timer_setup(Timer *pTimer, TimerCount count, TimerOpMode mode,
                        uint32_t period_ms, void (*irqHandler)(void));
static bool Timer_setupTc(Timer *pTimer, TimerCount count);
static bool Timer_setupTcc(Timer *pTimer);
static bool Timer_setPeriod(Timer *pTimer, uint32_t period_ms, TimerCount count);
static bool Timer_setPeriodTc(Timer *pTimer, TimerCount count, uint16_t prescaler,
                              uint32_t compareValue);
static bool Timer_setPeriodTcc(Timer *pTimer, uint16_t prescaler,
                               uint32_t compareValue);

/* Functions for interrupt configuration */
static void Timer_EnableInterrupt(TimerInstance instance);
static void Timer_DisableInterrupt(TimerInstance instance);
static void Timer_HandleInterrupt(HardwareTimer *pTc, TimerInstance instance);

/* Debug output*/
#define DEBUG
#ifdef DEBUG
char buf[256];
#endif

void OutputDebug(const char format[], ...);
void OutputDebug(const char format[], ...)
{
#ifdef DEBUG
  va_list ap;

  va_start(ap, format);
  vsnprintf(buf, sizeof(buf), format, ap);
  Serial.print(buf);
  va_end(ap);
#endif
}

/* timer handler provided by API
 * Call own handler within */
void TCC0_Handler() { Timer_HandleInterrupt((HardwareTimer *)(TCC0), Timer0); }

void TCC1_Handler() { Timer_HandleInterrupt((HardwareTimer *)(TCC1), Timer1); }

void TCC2_Handler() { Timer_HandleInterrupt((HardwareTimer *)(TCC2), Timer2); }

void TC3_Handler() { Timer_HandleInterrupt((HardwareTimer *)(TC3), Timer3); }

void TC4_Handler() { Timer_HandleInterrupt((HardwareTimer *)(TC4), Timer4); }

void TC5_Handler() { Timer_HandleInterrupt((HardwareTimer *)(TC5), Timer5); }

/**
 * @brief  Interrupt handler
 * @param  pTC Pointer to hardware timer
 * @param  instance Timer instance
 * @retval none
 */
static void Timer_HandleInterrupt(HardwareTimer *pTc, TimerInstance instance)
{
  switch (instance)
  {
  /* TCC hardware */
  case Timer0:
  case Timer1:
  case Timer2:
  {
    if (((Tcc *)pTc)->INTFLAG.bit.MC0 == 1)
    {
      if (NULL != timer_irq_config[instance].irqHandler)
      {
        timer_irq_config[instance].irqHandler();
      }

      if (Timer_OneShot == timer_irq_config[instance].timerMode)
      {
        ((Tcc *)pTc)->INTENCLR.bit.MC0 = 1;
        Timer_disable(pTc, instance);
      }

      ((Tcc *)pTc)->INTFLAG.bit.MC0 = 1;
    }

    break;
  }
  /* TC hardware*/
  case Timer3:
  case Timer4:
  case Timer5:
  {
    if (((Tc *)pTc)->COUNT16.INTFLAG.bit.MC0 == 1)
    {
      if (NULL != timer_irq_config[instance].irqHandler)
      {
        timer_irq_config[instance].irqHandler();
      }

      if (Timer_OneShot == timer_irq_config[instance].timerMode)
      {
        ((Tc *)pTc)->COUNT16.INTENCLR.bit.MC0 = 1;
        Timer_disable(pTc, instance);
      }

      ((Tc *)pTc)->COUNT16.INTFLAG.bit.MC0 = 1;
    }
    break;
  }
  }
}

/**
 * @brief  Create a timer instance
 * @param  pTC Pointer to hardware timer
 * @param  instance Timer instance
 * @retval true if successful else false
 */
bool Timer_create(Timer *pTimer, TimerInstance timerInstance)
{
  if (NULL == pTimer)
  {
    /* Error - can not proceed */
    return false;
  }

  HardwareTimer *pTC = NULL;

  switch (timerInstance)
  {
  case Timer0:
  {
    pTC = (HardwareTimer *)TCC0;
    break;
  }

  case Timer1:
  {
    pTC = (HardwareTimer *)TCC1;
    break;
  }
  case Timer2:
  {
    pTC = (HardwareTimer *)TCC2;
    break;
  }
  case Timer3:
  {
    pTC = (HardwareTimer *)TC3;
    break;
  }

  case Timer4:
  {
    pTC = (HardwareTimer *)TC4;
    break;
  }

  case Timer5:
  {
    pTC = (HardwareTimer *)TC5;
    break;
  }
  }

  /* if timer is not already running, initialize irqHandler*/
  if (false == Timer_isInUse(pTC, timerInstance))
  {
    timer_irq_config[timerInstance].irqHandler = NULL;
  }
  else
  {
    /* Timer already running - set pTC to null */
    OutputDebug("Timer %i is already is use \r\n", timerInstance);
    pTC = NULL;
  }

  pTimer->obj = pTC;
  pTimer->instance = timerInstance;

  /* Timer invalid */
  if (NULL == pTC)
  {
    return false;
  }
  return true;
}

/**
 * @brief  Start the timer
 * @param  pTC Pointer to timer
 * @retval true if successful else false
 */
bool Timer_start(Timer *pTimer)
{
  if (NULL == pTimer)
  {
    /* Error - can not proceed */
    return false;
  }

  HardwareTimer *TC = static_cast<HardwareTimer *>(pTimer->obj);

  if (Timer_isInUse(TC, pTimer->instance))
  {
    return false;
  }

  Timer_EnableInterrupt(pTimer->instance);

  if (UsesTccHardware(pTimer->instance))
  {
    ((Tcc *)TC)->INTENSET.bit.MC0 = 1;
  }
  else if (UsesTcHardware(pTimer->instance))
  {
    ((Tc *)TC)->COUNT16.INTENSET.bit.MC0 = 1;
  }
  else
  {
    return false;
  }

  /* Set the enable flag for the selected timer */
  if (false == Timer_enable(TC, pTimer->instance))
  {
    return false;
  }

  OutputDebug("Timer %u started \r\n", pTimer->instance);

  return true;
}

/**
 * @brief Stop the timer
 * @param  pTC Pointer to timer
 * @retval true if successful else false
 */
bool Timer_stop(Timer *pTimer)
{
  if (NULL == pTimer)
  {
    /* Error - can not proceed */
    return false;
  }

  HardwareTimer *TC = static_cast<HardwareTimer *>(pTimer->obj);

  /* Stop the timer */
  if (false == Timer_disable(TC, pTimer->instance))
  {
    return false;
  }

  if (UsesTccHardware(pTimer->instance))
  {
    ((Tcc *)TC)->INTENCLR.bit.MC0 = 1;
  }
  else if (UsesTcHardware(pTimer->instance))
  {
    ((Tc *)TC)->COUNT16.INTENCLR.bit.MC0 = 1;
  }
  else
  {
    return false;
  }

  Timer_DisableInterrupt(pTimer->instance);

  OutputDebug("Timer %u stopped \r\n", pTimer->instance);

  return true;
}

/**
 * @brief  Enable timer
 * @param  pTC Pointer to hardware timer
 * @param  instance Timer instace
 * @retval true if successful else false
 */
static bool Timer_enable(HardwareTimer *pTc, TimerInstance instance)
{
  if (NULL == pTc)
  {
    return false;
  }

  if (UsesTccHardware(instance))
  {
    ((Tcc *)pTc)->CTRLA.reg |= TCC_CTRLA_ENABLE;
  }
  else if (UsesTcHardware(instance))
  {
    ((Tc *)pTc)->COUNT16.CTRLA.reg |= TC_CTRLA_ENABLE;
  }
  else
  {
    return false;
  }

  if (false == Timer_WaitForSync(pTc, instance, TCC_SYNCBUSY_ENABLE))
  {
    return false;
  }

  OutputDebug("Timer %u enabled \r\n", instance);
  return true;
}

/**
 * @brief  Disable timer
 * @param  pTC Pointer to hardware timer
 * @param  instance Timer instace
 * @retval true if successful else false
 */
static bool Timer_disable(HardwareTimer *pTc, TimerInstance instance)
{
  if (NULL == pTc)
  {
    return false;
  }

  if (UsesTccHardware(instance))
  {
    ((Tcc *)pTc)->CTRLA.reg &= ~TCC_CTRLA_ENABLE;
  }
  else if (UsesTcHardware(instance))
  {
    ((Tc *)pTc)->COUNT16.CTRLA.reg &= ~TC_CTRLA_ENABLE;
  }
  else
  {
    return false;
  }

  if (false == Timer_WaitForSync(pTc, instance, TCC_SYNCBUSY_ENABLE))
  {
    return false;
  }

  OutputDebug("Timer %u disabled\r\n", instance);

  return true;
}

/**
 * @brief  Check if the timer is in use
 * @param  pTC Pointer to hardware timer
 * @param  instance Timer instace
 * @retval true if successful else false
 */
static bool Timer_isInUse(HardwareTimer *pTc, TimerInstance instance)
{
  if (NULL == pTc)
  {
    return false;
  }

  if (UsesTccHardware(instance))
  {
    TCC_CTRLA_Type ctrlA;
    ctrlA.reg = ((Tcc *)pTc)->CTRLA.reg;

    /* Check if timer is already enabled */
    if (TCC_CTRLA_ENABLE == (TCC_CTRLA_ENABLE & ctrlA.reg))
    {
      OutputDebug("Timer already running\r\n");
      return true;
    }

    /* Check if timer is used as slave */
    if (TCC_STATUS_SLAVE == (TCC_STATUS_SLAVE & ctrlA.reg))
    {
      OutputDebug("Timer used as slave\r\n");
      return true;
    }
  }
  else if (UsesTcHardware(instance))
  {
    TC_CTRLA_Type ctrlA;
    ctrlA.reg = ((Tc *)pTc)->COUNT16.CTRLA.reg;

    /* Check if timer is already enabled */
    if (TC_CTRLA_ENABLE == (TC_CTRLA_ENABLE & ctrlA.reg))
    {
      OutputDebug("Timer already running\r\n");
      return true;
    }

    /* Check if timer is used as slave */
    if (TC_STATUS_SLAVE == (TC_STATUS_SLAVE & ctrlA.reg))
    {
      OutputDebug("Timer used as slave\r\n");
      return true;
    }
  }
  return false;
}

/**
 * @brief  Timer reset
 * @param  pTC Pointer to hardware timer
 * @param  instance Timer instace
 * @retval true if successful else false
 */
bool Timer_reset(HardwareTimer *pTc, TimerInstance instance)
{
  if (NULL == pTc)
  {
    /* Error - can not proceed */
    return false;
  }

  if (UsesTccHardware(instance))
  {
    ((Tcc *)pTc)->CTRLA.reg |= TCC_CTRLA_SWRST;
    /* Wait for reset to be performed */
    while (((Tcc *)pTc)->CTRLA.bit.SWRST)
      ;
  }
  else if (UsesTcHardware(instance))
  {
    ((Tc *)pTc)->COUNT16.CTRLA.reg |= TC_CTRLA_SWRST;
    /* Wait for reset to be performed */
    while (((Tc *)pTc)->COUNT16.CTRLA.bit.SWRST)
      ;
  }
  else
  {
    return false;
  }

  OutputDebug("Timer %u reseted \r\n", instance);
  return true;
}

/**
 * @brief  Schedule a timer
 * @param  pTimer Pointer to timer
 * @param  runInStandby 
 * @param  mode Timer mode
 * @param  period_ms Period in milliseconds
 * @param  callback
 * @retval true if successful else false
 */
bool Timer_schedule(Timer *pTimer, bool runInStandby, TimerOpMode mode,
                    int period_ms, void (*callback)(void))
{
  if (NULL == pTimer)
  {
    /* Error - can not proceed */
    return false;
  }

  HardwareTimer *TC = static_cast<HardwareTimer *>(pTimer->obj);

  /* Check if timer is already enabled */
  if (true == Timer_isInUse(TC, pTimer->instance))
  {
    OutputDebug("Setup:Timer already running\r\n");
    return false;
  }

  /* If 16-bit is not enough, use 32 bit - only relevant for Tc hardware*/
  TimerCount count = COUNT16;
  if (MAX_COUNT16_PERIOD < period_ms)
  {
    count = COUNT32;
  }

  if (false == Timer_setup(pTimer, count, mode, period_ms, callback))
  {
    return false;
  }

  /* Standby bit not synchronized */
  if (runInStandby)
  {
    if (UsesTccHardware(pTimer->instance))
    {
      ((Tcc *)TC)->CTRLA.reg |= TCC_CTRLA_RUNSTDBY;
    }
    else if (UsesTcHardware(pTimer->instance))
    {
      ((Tc *)TC)->COUNT16.CTRLA.reg |= TC_CTRLA_RUNSTDBY;
    }
    else
    {
      return false;
    }
  }
  OutputDebug("Timer %u scheduled \r\n", pTimer->instance);
  return true;
}

/**
 * @brief  Schedule a timer
 * @param  pTimer Pointer to timer
 * @param  count 
 * @param  mode Timer mode
 * @param  period_ms Period in milliseconds
 * @param  callback
 * @retval true if successful else false
 */
static bool Timer_setup(Timer *pTimer, TimerCount count, TimerOpMode mode,
                        uint32_t period_ms, void (*irqHandler)(void))
{
  if (NULL == pTimer)
  {
    /* Error - can not proceed */
    return false;
  }

  /* Enable clock to feed timer */
  if (false == Timer_setupClock(pTimer->instance))
  {
    return false;
  }

  /* Setup timer hardware */
  if (UsesTccHardware(pTimer->instance))
  {
    if (false == Timer_setupTcc(pTimer))
    {
      return false;
    }
  }
  else if (UsesTcHardware(pTimer->instance))
  {
    if (false == Timer_setupTc(pTimer, count))
    {
      return false;
    }
  }
  else
  {
    return false;
  }

  /* Set prescaler and capture/compare register according to period */
  if (Timer_setPeriod(pTimer, period_ms, count))
  {
    timer_irq_config[pTimer->instance].irqHandler = irqHandler;
  }
  else
  {
    OutputDebug("Setup: Could not set period\r\n");
    return false;
  }

  if (Timer_OneShot == mode)
  {
    timer_irq_config[pTimer->instance].timerMode = Timer_OneShot;
  }
  else
  {
    timer_irq_config[pTimer->instance].timerMode = Timer_Periodic;
  }

  OutputDebug("Setup done\r\n");

  return true;
}

/**
 * @brief  Set up clock
 * @param  instance Timer instance
 * @retval true if successful else false
 */
static bool Timer_setupClock(TimerInstance instance)
{
  /* Enable TC bus clock
   * The timers are paired on the bus: TCC0<->TCC1;
   * TCC2<->TC3; TC4<->TC5
   * */
  if ((Timer0 == instance) || (Timer1 == instance))
  {
    GCLK->CLKCTRL.reg = (uint16_t)(GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK0 |
                                   GCLK_CLKCTRL_ID(GCM_TCC0_TCC1));
  }
  else if ((Timer2 == instance) || (Timer3 == instance))
  {
    GCLK->CLKCTRL.reg = (uint16_t)(GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK0 |
                                   GCLK_CLKCTRL_ID(GCM_TCC2_TC3));
  }
  else if ((Timer4 == instance) || (Timer5 == instance))
  {
    GCLK->CLKCTRL.reg = (uint16_t)(GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK0 |
                                   GCLK_CLKCTRL_ID(GCM_TC4_TC5));
  }
  else
  {
    return false;
  }

  return true;
}

/**
 * @brief  Set up timer counter
 * @param  pTimer Pointer to timer
 * @param  count  Timer count
 * @retval true if successful else false
 */
static bool Timer_setupTc(Timer *pTimer, TimerCount count)
{
  if (NULL == pTimer)
  {
    return false;
  }

  /* Setup only for Tc, not Tcc */
  if (false == UsesTcHardware(pTimer->instance))
  {
    return false;
  }

  TcCount16 *TC = static_cast<TcCount16 *>(pTimer->obj);

  /* Timer must me disabled to initialize */
  TC->CTRLA.reg &= ~TC_CTRLA_ENABLE;
  if (false == Timer_WaitForSync((HardwareTimer *)TC, pTimer->instance, TCC_SYNCBUSY_ENABLE))
  {
    return false;
  }

  /* Set count mode */
  if (COUNT8 == count)
  {
    TC->CTRLA.reg |= TC_CTRLA_MODE_COUNT8;
  }
  else if (COUNT16 == count)
  {
    TC->CTRLA.reg |= TC_CTRLA_MODE_COUNT16;
  }
  else if (COUNT32 == count)
  {
    /* 32 bit timer only allowed for even timers. Odd timer is used as slave
     */
    if ((pTimer->instance & 0x01) == 0)
    {
      /* Check if slave timer is already running - hardcoded, as only TC4 and
       * TC5 can run in this configuration */
      if (true == Timer_isInUse((HardwareTimer *)TC5, Timer5))
      {
        OutputDebug(
            "Can not set Timer %i to 32-bit mode, because slave timer %i is "
            "already running \r\n",
            pTimer->instance, (pTimer->instance) + 1);
        return false;
      }

      TC->CTRLA.reg |= TC_CTRLA_MODE_COUNT32;
    }
    else
    {
      return false;
    }
  }

  /* Set wave generator to match mode */
  TC->CTRLA.reg |= TC_CTRLA_WAVEGEN_MFRQ;

  return true;
}

/**
 * @brief  Set up Tcc
 * @param  pTimer Pointer to timer
 * @retval true if successful else false
 */
static bool Timer_setupTcc(Timer *pTimer)
{
  if (NULL == pTimer)
  {
    return false;
  }

  /* Setup only for Tcc, not Tc */
  if (false == UsesTccHardware(pTimer->instance))
  {
    return false;
  }

  Tcc *TC = static_cast<Tcc *>(pTimer->obj);

  TC->CTRLA.reg &= ~TCC_CTRLA_ENABLE;
  if (false == Timer_WaitForSync((HardwareTimer *)TC, pTimer->instance, TCC_SYNCBUSY_ENABLE))
  {
    return false;
  }

  /* On trigger event reload or reset counter on next GCLK and reset prescaler
   * counter - not synchronized*/
  TC->CTRLA.bit.PRESCSYNC = TCC_CTRLA_PRESCSYNC_RESYNC_Val;
  /* Do not use dithering. If dithering is used, CCx registers lower bit
   * defines the dithering. We don't need dithering anyway. So disable to have
   * entire CCx range available - not syncronized*/
  TC->CTRLA.bit.RESOLUTION = TCC_CTRLA_RESOLUTION_NONE_Val;
  /* Do not lock register on update - not syncronized */
  TC->CTRLA.bit.ALOCK = 0;

  /* When using MFRQ, the top value is defined by the CC0 register value, for
   * all other waveforms operation the top value is defined by Period (PER)
   * register value - synchronized*/
  TC->WAVE.bit.WAVEGEN = TCC_WAVE_WAVEGEN_MFRQ_Val;
  if (false == Timer_WaitForSync((HardwareTimer *)TC, pTimer->instance, TCC_SYNCBUSY_WAVE))
  {
    return false;
  }
  /* Prescaler and CC reg are configured in Timer_setPeriodTcc*/
  return true;
}

/**
 * @brief  Set up timer period
 * @param  pTimer Pointer to timer
 * @param  period_ms Period in millisecond
 * @param  count  Timer count
 * @retval true if successful else false
 */
static bool Timer_setPeriod(Timer *pTimer, uint32_t period_ms, TimerCount count)
{
  if (NULL == pTimer)
  {
    /* Error - can not proceed */
    return false;
  }

  uint32_t maxValue;
  if ((Timer0 == pTimer->instance) || (Timer1 == pTimer->instance))
  {
    /* CCx size is fix 24 bit - dithering is disabled*/
    maxValue = UINT24_MAX - 1;
  }
  else if (Timer2 == pTimer->instance)
  {
    /* CCx size is fix 16 bit - dithering is disabled*/
    maxValue = UINT16_MAX - 1;
  }
  else if (UsesTcHardware(pTimer->instance))
  {
    /* For Tc hardware, Count defines the CCx size*/
    if (COUNT8 == count)
    {
      maxValue = UINT8_MAX - 1;
    }
    else if (COUNT16 == count)
    {
      maxValue = UINT16_MAX - 1;
    }
    else if (COUNT32 == count)
    {
      maxValue = UINT32_MAX - 1;
    }
    else
    {
      return false;
    }
  }
  else
  {
    return false;
  }
  uint32_t ticksPerPeriod = period_ms * (SystemCoreClock / 1000);

  OutputDebug("Systemclock: %lu \r\n", SystemCoreClock);
  OutputDebug("Ticks per period: %lu \r\n", ticksPerPeriod);

  /* Get minimum factor to represent the period with the available CCx
   * registers
   */
  uint32_t minPreScaler = (ticksPerPeriod / maxValue) + 1;
  if (minPreScaler > MAX_PRESCALER_VALUE)
  {
    OutputDebug("period to large\r\n");
    /* Value can not be represented with the timer */
    return false;
  }

  uint16_t selectedPrescaler = 1;
  while (selectedPrescaler <= MAX_PRESCALER_VALUE)
  {
    if (minPreScaler <= selectedPrescaler)
    {
      break;
    }

    selectedPrescaler = selectedPrescaler << 1;

    /* 32, 128 and 512 are no valid prescalers  */
    if ((selectedPrescaler == 32) || (selectedPrescaler == 128) ||
        (selectedPrescaler == 512))
    {
      selectedPrescaler = selectedPrescaler << 1;
    }
  }
  OutputDebug("Prescaler: %u \r\n", selectedPrescaler);

  uint32_t compareValue = (ticksPerPeriod / selectedPrescaler) - 1;

  OutputDebug("Compare/Capture: %lu \r\n", compareValue);

  /* Set prescaler and compare reg */
  if (UsesTccHardware(pTimer->instance))
  {
    if (false == Timer_setPeriodTcc(pTimer, selectedPrescaler, compareValue))
    {
      return false;
    }
  }
  else if (UsesTcHardware(pTimer->instance))
  {
    if (false ==
        Timer_setPeriodTc(pTimer, count, selectedPrescaler, compareValue))
    {
      return false;
    }
  }
  else
  {
    return false;
  }

  return true;
}

/**
 * @brief  Set up timer period
 * @param  pTimer Pointer to timer
 * @param  count  Timer count
 * @param  prescaler Prescaler
 * @param  compareValue compare value
 * @retval true if successful else false
 */
static bool Timer_setPeriodTc(Timer *pTimer, TimerCount count, uint16_t prescaler,
                              uint32_t compareValue)
{
  if (NULL == pTimer)
  {
    return false;
  }

  uint16_t prescalerRegVal = 0;
  switch (prescaler)
  {
  case 1:
  {
    prescalerRegVal = TC_CTRLA_PRESCALER_DIV1;
    break;
  }
  case 2:
  {
    prescalerRegVal = TC_CTRLA_PRESCALER_DIV2;
    break;
  }
  case 4:
  {
    prescalerRegVal = TC_CTRLA_PRESCALER_DIV4;
    break;
  }
  case 8:
  {
    prescalerRegVal = TC_CTRLA_PRESCALER_DIV8;
    break;
  }
  case 16:
  {
    prescalerRegVal = TC_CTRLA_PRESCALER_DIV16;
    break;
  }
  case 64:
  {
    prescalerRegVal = TC_CTRLA_PRESCALER_DIV64;
    break;
  }
  case 256:
  {
    prescalerRegVal = TC_CTRLA_PRESCALER_DIV256;
    break;
  }
  case 1024:
  {
    prescalerRegVal = TC_CTRLA_PRESCALER_DIV1024;
    break;
  }

  default:
    return false;
    break;
  }

  if (COUNT8 == count)
  {
    TcCount8 *TC = static_cast<TcCount8 *>(pTimer->obj);
    TC->CTRLA.reg |= prescalerRegVal;

    TC->COUNT.reg = map(TC->COUNT.reg, 0, TC->CC[0].reg, 0, compareValue);
    TC->CC[0].reg = compareValue;
  }
  else if (COUNT16 == count)
  {
    TcCount16 *TC = static_cast<TcCount16 *>(pTimer->obj);
    TC->CTRLA.reg |= prescalerRegVal;

    TC->COUNT.reg = map(TC->COUNT.reg, 0, TC->CC[0].reg, 0, compareValue);
    TC->CC[0].reg = compareValue;
  }
  else if (COUNT32 == count)
  {
    TcCount32 *TC = static_cast<TcCount32 *>(pTimer->obj);
    TC->CTRLA.reg |= prescalerRegVal;

    TC->COUNT.reg = map(TC->COUNT.reg, 0, TC->CC[0].reg, 0, compareValue);
    TC->CC[0].reg = compareValue;
  }
  else
  {
    return false;
  }

  /* CCx is synchronized */
  if (false == Timer_WaitForSync(static_cast<HardwareTimer *>(pTimer->obj), pTimer->instance,
                                 TCC_SYNCBUSY_CC0))
  {
    return false;
  }

  return true;
}

/**
 * @brief  Set up timer period
 * @param  pTimer Pointer to timer
 * @param  prescaler Prescaler
 * @param  compareValue compare value
 * @retval true if successful else false
 */
static bool Timer_setPeriodTcc(Timer *pTimer, uint16_t prescaler,
                               uint32_t compareValue)
{
  if (NULL == pTimer)
  {
    return false;
  }

  uint16_t prescalerRegVal = 0;
  switch (prescaler)
  {
  case 1:
  {
    prescalerRegVal = TCC_CTRLA_PRESCALER_DIV1;
    break;
  }
  case 2:
  {
    prescalerRegVal = TCC_CTRLA_PRESCALER_DIV2;
    break;
  }
  case 4:
  {
    prescalerRegVal = TCC_CTRLA_PRESCALER_DIV4;
    break;
  }
  case 8:
  {
    prescalerRegVal = TCC_CTRLA_PRESCALER_DIV8;
    break;
  }
  case 16:
  {
    prescalerRegVal = TCC_CTRLA_PRESCALER_DIV16;
    break;
  }
  case 64:
  {
    prescalerRegVal = TCC_CTRLA_PRESCALER_DIV64;
    break;
  }
  case 256:
  {
    prescalerRegVal = TCC_CTRLA_PRESCALER_DIV256;
    break;
  }
  case 1024:
  {
    prescalerRegVal = TCC_CTRLA_PRESCALER_DIV1024;
    break;
  }

  default:
    return false;
    break;
  }

  Tcc *TC = static_cast<Tcc *>(pTimer->obj);
  TC->CTRLA.reg |= prescalerRegVal;
  TC->CC[0].reg = compareValue;

  /* CCx is synchronized */
  if (false == Timer_WaitForSync((HardwareTimer *)TC, pTimer->instance, TCC_SYNCBUSY_CC0))
  {
    return false;
  }

  return true;
}

/**
 * @brief  Enable timer interrupt
 * @param  instance Timer instance
 * @retval none
 */
static void Timer_EnableInterrupt(TimerInstance instance)
{
  IRQn_Type interrupt;

  switch (instance)
  {
  case Timer0:
  {
    interrupt = TCC0_IRQn;
    break;
  }

  case Timer1:
  {
    interrupt = TCC1_IRQn;
    break;
  }

  case Timer2:
  {
    interrupt = TCC2_IRQn;
    break;
  }

  case Timer3:
  {
    interrupt = TC3_IRQn;
    break;
  }

  case Timer4:
  {
    interrupt = TC4_IRQn;
    break;
  }

  case Timer5:
  {
    interrupt = TC5_IRQn;
    break;
  }

  default:
    return;
  }

  NVIC_DisableIRQ(interrupt);
  NVIC_ClearPendingIRQ(interrupt);
  NVIC_SetPriority(interrupt, 0);
  NVIC_EnableIRQ(interrupt);
}

/**
 * @brief  Disable timer interrupt
 * @param  instance Timer instance
 * @retval none
 */
static void Timer_DisableInterrupt(TimerInstance instance)
{
  IRQn_Type interrupt;

  switch (instance)
  {
  case Timer0:
  {
    interrupt = TCC0_IRQn;
    break;
  }

  case Timer1:
  {
    interrupt = TCC1_IRQn;
    break;
  }

  case Timer2:
  {
    interrupt = TCC2_IRQn;
    break;
  }

  case Timer3:
  {
    interrupt = TC3_IRQn;
    break;
  }

  case Timer4:
  {
    interrupt = TC4_IRQn;
    break;
  }

  case Timer5:
  {
    interrupt = TC5_IRQn;
    break;
  }

  default:
    return;
  }

  NVIC_DisableIRQ(interrupt);
  NVIC_ClearPendingIRQ(interrupt);
}

/**
 * @brief  Wait for sync
 * @param  pTc Pointer to hardware timer
 * @param  instance Timer instance
 * @param  syncFlag
 * @retval true if successful else false
 */
static bool Timer_WaitForSync(HardwareTimer *pTc, TimerInstance instance,
                              uint32_t syncFlag)
{
  if (NULL == pTc)
  {
    /* Error - can not proceed */
    return false;
  }

  /* No maximum retries yet - might be an good idea to avoid hangup of CPU
   * at this point. This could happen if timer hardware is not yet running,
   * e.g. after standby*/

  if (UsesTccHardware(instance))
  {
    /* Tcc has various scnyflags in the Syncbusy register.*/
    while (((Tcc *)pTc)->SYNCBUSY.reg & syncFlag)
      ;
  }
  else if (UsesTcHardware(instance))
  {
    /* Wait for sync flag to clear */
    while (((Tc *)pTc)->COUNT16.STATUS.reg & TC_STATUS_SYNCBUSY)
      ;
  }
  else
  {
    OutputDebug("WaitForSync: Timer %i not supported \r\n", instance);
    return false;
  }

  return true;
}
