#include "sqlite.hpp"

namespace SQLite3
{
    int SQLite::callback(void *data, int argc, char **argv, char **azColName)
    {
        for (int i = 0; i < argc; i++)
        {
            const char *columnName = azColName[i];
            const char *value = argv[i] ? argv[i] : "";
            result[columnName].emplace_back(value);
        }
        return 0;
    }

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
        command("SELECT name FROM sqlite_master WHERE type='table' AND name NOT LIKE 'sqlite_%'");
        return not get_result().empty();
    }

    void SQLite::command(std::string sql_)
    {
        sql = sql_;
        rc = sqlite3_exec(db, sql.c_str(), callback, nullptr, &zErrMsg);
        if (rc != SQLITE_OK)
        {
            std::string error = "SQL error: " + std::string(zErrMsg);
            sqlite3_free(zErrMsg);
            throw std::runtime_error(error);
        }
    }

    const int SQLite::get_rc() const
    {
        return rc;
    }

    std::map<std::string, std::vector<std::string>> &SQLite::get_result() const
    {
        return result;
    }

} // namespace SQLite3
