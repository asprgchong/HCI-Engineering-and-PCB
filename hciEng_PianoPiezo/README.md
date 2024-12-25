Final Project: HCI Engineering

Piano gloves: an interactive wearable with left and right hand modulation using 10 piezos (7 for notes, 3 for key/octave changes). User input of pressing down fingers with variable force will trigger MIDI messages sent to Garageband via USB from left-hand microcontroller.

Dependencies (all included in file):
    SPI.h
    Wire.h
    Adafruit_GFX.h
    Adafruit_SSD1306.h
    MIDI.h
    USB-MIDI.h: midi library that allows midi messages to be sent via USB transport, source: lathoub https://github.com/lathoub/Arduino-USBMIDI 
    Garageband with MIDI USB Software

How to interact with wearable:
SENSORS:
    - 10 piezzos 
    - 1 red button for toggle device "on/off"
    - 1 button for toggle "Regular Mode/Learn Mode"
    - 2 HC05 Bluetooths paired to each other
ACTUATORS:
    - OLED Screen to display note played
    - Garageband playing piano sounds
ORDER OF FINGER KEYS ON LEFT HAND:
    pinky (leftmost): C key
    ring finger: D key
    middle finger: E key
    index finger: F key
    thumb (rightmost): half-step key
ORDER OF FINGER KEYS ON RIGHT HAND:
    thumb (leftmost): down octave key
    index finger: G key
    middle finger: A key
    ring finger: B key
    pinky (rightmost): up octave key
- To toggle whether device is listening to finger sensors, press red button on left hand board
- To toggle between main mode and learn mode, press other button on left hand board
- 10 total fingers inputs, three outliers as follows:
    - half-step key: TAP to toggle increment of half-step for all notes , e.g. C --> C#
    - up octave key: increments all notes to one octave higher (up to midi note number 119 or B8), e.g. C4 --> C5
    - down octave key: decrements all notes to one octave lower (down to midi note number 24 or C1), e.g. C4 --> C3

Main Mode: Main mode is a piano simulation with all keys and toggles outlined above functioning. Can play multiple notes at the same time by pressing multiple note fingers at the same time. Midi notes are sent as (pitch, velocity, channel) where pitch is the corresponding midi note number to the finger key, velocity is the force of piezzo, and channel is channel 1 for piano. Right hand (server/sender) sensing is sent to left hand (client/receiver) through bluetooth serial messages
    MESSAGE FORMAT:
        [255]['n'][piezzo number 5-8][velocity][1/0 - on/off][1/2 parity vs checksum][0/1 error val]: midi note message to ultimately send to garageband, either note on or off, with error detection implementation
        [255]['o'][1/0 - up/down an octave]: octave change message with either 1 or 0 for up or down octave

Learn Mode: Guided tutorial on how to play twinkle twinkle little star, with OLED screen display indicating what note to play next. Only plays note sound when you hit the correct key.
 





