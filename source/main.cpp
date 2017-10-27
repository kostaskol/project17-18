#include <iostream>
#include <cmd_parser.hpp>
#include <parser.hpp>
#include "trie.hpp"

using std::cout;
using std::endl;
using std::string;
using mstd::vector;
using mstd::logger;

std::string mstd::logger::file;

int main(int argc, char **argv) {
    logger::file = "../debug/log.txt";
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
    bool debug;
    try {
        init_file = c_parser->get_string("-i");
        query_file = c_parser->get_string("-q");
    } catch (std::runtime_error &e) {
        logger::error("main", "User has not provided both -i and -q. Exiting..", BOTH);
        return -1;
    }

    debug = c_parser->is_set("--debug");

    delete c_parser;
    // End command line arguments parsing

    trie t;

    // Begin initialisation file parsing
    parser init_parser(init_file);

    vector<string> v;
    while (true) {
        bool stop = init_parser.next_init(&v);
        if (v.size() == 0) break;
        string s = helpers::join(v, ' ');
        t.add(v);
        logger::success("init", "Added N-Gram " + s);
        v.clear();
        if (stop) break;
    }

    // End initialisation file parsing

    // Begin query file parsing
    parser query_parser(query_file);

    int cmd_type;

    mstd::queue<std::string> results;
    while (true) {
        bool stop = query_parser.next_command(&v, &cmd_type);
        string s;
        switch (cmd_type) {
            case INSERTION:
                s = helpers::join(v, ' ');
                logger::success("add", "Added N-Gram \"" + s + "\"");
                t.add(v);
                break;
            case QUERY:
                t.search(v,&results);
                break;
            case DELETION:
                s = helpers::join(v, ' ');
                if (t.r_delete_ngram(v)) {
                    string succ = "The N-Gram \"" + s + "\" had been deleted!";
                    logger::success("query", succ, BOTH);
                } else {
                    string fail = "The N-Gram \"" + s + "\" does not exist, so it cannot be deleted!";
                    logger::error("query", fail, BOTH, false);
                }
                break;
            case FINISH:
                // Print query results
                string succ = "";
                while(!results.empty()){
                    succ = results.pop();
                    if (succ == "$$END$$") {
                        logger::error("query", succ, STDOUT);
                    }
                    else{
                        logger::success("query", succ, STDOUT);
                    }
                }
                break;
        }
        if (stop) break;
        v.clear();
    }
    results.clear();
    t.print_tree();
    // End query file parsing
}
