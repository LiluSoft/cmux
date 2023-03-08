#pragma once
#define UNITY_SUPPORT_TEST_CASES
#include <unity.h>

#ifdef __cplusplus
extern "C"
{
#endif

struct UnityTest_t{
    UnityTestFunction Func;
    const char* FuncName; 
    int FuncLineNum;
};

const struct UnityTest_t AddUnityTest(UnityTestFunction Func, const char* FuncName, const int FuncLineNum);
void RunAllUnityTests();

#define TEST(func) \
void func(); \
const struct UnityTest_t func ## test = AddUnityTest(func, #func, __LINE__); \
void func()



#ifdef __cplusplus
}
#endif