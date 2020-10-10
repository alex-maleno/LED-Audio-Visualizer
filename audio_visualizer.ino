// Alex Maleno
// audio_visualizer.ino

#include <FastLED.h>

#define NUM_LEDS 150
#define LED1_PIN 6
#define LED2_PIN 7

//declaring spectrum shield pin connections
#define STROBE 4
#define RESET 5
#define DC_Two A1

//define spectrum variables
int freq_amp;
double leftFreq[7];

//array of ints seeing which freq is the most "active"
int activity[7];
int count;

//used to determine which frequency we are looking at
int thisFreq;

//determine the thresholds for each color
int LB; //lower bound
int range;

CRGB led[NUM_LEDS];

struct color {
  int red = 0;
  int green = 0;
  int blue = 0;
};

typedef struct color Color;

void setup() {
  Serial.begin(9600);

  thisFreq = 0;
  LB = 115;
  range = 120; //in the other sketches, range = 120

  //Set spectrum Shield pin configurations
  pinMode(STROBE, OUTPUT); //pin 4
  pinMode(RESET, OUTPUT); //pin 5
  pinMode(DC_Two, INPUT); //A1
  digitalWrite(STROBE, HIGH);
  digitalWrite(RESET, HIGH);

  //Initialize Spectrum Analyzers
  digitalWrite(STROBE, LOW);
  delay(1);
  digitalWrite(RESET, HIGH);
  delay(1);
  digitalWrite(STROBE, HIGH);
  delay(1);
  digitalWrite(STROBE, LOW);
  delay(1);
  digitalWrite(RESET, LOW);


  FastLED.addLeds<NEOPIXEL, LED1_PIN>(led, NUM_LEDS);
  FastLED.addLeds<NEOPIXEL, LED2_PIN>(led, NUM_LEDS);

  Color c;

  //starts the strip off as all white
  for (int i = 0; i < NUM_LEDS; i++) {
    led [i] = CRGB(c.red, c.green, c.blue);
  }

  FastLED.show();
  
  for (int i = 0; i < 7; i++){
    activity[i] = 0;
  }
  
  count = 0;

}

//sets the first 4 LEDs the randomly generated color
void setFirst(Color c) {
  led[0] = CRGB(c.red, c.green, c.blue);

  FastLED.show();
}

//shifts the LED colors over in groups of 5
void shift() {
  shiftBack10();

  for (int i = 135; i > 9; i -= 5) {
    for (int j = 0; j < 5; j++) {
      led[i + j] = led[i - 1];
    }
  }

  shiftFront10();

  FastLED.show();
}

//slowly adds in the new colors
void shiftFront10() {
  //group of 4
  for (int i = 6; i < 7; i++) { //set i = the first element in the group, then j goes from
    // 0 -> desired size of the group
    for (int j = 0; j < 4; j++) {
      led[i + j] = led[i - 1];
    }
  }

  //group of 3
  for (int i = 3; i < 4; i++) {
    for (int j = 0; j < 3; j++) {
      led[i + j] = led[i - 1];
    }
  }

  //group of 2
  for (int i = 1; i < 2; i++) {
    for (int j = 0; j < 2; j++) {
      led[i + j] = led[i - 1];
    }
  }

}

//slowly takes away the old colors
void shiftBack10() {
  //last LED
  led[149] = led[148];

  //group of 2
  for (int i = 147; i < 148; i++) {
    for (int j = 0; j < 2; j++) {
      led[i + j] = led[i - 1];
    }
  }

  //group of 3
  for (int i = 144; i < 145; i++) {
    for (int j = 0; j < 3; j++) {
      led[i + j] = led[i - 1];
    }
  }

  //group of 4
  for (int i = 140; i < 141; i++) { //set i = the first element in the group, then j goes from
    // 0 -> desired size of the group
    for (int j = 0; j < 4; j++) {
      led[i + j] = led[i - 1];
    }
  }

}

//reads in the frequencies for each band
void read_frequencies() {
  for (freq_amp = 0; freq_amp < 7; freq_amp++) {
    leftFreq[freq_amp] = analogRead(DC_Two);
    digitalWrite(STROBE, HIGH);
    digitalWrite(STROBE, LOW);
  }
  
  
}

int average(){
  int sum = 0;
  for (int i = 0; i < 7; i++){
      sum += leftFreq[i];
    }
  return sum / 7;
}

// Activity Scale
// - 1 -> no activity
// + 1 -> minimal activity
// + 2 -> active
// + 1 -> very active
// The purpose of this is to rate which frequency bands are currently playing in the music, and
// trying to prevent just a single color being displayed on the LED strips
void setActivity(){
  for (int i = 0; i < 7; i++){
    if (leftFreq[i] < LB and activity[i] > 0){ //only decrement the activity if there has been activity
      activity[i] += 2; //changed from last version
    } else if (leftFreq[i] >= LB and leftFreq[i] < LB + 3*range){
      activity[i] += 1;
    } else if (leftFreq[i] >= LB + 3*range and leftFreq[i] < LB + 17*range){
      activity[i] += 2;
    } else if (leftFreq[i] >= LB + 17*range){
      activity[i] += 1;
    }
  }
  
  if (count >= 20){
    for (int i = 0; i < 7; i++){
      activity[i] = 0;
    }
    count = 0;
  }
}


//calls setActivity to analyze the frequencies
void fixFreq(){
  if(average() > LB and average() < LB + 2*range){
    for (int i = 0; i < 7; i++){
      leftFreq[i] *= 3;
    }
  } else if (average() >= 2*range and average() < LB + 6*range){
    for (int i = 0; i < 7; i++){
      leftFreq[i] *= 2;
    }
  }  else if (average() < LB and average() > LB - 50){
    for (int i = 0; i < 7; i++){
      leftFreq[i] *= 4;
    }
  }  else if (average() > 0 and average() < LB - 50){
    for (int i = 0; i < 7; i++){
      leftFreq[i] = 0;
    }
  } 
  
  setActivity();
}


