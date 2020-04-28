#include	"CodeGenerator.h"
#include "tree.h"

typedef struct array_table {
    int typeSize;
    int dimensions;
    int* dimSize;
}Array_table;

typedef struct symbol_table {
	char* name;
    char* type;
    int address;
    int varSize;
    Array_table* arrayTable;
    int isArray;
} Symbol_table;

typedef struct variable {

	Symbol_table* table;
    struct variable* next;
    struct variable* prev;
    struct variable* first;
    struct variable* last;

} Variable;

typedef struct Struct_table{
    int totalSize;
    struct variable* Data;
    char* name;
    struct Struct_table* next;
}Struct_table;

char* tmpName;
char* tmpType;
int tmpSize;
int tmpArraySize;
Variable *head;
Struct_table* startHead;
int isLoadingVariable = 0;
int loop_level = 0;
int tmpArrayDim = 0;
int* tmpArrayDimSizes = NULL;
int arrayCounter = 0;
Symbol_table *pVariable = NULL;
int isPointer = 0;
int tmpIsArray = 0;
int isAssaigment = 0;
int tmpPointerSize = 0;
int isDereferance = 0;
int derefLoop = 0;
int isComponent = 0;
int isObjDef = 0;
char* objDefName;
int objSize = 0;
int isDeclaration = 0;
int isObjRef = 0;
int scope_counter = 0;
int prev_scope;
int isAccessStruct = 0;
int enterLeftSon = 0;
char* structIdent;
int isStructSelector = 0;
int isINCRDECR = 0;

Symbol_table* Find (const char* name, Variable* start)
{
    Variable* currentPtr = start;
    while (currentPtr != NULL)
    {
        if(currentPtr->table->name == name)
        {
            return currentPtr->table;
        }
        currentPtr = currentPtr->next;
    }
    return NULL;
}

Struct_table * FindStruct(const char* name)
{
    Struct_table* ptr = startHead;
    while (ptr != NULL)
    {
        if (ptr->name == name)
        {
            return ptr;
        }
        ptr = ptr->next;
    }
}
Symbol_table* FindByType (const char* type)
{
    Variable* currentPtr = head;
    while (currentPtr != NULL)
    {
        if(currentPtr->table->type == type)
        {
            return currentPtr->table;
        }
        currentPtr = currentPtr->next;
    }
    return NULL;
}

/*Adding data to the Symbol table*/
void Add (Symbol_table * newSymbol, Variable* start)
{
    Variable *ptr = start;
    // Not our first variable
    if (start != NULL)
    {
        Variable *newVariable = (Variable *) malloc(sizeof(Variable));
        while (ptr->next != NULL)
        {
            ptr->last = newVariable;
            ptr = ptr->next;
        }

        if(isComponent)
        {
            newSymbol->address = ptr->table->address + 1;
        }
        else{
            newSymbol->address = ptr->table->address + ptr->table->varSize;
        }
      
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
        start = (Variable*)malloc(sizeof(Variable));
        if(isComponent)
        {
            newSymbol->address = 1;
        } else{
            newSymbol->address = 5;
        }
        start->table = newSymbol;
        start->next = NULL;
        start->prev = NULL;
        start->last = start;
        start->first = start;
        if (isComponent)
        {
            startHead->Data = start;
        } else{
            head = start;
        }

    }
}
int GetSizeByType(char* type)
{
    Struct_table *tmp;
    if ((strcmp(type,  "int") == 0)||(strcmp(type,"double") == 0)||(strcmp(type, "float") == 0))
    {
        return 1;
    } else{
        tmp = FindStruct(type);
        return tmp->totalSize;
    }

}
void AddStruct(Struct_table* newStruct)
{
    if (startHead == NULL)
    {
        startHead = (Struct_table*)malloc(sizeof(Struct_table));
        startHead = newStruct;
        startHead->next = NULL;
        startHead->Data = NULL;
    }

    else
    {
        Struct_table* ptr = startHead;
        while(ptr->next != NULL)
        {
            ptr = ptr->next;
        }

        ptr->next = newStruct;
        newStruct->next = NULL;
        newStruct->Data = NULL;
    }
}

