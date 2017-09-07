#include "hw2.h"
/* Great filename. */
void processDictionary(FILE* f){

    struct dict_word* currWord;


    int counter = 0;
    int firstWord = 1;
    char word[MAX_SIZE];
    char line[MAX_SIZE];
    char* wdPtr = word;
    char* character = NULL;

    //dict->num_words = 0;
    while(fgets(line,MAX_SIZE+1,f)!=NULL){

        if((currWord = (struct dict_word*) malloc(sizeof(struct dict_word))) == NULL){
            return;
        }
        currWord->num_misspellings = 0;
        currWord->misspelled_count = 0;

        if((line[strlen(line)-2] != ' ' && line[strlen(line)-1] == '\n') || (line[strlen(line)-1] != ' ' && line[strlen(line)-1] != '\n'))
            strcat(line, " ");
        character = line;
        while(*character != 0){
            if(counter >= MAX_MISSPELLED_WORDS+1)
                break;
            if(*character == ' '){
                *wdPtr = 0;
                wdPtr = word;
                if(firstWord){
                    addWord(currWord,wdPtr);
                    dict->num_words++;
                    firstWord = 0;
                }
                else{
                    struct misspelled_word* currMisspelling;
                    if((currMisspelling = malloc(sizeof(struct misspelled_word))) == NULL){
                        return;
                    }
                    addMisspelledWord(currMisspelling, currWord, wdPtr);
                }
            }
            else if(*character != '\n'){
                *(wdPtr++) = *character;
            }
            character++;
        }

        firstWord = 1;
    }
}

// LINK NEW WORD WITH PREVIOUS WORD LINK
void addWord(struct dict_word* dWord, char* word){
    //struct dict_word* ptr;
    //setting up dWord fields
    dWord->misspelled_count = 0;
    dWord->num_misspellings = 0;
    dWord->next = dict->word_list;
    strcpy(dWord->word,word);
    dict->word_list = dWord;
}
void addMisspelledWord(struct misspelled_word* misspelledWord, struct dict_word* correctWord, char* word){
    //setting up misspelledWord fields
    strcpy(misspelledWord->word, word);
    misspelledWord->misspelled = 0;
    misspelledWord->correct_word = correctWord;
    misspelledWord->next = m_list;
    (correctWord->misspelled)[correctWord->num_misspellings++] = misspelledWord;
    m_list = misspelledWord;
}
void freeWords(struct dict_word* currWord){
    int t = dict->num_words;
    int count = 0;

    while(t!=count){
        while(t-1!=count){
            currWord = currWord->next;
            t--;
        }

        free(currWord);

        currWord = dict->word_list;
        t = dict->num_words;
        count++;
    }

}

void freeMiss(struct misspelled_word* miss){
    int temp = 0;
    int i = 0;
    int count = 0;

    while(miss != NULL){
        miss=miss->next;
        i++;
    }
    temp = i;
    miss= m_list;
    while(i!=count){
        while(i-1!=count){
            miss = miss->next;
            i--;
        }
        free(miss);
        miss = m_list;
        i = temp;
        count++;
    }
}

void printStats(struct dict_word* head){
    //struct dict_word* currPtr;

    int total_words = 0;
    int size_dict  = 0;
    int size_misspelled = 0;
    int total_misspelled = 0;

    if(head){}
    //first->misspelled_count = 0;
    //second->misspelled_count = 0;
    //third->misspelled_count = 0;
    //int second_count = 0;
    //int thrid_count = 0;
    //char* second = NULL;
    //char* third = NULL;
    total_words = dict->num_words;
    size_dict = sizeof(struct dictionary) + sizeof(struct dict_word)*(dict->num_words);


    /* size of misspelled */

    struct misspelled_word* currPtr;
    int count = 0;

    currPtr = m_list;
    while(currPtr!=NULL){
        count++;
        currPtr = currPtr->next;
    }

    size_misspelled = sizeof(struct misspelled_word)*count;

    /* total misspelled */

    struct dict_word* isMiss;
    count = 0;

    isMiss = dict->word_list;
    while(isMiss!=NULL){
        if((isMiss->misspelled_count)>0){
            count++;
        }
        isMiss = isMiss->next;
    }

    total_misspelled = count;

    struct dict_word* temp = NULL;
    struct dict_word* orig = NULL;
    struct dict_word* first = NULL;
    struct dict_word* second = NULL;
    struct dict_word* third = NULL;
    struct dict_word* top = NULL;
    int taken = 0;
    int changed = 0;

    top = head;

    while(top != NULL){
        orig = top;
        if(first == NULL){
            first = top;
            taken = 1;
        }
        else{
            if(first != NULL && (top->misspelled_count > first->misspelled_count)){
                temp = first;
                first = top;
                top = temp;
                changed = 1;

                //top = top->next;
            }
        }
        if(second == NULL && taken !=1){
            second = top;
            taken = 1;

        }
        else{
            if(second != NULL&&(top->misspelled_count > second->misspelled_count) && taken!=1 ){
                temp = second;
                second = top;
                top = temp;
                changed = 1;

            }//top = top->next;
        }
        if(third == NULL && taken != 1){
            third = top;
            taken = 1;
        }
        else{
            if(third != NULL&&(top->misspelled_count > third->misspelled_count) && taken!=1){
                temp = third;
                third = top;
                top= temp;
                changed = 1;

            }
        }
        if(changed == 1 && orig != NULL){
            top = orig;
        }
        temp = NULL;
        orig = NULL;
        top = top->next;
        taken = 0;
        changed= 0;
    }

    int isFirst = 0;
    int num = 0;

    fprintf(stderr,"Total number of words in dictionary: %d\n",total_words);
    fprintf(stderr,"Size of dictionary (in bytes): %d\n",size_dict);
    fprintf(stderr,"Size of misspelled word list (in bytes): %d\n",size_misspelled);
    fprintf(stderr,"Total number of misspelled words: %d\n",total_misspelled);
    fprintf(stderr,"Top 3 misspelled words:\n");


    fprintf(stderr,"%s (%d times): ",first->word,first->misspelled_count);
    num = (first->num_misspellings)-1;
    //while((*(first->misspelled))!=NULL){
    while(num>-1){
        if((first->misspelled[num]->misspelled) == true){
            if(isFirst == 0){
                fprintf(stderr,"%s",first->misspelled[num]->word);
                isFirst = 1;
            }
            else{
                fprintf(stderr,", %s",first->misspelled[num]->word);
            }
            num--;
        }
        else{
            num--;
        }
    }
    fprintf(stderr,"%c",'\n');


    isFirst = 0;
    num = (second->num_misspellings)-1;

    fprintf(stderr,"%s (%d times): ",second->word,second->misspelled_count);
    while(num>-1){
        if(second->misspelled[num]->misspelled == true){
            if(isFirst == 0){
                fprintf(stderr,"%s",second->misspelled[num]->word);
                isFirst = 1;
            }
            else{
                fprintf(stderr,", %s",second->misspelled[num]->word);
            }
            num--;
        }
        else{
            num--;
        }
    }
    fprintf(stderr,"%c",'\n');
;
    isFirst = 0;
    num = (third->num_misspellings)-1;
    fprintf(stderr,"%s (%d times): ",third->word,third->misspelled_count);
    while(num>-1){
        if(third->misspelled[num]->misspelled == true){
            if(isFirst == 0){
                fprintf(stderr,"%s\n",third->misspelled[num]->word);
                isFirst = 1;
            }
            else{
                fprintf(stderr,", %s",third->misspelled[num]->word);
            }
            num--;
        }
        else{
            num--;
        }
    }
    fprintf(stderr,"%c",'\n');
}

