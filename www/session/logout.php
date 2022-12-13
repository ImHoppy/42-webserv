<?php
	session_start();
?>

<?php
	if (!isset($_SESSION['username'])) {
		header('Location: login.php');
		exit();
	}
	if ($_SERVER["REQUEST_METHOD"] != "DELETE") {
		header('Location: index.php');
		exit();
	}
	session_destroy();
?>

<!DOCTYPE html>
<html lang="en">
<head>
	<meta charset="UTF-8">
	<title>Logout</title>
</head>
<body>
	<div>
		Succesfully logged out.
	</div>
</body>
</html>
