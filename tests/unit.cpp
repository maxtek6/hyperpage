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

#include <maxtest.hpp>

#include <filesystem>

class test_page : public hyperpage::page
{
public:
    test_page(const std::string &path, const std::string &mime_type, const std::string &content) : 
        _path(path), _mime_type(mime_type), _content(content)
    {
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
        return reinterpret_cast<const uint8_t *>(_content.data());
    }

    size_t get_length() const override
    {
        return _content.size();
    }

private:
    std::string _path;
    std::string _mime_type;
    std::string _content;
};

static bool match_buffers(const uint8_t *a, size_t a_len, const uint8_t *b, size_t b_len)
{
    bool result(false);
    if(a_len == b_len)
    {
        result = true;
        int offset = 0;
        while(result && offset < a_len)
        {
            if(a[offset] != b[offset])
            {
                result = false;
            }
            offset++;
        }
    }
    return result;
}

MAXTEST_MAIN
{
    MAXTEST_TEST_CASE(store_load)
    {
        std::filesystem::path db_path = std::filesystem::path(args[0]) / "hyperpage_test.db";
        
        hyperpage::writer writer(db_path.string());

        test_page page("/test1.html", "text/html", "<html><body>Test 1</body></html>");
        writer.store(page);

        hyperpage::reader reader(db_path.string());
        auto loaded_page = reader.load("/test1.html");
        MAXTEST_ASSERT(loaded_page != nullptr);
        MAXTEST_ASSERT(loaded_page->get_path() == page.get_path());
        MAXTEST_ASSERT(loaded_page->get_mime_type() == page.get_mime_type());
        MAXTEST_ASSERT(loaded_page->get_length() == page.get_length());
        MAXTEST_ASSERT(match_buffers(loaded_page->get_content(), loaded_page->get_length(),
                                    page.get_content(), page.get_length()));
        loaded_page = reader.load("/test2.html");
        MAXTEST_ASSERT(loaded_page == nullptr);
    };

    MAXTEST_TEST_CASE(open_database)
    {
        std::filesystem::path valid_path = std::filesystem::path(args[0]) / "hyperpage_test.db";
        std::filesystem::path invalid_path = std::filesystem::path(args[0]);
        std::unique_ptr<hyperpage::reader> reader;
        std::unique_ptr<hyperpage::writer> writer;
        bool exception_thrown = false;

        reader.reset(new hyperpage::reader(valid_path.string()));
        MAXTEST_ASSERT(reader != nullptr);
        writer.reset(new hyperpage::writer(valid_path.string()));
        MAXTEST_ASSERT(writer != nullptr);

        try
        {
            reader.reset(new hyperpage::reader(invalid_path.string()));
        }
        catch(...)
        {
            exception_thrown = true;
        }

        MAXTEST_ASSERT(exception_thrown);

        exception_thrown = false;
        try
        {
            writer.reset(new hyperpage::writer(invalid_path.string()));
        }
        catch(...)
        {
            exception_thrown = true;
        }
        MAXTEST_ASSERT(exception_thrown);
    };

    MAXTEST_TEST_CASE(mime_type)
    {
        const std::string json_path = "test.json";
        const std::string json_mime_type = "application/json";

        auto file_type = hyperpage::mime_type(json_path);
        MAXTEST_ASSERT(file_type == json_mime_type);
    };

    MAXTEST_TEST_CASE(overwrite_test)
    {
        std::filesystem::path db_path = std::filesystem::path(args[0]) / "hyperpage_overwrite_test.db";
        
        // Remove any existing database file to start fresh
        if (std::filesystem::exists(db_path)) {
            std::filesystem::remove(db_path);
        }
        
        hyperpage::writer writer(db_path.string());
        
        // Create a page and store it
        test_page original_page("/index.html", "text/html", "<html><body>Original Content</body></html>");
        writer.store(original_page);

        // Now overwrite with updated content using the same writer
        test_page updated_page("/index.html", "text/html", "<html><body>Updated Content</body></html>");
        writer.store(updated_page);

        // Verify updated content is actually stored
        hyperpage::reader reader(db_path.string());
        auto loaded_page = reader.load("/index.html");
        MAXTEST_ASSERT(loaded_page != nullptr);
        MAXTEST_ASSERT(loaded_page->get_path() == "/index.html");
        MAXTEST_ASSERT(loaded_page->get_mime_type() == "text/html");
        
        // Check updated content - this is where the issue should manifest
        std::string updated_content = "<html><body>Updated Content</body></html>";
        MAXTEST_ASSERT(loaded_page->get_length() == updated_content.size());
        MAXTEST_ASSERT(match_buffers(loaded_page->get_content(), loaded_page->get_length(),
                                    reinterpret_cast<const uint8_t*>(updated_content.data()), updated_content.size()));
    };

    MAXTEST_TEST_CASE(overwrite_cross_writer_test)
    {
        std::filesystem::path db_path = std::filesystem::path(args[0]) / "hyperpage_cross_writer_test.db";
        
        // Remove any existing database file to start fresh
        if (std::filesystem::exists(db_path)) {
            std::filesystem::remove(db_path);
        }
        
        // Create a page and store it with one writer
        {
            hyperpage::writer writer(db_path.string());
            test_page original_page("/index.html", "text/html", "<html><body>Original Content</body></html>");
            writer.store(original_page);
        }
        
        // Read the content to verify it was stored
        {
            hyperpage::reader reader(db_path.string());
            auto loaded_page = reader.load("/index.html");
            MAXTEST_ASSERT(loaded_page != nullptr);
            std::string original_content = "<html><body>Original Content</body></html>";
            MAXTEST_ASSERT(loaded_page->get_length() == original_content.size());
            MAXTEST_ASSERT(match_buffers(loaded_page->get_content(), loaded_page->get_length(),
                                        reinterpret_cast<const uint8_t*>(original_content.data()), original_content.size()));
        }

        // Now overwrite with updated content using a new writer
        {
            hyperpage::writer writer(db_path.string());
            test_page updated_page("/index.html", "text/html", "<html><body>Updated Content</body></html>");
            writer.store(updated_page);
        }

        // Verify updated content is actually stored with a new reader
        {
            hyperpage::reader reader(db_path.string());
            auto loaded_page = reader.load("/index.html");
            MAXTEST_ASSERT(loaded_page != nullptr);
            MAXTEST_ASSERT(loaded_page->get_path() == "/index.html");
            MAXTEST_ASSERT(loaded_page->get_mime_type() == "text/html");
            
            // Check updated content - this is where the issue should manifest
            std::string updated_content = "<html><body>Updated Content</body></html>";
            MAXTEST_ASSERT(loaded_page->get_length() == updated_content.size());
            MAXTEST_ASSERT(match_buffers(loaded_page->get_content(), loaded_page->get_length(),
                                        reinterpret_cast<const uint8_t*>(updated_content.data()), updated_content.size()));
        }
    };
}