#include "trie.hpp"
#include "constants.hpp"
#include <iostream>
#include <cmd_parser.hpp>
#include <parser.hpp>
#include <unistd.h>
#include <fstream>


using std::cout;
using std::endl;
using std::string;
using mstd::vector;
using mstd::logger;
using std::ifstream;

bool __debug__;

int main(int argc, char **argv) {
    // Start command line arguments parsing
    string legal = " -i <init-file> -q <query-file> [--debug]";
    if (argc < 3) {
        cout << "Invalid number of arguments. Usage: " << argv[0] << legal << endl;
        return 1;
    }

    auto *ht = new mstd::hash_table<string>(3);
    ht->put("-i", "<b>");
    ht->put("-q", "<b>");
    ht->put("--debug", "<u>");
    auto *c_parser = new mstd::cmd_parser(true, std::move(legal));
    c_parser->parse(argc, argv, *ht);
    delete ht;

    string init_file;
    string query_file;
    try {
        init_file = c_parser->get_string("-i");
        query_file = c_parser->get_string("-q");
    } catch (std::runtime_error &e) {
        logger::error("main", "User has not provided both -i and -q. Exiting..");
        return -1;
    }

    __debug__ = c_parser->is_set("--debug");

    delete c_parser;
    // End command line arguments parsing

    vector<string> v(100);
    trie* t;

    // Begin initialisation file parsing
    parser init_parser(init_file);

    bool stop = init_parser.next_init(&v);
    bool compress = false;
    if (v.get(0) == "STATIC") {
        t = new static_trie();
        compress = true;
    } else if (v.get(0) == "DYNAMIC") {
        t = new trie();
    }
    v.clear(100);

    while (true) {
        stop = init_parser.next_init(&v);
        if (v.size() == 0 && stop) break;
        t->add(v);
//        logger::success("init", "Added N-Gram " + s);
        v.clear(100);
        if (stop) break;
    }
    if (compress) {
        t->compress();
    }



    // End initialisation file parsing

    // Begin query file parsing
    parser query_parser(query_file);

    int cmd_type;

    mstd::queue<std::string> results;
    while (true) {
        stop = query_parser.next_command(&v, &cmd_type);
        if (v.size() == 0 && stop) break;
        string s;
        switch (cmd_type) {
            case INSERTION:
                t->add(v);
                break;
            case QUERY:
                t->search(v,&results);
                break;
            case DELETION:
                t->delete_ngram(v);
                break;
            case FINISH: {
                // Print query results
                string succ;
                while (!results.empty()) {
                    succ = results.pop();
                    if (succ == "$$END$$") {
                        std::cout << "-1" << '\n';
                    } else {
                        std::cout << succ << '\n';
                    }
                }
                break;
            }
            default:
                break;
        }
        if (stop) break;
        v.clear(100);
    }
    results.clear();
    // End query file parsing
    delete t;
}