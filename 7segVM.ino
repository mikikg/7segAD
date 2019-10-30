/*
  7-seg voltmeter 30.000 count
*/

//Za tajmer/enkoder
#include "HardwareTimer.h"

#define dig1_on   gpio_write_bit(GPIOA,9, HIGH);
#define dig1_off  gpio_write_bit(GPIOA,9, LOW);
#define dig2_on   gpio_write_bit(GPIOA,10, HIGH);
#define dig2_off  gpio_write_bit(GPIOA,10, LOW);
#define dig3_on   gpio_write_bit(GPIOA,15, HIGH);
#define dig3_off  gpio_write_bit(GPIOA,15, LOW);
#define dig4_on   gpio_write_bit(GPIOB,3, HIGH);
#define dig4_off  gpio_write_bit(GPIOB,3, LOW);
#define dig5_on   gpio_write_bit(GPIOB,6, HIGH);
#define dig5_off  gpio_write_bit(GPIOB,6, LOW);
#define dig6_on   gpio_write_bit(GPIOB,7, HIGH);
#define dig6_off  gpio_write_bit(GPIOB,7, LOW);
#define DP_1 gpio_write_bit(GPIOB,14, LOW); //DP ON
#define DP_0 gpio_write_bit(GPIOB,14, HIGH); //DP OFF
#define A_0  gpio_write_bit(GPIOB,4,  HIGH); //A seg
#define B_0  gpio_write_bit(GPIOB,8,  HIGH); //B seg
#define C_0  gpio_write_bit(GPIOB,15, HIGH); //C seg
#define D_0  gpio_write_bit(GPIOB,13, HIGH); //D seg
#define E_0  gpio_write_bit(GPIOB,12, HIGH); //E seg
#define F_0  gpio_write_bit(GPIOB,5,  HIGH); //F seg
#define G_0  gpio_write_bit(GPIOA,8,  HIGH); //G seg
#define A_1  gpio_write_bit(GPIOB,4,  LOW); //A seg
#define B_1  gpio_write_bit(GPIOB,8,  LOW); //B seg
#define C_1  gpio_write_bit(GPIOB,15, LOW); //C seg
#define D_1  gpio_write_bit(GPIOB,13, LOW); //D seg
#define E_1  gpio_write_bit(GPIOB,12, LOW); //E seg
#define F_1  gpio_write_bit(GPIOB,5,  LOW); //F seg
#define G_1  gpio_write_bit(GPIOA,8,  LOW); //G seg

char My7segBUFF[6] = {'t', 'E', 'S', 't' , ' ' , 'u'};
const int analogInputPin = 2;
int cnt = 0;
int digit_current_pos = 0; //0=prva levo,1=sredina 2=desno
int digit_current_dp = 0; //DP 0=nema, 1=prva levo,2=sredina 3=desno
int tmp_C0, tmp_C1, tmp_C2, tmp_C3, tmp_C4, tmp_C5;
int sensorValue = 0;
long ovs, xx;
int chan_to_read;
int tempRES1, tempRES2;
int range_selector;

//Glavni brojac za enkoder, koristi se TIM2
HardwareTimer timer(2);

// the setup function runs once when you press reset or power the board
void setup() {

  // Setup Timer
  timer.setChannel1Mode(TIMER_OUTPUTCOMPARE);
  timer.setPeriod(100); // in microseconds
  timer.setCompare1(1); // overflow might be small
  timer.setPrescaleFactor(10); //prescaler.
  timer.attachCompare1Interrupt(handler_led);

  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);

  //oslobodi JTAG nozice, iskljuci JTAG
  afio_cfg_debug_ports(AFIO_DEBUG_SW_ONLY);

  //sel
  pinMode(PB3, OUTPUT); //4. cifra
  pinMode(PB6, OUTPUT); //5.cifra
  pinMode(PB7, OUTPUT); //6.cifra
  pinMode(PA9, OUTPUT); //1.cifra
  pinMode(PA10, OUTPUT); //2. cifra
  pinMode(PA15, OUTPUT); //3.cifra
  //segments
  pinMode(PA8, OUTPUT); //G
  pinMode(PB4, OUTPUT); //A
  pinMode(PB5, OUTPUT); //F
  pinMode(PB8, OUTPUT); //B
  pinMode(PB12, OUTPUT); //E
  pinMode(PB13, OUTPUT); //D
  pinMode(PB15, OUTPUT); //C
  pinMode(PB14, OUTPUT); //DP

  //postavi sve hi
  //sel
  digitalWrite(PB3, LOW);
  digitalWrite(PB6, LOW);
  digitalWrite(PB7, LOW);
  digitalWrite(PA9, LOW);
  digitalWrite(PA10, LOW);
  digitalWrite(PA15, LOW);

  //segments
  digitalWrite(PA8,  HIGH); //+G seg
  digitalWrite(PB4,  HIGH); //+A seg
  digitalWrite(PB5,  HIGH); //+F seg
  digitalWrite(PB8,  HIGH); //+B seg
  digitalWrite(PB12, HIGH); //+E seg
  digitalWrite(PB13, HIGH); //+D seg
  digitalWrite(PB15, HIGH); //+C seg
  digitalWrite(PB14, HIGH); //?DP

  pinMode(PA5, INPUT); // range input 0
  pinMode(PA6, INPUT); // range input 1
  pinMode(PA7, INPUT); // range input 2

  // Configure the ADC pin
  pinMode(analogInputPin, INPUT_ANALOG);
  ADC1->regs->SMPR2 = 0xFFFFFFFF;

  delay(1000);

}

