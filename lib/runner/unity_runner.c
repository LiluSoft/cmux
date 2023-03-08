#include "unity_runner.h"
#include "simple_linked_list.h"

static struct simple_linked_list_node * root = NULL;



const struct UnityTest_t  AddUnityTest(UnityTestFunction Func, const char* FuncName, const int FuncLineNum){
    struct UnityTest_t * test = (struct UnityTest_t *)malloc(sizeof(struct UnityTest_t));
    test->Func = Func;
    test->FuncName = FuncName;
    test->FuncLineNum = FuncLineNum;


    if (root == NULL){
         root = (simple_linked_list_node_t *) malloc(sizeof(simple_linked_list_node_t));
         root->val = test;
         root->next = NULL;
    }else{
        simple_linked_list_push(root, test);
    }

    return *test;
}

void RunAllUnityTests(){
    simple_linked_list_node_t * current = root;
    while (current != NULL) {
        struct UnityTest_t * test = (struct UnityTest_t *)current->val;
        UnityDefaultTestRun(test->Func, test->FuncName, test->FuncLineNum);
        free(current->val);

        simple_linked_list_node_t * last = current;
        current = current->next;
        free(last);
    }
}