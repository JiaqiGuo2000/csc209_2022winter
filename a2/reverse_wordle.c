#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "wordle.h"
#include "constraints.h"

/* Read the wordle grid and solution from fp. 
 * Return a pointer to a wordle struct.
 * See sample files for the format. Assume the input file has the correct
 * format.  In other words, the word on each is the correct length, the 
 * words are lower-case letters, and the line ending is either '\n' (Linux,
 * Mac, WSL) or '\r\n' (Windows)
 */
struct wordle *create_wordle(FILE *fp) {
    struct wordle *w = malloc(sizeof(struct wordle));
    char line[MAXLINE]; 
    w->num_rows = 0;

    while(fgets(line, MAXLINE, fp ) != NULL) {
        
        // remove the newline character(s) 
        char *ptr;
        if(((ptr = strchr(line, '\r')) != NULL) ||
           ((ptr = strchr(line, '\n')) != NULL)) {
            *ptr = '\0';
        }
        strncpy(w->grid[w->num_rows], line, SIZE);
        w->grid[w->num_rows][SIZE - 1] = '\0';
        w->num_rows++;
    }
    return w;
}


/* Create a solver_node and return it.
 * If con is not NULL, copy con into dynamically allocated space in the struct
 * If con is NULL set the new solver_node con field to NULL.
 * Tip: struct assignment makes copying con a one-line statements
 */
struct solver_node *create_solver_node(struct constraints *con, char *word) {
    struct solver_node *ret = malloc(sizeof(*ret));
    int i;
    if (con != NULL){
        struct constraints *new_con = init_constraints();
        for (i = 0; i < ALPHABET_SIZE; i++){
            new_con->cannot_be[i] = con->cannot_be[i];
        }
        for (i = 0; i < WORDLEN; i++){
            strcpy(new_con->must_be[i], con->must_be[i]);
        }
        ret->con = new_con;
    }
    else {
        ret->con = NULL;
    }
    strcpy(ret->word, word);
    ret->child_list = NULL;
    ret->next_sibling = NULL;
    
    return ret;
}

/* Return 1 if "word" matches the constraints in "con" for the wordle "w".
 * Return 0 if it does not match
 */
int match_constraints(char *word, struct constraints *con, 
struct wordle *w, int row) {
    int i;
    int j;
    char curr_letter;
    char comp_letter;
    int have_letter;
    if (strlen(word) != 5){
        return 0;
    }
    
    for (i = 0; i < WORDLEN; i++){
        curr_letter = word[i];

        // disqualify the word if the letter is not in the must be list
        if (con->must_be[i][0] != '\0'){
            have_letter = 0;
            for (j = 0; j < strlen(con->must_be[i]); j++){
                if (curr_letter == con->must_be[i][j]){
                    have_letter = 1;
                }
            }
            if (have_letter == 0){
                return 0;
            }
        }

        // must_be[i] is empty, we look into the cannot be list
        else {
            // disqualify the word if the letter is in the cannot be list
            for (j = 0; j < ALPHABET_SIZE; j++){
                if (con->cannot_be[j] == '1'){
                    comp_letter = j+97;
                    if (comp_letter == curr_letter){
                        return 0;
                    }
                }
            }
        }

        // rule out the possibility that a yellow letter could be at correct position as a green letter
        if (word[i] == w->grid[0][i] && w->grid[row][i] == 'y'){
            return 0;
        }
    }
    
    // rule out the words with duplicated solution letters
    for (i = 0; i < WORDLEN; i++){
        for (j = i + 1; j < WORDLEN; j++){
            if (word[i] == word[j]){
                for (int k = 0; k < WORDLEN; k++){
                    if (w->grid[0][k] == word[i]){
                        return 0;
                    }
                }
            }
        }
    }

    // otherwise it is qualified
    return 1;
}
/* remove "letter" from "word"
 * "word" remains the same if "letter" is not in "word"
 */
void remove_char(char *word, char letter) {
    char *ptr = strchr(word, letter);
    if(ptr != NULL) {
        *ptr = word[strlen(word) - 1];
        word[strlen(word) - 1] = '\0';
    }
}

