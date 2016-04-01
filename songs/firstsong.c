#include <stdio.h>
#include <math.h>
#include "portaudio.h"
#include <stdint.h>
#include <unistd.h> // for usleep()

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
int metro = 1000;

/* This routine will be called by the PortAudio engine when audio is needed.
** It may called at interrupt level on some machines so don't do anything
** that could mess up the system like calling malloc() or free().
*/
static int patestCallback( const void *inputBuffer, void *outputBuffer,
                           unsigned long framesPerBuffer,
                           const PaStreamCallbackTimeInfo* timeInfo,
                           PaStreamCallbackFlags statusFlags,
                           void *userData )
{
  paTestData *data = (paTestData*)userData;
  uint8_t *out = (uint8_t*)outputBuffer;
  unsigned long i;
  uint32_t freq = data->freq;

  (void) timeInfo; /* Prevent unused variable warnings. */
  (void) statusFlags;
  (void) inputBuffer;

  for( i=0; i<framesPerBuffer; i++ )
    {
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

void msleep(int d){
  usleep(d*1000);
}

void melody(){
  buzzer_set_freq(A4);
  msleep(metro/2);
}

void bass(){
  buzzer_set_freq(A0);
  msleep(metro/10);
  buzzer_set_freq(A0);
  msleep(metro/10);
  buzzer_set_freq(E0);
  msleep(metro/10);
  buzzer_set_freq(Gb0);
  msleep(metro/10);
  buzzer_set_freq(A0);
  msleep(metro/10);
  buzzer_set_freq(A0);
  msleep(metro/10);
  buzzer_set_freq(E0);
  msleep(metro/10);
  buzzer_set_freq(Gb0);
  msleep(metro/10);
  buzzer_set_freq(A0);
  msleep(metro/10);
  buzzer_set_freq(A0);
  msleep(metro/10);
  buzzer_set_freq(E0);
  msleep(metro/10);
  buzzer_set_freq(Gb0);
  msleep(metro/10);

  buzzer_set_freq(A0);
  msleep(metro/10);
  buzzer_set_freq(A0);
  msleep(metro/10);
  buzzer_set_freq(E0);
  msleep(metro/10);
  buzzer_set_freq(Gb0);
  msleep(metro/10);
  buzzer_set_freq(A0);
  msleep(metro/10);
  buzzer_set_freq(A0);
  msleep(metro/10);
  buzzer_set_freq(E0);
  msleep(metro/10);
  buzzer_set_freq(Gb0);
  msleep(metro/10);
  buzzer_set_freq(A0);
  msleep(metro/10);
  buzzer_set_freq(A0);
  msleep(metro/10);
  buzzer_set_freq(E0);
  msleep(metro/10);
  buzzer_set_freq(Gb0);
  msleep(metro/10);

  buzzer_set_freq(A0);
  msleep(metro/10);
  buzzer_set_freq(A0);
  msleep(metro/10);
  buzzer_set_freq(E0);
  msleep(metro/10);
  buzzer_set_freq(Gb0);
  msleep(metro/10);
  buzzer_set_freq(A0);
  msleep(metro/10);
  buzzer_set_freq(A0);
  msleep(metro/10);
  buzzer_set_freq(E0);
  msleep(metro/10);
  buzzer_set_freq(Gb0);
  msleep(metro/10);
  buzzer_set_freq(A0);
  msleep(metro/10);
  buzzer_set_freq(A0);
  msleep(metro/10);
  buzzer_set_freq(E0);
  msleep(metro/10);
  buzzer_set_freq(Gb0);
  msleep(metro/10);

  buzzer_set_freq(C1);
  msleep(metro/10);
  buzzer_set_freq(C1);
  msleep(metro/10);
  buzzer_set_freq(E0);
  msleep(metro/10);
  buzzer_set_freq(Gb0);
  msleep(metro/10);
  buzzer_set_freq(A0);
  msleep(metro/10);
  buzzer_set_freq(C1);
  msleep(metro/10);
  buzzer_set_freq(E0);
  msleep(metro/10);
  buzzer_set_freq(Gb0);
  msleep(metro/10);
  buzzer_set_freq(A0);
  msleep(metro/10);
  buzzer_set_freq(C1);
  msleep(metro/10);
  buzzer_set_freq(E0);
  msleep(metro/10);
  buzzer_set_freq(Gb0);
  msleep(metro/10);
}

int main(void) {

  pid_t pid = fork();
  if (pid == 0){
    printf("wot child\n");
    buzzer_start();
    bass();
    buzzer_stop();
  } else if (pid > 0 ) {
    printf("wot parent\n");
    // buzzer_start();
    // bass();
    // buzzer_stop();
  } else {
    printf("fork() failed!\n");
    return 1;
  }
  return 0;
}
