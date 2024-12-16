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
        {
            sqlite3_close(db);
        }
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
        return db != nullptr;
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

    template <typename... Args>
    void SQLite::command(const std::string &sql, Args &&...args)
    {
        result.clear();

        sqlite3_stmt *stmt;
        rc = sqlite3_prepare_v2(db, sql.c_str(), sql.length(), &stmt, NULL);
        if (rc != SQLITE_OK)
        {
            std::string error = "Error al compilar la sentencia SQL: " + std::string(sqlite3_errmsg(db)) + "\nSQL: " + sql + "\n";
            throw std::runtime_error(error);
        }

        int index = 1;
        (void)std::initializer_list<int>{(bindArgument(stmt, index++, std::forward<Args>(args)), 0)...};

        while (sqlite3_step(stmt) == SQLITE_ROW)
        {
            int columns = sqlite3_column_count(stmt);
            for (int i = 0; i < columns; i++)
            {
                const char *columnName = sqlite3_column_name(stmt, i);
                const char *value = reinterpret_cast<const char *>(sqlite3_column_text(stmt, i));
                result[columnName].emplace_back(value ? value : "");
            }
        }

        sqlite3_finalize(stmt);

        if (rc != SQLITE_OK)
        {
            std::string error = "Error al ejecutar la sentencia SQL: " + std::string(sqlite3_errmsg(db)) + "\nSQL: " + sql + "\n";
            throw std::runtime_error(error);
        }
    }

    template <typename T>
    void SQLite::bindArgument(sqlite3_stmt *stmt, int index, T &&value)
    {
        if constexpr (std::is_integral_v<std::remove_reference_t<T>> || std::is_floating_point_v<std::remove_reference_t<T>>)
        {
            rc = sqlite3_bind_int(stmt, index, value);
        }
        else if constexpr (std::is_same_v<std::remove_reference_t<T>, const char *>)
        {
            rc = sqlite3_bind_text(stmt, index, value, -1, SQLITE_STATIC);
        }

        if (rc != SQLITE_OK)
        {
            throw std::runtime_error("Error al bindear el argumento");
        }
    }
} // namespace SQLite3
