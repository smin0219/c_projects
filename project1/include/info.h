#ifndef INFO_H
#define INFO_H
#include <stdlib.h>
#include <stdio.h>
#ifdef INFO

#define info(Alphabet,num,count,in,out,oper)\
	do{\
		int origin = 0;\
		int zero = 0;\
		int sum = 0;\
		int shift = 0;\
		shift = num;\
		fprintf(stderr, "INFO: shifed alphabet: ");\
		if(oper==0){\
			origin = num;\
			while(num<count){\
				fprintf(stderr, "%c", *(Alphabet+num));\
				num++;\
			}\
			while(zero<origin){\
				fprintf(stderr, "%c", *(Alphabet+zero));\
				zero++;\
			}\
		}\
		if(oper==1){\
			origin = count-num;\
			sum = count-num;\
			while(sum<count){\
				fprintf(stderr, "%c", *(Alphabet+sum));\
				sum++;\
			}\
			while(zero<origin){\
				fprintf(stderr, "%c", *(Alphabet+zero));\
				zero++;\
			}\
		}\
		fprintf(stderr,"\n");\
		fprintf(stderr, "INFO: shift amount: %d\n",shift);\
		if(*in=='-'){\
			fprintf(stderr, "INFO: input file: %s\n","STDIN");\
		}\
		else{\
			fprintf(stderr, "INFO: input file: %s\n",in);\
		}\
		if(*out=='-'){\
			fprintf(stderr, "INFO: output file: %s\n","STDOUT");\
		}\
		else{\
			fprintf(stderr, "INFO: output file: %s\n",out);\
		}\
		if(oper == 0){\
			fprintf(stderr, "INFO: operation: %s\n","encryption");\
		}\
		else{\
			fprintf(stderr, "INFO: operation: %s\n","decryption");\
		}\
	}while(0);\

#else
	#define info(Alphabet,num,count,in,out,oper)

#endif

#endif /* INFO_H */
