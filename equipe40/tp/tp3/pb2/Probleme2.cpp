#define F_CPU 8000000

#include <util/delay.h>
#include <avr/io.h> 




const uint8_t DDR_OUT = 0xff;
const uint8_t DDR_ACTIVE = 10;
const uint8_t DDR_OFF = 00;


const double TIME_PER_INTENSITY_STATE = 2;	//s
const double FREQUENCY_1 = 60;				//Hz
const double PERIOD_1 = 1/FREQUENCY_1;		//s

const double FREQUENCY_2 = 400;				//Hz
const double PERIOD_2 = 1/FREQUENCY_2;		//s

const double TO_MICR0SECOND = 1000000;
const double MAX_INTENSITY = 1;
const int LATENCY_REDUCER = 10;

enum IntensityState{
    I_0,
    I_25,
    I_50,
    I_75,
    I_100
};

enum FrequencyState{
    F_60,
    F_400
};

void variableMicroSecDelay(int delay){
    
    for(int i = 0; i < delay/LATENCY_REDUCER; ++i){
        _delay_us(1*LATENCY_REDUCER);

    }
}

void changeFrequencyState(FrequencyState f_State, double& period){
	switch (f_State){
		case F_60:
			f_State = F_400;
			period = PERIOD_2;
			break;
		case F_400:
			f_State = F_60;
			period = PERIOD_1;
			break;
	}

}

void changeStates(FrequencyState& f_State, IntensityState& i_State, double& intensity, double& period){
	switch (i_State){
		case I_0:
			i_State = I_25;
			intensity = 0.25;
			break;
		case I_25:
			i_State = I_50;
			intensity = 0.50;
			break;
		case I_50:
			i_State = I_75;
			intensity = 0.75;
			break;
		case I_75:
			i_State = I_100;
			intensity = 1.00;
			break;
		case I_100:
			i_State = I_0;
			intensity = 0.00;
			changeFrequencyState(f_State, period);
			break;
	}



}

int main()
{
    double intensity = 0;
	double period = PERIOD_1;
    
    double highTime = 0;
    double lowTime = 0;

	FrequencyState f_State = F_60;
	IntensityState i_State = I_0;

    DDRB = DDR_OUT;

    while(true){

        
        
        for(double i = 0; i < TIME_PER_INTENSITY_STATE ;i = i + period){
            PORTB = DDR_ACTIVE;

            highTime = intensity*period;   

            variableMicroSecDelay(highTime*TO_MICR0SECOND);

            PORTB = DDR_OFF;


            lowTime = period - highTime;         

            variableMicroSecDelay(lowTime*TO_MICR0SECOND);

        }

		changeStates(f_State, i_State, intensity, period);

    }
    return 0;
}