#include "hw2.h"
int main(int argc, char *argv[]){
    FILE* DEFAULT_INPUT = stdin;
    FILE* DEFAULT_OUTPUT = stdout;
    char DEFAULT_DICT_FILE[]= "rsrc/dictionary.txt";
    // Variables for -An argument
    int firstOpen = 1;
    int num = 0;
    int count = 0;
    int i = 0;
    bool a = false;

    //create dictionary
    if((dict = (struct dictionary*) malloc(sizeof(struct dictionary))) == NULL)
    {
        //printf("ERROR: OUT OF MEMORY.\n");
        return EXIT_FAILURE;
    }
    //if((m_list = (struct misspelled_word*) malloc(sizeof(struct misspelled_word*))) == NULL)
    //{
        //printf("ERROR: OUT OF MEMORY.\n");
    //    return EXIT_FAILURE;
    //}
    m_list = NULL;

    dict->num_words = 0;
    dict->word_list = NULL;

    struct Args args;
    // Set struct default values
    args.d = false;
    args.i = false;
    args.o = false;
    memmove(args.dictFile, DEFAULT_DICT_FILE,sizeof(args.dictFile));
    // Make a loop index
    char line[MAX_SIZE];
    //Declare Files
    FILE* dFile = NULL;
    FILE* iFile = DEFAULT_INPUT;
    FILE* oFile = DEFAULT_OUTPUT;
    char sp_char1 = 0;
    char sp_char2 = 0;
    char opt = '\0';
    const char s[2] = "/";
    char* token = "";
    char* token_next = "";
    char* currArg = NULL;
    char directory[MAX_SIZE];
    char dicfile[MAX_SIZE];
    char filename[MAX_SIZE];

    memset(directory,0,MAX_SIZE);
    memset(dicfile,0,MAX_SIZE);
    memset(filename,0,MAX_SIZE);

    /* -h OPTION DETECTER */
    for(i=1; i<argc; i++){
        currArg = argv[i];
        if((strcmp(currArg,"-h"))==0){
            free(dict);
            free(m_list);
            USAGE(EXIT_SUCCESS);
        }
    }
    /* OTHER ARGUMENTS */
    while((opt=getopt(argc,argv,"o:i:d:A:"))!=-1){
        /* CURRENT ARGUEMENT */
        currArg = optarg;
        if(*optarg=='-'){
            USAGE(EXIT_FAILURE);
        }
        switch(opt){
            case 'o':
                memmove(args.output, currArg,strlen(currArg));
                args.o = true;
                oFile = fopen(currArg, "w");
                break;
            case 'i':
                memmove(args.input, currArg,strlen(currArg));
                args.i = true;
                iFile = fopen(currArg, "r");
                break;
            case 'd':
                memset(args.dictFile,0,MAX_SIZE);
                memmove(args.dictFile, currArg,strlen(currArg));
                args.d = true;
                memmove(dicfile,currArg,strlen(currArg));
                break;
            case 'A':
                a = true;
                num = *currArg-48;
                break;
            case ':':
                USAGE(EXIT_FAILURE);
                break;
            default:
                USAGE(EXIT_FAILURE);
                break;
        }
    }
    if(args.d!=true){
        memmove(dicfile,"rsrc/dictionary.txt",19);
    }
    token = strtok(dicfile,s);
    token_next = strtok(NULL,s);
    if(token_next != NULL){
        strcat(directory,token);
        strcat(directory,"/");
    }
    while(token!=NULL){
        //memmove(dicfile,token,strlen(token));
        //memset(token,0,strlen(token));
        if(token_next == NULL){
            memmove(filename,token,strlen(token));
        }
        token = token_next;
        token_next = strtok(NULL,s);
        if(token_next != NULL){
            strcat(directory,token);
            strcat(directory,"/");
        }
    }
    dFile = fopen(args.dictFile, "r");
    if(dFile == NULL)
    {
        if(args.o==true && oFile != NULL){
            fclose(oFile);
        }
        if(args.i==true && iFile != NULL){
            fclose(iFile);
        }
        free(dict);
        free(m_list);
        USAGE(EXIT_FAILURE);
    }
    if(iFile == NULL && args.i == true)
    {
        if(args.o==true && oFile !=NULL){
            fclose(oFile);
        }
        fclose(dFile);
        free(dict);
        free(m_list);
        USAGE(EXIT_FAILURE);
    }

    else
    {
        processDictionary(dFile);
        rewind(dFile);
    }
    while((fgets(line, MAX_SIZE+1, iFile)!=NULL))
    {
        //char ch = 0;
        int t = 0;
        int z = 0;
        int length = 0;
        int n = 0;
        //int isWord = 0;
        char word[MAX_SIZE];
        char newWord[MAX_SIZE];
        char* wdPtr = word;
        //char line[MAX_SIZE];
        char* character = line;
        char newname[MAX_SIZE];
        char temp[MAX_SIZE];
        char temp2[MAX_SIZE];
        //strcat(newname,dicfile);
        //bFILE* newfile = NULL;
        memset(newname,0,MAX_SIZE);
        memset(newWord,0,MAX_SIZE);
        memset(temp,0,MAX_SIZE);
        memset(temp2,0,MAX_SIZE);
        memset(word,0,MAX_SIZE);
        memmove(newname,directory,strlen(directory));
        strcat(newname,"new_");
        strcat(newname,filename);
        //fgets(line, MAX_SIZE+1, iFile);
        //if there isn't a space or newline at the end of the line, put one there
       // if((line[strlen(line)-1] != ' ') && (line[strlen(line)-1] != '\n'))
       //     strcat(line, " ");
        //replaces spaces within a line with new lines
        //while(*character != NULL)
        while(*character != 0)
        {
            *character = tolower(*character);
            if(*character == ' ' || *character == '\n')
            {
                wdPtr = word;
                length = strlen(word)-1;
                while(*(wdPtr+n)!=0){
                    if((*wdPtr<97 || *wdPtr>122) && (n==0)){
                        sp_char1 = *wdPtr;
                        n++;
                    }
                    else if((*(wdPtr+n)<97 || *(wdPtr+n)>122) && (n==1) && (n!=length)){
                    	temp2[0] = sp_char1;
                    	sp_char1 = 0;
                    	while((*(wdPtr+n)!=0)&&(*(wdPtr+n)<97 || *(wdPtr+n)>122)){
                    		temp2[t+1] = *(wdPtr+n);
                    		t++;
                    		n++;
                    	}
                    	if(*(wdPtr+n)!=0){
                            t=0;
                        }
                    }
                    else if((*(wdPtr+n)<97 || *(wdPtr+n)>122) && (n!=length)){
                        //sp_char2 = *(wdPtr+n);
                        while((*(wdPtr+n)!=0)&&(*(wdPtr+n)<97 || *(wdPtr+n)>122)){
                            temp[t] = *(wdPtr+n);
                            t++;
                            n++;
                        }
                        if(*(wdPtr+n)!=0){
                            strcat(newWord,temp);
                            memset(temp,0,sizeof(temp));
                            z+=t;
                            t=0;
                        }
                    }
                    else if((*(wdPtr+n)<97 || *(wdPtr+n)>122) && (n==length)){
                        sp_char2 = *(wdPtr+n);
                        n++;
                    }
                    else{
                        newWord[z]=*(wdPtr+n);
                        z++;
                        n++;
                    }
                }
                if(args.o==true){
                    firstOpen = processWord(newWord,newname,dFile,a,num,firstOpen);
                }
                else{
                    if(strcmp(wdPtr,"")!=0){
                        count++;
                    }
                }
                if(args.o==true){
                	strcat(temp2,newWord);
                    strcat(temp2,temp);
                    if(sp_char1!=0){
                        fprintf(oFile,"%c",sp_char1);
                    }
                    fprintf(oFile,"%s",temp2);
                    if(sp_char2!=0){
                        fprintf(oFile,"%c",sp_char2);
                    }
                    fprintf(oFile,"%c",*character);
                }
                memset(word,0,MAX_SIZE);
                memset(newWord,0,MAX_SIZE);
                memset(temp2,0,sizeof(temp2));
                memset(temp,0,sizeof(temp));
                sp_char1 = 0;
                sp_char2 = 0;
                z = 0;
                n = 0;
            }
            else
            {
                *(wdPtr++) = *character;
            }
            character++;
        }
        /* SINCE THERE IS NO SPACE OR NEWLINE AT THE END OF FILE*/
        if(*character == 0){
            wdPtr = word;
            if(strcmp(wdPtr,"")!=0){
               length = strlen(word)-1;
                while(*(wdPtr+n)!=0){
                    if((*wdPtr<97 || *wdPtr>122) && (n==0)){
                        sp_char1 = *wdPtr;
                        n++;
                    }
                    else if((*(wdPtr+n)<97 || *(wdPtr+n)>122) && (n!=length)){
                        //sp_char2 = *(wdPtr+n);
                        while((*(wdPtr+n)!=0)&&(*(wdPtr+n)<97 || *(wdPtr+n)>122)){
                            temp[t] = *(wdPtr+n);
                            t++;
                            n++;
                        }
                        if(*(wdPtr+n)!=0){
                            strcat(newWord,temp);
                            memset(temp,0,sizeof(temp));
                            z+=t;
                            t=0;
                        }
                    }
                    else if((*(wdPtr+n)<97 || *(wdPtr+n)>122) && (n==length)){
                        sp_char2 = *(wdPtr+n);
                        n++;
                    }
                    else{
                        newWord[z]=*(wdPtr+n);
                        z++;
                        n++;
                    }
                }

                //strcpy(word,newWord);
                if(args.o==true){
                    firstOpen = processWord(newWord,newname,dFile,a,num,firstOpen);
                }
                else{
                    if(strcmp(wdPtr,"")!=0){
                        count++;
                    }
                }
            }
            if(args.o==true){
                strcat(newWord,temp);
                if(sp_char1!=0){
                    fprintf(oFile,"%c",sp_char1);
                }
                fprintf(oFile,"%s",newWord);
                if(sp_char2!=0){
                    fprintf(oFile,"%c",sp_char2);
                }
            }
        }
        if(iFile == stdin)
           break;
    }

    printStats(dict->word_list);
    if(args.o==true){
        fclose(oFile);
    }
    else{
        fprintf(stdout,"%d\n",count);
    }
    if(args.i==true){
        fclose(iFile);
    }

    freeWords(dict->word_list);
    freeMiss(m_list);
    free(dict);
    fclose(dFile);
    return EXIT_SUCCESS;
}