/* Build a tree starting at "row" in the wordle "w". 
 * Use the "parent" constraints to set up the constraints for this node
 * of the tree
 * For each word in "dict", 
 *    - if a word matches the constraints, then 
 *        - create a copy of the constraints for the child node and update
 *          the constraints with the new information.
 *        - add the word to the child_list of the current solver node
 *        - call solve_subtree on newly created subtree
 */

void solve_subtree(int row, struct wordle *w,  struct node *dict, 
                   struct solver_node *parent) {
    struct solver_node *new_node;
    struct solver_node *prev_node = NULL;
    struct solver_node *first_node = NULL;
    int i;
    char all_g[SIZE];

    if(verbose) {
        printf("Running solve_subtree: %d, %s\n", row, parent->word);
    }
    
    if (row == w->num_rows + 1){
        // base case, the tree has reached its target depth
        return;
    }

    if (row == 1){
        // the root node cannot have siblings
        parent->next_sibling = NULL;
        // this is for the solution word's pseudo grid
        for (int j = 0; j < WORDLEN; j++){
            all_g[j] = 'g';
        }
        all_g[WORDLEN] = '\0';
    }

    // setup constrains
    for (i = 0; i < WORDLEN; i++){
        if (w->grid[row][i] == 'g'){
            set_green(parent->word[i], i, parent->con);
        }
        else if (w->grid[row][i] == 'y'){
            if (row == 1){
                // we consider the grid for solution word as an all green sequence
                set_yellow(i, w->grid[row], all_g, parent->word, parent->con);
            }
            else {
                set_yellow(i, w->grid[row], w->grid[row - 1], parent->word, parent->con);
            }
        }
    }
    add_to_cannot_be(parent->word, parent->con);
    
    struct node *ptr = dict;
    while (ptr != NULL){
        // if we find a valid guess, then we add a new node for it
        if (match_constraints(ptr->word, parent->con, w, row) == 1){
            
            // setup word and init new constrains
            new_node = create_solver_node(NULL, ptr->word);
            new_node->con = init_constraints();
            for (i = 0; i < ALPHABET_SIZE; i++){
                new_node->con->cannot_be[i] = parent->con->cannot_be[i];
            }
            new_node->child_list = NULL;
            new_node->next_sibling = NULL;

            // recurrsive call
            if (row < w->num_rows){
                solve_subtree(row + 1, w, dict, new_node);
            }

            // setup child_list in the parent node if this is the first node of this depth
            if (first_node == NULL){
                first_node = new_node;
                parent->child_list = new_node;
            }

            // setup next_sibling
            if (prev_node == NULL){
                prev_node = new_node;
            }
            else {
                prev_node->next_sibling = new_node;
                prev_node = new_node;
            }
        }
        // go for next word
        ptr = ptr->next;
    }
}

/* Print to standard output all paths that are num_rows in length.
 * - node is the current node for processing
 * - path is used to hold the words on the path while traversing the tree.
 * - level is the current length of the path so far.
 * - num_rows is the full length of the paths to print
 */

void print_paths(struct solver_node *node, char **path, 
                 int level, int num_rows) {
    //base case, print out the path at target depth
    if (level == num_rows + 1){
        for (int i = 0; i < num_rows; i++){
            printf("%s ", path[i]);
        }
        printf("\n");
        return;
    }

    struct solver_node* curr_node = node;
    // while loop for nodes in the same depth
    while (curr_node != NULL){
        path[level - 1] = curr_node->word;
        // recursive call for nodes in higher depth
        print_paths(curr_node->child_list, path, level+1, num_rows);
        curr_node = curr_node->next_sibling;
    }
}

/* Free all dynamically allocated memory pointed to from w.
 */ 
void free_wordle(struct wordle *w){
    free(w);
}

/* Free all dynamically allocated pointed to from node
 */
void free_tree(struct solver_node *node){
    // recursive call
    if (node->next_sibling != NULL){
        free_tree(node->next_sibling);
    }
    if (node->child_list != NULL){
        free_tree(node->child_list);
    }

    // base case, no sibling and child, free the node
    free(node->con);
    free(node);
}
