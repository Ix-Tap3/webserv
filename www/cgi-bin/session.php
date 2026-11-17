<?php
session_save_path(sys_get_temp_dir());
session_start();
if (!isset($_SESSION["visits"])) {
    $_SESSION["visits"] = 0;
}
$_SESSION["visits"]++;
header("Content-Type: text/html");
?>
<h1>Session PHP</h1>
<p>Session id : <?php echo htmlspecialchars(session_id()); ?></p>
<p>Nombre de visites : <?php echo $_SESSION["visits"]; ?></p>
