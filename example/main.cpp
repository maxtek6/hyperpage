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

// Hyperpage API
#include <hyperpage.hpp>

// Libevent HTTP server
#include <evhttp.h>

// signal handling
#include <sigfn.hpp>

// std C++ headers
#include <filesystem>
#include <memory>

class server
{
public:
    server(const std::string &dbpath) : _base(event_base_new(), &event_base_free),
                                        _http(evhttp_new(_base.get()), &evhttp_free)
    {
        _reader = std::make_unique<hyperpage::reader>(dbpath);
        evhttp_bind_socket(_http.get(), "localhost", 12345);
        evhttp_set_cb(_http.get(), "/", handle_index, this);
        evhttp_set_gencb(_http.get(), handle_request, this);
    }

    void serve()
    {
        event_base_dispatch(_base.get());
    }

    void shutdown()
    {
        event_base_loopbreak(_base.get());
    }

private:
    static void handle_request(struct evhttp_request *req, void *arg)
    {
        server *self = static_cast<server *>(arg);
        self->load_page(req, evhttp_uri_get_path(evhttp_request_get_evhttp_uri(req)));
    }

    static void handle_index(struct evhttp_request *req, void *arg)
    {
        server *self = static_cast<server *>(arg);
        self->load_page(req, "/index.html");
    }

    void load_page(struct evhttp_request *req, const std::string &path)
    {
        auto page = _reader->load(path);
        if (page)
        {
            evhttp_add_header(req->output_headers, "Content-Type", page->get_mime_type().c_str());
            evbuffer_add(req->output_buffer, page->get_content(), page->get_length());
            evhttp_send_reply(req, HTTP_OK, "OK", req->output_buffer);
        }
        else
        {
            evhttp_send_error(req, HTTP_NOTFOUND, "Page not found");
        }
    }
    std::unique_ptr<hyperpage::reader> _reader;
    std::unique_ptr<event_base, decltype(&event_base_free)> _base;
    std::unique_ptr<evhttp, decltype(&evhttp_free)> _http;
};

int main(int argc, char *argv[])
{
    const std::filesystem::path db_path = std::filesystem::canonical(argv[0]).parent_path() / "hyperpage.db"; 
#ifdef _WIN32
    WSADATA wsa_data;
    WSAStartup(MAKEWORD(2, 2), &wsa_data);
#endif
    std::unique_ptr<server> app;

    app = std::make_unique<server>(db_path.string());
    
    if (app)
    {
        sigfn::handler_function handler = [&app](int)
        {
            app->shutdown();
        };
        sigfn::handle(SIGINT, handler);
        sigfn::handle(SIGTERM, handler);
        app->serve();
    }
#ifdef _WIN32
    WSACleanup();
#endif
    return 0;
}