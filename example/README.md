# Example: React over HTTP

This example is intended to demonstrate how a ReactJS app can be stored 
in the hyperpage database and then served over HTTP.

## Basic Mechanism

During the CMake build process, the app is built using `vite`. Using the 
resulting `dist` directory, `hyperpack` archives its contents. In the 
C++ code, a Libevent HTTP server uses a `hyperpage::reader` to load 
the contents and serve them over HTTP using the URI path from each HTTP 
request to determine what content to serve.

## Relevant Code

In `CMakeLists.txt`:

```cmake
add_custom_command(
    TARGET server POST_BUILD
    COMMAND $<TARGET_FILE:hyperpack> -o $<TARGET_FILE_DIR:server>/hyperpage.db ${CMAKE_CURRENT_SOURCE_DIR}/react-app/dist
    COMMENT "Building hyperpack archive from React app dist folder"
)
```

This is a custom command run after building the C++ code. It uses the 
`dist` folder to build a hyperpage database.

In `main.cpp`:

```c++
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
```

This function takes a path and loads a page from the `_reader object`, 
writing the page info and contents to the `evhttp_request` on if the 
page was found, and sending a 404 error if the path did not represent 
an entry in the database.