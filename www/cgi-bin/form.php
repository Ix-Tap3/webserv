<?php
header("Content-Type: text/html");
$name = "";
if (isset($_GET["name"])) {
    $name = $_GET["name"];
}
if (isset($_POST["name"])) {
    $name = $_POST["name"];
}
?>
<h1>Formulaire PHP (<?php echo $_SERVER["REQUEST_METHOD"]; ?>)</h1>
<?php if ($name !== "") { ?>
<p>Bonjour <b><?php echo htmlspecialchars($name); ?></b> !</p>
<?php } ?>
<form method="GET" action="/cgi-bin/form.php">
  <input name="name" placeholder="nom (GET)"> <button>Envoyer en GET</button>
</form>
<form method="POST" action="/cgi-bin/form.php">
  <input name="name" placeholder="nom (POST)"> <button>Envoyer en POST</button>
</form>
