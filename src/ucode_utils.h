#pragma once
#include <vector>
#include <algorithm>

template<typename T>
struct index_vector: public std::vector<T> {
    int index = 0;

    void move_index(int delta);
    T& get_index_item() { return this->at(this->index); }
};

template<typename T>
void index_vector<T>::move_index(int delta) {
    this->index += delta;
    this->index = std::clamp(this->index, 0, static_cast<int>(this->size()) - 1);
}
