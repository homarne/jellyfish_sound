// Simple WAV file player example
//
// Three types of output may be used, by configuring the code below.
//
//   1: Digital I2S - Normally used with the audio shield:
//         http://www.pjrc.com/store/teensy3_audio.html
//
//   2: Digital S/PDIF - Connect pin 22 to a S/PDIF transmitter
//         https://www.oshpark.com/shared_projects/KcDBKHta
//
//   3: Analog DAC - Connect the DAC pin to an amplified speaker
//         http://www.pjrc.com/teensy/gui/?info=AudioOutputAnalog
//
// To configure the output type, first uncomment one of the three
// output objects.  If not using the audio shield, comment out
// the sgtl5000_1 lines in setup(), so it does not wait forever
// trying to configure the SGTL5000 codec chip.
//
// The SD card may connect to different pins, depending on the
// hardware you are using.  Uncomment or configure the SD card
// pins to match your hardware.
//
// Data files to put on your SD card can be downloaded here:
//   http://www.pjrc.com/teensy/td_libs_AudioDataFiles.html
//
// This example code is in the public domain.

#include "audio_files.h"

#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

#define RMS_REPORT

#ifndef RMS_REPORT
AudioPlaySdWav           playSdWav1;
// Use one of these 3 output types: Digital I2S, Digital S/PDIF, or Analog DAC
AudioOutputI2S           audioOutput;
//AudioOutputSPDIF       audioOutput;
//AudioOutputAnalog      audioOutput;
AudioConnection          patchCord1(playSdWav1, 0, audioOutput, 0);
AudioConnection          patchCord2(playSdWav1, 1, audioOutput, 1);
AudioControlSGTL5000     sgtl5000_1;
#endif

#ifdef RMS_REPORT
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

// GUItool: begin automatically generated code
AudioPlaySdWav           playSdWav1;     //xy=207,225
AudioAnalyzeRMS          rms1;           //xy=364,268
AudioAnalyzeRMS          rms2; //xy=366,176
AudioOutputI2S           i2s1;           //xy=504,224
AudioConnection          patchCord1(playSdWav1, 0, rms2, 0);
AudioConnection          patchCord2(playSdWav1, 0, i2s1, 0);
AudioConnection          patchCord3(playSdWav1, 1, rms1, 0);
AudioConnection          patchCord4(playSdWav1, 1, i2s1, 1);
AudioControlSGTL5000     sgtl5000_1;     //xy=202,189
// GUItool: end automatically generated code
#endif

// Use these with the Teensy Audio Shield
#define SDCARD_CS_PIN    10
#define SDCARD_MOSI_PIN  7
#define SDCARD_SCK_PIN   14

#define BACKGROUND 11
#define SPEAKING 23
#define LISTENING 37

#define SIG_LEVEL_MIN 64
#define SIG_LEVEL_MAX 96
#define SIG_LEVEL_RANGE SIG_LEVEL_MAX - SIG_LEVEL_MIN

//#define MODE_MOON
#define MODE_JELLY

int theme_step = 0;
int ambient_step = 0;
int radio_step = 0;

const int TIMES_TO_BACKGROUND = 3;
int background_count = TIMES_TO_BACKGROUND;

const int TIMES_TO_TALK = 4;
int talk_count = TIMES_TO_TALK;

int talk_step = 0;

int state = LISTENING;

unsigned long current_time;
unsigned long listen_timeout;

const unsigned long LISTEN_WAIT_TIME = 10000L; //milliseconds

void setup() {
  Serial.begin(9600);
  Serial1.begin(9600);

  // Audio connections require memory to work.  For more
  // detailed information, see the MemoryAndCpuUsage example
  AudioMemory(12);

  // Comment these out if not using the audio adaptor board.
  // This may wait forever if the SDA & SCL pins lack
  // pullup resistors
  sgtl5000_1.enable();
  sgtl5000_1.volume(0.7);

  SPI.setMOSI(SDCARD_MOSI_PIN);
  SPI.setSCK(SDCARD_SCK_PIN);
  if (!(SD.begin(SDCARD_CS_PIN))) {
    // stop here, but print a message repetitively
    while (1) {
      Serial.println("Unable to access the SD card");
      delay(500);
    }
  }

  listen_timeout = millis() + LISTEN_WAIT_TIME;

}

void playFile(const char *filename)
{
  Serial.print("Playing file: ");
  Serial.println(filename);

  // Start playing the file.  This sketch continues to
  // run while the file plays.
  playSdWav1.play(filename);

  // A brief delay for the library read WAV info
  delay(5);
  
  // Simply wait for the file to finish playing.
  while (playSdWav1.isPlaying()) {
    // uncomment these lines if you audio shield
    // has the optional volume pot soldered
    //float vol = analogRead(15);
    //vol = vol / 1024;
    // sgtl5000_1.volume(vol);
    
//    if (rms1.available()) {
//      int signal_level = 0;
//      signal_level = rms1.read() * 100.0;
//      //Serial.print("signal level ");
//      //Serial.println(signal_level);
//      if (signal_level > SIG_LEVEL_RANGE){signal_level = SIG_LEVEL_RANGE;}
//      Serial1.write(signal_level + SIG_LEVEL_MIN);
//    }
//    delay(100);
    
  }
}

