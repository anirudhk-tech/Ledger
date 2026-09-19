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

using namespace std;

struct PageOneHeader {
    string db_name;
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

bool open(string db_name) {
    char* buffer = new char[BUFFER_SIZE];
    memcpy(buffer, db_name.c_str(), db_name.length());
    buffer[db_name.length()] = '\0';

    uint32_t page_ctr = 0;
    memcpy(buffer + PAGE_SIZE, &page_ctr, sizeof(page_ctr));

    ofstream file(db_name, ios::out | ios::binary);

    if (!file.is_open()) {
        cout << "Could not open database file!";
        return false;
    }

    file.write(buffer, BUFFER_SIZE);
    file.close();

    delete[] buffer;
    return true;
}

int get_block(string db_name) {
    ifstream db_file(db_name, ios::in | ios::binary | ios::ate);

    if (db_file.is_open()) {
        streamsize file_size = db_file.tellg();
        db_file.seekg(0, ios::beg);

        vector<char> buffer(file_size);

        db_file.read(buffer.data(), file_size);
        db_file.close();

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

        return new_page_num;
    } else {
        cout << "Database info could not be loaded!";
        return -1;
    }
}

int main() {
    bool db_opened = open("HelloWorld");
    return 0;
}
