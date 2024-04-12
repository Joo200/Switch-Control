//
// Created by johannes on 16.04.24.
//

#include <gmock/gmock.h>
#include <gtest/gtest.h>

TEST(Todotest, FooBar) {
}

namespace {
    int argc;
    char **argv;

    [[gnu::constructor]] void copy_argc_argv(int argc, char **argv) {
        ::argc = argc;
        ::argv = argv;
    }
}

extern "C" void app_main() {
    testing::InitGoogleTest(&argc, argv);
    testing::InitGoogleMock(&argc, argv);
    exit(RUN_ALL_TESTS());
}
