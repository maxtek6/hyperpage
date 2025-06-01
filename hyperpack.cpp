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

#include <argparse/argparse.hpp>
#include <mio/mmap.hpp>

#include <filesystem>

class mapped_page : public hyperpage::page
{
public:
    mapped_page(const std::filesystem::path &base, const std::filesystem::path &path)
    {
        _path = std::filesystem::relative(path, base).generic_string();
        _path.insert(0, "/"); // Ensure it starts with a slash for web paths
        _mime_type = hyperpage::mime_type(path.filename().string()); // Default MIME type
        _mmap = std::make_unique<mio::basic_mmap<mio::access_mode::read, uint8_t>>(path.string());
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
        return static_cast<const uint8_t*>(_mmap->data());
    }

    size_t get_length() const override
    {
        return _mmap->length();
    }

private:
    std::string _path;
    std::string _mime_type;
    std::unique_ptr<mio::basic_mmap<mio::access_mode::read, uint8_t>> _mmap;
};

int main(int argc, char *argv[])
{
    if (argc > 1)
    {
        hyperpage::writer writer("hyperpage.db");
        std::filesystem::path db_path(argv[1]);
        db_path = std::filesystem::absolute(db_path);
        for (auto entry : std::filesystem::recursive_directory_iterator(db_path))
        {
            if (entry.is_regular_file())
            {
                std::cerr << entry.path() << std::endl;
                mapped_page page(db_path, entry.path());
                writer.store(page);
            }
        }
    }
    return 0;
}