#ifndef HW_H
#define HW_H
#include <stdlib.h>
#include <stdio.h>

#include "const.h"
#include "debug.h"
#include "info.h"



/**
 * @brief Validates command line arguments passed to the program.
 * @details This function will validate all the arguments passed to the program
 * and will return 1 byte (char) that will contain the information necessary for
 * the proper execution of the program.
 *
 * @param argc The number of arguments passed to the program from the CLI.
 * @param argv The argument strings passed to the program from the CLI.
 * @param in The OUTPUT_FILE of the program.
 * @param out The INPUT_FILE of the program.
 * @return Refer to homework document for the return value of this function.
 */
char validargs(int argc, char **argv, FILE **in, FILE **out);

void encryption(int n, int count,char **argv, FILE** in, FILE** out);

void decryption(int n, int count,char **argv, FILE** in, FILE** out);

int count_alphabet(char* arr);

int find_alphabet(char c, char* arr);

char alphabet_bits(int len, int n, char bits);

int char2int(char* str);

int open_files(FILE** in,FILE** out,char* indir, char* outdir);

void print_info(char* Alph, int n, char** argv, int encdec);

void print_encrypt(char current_char, char** argv, FILE** in, FILE** out, int n, int count);

void print_decrypt(char current_char, char** argv, FILE** in, FILE** out, int n, int count);

void tutnese_enc(char** argv, FILE** in, FILE** out);

int tutnese_dec(char** argv, FILE** in, FILE** out);

void print_syllable_enc(char** argv,char current_char,char revised_char, FILE** in, FILE** out);

int print_tutnese_enc(char current_char, char next_char, char** argv, FILE** in, FILE** out);

int print_tutnese_dec(char current_char, char** argv, FILE** in, FILE** out);
/*
 * @param mode The returned bits from the validargs function
 * @param option Specific bits to check whether an option is generated or not
 * @param n Position of the bit that needs to be checked
 * @return Return 0 or 1 according to the result
 */

int check_bit(char mode,char option,int n);

#define USAGE(retcode) do{                                                        \
fprintf(stderr, "%s\n",                                                           \
"usage: ./hw1 [-s | -t | -h]\n"                                                   \
"    -s       Substitution cipher\n"                                              \
"             Additional parameters: [-e | -d] n INPUT_FILE OUTPUT_FILE\n"        \
"                 -e           Encrypt using the substitution cipher.\n"          \
"                 -d           Decrypt using the substitution cipher.\n"          \
"                  n           The amount of position to shift by.\n"             \
"                  INPUT_FILE  This can be any file on the file system or '-'\n"  \
"                              which specifies stdin.\n"                          \
"                  OUTPUT_FILE This can be any file on the system or '-'\n"       \
"                              which specifies stdout.\n"                         \
"\n"                                                                              \
"    -t       Tutnese Translation\n"                                              \
"             Additional parameters: [-e | -d]   INPUT_FILE OUTPUT_FILE\n"        \
"                 -e          Encode into tutnese\n"                              \
"                 -d          Decode from tutnese\n"                              \
"                 INPUT_FILE  This can be any file on the file system or '-'\n"   \
"                             which specifies stdin\n"                            \
"                 OUTPUT_FILE This can be any file on the system or '-'\n"        \
"                             which specifies stdout.\n"                          \
"\n"                                                                              \
"    -h       Display this help menu.");                                          \
exit(retcode);                                                                    \
} while(0)

#endif
