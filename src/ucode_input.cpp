#include <doctest.h>
#include "ucode_input.h"


TEST_CASE("operator overloading for convenient config") {
    SUBCASE("Key + CTRL") {
        auto a = Key::j + MOD_CTRL;
        CHECK(a.key == Key::j);
        CHECK(a.ctrl.value == true);
        CHECK(a.shift.value == false);
    }

    SUBCASE("Key + SHIFT") {
        auto a = Key::j + UCODE_MOD_SHIFT;
        CHECK(a.key == Key::j);
        CHECK(a.ctrl.value == false);
        CHECK(a.shift.value == true);
    }

    SUBCASE("Key + SHIFT + CTRL") {
        auto a = Key::j + UCODE_MOD_SHIFT + MOD_CTRL;
        CHECK(a.key == Key::j);
        CHECK(a.ctrl.value == true);
        CHECK(a.shift.value == true);
    }

    SUBCASE("Key + CTRL + SHIFT") {
        auto a = Key::j + MOD_CTRL + UCODE_MOD_SHIFT;
        CHECK(a.key == Key::j);
        CHECK(a.ctrl.value == true);
        CHECK(a.shift.value == true);
    }
}

KeyDef operator+(const Key a, const ctrl_flag b) {
    auto res = KeyDef(a);
    res.ctrl = b;
    return res;
}

KeyDef operator+(const Key a, const shift_flag b) {
    auto res = KeyDef(a);
    res.shift = b;
    return res;
}

KeyDef operator+(const KeyDef a, const ctrl_flag b) {
    auto res = a;
    res.ctrl = b;
    return res;
}

KeyDef operator+(const KeyDef a, const shift_flag b) {
    auto res = a;
    res.shift = b;
    return res;
}
