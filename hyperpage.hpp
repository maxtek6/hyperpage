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

#ifndef HYPERPAGE_HPP
#define HYPERPAGE_HPP

/**
 * @file hyperpage.hpp
 * @brief Hyperpage API header file
 * @author John R. Patek Sr.
 */

#include <functional>
#include <memory>
#include <stdexcept>
#include <string>

namespace hyperpage
{
    /**
     * @brief page
     *
     * @class abstract class representing an entry in the hyperpage
     * database.
     */
    class page
    {
    public:
        /**
         *  @brief gets the URI path of the page.
         */
        virtual const std::string &get_path() const = 0;

        /**
         *  @brief gets the MIME type of the page.
         */
        virtual const std::string &get_mime_type() const = 0;

        /**
         *  @brief gets the content of the page.
         *
         *  @return a pointer to the content data.
         */
        virtual const uint8_t *get_content() const = 0;

        /**
         *  @brief gets the content length of the page.
         *
         *  @return the length of the content in bytes.
         */
        virtual size_t get_length() const = 0;
    };

    /**
     *  @brief reader
     *
     *  @class class for loading pages from the hyperpage database.
     */
    class reader
    {
    public:
        /**
         *  @brief Constructs a reader for the hyperpage database.
         *
         *  @param db_path The path to the hyperpage database file.
         */
        reader(const std::string &db_path);

        /**
         *  @brief Loads a page from the hyperpage database.
         *
         *  @param page_path The path of the page to load.
         *  @return A unique pointer to the loaded page, or nullptr if not found.
         */
        std::unique_ptr<page> load(const std::string &page_path);

    private:
        std::unique_ptr<void, std::function<void(void *)>> _handle;
    };

    /**
     *  @brief writer
     *
     *  @class class for storing pages in the hyperpage database.
     */
    class writer
    {
    public:
        /**
         *  @brief Constructs a writer for the hyperpage database.
         *
         *  @param db_path The path to the hyperpage database file.
         */
        writer(const std::string &db_path);

        /**
         *  @brief Stores a page in the hyperpage database.
         *
         *  @param page The page to store.
         */
        void store(const page &page);

    private:
        std::unique_ptr<void, std::function<void(void *)>> _handle;
    };

    std::string mime_type(const std::string &path);
}

#endif