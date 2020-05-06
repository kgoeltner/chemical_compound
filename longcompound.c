/* 
 * LONGCOMPOUND: A program that calculates the atomic weight of a compound & prints unique elements
 *  - Loads in elements from a text file of symbols & atomic weights
 *  - Parse compound into symbols & quantities
 *  - Calculate the atomic weight based on element structures
 *  - Prints out the unique element list in alphabetical order
 * 
 * Karl Goeltner, ECS 36A
 * December 5, 2019 - Created program to read in file, parse compound, and calculate atomic weight
 */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

/* Useful macros */
#define MAXELEMENTS  300    /* Maximum number of elements */
#define BUFFERSIZE  1024    /* Maximum buffer size for chemical compound */

/* Structure that holds element weight and symbol */
struct chemical {
    double weight;      /* Atomic weight of element */
    char *symbol;       /* Symbol of element */
    char *name;         /* Name of element */
};

/* Define a type to make things easier */
typedef struct chemical ELEMENT;

int loadElements(char *);                       /* Function to load element data (below) */
void breakCompound();                           /* Function that breaks the compounds apart */
void calcWeight(char **, int *, char *, int);   /* Function to calculate atomic weight of a compound */
void pElements(char **, int);                   /* Function to print out the elements in alphabetical order */

/*
 * Array of pointers to element weights and symbols.
 * We allocate the space for each ELEMENT as we read each in
 */
ELEMENT *elements[MAXELEMENTS];     /* Array of ELEMENT pointers */
int nelements = 0;                  /* Number of elements */

/*
 * cmpstr - Specific qsort string comparison method
 * 
 * in parameters:
 * const void *a        first pointer to a void type value
 * const void *b        second pointer to a void type value
 */
int cmpstr(const void *a, const void *b) { 
    const char *ia = *(const char **)a;     /* Cast first void value as a character array and dereference it */
    const char *ib = *(const char **)b;     /* Cast second void value as a character array and dereference it */
    return strcmp(ia, ib);
    /* String comparison: returns - if a is higher up the alphabet than b and + if a is lower down the alphabet than b */ 
} 

/*
 * main - Calls functions to:
 *      - load in elements from a text file
 *      - parse compound into symbols & quantities
 *      - calculate the atomic weight
 * 
 * in parameters:
 * int argc             count of arguments
 * char *argv[]         array to store file name from command line
 * 
 * out parameters:
 * int                  0 for success, 1 for failure of program 
 */
int main(int argc, char *argv[]) {

    int exists = 0;     /* Boolean to make sure there contents inside file */

    /* Check to make there there is an element file */
    if (argc != 2) {
        fprintf(stderr, "Usage: %s weightsfile\n", argv[0]);
        exit(1);
    }

    /* Load the elements into the structure array from text file */
    exists = loadElements(argv[1]);

    /* Ask the user for a compound input, parse, and calculate atomic weight */
    if (exists == 1)
        breakCompound();

    printf("\n");

    /* Done! */
    exit(0);
}

/*
 * loadElements - Loads in elements from a file and stores them inside the elements array
 * 
 * in parameters:
 * char *fname          file name with atomic weights & symbols to load in elements
 *
 * out parameters:
 * int                  0 for successful read, 1 for failure to read
*/
int loadElements(char *fname) {
    FILE *fp;               /* Points to input element file */
    ELEMENT *tmp;           /* Points to space for new element */

    int lineno = 1;         /* Current input line number */
    int n;                  /* Number of inputs scanf read */
    
    int symblen = 2;        /* Length of symbol set to 2 */
    int namelen = 15;       /* Length of name set to 15 */
    char symbol[2];         /* Element symbol */
    float weight = 0.0;     /* Weight of element in atm */
    char name[100];         /* Element name, will not be used */


    /* Open the element file */
    if ((fp = fopen(fname, "r")) == NULL) {
        perror(fname);
        exit(1);
    }

    /* Read in each line and store the atomic weight and symbol. Ignore the element name */
    while (nelements < MAXELEMENTS && (n = fscanf(fp, "%f %s %s\n", &weight, symbol, name)) == 3) {

        /* Get one element and allocate space for the structure */
        if ((tmp = malloc(sizeof(ELEMENT))) == NULL){
            sprintf(fname, "%s(%d):", fname, lineno);
            perror(fname);
            exit(1);
        }

        /* Allocate space for the string symbol */
        if ((tmp->symbol = malloc((symblen) * sizeof(char))) == NULL){
            sprintf(fname, "%s(%d):", fname, lineno);
            perror(fname);
            exit(1);
        }

        /* Allocate space for the string name */
        if ((tmp->name = malloc((namelen) * sizeof(char))) == NULL){
            sprintf(fname, "%s(%d):", fname, lineno);
            perror(fname);
            exit(1);
        }

        /* Input the element symbol, name, and atomic weight into the structure */
        (void) strcpy(tmp->symbol, symbol);
        (void) strcpy(tmp->name, name);
        tmp->weight = weight;

        /* Save the structure in the array */
        elements[nelements++] = tmp;

        /* Go to the next line */
        lineno++;
    }

    /* See why you dropped out */
    if (n != EOF){
        fprintf(stderr, "%s: malformed line %d\n", fname, lineno);
        exit(1);
    }

    /* Print error if there are no element symbols or atomic weights in file */
    if (nelements == 0) {
        fprintf(stderr, "%s: no atomic weights there!\n", fname);
        exit(1);
    }

    /* Close the file */
    (void) fclose(fp);
    return(1);
}


