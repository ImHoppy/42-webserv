<?php
	session_start();
?>

<?php
	if (isset($_POST['username']) && isset($_POST['password'])) {
		$_SESSION['username'] = $_POST['username'];
		$_SESSION['password'] = $_POST['password'];
	}
	if (!isset($_SESSION['username'])) {
		header('Location: login.php');
		exit();
	}
?>
<!DOCTYPE html>
<html lang="en">
<head>
	<meta charset="UTF-8">
	<title>g</title>
</head>
<body>
	<div>
		Hello <?php echo $_SESSION['username']; ?>
	</div>
</body>
</html>