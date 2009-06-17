/*
 * wavrec/mpegrec for Linux ( Direct-to-disk WAV file and MP3 capture )
 *                          ( MP3 is available via lame: www.sulaco.org/mp3 )
 * Developed by Andrew L. Sandoval -- sandoval@netwaysglobal.com -- Feb. 2000
 *
 * Copyright (C) 2000 Andrew L. Sandoval
 * 
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 * 
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 * 
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 *  ChangeLog:
 *  	2008-03-28 Use Recorder class. penglj
 */

#include <iostream>
#include <string>
#include <vector>
#include <deque>
#include <cstring>
#include <map>
//#include "record.h"
extern "C"
{
 #include <stdio.h>
 #include <unistd.h>
 #include <sys/ioctl.h>
 #include <sys/soundcard.h>
 #include <pthread.h>
 #include <signal.h>
 #include <sys/types.h>
 #include <sys/stat.h>
 #include <fcntl.h>
 #include <stdio.h>
 #include <errno.h>
}

#define BUFFER_SIZE	65535
#define MIN_BUFFERS	10
#define WAV_ID_RIFF 0x46464952 /* "RIFF" */
#define WAV_ID_WAVE 0x45564157 /* "WAVE" */
#define WAV_ID_FMT  0x20746d66 /* "fmt " */
#define WAV_ID_DATA 0x61746164 /* "data" */
#define WAV_ID_PCM  0x0001


using namespace std;
class Recorder
{
	public:
		struct WaveHeader
		{
			unsigned long riff;
			unsigned long file_length;
			unsigned long wave;
			unsigned long fmt;
			unsigned long fmt_length;
			short fmt_tag;
			short channels;
			unsigned long sample_rate;
			unsigned long bytes_per_second;
			short block_align;
			short bits;
			unsigned long data;
			unsigned long data_length;
		};

	private:
		static WaveHeader m_genericWaveHeader;

		struct SndBuffer
		{
			unsigned long used;
			unsigned char buffer[BUFFER_SIZE];
		}*m_pSndbuf;

		int m_kbps;
		string m_enc_proc_name;
		int m_sec;
		int m_rate;
		string m_out_fname;
		string m_extra;
		deque <struct SndBuffer *> m_inputBuffers;
		deque <struct SndBuffer *> m_fullBuffers;
		static int m_recordingNow;
		static int m_doneRecording;
		static int m_stopRequested;
		static int m_binitialized;
		static pthread_mutex_t m_fullBuffersLock;
		static pthread_mutex_t m_inputBuffersLock;
		static int m_outputFd;
	private:
		void stop(int signal);
	public:
		Recorder(void);
		Recorder(int kbps, char *enc_proc_name, int sec, int rate, char *o_fname, char *extra);
		int start(void);
		int start(int kbps, char *enc_proc_name, int sec, int rate, char *o_fname, char *extra);
		unsigned long shipToEncoder(unsigned long *processed);
		int is_recording_done() {return m_doneRecording;};
		void buffer_prepare();
		void input_buffer_push();
		void input_buffer_pop();
		long int input_buffer_get_size();
		void input_buffer_ready();
		int get_bytes_per_sec();
};
Recorder::WaveHeader Recorder::m_genericWaveHeader = { WAV_ID_RIFF, 0xFFFFFFFF, WAV_ID_WAVE,
	WAV_ID_FMT, 16, WAV_ID_PCM, 
	2, 44100, 192000,
	4, 16, WAV_ID_DATA,
	0xFFFFFFFF };
int Recorder::m_recordingNow = 0;
int Recorder::m_doneRecording = 0;
int Recorder::m_stopRequested = 0;
int Recorder::m_binitialized = 0;
pthread_mutex_t Recorder::m_fullBuffersLock = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t Recorder::m_inputBuffersLock = PTHREAD_MUTEX_INITIALIZER;
int Recorder::m_outputFd = 1;

Recorder::Recorder() 
{
	m_kbps = 128; 
	m_sec = 0; 
	m_rate = 44100; 
	m_enc_proc_name = "lame";  
	m_out_fname = m_extra = "";
}

Recorder::Recorder(int kbps, char *enc_proc_name, int sec, int rate, char *o_fname, char *extra)
{
	m_kbps = kbps;
	m_sec = sec;
	if (enc_proc_name)
	{
		m_enc_proc_name = enc_proc_name; 
	}
	else
	{
		m_enc_proc_name = ""; 
	}
	m_sec = sec;
	m_rate = rate;
	if (o_fname)
	{
		m_out_fname = o_fname;
	}
	else
	{
		m_out_fname = "";
	}
	if (extra)
	{
		m_extra = extra;
	}
	else
	{
		m_extra = "";
	}
}
void Recorder::stop(int signal)
{
	m_stopRequested++;
}

