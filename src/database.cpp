#include <filesystem>
#include "../external/sqlite-amalgamation-3530400/sqlite3.h" // TODO: Fix this bologne....

void init_database(const std::string_view db_name) {
    if (!std::filesystem::exists(db_name)) {
        return;
    }
}

int main() {
    const std::string db_name{"temp.db"};

    sqlite3 *db = nullptr;

    return 0;
}
