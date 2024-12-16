#pragma once
#include <sqlite3.h>
#include <iostream>
#include <vector>
#include <map>

namespace SQLite3
{
    class SQLite
    {
    private:
        const std::string FILE_DATABASE;
        sqlite3 *db;
        sqlite3_stmt *stmt;
        char *zErrMsg = 0;
        int rc;
        std::string sql;
        static int callback(void *, int, char **, char **);
        static inline std::map<std::string, std::vector<std::string>> result;

    public:
        SQLite(const std::string &DB_);
        ~SQLite();
        const bool open();
        const bool is_created();
        void command(std::string);
        const int get_rc() const;
        std::map<std::string, std::vector<std::string>> &get_result() const;

        template <typename... Args>
        void command(const std::string &sql, Args &&...args);

        template <typename T>
        void bindArgument(sqlite3_stmt *stmt, int index, T &&value);
    };
} // namespace SQLite3