void setup() {
  pinMode(11, OUTPUT);  // OC2A = pin 11

    // Phase Correct PWM, TOP=255 (mode 1)
  // TCCR2A = _BV(COM2A1) | _BV(WGM20);
  // TCCR2B = _BV(CS20);  // no prescaler (N=1)

  // Fast PWM, TOP=255 (mode 3)
  TCCR2A = _BV(COM2A1) | _BV(WGM21) | _BV(WGM20);
  TCCR2B = _BV(CS20);  // no prescaler
//Fast PWM, TOP = 255 (mode 3)
//TCCR2A = _BV(COM2A1) | _BV(WGM21) | _BV(WGM20);

// // Set prescaler = 8  --> ~7.8 kHz
// TCCR2B = _BV(WGM22) | _BV(CS21);

  // Frequency = 16 MHz / (256 * 1) = 62.5 kHz
  // To get ~31.25 kHz, we need phase-correct PWM instead of fast PWM:
  // → Frequency = 16 MHz / (510 * 1) ≈ 31.37 kHz
}

void loop() {
  // Duty cycle control (0–255)
  analogWrite(11, 153);  // ~25%
  delay(2000);

  analogWrite(11, 132);  // ~50%
  delay(2000);

  // analogWrite(11, 112);  // ~75%
  delay(2000);

  analogWrite(11, 111);  // ~100%
  delay(2000);

  // analogWrite(11, 0);  // 0%
  delay(2000);
}



















// void setup() {
//   pinMode(9, OUTPUT);

//   // Set Timer1 to Fast PWM mode with ICR1 as TOP
//   TCCR1A = _BV(COM1A1) | _BV(WGM11);
//   TCCR1B = _BV(WGM13) | _BV(WGM12) | _BV(CS10); // No prescaler

//   ICR1 = 533;   // TOP → ~30kHz
// }

// void loop() {

//   OCR1A =133 ;
//   delay(5000);
//   OCR1A =267 ;
//   delay(5000);
//   OCR1A =533 ;
//   delay(5000);


//   // // Ramp up duty cycle from 100 to 533
//   // for (int duty = 100; duty <= 533; duty++) {
//   //   OCR1A = duty;          // Set duty cycle
//   //   delay(10);             // Adjust ramp speed (10 ms step)
//   // }

//   // // Ramp down duty cycle from 533 back to 100
//   // for (int duty = 533; duty >= 100; duty--) {
//   //   OCR1A = duty;
//   //   delay(10);
//   // }
// }
