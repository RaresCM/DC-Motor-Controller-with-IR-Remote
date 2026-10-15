#include <avr/io.h>
#include <util/delay.h>
// 0xBA45FF00 = POWER
// 0xB847FF00 = FUNC/STOP
// 0xB946FF00 = VOL+
// 0xBB44FF00 = FAST BACK
// 0xBF40FF00 = PAUSE
// 0xBC43FF00 = FAST FORWARD
// 0xF807FF00 = DOWN
// 0xEA15FF00 = VOL-
// 0xF609FF00 = UP
// 0xE619FF00 = EQ
// 0xF20DFF00 = ST/REPT
// 0xE916FF00 = 0
// 0xF30CFF00 = 1
// 0xE718FF00 = 2
// 0xA15EFF00 = 3
// 0xF708FF00 = 4
// 0xE31CFF00 = 5
// 0xA55AFF00 = 6
// 0xBD42FF00 = 7
// 0xAD52FF00 = 8
// 0xB54AFF00 = 9

static uint32_t last_decoded_raw = 0;

uint32_t readIR(void) {
    if (PINE & (1 << PE5)) return 0; //Wait for initial 9ms low pulse to show start or return 0 if line is currently high
    
    while (!(PINE & (1 << PE5))); //Wait for pin to go High

    TCNT1 = 0;//Measure how long high pulse is
    while (PINE & (1 << PE5));// Wait for pin to go Low

    if (TCNT1 < 700) {//Check if it is a repeat code
        return last_decoded_raw;// Return the last button pressed
    } 

    uint32_t data = 0;

    for (int i = 0; i < 32; i++) {//Decode 32 bit data stream
        while (!(PINE & (1 << PE5)));//Wait for low pulse initialisation

        TCNT1 = 0;//Measure the high pulse duration to determine if bit is 0 or 1
        while (PINE & (1 << PE5));//Check if its 1 or 0 and shift it into data.
        data >>= 1;
        if (TCNT1 > 250) {
            data |= 0x80000000; 
        }
    }

    last_decoded_raw = data;
    return data;
}

void setup(void) {
    DDRE &= ~(1 << DDE5);//D3 IR input pin
    PORTE |= (1 << PE5);
    TCCR1B |= (1 << CS11) | (1 << CS10);//Prescaler for timer 1

    DDRG |= (1 << DDG5);//D4 Motor On\Off
    PORTG &= ~(1 << PG5);

    DDRE |= (1 << DDE4);//D2 for PWM for motor control
    PORTE &= ~(1 << PE4);
    TCCR3A |= (1 << COM3B1) | (1 << WGM31);
    TCCR3B |= (1 << WGM33) | (1 << WGM32) | (1 << CS31);
    ICR3 = 2000;
    OCR3B = 250;
}

int main(void){
    setup();
    uint32_t data = 0;
    uint16_t speed = 0;

    while(1) {
        if(!(PINE & (1 << PE5))) {//Look for low initilisation pulse
            data = readIR();//Read IR signal and place into data
        }

        if(data != 0) {//Only proccesses command if there is a new input
            if(data == 0xBA45FF00) {//On/Off logic
                PORTG |= (1 << PG5);
            }   else if(data == 0xB847FF00) {
                PORTG &= ~(1 << PG5);
            }

            if(0xBC43FF00 == data) {//Speed up
                if(speed < 1750) {
                    speed += 250;
                    OCR3B = speed;
                }
            } else if(0xBB44FF00 == data) {//Speed down
                if(speed > 250) {
                    speed -= 250;
                    OCR3B = speed;
                }
            }

            data = 0;
            _delay_ms(250);
        }
    }
}