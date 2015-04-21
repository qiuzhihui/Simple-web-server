# Simple-web-server
A simple web server can support multiple client by fork.

One main process keep listening the request from the socket. When a new request comes in it forks another process to handle the client.
It will send an HTML page which contains information latest 10 client that access to this website. And also have a link to an about me page.
