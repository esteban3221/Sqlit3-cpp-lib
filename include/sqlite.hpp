#pragma once
#include <sqlite3.h>
#include <iostream>
#include <vector>
#include <mutex>
#include <memory>
#include <map>

using ResultMap = std::map<std::string, std::vector<std::string>>;
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
        static int callback(void *, int, char **, char **);

    public:
        SQLite(const std::string &DB_);
        ~SQLite();
        const bool open();
        const bool is_created();
        const int get_rc() const;

        template <typename... Args>
        std::shared_ptr<ResultMap> command(const std::string &sql, Args &&...args)
        {
            // std::lock_guard<std::mutex> lock(db_mutex);
            auto result = std::make_shared<ResultMap>();

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
                    (*result)[columnName].emplace_back(value ? value : "");
                }
            }

            sqlite3_finalize(stmt);

            if (rc != SQLITE_OK)
            {
                std::string error = "Error al ejecutar la sentencia SQL: " + std::string(sqlite3_errmsg(db)) + "\nSQL: " + sql + "\n";
                throw std::runtime_error(error);
            }

            return result;
        }

        template <typename T>
        void bindArgument(sqlite3_stmt *stmt, int index, T &&value)
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
    };
} // namespace SQLite3