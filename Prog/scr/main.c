#include <avr/io.h>
#include <avr/interrupt.h>
#include <avr/pgmspace.h>
#include <avr/wdt.h>
#include <util/delay.h>
#include <string.h>

#include "RGBLED.h"
#include "softPWM.h"
#include "speed.h"
#include "WorkMode.h"
#include "KeyScan.h"
#include "setup.h"

#include "usbdrv.h"

/* Hardware documentation:
 * ATmega-48 @20.000 MHz
 * PD0		D-
 * PD2		D+
 * PD3		KEY_LED1
 * PD4		KEY_LED2
 * PD5-7	JP1-JP3
 * PB0-2	JP4-JP6
 * PC0		Button1
 * PC1		Button2
 * PC2		Button3
 * PC3-PC5	RGBLED(G/R/B)
 */

/* USB report descriptor (length is defined in usbconfig.h)
   This has been changed to conform to the USB keyboard boot
   protocol */
PROGMEM const char usbHidReportDescriptor[USB_CFG_HID_REPORT_DESCRIPTOR_LENGTH]
={
    0x05, 0x01,                    // USAGE_PAGE (Generic Desktop)
    0x09, 0x06,                    // USAGE (Keyboard)
    0xa1, 0x01,                    // COLLECTION (Application)

    0x05, 0x07,                    //   USAGE_PAGE (Keyboard)
    0x19, 0xe0,                    //   USAGE_MINIMUM (Keyboard LeftControl)
    0x29, 0xe7,                    //   USAGE_MAXIMUM (Keyboard Right GUI)
    0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
    0x25, 0x01,                    //   LOGICAL_MAXIMUM (1)
    0x75, 0x01,                    //   REPORT_SIZE (1)
    0x95, 0x08,                    //   REPORT_COUNT (8)
    0x81, 0x02,                    //   INPUT (Data,Var,Abs)
    0x95, 0x01,                    //   REPORT_COUNT (1)
    0x75, 0x08,                    //   REPORT_SIZE (8)
    0x81, 0x03,                    //   INPUT (Cnst,Var,Abs)

/*
    0x95, 0x05,                    //   REPORT_COUNT (5)
    0x75, 0x01,                    //   REPORT_SIZE (1)
    0x05, 0x08,                    //   USAGE_PAGE (LEDs)
    0x19, 0x01,                    //   USAGE_MINIMUM (Num Lock)
    0x29, 0x05,                    //   USAGE_MAXIMUM (Kana)
    0x91, 0x02,                    //   OUTPUT (Data,Var,Abs)
    0x95, 0x01,                    //   REPORT_COUNT (1)
    0x75, 0x03,                    //   REPORT_SIZE (3)
    0x91, 0x03,                    //   OUTPUT (Cnst,Var,Abs)
*/
    0x95, 0x06,                    //   REPORT_COUNT (6)
    0x75, 0x08,                    //   REPORT_SIZE (8)
    0x15, 0x00,                    //   LOGICAL_MINIMUM (0)
    0x25, 0x65,                    //   LOGICAL_MAXIMUM (101)
    0x05, 0x07,                    //   USAGE_PAGE (Keyboard)
    0x19, 0x00,                    //   USAGE_MINIMUM (Reserved (no event indicated))
    0x29, 0x65,                    //   USAGE_MAXIMUM (Keyboard Application)
    0x81, 0x00,                    //   INPUT (Data,Ary,Abs)
    0xc0                           // END_COLLECTION  
};

/* The ReportBuffer contains the USB report sent to the PC */
volatile uchar reportBuffer[8];    /* buffer for HID reports */
volatile uchar idleRate;           /* in 4 ms units */
volatile uchar protocolVer=1;      /* 0 is the boot protocol, 1 is report protocol */

static void hardwareInit(void) {
  PORTB = 0x3F;
  DDRB  = 0x00; 

  PORTC = 0xff;   
  DDRC  = (1<<PC3)|(1<<PC4)|(1<<PC5);
  
  PORTD = 0xfa;   /* 1111 1010 bin: activate pull-ups except on USB lines */
  DDRD  = (1<<PD0)|(1<<PD2)|(1<<PD3)|(1<<PD4);   /* 0000 0111 bin: all pins input except USB (-> USB reset) */

  /* USB Reset by device only required on Watchdog Reset */
  _delay_us(11);   /* delay >10ms for USB reset */ 

  DDRD &=~((1<<PD0)|(1<<PD2));    /* 0000 0010 bin: remove USB reset condition */
  /* configure timer 0 for a rate of 20M/(1024 * 256) = 76.29 Hz (~13ms) */
  TCCR0B = 2;      /* timer 0 prescaler: 512 供软件pwm使用 64分级 */	
  //TIMSK0 |= (1<<TOIE0);
  //timer2 for scanKey 20M/(128 * 256 ) = 610 (~1.6ms)
  TCCR2B = 5;
  TIMSK2 |= (1<<TOIE2);
}

