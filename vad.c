#include <math.h>
#include <stdlib.h>
#include <stdio.h>

#include "vad.h"
#include "pav_analysis.h"

const float FRAME_TIME = 10.0F; /* in ms. */

int undefined_counter=0;
float k1=0.0F, k2=0.0F;
VAD_STATE previous_state=ST_INIT;

/* 
 * As the output state is only ST_VOICE, ST_SILENCE, or ST_UNDEF,
 * only this labels are needed. You need to add all labels, in case
 * you want to print the internal state in string format
 */

const char *state_str[] = {
  "UVOICE", "uSILENCE", "S", "V", "INIT"
};

const char *state2str(VAD_STATE st) {
  return state_str[st];
}

/* Define a datatype with interesting features */
typedef struct {
  float zcr;
  float p;
  float am;
  float fm;
} Features;

/* 
 * TODO: Delete and use your own features!
 */

Features compute_features(const float *x, int N, float fm) {
  /*
   * Input: x[i] : i=0 .... N-1 
   * Ouput: computed features
   */
  /* 
   * DELETE and include a call to your own functions
   *
   * For the moment, compute random value between 0 and 1 
   */
  Features feat;
  feat.fm=fm;
  feat.p=compute_power(x, N);
  feat.am=compute_am(x, N);
  feat.zcr=compute_zcr(x, N, feat.fm);

  /*Features feat;
  feat.zcr = feat.p = feat.am = (float) rand()/RAND_MAX;*/
  return feat;
}

/* 
 * TODO: Init the values of vad_data
 */


VAD_DATA * vad_open(float rate) {
  VAD_DATA *vad_data = malloc(sizeof(VAD_DATA));
  vad_data->state = ST_INIT;
  vad_data->sampling_rate = rate;
  vad_data->frame_length = rate * FRAME_TIME * 1e-3;
  return vad_data;
}

VAD_STATE vad_close(VAD_DATA *vad_data) {
  /* 
   * TODO: decide what to do with the last undecided frames
   */
   if(vad_data->state==ST_USILENCE) vad_data->state = ST_SILENCE;
   else if (vad_data->state==ST_UVOICE) vad_data->state = ST_VOICE;
  VAD_STATE state = vad_data->state;

  free(vad_data);
  return state;
}

unsigned int vad_frame_size(VAD_DATA *vad_data) {
  return vad_data->frame_length;
}

/* 
 * DONE? TODO: Implement the Voice Activity Detection 
 * using a Finite State Automata
 */

VAD_STATE vad(VAD_DATA *vad_data, float *x, float k0, float ALFA1, float ALFA2, float MIN_SILENCE_TIME, float MIN_VOICE_TIME) {

  /*
   * DONE? TODO: You can change this, using your own features,
   * program finite state automaton, define conditions, etc.
   */
  k1 = k0 + ALFA1;
  k2 = k1 + ALFA2;
  Features f = compute_features(x, vad_data->frame_length, vad_data->sampling_rate);
  
  vad_data->last_feature = f.p; /* save feature, in case you want to show */
  if(k0==0) vad_data->state = ST_SILENCE;
  else{
    switch (vad_data->state) {
    case ST_INIT:
      vad_data->state = ST_SILENCE;
      break;

    case ST_SILENCE:
      if(f.p > k2){
        vad_data->state = ST_VOICE;
        previous_state = ST_VOICE;
        undefined_counter = 0;
      }
      else if(f.p > k1){
        vad_data->state = ST_UVOICE;
        previous_state = ST_SILENCE;
        undefined_counter = 0;
      }
      break;

    case ST_VOICE:
      if (f.p < k1){
        vad_data->state = ST_USILENCE;
        previous_state = ST_VOICE;
        undefined_counter = 0;
      }
      break;

    case ST_USILENCE:
      if(f.p > k2){
        vad_data->state = ST_VOICE;
        previous_state = ST_VOICE;
        undefined_counter = 0;
        break;
      }
      undefined_counter++;
      if(FRAME_TIME * undefined_counter >= MIN_SILENCE_TIME && previous_state == ST_VOICE && f.p < k1){
        vad_data->state = ST_SILENCE;
      }
      else if(previous_state == ST_VOICE && f.p > k1){
        vad_data->state = ST_VOICE;
      }
      else{
        vad_data->state = ST_USILENCE;
      } 
      break;

    case ST_UVOICE:
      undefined_counter++;
      if(FRAME_TIME * undefined_counter >= MIN_VOICE_TIME && previous_state == ST_SILENCE && f.p > k1){
        vad_data->state = ST_VOICE;
      }
      else if(previous_state == ST_SILENCE && f.p < k1){
        vad_data->state = ST_SILENCE;
      }
      else{
        vad_data->state = ST_UVOICE;
      } 
      break;
    }

  }
  return vad_data->state;
  /*if (vad_data->state == ST_SILENCE ||
      vad_data->state == ST_VOICE)
    return vad_data->state;
  else
    return ST_UNDEF;*/
}

void vad_show_state(const VAD_DATA *vad_data, FILE *out) {
  fprintf(out, "%d\t%f\n", vad_data->state, vad_data->last_feature);
}
