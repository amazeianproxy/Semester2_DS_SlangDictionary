#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h> //Using bool so that I don't have to use char and get confused

typedef struct node{
	char letter;
	char description[100]; //Store description for every possible ending point of a word.
	node *branch[128]; //ASCII is 0 - 127.
	bool end_of_slang;
} node;

node *root = NULL;

void menu_select(); //Calls menu function for other functions to be able to call it.

node *trie_node(char chr){
	node *trie = (node*) malloc(sizeof(node));
	trie->letter = chr;
	trie->end_of_slang = false;
	strcpy(trie->description, "x"); //Description is limited to more than 1 letter, so 1 letter is enough to set as a neutral state/placeholder.
	for(int i = 0; i < 128; i++){
		trie->branch[i] = NULL;
	}
	return trie;
}

//Recursively pushes letter from string. At the end, attach description to the end_of_slang node.
void insert(node *curr, char *chr, char desc[]){ 
	while(*chr != NULL){ //Traverse per *chr while it exists.
		if(curr->branch[*chr] == NULL){ //Makes new trie nodes if they don't exist.
			curr->branch[*chr] = trie_node(*chr);
		}
		curr = curr->branch[*chr]; //Traverse to next node.
		chr++; //Move to next character on string.
	}
	curr->end_of_slang = true;
	if(strcmp(curr->description, "x") == 0){ //Check if curr has description.
		puts("");
		puts("Successfully released new slang word.");
	}
	else{
		puts("");
		puts("Successfully updated a slang word.");
	}
	strcpy(curr->description, desc);
}

//Recursively matches letter from string.
node *search(node *curr, char *chr){
	while(*chr != NULL){
		if(curr->branch[*chr] == NULL) return NULL; //Character does not exist in trie, so return NULL.
		curr = curr->branch[*chr];
		chr++;
	}
	return curr; //Returns the node itself if word exists.
}

//Prints all slang words.
void print_all(node *curr, char *hold_word, int depth, char *prefix_str, int *count){
	if(curr == NULL) return;
	
	char *buffer = (char*) malloc((depth+2)*sizeof(char)); //Create a buffer to store the previous resulting segment + the new segment.
	strcpy(buffer, hold_word);
	
	if(curr->end_of_slang == true){ //Check for end of slang.
		printf("%d. ", *count); //No. of words printed.
		(*count)++; //Increments the count outside of this function.
		
		if(prefix_str != NULL) printf("%s", prefix_str); //For use in printing prefix prints. (See below function.)
		buffer[depth] = '\0'; //Ends the string in buffer.
		printf("%s\n", buffer); //Outputs everything in buffer.
	}
	
	for(int i = 0; i < 128; i++){
		if(curr->branch[i]){ //Check if the next branch exists.
			buffer[depth] = i; //Sets the ASCII value (i) into the buffer[depth], which becomes a char.
			print_all(curr->branch[i], buffer, depth+1, prefix_str, count);
		}
	}
	
	free(buffer);
}

//Calls the print_all function by passing the "prefix start node" as a starting point (prefix in the buffer).
void print_prefix(node *curr, char *chr, int *count){
	char *hold_prefix_pos = chr;
	while(*chr != NULL){
		//Contingency, just in case. In theory, this shouldn't happen, since we checked before using this function.
		if(curr->branch[*chr] == NULL) return; 
		curr = curr->branch[*chr];
		chr++;
	}
	//Calls the print_all function to just print all words with prefix attached.
	print_all(curr, hold_prefix_pos, strlen(chr), hold_prefix_pos, count);
}

//Function 1: Inputs new slang into the trie.
void new_slang(){
	puts("");
	
	//Inputs slang word.
	puts("Input a new slang word [Must be more than 1 character and contains no space(s)]: ");
	printf("> ");
	char str[30];
	scanf("%[^\n]", &str);
	getchar();
	
	int len = strlen(str);
	
	//Check for single character.
	if(len == 1){
		puts("ERROR: Slang only contains 1 character!");
		new_slang();
	}
	
	//Check for spaces.
	else if(strchr(str, ' ')){
		puts("ERROR: Slang contains spaces!");
		new_slang();
	}
	
	//Inputs slang description.
	puts("Input a new slang word description [Must be more than 1 word]: ");
	printf("> ");
	char desc[100];
	scanf("%[^\n]", &desc);
	getchar();
	
	//Check if two words.
	int index = 0;
	len = strlen(desc);
	while(desc[index] != '\0'){
		if(desc[index] == ' ') break;
		index++;
	}
	if(index == len || desc[index+1] == '\0' || desc[index+1] == ' '){ //Handles cases like "word " and "word  ".
		puts("ERROR: Description only contains 1 word!");
		new_slang();
	}
	
	//Insert string.
	insert(root, str, desc);
	
	puts("Press enter to continue.");
	getchar();
	menu_select();
}

