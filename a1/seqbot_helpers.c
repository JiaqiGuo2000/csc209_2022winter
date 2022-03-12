#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include "seqbot_helpers.h"

/* Return the melting temperature of sequence, or -1 if the sequence is invalid.
 * The melting temperature formula is given in the handout.
 * An invalid sequence is a sequence of length 0, or a sequence that contains
 * characters other than 'A', 'C', 'G', 'T'.
 */
int calculate_melting_temperature(char *sequence, int sequence_length)
{

    int ret = 0;
    int i;
    if (sequence_length == 0){
        return -1;
    }
    for (i = 0; i < sequence_length; i++){
        if (sequence[i] == 'A'){
            ret = ret + 2;
        }
        else if (sequence[i] == 'T'){
            ret = ret + 2;
        }
        else if (sequence[i] == 'C'){
            ret = ret + 4;
        }
        else if (sequence[i] == 'G'){
            ret = ret + 4;
        }
        else {
            return -1;
        }
    }
    return ret;
}

/* Prints the instructions to make a molecule from sequence.
 * If an invalid character is found in sequence print
 * "INVALID SEQUENCE" and return immediately
 */
void print_instructions(char *sequence, int sequence_length)
{
    int i;
    char cur_nc;
    int section_start;
    printf("START\n");
    if (sequence_length == 0){
        printf("INVALID SEQUENCE\n");
        return;
    }
    else {
        cur_nc = *sequence;
        section_start = 0;
    }
    for (i = 1; i < sequence_length; i++){
        if (sequence[i] != 'A' && sequence[i] != 'T' && sequence[i] != 'C' && sequence[i] != 'G'){
            printf("INVALID SEQUENCE\n");
            return;
        }
        if (sequence[i] != cur_nc){
            printf("WRITE %c %d\n", cur_nc, i - section_start);
            cur_nc = sequence[i];
            section_start = i;
        }
    }
    printf("WRITE %c %d\n", cur_nc, i - section_start);
    printf("SET_TEMPERATURE %d\n", calculate_melting_temperature(sequence, sequence_length));
    printf("END\n");
    return;
}


/* Print to standard output all of the sequences of length k.
 * The format of the output is "<length> <sequence> 0" to 
 * correspond to the input format required by generate_molecules_from_file()
 * 
 * Reminder: you are not allowed to use string functions in these files.
 */
void generate_all_molecules(int k)
{
    // DFS
    char sequence[k+1];
    void genall_helper(int k, int cur_pos, char *sequence, char next){
        if (cur_pos == k - 1){
            sequence[k - 1] = next;
            sequence[k] = '\0';
            printf("%d %s 0\n", k, sequence);
            return;
        }
        sequence[cur_pos] = next;
        genall_helper(k, cur_pos+1, sequence, 'A');
        genall_helper(k, cur_pos+1, sequence, 'C');
        genall_helper(k, cur_pos+1, sequence, 'G');
        genall_helper(k, cur_pos+1, sequence, 'T');
        return;
    }
    
    genall_helper(k, 0, sequence, 'A');
    genall_helper(k, 0, sequence, 'C');
    genall_helper(k, 0, sequence, 'G');
    genall_helper(k, 0, sequence, 'T');
    return;

    // BFS
    /*char sequence[k];
    int i;
    if (k <= 0){
        return;
    }
    int total = 1;
    for (i = 0; i < k; i++){
        total = total * 4;
    }
    int cur_pos = 0;
    int cur_nc;
    int index_remain;
    int cur_div;
    for (i = 0; i < total; i++){
        index_remain = i;
        cur_div = total/4;
        for (cur_pos = 0; cur_pos < k; cur_pos++){
            cur_nc = index_remain / cur_div;
            index_remain = index_remain % cur_div;
            cur_div = cur_div/4;
            if (cur_nc == 0){
                sequence[cur_pos] = 'A';
            }
            else if (cur_nc == 1){
                sequence[cur_pos] = 'C';
            }
            else if (cur_nc == 2){
                sequence[cur_pos] = 'G';
            }
            else if (cur_nc == 3){
                sequence[cur_pos] = 'T';
            }
        }
        sequence[k] = '\0';
        printf("%d %s 0\n", k, sequence);
        
        // another approach...
        index_remain = i;
        cur_div = total/4;
        printf("%d ", k);
        for (cur_pos = 0; cur_pos < k; cur_pos++){
            cur_nc = index_remain / cur_div;
            index_remain = index_remain % cur_div;
            cur_div = cur_div/4;
            if (cur_nc == 0){
                printf("A");
            }
            else if (cur_nc == 1){
                printf("C");
            }
            else if (cur_nc == 2){
                printf("G");
            }
            else if (cur_nc == 3){
                printf("T");
            }
        }
        printf(" 0\n");
        
    }*/
}