/* Table for decoding bit positions of the last three rows */
//const char extrows[3] PROGMEM = { 0x10, 0x20, 0x08 };

void buildReport(void){
	uchar reportIndex=1; /* First available report entry is 2 */
	uchar i;
	memset(reportBuffer,0,sizeof(reportBuffer)); /* Clear report buffer */
	
	for(i=0;i<KEYamount;i++){
		if(KeyScan_state[i]>0){
			reportBuffer[++reportIndex]=KeyScan_keyValue[i];
		}
		//这里没有判断越界
	}
}

/*
ISR(TIMER0_OVF_vect){
	TCNT0 = 255 - 151;		
    if(KeyScan_keyChanged && usbInterruptIsReady()){
      KeyScan_keyChanged = 0;
	  buildReport();
      usbSetInterrupt(reportBuffer, sizeof(reportBuffer));
    }else{	
		if(TIFR0&(1<<TOV0)){	
			TIFR0 |= 1<<TOV0;
			PWM_Generator();
		}
			
	}
}
*/
volatile uint8_t t2counter = 0;
ISR(TIMER2_OVF_vect){
	TCNT2 = 255 - 151;	
	KeyScan();
	
	if(KeyScan_keyChanged) return;
	t2counter++;
	
	if(t2counter>242){	//22ms重新报告一次
		t2counter = 0;
		KeyScan_keyChanged = 1;	
		
	}
	
	if(t2counter%22==0){
		WorkMode_CLK();
	}
	
	
}

uchar expectReport=0;

uchar usbFunctionSetup(uchar data[8]) {
  usbRequest_t *rq = (void *)data;
  usbMsgPtr = reportBuffer;
  if((rq->bmRequestType & USBRQ_TYPE_MASK) == USBRQ_TYPE_CLASS){    /* class request type */
    if(rq->bRequest == USBRQ_HID_GET_REPORT){  
      /* wValue: ReportType (highbyte), ReportID (lowbyte) */
      /* we only have one report type, so don't look at wValue */
      return sizeof(reportBuffer);
    }/*else if(rq->bRequest == USBRQ_HID_SET_REPORT){
      if (rq->wLength.word == 1) { // We expect one byte reports 
        expectReport=1;
        return 0xFF; // Call usbFunctionWrite with data 
      }  
    }*/else if(rq->bRequest == USBRQ_HID_GET_IDLE){
      usbMsgPtr = &idleRate;
      return 1;
    }else if(rq->bRequest == USBRQ_HID_SET_IDLE){
      idleRate = rq->wValue.bytes[1];
    }/*else if(rq->bRequest == USBRQ_HID_GET_PROTOCOL) {
      if (rq->wValue.bytes[1] < 1) {
        protocolVer = rq->wValue.bytes[1];
      }
    }else if(rq->bRequest == USBRQ_HID_SET_PROTOCOL) {
      usbMsgPtr = &protocolVer;
      return 1;
    }*/
  }
  return 0;
}
/*
uchar usbFunctionWrite(uchar *data, uchar len) {
  expectReport=0;
  return 0x01;
}
*/

int main(void) {
  	wdt_enable(WDTO_2S); /* Enable watchdog timer 2s */
	hardwareInit(); /* Initialize hardware (I/O) */  
  	usbInit(); /* Initialize USB stack processing */
	Setup_init();
  	speed_init();
  	PWM_init();
	KeyScan_init();
	
	switch (Setup_key12LED){
		case Setup_key12LED_Always:
		case Setup_key12LED_OftenOn:
			PWM_setOutputLevel(0,PWM_TotalLevel);
			PWM_setOutputLevel(1,PWM_TotalLevel);			
			break;
		case Setup_key12LED_Never:
		case Setup_key12LED_OftenOff:	
			PWM_setOutputLevel(0,0);
			PWM_setOutputLevel(1,0);			
	}
	


  	sei(); /* Enable global interrupts */
	WorkMode_set(WorkMode_Unused);

  	for(;;){  /* Main loop */
	    wdt_reset(); /* Reset the watchdog */
		
		usbPoll();

	    if(KeyScan_keyChanged && usbInterruptIsReady()){
	      KeyScan_keyChanged = 0;
		  buildReport();
	      usbSetInterrupt(reportBuffer, sizeof(reportBuffer));
	    }else{	
			if(TIFR0&(1<<TOV0)){	
				TIFR0 |= 1<<TOV0;
				PWM_Generator();
			}
				
		}		
  	}
  	return 0;
}
	