//----------------------------------------------------
//Interupt from TIM2, handle 7-seg LED
//----------------------------------------------------
void handler_led() {
  //gpio_write_bit(GPIOC, 13, LOW);

  //all digits off
  dig1_off;
  dig2_off;
  dig3_off;
  dig4_off;
  dig5_off;
  dig6_off;
  DP_0;

  //Set7seg (My7segBUFF[digit_current_pos]);

  switch (My7segBUFF[digit_current_pos]) {
    case 0: A_1; B_1; C_1; D_1; E_1; F_1; G_0; break;
    case 1: A_0; B_1; C_1; D_0; E_0; F_0; G_0; break;
    case 2: A_1; B_1; C_0; D_1; E_1; F_0; G_1; break;
    case 3: A_1; B_1; C_1; D_1; E_0; F_0; G_1; break;
    case 4: A_0; B_1; C_1; D_0; E_0; F_1; G_1; break;
    case 5: A_1; B_0; C_1; D_1; E_0; F_1; G_1; break;
    case 6: A_1; B_0; C_1; D_1; E_1; F_1; G_1; break;
    case 7: A_1; B_1; C_1; D_0; E_0; F_0; G_0; break;
    case 8: A_1; B_1; C_1; D_1; E_1; F_1; G_1; break;
    case 9: A_1; B_1; C_1; D_1; E_0; F_1; G_1; break;
    case 0xA: case 'A': A_1; B_1; C_1; D_0; E_1; F_1; G_1; break;
    case 0xB: A_0; B_0; C_1; D_1; E_1; F_1; G_1; break;
    case 0xC: A_1; B_0; C_0; D_1; E_1; F_1; G_0; break;
    case 0xD: case 'd': A_0; B_1; C_1; D_1; E_1; F_0; G_1; break;
    case 0xE: A_1; B_0; C_0; D_1; E_1; F_1; G_1; break;
    case 0xF: case 'F': A_1; B_0; C_0; D_0; E_1; F_1; G_1; break;
    case 'E': A_1; B_0; C_0; D_1; E_1; F_1; G_1; break;
    case 'H': A_0; B_1; C_1; D_0; E_1; F_1; G_1; break;
    case 'L': A_0; B_0; C_0; D_1; E_1; F_1; G_0; break;
    case 'r': A_0; B_0; C_0; D_0; E_1; F_0; G_1; break;
    case '-': A_0; B_0; C_0; D_0; E_0; F_0; G_1; break;
    case 't': A_0; B_0; C_0; D_1; E_1; F_1; G_1; break;
    case 'S': A_1; B_0; C_1; D_1; E_0; F_1; G_1; break;
    case 'P': A_1; B_1; C_0; D_0; E_1; F_1; G_1; break;
    case ' ': A_0; B_0; C_0; D_0; E_0; F_0; G_0; break;
    case 'o': A_0; B_0; C_1; D_1; E_1; F_0; G_1; break;
    case '=': A_0; B_0; C_0; D_1; E_0; F_0; G_1; break;
    case 'n': A_0; B_0; C_1; D_0; E_1; F_0; G_1; break;
    case 'i': A_0; B_0; C_0; D_0; E_1; F_0; G_0; break;
    case 'I': A_0; B_0; C_0; D_0; E_1; F_1; G_0; break;
    case 'c': A_0; B_0; C_0; D_1; E_1; F_0; G_1; break;
    case 'J': A_0; B_1; C_1; D_1; E_0; F_0; G_0; break;
    case 'u': A_0; B_0; C_1; D_1; E_1; F_0; G_0; break;
    case 'U': A_0; B_1; C_1; D_1; E_1; F_1; G_0; break;
  }

  if (digit_current_pos == 0) {
    if (digit_current_dp == 1) {
      DP_1;
    }
    dig1_on;
  }
  if (digit_current_pos == 1) {
    if (digit_current_dp == 2) {
      DP_1;
      //if (cnt % 2 == 0) {DP_1;} else {
      //  DP_0;}
    }
    dig2_on;
  }
  if (digit_current_pos == 2) {
    if (digit_current_dp == 3) {
      DP_1;
    }
    dig3_on;
  }
  if (digit_current_pos == 3) {
    if (digit_current_dp == 4) {
      DP_1;
    }
    dig4_on;
  }
  if (digit_current_pos == 4) {
    if (digit_current_dp == 5) {
      DP_1;
    }
    dig5_on;
  }
  if (digit_current_pos == 5) {
    if (digit_current_dp == 6) {
      DP_1;
    }
    dig6_on;
  }

  if (digit_current_pos++ >= 5) digit_current_pos = 0;

  /*
    //brigtness
    //small delay
    for (int xx = 0; xx < 400; xx++) {
      gpio_write_bit(GPIOC, 13, HIGH);
    }
    //all digits off
    //dig1_off;
    //dig2_off;
    //dig3_off;
    //dig4_off;
    dig5_off;
    //dig6_off;
  */
  //gpio_write_bit(GPIOC, 13, HIGH);
  //dig5_off;
  //dig6_off;

}