int Recorder::get_bytes_per_sec()
{
	return m_rate<<2;
}
unsigned long Recorder::shipToEncoder(unsigned long *processed)
{
	unsigned long lastSize;

	pthread_mutex_lock( &m_fullBuffersLock );
	lastSize = m_fullBuffers.size();
	if(!lastSize)
	{
		pthread_mutex_unlock( &m_fullBuffersLock );
		if(processed) *processed = 0;
		return lastSize;
	}
	m_pSndbuf = m_fullBuffers[0];
	m_fullBuffers.pop_front();
	pthread_mutex_unlock( &m_fullBuffersLock );
	write( m_outputFd, reinterpret_cast<void*>(m_pSndbuf->buffer), m_pSndbuf->used );
	if(processed) *processed = m_pSndbuf->used;
	lastSize--;
	//delete m_pSndbuf;
	return lastSize;
}

static void *encode(Recorder *pRec)
{
	unsigned long lastSize = 1;
	unsigned long bytesProcessed = 0;
	unsigned long Processed = 0;

	while(!pRec->is_recording_done() || lastSize)
	{
		lastSize = pRec->shipToEncoder(&Processed);
		bytesProcessed += Processed;
		if( bytesProcessed > (pRec->get_bytes_per_sec() / 2) )
		{
			bytesProcessed = 0;
		}
		if(!lastSize)
		{
			sched_yield();
			continue;
		}
	}
}

void Recorder::buffer_prepare()
{
	m_pSndbuf = new SndBuffer;
	memset( reinterpret_cast<void*>(m_pSndbuf), 0, sizeof(SndBuffer) );
}
void Recorder::input_buffer_push()
{
	pthread_mutex_lock( &m_inputBuffersLock );
	m_inputBuffers.push_back(m_pSndbuf);
	pthread_mutex_unlock( &m_inputBuffersLock );
}
void Recorder::input_buffer_pop()
{
	pthread_mutex_lock( &m_inputBuffersLock );
	m_inputBuffers.pop_front();
	pthread_mutex_unlock( &m_inputBuffersLock );
}
long int Recorder::input_buffer_get_size()
{
	long int size;
	pthread_mutex_lock( &m_inputBuffersLock );
	size =  m_inputBuffers.size();
	pthread_mutex_unlock( &m_inputBuffersLock );
	return size;
}
void Recorder::input_buffer_ready()
{
	m_binitialized = 1;
}

static void *prepareBuffers(Recorder *pRec)
{
	while(1)
	{
		// Prepare enough input buffers for one second:
		unsigned long onesecond = (pRec->get_bytes_per_sec()) / BUFFER_SIZE;
		onesecond++;

		for(int i=0; i < onesecond; i++)
		{ 
			pRec->buffer_prepare();
			pRec->input_buffer_push();
		}
		unsigned long sz = pRec->input_buffer_get_size();
		pRec->input_buffer_ready();

		while(sz > MIN_BUFFERS)
		{
			sched_yield();
			sz = pRec->input_buffer_get_size();
		}
	}
	return NULL;
}

