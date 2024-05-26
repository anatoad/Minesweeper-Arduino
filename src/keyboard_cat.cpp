#include "Arduino.h"
#include "buzzer.h"

/* 
  Keyboard cat
  Connect a piezo buzzer2 or speaker to pin 11 or select a new pin.
  More songs available at https://github.com/robsoncouto/arduino-songs                                            
                                              
                                              Robson Couto, 2019
*/

// change this to make the song slower or faster
int tempo2 = 160;

// change this to whichever pin you want to use
int buzzer2 = 6;

// notes2 of the moledy followed by the duration.
// a 4 means a quarter note, 8 an eighteenth , 16 sixteenth, so on
// !!negative numbers are used to represent dotted notes2,
// so -4 means a dotted quarter note, that is, a quarter plus an eighteenth!!
int melody2[] = {

  // Keyboard cat
  // Score available at https://musescore.com/user/142788/scores/147371

    REST,1,
    REST,1,
    NOTE_C4,4, NOTE_E4,4, NOTE_G4,4, NOTE_E4,4, 
    NOTE_C4,4, NOTE_E4,8, NOTE_G4,-4, NOTE_E4,4,
    NOTE_A3,4, NOTE_C4,4, NOTE_E4,4, NOTE_C4,4,
    NOTE_A3,4, NOTE_C4,8, NOTE_E4,-4, NOTE_C4,4,
    NOTE_G3,4, NOTE_B3,4, NOTE_D4,4, NOTE_B3,4,
    NOTE_G3,4, NOTE_B3,8, NOTE_D4,-4, NOTE_B3,4,

    NOTE_G3,4, NOTE_G3,8, NOTE_G3,-4, NOTE_G3,8, NOTE_G3,4, 
    NOTE_G3,4, NOTE_G3,4, NOTE_G3,8, NOTE_G3,4,
    NOTE_C4,4, NOTE_E4,4, NOTE_G4,4, NOTE_E4,4, 
    NOTE_C4,4, NOTE_E4,8, NOTE_G4,-4, NOTE_E4,4,
    NOTE_A3,4, NOTE_C4,4, NOTE_E4,4, NOTE_C4,4,
    NOTE_A3,4, NOTE_C4,8, NOTE_E4,-4, NOTE_C4,4,
    NOTE_G3,4, NOTE_B3,4, NOTE_D4,4, NOTE_B3,4,
    NOTE_G3,4, NOTE_B3,8, NOTE_D4,-4, NOTE_B3,4,

    NOTE_G3,-1, 
  
};

// sizeof gives the number of bytes, each int value is composed of two bytes (16 bits)
// there are two values per note (pitch and duration), so for each note there are four bytes
int notes2 = sizeof(melody2) / sizeof(melody2[0]) / 2;

// this calculates the duration of a whole note in ms
int wholenote2 = (60000 * 4) / tempo2;

int divider2 = 0, noteDuration2 = 0;

int thisNote2 = 0;

void play_song_game_won() {
    // repeat the melody.
    if (thisNote2 >= notes2 * 2) {
        thisNote2 = 0;
    }

    // calculates the duration of each note
    divider2 = melody2[thisNote2 + 1];
    if (divider2 > 0) {
        // regular note, just proceed
        noteDuration2 = (wholenote2) / divider2;
    } else if (divider2 < 0) {
        // dotted notes2 are represented with negative durations!!
        noteDuration2 = (wholenote2) / abs(divider2);
        noteDuration2 *= 1.5; // increases the duration in half for dotted notes2
    }

    // we only play the note for 90% of the duration, leaving 10% as a pause
    tone(buzzer2, melody2[thisNote2], noteDuration2 * 0.9);

    // Wait for the specief duration before playing the next note.
    delay(noteDuration2);

    // stop the waveform generation before the next note.
    noTone(buzzer2);

    thisNote2 += 2;
}
