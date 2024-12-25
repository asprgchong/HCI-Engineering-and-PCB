
#include <MIDI.h>
#include <USB-MIDI.h>
/* MIDI LIBRARY THAT ALLOWS MIDI NOTES TO BE SENT OVER USB by lathoub https://github.com/lathoub/Arduino-USBMIDI 

MIDI FUNCTIONALITY:
MIDI.sendNoteOn(note-pitch, velocity, channel): play note message
- note pitch 0 to 127 https://inspiredacoustics.com/en/MIDI_note_numbers_and_center_frequencies
- velocity 0 to 127
MIDI.sendNoteOff(note-pitch, 0, channel): stop note message
*/
USBMIDI_NAMESPACE::usbMidiTransport usbMIDI2(1);
MIDI_NAMESPACE::MidiInterface<USBMIDI_NAMESPACE::usbMidiTransport> MIDI((USBMIDI_NAMESPACE::usbMidiTransport&)usbMIDI2);
#define TRUE 1
#define FALSE 0


/* ORDER OF PIEZO ON LEFTHAND (pinky leftmost - thumb rightmost)
4 3 2 1 5
C D E F half
*/

int button = 8;
int piezo_C = 0;
int piezo_D = 1;
int piezo_E = 2;
int piezo_F = 3;
int piezo_half = 4;                     
int piano_chnl = 1;

int onC = FALSE;
int onD = FALSE;
int onE = FALSE;
int onF = FALSE;
int on_half = FALSE;
int prev_time = 0;
const int midi_octave_change = 12; //num midi notes to get from C4 to C5 for example
const int half_step = 1;
int thresh = 70;
int gloves_on = FALSE;
//base pitches:
int C = 60; //middle C
int D = 62; //D4
int E = 64; //E4
int F = 65; //F4
int G = 67; //G4
int A = 69; //A4
int B = 71; //B4
void setup() {
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(button, INPUT_PULLUP);
  // pinMode(learnMode_button, INPUT_PULLUP);
  MIDI.begin(MIDI_CHANNEL_OMNI); //listens to incoming midi on piano channel
  Serial1.begin(115200);
}

void loop() {
  while (!gloves_on) {
    if (digitalRead(button) == LOW) {
      while (digitalRead(button) != HIGH) {
      }
      while (Serial1.available()) {
        Serial1.read();
      }
      gloves_on = TRUE;
      Serial.println("LISTENING");
    }
    delay(300);
  }
  if (digitalRead(button) == LOW) {
    releaseAll();
    while (Serial1.available()) {
      Serial1.read();
    }
    Serial.println("NOT LISTENING");
    gloves_on = FALSE;
    delay(300);
  }

  MIDI.read();
  int rawC = analogRead(piezo_C);
  int rawD = analogRead(piezo_D);
  int rawE = analogRead(piezo_E);
  int rawF = analogRead(piezo_F);
  int half = analogRead(piezo_half);
  int forceC = map(rawC, 0, 1023, 0, 127);
  int forceD = map(rawD, 0, 1023, 0, 127);
  int forceE = map(rawE, 0, 1023, 0, 127);
  int forceF = map(rawF, 0, 1023, 0, 127);
  // Serial.println(half);
  if (half > 200 && millis() - prev_time > 800) { //INSTRUCTION FOR USER: ONLY TAP FOR HALF STEP

    releaseAll();
    // Serial.print("HALF STEP: ");Serial.println(on_half);
    if (on_half) {
      on_half = 0;

    } else { 
      on_half = TRUE;
    }
    prev_time = millis();
    Serial.print("HALF STEP UPDATE: ");Serial.println(on_half);
  }
  /*BLUETOOTH CLIENT/RECEIVER - to read from serial exclusively
    2 possible message forms to receive from RIGHT HAND:
    [255][n][5-8][velocity][1/0 - on/off][1/2 parity vs checksum][0/1 error val] - midi note from piezos
    [255][o][1/0 - up/down an octave]
  */
  if (Serial1.available()) {
    // Serial.println("TEST");
    while (Serial1.read() == 255) {
      byte message = Serial1.read();
      int ed_type, checkVal;
      byte piezo_num, velocity, on_off, shift;
      switch (char(message)) {
        case 'o':
          shift = Serial1.read();
          if (shift == 1) {
            if (C + 12 <= 108) {
              C += midi_octave_change;
              D += midi_octave_change;
              E += midi_octave_change;
              F += midi_octave_change;
              G += midi_octave_change;
              A += midi_octave_change;
              B += midi_octave_change;
              Serial.print("up an octave: "); Serial.println(C);
            } else {
              Serial.println("ON HIGHEST OCTAVE");
            }
          }
          if (shift == 0) {
            if (C - 12 >= 24) {
              C -= midi_octave_change;
              D -= midi_octave_change;
              E -= midi_octave_change;
              F -= midi_octave_change;
              G -= midi_octave_change;
              A -= midi_octave_change;
              B -= midi_octave_change;
              Serial.print("lowered: "); Serial.println(C);
            } else {
              Serial.println("ON LOWEST OCTAVE");
            }
          }
          break;
        case 'n':

        
          piezo_num = Serial1.read();
           Serial.println("HERE: "); Serial.println(piezo_num);
          velocity = Serial1.read();
          on_off = Serial1.read();
          ed_type = Serial1.read();
          checkVal = Serial1.read();
          Serial.print("HERE piezzo: "); Serial.println(piezo_num);
          if ((ed_type == 1 && checkParity(255, message, piezo_num, velocity, on_off, ed_type, checkVal)) || //PARITY ERROR DETECTION
          (ed_type == 2 && checkSum(255, message, piezo_num, velocity, on_off, ed_type, checkVal))) { //CHECKSUM ERROR DETECTION
            Serial.print("HERE piezzo: "); Serial.println(piezo_num);
            if (piezo_num == 5) {
              if (on_off == 1) MIDI.sendNoteOn(G + on_half*half_step, velocity, piano_chnl);
              if (on_off == 0) MIDI.sendNoteOff(G + on_half*half_step, 0, piano_chnl);
            } else if (piezo_num == 6) {
              if (on_off == 1) MIDI.sendNoteOn(A + on_half*half_step, velocity, piano_chnl);
              if (on_off == 0) MIDI.sendNoteOff(A + on_half*half_step, 0, piano_chnl);
            } else if (piezo_num == 7) {
              if (on_off == 1) MIDI.sendNoteOn(B + on_half*half_step, velocity, piano_chnl);
              if (on_off == 0) MIDI.sendNoteOff(B + on_half*half_step, 0, piano_chnl);
            }
          }
          break;
        default:
          break;
      }
    }
  }
  if (is_pressed(rawC, C + on_half*half_step, forceC, onC)) onC = TRUE;
  if (is_pressed(rawD, D + on_half*half_step, forceD, onD)) onD = TRUE;
  if (is_pressed(rawE, E + on_half*half_step, forceE, onE)) onE = TRUE;
  if (is_pressed(rawF, F + on_half*half_step, forceF, onF)) onF = TRUE;

  if (is_released(rawC, C + on_half*half_step, onC)) onC = FALSE;
  if (is_released(rawD, D + on_half*half_step, onD)) onD = FALSE;
  if (is_released(rawE, E + on_half*half_step, onE)) onE = FALSE;
  if (is_released(rawF, F + on_half*half_step, onF)) onF = FALSE;
  delay(10);
}