/*
 * breakCompound - Accepts user input for a chemical compound and breaks up into symbols & quantities
*/
void breakCompound() {

    char buf[BUFFERSIZE];   /* Input buffer */
    int i = 0;              /* For loop interator through compound input */
    int num = 0;            /* Number of elements and corresponding quantities */
    int symblen = 3;        /* Length of symbol set to 2 */

    char *symbols[100];     /* Array of compound symbols */
    int quantity[100];      /* Array of compound quantitites */

    /* Prompt the user for their chemical compound */
    printf("Chemical composition? ");
    while(fgets(buf, BUFFERSIZE, stdin) != NULL) {
        /* Clobber the trailing newline to keep the output clean */
        if (buf[strlen(buf)-1] == '\n')
            buf[strlen(buf)-1] = '\0';

        /* For loop the user input compound and parse for element symbols and quantity */
        for(i = 0; i < strlen(buf); i++) {

            /* Check that the first element is uppercase */
            if (isupper(buf[i])) {

                /* Allocate space for the string symbol */
                symbols[num] = malloc((symblen) * sizeof(char));

                /* Copy the first UPPER symbol from buffer into the symbol array */
                strncpy(symbols[num], &buf[i], 1);
                symbols[num][1] = '\0';     /* Add null byte to signal end of symbol */

                i++;    /* Move to next index */
                /* Check to see if next value is lowercase */
                if (i < strlen(buf) && islower(buf[i])) {
                    /* Concatenate the second LOWER symbol from buffer into the symbol array */
                    strncat(symbols[num], &buf[i], 1);
                    symbols[num][2] = '\0';     /* Add null byte to signal end of symbol */

                    i++;    /* Move to next index since lowercase char exists */
                }

                /* Check to see that next value is a digit */
                if (i < strlen(buf) && isdigit(buf[i]))
                    quantity[num] = atoi(&buf[i]);
                else {
                    quantity[num] = 1;
                    /* Decrement to avoid overlapping a value */
                    i--;
                }
                /* Increment element index */
                num++;
            }
        }

        /* Calculate the weight of the inputted compound */
        calcWeight(symbols, quantity, buf, num);

        printf("Chemical composition? ");

        /* Reset the symbols and quantity arrays */
        memset(&symbols[0], 0, sizeof(symbols));
        memset(&quantity[0], 0, sizeof(quantity));

        num = 0;
    }

}

/*
 * calcWeight - Uses the symbols, quantity, and elements array to calculate atomic weight
 * 
 * in parameters:
 * char **symbols       compound array of element symbols
 * int *quantity        compound array of element quantity
 * char *input          string compound input
 * int num              number to check how many elements read in
*/
void calcWeight(char **symbols, int *quantity, char *input, int num) {

    int i = 0;          /* For loop iterator through compound input */
    int j = 0;          /* For loop iterator through elements array */
    int k = 0;          /* For loop iterator through names array */
    int found = 0;      /* Boolean to determine if element is found from element array */
    int contains = 0;   /* Boolean to determine if element name is already present in names array*/
    int ecnt = 0;       /* Counter for number of unique element names */
    double total = 0.0; /* Atomic weight count */

    char *names[100];   /* Array to store element names for alphabetization */

    /* Check if compound is valid if any elements were stored into array via num */
    if (num == 0) {
        fprintf(stderr, "%s: not a valid compound\n", input);
        return;
    }

    /* Iterate through inputted compound list */
    for (i = 0; i < num; i++) {

        /* Scan through stored elements array */
        for (j = 0; j < nelements; j++) {

            /* Check if compound symbol equals element symbol from array */
            if (strncmp(symbols[i], elements[j]->symbol, 2) == 0) {
                total += quantity[i]*elements[j]->weight;
                found = 1;

                /* Allocate space for the string symbol */
                names[i] = malloc((15) * sizeof(char));

                /* Check to see if element name is already stored inside names array */
                for(k = 0; k < i; k++) {
                    if (strcmp(names[k], elements[j]->name) == 0)
                        contains = 1;
                }

                /* Copy the element name into array if not already inside */
                if (contains == 0) {
                    strcpy(names[i], elements[j]->name);
                    names[i][strlen(elements[j]->name)] = '\0';     /* Add null byte to signal end of symbol */
                    ecnt++;
                    /*printf("%s\n", names[i]);*/
                }
                break;
            }
        }

        /* Check if the element is found/if the compound is valid */
        if (found == 0) {
            fprintf(stderr, "%s: no such element\n", symbols[i]);
            fprintf(stderr, "%s: not a valid compound\n", input);
            return;
        }

        found = 0;      /* Reset the found variable to false */
        contains = 0;   /* Reset contains variable for element name inside array to false */
    }

    /* Print out the input compound name and atomic weight */
    printf("The atomic weight of %s is %0.2f\n", input, total);

    /* Call to sort the elements and print them alphabetically with correct grammar */
    pElements(names, ecnt);

}

/*
 * pElements - Sorts the element names using qsort and prints out the elements with proper grammar
 * 
 * in parameters:
 * char **names       compound array of element names
 * int cnt            number for how many total elements
*/
void pElements(char **names, int cnt) {

    int i = 0;          /* For loop iterator through element names */

    /* Perform qsort() on element name array */
    qsort(names, cnt, sizeof(char*), cmpstr);

    /* Elements output for 1 element */
    if (cnt == 1)
        printf("The element is %s\n", names[0]);

    /* Elements output for 2 elements */
    else if (cnt == 2)
        printf("The elements are %s and %s\n", names[0], names[1]);
    
    /* Elements output for more than 2 elements */
    else {
        printf("The elements are");
        for (i = 0; i < cnt-2; i++) {
            printf(" %s,", names[i]);
        }
        printf(" %s and %s\n", names[cnt-2], names[cnt-1]);
    }
}
