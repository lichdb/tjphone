#define BENCHMARK	2000

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>

#include "gsm.h"

void main()
{
    int i, wrong = 0;
    gsm gsmh;
    gsm_signal a = -8000, src[160], dec[160];
    gsm_frame dst;
#ifdef BENCHMARK
	long start, e, d;
#endif

	static gsm_signal correctResults[] = {
		-7168,
		-7320,
		-7528,
		-8792,
		-9168,
		-9664,
		-11032,
		-11104,
		-11672,
		-12744,
		-12760,
		-12856,
		-13872,
		-13840,
		-13872,
		-14792,
		-14688,
		-14632,
		-15512,
		-15336,
		-15248,
		-16088,
		-15880,
		-15752,
		-16560,
		-16328,
		-16176,
		-16968,
		-16712,
		-16544,
		-17312,
		-17040,
		-16864,
		-17608,
		-17328,
		-17128,
		-17872,
		-17568,
		-17368,
		-17072,
		-16704,
		-16480,
		-16152,
		-15776,
		-15472,
		-15088,
		-14704,
		-14328,
		-13944,
		-13560,
		-13192,
		-12816,
		-12448,
		-12088,
		-11728,
		-11368,
		-11032,
		-10696,
		-10360,
		-10048,
		-9736,
		-9424,
		-9128,
		-8840,
		-8552,
		-8288,
		-8024,
		-7760,
		-7512,
		-7272,
		-6800,
		-6568,
		-6344,
		-5888,
		-5656,
		-5432,
		-4992,
		-4792,
		-4576,
		-4384,
		-4192,
		-3872,
		-3664,
		-3504,
		-3136,
		-2984,
		-2840,
		-2496,
		-2360,
		-2216,
		-1888,
		-1768,
		-1640,
		-1328,
		-1224,
		-1104,
		-816,
		-720,
		-624,
		-272,
		-184,
		-96,
		248,
		320,
		400,
		736,
		792,
		872,
		1192,
		1240,
		1304,
		1544,
		1720,
		1776,
		2000,
		2176,
		2224,
		2440,
		2608,
		2640,
		2664,
		2952,
		3048,
		3080,
		3272,
		3408,
		3416,
		3680,
		3800,
		3800,
		4072,
		4184,
		4184,
		4448,
		4552,
		4544,
		4800,
		4896,
		4888,
		5136,
		5272,
		5256,
		5496,
		5632,
		5608,
		5848,
		5976,
		5944,
		6176,
		6296,
		6256,
		6480,
		6544,
		6584,
		6808,
		6856,
		6896,
		7112,
		7160,
		7192
};    

    gsmh = gsm_create();
    for (i = 0; i < 160; i++) {
		src[i] = a;
		a += 100;
    }
    gsm_encode(gsmh, src, dst);
    gsm_decode(gsmh, dst, dec);
    
    for (i = 0; i < 160; i++) {
    	if (dec[i] != correctResults[i]) {
    		if (wrong == 0) {
    			wrong = i + 1;
    		}
    	}
    }
	if (wrong) {
		printf("Wrong results starting at sample %d (%04X != %04X)!\n",
			wrong - 1, dec[wrong - 1], correctResults[wrong - 1]);
	}

#ifdef BENCHMARK
	printf("Running benchmark: please be patient.\n");    
    start = GetTickCount();
    for (i = 0; i < BENCHMARK; i++) {
    	gsm_encode(gsmh, src, dst);
	}
	e = (BENCHMARK * 160L * 1000) / (GetTickCount() - start);                
    start = GetTickCount();
    for (i = 0; i < BENCHMARK; i++) {
    	gsm_decode(gsmh, dst, dec);
	}
	d = (BENCHMARK * 160L * 1000) / (GetTickCount() - start);
	printf("Encode: %ld bytes per second.\n", e);                
	printf("Decode: %ld bytes per second.\n", d);                
    printf("\nPlease E-mail the line:\n\n    %d-%ld-%ld\n\nalong with a description of your machine\n", wrong, e, d);
    printf("(for example 486/50) to kelvin@fourmilab.ch.\n\nThank you!\n");
#else                
    gsm_decode(gsmh, dst, dec);
	for (i = 0; i < 33; i++) {
		printf("%3d%:  %3u\n", i, dst[i]);
	}
    for (i = 0; i < 160; i++) {
        printf("%3d%:  %6d  %6d\n", i, src[i], dec[i]);
    }
#endif
}
