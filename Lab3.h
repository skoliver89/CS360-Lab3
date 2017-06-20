#ifndef LAB3_H_
#define LAB3_H_

int initialize (int argc, char ** argv);

int readFile(FILE *fp);

int findHeader(unsigned char * data, size_t bytesRead);

void bitRate(int bits[]);

void freq(int bits[]);

void copyright(int bits[]);

int isMP3(int bits[]);

void original(int bits[]);

#endif