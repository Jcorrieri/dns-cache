#ifndef JC_BP_DB_RAII
#define JC_BP_DB_RAII

#include <string>
#include <string_view>

struct sqlite3_stmt;
struct sqlite3;

class Database {
public:
    // Explicit to prevent implicit type coersion in accepting functions
    explicit Database(const std::string& db_path);

    ~Database() noexcept;

    Database(const Database&) = delete; // Remove copy constructor
    Database& operator=(const Database&) = delete; // Remove copy assignment
    
    Database(Database&& other) noexcept; // Move constructor
    Database& operator=(Database&& other) noexcept; // Move assignment
    
    sqlite3_stmt* prepare(std::string_view sql) const;

    void execute(const std::string& sql) const;

    sqlite3* get() const;
    
private:
    sqlite3* m_db{nullptr};
};

#endif
