#include <stdio.h>
#include <math.h>
#include "portaudio.h"
#include <stdint.h>
#include <unistd.h> // for usleep()
#include <time.h> // because music needs time

#define SAMPLE_RATE   (44100)
#define FRAMES_PER_BUFFER  (64)

typedef struct
{
  uint32_t total_count;
  uint32_t up_count;

  uint32_t counter;
  uint32_t prev_freq;
  uint32_t freq;
} paTestData;

//volatile int freq = 0;

// NOTES HERE
// notes frequency chart: http://www.phy.mtu.edu/~suits/notefreqs.html

int C0  = 16;
int Db0 = 17;
int D0  = 18;
int Eb0 = 19;
int E0  = 20;
int F0  = 22;
int Gb0 = 23;
int G0  = 24;
int Ab0 = 25;
int A0  = 27;
int Bb0 = 29;
int B0  = 31;
int C1  = 32;
int Db1 = 34;
int D1  = 36;
int Eb1 = 38;
int E1  = 41;
int F1  = 43;
int Gb1 = 46;
int G1  = 49;
int Ab1 = 51;
int A1  = 55;
int Bb1 = 58;
int B1  = 61;
int C2  = 65;
int Db2 = 69;
int D2  = 73;
int Eb2 = 77;
int E2  = 82;
int F2  = 87;
int Gb2 = 92;
int G2  = 98;
int Ab2 = 103;
int A2  = 110;
int Bb2 = 116;
int B2  = 123;
int C3  = 130;
int Db3 = 138;
int D3  = 146;
int Eb3 = 155;
int E3  = 164;
int F3  = 174;
int Gb3 = 185;
int G3  = 196;
int Ab3 = 207;
int A3  = 220;
int Bb3 = 233;
int B3  = 246;
int C4  = 261;
int Db4 = 277;
int D4  = 293;
int Eb4 = 311;
int E4  = 329;
int F4  = 349;
int Gb4 = 370;
int G4  = 392;
int Ab4 = 415;
int A4  = 440;
int Bb4 = 466;
int B4  = 494;
int C5  = 523;
int Db5 = 554;
int D5  = 587;
int Eb5 = 622;
int E5  = 659;
int F5  = 698;
int Gb5 = 740;
int G5  = 784;
int Ab5 = 830;
int A5  = 880;

// how quick? in ms
int ms_per_min = 60000;
int bpm = 170; // ?
int beat_length =  400; // = ms_per_min / bpm;

/* This routine will be called by the PortAudio engine when audio is needed.
** It may called at interrupt level on some machines so don't do anything
** that could mess up the system like calling malloc() or free().
*/
static int patestCallback( const void *inputBuffer, void *outputBuffer,
                           unsigned long framesPerBuffer,
                           const PaStreamCallbackTimeInfo* timeInfo,
                           PaStreamCallbackFlags statusFlags,
                           void *userData ) {
  paTestData *data = (paTestData*)userData;
  uint8_t *out = (uint8_t*)outputBuffer;
  unsigned long i;
  uint32_t freq = data->freq;

  (void) timeInfo; /* Prevent unused variable warnings. */
  (void) statusFlags;
  (void) inputBuffer;

  for( i=0; i<framesPerBuffer; i++ ) {
      if(data->up_count > 0 && data->total_count == data->up_count) {
	       *out++ = 0x00;
	        continue;
      }
      data->total_count++;
      if(freq != data->prev_freq) {
	       data->counter = 0;
      }

      if(freq) {
	      int overflow_max = SAMPLE_RATE / freq;
        uint32_t data_cnt = data->counter % overflow_max;
        if(data_cnt > overflow_max/2)
        *out++ = 0xff;
        else {
          *out++ = 0x00;
        }
        data->counter++;
      }
      else {
        data->counter = 0;
        *out++ = 0;
      }
      data->prev_freq = freq;
    }

  return paContinue;
}

static PaStream *stream;
static paTestData data;


void buzzer_set_freq(int frequency)
{
  data.up_count = 0; // do not stop!
  data.freq = frequency;
}

void buzzer_beep(int frequency, int msecs)
{
  data.total_count = 0;
  data.up_count = SAMPLE_RATE * msecs / 1000;
  data.freq = frequency;
}