// I manually chose the different RBG values for different frequency ranges, so this is not
// the prettiest function
Color setColor (Color c){
  if (leftFreq[thisFreq] >= LB and leftFreq[thisFreq] < LB + range) { 
    //soft purple
    c.red = 110;
    c.green = 0;
    c.blue = 255;
  } else if (leftFreq[thisFreq] >= LB +  range and leftFreq[thisFreq] < LB +  2*range) { 
    //dark purple
    c.red = 113;
    c.green = 47;
    c.blue = 198;
  } else if (leftFreq[thisFreq] >= LB + 2*range and leftFreq[thisFreq] < LB +  3*range) { 
    //bright purple
    c.red = 207;
    c.green = 112;
    c.blue = 239;
  } else if (leftFreq[thisFreq] >= LB + 3*range and leftFreq[thisFreq] < LB +  4*range) { 
    //dark blue
    c.red = 13;
    c.green = 13;
    c.blue = 216;
  } else if (leftFreq[thisFreq] >= LB + 4*range and leftFreq[thisFreq] < LB +  5*range) { 
    //sky blue
    c.red = 0;
    c.green = 131;
    c.blue = 255;
  } else if (leftFreq[thisFreq] >= LB + 5*range and leftFreq[thisFreq] < LB +  6*range) { 
    //cyan
    c.red = 56;
    c.green = 238;
    c.blue = 255;
  } else if (leftFreq[thisFreq] >= LB + 6*range and leftFreq[thisFreq] < LB +  7*range) { 
    //dark green
    c.red = 19;
    c.green = 173;
    c.blue = 39;
  } else if (leftFreq[thisFreq] >= LB + 7*range and leftFreq[thisFreq] < LB +  8*range) { 
    //greenish
    c.red = 131;
    c.green = 196;
    c.blue = 134;
  } else if (leftFreq[thisFreq] >= LB + 8*range and leftFreq[thisFreq] < LB +  9*range) { 
    //neon green
    c.red = 2;
    c.green = 255;
    c.blue = 36;
  } else if (leftFreq[thisFreq] >= LB + 9*range and leftFreq[thisFreq] < LB +  10*range) { 
    //dark yellow
    c.red = 226;
    c.green = 223;
    c.blue = 47;
  } else if (leftFreq[thisFreq] >= LB + 10*range and leftFreq[thisFreq] < LB +  11*range) { 
    //light yellow
    c.red = 255;
    c.green = 253;
    c.blue = 145;
  } else if (leftFreq[thisFreq] >= LB + 11*range and leftFreq[thisFreq] < LB +  12*range){ 
    //bright yellow
    c.red = 255;
    c.green = 255;
    c.blue = 2;
  } else if (leftFreq[thisFreq] >= LB + 12*range and leftFreq[thisFreq] < LB +  13*range){ 
    //light orange
    c.red = 255;
    c.green = 196;
    c.blue = 89;
  } else if (leftFreq[thisFreq] >= LB + 13*range and leftFreq[thisFreq] < LB +  14*range){ 
    //dark orange
    c.red = 255;
    c.green = 165;
    c.blue = 0;
  } else if (leftFreq[thisFreq] >= LB + 14*range and leftFreq[thisFreq] < LB + 15*range) { 
    //burgandy
    c.red = 133;
    c.green = 33;
    c.blue = 33;
  } else if (leftFreq[thisFreq] >= LB + 15*range and leftFreq[thisFreq] < LB +  16* range) { 
    //dark red
    c.red = 183;
    c.green = 20;
    c.blue = 20;
  } else if (leftFreq[thisFreq] >= LB + 16*range and leftFreq[thisFreq] < LB + 17*range) { 
    //bright red
    c.red = 255;
    c.green = 56;
    c.blue = 56;
  } else if (leftFreq[thisFreq] >= LB + 17*range and leftFreq[thisFreq] < LB + 18*range) { 
    //light red/salmon
    c.red = 239;
    c.green = 186;
    c.blue = 186;
  } else if (leftFreq[thisFreq] >= LB + 18*range and leftFreq[thisFreq] < LB + 19*range) { 
    //pink
    c.red = 255;
    c.green = 140;
    c.blue = 222;
  } else if (leftFreq[thisFreq] >= LB +  19*range and leftFreq[thisFreq] < LB + 20*range) { 
    //hot pink
    c.red = 255;
    c.green = 0;
    c.blue = 182;
  } else if (leftFreq[thisFreq] >= LB +  20*range and leftFreq[thisFreq] < LB + 21*range) { 
    //white
    c.red = 255;
    c.green = 255;
    c.blue = 255;
  }
  
  return c;
}

//sets it to the most active freq. If none of them are active, the song is most likely at a low point,
//so set the bestFreq equal to the lowest frequency (at the zero position)
int setFreq(){
  int bestFreq = 3;
  int sumFreq = 0;
  
  for (int i = 0; i < 7; i++){
    if (activity[bestFreq] < activity[i]){
      bestFreq = i;
    }
    sumFreq += activity[i];
  }
  
  if (sumFreq == 0)
    bestFreq = 0;
  
  return bestFreq;
}

void loop() {
  Color c;
  
  //every second, the program will reasses which frequency is most active
  count++;

  //gets the newest frequencies
  read_frequencies();
  
  //accounts for volume change
  fixFreq();
  
  //determines which freq to look at
  thisFreq = setFreq();
  
  c = setColor(c);
  
  shift();
  setFirst(c);

  delay(50);

}





