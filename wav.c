/*
 *
 * ao_example.c
 *
 *     Written by Stan Seibert - July 2001
 *
 * Legal Terms:
 *
 *     This source file is released into the public domain.  It is
 *     distributed without any warranty; without even the implied
 *     warranty * of merchantability or fitness for a particular
 *     purpose.
 *
 * Function:
 *
 *     This program opens the default driver and plays a 440 Hz tone for
 *     one second.
 *
 * Compilation command line (for Linux systems):
 *
 *     gcc -o ao_example ao_example.c -lao -ldl -lm
 *
 */
#include <wiringPi.h>
#include <unistd.h>
#include <pthread.h>
#include <stdio.h>
#include <string.h>
#include <ao/ao.h>
#include <ao/plugin.h>
#include <math.h>

#define BUF_SIZE 4096*10

int sample_count;
int sample_size;
int sample_rate;
int cube_shape2[5][25] = {0};

struct WavHeader
{
	char chunk_id[4];
	int chunk_size;
	char format[4];
	char subchunk1_id[4];
	int subchunk1_size;
	short audio_format;
	short num_channels;
	int sample_rate;
	int byte_rate;
	short block_align;
	short bits_per_sample;
	char subchunk2_id[4];
	int subchunk2_size;
};

struct WavHeader* pHeader;

void set_wave(int level, int x, int arr[][25])
{
	int i, j;
	int shape[5][25] = {{0, 0, 0, 0, 0,  1, 1, 1, 1, 1,  1, 1, 1, 1, 1,  1, 1, 1, 1, 1,  1, 1, 1, 1, 1 },
				{0, 0, 0, 0, 0,  1, 1, 0, 1, 1,  1, 1, 1, 1, 1,  1, 1, 1, 1, 1,  1, 1, 1, 1, 1 },
				{0, 0, 0, 0, 0,  1, 0, 0, 0, 1,  1, 1, 0, 1, 1,  1, 1, 1, 1, 1,  1, 1, 1, 1, 1 },
				{0, 0, 0, 0, 0,  1, 0, 0, 0, 1,  1, 0, 0, 0, 1,  1, 1, 0, 1, 1,  1, 1, 1, 1, 1 },
				{0, 0, 0, 0, 0,  0, 0, 0, 0, 0,  1, 0, 0, 0, 1,  1, 0, 0, 0, 1,  1, 1, 0, 1, 1 },
				};
		for(i=0;i<5;i++)
			for(j=0;j<5;j++)
				arr[i][x*5+j] = shape[level][i*5+j];
}

void next_wave(int arr[][25])
{
	int x, i, j;
	for(x=4;x>=1;x--)
		for(i=0;i<5;i++)
			for(j=0;j<5;j++)
                		arr[i][x*5+j] = arr[i][(x-1)*5+j];
}

void handle_pcm_wave(char* buffer)
{
	int i, j, k;
	int pLED = 0;
	int total_time = sample_count/ pHeader->num_channels / sample_rate;
	int led_rate = 24;
	int my_sample_count = total_time * led_rate;
	printf("total time: %d", total_time);
	int cube_shape2[5][25];
	int raw_idx;
	int* pcm;
	int rPcm;
	int on_num;
	for(i=0;i<5;i++)
		for(j=0;j<25;j++)
			cube_shape2[i][j] = 1;

	for(i=0;i < my_sample_count;i++)
	{
		if(i%10==0)
		{
			
			raw_idx = i*sample_rate*pHeader->num_channels/led_rate;
			pcm = (int*) &buffer[raw_idx];
			rPcm = 0xFF & (*pcm);
			if(pHeader->num_channels >= 2)
				rPcm |= (0xFF00) & (*pcm);
			rPcm = (int)log( (double)((unsigned int)++rPcm) );
			on_num = 0;
			on_num += rPcm*4/11;
			if(on_num>=5)
				on_num = 4;
			next_wave(cube_shape2);
			set_wave(on_num, 0, cube_shape2);

		}
		cube_output(cube_shape2);
		delayMicroseconds(18000);
	}
	fprintf(stderr, "here\n");
	return;
}

void refresh_cube()
{
	while(1)
	{
		cube_output(cube_shape2);
		delay(10);
	}
}