//Removing a variable from the Symbol table
void Remove(char* name)
{
   Variable *ptr = head;
    Symbol_table *tmpTable;
    while (ptr->next != NULL)
    {
        tmpTable = ptr->table;
        if (tmpTable->name == name)
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

void FreeSymbolTableMemory(Variable* start)
{
    Variable* ptr = start;
    Symbol_table* tmpTable;
    Variable* tmp;
    while (ptr->next != NULL)
    {
        if (ptr->table->isArray == 1)
        {
            free(ptr->table->arrayTable->dimSize);
        }
        tmp = ptr;
        ptr = ptr->next;

        free(tmp->table->arrayTable);
        free(tmp->table);
        free(tmp);
    }

    tmpTable = ptr->table;
    if (tmpTable->isArray == 1)
    {
        free(tmpTable->arrayTable->dimSize);
    }
    free(tmpTable->arrayTable);
    free(tmpTable);
    free(ptr);
    free(start);
}
void FreeStructs(Struct_table* start)
{
    Struct_table* ptr = start;
    Struct_table* tmp;
    if (ptr == NULL)
    {
        return;
    }
    while (ptr->next != NULL)
    {
        FreeSymbolTableMemory(ptr->Data);
        tmp = ptr;
        ptr = ptr->next;
        free(tmp);
    }
    if(ptr != NULL)
    {
        FreeSymbolTableMemory(ptr->Data);
        free(ptr->Data);
        free(ptr);
    }
    if (start != NULL)
    {
        free(start);
    }
}
void ClearData()
{
    tmpSize = 0;
    tmpName = "";
    tmpType = "";
    tmpPointerSize = 0;
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
int  code_recur(treenode *root) {
    if_node *ifn;
    for_node *forn;
    leafnode *leaf;

    if (!root)
        return SUCCESS;

    switch (root->hdr.which) {
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
                    if (!isDeclaration) {
                        if (isAccessStruct) {
                            //tmpType hold us the struct name;
                            pVariable = Find(leaf->data.sval->str, FindStruct(tmpType)->Data);
                            printf("inc %d\n", pVariable->address - 1);
                            if (isLoadingVariable){
                               if(!isINCRDECR)
                                {
                                    printf("ind\n");
                                }

                            }
                            if (isDereferance) {
                                derefLoop = tmpPointerSize;
                                while (derefLoop > 0) {
                                    printf("ind\n");
                                    derefLoop--;
                                }
                            }
                            } else {
                                pVariable = Find(leaf->data.sval->str, head);
                                if (pVariable != NULL) {
                                    printf("ldc %d\n", pVariable->address);
                                    if ((isLoadingVariable)&&(!enterLeftSon)) {
                                        printf("ind\n");
                                    }
                                    if (isDereferance) {
                                        derefLoop = tmpPointerSize;
                                        while (derefLoop > 0) {
                                            printf("ind\n");
                                            derefLoop--;
                                        }
                                    }
                                    if (isStructSelector)
                                    {
                                        structIdent = strcat(leaf->data.sval->str, "\0");
                                    }
                                }
                            }
                        } else {
                            if (isObjDef) {
                                objDefName = strcat(leaf->data.sval->str, "\0");
                            } else if (isObjRef) {
                                tmpType = strcat(leaf->data.sval->str, "\0");
                                tmpSize = FindStruct(leaf->data.sval->str)->totalSize;
                            } else {
                                tmpName = strcat(leaf->data.sval->str, "\0");
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
                            if (leaf->hdr.tok == INT) {
                                tmpType = "int";
                                tmpSize = 1;
                            } else if (leaf->hdr.tok == FLOAT) {
                                tmpType = "float";
                                tmpSize = 1;
                            } else if (leaf->hdr.tok == DOUBLE) {
                                tmpType = "double";
                                tmpSize = 1;
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
                            printf("ldc %f\n", leaf->data.dval);
                        break;
                    }
                    break;

                case IF_T:
                    ifn = (if_node *) root;
                    prev_scope = loop_level;
                    loop_level = scope_counter++;
                    switch (ifn->hdr.type) {

                        case TN_IF:
                            if (ifn->else_n == NULL) {
                                /* if case (without else)*/
                                code_recur(ifn->cond);
                                printf("fjp end_%i\n", loop_level);
                                code_recur(ifn->then_n);
                                printf("end_%i:\n", loop_level);
                            } else {
                                //loop_level = root->hdr.c_contxt->syms->clevel;
                                /* if - else case*/
                                code_recur(ifn->cond);
                                printf("fjp else_%i\n", loop_level);
                                code_recur(ifn->then_n);
                                printf("ujp end_%i\n", loop_level);
                                printf("else_%i:\n", loop_level);
                                code_recur(ifn->else_n);
                                printf("end_%i:\n", loop_level);
                            }
                            break;

                        case TN_COND_EXPR:
//                loop_level = root->hdr.c_contxt->syms->clevel;
                            /* (cond)?(exp):(exp); */
                            code_recur(ifn->cond);
                            printf("fjp else_%i\n", loop_level);
                            code_recur(ifn->then_n);
                            printf("ujp end_%i\n", loop_level);
                            printf("else_%i:\n", loop_level);
                            code_recur(ifn->else_n);
                            printf("end_%i:\n", loop_level);
                            break;

                        default:
                            /* Maybe you will use it later */
                            code_recur(ifn->cond);
                            code_recur(ifn->then_n);
                            code_recur(ifn->else_n);
                    }
                    loop_level = prev_scope;
                    break;

                case FOR_T:
                    prev_scope = loop_level;
                    loop_level = scope_counter++;
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
                            //loop_level = root->hdr.c_contxt->syms->clevel;
                            code_recur(forn->init);
                            printf("for_loop_%i:\n", loop_level);
                            code_recur(forn->test);
                            printf("fjp end_%i\n", loop_level);
                            code_recur(forn->stemnt);
                            code_recur(forn->incr);
                            printf("ujp for_loop_%i\n", loop_level);
                            printf("end_%i:\n", loop_level);
                            break;

                        default:
                            /* Maybe you will use it later */
                            code_recur(forn->init);
                            code_recur(forn->test);
                            code_recur(forn->stemnt);
                            code_recur(forn->incr);
                    }
                    loop_level = prev_scope;
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
                            //First stop in Struct
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
                            if (strcmp(((leafnode *) root->lnode)->data.sval->str, "printf") == 0) {
                                /* printf case */
                                /* The expression that you need to print is located in */
                                /* the currentNode->right->right sub tree */
                                /* Look at the output AST structure! */
                                isLoadingVariable = 1;
                                code_recur(root->rnode->rnode);
                                isLoadingVariable = 0;
                                printf("print\n");
                            } else {
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
                            tmpArrayDim++;
                            code_recur(root->lnode);
                            if (tmpArrayDimSizes == NULL) {
                                tmpArrayDimSizes = (int *) malloc(tmpArrayDim * sizeof(int));
                            }
                            if (arrayCounter == 0) {
                                arrayCounter = tmpArrayDim;
                            }
                            leaf = (leafnode *) root->rnode;
                            tmpArraySize = leaf->data.ival;
                            tmpSize = tmpArraySize * tmpSize;
                            tmpArrayDimSizes[arrayCounter - 1] = leaf->data.ival;
                            arrayCounter--;
                            // We don't want to enter the right node.
                            //After here we are getting the varSize of the Array and it will be held in tmpSize
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
                            code_recur(root->lnode);
                            code_recur(root->rnode);
                            break;

                        case TN_COMP_DECL:
                            /* struct component declaration - for HW2 */
                            isComponent = 1;
                            isDeclaration = 1;
                            code_recur(root->lnode);
                            code_recur(root->rnode);
                            Symbol_table *tmpCompTable = (Symbol_table *) malloc(sizeof(Symbol_table));
                            tmpCompTable->name = tmpName;
                            tmpCompTable->type = tmpType;
                            pVariable = Find(tmpType, head);
                            if (pVariable != NULL) {
                                leaf = (leafnode *) root->rnode->lnode;
                                if (tmpType != pVariable->name) {
                                    if (leaf->hdr.type == TN_PNTR) {
                                        tmpSize = 1;
                                    } else {
                                        tmpSize = pVariable->varSize;
                                    }
                                } else {
                                    tmpSize = 1;
                                }
                            }

                            tmpCompTable->varSize = tmpSize;
                            if (tmpSize == 0) {
                                objSize += 1;
                            } else {
                                objSize += tmpSize;
                            }

                            leaf = (leafnode *) root->rnode;
                            if (leaf != NULL) {
                                if (leaf->hdr.type == TN_ARRAY_DECL) {
                                    tmpCompTable->arrayTable = (Array_table *) malloc(sizeof(Array_table));
                                    tmpCompTable->arrayTable->dimensions = tmpArrayDim;
                                    tmpCompTable->arrayTable->dimSize = tmpArrayDimSizes;
                                    tmpCompTable->arrayTable->typeSize = GetSizeByType(tmpCompTable->type);
                                    tmpIsArray = 1;
                                    tmpArrayDim = 0;
                                    tmpArrayDimSizes = NULL;
                                    arrayCounter = 0;
                                }
                            }
                            tmpCompTable->isArray = tmpIsArray;
                            tmpIsArray = 0;
                            if (!isPointer) {
                                Add(tmpCompTable, FindStruct(objDefName)->Data);
                                ClearData();
                            } else {
                                isPointer = 0;
                            }
                            isComponent = 0;
                            isDeclaration = 0;
                            //After here we have a property of the struct for example int c;
                            break;

                        case TN_DECL:
                            //2nd stop in struct
                            isDeclaration = 1;
                            code_recur(root->lnode);
                            code_recur(root->rnode);
                            if (tmpName == "") {
                                break;
                            }
                            Symbol_table *tmpSymbolTable = (Symbol_table *) malloc(sizeof(Symbol_table));
                            tmpSymbolTable->name = tmpName;
                            tmpSymbolTable->type = tmpType;
                            tmpSymbolTable->varSize = tmpSize;
                            leaf = (leafnode *) root->rnode;
                            if (leaf != NULL) {
                                if (leaf->hdr.type == TN_ARRAY_DECL) {
                                    tmpSymbolTable->arrayTable = (Array_table *) malloc(sizeof(Array_table));
                                    tmpSymbolTable->arrayTable->dimensions = tmpArrayDim;
                                    tmpSymbolTable->arrayTable->dimSize = tmpArrayDimSizes;
                                    tmpSymbolTable->arrayTable->typeSize = GetSizeByType(tmpSymbolTable->type);
                                    tmpIsArray = 1;
                                    tmpArrayDim = 0;
                                    tmpArrayDimSizes = NULL;
                                    arrayCounter = 0;
                                }
                            }
                            tmpSymbolTable->isArray = tmpIsArray;
                            tmpIsArray = 0;
                            if (!isPointer) {
                                Add(tmpSymbolTable, head);
                                ClearData();
                            } else {
                                isPointer = 0;
                            }
                            isDeclaration = 0;
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
                            isPointer = 1;
                            tmpPointerSize++;
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
                            isObjDef = 1;
                            code_recur(root->lnode);
                            isObjDef = 0;
                            Struct_table *tmpTable = (Struct_table *) malloc(sizeof(Struct_table));
                            Symbol_table *tmpSymTable = (Symbol_table *) malloc(sizeof(Symbol_table));
                            tmpSymTable->name = objDefName;
                            tmpSymTable->varSize = -1;
                            tmpSymTable->type = "Struct";
                            tmpSymTable->isArray = 0;
                            tmpTable->name = objDefName;
                            AddStruct(tmpTable);
                            Add(tmpSymTable, head);
                            code_recur(root->rnode);
                            tmpTable->totalSize = objSize;
                            tmpSymTable->varSize = objSize;
                            break;

                        case TN_OBJ_REF:
                            /* Maybe you will use it later */
                            isObjRef = 1;
                            code_recur(root->lnode);
                            code_recur(root->rnode);
                            isObjRef = 0;
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
                                // In normal case we can't release memory here
                                FreeSymbolTableMemory(head);
                                FreeStructs(startHead);
//                                FreeSymbolTableMemory(startHead);
                               // printf("ujp end_%i\n", prev_scope);
                            } else if (root->hdr.tok == BREAK) {
                                /* break jump - for HW2! */
                                code_recur(root->lnode);
                                code_recur(root->rnode);
                                printf("ujp end_%i\n", prev_scope);
                            } else if (root->hdr.tok == GOTO) {
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
                            if (arrayCounter != 0) {
                                arrayCounter = 0;
                            }
                            arrayCounter++;
                            code_recur(root->lnode); //This will eventually print the address
                            //We done going down the most possible when lnode is identifier
                            leaf = (leafnode *) root->lnode;
                            if (leaf->hdr.type == TN_IDENT) {
                                if (isAccessStruct) {
//                            pVariable = Find(leaf->data.sval->str, startHead);
                                } else {
                                    pVariable = Find(leaf->data.sval->str, head);
                                }
                            }

                            code_recur(root->rnode); //This will print us the index => ldc index
                            if (arrayCounter > 0) {
                                printf("ixa %d\n",
                                       pVariable->arrayTable->dimSize[arrayCounter - 1] *
                                       pVariable->arrayTable->typeSize);
                                arrayCounter--;
                            } else {
                                printf("ixa %d\n", pVariable->arrayTable->typeSize);
                            }


                            break;

                        case TN_DEREF:
                            /* pointer derefrence - for HW2! */
                            isDereferance = 1;
                            tmpPointerSize++;
                            code_recur(root->lnode);
                            code_recur(root->rnode);
                            isDereferance = 0;
                            break;

                        case TN_SELECT:
                            isStructSelector = 1;
                            /* struct case - for HW2! */
                            if (root->hdr.tok == ARROW) {
                                /* Struct select case "->" */
                                /* e.g. struct_variable->x; */
                                enterLeftSon = 1;
                                code_recur(root->lnode);
                                enterLeftSon = 0;
                                isAccessStruct = 1;
                                leaf = (leafnode *) root->lnode;
                                if (leaf->hdr.type == TN_INDEX)
                                {
                                    tmpType = Find(structIdent, head)->type;
                                } else
                                {
                                    tmpType = Find(leaf->data.sval->str, head)->type;
                                }
                                code_recur(root->rnode);

                            } else {
                                /* Struct select case "." */
                                /* e.g. struct_variable.x; */
                                enterLeftSon = 1;
                                code_recur(root->lnode);
                                enterLeftSon = 0;
                                isAccessStruct = 1;
                                leaf = (leafnode *) root->lnode;
                                if (leaf->hdr.type == TN_INDEX)
                                {
                                    tmpType = Find(structIdent, head)->type;
                                } else
                                {
                                    tmpType = Find(leaf->data.sval->str, head)->type;
                                }
                                code_recur(root->rnode);

                            }
                            tmpType = "";
                            structIdent = "";
                            isAccessStruct = 0;
                            isStructSelector = 0;
                            break;

                        case TN_ASSIGN:
                            isAssaigment = 1;
                            if (root->hdr.tok == EQ) {
                                /* Regular assignment "=" */
                                /* e.g. x = 5; */

                                code_recur(root->lnode); //Going to left son we only need to load the address here.
                                isLoadingVariable =1;
                                code_recur(root->rnode);
                                isLoadingVariable = 0;
                                printf("sto\n");
                                isPointer = 0;
                            } else if (root->hdr.tok == PLUS_EQ) {
                                /* Plus equal assignment "+=" */
                                /* e.g. x += 5; */
                                code_recur(root->lnode);
                                isLoadingVariable = 1;
                                code_recur(root->rnode);
                                isLoadingVariable = 0;
                                printf("inc 1\n");
                                printf("sto\n");
                            } else if (root->hdr.tok == MINUS_EQ) {
                                /* Minus equal assignment "-=" */
                                /* e.g. x -= 5; */
                                code_recur(root->lnode);
                                isLoadingVariable = 1;
                                code_recur(root->rnode);
                                isLoadingVariable = 0;
                                printf("dec 1\n");
                                printf("sto\n");

                            } else if (root->hdr.tok == STAR_EQ) {
                                /* Multiply equal assignment "*=" */
                                /* e.g. x *= 5; */
                                code_recur(root->lnode);
                                isLoadingVariable = 1;
                                code_recur(root->rnode);
                                isLoadingVariable = 0;
                                printf("mul\n");
                                printf("sto\n");

                            } else if (root->hdr.tok == DIV_EQ) {
                                /* Divide equal assignment "/=" */
                                /* e.g. x /= 5; */
                                code_recur(root->lnode);
                                isLoadingVariable = 1;
                                code_recur(root->rnode);
                                isLoadingVariable = 0;
                                printf("div\n");
                                printf("sto\n");

                            }
                            isPointer = 0;
                            isAssaigment = 0;
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
                                    isINCRDECR = 1;
                                    //Test above
                                    leaf = (leafnode *) root->lnode;
                                    if (leaf != NULL) {
//                              printf("ldc %d\n", Find(leaf->data.sval->str)->address);
                                        code_recur(root->lnode);
                                        isLoadingVariable = 1;
                                        code_recur(root->lnode);
                                        //printf("ind\n");
                                        printf("inc 1\n");
                                        printf("sto\n");
                                        if (isAssaigment) {
                                            code_recur(root->lnode);
                                        }
                                        isLoadingVariable = 0;
                                    }
                                    leaf = (leafnode *) root->rnode;
                                    if (leaf != NULL) {
                                        if (isAssaigment) {
                                            isLoadingVariable = 1;
                                            code_recur(root->rnode);
                                            isLoadingVariable = 0;
                                            printf("sto\n");
                                        }
                                        code_recur(root->rnode);
                                        isLoadingVariable = 1;
                                        code_recur(root->rnode);
                                        isLoadingVariable = 0;
                                        printf("inc 1\n");
                                        printf("sto\n");

                                    }
                                    isINCRDECR = 0;
                                    break;

                                case DECR:
                                    isINCRDECR =1;
                                    /* Decrement token "--" */
                                    leaf = (leafnode *) root->lnode;
                                    if (leaf != NULL) {
//                              printf("ldc %d\n", Find(leaf->data.sval->str)->address);
                                        code_recur(root->lnode);
                                        isLoadingVariable = 1;
                                        code_recur(root->lnode);
                                        printf("dec 1\n");
                                        printf("sto\n");
                                        if (isAssaigment) {
                                            code_recur(root->lnode);
                                        }
                                        isLoadingVariable = 0;
                                    }
                                    leaf = (leafnode *) root->rnode;
                                    if (leaf != NULL) {

                                        if (isAssaigment) {
                                            isLoadingVariable = 1;
                                            code_recur(root->rnode);
                                            isLoadingVariable = 0;
                                            printf("sto\n");
                                        }
                                        code_recur(root->rnode);
                                        isLoadingVariable = 1;
                                        code_recur(root->rnode);
                                        isLoadingVariable = 0;
                                        printf("dec 1\n");
                                        printf("sto\n");

                                    }
                                    isINCRDECR =0;
                                    break;

                                case PLUS:
                                    /* Plus token "+" */
                                    isLoadingVariable = 1;
                                    code_recur(root->lnode);
                                    code_recur(root->rnode);
                                    isLoadingVariable = 0;
                                    printf("add\n");
                                    break;

                                case MINUS:
                                    /* Minus token "-" */
                                    isLoadingVariable = 1;
                                    code_recur(root->lnode);
                                    code_recur(root->rnode);
                                    isLoadingVariable = 0;
                                    leaf = (leafnode *) root->lnode;

                                    if (leaf != NULL) {
                                        printf("sub\n");
                                    } else {
                                        printf("neg\n");
                                    }
                                    break;

                                case DIV:
                                    /* Divide token "/" */
                                    isLoadingVariable = 1;
                                    code_recur(root->lnode);
                                    code_recur(root->rnode);
                                    isLoadingVariable = 0;
                                    printf("div\n");
                                    break;

                                case STAR:
                                    /* multiply token "*" */
                                    isLoadingVariable = 1;
                                    code_recur(root->lnode);
                                    code_recur(root->rnode);
                                    isLoadingVariable = 0;
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

                                case B_AND:
                                    code_recur(root->lnode);
                                    code_recur(root->rnode);
                                    isPointer = 1;
                                    break;

                                default:
                                    code_recur(root->lnode);
                                    code_recur(root->rnode);
                                    break;
                            }
                            break;


                        case TN_WHILE:
                            prev_scope = loop_level;
                            loop_level = scope_counter++;
                            printf("while_loop_%i:\n", loop_level);
                            code_recur(root->lnode);
                            printf("fjp end_%i\n", loop_level);
                            code_recur(root->rnode);
                            printf("ujp while_loop_%i\n", loop_level);
                            printf("end_%i:\n", loop_level);
                            loop_level = prev_scope;
                            break;

                        case TN_DOWHILE:
                            /* Do-While case */
                            prev_scope = loop_level;
                            loop_level = scope_counter++;
                            printf("do_while_loop_%i:\n", loop_level);
                            code_recur(root->rnode);
                            code_recur(root->lnode);
                            printf("fjp end_%i\n", loop_level);
                            printf("ujp do_while_loop_%i\n", loop_level);
                            printf("end_%i:\n", loop_level);
                            loop_level = prev_scope;
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
	 *	What is needed to be done is to traverse the tree like in Code_recur but without the prints
	 *	And without deleting the table in the return case
	 *	After that was done we need to go over the symbol table and print everything that is needed
	*/
}