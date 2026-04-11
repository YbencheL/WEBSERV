#!/usr/bin/php-cgi

<?php
header("Content-Type: text/html");
?>
<!DOCTYPE html>
<html>
<body>
    <h1>PHP CGI Test</h1>
    <p>PHP Version: <?php echo phpversion(); ?></p>
    <p>Request Method: <?php echo $_SERVER['REQUEST_METHOD']; ?></p>
    <p>Query String: <?php echo $_SERVER['QUERY_STRING']; ?></p>
    <p>Script Name: <?php echo $_SERVER['SCRIPT_NAME']; ?></p>
</body>
</html>
