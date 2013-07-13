#include <stdio.h>
#include <string.h>
#include <math.h>

#define PACKAGE_SIZE 10
#define REDUNDANCY 3
#define BW 200
#define FREQUENCY 10000
#define SNR 500

#define PROB_ERROR 10

//#define PROB_GIVEN
#define PROB_CALC

/**********************************************/
/**     BW em Hz	PACKAGE_SIZE em bits       **/
/**********************************************/

/**********************************************/
/**   teste: pktsze 10, red 3, bw 100,       **/
/**   freq 10000, snr 50, prob_calc, seed 10 **/
/**********************************************/

// apresentação: willian, crc, erro
//               marcelo, estrutura de input, fluxo de execução para detecção de erro



void bin2str(long int *x, long int num_of_bits);
long int str2bin (char* str, long int size);
long int crc (long int package, long int gen);
double error ();

int main () {
	
	char buffer[PACKAGE_SIZE+1];
	long int package;
	long int gen;
	long int rem;
	double error_prob = error();
	long int error;
	long int i;

	printf("Channel error: %f%% per bit\n", error_prob*100);
	srand(10);
    while(1) {
   		// get the polynomial generator
       	do {
        	printf("generator: ");
          	gets(buffer);
       	} while (strlen(buffer) != REDUNDANCY + 1);
       	gen = str2bin(buffer, REDUNDANCY+1);
       
       	// get the package
       	do {
        	printf("package: ");
          	gets(buffer);
       	} while (strlen(buffer) != PACKAGE_SIZE-REDUNDANCY && buffer[0] != '-');
       	package = str2bin(buffer, PACKAGE_SIZE-REDUNDANCY);
        
       	while (buffer[0] != '-') {
      		// create CRC redundancy
       		package = package << REDUNDANCY;
       		rem = crc(package, gen);
       		package += rem;

       		printf("Redundancy: ");
       		bin2str(&rem, REDUNDANCY);
       		printf("Codeword: ");
       		bin2str(&package, PACKAGE_SIZE);

          // apply error
          for (i=PACKAGE_SIZE-1;i>=0;i--) {
				    #ifdef PROB_CALC
				    error = (((float)(rand()%100))/100)>error_prob;
				    #endif
				    #ifdef PROB_GIVEN
				    error = (rand()%100)>PROB_ERROR;
				    #endif
				    //printf("error: %d, packmod2: %d\n", error, package);
				    buffer[i] = 48+(!(package%2 ^ error));
				    package = package >> 1;
          }
          buffer[PACKAGE_SIZE] = '\0';

          package = str2bin(buffer, PACKAGE_SIZE);

          printf("Codeword after transmission: ");
          bin2str(&package, PACKAGE_SIZE);

          // CRC
          rem = crc(package, gen);
          printf("%s\n", rem==0?"OK":"ERROR");
          
          // get the package
          do {
            printf("package: ");
            gets(buffer);
          } while (strlen(buffer) != PACKAGE_SIZE-REDUNDANCY && buffer[0] != '-');
          package = str2bin(buffer, PACKAGE_SIZE-REDUNDANCY);
       	}
  	}
}

long int str2bin (char* str, long int size) {

    long int i,j,len,r=0,w;
    
    for(i=0,j=size-1;i<size;i++,j--) {
       w = pow(2, i);
       r+=(str[j]-48)*w;
    }     
    return r;
}

void bin2str(long int *x, long int num_of_bits) {
    unsigned long int shift = pow(2, num_of_bits-1);
    unsigned long int shifted;
    unsigned long int bin;
    long int i;

    for(i=0; i<num_of_bits; i++)
    {
        shifted = shift >> i;
        bin = (*x & shifted);

        if (bin == 0)
            printf("0");
        else
            printf("1");
    }
    printf("\n");
}

long int crc (long int package, long int gen) {

	long int gen_temp;
	long int rem;
	long int checker;

	gen_temp = gen << (PACKAGE_SIZE - REDUNDANCY - 1);
	rem = package ^ gen_temp;
	checker = pow(2,PACKAGE_SIZE-1);
	while (gen_temp > gen) {
		gen_temp = gen_temp >> 1;
		checker = checker >> 1;
		if (rem >= checker)
			rem = rem ^ gen_temp;
	}

	return rem;
}

double error () {

	// first calculate Es/N0 (energy per symbol to noise power spectral density)
	double EsN0;
	EsN0 = (double)SNR*BW/FREQUENCY;
	EsN0 = sqrt(EsN0);

	// than calculate the gaussian error function of Es/N0
	double t = 1/(1+0.3275911*EsN0);
	double errf = ((t*(0.254829592+t*(-0.284496736+t*(1.421413741+t*(-1.453152027 + t*1.061405429)))))*exp(-EsN0*EsN0));
	//printf("EsN0: %f, t: %f, erf: %f\n", EsN0, t, errf);
	return errf / 2;

}
