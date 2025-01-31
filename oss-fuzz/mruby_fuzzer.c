#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <mruby.h>
#include <mruby/compile.h>

static int saved_null;
static int saved_stdout;
static int saved_stderr;

int LLVMFuzzerInitialize(int *argc, char ***argv) {
    saved_null = open("/dev/null", O_WRONLY);
    saved_stdout = dup(STDOUT_FILENO);
    saved_stderr = dup(STDERR_FILENO);
    return 0;
}

int LLVMFuzzerTestOneInput(uint8_t *Data, size_t size) {
    if (size < 1) {
        return 0;
    }

    dup2(saved_null, STDOUT_FILENO);
    dup2(saved_null, STDERR_FILENO);
    if (freopen("/dev/null", "w", stdout) == NULL) {
        return 0;
    }
    if (freopen("/dev/null", "w", stderr) == NULL) {
        return 0;
    }

    char *code = malloc(size+1);
    memcpy(code, Data, size);
    code[size] = '\0';
    mrb_state *mrb = mrb_open();
    mrb_load_string(mrb, code);
    mrb_close(mrb);
    free(code);

    fflush(stdout);
    fflush(stderr);
    dup2(saved_stdout, STDOUT_FILENO);
    dup2(saved_stderr, STDERR_FILENO);
    return 0;
}
