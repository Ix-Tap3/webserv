<?php
header("Content-Type: text/plain");
$body = file_get_contents("php://input");
echo "Recu " . strlen($body) . " octets (CONTENT_TYPE=" . $_SERVER["CONTENT_TYPE"] . ") :\n";
echo $body;
if (!empty($_POST)) {
    echo "\n\n\$_POST :\n";
    print_r($_POST);
}
