/****************************************************************************
 * dictionary.c
 *
 * Computer Science 50
 * Problem Set 6
 *
 * Implements a dictionary's functionality.
 ***************************************************************************/

#include <stdbool.h>
#include <cs50.h>
#include <ctype.h>
#include "dictionary.h"
#include <stdio.h>

typedef struct node
{
    bool EOW;
    struct node *letter[27];
} 
node;

node *root = NULL;
int *word_count;

node* find_last(node *curr_node, node *prev_node, int *prev_int, int *root_switch);


/**
 * Returns true if word is in dictionary else false.
 */
bool check(const char* word)
{
    node *curr;
    curr = root;
    
    int i = 0;
    
    /* start a loop moving through each char of word until it gets to null.
     * 
     * if it ever hits a point where the char in word[] does not have a
     * a corresponding char in the load trie, it will return false.
     * 
     * similarly, if the given char suggests the end of the word[] (meaning
     * the given char is \0), the program will check if the node's EOW switch
     * has been set to true. if not, will return false.
     *
     * if given char is \O and EOW is true, will return true. this is the only
     * case in which check() will return true.
    */
    while (true)
    {                        
        char word_char = tolower(word[i]);
        
        if (word_char == '\0')
        {
            if (curr->EOW == true)
            {
                return true;
            }
            else
            {
                return false;
            };       
        }
        else if (word_char == 39)
        {
            if (curr->letter[word_char - 13] == NULL)
            {
                return false;
            }
            else
            {
                curr = curr->letter[word_char - 13];
            };      
        }
        else
        {
            if (curr->letter[word_char - 97] == NULL)
            {
                return false;
            }
            else
            {          
                curr = curr->letter[word_char - 97];           
            }
        };
        
        i++;      
    }; 
}

/**
 * Loads dictionary into memory.  Returns true if successful else false. Uses char checking process from speller.c in CS50.
 */
bool load(const char* dictionary)
{    
    if (dictionary == NULL)
    {
        printf("Error loading dictionary.");
        return false;   
    };
    
    //set up dictionary_file
    FILE *dictionary_file = fopen(dictionary, "r");
    if (dictionary_file == NULL)
    {
        printf("Error loading dictionary.");
        return false;
    };
    
    // set up root node
    root = malloc(sizeof(node));       
    root->EOW = NULL;
    for (int i = 0; i < 27; i++)
    {
        root->letter[i] = NULL;   
    };    
    
    // set up word_count node to be used later in size()
    word_count = malloc(sizeof(int));
    *word_count = 0;    

    // set up nodes with which to build trie    
    node *curr;
    node *next;
    curr = root;
        
    //set up dictionary_char
    int dictionary_char = 0;
             
    // until EOF
    while (dictionary_char != EOF)
    {                                        
        // until '\n'
        while (true)
        {            
            dictionary_char = fgetc(dictionary_file);          
            
            // check for '\n' and EOF
            if (dictionary_char == '\n' || feof(dictionary_file))
            {
                break;
            }            
            // special case of apostrophe in ASCII.
            else if (dictionary_char == 39)
            {
                if (curr->letter[dictionary_char - 13] == NULL)
                {
                    next = malloc(sizeof(node));
                    next->EOW = NULL;
                    for (int i = 0; i < 27; i++)
                    {
                        next->letter[i] = NULL;   
                    }; 
                    curr->letter[dictionary_char - 13] = next;
                    curr = next;
                }
                else
                {
                    curr = curr->letter[dictionary_char - 13];
                };
            }                        
            // a-z in ASCII
            else
            {
                if (curr->letter[dictionary_char - 97] == NULL)
                {
                    next = malloc(sizeof(node));
                    next->EOW = NULL;
                    for (int i = 0; i < 27; i++)
                    {
                        next->letter[i] = NULL;   
                    }; 
                    curr->letter[dictionary_char - 97] = next;
                    curr = next;
                }
                else
                {
                    curr = curr->letter[dictionary_char - 97];
                };
            };

        };
        
        //end of word sequence
        *word_count = *word_count+1;
        curr->EOW = true;
        curr = root;               
    };
    
    fclose(dictionary_file);
    return true;
}

/**
 * Returns number of words in dictionary if loaded else 0 if not yet loaded.
 */
unsigned int size(void)
{
    return *word_count;
}

/**
 * Unloads dictionary from memory.  Returns true if successful else false.
 */
bool unload(void)
{    
    node *curr_node;
    curr_node = root;
    
    node *prev_node;
    prev_node = root;
    
    int *prev_int;
    prev_int = malloc(sizeof(int));
    
    int *root_switch;
    root_switch = malloc(sizeof(int));
    *root_switch = 1; 
    
    
    // uses find_last to repeatedly free the lowest node until only the root is left 
    while (*root_switch != 0)
    {               
        *root_switch = 0;
        free(find_last(curr_node, prev_node, prev_int, root_switch));
    }; 
    
    // frees pointers used in the course of speller.c and dictionary.c
    free(prev_int);
    free(root_switch);
    free(word_count);
       
    return true;
}

/* 
 * searches down a trie, choosing the first available path
 * once it hits the end of a path (meaning the current node
 * has no children, it will return the location of that node, 
 * which will be freed, per unload()
*/
node* find_last(node *curr_node, node *prev_node, int *prev_int, int *root_switch)
{
    int i = 0;
    while (i < 27)
    {
        if (curr_node->letter[i] != NULL)
        {
            *prev_int = i;
            *root_switch = 1;
            prev_node = curr_node;
            
            /* recursive quality means that each time find_last is given the root node
             * it will search down the trie until it finds the furthest node. since unload() is
             * freeing these nodes, it's important that the function can produce different
             * outputs even though it's repeatedly being given the root node for input.
             * once the root node is both input and output, *root_switch will remain at 0,
             * per unload(), as it has not been set to 1 in find_last. this will break the 
             * loop in unload().
             */
            return find_last(curr_node->letter[i], prev_node, prev_int, root_switch);       
        }
        else
        {
            i++;
        };  
    };
    
    /* sets furthest node to null, and sets the pointer that points to curr node 
    * (aka prev_node->letter[*prev_int] to null). also sets the EOW for prev node to
    * null.
    */
    curr_node->EOW = NULL;
    prev_node->EOW = NULL;
    prev_node->letter[*prev_int] = NULL;
    return curr_node;
}