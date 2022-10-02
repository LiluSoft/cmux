Import("env")
import subprocess
import os

def runNativeChecks(source, target, env):
    print("Running Dr. Memory", source, target)
    # ,"-logdir","./.reports"
    cppcheckargs = ["drmemory", "-brief","-exit_code_if_errors","1","-batch","-suppress","scripts/drmemory_suppress.txt","--", target[0].rstr()]
    print(" ".join(cppcheckargs))
    env.Execute(" ".join(cppcheckargs))

env.AddPostAction("$BUILD_DIR/$PROGNAME$PROGSUFFIX", runNativeChecks)