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

#ifndef ARDUINOTIMER_H
#define ARDUINOTIMER_H

/**         Includes         */

#ifdef __cplusplus
extern "C"
{
#endif
#define TIMER_INSTANCES 6

  /* Maps to TCC0-TCC2 and TC3 - TC5.*/
  typedef enum TimerInstance
  {
    Timer0 = 0,
    Timer1,
    Timer2,
    Timer3,
    Timer4,
    Timer5,
  } TimerInstance;

  typedef enum TimerOpMode
  {
    Timer_OneShot,
    Timer_Periodic,
  } TimerOpMode;

  typedef enum TimerCount
  {
    COUNT8,
    COUNT16,
    COUNT32,
  } TimerCount;

  typedef void (*timerIRQHandler)(void);

  typedef struct Timer_IRQ_Config
  {
    timerIRQHandler irqHandler;
    TimerOpMode timerMode;
  } Timer_IRQ_Config;

  typedef struct Timer
  {
    TimerInstance instance;
    void *obj;
  } Timer;

  bool Timer_create(Timer *pTimer, TimerInstance timerInstance);
  bool Timer_start(Timer *pTimer);
  bool Timer_stop(Timer *pTimer);

  bool Timer_schedule(Timer *pTimer, bool runInStandby, TimerOpMode mode,
                      int period_ms, void (*callback)(void));

#ifdef __cplusplus
}
#endif

#endif /* ARDUINOTIMER_H */
