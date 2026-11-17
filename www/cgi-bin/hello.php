<?php
header("Content-Type: text/html");
?>
<h1>Hello depuis un CGI PHP</h1>
<p>Methode : <?php echo htmlspecialchars($_SERVER["REQUEST_METHOD"]); ?></p>
<p>Query string : <?php echo htmlspecialchars($_SERVER["QUERY_STRING"]); ?></p>
<p>Dossier courant : <?php echo htmlspecialchars(getcwd()); ?></p>
<p>Heure serveur : <?php echo date("H:i:s"); ?></p>
