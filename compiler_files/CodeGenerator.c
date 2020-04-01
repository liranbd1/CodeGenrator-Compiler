#include	"CodeGenerator.h"
#include "tree.h"

typedef struct symbol_table {

	char* name;
    char* type;
    int address;
    int size;

} Symbol_table;

typedef struct variable {

	Symbol_table* table;
    struct variable* next;
    struct variable* prev;
    struct variable* first;
    struct variable* last;

} Variable;

Variable *head;
Symbol_table *tmp;
int isLoadingVariable = 0;

int Find (char* name)
{
    Variable* currentPtr = head;
    while (currentPtr != NULL)
    {
        if(currentPtr->table->name == name)
        {
            return currentPtr->table->address;
        }
        currentPtr = currentPtr->next;
    }
    return 0;
}

/*Adding data to the Symbol table*/
void Add (Symbol_table * newSymbol)
{
    // Not our first variable
    if (head != NULL)
    {
        Variable *ptr = head;
        Variable *newVariable = (Variable *) malloc(sizeof(Variable));
        while (ptr->next != NULL)
        {
            ptr->last = newVariable;
            ptr = ptr->next;
        }
        newSymbol->address = ptr->table->address + ptr->table->size;
        newVariable->table = newSymbol;
        newVariable->prev = ptr;
        newVariable->next = NULL;
        newVariable->last = newVariable;
        ptr->next = newVariable;
        ptr->last = newVariable;
        newVariable->first = ptr->first;
    }
    // Our first variable
    else
    {
        head = (Variable*)malloc(sizeof(Variable));
        newSymbol->address = 5;
        head->table = newSymbol;
        head->next = NULL;
        head->prev = NULL;
        head->last = head;
        head->first = head;
    }
}

//Removing a variable from the Symbol table
void Remove(char name)
{
   Variable *ptr = head;
    Symbol_table *tmpTable;
    while (ptr->next != NULL)
    {
        tmpTable = ptr->table;
        if (*tmpTable->name == name)
        {
            free(tmpTable);
            Variable *prev = ptr->prev;
            Variable *next = ptr->next;
            prev->next=ptr->next;
            next->prev = prev;
            free(ptr);
        }
        ptr = ptr->next;
    }
}
/*
*	You need to build a data structure for the symbol table
*	I recommend to use linked list.
*
*	You also need to build some functions that add/remove/find element in the symbol table
*/