// void check_found(int raw_val, int pitch, int velocity) {
//  if (raw_val > thresh) {
//     found = TRUE;
//     MIDI.sendNoteOn(pitch, velocity, piano_chnl);
//   }
// }
int is_pressed(int raw_val, int pitch, int velocity, int on) {
  if (raw_val > thresh && !on)  {
    MIDI.sendNoteOn(pitch, velocity, piano_chnl);
    return TRUE;
  }

  return FALSE;
}

int is_released(int raw_val, int pitch, int on) {
  if (raw_val < thresh && on)  {
    MIDI.sendNoteOff(pitch, 0, piano_chnl);
    return TRUE;
  }
  return FALSE;
}

void resetOctave() {
  C = 60; //middle C
  D = 62; //D4
  E = 64; //E4
  F = 65; //F4
  G = 67; //G4
  A = 69; //A4
  B = 71; //B4
}
void releaseAll(){
  MIDI.sendNoteOff(C + on_half*half_step, 0, piano_chnl);
  MIDI.sendNoteOff(D + on_half*half_step, 0, piano_chnl);
  MIDI.sendNoteOff(E + on_half*half_step, 0, piano_chnl);
  MIDI.sendNoteOff(F + on_half*half_step, 0, piano_chnl);
  MIDI.sendNoteOff(G + on_half*half_step, 0, piano_chnl);
  MIDI.sendNoteOff(A + on_half*half_step, 0, piano_chnl);
  MIDI.sendNoteOff(B + on_half*half_step, 0, piano_chnl);
}


//Error Detection Functions
//==========================

//numOnes: counts number of 1 bits in a byte
int numOnes(uint8_t b) {
  int ones_count = 0;
  for (int i = 0; i < 8; i++) {
    if ((b >> i) % 2 == 1) {
      ones_count++;
    }
  }
  return ones_count;
}

int checkParity(byte b1, byte b2, byte b3, byte b4, byte b5, byte b6, byte b7) {
  int totalOnes = numOnes(b1) + numOnes(b2) + numOnes(b3) + numOnes(b4) + numOnes(b5) + 1;
  if (totalOnes % 2 == b7) return 1;
  return 0; 
}

int checkSum(byte b1, byte b2, byte b3, byte b4, byte b5, byte b6, byte b7) {
  if (uint8_t(b1 + b2 + b3 + b4 + b5 + b6 + b7) == 0) return 1;
  return 0;
}


