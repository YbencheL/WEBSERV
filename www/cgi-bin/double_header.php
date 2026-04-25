#!/usr/bin/php-cgi
<?php
// PHP-CGI will output the headers for you. 
// You don't necessarily need to call header() unless you want a custom one.
echo "Content-type: text/html\r\n\r\n"; 
?>
<!DOCTYPE html>
<html>
<head><title>CGI Success</title></head>
<body>
    <h1>PHP CGI is Working!</h1>
    <ul>
        <li><strong>Method:</strong> <?php echo getenv('REQUEST_METHOD'); ?></li>
        <li><strong>Query:</strong> <?php echo getenv('QUERY_STRING'); ?></li>
        <li><strong>Remote Addr:</strong> <?php echo getenv('REMOTE_ADDR'); ?></li>
    </ul>
</body>
</html>