#include "sqlite.hpp"

namespace SQLite3
{

    SQLite::SQLite(const std::string &DB_) : FILE_DATABASE(DB_), db(nullptr), stmt(nullptr), rc(0)
    {
    }

    SQLite::~SQLite()
    {
        if (db)
            sqlite3_close(db);
    }

    const bool SQLite::open()
    {
        rc = sqlite3_open(FILE_DATABASE.c_str(), &db);
        if (rc)
        {
            sqlite3_close(db);
            db = nullptr;
            return false;
        }
        return true;
    }

    const bool SQLite::is_created()
    {
        auto result = command("SELECT name FROM sqlite_master WHERE type='table' AND name NOT LIKE 'sqlite_%'");
        return not result->empty();
    }

    int SQLite::callback(void *data, int argc, char **argv, char **azColName)
    {
        auto result_map = *static_cast<std::shared_ptr<ResultMap>*>(data);

        for (int i = 0; i < argc; i++)
        {
            const char *columnName = azColName[i];
            const char *value = argv[i] ? argv[i] : "";
            (*result_map)[columnName].emplace_back(value);
        }
        return 0;
    }

    const int SQLite::get_rc() const
    {
        return rc;
    }

} // namespace SQLite3
