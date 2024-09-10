WebServ is a project developed as part of the 42 curriculum. The goal of this project is to create a web server in C++98 that mimics the functionality of an NGINX server. The server is capable of handling configuration files, supporting CGI, directory listing, and custom error pages.

## Features

- **Configuration File**: WebServ uses a configuration file to set up server parameters such as host, port, server names, root directory, and more.
- **CGI Support**: The server can execute CGI scripts written in various languages like PHP, Python, and Perl.
- **Directory Listing**: WebServ can list the contents of directories if configured to do so.
- **Error Pages**: Custom error pages can be specified for different HTTP error codes.
- **Redirection**: The server can handle URL redirections, both internal and external.

## Configuration

The server configuration is defined in a `.conf` file. Below is an example configuration:

- **server**: Defines a server block.
   - **host**: IP address of the server.
   - **port**: Port number the server listens on.
   - **server_names**: List of server names.
   - **max_body_size**: Maximum size of the request body.
   - **root**: Root directory for the server.
   - **error**: Defines custom error pages.
     - **404**: Path to the custom 404 error page.
   - **location**: Defines location blocks.
     - **/**: Root location.
       - **root**: Root directory for this location.
       - **index**: Default file to serve.
       - **method**: Allowed HTTP methods.
       - **dir_list**: Enable directory listing.
       - **redirect**: URL to redirect to.
     - **.php**: CGI configuration for PHP.
       - **cgi_cmd**: Command to execute PHP CGI.
       - **method**: Allowed HTTP methods.

## Usage

1. **Clone the Repository**:
   ```sh
   git clone https://github.com/ImHoppy/42-webserv.git
   cd 42-webserv
   ```

2. **Build the Project**:
   ```sh
   make
   ```

3. **Run the Server**:
   ```sh
   ./webserv path/to/config.conf
   ```

## References

- [NGINX Configuration File Common Mistakes](https://www.nginx.com/resources/wiki/start/topics/tutorials/config_pitfalls/)
- [RFC 7230](https://www.rfc-editor.org/rfc/rfc7230)

---

For more detailed information, refer to the [project checklist](https://github.com/mharriso/school21-checklists/blob/master/ng_5_webserv.pdf).
