
#include <iostream>
#include <ledger/pager.hpp>

using namespace std;

int main() {
    ledger::PageManager page_manager;

    cout << "Initializing database...\n";

    string DB_NAME = "HelloWorld";
    bool db_opened = page_manager.open(DB_NAME);
    const char* data = "Hello!";

    int block_num = page_manager.write_block(DB_NAME, data, strlen(data));

    if (block_num < 0) {
        cout << "Write failed!";
        return -1;
    }

    cout << "Data written!\n";

    page_manager.print_page_details(DB_NAME, block_num);

    return 0;
}
