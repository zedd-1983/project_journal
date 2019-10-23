/***********************************************************************************************************************
 * This file was generated by the MCUXpresso Config Tools. Any manual edits made to this file
 * will be overwritten if the respective MCUXpresso Config Tools is used to update this file.
 **********************************************************************************************************************/

/* clang-format off */
/* TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
!!GlobalInfo
product: Peripherals v6.0
processor: MK64FN1M0xxx12
package_id: MK64FN1M0VLL12
mcu_data: ksdk2_0
processor_version: 6.0.1
board: FRDM-K64F
functionalGroups:
- name: BOARD_InitPeripherals
  called_from_default_init: true
  selectedCore: core0
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS **********/

/* TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
component:
- type: 'system'
- type_id: 'system'
- global_system_definitions: []
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS **********/

/* TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
component:
- type: 'msg'
- type_id: 'msg_6e2baaf3b97dbeef01c0043275f9a0e7'
- global_messages: []
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS **********/
/* clang-format on */

/***********************************************************************************************************************
 * Included files
 **********************************************************************************************************************/
#include "peripherals.h"

/***********************************************************************************************************************
 * BOARD_InitPeripherals functional group
 **********************************************************************************************************************/
/***********************************************************************************************************************
 * RTC_1 initialization code
 **********************************************************************************************************************/
/* clang-format off */
/* TEXT BELOW IS USED AS SETTING FOR TOOLS *************************************
instance:
- name: 'RTC_1'
- type: 'rtc'
- mode: 'general'
- type_id: 'rtc_603f70732a5387a85b5715615cba9e65'
- functional_group: 'BOARD_InitPeripherals'
- peripheral: 'RTC'
- config_sets:
  - fsl_rtc:
    - clockConfig_t: []
    - rtc_config:
      - updateMode: 'false'
      - supervisorAccess: 'false'
      - compensationIntervalInt: '1'
      - compensationTimeInt: '0'
      - setDateTime: 'true'
      - rtc_datetime:
        - year: '2019'
        - month: '10'
        - day: '23'
        - hour: '12'
        - minute: '00'
        - second: '0'
      - setAlarm: 'false'
      - start: 'true'
    - interruptsCfg:
      - interruptSources: 'kRTC_AlarmInterruptEnable'
      - isSecondsInterruptEnabled: 'true'
      - secondsInterrupt:
        - IRQn: 'RTC_Seconds_IRQn'
        - enable_priority: 'true'
        - priority: '12'
        - enable_custom_name: 'false'
      - isInterruptEnabled: 'true'
      - commonInterrupt:
        - IRQn: 'RTC_IRQn'
        - enable_priority: 'true'
        - priority: '11'
        - enable_custom_name: 'false'
 * BE CAREFUL MODIFYING THIS COMMENT - IT IS YAML SETTINGS FOR TOOLS **********/
/* clang-format on */
const rtc_config_t RTC_1_config = {
  .wakeupSelect = false,
  .updateMode = false,
  .supervisorAccess = false,
  .compensationInterval = 0x0U,
  .compensationTime = 0x0U
};
rtc_datetime_t RTC_1_dateTimeStruct = {
  .year = 2019,
  .month = 10,
  .day = 23,
  .hour = 12,
  .minute = 00,
  .second = 0
};

void RTC_1_init(void) {
  /* RTC initialization */
  RTC_Init(RTC_1_PERIPHERAL, &RTC_1_config);
  /* Stop RTC timer */
  RTC_StopTimer(RTC_1_PERIPHERAL);
  /* Date and time initialization */
  RTC_SetDatetime(RTC_1_PERIPHERAL, &RTC_1_dateTimeStruct);
  /* Start RTC timer */
  RTC_StartTimer(RTC_1_PERIPHERAL);
  /* Enable interrupts */
  RTC_EnableInterrupts(RTC_1_PERIPHERAL, (kRTC_AlarmInterruptEnable));
  /* Interrupt vector RTC_IRQn priority settings in the NVIC */
  NVIC_SetPriority(RTC_IRQn, RTC_1_COMMON_IRQ_PRIORITY);
  /* Enable interrupt RTC_IRQn request in the NVIC */
  EnableIRQ(RTC_IRQn);
  /* Interrupt vector RTC_Seconds_IRQn priority settings in the NVIC */
  NVIC_SetPriority(RTC_Seconds_IRQn, RTC_1_SECONDS_IRQ_PRIORITY);
  /* Enable interrupt RTC_Seconds_IRQn request in the NVIC */
  EnableIRQ(RTC_Seconds_IRQn);
}

/***********************************************************************************************************************
 * Initialization functions
 **********************************************************************************************************************/
void BOARD_InitPeripherals(void)
{
  /* Initialize components */
  RTC_1_init();
}

/***********************************************************************************************************************
 * BOARD_InitBootPeripherals function
 **********************************************************************************************************************/
void BOARD_InitBootPeripherals(void)
{
  BOARD_InitPeripherals();
}