int processWord(char* inputWord,char newname[MAX_SIZE], FILE* dictionary, bool a, int num, int firstOpen){
    FILE* newfile = NULL;
    int num_cp = num;
    char ch = 0;
    char temp[MAX_SIZE];
    //char newname[MAX_SIZE] = "rsrc/new_";
    char** misspelled_arr = NULL;
    if(foundMisspelledMatch(inputWord))
        return firstOpen;
    if(foundDictMatch(inputWord))
        return firstOpen;

    memset(temp,0,sizeof(temp));


    if((a==true) && (strcmp(inputWord,"")!=0)){
        if(num<0 || num>5){
            USAGE(EXIT_FAILURE);
        }

        if(firstOpen == 1){
            newfile=fopen(newname,"w");
            while(1){
                ch = fgetc(dictionary);
                if(ch == EOF){
                    break;
                }
                else{
                    putc(ch,newfile);
                }
            }
            fclose(newfile);
            newfile = fopen(newname,"a");
            firstOpen = 0;
        }
        else{
            newfile = fopen(newname,"a");
        }
        struct dict_word* newWord;

         if((newWord = (struct dict_word*) malloc(sizeof(struct dict_word))) == NULL)
        {
            USAGE(EXIT_FAILURE);
        }

        //strcat(newname,dicfile);
        //int counter = 0;
        addWord(newWord, inputWord);
        dict->num_words++;
        fprintf(newfile,"%s",inputWord);
        misspelled_arr = gentypos(num,inputWord);
        //memmove(temp,*misspelled_arr,sizeof(temp));

        num -= 1;
        while(num>=0){
            struct misspelled_word* currMisspelling;
            if((currMisspelling = malloc(sizeof(struct misspelled_word))) == NULL)
            {
                //printf("ERROR: OUT OF MEMORY.");
                free(newWord);
                USAGE(EXIT_FAILURE);
            }
            addMisspelledWord(currMisspelling,newWord,*(misspelled_arr+num));
            fprintf(newfile," %s",*(misspelled_arr+num));
            num--;
        }

        num_cp-=1;
        while(num_cp >= 0){
            free(*(misspelled_arr+num_cp));
            num_cp--;
        }
        free(misspelled_arr);
        fprintf(newfile,"%c",'\n');
        rewind(dictionary);
        fclose(newfile);
    }
    return firstOpen;

}
bool foundMisspelledMatch(char* inputWord){
    struct misspelled_word* listPtr = m_list;
    while(listPtr != NULL)
    {
        if(strcasecmp(inputWord, listPtr->word) == 0)
        {
            strcpy(inputWord, listPtr->correct_word->word);
            listPtr->misspelled = 1;
            listPtr->correct_word->misspelled_count++;
            return true;
        }
        listPtr = listPtr->next;
    }
    return false;
}
bool foundDictMatch(char* inputWord){
    struct dict_word* listPtr = dict->word_list;
    while(listPtr != NULL)
    {
        if(strcasecmp(inputWord, listPtr->word) == 0)
        {
            return true;
        }
        listPtr = listPtr->next;
    }
    return false;
}
