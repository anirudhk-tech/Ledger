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
    int page_ctr;
    int page_size;
};

struct Page {
    int page_number;
    int used;
    char data[];
};

int PAGE_SIZE = 4096;
int BUFFER_SIZE = 2 * PAGE_SIZE;

const char* get_page_ptr(const vector<char>& buffer, int block_num) {
    return buffer.data() + (PAGE_SIZE) + (block_num * PAGE_SIZE);
};

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
    page_one_header -> page_ctr = 0;
    page_one_header -> page_size = PAGE_SIZE;

    save_to_db(db_name, buffer, buffer.size());

    return true;
}

int get_block(string db_name) {
    vector<char> buffer = read_from_db(db_name);

    if (buffer.size()) {
        char* page1_address = buffer.data();
        PageOneHeader* header = reinterpret_cast<PageOneHeader*>(page1_address);
        uint32_t page_ctr = header -> page_ctr;

        for (uint32_t x = 0; x < page_ctr; x++) {
            const char* curr = get_page_ptr(buffer, x);

            Page* page = reinterpret_cast<Page*>(const_cast<char*>(curr));

            if (!(page->used)) {
                cout << "Unused page found." << "\n";
                return page->page_number;
            }
        }

        uint32_t new_page_num = page_ctr;
        buffer.resize(buffer.size() + PAGE_SIZE);

        header = reinterpret_cast<PageOneHeader*>(buffer.data());
        header -> page_ctr++;

        const char* new_page_ptr = get_page_ptr(buffer, new_page_num);
        Page* new_page = reinterpret_cast<Page*>(const_cast<char*>(new_page_ptr));
        new_page->page_number = new_page_num;
        cout << "New page created! Page number: " << new_page_num << "\n";
        new_page->used = 0;

        save_to_db(db_name, buffer, buffer.size());

        return new_page_num;
    } else {
        cout << "Database info could not be loaded!";
        return -1;
    }
}

void read_block(string db_name, int block_num, char* page) {
    vector<char> buffer = read_from_db(db_name);
    const char* page_ptr = get_page_ptr(buffer, block_num);
    memcpy(page, page_ptr, PAGE_SIZE);
}

int write_block(string db_name, const char* data, size_t size) {
    int block_num = get_block(db_name);
    vector<char> buffer = read_from_db(db_name);

    const char* page_ptr = get_page_ptr(buffer, block_num);
    Page* page = reinterpret_cast<Page*>(const_cast<char*>(page_ptr));

    if (size > PAGE_SIZE) {
        return -1;
    }

    memcpy(page->data, data, size);
    page->used = 1;

    save_to_db(db_name, buffer, buffer.size());
    return page->page_number;
}

void delete_block(string db_name, int block_num) {
    vector<char> buffer = read_from_db(db_name);

    const char* page_ptr = get_page_ptr(buffer, block_num);
    Page* page = reinterpret_cast<Page*>(const_cast<char*>(page_ptr));

    page->used = 0;
    save_to_db(db_name, buffer, buffer.size());
}

int main() {
    cout << "Initializing database...\n";

    string DB_NAME = "HelloWorld";
    bool db_opened = open(DB_NAME);
    const char* data = "Hello!";

    int block_num = write_block(DB_NAME, data, strlen(data));

    if (block_num < 0) {
        cout << "Write failed!";
        return -1;
    }

    cout << "Data written!\n";

    char raw[PAGE_SIZE];
    read_block(DB_NAME, block_num, raw);
    Page* page = reinterpret_cast<Page*>(page);

    cout << "Data read: " << page->data << "\n";
    cout << "Page number: " << page->page_number << "\n";
    cout << "Page used: " << page->used << "\n";

    delete[] reinterpret_cast<char*>(page);

    cout << "Memory freed! Done.\n";

    return 0;
}
