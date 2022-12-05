<?php
    session_start();
?>

<?php
    // If the user is logged in, redirect to index.php
    if (isset($_SESSION['username'])) {
        header('Location: index.php');
        exit();
    }
?>


<html>
	<head>
		<title>Login</title>
	</head>
	<body>
		<form action="index.php" method="post">
			<label for="username">Username:</label>
			<input type="text" name="username" id="username" />
			<label for="password">Password:</label>
			<input type="password" name="password" id="password" />
			<input type="submit" name="login" value="Login" />
		</form>

	</body>
</html>