/*
*	This recursive function is the main method for Code Generation
*	Input: treenode (AST)
*	Output: prints the Pcode on the console
*/
int  code_recur(treenode *root)
{
	if_node  *ifn;
	for_node *forn;
	leafnode *leaf;
	int addressOfVariable;
				
    if (!root)
        return SUCCESS;

    switch (root->hdr.which){
		case LEAF_T:
			leaf = (leafnode *) root;
			switch (leaf->hdr.type) {
				case TN_LABEL:
					/* Maybe you will use it later */
					break;
				
				case TN_IDENT:
					/* variable case */
					/*
					*	In order to get the identifier name you have to use:
					*	leaf->data.sval->str
					*/
				// When we have for example int a; we first get to this one meaing we are going to the identifier first
				// From that I can see that we first need to see if this variable is already in our Symbol_table.
				//The case we don't return a 0
				//printf("TN_IDENT\n");
				addressOfVariable = Find(leaf->data.sval->str);
				if(addressOfVariable != 0) {
                    printf("ldc %d\n", addressOfVariable);
                    if(isLoadingVariable)
                    {
                        printf("ind\n");
                    }
                }
				//	printf("We are in teh Variable case\n");
					break;

				case TN_COMMENT:
					/* Maybe you will use it later */
					break;

				case TN_ELLIPSIS:
					/* Maybe you will use it later */
					break;

				case TN_STRING:
					/* Maybe you will use it later */
					break;

				case TN_TYPE:
					if(leaf->hdr.tok == INT)
                    {
					    tmp->type = "int";
					    tmp->size = sizeof(int);
                    }

					else if (leaf->hdr.tok == FLOAT)
                    {
					    tmp->type = "float";
					    tmp->size = sizeof(float);
                    }

					else if(leaf->hdr.tok == DOUBLE)
                    {
					    tmp->type = "double";
					    tmp->size = sizeof(double);
                    }
					break;

				case TN_INT:
					/* Constant case */
					/* 
					*	In order to get the int value you have to use: 
					*	leaf->data.ival 
					*/
					printf("ldc %d\n", leaf->data.ival);
					break;

				case TN_REAL:
					/* Constant case */
					/*
					*	In order to get the real value you have to use:
					*	leaf->data.dval
					*/
					printf("ldc %f\n",leaf->data.dval);
					break;
			}
			break;

		case IF_T:
			ifn = (if_node *) root;
            int loop_level;
            switch (ifn->hdr.type) {

			case TN_IF:
				if (ifn->else_n == NULL) {
                    loop_level = root->hdr.c_contxt->syms->clevel;
                    /* if case (without else)*/
					code_recur(ifn->cond);
					printf("fjp end_if_%i\n",loop_level);
					code_recur(ifn->then_n);
					printf("end_if_%i:\n",loop_level);
				}
				else {
                    loop_level = root->hdr.c_contxt->syms->clevel;
					/* if - else case*/ 
					code_recur(ifn->cond);
					printf("fjp else_%i\n",loop_level);
					code_recur(ifn->then_n);
					printf("ujp end_if_%i\n",loop_level);
					printf("else_%i:\n",loop_level);
					code_recur(ifn->else_n);
					printf("end_if_%i:\n",loop_level);
				}
				break;
				
			case TN_COND_EXPR:
                loop_level = root->hdr.c_contxt->syms->clevel;
				/* (cond)?(exp):(exp); */
				code_recur(ifn->cond);
				printf("fjp else_%i\n",loop_level);
				code_recur(ifn->then_n);
				printf("ujp end_cond_%i\n",loop_level);
				printf("else_%i:\n",loop_level);
				code_recur(ifn->else_n);
				printf("end_cond_%i:\n",loop_level);
				break;

			default:
				/* Maybe you will use it later */
				code_recur(ifn->cond);
				code_recur(ifn->then_n);
				code_recur(ifn->else_n);
			}
			break;

		case FOR_T:
			forn = (for_node *) root;
			switch (forn->hdr.type) {

			case TN_FUNC_DEF:
				/* Function definition */
				/* e.g. int main(...) { ... } */
				/* Look at the output AST structure! */
				code_recur(forn->init);
				code_recur(forn->test);
				code_recur(forn->incr);
				code_recur(forn->stemnt);
				break;

			case TN_FOR:
				/* For case*/
				/* e.g. for(i=0;i<5;i++) { ... } */
				/* Look at the output AST structure! */
                loop_level = root->hdr.c_contxt->syms->clevel;
				code_recur(forn->init);
				printf("for_loop_%i:\n",loop_level);
				code_recur(forn->test);
				printf("fjp end_for_%i\n",loop_level);
				code_recur(forn->stemnt);
				code_recur(forn->incr);
                printf("ujp for_loop_%i\n",loop_level);
                printf("end_for_%i:\n",loop_level);
				break;

			default:
				/* Maybe you will use it later */
				code_recur(forn->init);
				code_recur(forn->test);
				code_recur(forn->stemnt);
				code_recur(forn->incr);
			}
			break;

		case NODE_T:
			switch (root->hdr.type) {
				case TN_PARBLOCK:
					/* Maybe you will use it later */
					code_recur(root->lnode);
					code_recur(root->rnode);
					break;
				
				case TN_PARBLOCK_EMPTY:
					/* Maybe you will use it later */
					code_recur(root->lnode);
					code_recur(root->rnode);
					break;
					
				case TN_TRANS_LIST:
					/* Maybe you will use it later */
					code_recur(root->lnode);
					code_recur(root->rnode);
					break;

				case TN_FUNC_DECL:
					/* Maybe you will use it later */
					code_recur(root->lnode);
					code_recur(root->rnode);
					break;

				case TN_FUNC_CALL:
					/* Function call */
					if (strcmp(((leafnode*)root->lnode)->data.sval->str, "printf") == 0) {
						/* printf case */
						/* The expression that you need to print is located in */
						/* the currentNode->right->right sub tree */
						/* Look at the output AST structure! */
						isLoadingVariable = 1;
						code_recur(root->rnode->rnode);
						isLoadingVariable = 0;
						printf("print\n");
					}
					else {
						/* other function calls - for HW3 */
						code_recur(root->lnode);
						code_recur(root->rnode);
					}
					break;

				case TN_BLOCK:
					/* Maybe you will use it later */
					code_recur(root->lnode);
					code_recur(root->rnode);
					break;

				case TN_ARRAY_DECL:
					/* array declaration - for HW2 */
					code_recur(root->lnode);
					code_recur(root->rnode);
					break;

				case TN_EXPR_LIST:
					/* Maybe you will use it later */
					code_recur(root->lnode);
					code_recur(root->rnode);
					break;

				case TN_NAME_LIST:
					/* Maybe you will use it later */
					code_recur(root->lnode);
					code_recur(root->rnode);
					break;

				case TN_ENUM_LIST:
					/* Maybe you will use it later */
					code_recur(root->lnode);
					code_recur(root->rnode);
					break;

				case TN_FIELD_LIST:
					/* Maybe you will use it later */
					code_recur(root->lnode);
					code_recur(root->rnode);
					break;

				case TN_PARAM_LIST:
					/* Maybe you will use it later */
					code_recur(root->lnode);
					code_recur(root->rnode);
					break;

				case TN_IDENT_LIST:
					/* Maybe you will use it later */
					code_recur(root->lnode);
					code_recur(root->rnode);
					break;

				case TN_TYPE_LIST:
					/* Maybe you will use it later */
					code_recur(root->lnode);
					code_recur(root->rnode);
					break;

				case TN_COMP_DECL:
					/* struct component declaration - for HW2 */
					code_recur(root->lnode);
					code_recur(root->rnode);
					break;
					
				case TN_DECL:
                    tmp = (Symbol_table*)malloc(sizeof(Symbol_table));
                    code_recur(root->lnode);
                    code_recur(root->rnode);
                    leaf =(leafnode*)root->rnode;
                    tmp->name = leaf->data.sval->str;
                    leaf = (leafnode*)root->lnode->lnode;
                    if (leaf->hdr.tok == INT)
                    {
                        tmp->size = sizeof(int);
                        tmp->type = "int";
                    }

                    if (leaf->hdr.tok == DOUBLE)
                    {
                        tmp->size = sizeof(double);
                        tmp->type = "double";
                    }

                    if (leaf->hdr.tok == FLOAT)
                    {
                        tmp->size = sizeof(float);
                        tmp->type = "float";
                    }

                    Add(tmp);
                   // printf("ldc %d\n", tmp->address);
                    break;

				case TN_DECL_LIST:
					/* Maybe you will use it later */
					code_recur(root->lnode);
					code_recur(root->rnode);
					break;

				case TN_DECLS:
					/* Maybe you will use it later */
					code_recur(root->lnode);
					code_recur(root->rnode);
					break;

				case TN_STEMNT_LIST:
					/* Maybe you will use it later */
					code_recur(root->lnode);
					code_recur(root->rnode);
					break;

				case TN_STEMNT:
					/* Maybe you will use it later */
					code_recur(root->lnode);
					code_recur(root->rnode);
					break;

				case TN_BIT_FIELD:
					/* Maybe you will use it later */
					code_recur(root->lnode);
					code_recur(root->rnode);
					break;

				case TN_PNTR:
					/* pointer - for HW2! */
					code_recur(root->lnode);
					code_recur(root->rnode);
					break;

				case TN_TYPE_NME:
					/* Maybe you will use it later */
					code_recur(root->lnode);
					code_recur(root->rnode);
					break;

				case TN_INIT_LIST:
					/* Maybe you will use it later */
					code_recur(root->lnode);
					code_recur(root->rnode);
					break;

				case TN_INIT_BLK:
					/* Maybe you will use it later */
					code_recur(root->lnode);
					code_recur(root->rnode);
					break;

				case TN_OBJ_DEF:
					/* Maybe you will use it later */
					code_recur(root->lnode);
					code_recur(root->rnode);
					break;

				case TN_OBJ_REF:
					/* Maybe you will use it later */
					code_recur(root->lnode);
					code_recur(root->rnode);
					break;

				case TN_CAST:
					/* Maybe you will use it later */
					code_recur(root->lnode);
					code_recur(root->rnode);
					break;

				case TN_JUMP:
					if (root->hdr.tok == RETURN) {
						/* return jump - for HW2! */
						code_recur(root->lnode);
						code_recur(root->rnode);
					}
					else if (root->hdr.tok == BREAK) {
						/* break jump - for HW2! */
						code_recur(root->lnode);
						code_recur(root->rnode);
					}
					else if (root->hdr.tok == GOTO) {
						/* GOTO jump - for HW2! */
						code_recur(root->lnode);
						code_recur(root->rnode);
					}
					break;

				case TN_SWITCH:
					/* Switch case - for HW2! */
					code_recur(root->lnode);
					code_recur(root->rnode);
					break;

				case TN_INDEX: 
					/* call for array - for HW2! */
					code_recur(root->lnode);
					code_recur(root->rnode);
					break;

				case TN_DEREF:
					/* pointer derefrence - for HW2! */
					code_recur(root->lnode);
					code_recur(root->rnode);
					break;

				case TN_SELECT:
					/* struct case - for HW2! */
					if (root->hdr.tok == ARROW){
						/* Struct select case "->" */
						/* e.g. struct_variable->x; */
						code_recur(root->lnode);
						code_recur(root->rnode);
					}
					else{
						/* Struct select case "." */
						/* e.g. struct_variable.x; */
						code_recur(root->lnode);
						code_recur(root->rnode);
					}
					break;

				case TN_ASSIGN:
					if(root->hdr.tok == EQ){
						/* Regular assignment "=" */
						/* e.g. x = 5; */
						code_recur(root->lnode);
						isLoadingVariable = 1;
						code_recur(root->rnode);
						isLoadingVariable = 0;
						printf("sto\n");
					}
					else if (root->hdr.tok == PLUS_EQ){
						/* Plus equal assignment "+=" */
						/* e.g. x += 5; */
						code_recur(root->lnode);
						isLoadingVariable = 1;
						code_recur(root->rnode);
						isLoadingVariable = 0;
						printf("inc 1\n");
						printf("sto\n");
					}
					else if (root->hdr.tok == MINUS_EQ){
						/* Minus equal assignment "-=" */
						/* e.g. x -= 5; */
						code_recur(root->lnode);
						isLoadingVariable = 1;
						code_recur(root->rnode);
						isLoadingVariable = 0;
						printf("dec 1\n");
						printf("sto\n");
					
					}
					else if (root->hdr.tok == STAR_EQ){
						/* Multiply equal assignment "*=" */
						/* e.g. x *= 5; */
						code_recur(root->lnode);
						isLoadingVariable = 1;
						code_recur(root->rnode);
						isLoadingVariable = 0;
						printf("mul\n");
						printf("sto\n");
					
					}
					else if (root->hdr.tok == DIV_EQ){
						/* Divide equal assignment "/=" */
						/* e.g. x /= 5; */
						code_recur(root->lnode);
						isLoadingVariable = 1;
						code_recur(root->rnode);
						isLoadingVariable = 0;
						printf("div\n");
						printf("sto\n");
					
					}
					break;

				case TN_EXPR:
					switch (root->hdr.tok) {
					  case CASE:
					      /* you should not get here */
						  code_recur(root->lnode);
						  code_recur(root->rnode);
						  break;

					  case INCR:
						  /* Increment token "++" */
                          leaf = (leafnode*)root->lnode;
                          if( leaf != NULL) {
                              if (leaf->hdr.type == TN_IDENT) {
                                  printf("ldc %d\n", Find(leaf->data.sval->str));
                              }
                          }
                          leaf = (leafnode*)root->rnode;
                          if (leaf !=NULL)
                          {
                              if (leaf->hdr.type == TN_IDENT) {
                                    printf("ldc %d\n", Find(leaf->data.sval->str));
                              }
                          }
						  isLoadingVariable = 1;
						  code_recur(root->lnode);
						  code_recur(root->rnode);
						  isLoadingVariable = 0;
						  printf("inc 1\n");
                          printf("sto\n");
						  break;

					  case DECR:
						  /* Decrement token "--" */
                          leaf = (leafnode*)root->lnode;
                          if( leaf != NULL)
                          {
                              if (leaf->hdr.type == TN_IDENT) {
                                  printf("ldc %d\n", Find(leaf->data.sval->str));
                              }
                          }
                          leaf = (leafnode*)root->rnode;
                          if (leaf !=NULL)
                          {
                              if (leaf->hdr.type == TN_IDENT) {
                                  printf("ldc %d\n", Find(leaf->data.sval->str));
                              }
                          }
						  isLoadingVariable = 1;
						  code_recur(root->lnode);
						  code_recur(root->rnode);
						  isLoadingVariable = 0;
						  printf("dec 1\n");
						  printf("sto\n");
						  break;

					  case PLUS:
					  	  /* Plus token "+" */
						  code_recur(root->lnode);
						  code_recur(root->rnode);
						  printf("add\n");
						  break;

					  case MINUS:
					  	  /* Minus token "-" */
						  code_recur(root->lnode);
						  code_recur(root->rnode);
						  leaf = (leafnode*)root->lnode;

						  if (leaf != NULL)
                          {
                              printf("sub\n");
						  }

						  else
                          {
						      printf("neg\n");
                          }

						  break;

					  case DIV:
					  	  /* Divide token "/" */
						  code_recur(root->lnode);
						  code_recur(root->rnode);
						  printf("div\n");
						  break;

					  case STAR:
					  	  /* multiply token "*" */
						  code_recur(root->lnode);
						  code_recur(root->rnode);
						  printf("mul\n");
						  break;

					  case AND:
					  	  /* And token "&&" */
					  	  isLoadingVariable = 1;
						  code_recur(root->lnode);
						  code_recur(root->rnode);
						  isLoadingVariable = 0;
						  printf("and\n");
						  break;

					  case OR:
					  	  /* Or token "||" */
					  	  isLoadingVariable = 1;
						  code_recur(root->lnode);
						  code_recur(root->rnode);
						  isLoadingVariable = 0;
						  printf("or\n");
						  break;
						
					  case NOT:
					  	  /* Not token "!" */
					  	  isLoadingVariable = 1;
						  code_recur(root->lnode);
						  code_recur(root->rnode);
						  isLoadingVariable = 0;
						  printf("not\n");
						  break;

					  case GRTR:
					  	  /* Greater token ">" */
					  	  isLoadingVariable = 1;
						  code_recur(root->lnode);
						  code_recur(root->rnode);
						  isLoadingVariable = 0;
						  printf("grt\n");
						  break;

					  case LESS:
					  	  /* Less token "<" */
                          isLoadingVariable = 1;
                          code_recur(root->lnode);
                          code_recur(root->rnode);
                          isLoadingVariable = 0;
						  printf("les\n");
						  break;
						  
					  case EQUAL:
					  	  /* Equal token "==" */
					  	  isLoadingVariable = 1;
						  code_recur(root->lnode);
						  code_recur(root->rnode);
						  isLoadingVariable = 0;
						  printf("equ\n");
						  break;

					  case NOT_EQ:
					  	  /* Not equal token "!=" */
					  	  isLoadingVariable = 1;
						  code_recur(root->lnode);
						  code_recur(root->rnode);
						  isLoadingVariable = 0;
						  printf("neq\n");
						  break;

					  case LESS_EQ:
					  	  /* Less or equal token "<=" */
					  	  isLoadingVariable = 1;
						  code_recur(root->lnode);
						  code_recur(root->rnode);
						  isLoadingVariable = 0;
						  printf("leq\n");
						  break;

					  case GRTR_EQ:
					  	  /* Greater or equal token ">=" */
					  	  isLoadingVariable = 1;
						  code_recur(root->lnode);
						  code_recur(root->rnode);
						  isLoadingVariable = 0;
						  printf("geq\n");
						  break;

					  default:
						code_recur(root->lnode);
						code_recur(root->rnode);
						break;
					}
					break;


                case TN_WHILE:
                    loop_level = root->hdr.c_contxt->syms->clevel;
				    printf("while_loop_%i:\n",loop_level);
                    code_recur(root->lnode);
                    printf("fjp end_loop_%i\n",loop_level);
                    code_recur(root->rnode);
                    printf("ujp while_loop_%i\n",loop_level);
                    printf("end_loop_%i:\n",loop_level);
					break;

				case TN_DOWHILE:
                    /* Do-While case */
                    loop_level = root->hdr.c_contxt->syms->clevel;
                    printf("do_while_loop_%i:\n",loop_level);
                    code_recur(root->rnode);
                    code_recur(root->lnode);
                    printf("fjp end_loop_%i\n",loop_level);
                    printf("ujp do_while_loop_%i\n",loop_level);
                    printf("end_loop_%i:\n",loop_level);
                    break;

				case TN_LABEL:
					code_recur(root->lnode);
					code_recur(root->rnode);
					break;

				default:
					code_recur(root->lnode);
					code_recur(root->rnode);
			}
			break;

		case NONE_T:
			printf("Error: Unknown node type!\n");
			exit(FAILURE);	
    }

    return SUCCESS;
}


/*
*	This function prints all the variables on your symbol table with their data
*	Input: treenode (AST)
*	Output: prints the Symbol Table on the console
*/
void print_symbol_table(treenode *root) {
	printf("---------------------------------------\n");
	printf("Showing the Symbol Table:\n");
	/*
	*	add your code here
	*/
}