<?php
if ($_SERVER['REQUEST_METHOD'] === 'POST') {
    header("Content-Type: text/html");
    $name = $_POST['name'];
    $message = $_POST['message'];
    echo "<!DOCTYPE html><html><body>";
    echo "<h1>POST works!</h1>";
    echo "<p>Name: " . $name . "</p>";
    echo "<p>Message: " . $message . "</p>";
    echo "<a href='/cgi-bin/post_php_test.php'>Go back</a>";
    echo "</body></html>";
} else {
    header("Content-Type: text/html");
    echo "<!DOCTYPE html><html><body>";
    echo "<h1>POST CGI Test</h1>";
    echo '<form method="POST" action="/cgi-bin/post_php_test.php">';
    echo '<input type="text" name="name" placeholder="Your name"><br><br>';
    echo '<input type="text" name="message" placeholder="Your message"><br><br>';
    echo '<button type="submit">Send POST</button>';
    echo "</form>";
    echo "</body></html>";
}
?>