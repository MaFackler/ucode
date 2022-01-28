#pragma once
#include <string>
#include <vector>
#include <algorithm>
#include <filesystem>

namespace fs = std::filesystem;
using std::string;

template<typename T>
struct index_vector: public std::vector<T> {
private:
    int _index = 0;

public:
    void move_index(int delta) {
        this->set_index(this->_index + delta);
    }
    void set_index(int index) {
        this->_index = index;
        this->_index = std::clamp(this->_index, 0, static_cast<int>(this->size()) - 1);
    }
    int index() { return this->_index; }
    T& get_index_item() { return this->at(this->_index); }
};

inline
string path_join(const std::string_view &a, const std::string_view &b) {
    return (fs::path(a) / fs::path(b)).string();
}
