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

// hyperpage interface
#include <hyperpage.hpp>

// command line argument parsing
#include <argparse/argparse.hpp>

// mapping input files
#include <mio/mmap.hpp>

// filesystem operations
#include <filesystem>

class mapped_page : public hyperpage::page
{
public:
    mapped_page(const std::filesystem::path &base, const std::filesystem::path &path);
    const std::string &get_path() const override;
    const std::string &get_mime_type() const override;
    const uint8_t *get_content() const override;
    size_t get_length() const override;

private:
    std::string _path;
    std::string _mime_type;
    std::unique_ptr<mio::basic_mmap<mio::access_mode::read, uint8_t>> _mmap;
};

static void run(int argc, char *argv[]);

int main(int argc, char *argv[])
{
    int exit_code(0);
    try
    {
        run(argc, argv);
    }
    catch (const std::exception &e)
    {
        std::cerr << "error: " << e.what() << std::endl;
        exit_code = 1;
    }
    return exit_code;
}

mapped_page::mapped_page(const std::filesystem::path &base, const std::filesystem::path &path)
{
    _path = std::filesystem::relative(path, base).generic_string();
    _path.insert(0, "/"); // Ensure it starts with a slash for web paths
    _mime_type = hyperpage::mime_type(path.filename().string());
    _mmap = std::make_unique<mio::basic_mmap<mio::access_mode::read, uint8_t>>(path.string());
}

const std::string &mapped_page::get_path() const
{
    return _path;
}

const std::string &mapped_page::get_mime_type() const
{
    return _mime_type;
}

const uint8_t *mapped_page::get_content() const
{
    return static_cast<const uint8_t *>(_mmap->data());
}

size_t mapped_page::get_length() const
{
    return _mmap->length();
}

void run(int argc, char *argv[])
{
    argparse::ArgumentParser program("hyperpack");
    std::unique_ptr<hyperpage::writer> writer;

    program.add_argument("directory")
        .help("Directory to scan for files to pack into the hyperpage database")
        .required();
    program.add_argument("-o", "--output")
        .help("Output file for the hyperpage database")
        .default_value("hyperpage.db");
    program.add_argument("-v", "--verbose")
        .help("Show detailed output information")
        .default_value(false)
        .implicit_value(true);
    
    program.parse_args(argc, argv);
    

    auto directory = program.get<std::string>("directory");
    if (!std::filesystem::exists(directory) || !std::filesystem::is_directory(directory))
    {
        throw std::runtime_error("The specified directory does not exist or is not a directory.");
    }

    auto output_file = program.get<std::string>("--output");

    writer = std::make_unique<hyperpage::writer>(output_file);
    for (const auto &entry : std::filesystem::recursive_directory_iterator(directory))
    {
        if (entry.is_regular_file())
        {
            mapped_page page(directory, entry.path());
            writer->store(page);
        }
    }
}