/*
 * sound.h
 *
 *  Created on: Jul 6, 2017
 *      Author: kevywilly
 */

#ifndef SOUND_H_
#define SOUND_H_

class NoiseMaker {
  public:
    NoiseMaker(uint8_t pin) {
      piezzo_pin = pin;
    }

    void buzzfor(int count, int dur, int volume) {
      for(int i = 0; i < count; i++) {
        buzz(volume);
        delay(dur);
        silent();
      }
    }

    void buzz(int vol) {
      analogWrite(piezzo_pin,map(vol, 0, 100, 0, 255));
    }
    void silent() {
      analogWrite(piezzo_pin,0);
    }
    void sing(int tempo, char notes[], int beats[], int num) {
      /*
        for(int i=0; i < count; i++) {

          int dur = beats[i]*tempo;
          if(notes[i] == ' ') {
            delay(dur);
          } else {
             tone(piezzo_pin, frequency(notes[i]), dur);
          }


        }
        */
    }

  private:
    uint8_t piezzo_pin;
    char noteNames[8] = { 'c', 'd', 'e', 'f', 'g', 'a', 'b', 'C' };
    int frequencies[8] = {262, 294, 330, 349, 392, 440, 494, 523};

    int numNotes = 8;


    int frequency(char note) {

      int i;
      const int numNotes = 8;  // number of notes we're storing

      // Now we'll search through the letters in the array, and if
      // we find it, we'll return the frequency for that note.

      for (i = 0; i < numNotes; i++)  // Step through the notes
      {
        if (noteNames[i] == note)         // Is this the one?
        {
          return(frequencies[i]);     // Yes! Return the frequency
        }
      }
      return(0);  // We looked through everything and didn't find it,
                  // but we still need to return a value, so return 0.
    }

};


#endif /* SOUND_H_ */
