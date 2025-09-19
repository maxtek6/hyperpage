/*
 * Copyright (c) 2025 Maxtek Consulting
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#include <hyperpage.hpp>

#include <sqlite3.h>
extern "C"
{
#include <MegaMimes.h>
}

static sqlite3 *get_handle(std::unique_ptr<void, std::function<void(void *)>> &handle)
{
    return static_cast<sqlite3 *>(handle.get());
}

template <bool IsWriter>
static void close_handle(void *handle)
{
    sqlite3 *db = static_cast<sqlite3 *>(handle);
    if (IsWriter)
    {
        sqlite3_exec(db, "VACUUM;", nullptr, nullptr, nullptr);
    }
    sqlite3_close(db);
}

template <class Func, class... Args>
static inline bool sqlite_call(int expected, Func func, Args... args) noexcept
{
    const int rc = func(args...);
    return rc == expected;
}

class stored_page : public hyperpage::page
{
public:
    stored_page(sqlite3 *db, const std::string &path) : _found(false), _stmt(nullptr, &sqlite3_finalize), _path(path), _content(nullptr), _length(0)
    {
        const std::string query = "SELECT mime_type, content FROM hyperpage WHERE path = ?;";
        sqlite3_stmt *stmt = nullptr;
        sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr);
        _stmt.reset(stmt);
        sqlite3_bind_text(_stmt.get(), 1, _path.c_str(), -1, SQLITE_STATIC);

        if (sqlite_call(SQLITE_ROW, sqlite3_step, _stmt.get()))
        {
            _found = true;
            _mime_type = reinterpret_cast<const char *>(sqlite3_column_text(_stmt.get(), 0));
            _content = static_cast<const uint8_t *>(sqlite3_column_blob(_stmt.get(), 1));
            _length = sqlite3_column_bytes(_stmt.get(), 1);
        }
    }

    bool found() const
    {
        return _found;
    }

    const std::string &get_path() const override
    {
        return _path;
    }

    const std::string &get_mime_type() const override
    {
        return _mime_type;
    }

    const uint8_t *get_content() const override
    {
        return _content;
    }

    size_t get_length() const override
    {
        return _length;
    }

private:
    bool _found;
    std::unique_ptr<sqlite3_stmt, decltype(&sqlite3_finalize)> _stmt;
    std::string _path;
    std::string _mime_type;
    const uint8_t *_content;
    size_t _length;
};

hyperpage::reader::reader(const std::string &db_path) : _handle(nullptr, close_handle<false>)
{
    sqlite3 *db = nullptr;
    if (!sqlite_call(SQLITE_OK, sqlite3_open, db_path.c_str(), &db))
    {    
        throw std::runtime_error("Failed to open database: " + db_path);
    }
    _handle.reset(db);
}

std::unique_ptr<hyperpage::page> hyperpage::reader::load(const std::string &page_path)
{
    std::unique_ptr<hyperpage::page> result;
    std::unique_ptr<stored_page> page(new stored_page(get_handle(_handle), page_path));
    if (page->found())
    {
        result.reset(page.release());
    }
    return result;
}

hyperpage::writer::writer(const std::string &db_path) : _handle(nullptr, close_handle<true>)
{
    sqlite3 *db = nullptr;
    if (!sqlite_call(SQLITE_OK, sqlite3_open, db_path.c_str(), &db))
    {
        throw std::runtime_error("Failed to open database: " + db_path);
    }
    const std::string create_table_query =
        "CREATE TABLE IF NOT EXISTS hyperpage ("
        "path TEXT PRIMARY KEY, "
        "mime_type TEXT, "
        "content BLOB);"
        "CREATE UNIQUE INDEX IF NOT EXISTS path_index ON hyperpage (path);";
    sqlite3_exec(db, create_table_query.c_str(), nullptr, nullptr, nullptr);
    _handle.reset(db);
}

void hyperpage::writer::store(const hyperpage::page &page)
{
    sqlite3 *db = get_handle(_handle);
    const std::string query = 
        "INSERT INTO hyperpage (path, mime_type, content) VALUES (?, ?, ?) "
        "ON CONFLICT(path) DO UPDATE SET mime_type=excluded.mime_type, content=excluded.content;";
    sqlite3_stmt *stmt = nullptr;

    sqlite3_prepare_v2(db, query.c_str(), -1, &stmt, nullptr);
    sqlite3_bind_text(stmt, 1, page.get_path().c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_text(stmt, 2, page.get_mime_type().c_str(), -1, SQLITE_STATIC);
    sqlite3_bind_blob(stmt, 3, page.get_content(), static_cast<int>(page.get_length()), SQLITE_STATIC);
    sqlite3_step(stmt);
    sqlite3_finalize(stmt);
}

std::string hyperpage::mime_type(const std::string &path)
{
    const char *mime = getMegaMimeType(path.c_str());
    return std::string(mime ? mime : "application/octet-stream");
}