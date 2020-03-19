#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sndfile.h>
#include <math.h>

#include "vad.h"
#include "vad_docopt.h"
#include "pav_analysis.h"

#define DEBUG_VAD 0x1

int main(int argc, char *argv[]) {
  int verbose = 0; /* To show internal state of vad: verbose = DEBUG_VAD; */

  SNDFILE *sndfile_in, *sndfile_out = 0;
  SF_INFO sf_info;
  FILE *vadfile, *power;
  int n_read = 0, i;

  VAD_DATA *vad_data;
  VAD_STATE state, last_state;
  float *buffer, *buffer_zeros;
  int frame_size;         /* in samples */
  float frame_duration;   /* in seconds */
  unsigned int t, last_t, undef_t = -1, zeros_start_t; /* in frames */
  int undef_state = 1, zeros_subs = 0;

  int N_INITIAL_FRAMES=10;
  float frame_power=0.0F;
  float k0 = 0.0F;

  char	*input_wav, *output_vad, *output_wav;
  float min_silence_time=155.0;
  float min_voice_time=20.0F;
  float alfa1=8.15F;
  float alfa2=10.5F;

  DocoptArgs args = docopt(argc, argv, /* help */ 1, /* version */ "2.0");

  verbose    = args.verbose ? DEBUG_VAD : 0;
  input_wav  = args.input_wav;
  output_vad = args.output_vad;
  output_wav = args.output_wav;

  if(args.a1!=0) alfa1 = atof(args.a1);
  if(args.a2!=0) alfa2 = atof(args.a2);
  if(args.min_silence_t!=0) min_silence_time = atof(args.min_silence_t);
  if(args.min_voice_t!=0) min_voice_time = atof(args.min_voice_t);
  if(args.initial_frames!=0) N_INITIAL_FRAMES = (int)atof(args.initial_frames);
  
  
  
  


  if (input_wav == 0 || output_vad == 0) {
    fprintf(stderr, "%s\n", args.usage_pattern);
    return -1;
  }

  /* Open input sound file */
  if ((sndfile_in = sf_open(input_wav, SFM_READ, &sf_info)) == 0) {
    fprintf(stderr, "Error opening input file %s (%s)\n", input_wav, strerror(errno));
    return -1;
  }

  if (sf_info.channels != 1) {
    fprintf(stderr, "Error: the input file has to be mono: %s\n", input_wav);
    return -2;
  }

  /* Open vad file */
  if ((vadfile = fopen(output_vad, "wt")) == 0) {
    fprintf(stderr, "Error opening output vad file %s (%s)\n", output_vad, strerror(errno));
    return -1;
  }

  /* Open output sound file, with same format, channels, etc. than input */
  if (output_wav) {
    if ((sndfile_out = sf_open(output_wav, SFM_WRITE, &sf_info)) == 0) {
      fprintf(stderr, "Error opening output wav file %s (%s)\n", output_wav, strerror(errno));
      return -1;
    }
  }
  if ((power = fopen("power.txt", "wt")) == 0) {
    fprintf(stderr, "Error opening output vad file %s (%s)\n", "power.txt", strerror(errno));
    return -1;
  }


  vad_data = vad_open(sf_info.samplerate);
  /* Allocate memory for buffers */
  frame_size   = vad_frame_size(vad_data);
  buffer       = (float *) malloc(frame_size * sizeof(float));
  buffer_zeros = (float *) malloc(frame_size * sizeof(float));
  for (i=0; i< frame_size; ++i) buffer_zeros[i] = 0.0F;

  frame_duration = (float) frame_size/ (float) sf_info.samplerate;
  last_state = ST_SILENCE;

  for (t = last_t = 0; ; t++) { /* For each frame ... */
    /* End loop when file has finished (or there is an error) */
    if  ((n_read = sf_read_float(sndfile_in, buffer, frame_size)) != frame_size) break;

    if (sndfile_out != 0){
       //DONE?-TODO: copy all the samples into sndfile_out
      sf_write_float(sndfile_out, buffer, frame_size);
    }
    if(t<N_INITIAL_FRAMES) frame_power += pow(10, (compute_power(buffer, frame_size)/(float)10));
    if(t == N_INITIAL_FRAMES) k0=10*log10((1/(float)N_INITIAL_FRAMES)*frame_power);

    state = vad(vad_data, buffer, k0, alfa1, alfa2, min_silence_time, min_voice_time);
    
    //for(i=0; i<frame_size;i++) printf("%f\n", buffer[i]);
    vad_show_state(vad_data, power);
    if (verbose & DEBUG_VAD) vad_show_state(vad_data, stdout);
    
    /* TODO: print only SILENCE and VOICE labels */
    /* As it is, it prints UNDEF segments but is should be merge to the proper value */

    if(state == ST_VOICE || state == ST_SILENCE){
      if (state != last_state){        
        if (t != last_t && undef_t != last_t){
          if (undef_state==0){
            fprintf(vadfile, "%.5f\t%.5f\t%s\n",last_t * frame_duration, t * frame_duration, state2str(last_state));
            if(last_state == ST_SILENCE){
              zeros_start_t = last_t;
              zeros_subs = 1;
            } 
            last_t = t;
          }
          else{
            fprintf(vadfile, "%.5f\t%.5f\t%s\n",last_t * frame_duration, undef_t * frame_duration, state2str(last_state));
            if(last_state == ST_SILENCE){
              zeros_start_t = last_t;
              zeros_subs = 1;
            } 
            last_t = undef_t;
          }
          last_state = state;
        } 
      }
      undef_state = 0;
    }
    if((state == ST_USILENCE || state == ST_UVOICE) && undef_state == 0){
      undef_t = t;
      undef_state = 1;
    }

    if (sndfile_out != 0 && zeros_subs == 1) {
      /* TODO: go back and write zeros in silence segments */
      sf_seek(sndfile_out, zeros_start_t*frame_size, SEEK_SET);
      for(int j = 0; j < last_t - zeros_start_t; j++){
        sf_write_float(sndfile_out, buffer_zeros, frame_size);
      }
      zeros_subs = 0;
    }
  }

  state = vad_close(vad_data);
  /* TODO: what do you want to print, for last frames? */
  if (t != last_t)
    //fprintf(vadfile, "%.5f\t%.5f\t%s\n", last_t * frame_duration, t * frame_duration + n_read / (float) sf_info.samplerate, state2str(state));
    fprintf(vadfile, "%.5f\t%.5f\t%s\n", last_t * frame_duration, t * frame_duration + n_read / (float) sf_info.samplerate, state2str(state));
  /* clean up: free memory, close open files */
  free(buffer);
  free(buffer_zeros);
  sf_close(sndfile_in);
  fclose(vadfile);
  fclose(power);
  if (sndfile_out) sf_close(sndfile_out);
  return 0;
}
