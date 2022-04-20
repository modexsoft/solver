#include <errno.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/soundcard.h>

#define BUF_SIZE 4096
#define DEV_NAME "/dev/dsp"
#define MAX_SOUNDS 6
#define MAX_CHANNELS 8
#define DEFAULT_READ_POSITION 44

enum sounds{wrong,correct,typing,selection,move,drop};
enum sound_states{PLAY,STOP};

struct wavfile {
	int	size;
	int 	maxsize;
	int	nframes;
	unsigned char* current_sound;
}wavfiles[MAX_SOUNDS];

struct wavchannel {
	int	read_position;
	int	state;
	int 	sound_number;
} wavchannels[MAX_CHANNELS];

struct fileoperation {
	int 	devfd;
}fileops;

// Assign wav file and a state to a given channel. Check for a channel that is 
// not in use. Report which channel has been selected.
int wav_setstate(int sound_number, int state) {
	int return_channel = -1;
	for (int i = 0; i < MAX_CHANNELS; i++) {
		if (wavchannels[i].state == STOP) {
			wavchannels[i].state = state;
			wavchannels[i].sound_number = sound_number;
			return_channel = i;
			break;
		}
	}

	return return_channel;
}

// Overwrite the current state of a given channel. Report the current state of 
// the channel.
int wav_resetstate(int channel, int state) {
	wavchannels[channel].state = state;

	return 0;
}

// Report the state of a given channel. If it is playing or stopped.
int wav_getstate(int channel) {
	int currentstate = wavchannels[channel].state;

	return currentstate;
}

// Check if the channel has not been stopped. Get the channels wav file number. 
// If the we have not reached the end of the file then process. Go to the next 
// bit of data for the channel. Combine the channel data into a buffer. Add to 
// the next buffer and increase the buffer length. Do this until the buffer 
// has been filled up. Write the buffer contents out to the device. Go through 
// each channel and check the read position. If the read position is  at the 
// end set the state to STOP and then set the position to the default
int wav_process() {
	unsigned char buf[BUF_SIZE];
	size_t buf_len = 0;
	int snd = 0;
	int temp_a = 0;
	int temp_buf = 0;
	int limit = 32768;

	while (buf_len < BUF_SIZE) {
	temp_buf = limit;
	for (int i = 0;i<MAX_CHANNELS;i++) {
		if (wavchannels[i].state != STOP) {
			snd = wavchannels[i].sound_number;
			if (wavchannels[i].read_position < wavfiles[snd].maxsize) {
				temp_a = (int)wavfiles[snd].current_sound[wavchannels[i].read_position];
				temp_buf += (temp_a * temp_buf)/limit;
				wavchannels[i].read_position++;
			}
		}
	}

	buf[buf_len] = (unsigned char)temp_buf;
	buf_len++;
	}
	write(fileops.devfd, buf, buf_len);

	for (int i = 0;i<MAX_CHANNELS;i++) {
		snd = wavchannels[i].sound_number;
		if (wavchannels[i].read_position >= wavfiles[snd].maxsize) {
			wavchannels[i].state = STOP;
			wavchannels[i].read_position  = DEFAULT_READ_POSITION;
		}
	}

	return 0;
}

// Load a set of predefined wav files and their data. Open the sound device 
// (/dev/dsp). Set each channel to the default state and read position.
int wav_init() {
	int nchannels = 1;
	int framerate = 22050;
	int devformat = AFMT_S16_LE;
	int sfx[MAX_SOUNDS][3] = { 
	{7354,14798,14752},{8644,17288,17332},{2536,5072,5116},
        {2622,5244,5288},{2976,5952,5996},{4774,9548,9592}
	};

	unsigned char *sfx_name[MAX_SOUNDS] = {data0,data1,data2,data3,data4,data5};

	for (int i = 0; i < MAX_SOUNDS; i++) {
		wavfiles[i].nframes = sfx[i][0];
		wavfiles[i].size = sfx[i][1];
		wavfiles[i].maxsize = sfx[i][2];
		wavfiles[i].current_sound = sfx_name[i];
	}

	fileops.devfd = open(DEV_NAME, O_WRONLY);	

	if (fileops.devfd > -1) {
		fcntl(fileops.devfd, F_GETFD);
			if (errno == EBADF) {
				fileops.devfd = -1;
			}
	}

	ioctl(fileops.devfd, SNDCTL_DSP_RESET, NULL);
	ioctl(fileops.devfd, SNDCTL_DSP_SETFMT, &(devformat));
	ioctl(fileops.devfd, SNDCTL_DSP_CHANNELS, &(nchannels));
	ioctl(fileops.devfd, SNDCTL_DSP_SPEED, &(framerate));	

	for (int i = 0; i < MAX_CHANNELS; i++) {
		wavchannels[i].state = STOP;
		wavchannels[i].read_position  = DEFAULT_READ_POSITION;
	}

	return 0;
}

// Close the device currently in use.
int wav_uninit() {
	if (fileops.devfd) {
		close(fileops.devfd);
	}

	return 0;
}
