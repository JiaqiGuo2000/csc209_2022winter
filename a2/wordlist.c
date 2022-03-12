#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "wordlist.h"


/* Read the words from a filename and return a linked list of the words.
 *   - The newline character at the end of the line must be removed from
 *     the word stored in the node.
 *   - You an assume you are working with Linux line endings ("\n").  You are
 *     welcome to also handle Window line endings ("\r\n"), but you are not
 *     required to, and we will test your code on files with Linux line endings.
 *   - The time complexity of adding one word to this list must be O(1)
 *     which means the linked list will have the words in reverse order
 *     compared to the order of the words in the file.
 *   - Do proper error checking of fopen, fclose, fgets
 */
struct node *read_list(char *filename) {
    struct node* curr_node = malloc(sizeof(*curr_node));
    struct node* next_node;
    struct node* ret = curr_node;
    struct node* previous;
    FILE *fp;
    char buffer[SIZE];
    char word[SIZE];

    // open file and check
    fp = fopen(filename, "r");
    if(fp == NULL)
    {
        printf("Error opening file\n");
        exit(1);
    }
    while (fgets(buffer, SIZE+1, fp) != NULL){
        sscanf(buffer, "%s\n", word);
        if (strlen(word) == 0){
            break;
        }
        // error check
        else if (strlen(word) != WORDLEN){
            printf("word length problem!\n");
            exit(1);
        }
        // set word
        strcpy(curr_node->word, word);
        curr_node->word[WORDLEN] = '\0';
        // init next node
        next_node = malloc(sizeof(*next_node));
        curr_node->next = next_node;
        // record previous node
        previous = curr_node;
        curr_node = next_node;
    }
    if (previous != NULL){
        // remove the last node which is not initialized and free it
        free(curr_node);
        previous->next = NULL;
    }
    // close file and check
    if (fclose(fp) != 0){
        printf("error closing the file!\n");
        exit(1);
    }
    return ret;
}

/* Print the words in the linked-list list one per line
 */
void print_dictionary(struct node *list) {
    struct node* curr_node = list;
    // traverse the linked list
    while (curr_node->next != NULL){
        printf("%s\n", curr_node->word);
        curr_node = curr_node->next;
    }
}
/* Free all of the dynamically allocated memory in the dictionary list 
 */
void free_dictionary(struct node *list) {
    struct node* curr_node = list;
    struct node* next_node = curr_node->next;
    // free the linked list from root to end accordingly
    while (curr_node->next != NULL){
        free(curr_node);
        curr_node = next_node;
        next_node = curr_node->next;
    }
    free(curr_node);
}

