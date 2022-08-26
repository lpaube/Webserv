![webserv logo](img/Webserv.png)

`webserv` is an HTTP/1.1 server written in C++98. The functionalities and configurations are heavily based on Nginx's.

**Usage**
---
1. Compile the program with `make`.
2. Launch `webserv` with a configuration file. 

    ```
    ./webserv 42_test_config.txt 
    ```
3. Launch an HTTP request with a browser by entering as the url the same IP/Port address as specified by the `listen` directive of the configuration file (i.e. `127.0.0.1:8000`).

**Configuration File**
---
  - **`listen`** `address:port`<br />
    The IP/Port address combination that this server block is designed to respond to.
  - **`error_page`** `code ... uri`<br />
    Configures the server’s statuses and redirections when errors occur.
  - **`client_max_body_size`** `size`<br />
    Sets the maximum file upload size.
  - **`limit_except`** `method`<br />
    Limits allowed HTTP methods.
  - **`root`** `path`<br />
    Sets the root directory for requests.
  - **`autoindex`** `on | off`<br />
    Enables or disables the directory listing output.
  - **`index`** `file ...`<br />
    Defines files that will be used by default when no specific file is requested.
  - **`cgi_ext`** `extension binary`<br />
    Defines the possible executable CGI scripts.