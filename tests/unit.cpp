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

MAXTEST_MAIN
{
    MAXTEST_TEST_CASE(store_load)
    {
        std::filesystem::path db_path = std::filesystem::path(args[0]) / "hyperpage_test.db";
        
    };
}