int buzzer_start(void) {
  PaStreamParameters outputParameters;

  PaError err;
  int i;

  err = Pa_Initialize();
  if( err != paNoError ) goto error;

  outputParameters.device = Pa_GetDefaultOutputDevice(); /* default output device */
  outputParameters.channelCount = 1;       /* stereo output */
  outputParameters.sampleFormat = paUInt8; /* 32 bit floating point output */
  outputParameters.suggestedLatency = Pa_GetDeviceInfo( outputParameters.device )->defaultLowOutputLatency;
  outputParameters.hostApiSpecificStreamInfo = NULL;

  err = Pa_OpenStream(
		      &stream,
		      NULL, /* no input */
		      &outputParameters,
		      SAMPLE_RATE,
		      FRAMES_PER_BUFFER,
		      paClipOff,      /* we won't output out of range samples so don't bother clipping them */
		      patestCallback,
		      &data );
  if( err != paNoError ) goto error;

  err = Pa_StartStream( stream );
  if( err != paNoError ) goto error;

  return err;
 error:
  Pa_Terminate();
  fprintf( stderr, "An error occured while using the portaudio stream\n" );
  fprintf( stderr, "Error number: %d\n", err );
  fprintf( stderr, "Error message: %s\n", Pa_GetErrorText( err ) );
  return err;

}

int buzzer_stop()  {
  PaError err = 0;
  err = Pa_StopStream( stream );
  if( err != paNoError ) goto error;

  err = Pa_CloseStream( stream );
  if( err != paNoError ) goto error;

  Pa_Terminate();

  return err;
 error:
  Pa_Terminate();
  fprintf( stderr, "An error occured while using the portaudio stream\n" );
  fprintf( stderr, "Error number: %d\n", err );
  fprintf( stderr, "Error message: %s\n", Pa_GetErrorText( err ) );
  return err;
}


// =============== NOTE TYPES ===============

// note is the pitch and style is the space after the pitch is played
void quarternote (int note, int style){
  if (note > 0){
    buzzer_start();
    buzzer_set_freq(note);
    usleep(1000*(beat_length/8) - style);
    buzzer_stop();
    usleep(style);
  } else {
    usleep(1000*(beat_length/8));
  }
}

// =============== THEMES ===============

void theme1(){
  quarternote(G3, 1);
  quarternote(G3, 1);
  quarternote(G3, 1);
  quarternote(G3, 1);

  quarternote(G3, 1);
  quarternote(G3, 1);
  quarternote(G3, 1);
  quarternote(G3, 1);

  quarternote(A3, 1);
  quarternote(B3, 1);
  quarternote(A3, 1);
  quarternote(G3, 1);

  quarternote(E3, 1);
  quarternote(E3, 1);
  quarternote(D3, 1);
  quarternote(E3, 1);

  quarternote(C3, 1);
  quarternote(C3, 1);
  quarternote(C3, 1);
  quarternote(C3, 1);

  quarternote(C3, 1);
  quarternote(C3, 1);
  quarternote(C3, 1);
  quarternote(C3, 1);

  quarternote(C3, 1);
  quarternote(C3, 1);
  quarternote(C3, 1);
  quarternote(C3, 1);

  quarternote(C3, 1);

  quarternote(E3, 1);
  quarternote(E3, 1);
  quarternote(D3, 1);
  quarternote(D3, 1);

  quarternote(D3, 1);
  quarternote(D3, 1);
  quarternote(D3, 1);
  quarternote(D3, 1);

  quarternote(D3, 1);
  quarternote(D3, 1);

  quarternote(D3, 1);

  quarternote(A3, 1);
  quarternote(B3, 1);
  quarternote(A3, 1);
  quarternote(G3, 1);

  quarternote(E3, 1);
  quarternote(E3, 1);
  quarternote(D3, 1);
  quarternote(E3, 1);

  quarternote(G3, 1);
  quarternote(G3, 1);
  quarternote(G3, 1);
  quarternote(G3, 1);

  quarternote(G3, 1);
  quarternote(G3, 1);
  quarternote(G3, 1);
  quarternote(G3, 1);

  quarternote(G3, 1);
  quarternote(G3, 1);
  quarternote(G3, 1);
  quarternote(G3, 1);

  quarternote(G3, 1);
  quarternote(G3, 1);
  quarternote(G3, 1);
  quarternote(G3, 1);
}

void beat1(){
  quarternote(0, 1);
  quarternote(0, 1);
  quarternote(G2, 1);
  quarternote(0, 1);

  quarternote(0, 1);
  quarternote(0, 1);
  quarternote(G2, 1);
  quarternote(0, 1);

  quarternote(0, 1);
  quarternote(0, 1);
  quarternote(G2, 1);
  quarternote(G2, 1);

  quarternote(0, 1);
  quarternote(0, 1);
  quarternote(G2, 1);
  quarternote(0, 1);
}

int main(void) {

  beat1();
  beat1();
  beat1();
  theme1();
  theme1();
  
  return 0;
}
