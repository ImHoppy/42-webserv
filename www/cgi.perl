#!/bin/perl

print("Content-Type: text/html\r\n\r\n");
print("<title>Perl CGI script</title>\n");
print("<H1>Hello from Perl CGI !</H1>\n");
print("Hello, world!\n");
print("<style type=\"text/css\">.key { color: blue;} .value { color:red;}</style>\n");
print "<ul>\n";
foreach $key (sort keys(%ENV)) {
	print "<li><span class=\"key\">$key</span> = <span class=\"value\">$ENV{$key}</span>";
}
print "</ul>\n";
