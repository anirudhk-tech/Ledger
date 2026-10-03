#pragma once

#include <vector>

using namespace std;

struct Page;

namespace ledger {

class PageManager {
    public:
        bool open(string db_name);
        int get_block(string db_name);
        int write_block(string db_name, const char* data, size_t size);
        void delete_block(string db_name, int block_num);
        void print_page_details(string db_name, int block_num);

    private:
        const char* get_page_ptr(const vector<char>& buffer, int block_num);
        void save_to_db (string db_name, const vector<char>& buffer, int buffer_size);
        vector<char> read_from_db(string db_name);
        void read_block(string db_name, int block_num, char* page);
};

} // namespace ledger
