#pragma once
#define UNITY_SUPPORT_TEST_CASES
#include <unity.h>
#include <exception>

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
void func ## _exception_wrapper(){ \
    try{ \
        func(); \
    }catch (std::exception &e){ \
        printf("%s", e.what()); \
        TEST_FAIL_MESSAGE("exception"); \
    } \
} \
const struct UnityTest_t func ## test = AddUnityTest(func ## _exception_wrapper, #func, __LINE__); \
void func()



#ifdef __cplusplus
}
#endif