void playTalkFile(const char *filename)
{
  Serial.print("Playing file: ");
  Serial.println(filename);

  // Start playing the file.  This sketch continues to
  // run while the file plays.
  playSdWav1.play(filename);

  // A brief delay for the library read WAV info
  delay(5);
  
  // Simply wait for the file to finish playing.
  while (playSdWav1.isPlaying()) {
    // uncomment these lines if you audio shield
    // has the optional volume pot soldered
    //float vol = analogRead(15);
    //vol = vol / 1024;
    // sgtl5000_1.volume(vol);
    if (rms1.available()) {
      int signal_level = 0;
      signal_level = rms1.read() * 100.0;
      //Serial.print("signal level ");
      //Serial.println(signal_level);
      if (signal_level > SIG_LEVEL_RANGE){signal_level = SIG_LEVEL_RANGE;}
      Serial1.write(signal_level + SIG_LEVEL_MIN);
    }
    delay(100);
    
  }
}

int playFileInterruptable(const char *filename)
{
  Serial.print("Playing file: ");
  Serial.println(filename);

  // Start playing the file.  This sketch continues to
  // run while the file plays.
  playSdWav1.play(filename);

  // A brief delay for the library read WAV info
  delay(5);

  int incoming_byte;
  int return_code = 0;
  
  // Wait for the file to finish playing 
  // while checking every 100 mS for SPEAKING message.
  while (playSdWav1.isPlaying()) {
    // uncomment these lines if you audio shield
    // has the optional volume pot soldered
    //float vol = analogRead(15);
    //vol = vol / 1024;
    // sgtl5000_1.volume(vol);

    incoming_byte = Serial1.read();
    
    if (incoming_byte == SPEAKING){
      Serial.println("speaking found in play");
      playSdWav1.stop();
      return_code = 1;
      break;
    }

//    if (rms1.available()) {
//      int signal_level = 0;
//      signal_level = rms1.read() * 100.0;
//      //Serial.print("signal level ");
//      //Serial.println(signal_level);
//      if (signal_level > SIG_LEVEL_RANGE){signal_level = SIG_LEVEL_RANGE;}
//      Serial1.write(signal_level + SIG_LEVEL_MIN);
//    }
    
    delay(100);
  }
  
  return return_code;
}


void loop() {

  int incoming_byte;
  incoming_byte = Serial1.read();
  //Serial.println(incoming_byte, DEC);
  //Serial.println(state, DEC);

  //    current_time = millis();
  //    Serial.println(current_time, DEC);

#ifdef MODE_MOON

  switch (state) {
    case BACKGROUND:
      sgtl5000_1.volume(0.75);
      int play_status;
      
      play_status = playFileInterruptable(moon_ambient[ambient_step]);
      //playFileInterruptable(moon_ambient[ambient_step]);
      ambient_step++;
      if (ambient_step >= 12) {
        ambient_step = 0;
      }

//      play_status = playFileInterruptable(theme[theme_step]);
//      theme_step++;
//      if (theme_step >= 13) {
//        theme_step = 0;
//      }
      
//      play_status = playFileInterruptable(radio[radio_step]);
//      radio_step++;
//      if (radio_step >= 8) {
//        radio_step = 0;
//      }

//      play_status = playFileInterruptable(jelly_talk[talk_step]);
//      talk_step++;
//      if (talk_step >= 20) {
//        talk_step = 0;
//      }
      
      
      if (play_status == 1) {;
        listen_timeout = millis() + LISTEN_WAIT_TIME;
        state = LISTENING;
      }
      Serial.print("background ");
      Serial.println(background_count);
      break;

    case SPEAKING:
      sgtl5000_1.volume(0.8);
      playTalkFile(moon_talk[talk_step]);
      talk_step++;
      if (talk_step >= 19) {
        talk_step = 0;
      }
      listen_timeout = millis() + LISTEN_WAIT_TIME;
      state = LISTENING;
      Serial1.write(LISTENING);
      break;

    case LISTENING:
      if (incoming_byte == LISTENING) {
        state = SPEAKING;
        Serial1.write(SPEAKING);
        break;
      }
      if (millis() > listen_timeout) {
        state = BACKGROUND;
        break;
      }
  }

#endif // moon

#ifdef MODE_JELLY

  switch (state) {

    case BACKGROUND:
      sgtl5000_1.volume(0.75);
      
      playFile(jelly_ambient[ambient_step]);
      ambient_step++;
      if (ambient_step >= 12) {
        ambient_step = 0;
      }

//      playFile(radio[radio_step]);
//      radio_step++;
//      if (radio_step >= 8) {
//        radio_step = 0;
//      }
      
      background_count--;
      if (background_count <= 0) {
        background_count = TIMES_TO_BACKGROUND;
        state = SPEAKING;
      }
      Serial.print("background ");
      Serial.println(background_count);
      break;


  case SPEAKING:
      Serial1.write(SPEAKING);
      playTalkFile(jelly_talk[talk_step]);
        talk_step++;
        if (talk_step >= 20) {
        talk_step = 0;
      }
    talk_count--;
    // in this case, listen first, then handle talk_count;
    listen_timeout = millis() + LISTEN_WAIT_TIME;
    Serial1.write(LISTENING);
    state = LISTENING;
    Serial.print("speaking ");
    Serial.println(talk_count);
    break;

  case LISTENING:
      if (incoming_byte == LISTENING) {
        if (talk_count <= 0) {
            talk_count = TIMES_TO_TALK;
            state = BACKGROUND;
          } else {
            state = SPEAKING;
          }
          break;
        }
      if (millis() > listen_timeout) {
        state = BACKGROUND;
        //state = SPEAKING;
        break;
      }
  }

#endif // jelly

}
