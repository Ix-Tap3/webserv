<?php
header("Content-Type: text/html");
$dir = __DIR__ . "/../upload/";
echo "<h1>Upload PHP</h1>";
if ($_SERVER["REQUEST_METHOD"] === "POST") {
    if (isset($_FILES["file"]) && $_FILES["file"]["error"] === UPLOAD_ERR_OK) {
        $name = basename($_FILES["file"]["name"]);
        if (move_uploaded_file($_FILES["file"]["tmp_name"], $dir . $name)) {
            echo "<p>Fichier <b>" . htmlspecialchars($name) . "</b> enregistre (" . $_FILES["file"]["size"] . " octets).</p>";
        } else {
            echo "<p>Erreur lors de l'enregistrement.</p>";
        }
    } else {
        echo "<p>Aucun fichier recu.</p>";
    }
}
?>
<form method="POST" action="/cgi-bin/upload.php" enctype="multipart/form-data">
  <input type="file" name="file"> <button>Envoyer</button>
</form>
