#!/usr/bin/php-cgi
<?php
// Note: php-cgi automatically sends "X-Powered-By" and "Content-type" headers.
echo "Custom-Header: Webserv-Test\r\n";
echo "\r\n";
?>
<!DOCTYPE html>
<html>
<head><title>CGI Test</title></head>
<body>
    <h1>CGI Status: Success</h1>
    <p><b>Method:</b> <?php echo getenv('REQUEST_METHOD'); ?></p>
    <p><b>Query:</b> <?php echo getenv('QUERY_STRING'); ?></p>
    <p><b>Post Data:</b> <?php echo file_get_contents('php://input'); ?></p>
</body>
</html>