# Webserv

**Webserv** is a C++98 project focused on building a fully functional **HTTP/1.1 web server from scratch**.

The goal is to understand how web servers work internally by implementing the core mechanisms usually handled by software such as Nginx or Apache, without relying on existing server libraries.

### Main Objectives

* Build a server capable of handling multiple HTTP clients.
* Parse and process HTTP/1.1 requests.
* Generate and send correct HTTP responses.
* Support different HTTP methods such as `GET`, `POST`, and `DELETE`.
* Handle HTTP status codes and errors properly.
* Implement configuration files to control server behavior.
* Serve static files and directories.
* Support file uploads.
* Implement CGI execution for dynamic content.
* Manage multiple connections efficiently using non-blocking I/O and `poll()`.

The project is mainly about understanding **network programming, sockets, HTTP, I/O multiplexing, request parsing, and server architecture**.

In short, the objective is to build a small HTTP server from the ground up and understand what happens behind the scenes when a browser communicates with a web server.
