// Reqs
// Open() a file
// Allocate() a block within the file
// Clean() a block within that file
// Unused() block number within that file
// Used() tells whether a block is used

#include <cstddef>
#include <cstdint>
#include <cstring>
#include <fstream>
#include <ios>
#include <iostream>
#include <string>
#include <vector>

using namespace std;

struct PageOneHeader {
    char db_name[256];
};

struct PageTwoHeader {
    int page_ctr;
};

struct Page {
    int page_number;
    int used;
    char data[];
};

int PAGE_SIZE = 4096;
int BUFFER_SIZE = 2 * PAGE_SIZE;

void save_to_db (string db_name, const vector<char>& buffer, int buffer_size) {
    ofstream file(db_name, ios::out | ios::binary);

    if (!file.is_open()) {
        cout << "Could not open database file!";
    }

    file.write(buffer.data(), buffer_size);
    file.close();
}

vector<char> read_from_db(string db_name) {
    ifstream db_file(db_name, ios::in | ios::binary | ios::ate);

    if (db_file.is_open()) {
        streamsize file_size = db_file.tellg();
        db_file.seekg(0, ios::beg);

        vector<char> buffer(file_size);

        db_file.read(buffer.data(), file_size);
        db_file.close();

        return buffer;
    }

    vector<char> buffer(0);
    return buffer;
}

bool open(string db_name) {
    vector<char> existing_buffer = read_from_db(db_name);

    if (existing_buffer.size() > 0) {
        return true;
    }

    vector<char> buffer(BUFFER_SIZE);

    char* page1_address = buffer.data();
    PageOneHeader* page_one_header = reinterpret_cast<PageOneHeader*>(page1_address);
    strncpy(page_one_header->db_name, db_name.c_str(), 255);

    char* page2_address = buffer.data() + PAGE_SIZE;
    PageTwoHeader* page_two_header = reinterpret_cast<PageTwoHeader*>(page2_address);
    page_two_header -> page_ctr = 0;

    save_to_db(db_name, buffer, buffer.size());

    return true;
}

int get_block(string db_name) {
    vector<char> buffer = read_from_db(db_name);

    if (buffer.size()) {
        char* page2_address = buffer.data() + PAGE_SIZE;
        PageTwoHeader* header = reinterpret_cast<PageTwoHeader*>(page2_address);
        uint32_t page_ctr = header -> page_ctr;

        if (page_ctr == 0) {
            buffer.resize(buffer.size() + PAGE_SIZE);

            header = reinterpret_cast<PageTwoHeader*>(buffer.data() + PAGE_SIZE);
            header -> page_ctr = 1;

            Page* first_page = reinterpret_cast<Page*>(buffer.data() + (2 * PAGE_SIZE));
            first_page->page_number = 0;
            first_page->used = 0;

            save_to_db(db_name, buffer, buffer.size());
            return 0;
        }

        for (uint32_t x = 0; x < page_ctr; x++) {
            char* curr = buffer.data() + (2 * PAGE_SIZE) + (x * PAGE_SIZE);

            Page* page = reinterpret_cast<Page*>(curr);

            if (!(page->used)) {
                return page->page_number;
            }
        }

        uint32_t new_page_num = page_ctr;
        buffer.resize(buffer.size() + PAGE_SIZE);

        header = reinterpret_cast<PageTwoHeader*>(buffer.data() + PAGE_SIZE);
        header -> page_ctr++;

        Page* new_page = reinterpret_cast<Page*>(buffer.data() + (2 * PAGE_SIZE) + (new_page_num * PAGE_SIZE));
        new_page->page_number = new_page_num;
        new_page->used = 0;

        save_to_db(db_name, buffer, buffer.size());

        return new_page_num;
    } else {
        cout << "Database info could not be loaded!";
        return -1;
    }
}

Page* read_block(string db_name, int block_num) {
    vector<char> buffer = read_from_db(db_name);
    char* page_ptr = buffer.data() + (2 * PAGE_SIZE) + (block_num * PAGE_SIZE);
    char* heap_cpy = new char[PAGE_SIZE];

    memcpy(heap_cpy, page_ptr, PAGE_SIZE);

    return reinterpret_cast<Page*>(heap_cpy);
}

int write_block(string db_name, char* data, size_t size) {
    int block_num = get_block(db_name);
    vector<char> buffer = read_from_db(db_name);

    char* page_ptr = buffer.data() + (2 * PAGE_SIZE) + (block_num * PAGE_SIZE);
    Page* page = reinterpret_cast<Page*>(page_ptr);
    memcpy(page->data, data, size);
    page->used = 1;

    save_to_db(db_name, buffer, buffer.size());
    return page->page_number;
}

void delete_block(string db_name, int block_num) {
    vector<char> buffer = read_from_db(db_name);

    char* page_ptr = buffer.data() + (2 * PAGE_SIZE) + (block_num * PAGE_SIZE);
    Page* page = reinterpret_cast<Page*>(page_ptr);

    page->used = 0;
    save_to_db(db_name, buffer, buffer.size());
}

int main() {
    bool db_opened = open("HelloWorld");
    return 0;
}
