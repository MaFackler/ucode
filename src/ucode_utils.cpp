#include "ucode_utils.h"
#include <doctest.h>

TEST_CASE("index_vector") {

    index_vector<int> i;
    i.push_back(1);
    i.push_back(2);

    CHECK(i.index() == 0);
    i.move_index(1);
    CHECK(i.index() == 1);
    i.move_index(1);
    CHECK(i.index() == 1);
    i.move_index(-1);
    CHECK(i.index() == 0);

    CHECK(i.get_index_item() == 1);

};

TEST_CASE("path_join") {
    string res = path_join("src", "main.cpp");
    CHECK(res == "src/main.cpp");
}