//Function 2: Searches a slang exactly.
void search_slang(){
	puts("");
	
	//Inputs slang word.
	puts("Input a slang word to be searched [Must be more than 1 character and contains no space(s)]: ");
	printf("> ");
	char target_str[30];
	scanf("%[^\n]", &target_str);
	getchar();
	
	int len = strlen(target_str);
	
	//Check for single character.
	if(len == 1){
		puts("ERROR: Slang only contains 1 character!");
		search_slang();
	}
	
	//Check for spaces.
	else if(strchr(target_str, ' ')){
		puts("ERROR: Slang contains spaces!");
		search_slang();
	}
	
	//Search
	puts("");
	node *search_node = search(root, target_str);
	
	//Check if the node is NULL or not. 
	//Also, check if description is filled out for the word. If so, then output word and description.
	//This description check is required, or search will also output true for prefixes.
	//This function of the search node is also being used by the view_all_prefix function.
	if(search_node != NULL && strlen(search_node->description) > 1){
		printf("Slang word: %s\n", target_str);
		printf("Description: %s\n", search_node->description);
		puts("");
	}
	else printf("There is no such word as \"%s\" in the dictionary.\n", target_str);
	
	puts("Press enter to continue.");
	getchar();
	menu_select();
}

//Function 3: Prints all slang words with prefix attached.
void view_all_prefix(){
	puts("");
	
	//Inputs slang prefix.
	puts("Input a prefix to be searched: ");
	printf("> ");
	char prefix[30];
	scanf("%[^\n]", &prefix);
	getchar();
	
	//Search if prefix exists.
	node *search_node = search(root, prefix);
	
	//In this case, only check if node exists or not, since any such prefix of an existing words comes back true.
	if(search_node == NULL){
		printf("There is no such prefix as \"%s\" in the dictionary.\n", prefix);
	}
	else{
		int count = 1;
		puts("");
		printf("Words that start with \"%s\":\n", prefix);
		print_prefix(root, prefix, &count);
		puts("");
	}
	
	puts("Press enter to continue.");
	getchar();
	menu_select();
}

// Function 4: Prints all slang words.
void view_all_slang(){
	bool trie_exist = false;
	for(int i = 0; i < 128; i++){ //Loop all branches of root to find existing node. If exists, then set trie_exist true.
		if(root->branch[i] != NULL){
			trie_exist = true;
			break;
		}
	}
	
	if(trie_exist == false){ //Trie doesn't exist, return to menu.
		puts("");
		puts("There are no slang words yet in the dictionary.");
	}
	else{
		int count = 1;
		puts("");
		puts("List of all slang words in the dictionary:");
		print_all(root, "", 0, "", &count);
		puts("");
	}

	puts("Press enter to continue.");
	getchar();
	menu_select();
}

void menu_select(){
	puts("1. Release a new slang word");
	puts("2. Search a slang word");
	puts("3. View all slang words starting with a certain prefix word");
	puts("4. View all slang words");
	puts("5. Exit");
	
	char choice;
	printf("Insert Choice: ");
	scanf("%c", &choice);
	getchar();
	
	switch(choice){
		case '1': //Make or update a slang word & description.
			new_slang();
			break;
			
		case '2': //Searches a slang word.
			search_slang();
			break;
			
		case '3': //Views all slang words starting with prefix.
			view_all_prefix();
			break;
			
		case '4': //Views all slang words.
			view_all_slang();
			break;
			
		case '5': //Exits program.
			puts("Thank you for using Boogle! Have a nice day!");
			exit(0);
			break;
			
		default: //Any choice not conforming to the selection above.
			puts("Choice unavailable, please choose again.");
			puts("");
			fflush(stdin); //In case of string inputs, which cause correct inputs to become false due to stdin carryover.
			menu_select();
	}
}

int main(){
	//Sets root.
	root = trie_node(' '); 
	//In this case, sets the root node to ' '. 
	//Technically, the character here doesn't really matter that much, because we immediately traverse by branch.
	//It's just set to ' ' for convenience.
	
	//Calls menu.
	menu_select();
	
	return 0;
}