//void Recoder::start(int argc, char *argv[])
int Recorder::start()
{
	char tmp[100];
	int userSuppliedEncoderExtras = 0;
	FILE *lame_encoder = NULL;

	m_genericWaveHeader.sample_rate = (m_rate<<2);
	m_genericWaveHeader.bytes_per_second = m_kbps;

	m_genericWaveHeader.data_length = (m_rate<<2) * (m_sec + 1);
	m_genericWaveHeader.file_length = m_genericWaveHeader.data_length +
		sizeof(m_genericWaveHeader);

#if 0
	// Setup the signal handler so we stop on CTRL-C
	signal( SIGHUP, stop );
	signal( SIGINT, stop );
	signal( SIGQUIT, stop );
	signal( SIGTERM, stop );
	signal( SIGPIPE, stop );
#endif

	// Start the buffer supply thread...
	pthread_t prepbufThread;
	pthread_create( &prepbufThread, NULL, reinterpret_cast<void* (*)(void *)>(prepareBuffers), this);
	while( !m_binitialized ) sched_yield();  //Give prepareBuffers a chance

	// Begin recording and continue until recordingNow = 0 or bytes of input
	// equals seconds requested...
	unsigned long totalBytesRequested = 0;
	if (m_sec) totalBytesRequested = m_sec * (m_rate << 2);

	// If output file is specified and this is NOT an argument of mpegrec
	if(m_enc_proc_name == "lame")
	{
		m_enc_proc_name += " -ms -b ";
		sprintf(tmp, "%d", m_kbps);
		m_enc_proc_name += tmp;
		m_enc_proc_name += " ";
		m_enc_proc_name += m_extra;
		m_enc_proc_name += " - ";
		m_enc_proc_name += m_out_fname;
	}
	else
	{
		m_enc_proc_name += " ";
		m_enc_proc_name += m_extra;
	}

	lame_encoder = popen( m_enc_proc_name.c_str(), "w" );
	if(!lame_encoder)
	{
		perror( m_enc_proc_name.c_str() );
		return 1;
	}
	m_outputFd = fileno( lame_encoder );

	//
	// Open the sound card
	//
	int dsp = open( "/dev/dsp", O_RDONLY );
	if(dsp == -1)
	{
		perror( "/dev/dsp" );
		return 1;
	}

	// Reset the card, set the format, stereo, and speed...
	ioctl( dsp, SNDCTL_DSP_RESET, 0);

	int format = AFMT_S16_LE;
	if( ioctl(dsp, SNDCTL_DSP_SETFMT, &format) == -1)
	{
		perror( "SNDCTL_DSP_SETFMT" );
		return 1;
	}

	int stereo = 1;
	if( ioctl(dsp, SNDCTL_DSP_STEREO, &stereo) == -1)
	{
		perror( "SNDCTL_DSP_STEREO" );
		return 1;
	}

	int samplerate = m_rate;
	if( ioctl(dsp, SNDCTL_DSP_SPEED, &samplerate) == -1)
	{
		perror( "SNDCTL_DSP_SPEED" );
		return 1;
	}

	//
	// Begin Recording...
	//
	m_recordingNow = 1;

	//
	// Write the WAV Header (generic because the length is MAX)
	//
	write( m_outputFd, reinterpret_cast<void*>(&m_genericWaveHeader),
			sizeof(m_genericWaveHeader) );

	// Start the encode supply thread...
	pthread_t encodeThread;
	pthread_create( &encodeThread, NULL, reinterpret_cast<void* (*)(void *)>(&encode), this);

	unsigned long bytesRead = 0; 
	while(1)
	{
		pthread_mutex_lock( &m_inputBuffersLock );
		m_pSndbuf = m_inputBuffers[0];
		if(!m_pSndbuf)
		{
			pthread_mutex_unlock( &m_inputBuffersLock );
			m_pSndbuf = new SndBuffer;
			m_pSndbuf->used = 0;
		}
		else m_inputBuffers.pop_front();
		pthread_mutex_unlock( &m_inputBuffersLock );
		m_pSndbuf->used = read( dsp, reinterpret_cast<void*>(m_pSndbuf->buffer),
				BUFFER_SIZE );
		bytesRead += m_pSndbuf->used;

		pthread_mutex_lock( &m_fullBuffersLock );
		m_fullBuffers.push_back( m_pSndbuf );
		pthread_mutex_unlock( &m_fullBuffersLock );

		if( m_stopRequested ) break;
		if(m_sec && bytesRead > totalBytesRequested) break;
	}

	m_recordingNow = 0;
	m_doneRecording = 1;
	// Reset the card
	ioctl( dsp, SNDCTL_DSP_RESET, 0);
	close( dsp );

	pthread_join( encodeThread, NULL );  //Do NOT exit until output is written
	while(shipToEncoder(NULL)) ;

	pclose( lame_encoder );
}
int Recorder::start(int kbps, char *enc_proc_name, int sec, int rate, char *o_fname, char *extra)
{
	m_kbps = kbps;
	m_rate = rate;
	if (o_fname != NULL)
	{
		m_out_fname = o_fname;
	}
	if (enc_proc_name != NULL)
	{
		m_enc_proc_name = enc_proc_name;
	}
	if (extra != NULL)
	{
		m_extra = extra;
	}
	m_sec = sec ? sec : 3600 * 24;  //24hr Max.
	return start();
}

int main()
{
	Recorder rec(128, "lame", 10, 44100, "/tmp/rec.mp3", NULL);
	rec.start();
	while(1);
}
