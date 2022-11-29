#!/bin/bash
export REDIRECT_STATUS=true
export GATEWAY_INTERFACE=CGI/1.1
export SCRIPT_FILENAME=/mnt/nfs/homes/cdefonte/webserv/prout/cdefonte_vs/posttest.php
export SCRIPT_NAME=posttest.php
export PATH_INFO=/
export SERVER_PROTOCOL=HTTP/1.1
export CONTENT_TYPE=application/x-www-form-urlencoded
export CONTENT_LENGTH=20
export REQUEST_METHOD=POST
export QUERY_STRING=""
php-cgi
