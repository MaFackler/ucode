// DOCTEST Wrapper

#define DOCTEST_CONFIG_IMPLEMENT

#ifndef DOCTEST_CONFIG_DISABLE
#include <doctest.h>

int handle_doctest(int argc, char **argv) {
    doctest::Context ctx;
    //ctx.setOption("--no-run", true);
    ctx.applyCommandLine(argc, argv);
    int res = ctx.run();
    if (ctx.shouldExit())
        return 1;
    
    return res;
}

#define UCODE_TEST \
    int should_close = handle_doctest(argc, argv); \
    if (should_close) { \
        return should_close; \
    }

#else // DOCTEST_CONFIG_DISABLE

#define UCODE_TEST

#endif // DOCTEST_CONFIG_DISABLE