/* Print the instructions for each of the sequences found in filename according
 * to the mode provided.
 * filename contains one sequence per line, and the format of each line is
 * "<length> <sequence> <mode>" where 
 *     - <length> is the number of characters in the sequence 
 *     - <sequence> is the array of DNA characters
 *     - <mode> is either 0, 1, 2, or 3 indicating how the <sequence> should 
 *              be modified before printing the instrutions. The modes have the 
 *              following meanings:
 *         - 0  - print instructions for sequence (unmodified)
 *         - 1  - print instructions for the the complement of sequence
 *         - 2  - print instructions for the reverse of sequence
 *         - 3  - print instructions for sequence where it is complemented 
 *                and reversed.
 * 
 * Error checking: If any of the following error conditions occur, the function
 * immediately prints "INVALID SEQUENCE" to standard output and exits the 
 * program with a exit code of 1.
 *  - length does not match the number of characters in sequence
 *  - length is not a positive number
 *  - sequence contains at least one invalid character
 *  - mode is not a number between 0 and 3 inclusive
 * 
 * You do not need to verify that length or mode are actually integer numbers,
 * only that they are in the correct range. It is recommended that you use a 
 * fscanf to read the numbers and fgetc to read the sequence characters.
 */
void generate_molecules_from_file(char* filename)
{
    FILE* fp = fopen(filename, "r");
    char buffer[1024];
    char sequence[1024];
    int sequence_length;
    int mode;
    int invalid;
    int len;
    
    void reverse(char *sequence, int sequence_length){
        char sequence_org[sequence_length];
        for (int j = 0; j < sequence_length; j++) {
            sequence_org[j] = sequence[j];
        }
        for (int j = 0; j < sequence_length; j++) {
            sequence[j] = sequence_org[sequence_length - j - 1];
        }
        return;
    }

    void complement(char *sequence, int sequence_length){
        for (int j = 0; j < sequence_length; j++){
            if (sequence[j] == 'A'){
                sequence[j] = 'T';
            }
            else if (sequence[j] == 'T'){
                sequence[j] = 'A';
            }
            else if (sequence[j] == 'C'){
                sequence[j] = 'G';
            }
            else if (sequence[j] == 'G'){
                sequence[j] = 'C';
            }
        }
        return;
    }

    while (fgets(buffer, 1024, fp) != NULL ){
        sscanf(buffer, "%d %s %d\n", &sequence_length, sequence, &mode);
        invalid = 0;
        for (int i = 0; i < sequence_length; i++){
            if (sequence[i] != 'A' && sequence[i] != 'T' && sequence[i] != 'C' && sequence[i] != 'G'){
                invalid = 1;
                break;
            }
        }
        for (len = 0; ; len ++){
            if (sequence[len] == '\0'){
                break;
            }
        }
        if (sequence_length != len || sequence_length < 0 || mode < 0 || mode > 3 || invalid == 1){
            fclose(fp);
            printf("INVALID SEQUENCE");
            exit(1);
        }
        if (mode == 1){
            complement(sequence, sequence_length);
        }
        else if (mode == 2){
            reverse(sequence, sequence_length);
        }
        else if (mode == 3){
            complement(sequence, sequence_length);
            reverse(sequence, sequence_length);
        }
        print_instructions(sequence, sequence_length);

    }
    

    fclose(fp);
    return;
}
