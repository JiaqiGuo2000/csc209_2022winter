#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>
#include "constraints.h"

/* Create and initialize a constraints struct. 
 * Sets the fields to 0 or empty string.
 * Return a pointer to the newly created constraints struct.
 */
struct constraints *init_constraints() {
    struct constraints *ret;
    ret = malloc(sizeof(struct constraints));
    int i;
    // set must_be to '\0'
    for (i = 0; i < WORDLEN; i++){
        ret->must_be[i][0] = '\0';
    }
    // set each char of cannot_be to '0'
    for (i = 0; i < ALPHABET_SIZE; i++){
        ret->cannot_be[i] = '0';
    }
    return ret;
}

/* Update the "must_be" field at "index" to be a string 
 * containing "letter"
 * The tile at this index is green, therefore the letter at "index"
 * must be "letter"
 */
void set_green(char letter, int index, struct constraints *con) {
    assert(islower(letter));
    assert(index >= 0 && index < SIZE);

    con->must_be[index][0] = letter;
    con->must_be[index][1] = '\0';
    return;
}

/* Update "con" by adding the possible letters to the string at the must_be 
 * field for "index".
 * - index is the index of the yellow tile in the current row to be updated
 * - cur_tiles is the tiles of this row
 * - next_tiles is the tiles of the row that is one closer to the solution row
 * - word is the word in the next row (assume word is all lower case letters)
 * Assume cur_tiles and next_tiles contain valid characters ('-', 'y', 'g')
 * 
 * Add to the must_be list for this index the letters that are green in the
 * next_tiles, but not green in the cur_tiles or green or yellow in the 
 * next_tiles at index.
 * Also add letters in yellow tiles in next_tiles.
 */
void set_yellow(int index, char *cur_tiles, char *next_tiles, 
                char *word, struct constraints *con) {

    assert(index >=0 && index < SIZE);
    assert(strlen(cur_tiles) == WORDLEN);
    assert(strlen(next_tiles) == WORDLEN);
    assert(strlen(word) == WORDLEN);

    int i;
    int must_be_index = 0;

    // if target tile is green, then it has to be the same letter in the next word
    if (cur_tiles[index] == 'g'){
        con->must_be[index][0] = word[index];
        con->must_be[index][1] = '\0';
    }
    // if target tile is yellow
    else if (cur_tiles[index] == 'y'){
        for (i = 0; i < WORDLEN; i++){
            // we take all the letters that are green or yellow in the next tile with that index in cur tile not green
            if (i != index && next_tiles[i] != '-' && cur_tiles[i] != 'g'){
                con->must_be[index][must_be_index] = word[i];
                must_be_index += 1;
            }
        }
    }
    // terminate the word
    con->must_be[index][must_be_index] = '\0';
    return;
}

/* Add the letters from cur_word to the cannot_be field.
 * See the comments in constraints.h for how cannot_be is structured.
 */
void add_to_cannot_be(char *cur_word, struct constraints *con) {
    assert(strlen(cur_word) <= WORDLEN);
    int i;
    // traverse the array
    for (i = 0; i < strlen(cur_word); i++){
        con->cannot_be[cur_word[i] - 'a'] = '1';
    }
    return;
}

void print_constraints(struct constraints *c) {
    printf("cannot_be: ");

    char curr_char;
    // traverse the cannot be array
    for (int i = 0; i < ALPHABET_SIZE; i++){
        if (c->cannot_be[i] == '1'){
            curr_char = 97 + i;
            printf("%c ", curr_char);
        }
    }
    
    printf("\nmust_be\n");

    // traverse the must be array
    for (int i = 0; i < WORDLEN; i++){
        printf("[%d] ", i);
        for (int j = 0; j < SIZE; j++){
            if (c->must_be[i][j] == '\0'){
                printf("\n");
                break;
            }
            printf("%c ", c->must_be[i][j]);
        }
    }
}

/* Free all dynamically allocated memory pointed to by c
 */
void free_constraints(struct constraints *c) {
    free(c);
}