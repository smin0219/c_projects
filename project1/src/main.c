#include "project1.h"

int main(int argc, char **argv) {

    FILE* in;
    FILE* out;

    char mode = 0;
    char len_alph = 0;
    int count = 0;

    /* Note: create a variable to assign the result of validargs */
   	mode = validargs(argc, argv, &in, &out);

   	/* INVALID CASES */
    if(mode == 0){
    	USAGE(EXIT_FAILURE);
    }
    /* -h ARGUMENT */
    if(check_bit(mode,0x80,0)==1){
        USAGE(EXIT_SUCCESS);
    }
    /* VALID ARGUMENTS */
    else{
        count = count_alphabet(Alphabet);

        /* DEREFERENCE LENGTH OF ALPHABET FROM MODE BYTE */
        len_alph = mode&0x1f;
        /* -s ARGUMENT */
        if(check_bit(mode,0x40,1)==1){
            /* -d ARGUMENT */
            if(check_bit(mode,0x20,2)==1){
                decryption(len_alph,count,argv,&in,&out);
                info(Alphabet,len_alph,count,*(argv+3),*(argv+4),1);
            }
            /* -e ARGUMENT */
            else{
                encryption(len_alph,count,argv,&in,&out);
                info(Alphabet,len_alph,count,*(argv+3),*(argv+4),0);
            }
            return EXIT_SUCCESS;
        }
        /* -t ARGUMENT */
        else if(check_bit(mode,0x40,1)==0){
            /* -d ARGUMENT */
            if(check_bit(mode,0x20,2)==1){
                tutnese_dec(argv,&in,&out);
            }
            /* -e ARGUMENT */
            else{
                tutnese_enc(argv,&in,&out);
            }
        }
        else{
    	   USAGE(EXIT_FAILURE);
        }
    }
}
