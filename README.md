*This project has been created as part of the 42 curriculum by pcaplat, marudlof and anfouger.*

# Webserv

## Description

**Webserv** is a 42 school project: a fully functional **HTTP/1.1 web server** written from scratch in **C++98**, without relying on any existing server library.

The goal is to understand how web servers work internally by reimplementing the core mechanisms usually handled by software such as Nginx or Apache, and to see what happens behind the scenes when a browser communicates with a web server.

### Features

- Handles **multiple clients simultaneously** using non-blocking sockets and `poll()`
- Parses and processes **HTTP/1.1** requests
- Generates and sends correct HTTP responses
- Supports the **`GET`**, **`POST`** and **`DELETE`** methods
- Handles **HTTP status codes** and errors (default and custom error pages)
- Serves **static files** and **directories**
- Supports **file uploads**
- Executes **CGI** scripts for dynamic content
- Configurable through a **configuration file** (ports, routes, allowed methods, etc.)

The project covers **network programming, sockets, HTTP, I/O multiplexing, request parsing and server architecture**.

## Instructions

### Requirements

- A C++ compiler supporting **C++98** (`c++`, `g++` or `clang++`)
- `make`
- Linux or macOS

### Compilation

```bash
git clone <repository-url> webserv
cd webserv
make
```

Available Makefile rules: `make`, `make clean`, `make fclean`, `make re`.

### Configuration file

The server is configured with a **JSON** file passed as an argument at launch. The file contains either a single server object or an array of server objects, each with its own ports and routes.

#### Configuration fields

> Fields in *italics* are mandatory.

| Field name | JSON type | Description |
| :--------- | :-------- | :---------- |
| *root* | STRING | Root directory of the server |
| *ports* | INT ARRAY | Ports the server listens on |
| *hosts* | STRING ARRAY | Hostnames the server responds to |
| *locations* | OBJECT ARRAY | Server locations (routes) and their rules |
| error_pages | OBJECT | Custom error pages, as `"error_code": "page_path"` |
| default_page | STRING | Default file served when a directory is requested |
| ip | STRING | IP address the server binds to |
| directory_listing | BOOL | Enables or disables directory listing for the server or a specific location |
| max_body_size | INT | Maximum accepted request body size (in bytes) |
| cgis | OBJECT | CGIs handled by the server, as `"extension": "/path/to/cgi"` |
| methods | STRING ARRAY | Allowed HTTP methods for the server or a specific location |
| *path* | STRING | Path of a location |
| redir | OBJECT | Redirections, as `"redirection_code": "redirection_path"` |

#### Examples

##### Minimal configuration

This is the **minimal configuration** required for the server to work properly. Omitted fields are set to default values by the `ConfigLoader` class.

```json
{
    "root": "/server/path",
    "ports": [ 8080 ],
    "hosts": [ "localhost" ],
    "locations": [
        {
            "path": "/"
        }
    ]
}
```

##### Multiple servers configuration

This configuration defines two servers listening on different ports.

```json
[
    {
        "ip": "127.0.0.1",
        "root": "/webserv/castorland/",
        "ports": [ 8080 ],
        "hosts": [ "castorland.fr" ],
        "default_page": "index.html",
        "locations": [
            {
                "path": "/",
                "methods": [ "GET", "POST" ]
            }
        ]
    },
    {
        "root": "/webserv/www",
        "ports": [ 9090 ],
        "hosts": [ "webserv.local" ],
        "error_pages": {
            "404": "404.html"
        },
        "max_body_size": 1048576,
        "locations": [
            {
                "path": "/",
                "methods": [ "GET" ]
            },
            {
                "path": "/upload",
                "methods": [ "POST", "DELETE" ]
            }
        ]
    }
]
```

### Usage

```bash
./webserv [configuration_file]
```

Once the server is running, open `http://localhost:<port>/` in your browser, or test it with `curl`:

```bash
# Simple GET request
curl -v http://localhost:<port>/

# File upload
curl -X POST -F "file=@test.txt" http://localhost:<port>/<upload_route>

# File deletion
curl -X DELETE http://localhost:<port>/<upload_route>/test.txt
```

## Resources

- [RFC 9110 — HTTP Semantics](https://www.rfc-editor.org/rfc/rfc9110)
- [RFC 9112 — HTTP/1.1](https://www.rfc-editor.org/rfc/rfc9112)
- [RFC 3875 — The Common Gateway Interface (CGI) 1.1](https://www.rfc-editor.org/rfc/rfc3875)
- [Beej's Guide to Network Programming](https://beej.us/guide/bgnet/)
- [MDN Web Docs — HTTP](https://developer.mozilla.org/en-US/docs/Web/HTTP)
- [NGINX documentation](https://nginx.org/en/docs/)
- Manual pages: `man socket`, `man poll`, `man fcntl`, `man bind`, `man listen`, `man accept`
