#ifndef TRIES_LINEAR_HASH_HPP
#define TRIES_LINEAR_HASH_HPP
#include <string>
#include "mvector.hpp"
#include "constants.hpp"
#include "mstack.hpp"
//#include "trie_nodes.hpp"

class static_node;
template <typename T>
class linear_hash {
public:
    explicit linear_hash(size_t initial_size = constants::LH_INIT_SIZE);

    ~linear_hash();

    T *insert(std::string &word, bool eow);

    T *get(const std::string &word, int *hash = nullptr, int *index = nullptr) const;

    T *get_static(const std::string &word) const;

    void delete_word(const std::string &word);

    size_t size() const;

    void print() const;

    void push_to_stack(mstd::stack<T*> *s);

    mstd::vector<T> *get_bucket(int i) const;

    std::string stats(bool v = true) const;

    bool empty() const;
private:
    size_t _size;
    size_t _num_items;
    size_t _p;

    mstd::vector<T> **_entries;

    int _hash(const std::string &word) const;

    void _resize();

    int _calculate_load() const;
};

//template  <> class linear_hash<static_node> {
//public:
//    static_node *get(const std::string &word) const {
//        int hash = _hash(word);
//        int index = hash % _size;
//        if (index < _p) {
//            index = hash % (_size * 2);
//        }
//
//
//        return static_bsearch(word, *_entries[index]);
//    }
//};

#endif //TRIES_LINEAR_HASH_HPP