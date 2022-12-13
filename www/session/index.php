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
	<div id="main">
		Hello <?php echo $_SESSION['username']; ?>
		<input id="logoutButton" type="submit" value="Logout"/>
	</div>
</body>
<script>
	var logoutButton = document.getElementById('logoutButton');
	logoutButton.addEventListener('click', function() {
		fetch("logout.php", {
			method: "DELETE"
		}).then(function(response) {
			if (response.status == 200) {
				document.getElementById('main').innerHTML = "Succesfully logged out.";
			}
		})
	});
	
</script>
</html>