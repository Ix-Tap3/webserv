<?php
header("Content-Type: text/html");
echo "<h1>\$_SERVER</h1><table border='1'>";
ksort($_SERVER);
foreach ($_SERVER as $key => $value) {
    if (is_array($value)) {
        $value = implode(" ", $value);
    }
    echo "<tr><td>" . htmlspecialchars($key) . "</td><td>" . htmlspecialchars((string)$value) . "</td></tr>";
}
echo "</table>";
