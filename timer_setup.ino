//
//In this file the timers for reading the receiver pulses and for creating the output ESC pulses are set.
//More information can be found in these two videos:
//STM32 for Arduino - Connecting an RC receiver via input capture mode: https://youtu.be/JFSFbSg0l2M
//STM32 for Arduino - Electronic Speed Controller (ESC) - STM32F103C8T6: https://youtu.be/Nju9rvZOjVQ
//
void timer_setup(void) {
  HardwareTimer Timer2(2);
  Timer2.attachCompare1Interrupt(handler_channel_1);
  TIM2_BASE->CR1 = TIM_CR1_CEN;
  TIM2_BASE->CR2 = 0;
  TIM2_BASE->SMCR = 0;
  TIM2_BASE->DIER = TIM_DIER_CC1IE;
  TIM2_BASE->EGR = 0;
  TIM2_BASE->CCMR1 = TIM_CCMR1_CC1S_INPUT_TI1;
  TIM2_BASE->CCMR2 = 0;
  TIM2_BASE->CCER = TIM_CCER_CC1E;
  //TIM2_BASE->CCER |= TIM_CCER_CC1P;    //Detect falling edge.
  TIM2_BASE->CCER &= ~TIM_CCER_CC1P; //Detect rising edge.
  TIM2_BASE->PSC = 71;
  TIM2_BASE->ARR = 0xFFFF;
  TIM2_BASE->DCR = 0;

  //A test is needed to check if the throttle input is active and valid. Otherwise the ESC's might start without any warning.
  loop_counter = 0;
  while ((channel_3 > 2100 || channel_3 < 900) && warning == 0) {
    delay(100);
    loop_counter++;
    if (loop_counter == 40) {
      Serial.println(F("Waiting for a valid receiver channel-3 input signal"));
      Serial.println(F(""));
      Serial.println(F("The input pulse should be between 1000 till 2000us"));
      Serial.print(F("Current channel-3 receiver input value = "));
      Serial.println(channel_3);
      Serial.println(F(""));
      Serial.println(F("Is the receiver connected and the transmitter on?"));
      Serial.println(F("For more support and questions: www.brokking.net"));
      Serial.println(F(""));
      Serial.print(F("Waiting for another 5 seconds."));
    }
    if (loop_counter > 40 && loop_counter % 10 == 0)Serial.print(F("."));

    if (loop_counter == 90) {
      Serial.println(F(""));
      Serial.println(F(""));
      Serial.println(F("The ESC outputs are disabled for safety!!!"));
      warning = 1;
    }
  }
  if (warning == 0) {
    TIM4_BASE->CR1 = TIM_CR1_CEN | TIM_CR1_ARPE;
    TIM4_BASE->CR2 = 0;
    TIM4_BASE->SMCR = 0;
    TIM4_BASE->DIER = 0;
    TIM4_BASE->EGR = 0;
    TIM4_BASE->CCMR1 = (0b110 << 4) | TIM_CCMR1_OC1PE | (0b110 << 12) | TIM_CCMR1_OC2PE;
    TIM4_BASE->CCMR2 = (0b110 << 4) | TIM_CCMR2_OC3PE | (0b110 << 12) | TIM_CCMR2_OC4PE;
    TIM4_BASE->CCER = TIM_CCER_CC1E | TIM_CCER_CC2E | TIM_CCER_CC3E | TIM_CCER_CC4E;
    TIM4_BASE->PSC = 71;
    TIM4_BASE->ARR = 4000;
    TIM4_BASE->DCR = 0;
    TIM4_BASE->CCR1 = 1000;

    TIM4_BASE->CCR1 = channel_3;
    TIM4_BASE->CCR2 = channel_3;
    TIM4_BASE->CCR3 = channel_3;
    TIM4_BASE->CCR4 = channel_3;
    pinMode(PB6, OUTPUT);
    pinMode(PB7, OUTPUT);
    pinMode(PB8, OUTPUT);
    pinMode(PB9, OUTPUT);
  }
}

