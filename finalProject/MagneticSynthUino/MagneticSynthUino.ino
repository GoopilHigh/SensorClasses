#include <MozziGuts.h>
#include <Oscil.h> // oscillator template
#include <tables/sin2048_int8.h> // sine table for oscillator
#include <Sample.h> // Sample template
#include <samples/burroughs1_18649_int8.h> // a converted audio sample included in the Mozzi download
#include <BlynkSimpleStream.h> // Graphic Interface....................!!!!!!!!!!!!!

//#define BLYNK_PRINT SwSerial
//#include <SoftwareSerial.h>
//SoftwareSerial SwSerial(10, 11); // RX, TX

char auth[] = "46131fec75ea4f32b58d24446c222bff";
char KNOB_PIN = 0; // set the input for the knob to analog pin 0
char LDR_PIN = 1; // set the input for the LDR to analog pin 1
char PIEZO_PIN = 3;  // set the analog input pin for the piezo 
short int threshold = 200; // threshold value to decide when the detected signal is a knock or not

// use: Sample <table_size, update_rate> SampleName (wavetable)
Sample <BURROUGHS1_18649_NUM_CELLS, AUDIO_RATE> aSample(BURROUGHS1_18649_DATA);
float recorded_pitch = (float) BURROUGHS1_18649_SAMPLERATE / (float) BURROUGHS1_18649_NUM_CELLS;

// use: Oscil <table_size, update_rate> oscilName (wavetable), look in .h file of table #included above
Oscil <SIN2048_NUM_CELLS, AUDIO_RATE> aSin(SIN2048_DATA);

byte volume;
boolean triggered = false;

void setup(){
  // Debug console
  //SwSerial.begin(115200);

  
  
  Serial.begin(9600);
  Blynk.begin(Serial, auth);
  startMozzi();
}


void updateControl(){
  // read the potentiometer
  short int knob_value = mozziAnalogRead(KNOB_PIN); // value is 0-1023
  
  // map it to an 8 bit volume range for efficient calculations in updateAudio
  volume = knob_value >> 2;  // 10 bits (0->1023) shifted right by 2 bits to give 8 bits (0->255)
  //Serial.print("volume -> ");
  //Serial.println(volume);
  //Serial.print(" ");
  
  // map it to values between 0.1 and about double the recorded pitch
  float pitch = (recorded_pitch * (float) knob_value / 512.f) + 0.1f; 
  
  // set the sample playback frequency
  aSample.setFreq(pitch);
    
  // read the piezo
  short int piezo_value = mozziAnalogRead(PIEZO_PIN); // value is 0-1023
/*Serial.print("piezo_value -> ");
  Serial.print(piezo_value);
  Serial.print(" ");*/

  // only trigger once each time the piezo goes over the threshold
    if (piezo_value>threshold) {
      if (!triggered){
        aSample.start();
        triggered = true;
      }
    }else{
      triggered = false;
  }

  // read the magnetic value
  short int magnetic_level = mozziAnalogRead(LDR_PIN); // value is 0-1023
/*Serial.print("magnetic_level -> ");
  Serial.print(magnetic_level);
  Serial.print(" ");*/

  // set the frequency
  aSin.setFreq(magnetic_level);
  
  Serial.println();
}

int updateAudio(){
  if (triggered == true)
    return aSample.next();
  else
    return ((int)aSin.next() * (volume + 250)) >> 8; // shift back into range after multiplying by 8 bit value
}

void loop(){
  Blynk.run();
  audioHook();
}