void scroll6 (char c1, char c2, char c3, char c4, char c5, char c6) {
  My7segBUFF[0] = c1; My7segBUFF[1] = c2; My7segBUFF[2] = c3;  delay(1500);
  My7segBUFF[0] = c2; My7segBUFF[1] = c3; My7segBUFF[2] = c4;  delay(100);
  My7segBUFF[0] = c3; My7segBUFF[1] = c4; My7segBUFF[2] = c5;  delay(100);
  My7segBUFF[0] = c4; My7segBUFF[1] = c5; My7segBUFF[2] = c6;  delay(1500);
}

void scroll8 (char c1, char c2, char c3, char c4, char c5, char c6, char c7, char c8) {
  //        12345678
  //Scroll "Error=01";
  My7segBUFF[0] = c1; My7segBUFF[1] = c2; My7segBUFF[2] = c3;  delay(1500);
  My7segBUFF[0] = c2; My7segBUFF[1] = c3; My7segBUFF[2] = c4;  delay(500);
  My7segBUFF[0] = c3; My7segBUFF[1] = c4; My7segBUFF[2] = c5;  delay(500);
  My7segBUFF[0] = c4; My7segBUFF[1] = c5; My7segBUFF[2] = c6;  delay(500);
  My7segBUFF[0] = c5; My7segBUFF[1] = c6; My7segBUFF[2] = c7;  delay(500);
  My7segBUFF[0] = c6; My7segBUFF[1] = c7; My7segBUFF[2] = c8;  delay(1500);
}

void IntTo7seg(int num) { //num=0-99999
    dig=5;
    if (num < 159999) {
        //1, 2, 3,....
        //digit_current_dp = 2;*/
        while (dig--) {
            My7segBUFF[dig]=num%10;
            num/=10;
        }
    } else {
        My7segBUFF[0] = 'H';
        My7segBUFF[1] = 'i';
        My7segBUFF[2] = 'E';
        My7segBUFF[3] = 'r';
        My7segBUFF[4] = 'r';
        digit_current_dp = 0;
    }
}

// the loop function runs over and over again forever
void loop() {

  /*
    My7segBUFF[0]='t';My7segBUFF[1]='S';My7segBUFF[2]='t';  delay(2000);
    My7segBUFF[0]='H';My7segBUFF[1]='L';My7segBUFF[2]='P';  delay(2000);
    //Scroll "Error=01";
    scroll8('J','E','J','A',' ','=',0,1);
    scroll8('E','r','r',' ',' ','=',0,1);
    scroll8('P','i','d',' ',' ','=',0,1);
    scroll8('P','r','o',' ',' ','=',2,4);
    scroll8('I','n','t',' ',' ','=',5,3);
    scroll8('d','i','F',' ',' ','=',8,2);
    scroll8('c','n','t',' ',' ','=',9,9);
    scroll6('E','r','r','=',0,1);
    scroll6('P','i','d','=',0,1);
    scroll6('P','r','o','=',2,4);
    scroll6('I','n','t','=',5,3);
    scroll6('d','i','F','=',8,2);
    scroll6('c','n','t','=',9,9);
    scroll6('t','c',' ','=',2,5);
  */

  ovs = 0;

  for (xx = 0; xx < 7588; xx++) {
    ovs += analogRead(chan_to_read);
  }

  tempRES1 = ovs / 100;
  tempRES2;

  if (tempRES1 > 200) {
    tempRES2 = tempRES1 + 190;
  } else {
    tempRES2 = tempRES1;
  }

  range_selector = 0; //1=200mV, 2=2V, 3=20V, 4=200V, 5=1200V
  if (digitalRead(PA5)) range_selector += 1;
  if (digitalRead(PA6)) range_selector += 2;
  if (digitalRead(PA7)) range_selector += 4;

  digit_current_dp = range_selector - 1;

  if (digit_current_dp) {
    IntTo7seg(tempRES2);
    My7segBUFF[5] = 'u';
    chan_to_read = 2;
  } else {
    //Vbat
    digit_current_dp = 3;
    IntTo7seg(tempRES2 / 50 + 496);
    //My7segBUFF[1]=' ';
    My7segBUFF[5] = 'u';
    chan_to_read = 3;
    My7segBUFF[0] = 0xb; My7segBUFF[1] = '=';  ;

  }

  //delay(10);

  cnt++;

}


