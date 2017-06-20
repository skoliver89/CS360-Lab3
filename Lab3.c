#include <stdio.h>
#include <stdlib.h>
#include "Lab3.h"

FILE *fp = NULL;

int main ( int argc, char ** argv)
{
	initialize(argc, argv);
	readFile(fp);
	
	/* Clean up */
	fclose(fp);						/* close and free the file */
	/*free((unsigned char *)data); */
	exit(EXIT_SUCCESS);				/* or return 0; */
}

/* Open the file given on the command line */
int initialize (int argc, char ** argv) 
{
	if( argc != 2 )
	{
		printf( "Usage: %s filename.mp3\n", argv[0] );
		return(EXIT_FAILURE);
	}
	
	fp = fopen(argv[1], "rb");
	if( fp == NULL )
	{
		printf( "Can't open file %s\n", argv[1] );
		return(EXIT_FAILURE);
	}
	return 0;
}

/*How many bytes are there in the file?  If you know the OS you're */
/* on you can use a system API call to find out.  Here we use ANSI standard */
/* function calls. */
int readFile (FILE *fp)
{
	const int max_size = 10485760;		/* max file size (bytes) [10 MB] */
	const int min_size = 1;				/* min file size (bytes) */
	
	double size = 0;
	fseek( fp, 0, SEEK_END );		/* go to 0 bytes from the end */
	size = ftell(fp);				/* how far from the beginning? */
	rewind(fp);						/* go back to the beginning */
	
	if( size < min_size || size > max_size )
	{
		printf("File size is not within the allowed range\n");
		return 0;
	}
	
	double sizeMB = size/1048576;
	
	/* Allocate memory on the heap for a copy of the file */
	 unsigned char * data = (unsigned char *)malloc(size);
	/* Read it into our block of memory */
	size_t bytesRead = fread( data, sizeof(unsigned char), size, fp );
	if( bytesRead != size )
	{
		printf( "Error reading file. Unexpected number of bytes read: %zu\n",bytesRead );
		free((unsigned char *)data);	/* plug mem leak! */
		return 0;
	}
	/* We now have a pointer to the first BYTE of data in a copy of the file, have fun */
	/* unsigned char * data    <--- this is the pointer  (data[0...eof])*/
	
	int i = findHeader(data, size);
	int bitArray[32] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	int *bitPtr;
	bitPtr = bitArray;
	int bitCount = 0;
	
	int j;
	if (i >= 0)
	{
		for (j=i;j<i+4;j++)
		{
			int k;
			for (k = 0; k < 8; k++)
			{
				*(bitPtr + bitCount) = (data[j] & 0x80)/128; /* most sig bit and shift */
				data[j] <<= 1;
				bitCount++;
			}
		}
		
	}
	else
	{
		printf("NO HEADER FOUND!");
		free((unsigned char *)data);	/* plug mem leak! */
		return 0;
	}
	
	if (isMP3(bitArray) == 1)
	{
		printf( "File size: %.02f MB\n", sizeMB);
		bitRate(bitArray);
		freq(bitArray);
		copyright(bitArray);
		original(bitArray);
	}
	else
	{
		printf("File not MPEG LAYER III");
	}
	free((unsigned char *)data);	/* plug mem leak! */
	return 0;
} 

/* (simple) Find the index in data where the header starts */
int findHeader(unsigned char * data, size_t bytesRead)
{
	int i = 0;
	unsigned int next_left;
	while (i < bytesRead) 
	{
		/*leftNib = data[i];
		leftNib>>=4;
		rightNib = data[i] & 0xF; */
		next_left = data[i+1];
		next_left>>=4;
		
		if (data[i] == 255 && next_left == 15)
		{
			return i;
		}
		i++;
	}
	return -1; /* no header found! */
}

void bitRate(int bits[])
{
	int value = 0;
	if (bits[16] == 1)
	{
		value += 8;
	}
	if (bits[17] == 1)
	{
		value += 4;
	}
	if (bits[18] == 1)
	{
		value += 2;
	}
	if (bits[19] == 1)
	{
		value += 1;
	}
	
	if (value >= 0 && value <= 10)
	{
		switch (value)
		{
			case 10 :
				value = 160;
				break;
			case 9 :
				value = 144;
				break;
			case 8 :
				value = 128;
				break;
			case 7 :
				value = 112;
				break;
			case 6 :
				value = 96;
				break;
			case 5 :
				value = 80;
				break;
			case 4 :
				value = 64;
				break;
			case 3 :
				value = 56;
				break;
			case 2 :
				value = 48;
				break;
			case 1 :
				value = 40;
				break;
			case 0 :
				value = 32;
				break;			
			default :
				value = -1;
		}
		printf("Bit Rate: %i kbps\n", value);
	}
	else
	{
		printf("Bit Rate: bad/unknown kbps\n");
	}
}

void freq(int bits[])
{
	int value = 0;
	if (bits[20] == 1)
	{
		value += 2;
	}
	if (bits[21] == 1)
	{
		value += 1;
	}
	
	switch (value)
	{
		case 0:
			value = 44100;
			break;
		case 1:
			value = 0;
			break;
		case 2 :
			value = 0;
			break;
		case 3 :
			value = 0;
			break;
	}
	
	printf("Frequency: %d Hz\n", value); /* convert from Hz to kHz and print */
}

void copyright(int bits[])
{
	if (bits[28] == 1)
	{
		printf("Copyright: Yes\n");
	}
	else
	{
		printf("Copyright: No\n");
	}
}

void original(int bits[])
{
	if (bits[29] == 1)
	{
		printf("Original: Yes\n");
	}
	else
	{
		printf("Original: No\n");
	}
}

int isMP3(int bits[])
{
	if (bits[12] == 1 && bits[13] == 0 && bits[14] == 1)
	{
		return 1;
	}
	
	return 0;
}