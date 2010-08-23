#include "aubioAnalyzer.h"


//--------------------------------------------------------------
void aubioAnalyzer::setup(){	 
			
	//the two variables that hold pitch and volume
	amplitude		= 0;
	pitch			= 0;
			
	//aubio stuff
	win_s      = 1024;                       /* window size */
	hop_s      = win_s/4;                    /* hop size */
	samplerate = 44100;                      /* samplerate */
	channels   = 1;                          /* number of channel */
	
	mode = aubio_pitchm_freq;				//change to aubio_pitchm_midi to get frequency as midi notes
	type = aubio_pitch_yinfft;
	
	//setup aubio buffer and pitch detection
	in  = new_fvec (hop_s, channels);
	pitch_output = new_aubio_pitchdetection(win_s, hop_s, channels, samplerate, type, mode);
	
	// beat tracking
	out = new_fvec(hop_s, channels);
	aubio_beats = new_aubio_beattracking(win_s,  channels);
	
}

//--------------------------------------------------------------
aubioAnalyzer::~aubioAnalyzer(){
	  del_aubio_beattracking(aubio_beats);
	  del_aubio_pitchdetection(pitch_output);
	  del_fvec(in);
	  del_fvec(out);
	  aubio_cleanup();
}

//--------------------------------------------------------------
void aubioAnalyzer::processAudio (float * input, int bufferSize){	
	
	float rmsAmplitude  = 0;

	for (int i = 0; i < bufferSize; i++){
		
		//calculate the root mean square amplitude
		rmsAmplitude += sqrt(input[i]*input[i]);
		
		//put the data into aubio
		in->data[0][i] = input[i];
	}
	
	//now we need to get the average
	rmsAmplitude /= bufferSize;
	amplitude = rmsAmplitude;
	
	//don't update the pitch if the sound is very quiet
	if( amplitude > 0.01 ){
		//finally get the pitch of the sound
		pitch = aubio_pitchdetection(pitch_output,in);	
	}
	
	// attempt beat tracking
	aubio_beattracking_do(aubio_beats, in, out);
}

void aubioAnalyzer::drawBeats(int x, int y)
{
	/*ba_uint_t length;   //< length of buffer 
	ba_uint_t channels; //< number of channels 
	smpl_t **data*/
	
	float scl = 10;
	int bufferSize = out->length;
	
	glPushMatrix();
	
		glTranslatef(x,y,0);
		
		for( int i = 0; i < bufferSize; i++)
		{
			ofLine(i*10,0,i*10,out->data[0][i]);
			if(out->data[0][i] > 0 ) cout << "beat: " << i << " at " << out->data[0][i] << endl;
		}
	
	glPopMatrix();
}