void print_wave(int n)
{
	int rPcm;
	int on_num;
	//printf("print wabe n: %d\n", n);
	
	rPcm = (int)log( (double)((unsigned int)++n) );
	on_num = 0;
	on_num += rPcm*4/11;
	if(on_num>=5)
		on_num = 4;
	//printf("on_num: %d\n", on_num);
	next_wave(cube_shape2);
	set_wave(on_num, 0, cube_shape2);
	//cube_output(cube_shape2);
	return;
}

void stream_audio(ao_device* device, char* buffer, int sample_size, int sample_count, int rate)
{
	int packet_size = rate / 5;
	char* ptr = buffer;
	int idx = 0;
	int i=0;
	int mask = 0xff;
	int total = 0;
	int sample_per_packet = packet_size / sample_size;
	if(sample_size == 2)
		mask = 0xffff;
	while(idx < sample_size * sample_count)
	{
		/*
		for(i = idx;i<idx+packet_size;i+=sample_size)
		{
			total += ( (*(int*)(ptr+i)) & mask);
		}
		total /= (sample_per_packet);
		*/
		
		print_wave(mask & (*(int*)(buffer+idx)));
		ao_play(device, ptr+idx, packet_size);
		idx += packet_size;
	}
}

int main(int argc, char **argv)
{
	wiringPiSetup();
	st_pin();
	ao_device *device;
	ao_sample_format format;
	int default_driver;
	char *buffer;
	int buf_size;
	int sample;
	float freq = 440.0;
	int i, j;
	for(i=0;i<5;i++)
		for(j=0;j<25;j++)
			cube_shape2[i][j] = 1;

	
	FILE* pF = fopen("./One_Republic-Stop_And_Stare.wav", "rb");
	if(!pF)
	{
		fprintf(stderr, "err while reading file!");
		return -1;
	}

	printf("sizeof wavheader: %d\n", sizeof(struct WavHeader));
	struct WavHeader header;
	memset((void*)&header, 0, sizeof(header));
	fread ((void*)&header, sizeof(header), 1, pF);
	printf("chunk2_size: %d\n", header.subchunk2_size);
	printf("num of channels: %d\n", header.num_channels);
	sample_count = header.subchunk2_size/(header.bits_per_sample/8);
	sample_size = (header.bits_per_sample/8);
	printf("sample_count: %d\n", sample_count);
	char* buffer2 = malloc(sample_count * sample_size);
	printf("sample_count * samaple_size: %d\n", sample_count*sample_size);
	fread (buffer2, sample_size, sample_count, pF);
	sample_rate = header.sample_rate;
	//for(i=0;i<sample_count * sample_size;i++)
	//	printf("test %d\n", (int)buffer2[i]);
	pHeader = &header;

	/* -- Initialize -- */

	fprintf(stderr, "libao example program\n");

	ao_initialize();

	/* -- Setup for default driver -- */

	default_driver = ao_default_driver_id();
//	default_driver = ao_driver_id("ALSA");
//	default_driver = 1;

	printf("default driver id: %d\n", default_driver);

        memset(&format, 0, sizeof(format));
	format.bits = header.bits_per_sample;
	format.channels = header.num_channels;
	format.rate = header.sample_rate;
	format.byte_format = AO_FMT_LITTLE;

	/* -- Open driver -- */
	device = ao_open_live(default_driver, &format, NULL /* no options */);
	if (device == NULL) {
		fprintf(stderr, "Error opening device.\n");
		return 1;
	}

	/* -- Play some stuff -- */
	buf_size = format.bits/8 * format.channels * format.rate;
	buffer = calloc(buf_size,
			sizeof(char));

	for (i = 0; i < format.rate; i++) {
		sample = (int)(0.75 * 32768.0 *
			sin(2 * M_PI * freq * ((float) i/format.rate)));

		/* Put the same stuff in left and right channel */
		buffer[4*i] = buffer[4*i+2] = sample & 0xff;
		buffer[4*i+1] = buffer[4*i+3] = (sample >> 8) & 0xff;
	}
	pthread_t thread1, thread2;
	pthread_create(&thread1, NULL, refresh_cube, NULL);
	//ao_play(device, buffer2, sample_count*sample_size);
	stream_audio(device, buffer2, sample_size, sample_count, header.sample_rate);
	/* -- Close and shutdown -- */
	ao_close(device);

	ao_shutdown();

  return (0);
}



