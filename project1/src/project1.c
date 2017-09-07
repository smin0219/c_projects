#include "project1.h"
// For your helper functions (you may add additional files also)
// DO NOT define a main function here

char validargs(int argc, char** argv, FILE** in, FILE** out) {
	char ret = 0;
	int len_alph = 0;
	int file_exist = 0;
 	int n = 320; /* DEFAULT VALUE FOR N */
	/* NO ARGUMENT */
	if(argc==1){
		ret = 0;
		return ret;
	}
	/* DISPLAY THE HELP MENU */
	if((*(*(argv+1)+1)) == 'h'){
		ret |= 0x80;
		return ret;
	}
	/* IF ARGUMENTS ARE TOO FEW OR TOO MANY */
	if((argc<5 || argc>6)&&(*(*(argv+1)+1)) == 's'){
		ret = 0;
		return ret;
	}
	if((argc<5 || argc>5)&&(*(*(argv+1)+1)) == 't'){
		ret = 0;
		return ret;
	}
	/* SUBSTITUTION CIPHER */
	else if((*(*(argv+1)+1)) == 's'){
		ret |= 0x40;
		/* IF USER WANTS TO DECODE */
		if((*(*(argv+2)+1)) == 'd'){
			ret |= 0x20;
			file_exist = open_files(in,out,*(argv+3),*(argv+4));
			/*IF FILE DOES NOT EXIST */
			if(file_exist == 1){
				return 0;
			}
		}
		/* IF USER WANTS TO ENCODE */
		else if((*(*(argv+2)+1)) == 'e'){
		/* KEEP THIRD MSB AS ZERO */
			file_exist = open_files(in,out,*(argv+3),*(argv+4));
			/* IF FILE DOES NOT EXIST */
			if(file_exist == 1){
				return 0;
			}
		}
		/* INVALID ARGUMENT */
		else{
			ret = 0;
			return ret;
		}
	}
	/* TUTNESE TRANSLATION */
	else if((*(*(argv+1)+1)) == 't'){
		/* IF USER WANTS TO DECODE */
		if((*(*(argv+2)+1)) == 'd'){
			ret |= 0x20;
			file_exist = open_files(in,out,*(argv+3),*(argv+4));
			if(file_exist == 1){
				return 0;
			}
		}
		/* IF USER WANTS TO ENCODE */
		else if((*(*(argv+2)+1)) == 'e'){
		/* KEEP THIRD MSB AS ZERO */
			file_exist = open_files(in,out,*(argv+3),*(argv+4));
			if(file_exist == 1){
				return 0;
			}
		}
		/* INVALID ARGUMENT */
		else{
			ret = 0;
			return ret;
		}
	}
	/* INVALID ARGUMENT */
	else{
		ret = 0;
		return ret;
	}
	/* COUNT NUMBER OF CHRARCTERS IN ALPHABET ARRAY */
	len_alph = count_alphabet(Alphabet);
	if(*(argv+5)!=NULL){
		n = char2int(*(argv+5));
	}
	/* SET 5 LEAST SIGNIFICANT BITS */
	ret = alphabet_bits(len_alph,n,ret);
	return ret;
}
/* ##################### PART 2 ##################### */
void encryption(int n, int count, char** argv, FILE** in, FILE** out){
	char current_char = 0;
	if(**(argv+3) == '-'){
		while((current_char=getchar())!=EOF){
			print_encrypt(current_char,argv,in,out,n,count);
		}
	}
	else{
		while(fread(&current_char,1,1,*in)!=0){
			print_encrypt(current_char,argv,in,out,n,count);
		}
	}
}
void decryption(int n, int count,char** argv, FILE** in,FILE** out){
	char current_char = 0;
	if(**(argv+3) == '-'){
		while((current_char=getchar())!=EOF){
			print_decrypt(current_char,argv,in,out,n,count);
		}
	}
	else{
		while(fread(&current_char,1,1,*in)!=0){
			print_decrypt(current_char,argv,in,out,n,count);
		}
	}
}
void tutnese_enc(char** argv, FILE** in, FILE** out){
	char current_char = 0;
	char next_char = 0;
	int isDouble = 0;
	if(**(argv+3)=='-'){
		current_char = getchar();
		while(current_char != EOF){
			next_char = getchar();
			isDouble = print_tutnese_enc(current_char,next_char,argv,in,out);
			if(isDouble == 1){
				next_char = getchar();
			}
			current_char = next_char;
		}
	}
	else{
		fread(&current_char,1,1,*in);
		while(current_char!=0){
			fread(&next_char,1,1,*in);
			if(feof(*in)){
				next_char = 0;
			}
			isDouble = print_tutnese_enc(current_char,next_char,argv,in,out);
			if(isDouble == 1){
				fread(&next_char,1,1,*in);
				if(feof(*in)){
					next_char = 0;
				}
			}
			current_char = next_char;
		}
	}
}
int tutnese_dec(char** argv, FILE** in, FILE** out){
	char current_char = 0;
	int pos = 0;
	if(**(argv+3)=='-'){
		current_char = getchar();
		while(current_char != EOF){
			pos = print_tutnese_dec(current_char,argv,in,out);
			if(pos<0){
				USAGE(EXIT_FAILURE);
			}
			while(pos!=0){
				current_char = getchar();
				pos--;
			}
		}
		return EXIT_SUCCESS;
	}
	else{
		fread(&current_char,1,1,*in);
		while(current_char!=0){
			if(feof(*in)){
				return EXIT_SUCCESS;
			}
			pos = print_tutnese_dec(current_char,argv,in,out);
			if(pos<0){
				USAGE(EXIT_FAILURE);
			}
			while(pos!=0){
				fread(&current_char,1,1,*in);
				pos--;
			}
		}
		return EXIT_SUCCESS;
	}
}
int print_tutnese_dec(char current_char, char** argv, FILE** in, FILE** out){
	int pos = 0;
	int c = 0;
	int x = 0;
	//int t = 0;
	int i = 0;
	//int z = 0;
	//int check = 0;
	int isLower = 0;
	int next = 1;
	char revised_char = 0;
	char revised_next = 0;
	char next_char = 0;
	char next_char_org = 0;
	char* squat = "squat";
	char* squa = "squa";
	if(current_char>64 && current_char<91){
		isLower = 1;
		revised_char = current_char+32;
	}
	else{
		revised_char = current_char;
	}
	if(revised_char=='a'||revised_char=='e'||revised_char=='i'||revised_char=='o'||revised_char=='u'){
		if(**(argv+4)=='-'){
			fprintf(stdout,"%c",current_char);
			c++;
		}
		else{
			fputc(current_char,*out);
			c++;
		}
	}
	else{
		while(*(Tutnese+pos)!=NULL){
			if((revised_char=='s') && (**(Tutnese+pos)==revised_char)){
				while(next<4){
					if(**(argv+3)=='-'){
						next_char = getchar();
					}
					else{
						fread(&next_char,1,1,*in);
					}
					if(((*(*(Tutnese+pos)+next))==next_char) && (next_char != *(squat+next))){
						if(*(*(Tutnese+pos)+next+1)==0){
							if(**(argv+4) == '-'){
								fprintf(stdout,"%c",current_char);
								return 1;
							}
							else{
								fputc(current_char,*out);
								return 1;
							}
						}
						else{
							next++;
						}
					}
					else if(((*(*(Tutnese+pos)+next))!=next_char) && (next_char == *(squa+next))){
						if(next == 3){
							if(**(argv+3)=='-'){
								next_char = getchar();
							}
							else{
								fread(&next_char,1,1,*in);
							}
							if(next_char=='t'){
								if(**(argv+3)=='-'){
									next_char = getchar();
								}
								else{
									fread(&next_char,1,1,*in);
								}
								if(**(argv+4) == '-'){
									if(isLower==1){
										next_char_org=next_char-32;
										fprintf(stdout,"%c",next_char_org);
									}
									else{
										fprintf(stdout,"%c",next_char);
									}
									fprintf(stdout,"%c",next_char);
									return 1;
								}
								else{
									if(isLower==1){
										next_char_org=next_char-32;
										fputc(next_char_org,*out);
									}
									else{
										fputc(next_char,*out);
									}
									fputc(next_char,*out);
									return 1;
								}
							}
							else{
								if(isLower == 1){
									if(next_char>64 && next_char<91){
										next_char_org=next_char;
									}
									else{
										next_char_org=next_char-32;
									}
									if(**(argv+4)=='-'){
										fprintf(stdout,"%c",next_char_org);
									}
									else{
										fputc(next_char_org,*out);
									}
								}
								else{
									if(next_char>64 && next_char<91){
										next_char_org=next_char+32;
										}
									else{
										next_char_org=next_char;
									}
									if(**(argv+4)=='-'){
										fprintf(stdout,"%c",next_char_org);
									}
									else{
										fputc(next_char_org,*out);
									}
								}
								if(**(argv+4)=='-'){
									fprintf(stdout,"%c",next_char);
								}
								else{
									fputc(next_char,*out);
								}
								if(next_char>64 && next_char<91){
									revised_next = next_char+32;
									}
								else{
									revised_next = next_char;
								}
								while(*(Tutnese+x)!=NULL){
									if(**(Tutnese+x) == revised_next){
										while(*(*(Tutnese+x)+i)!=0){
											i++;
										}
										return i;
									}
									else{
										x++;
									}
								}
							}
						}
						else{
							next++;
						}
					}
				}
			}
			else if((revised_char == **(Tutnese+pos)) && (revised_char != 's')){
				c= 1;
				while(*(*(Tutnese+pos)+c)!=0){
					if(**(argv+3)=='-'){
						next_char = getchar();
					}
					else{
						fread(&next_char,1,1,*in);
					}
					if(*(*(Tutnese+pos)+c)!=next_char){
						return -1;
					}
					c++;
				}
				if(**(argv+4)=='-'){
					fprintf(stdout,"%c",current_char);
				}
				else{
					fputc(current_char,*out);
				}
				return 1;
			}
			else{
				pos++;
			}
		}
		if(**(argv+4)=='-'){
			fprintf(stdout,"%c",current_char);
			c++;
		}
		else{
			fputc(current_char,*out);
			c++;
		}
	}
	return c;
}
/* ################ HELPER FUNCTIONS ################ */
int print_tutnese_enc(char current_char,char next_char, char** argv, FILE** in, FILE** out){
	int pos = 0;
	int end = 0;
	char revised_char = 0;
	char check_double = 0;
	char revised_double = 0;
	if(current_char>64 && current_char<91){
		revised_char = current_char+32;
	}
	else{
		revised_char = current_char;
	}
	if(revised_char=='a'||revised_char=='e'||revised_char=='i'||revised_char=='o'||revised_char=='u'){
		check_double = next_char;
		if(check_double>64 && check_double<91){
			revised_double = check_double+32;
		}
		else{
			revised_double = check_double;
		}
		if(revised_double == revised_char){
			if(**(argv+4)=='-'){
				if(current_char>64 && current_char<91){
					fprintf(stdout,"%s","Squat");
				}
				else{
					fprintf(stdout,"%s","squat");
				}
				fprintf(stdout,"%c",check_double);
			}
			else{
				if(current_char>64 && current_char<91){
					fputs("Squat",*out);
				}
				else{
					fputs("squat",*out);
				}
				fputc(check_double,*out);
			}
			return 1;
		}
		else{
			if(**(argv+4)=='-'){
				fprintf(stdout,"%c",current_char);
			}
			else{
				fputc(current_char,*out);
			}
			return 0;
		}
	}
	else{
		check_double = next_char;
		if(check_double>64 && check_double<91){
			revised_double = check_double+32;
		}
		else{
			revised_double = check_double;
		}
		if(revised_double == revised_char){
			while((*(Tutnese+pos)!=NULL) && (end == 0)){
				if(**(Tutnese+pos)==revised_char){
					if(current_char>64 && current_char<91){
						if(**(argv+4)=='-'){
							fprintf(stdout,"%s","Squa");
							end = 1;
						}
						else{
							fputs("Squa",*out);
							end = 1;
						}
					}
					else{
						if(**(argv+4)=='-'){
							fprintf(stdout,"%s","squa");
							end = 1;
						}
						else{
							fputs("squa",*out);
							end = 1;
						}
					}
				}
				else{
					pos++;
				}
			}
			print_syllable_enc(argv,next_char,revised_double,in,out);
			if(end == 1){
				return 1;
			}
			else{
				return 0;
			}
		}
		else{
			print_syllable_enc(argv,current_char,revised_char,in,out);
			return 0;
		}
	}
}
void print_syllable_enc(char** argv,char current_char,char revised_char,FILE** in, FILE** out){
	int pos = 0;
	int c = 1;
	int end = 0;
	while((*(Tutnese+pos)!=NULL) && (end == 0)){
		if(**(Tutnese+pos)==revised_char){
			if(**(argv+4)=='-'){
				if(current_char>64 && current_char<91){
					fprintf(stdout,"%c",current_char);
					while((*(*(Tutnese+pos)+c))!=0){
						fprintf(stdout,"%c",(*(*(Tutnese+pos)+c)));
						c++;
					}
				}
				else{
					fprintf(stdout,"%s",*(Tutnese+pos));
				}
				end = 1;
			}
			else{
				if(current_char>64 && current_char<91){
					fputc(current_char,*out);
					while((*(*(Tutnese+pos)+c))!=0){
						fputc((*(*(Tutnese+pos)+c)),*out);
						c++;
					}
				}
				else{
					fputs(*(Tutnese+pos),*out);
				}
				end = 1;
			}
		}
		else{
			pos++;
		}
	}
	if(end == 0){
		if(**(argv+4)=='-'){
			fprintf(stdout,"%c",current_char);
		}
		else{
			fputc(current_char,*out);
		}
	}
}
void print_encrypt(char current_char, char** argv, FILE** in, FILE** out, int n, int count){
	int pos = 0;
	if(current_char>96 && current_char<123){
		current_char-=32;
	}
	pos = find_alphabet(current_char,Alphabet);
	if(pos == -1){
		if(**(argv+4) == '-'){
			fprintf(stdout,"%c",current_char);
		}
		else{
			fwrite(&current_char,1,1,*out);
		}
	}
	else if(pos+n>=count){
		if(**(argv+4) == '-'){
			fprintf(stdout,"%c",*(Alphabet+(pos+n-count)));
		}
		else{
			fwrite((Alphabet+(pos+n-count)),1,1,*out);
		}
	}
	else{
		if(**(argv+4) == '-'){
			fprintf(stdout,"%c",*(Alphabet+(pos+n)));
		}
		else{
			fwrite((Alphabet+(pos+n)),1,1,*out);
		}
	}
}
void print_decrypt(char current_char, char** argv, FILE** in, FILE** out, int n, int count){
	int pos = 0;
	if(current_char>96 && current_char<123){
		current_char-=32;
	}
	pos = find_alphabet(current_char,Alphabet);
	if(pos == -1){
		if(**(argv+4) == '-'){
			fprintf(stdout,"%c",current_char);
		}
		else{
			fwrite(&current_char,1,1,*out);
		}
	}
	else if(pos-n<0){
		if(**(argv+4) == '-'){
			fprintf(stdout,"%c",*(Alphabet+(pos-n+count)));
		}
		else{
			fwrite((Alphabet+(pos-n+count)),1,1,*out);
		}
	}
	else{
		if(**(argv+4) == '-'){
			fprintf(stdout,"%c",*(Alphabet+(pos-n)));
		}
		else{
			fwrite((Alphabet+(pos-n)),1,1,*out);
		}
	}
}
int find_alphabet(char c, char* arr){
	int count = 0;
	char current = 0;
	while((current = *(Alphabet+count)) != 0){
		if(current == c){
			return count;
		}
		count++;
	}
	return -1;
}
/* COVENRT CHAR TO INT */
int char2int(char* str){
	int count = 0;
	int result = 0;
	int value = 0;
	int i = 0;
	int pow = 1;
	while(*(str+count)!=0){
		count++;
	}
	while(count!=0){
		pow = 1;
		for(int j=1; j<count; j++){
			pow *= 10;
		}
		value = (*(str+i)-'0')*pow;
		result += value;
		i++;
		count--;
	}
	return result;
}
/* COUNT NUMBER OF CHARACTERS IN ALPHABET ARRAY */
int count_alphabet(char* arr){
	int count = 0;
	int i = 0;
	while(i<32){
		if(*(arr+i)!=0){
			count++;
		}
		i++;
	}
	return count;
}
/* SET 5 LEAST SIGNIFICANT BITS */
char alphabet_bits(int len, int n, char bits){
	/* n%LENGTH_OF_ALPHABET */
	int result = n%len;
	int mod = 0;
	char bit_pos = 0;
	/* CHANGE LENGTH OF ALPHABET(INT) TO BITS */
	while(result != 0){
		mod = result%2;
		if(mod == 1){
			if(bit_pos ==0){
				bit_pos = 1;
			}
			bits|=bit_pos;
			result = result/2;
		}
		else{
			if(bit_pos ==0){
				bit_pos = 1;
			}
			result = result/2;
		}
		bit_pos = 2*bit_pos;
	}
	return bits;
}
int open_files(FILE** in, FILE** out, char* indir, char* outdir){
	if(*indir != '-'){
		*in = fopen(indir,"r");
		/* IF INPUT_FILE DOESN'T EXIST OR CAN'T BE OPENED */
		if(*in == NULL){
			return EXIT_FAILURE;
		}
	}
	if(*outdir != '-'){
		*out = fopen(outdir,"w");
	}
	return EXIT_SUCCESS;
}
int check_bit(char mode,char option,int n){
    char result = 0;
    if(n==0){
    	result = mode&option;
    	return -result>>7;
    }
    else{
    	result = mode&option;
    	return (result << n)>>7;
    }
}
