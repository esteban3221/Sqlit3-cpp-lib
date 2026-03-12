#pragma once
#include <sqlite3.h>
#include <iostream>
#include <vector>
#include <mutex>
#include <memory>
#include <map>

using ResultMap = std::map<std::string, std::vector<std::string>>;

template <typename>
struct always_false : std::false_type
{
};

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
        bool debug = false;
        static int callback(void *, int, char **, char **);

    public:
        SQLite(const std::string &DB_);
        ~SQLite();
        const bool open();
        const bool is_created();
        const int get_rc() const;

        bool &property_debug() { return debug; }

        template <typename... Args>
        std::shared_ptr<ResultMap> command(const std::string &sql, Args &&...args)
        {
            auto result = std::make_shared<ResultMap>();
            sqlite3_stmt *stmt_local; // Variable local

            // 1. Usar una variable local para el return code
            int local_rc = sqlite3_prepare_v2(db, sql.c_str(), -1, &stmt_local, NULL);

            if (local_rc != SQLITE_OK)
            {
                throw std::runtime_error("Error SQL Prepare: " + std::string(sqlite3_errmsg(db)));
            }

            int index = 1;
            // Pasamos el stmt_local a bindArgument
            (void)std::initializer_list<int>{(bindArgument(stmt_local, index++, std::forward<Args>(args)), 0)...};

            while (sqlite3_step(stmt_local) == SQLITE_ROW)
            {
                int columns = sqlite3_column_count(stmt_local);
                for (int i = 0; i < columns; i++)
                {
                    const char *columnName = sqlite3_column_name(stmt_local, i);
                    // IMPORTANTE: Forzar la copia a std::string inmediatamente
                    const unsigned char *val = sqlite3_column_text(stmt_local, i);
                    std::string textValue = val ? reinterpret_cast<const char *>(val) : "";
                    (*result)[columnName].emplace_back(textValue);
                }
            }

            sqlite3_finalize(stmt_local);
            return result;
        }

        template <typename T>
        void bindArgument(sqlite3_stmt *stmt, int index, T &&value)
        {
            int b_rc = SQLITE_OK; // Variable local para el bindeo

            if constexpr (std::is_integral_v<std::remove_reference_t<T>> || std::is_floating_point_v<std::remove_reference_t<T>>)
            {
                if constexpr (std::is_floating_point_v<std::remove_reference_t<T>>)
                    b_rc = sqlite3_bind_double(stmt, index, static_cast<double>(value));
                else
                    b_rc = sqlite3_bind_int64(stmt, index, static_cast<sqlite3_int64>(value));
            }
            else if constexpr (std::is_same_v<std::remove_reference_t<T>, const char *>)
            {
                b_rc = sqlite3_bind_text(stmt, index, value, -1, SQLITE_STATIC);
            }
            else if constexpr (std::is_convertible_v<std::remove_reference_t<T>, std::string>)
            {
                std::string strValue = static_cast<std::string>(value);
                b_rc = sqlite3_bind_text(stmt, index, strValue.c_str(), -1, SQLITE_TRANSIENT);
            }

            if (b_rc != SQLITE_OK)
            {
                throw std::runtime_error("Error Bind (" + std::to_string(b_rc) + "): " + std::string(sqlite3_errmsg(db)));
            }
        }
    };
} // namespace SQLite3