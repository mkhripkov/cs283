#include <stdio.h>
#include <string.h>
#include <stdlib.h>


#define BUFFER_SZ 50

//prototypes
void usage(char *);
void print_buff(char *, int);
int  setup_buff(char *, char *, int);

//prototypes for functions to handle required functionality
int  count_words(char *, int, int);
char* reverseString(char*, int, int);
void wordPrint(char*, int, int);

int setup_buff(char *buff, char *user_str, int len){
    int stringLength = 0;
    int whitespaceCounter = 0;
    
    while(*user_str != '\0') {
        if (stringLength >= len - 1) {
            return -1;
        }

        if (*user_str == '\t' || *user_str == ' ') {
            if (!whitespaceCounter && stringLength > 0) {
                *(buff + stringLength) = ' ';
                stringLength++;
                whitespaceCounter = 1;
            }
        } else {
            *(buff + stringLength) = *user_str;
            stringLength++;
            whitespaceCounter = 0;
        }
        user_str++;
    }

    for (int i = stringLength; i < len - 1; i++) {
        *(buff + i) = '.';
    }

    *(buff + len - 1) = '\0';

    return stringLength; 
}

void print_buff(char *buff, int len){
    printf("Buffer:  ");
    for (int i=0; i<len; i++){
        putchar(*(buff+i));
    }
    putchar('\n');
}

void usage(char *exename){
    printf("usage: %s [-h|c|r|w|x] \"string\" [other args]\n", exename);

}

int count_words(char *buff, int len, int str_len){
    int wordCounter = 0;
    int whitespaceFlag = 1;
    for (int i = 0; i < str_len; i++) {
        char currentBuffChar = *(buff + (sizeof(char)*i));
        if (currentBuffChar != '.' && currentBuffChar != ' ' && currentBuffChar != '\t') {
            if (whitespaceFlag) { // we have not yet encountered a whitespace
                wordCounter++;
                whitespaceFlag = 0;
            } 
        } else { // we encountered a whitespace because if statement evaluated to false
            whitespaceFlag = 1;
        }
    }
    return wordCounter;
}

char* reverseString(char *buff, int len, int str_len) {
    char *start = buff;
    char *end = buff + str_len - 1;
    char temp;

    while (start < end) {
        temp = *start;
        *start = *end;
        *end = temp;

        start++;
        end--;
    }
    return buff;
}

void wordPrint(char* buff, int len, int str_len) {
    printf("Word Print\n");
    printf("----------\n");

    int wordIndex = 0;
    int i = 0;

    while (i < str_len) {
        if (*(buff + i) == ' ' || *(buff + i) == '\t') {
            i++;
            continue;
        }

        int start = i;
        while (i < str_len && *(buff + i) != ' ' && *(buff + i) != '\t') {
            i++;
        }

        int wordLength = i - start;
        printf("%d. ", ++wordIndex);

        for (int j = start; j < i; j++) {
            printf("%c", *(buff + j));
        }

        printf(" (%d)\n", wordLength);
    }
}

int main(int argc, char *argv[]){

    char *buff;             //placehoder for the internal buffer
    char *input_string;     //holds the string provided by the user on cmd line
    char opt;               //used to capture user option from cmd line
    int  rc;                //used for return codes
    int  user_str_len;      //length of user supplied string

    //TODO:  #1. WHY IS THIS SAFE, aka what if arv[1] does not exist?
    /*
    This is safe because we check to make sure that there are the correct amount of arguments.
    The first condition of the if statement checks that there are at least two arguments, this means that
    we can check arv[1] without worrying about erroring out. 
    */
    if ((argc < 2) || (*argv[1] != '-')){
        usage(argv[0]);
        exit(1);
    }

    opt = (char)*(argv[1]+1);   //get the option flag
 
    //handle the help flag and then exit normally
    if (opt == 'h'){
        usage(argv[0]);
        exit(0);
    }

    //WE NOW WILL HANDLE THE REQUIRED OPERATIONS

    //TODO:  #2 Document the purpose of the if statement below
    /*
    The purpose of this if statement is to make sure we have the appropriate amount of arguments.
    Without this if statement, if the user only eneters the flag without the string they want to process
    then arv[2] wont exist and the program could error out.
    */
    if (argc < 3){
        usage(argv[0]);
        exit(1);
    }

    input_string = argv[2]; //capture the user input string

    buff = malloc(BUFFER_SZ * sizeof(char));

    if (buff == NULL) {
        exit(99);
    }

    user_str_len = setup_buff(buff, input_string, BUFFER_SZ);    
    if (user_str_len < 0){
        printf("Error setting up buffer, error = %d", user_str_len);
        exit(2);
    }

    switch (opt){
        case 'c':
            rc = count_words(buff, BUFFER_SZ, user_str_len);
            if (rc < 0){
                printf("Error counting words, rc = %d", rc);
                exit(2);
            }
            printf("Word Count: %d\n", rc);
            break;
        case 'r':
            reverseString(buff, BUFFER_SZ, user_str_len);
            printf("Reversed String: ");
            int bufferIndex = 0;
            while (bufferIndex < user_str_len) {
                printf("%c", *(buff + (sizeof(char) * bufferIndex)));
                bufferIndex++;
            }
            printf("\n");
            break;
        case 'w':
            wordPrint(buff, BUFFER_SZ, user_str_len);
            break;
        case 'x':
            if (argc != 5) {
                usage(argv[0]);
                exit(1);
            }
            printf("Not Implemented!\n");
            break;
        default:
            usage(argv[0]);
            exit(1);
    }

    print_buff(buff,BUFFER_SZ);
    free(buff);
    exit(0);
}

//TODO:  #7  Notice all of the helper functions provided in the 
//          starter take both the buffer as well as the length.  Why
//          do you think providing both the pointer and the length
//          is a good practice, after all we know from main() that 
//          the buff variable will have exactly 50 bytes?
//  
//          Providing both the pointer and the length is good practice because
//          it improves both the flexibility and the safety of the program as a whole.
//          It improves the flexibility because it allows the functions to handle different
//          buffer sizes rather than assuming the given fixed length. It improves safety because
//          it prevents buffer overflows.