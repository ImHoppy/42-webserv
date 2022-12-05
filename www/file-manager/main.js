var popup = false;
function DeleteFile(file) {
	// Delete the file
	fetch("/api/delete/" + file, {
		method: "DELETE"
	}).then(function(response) {
		// Reload the page
		location.reload();
	});
}
function UploadFile(file) {
	if (popup)
		return;
	// Upload the file
	// Popup with file input
	var form = document.createElement("form");
	form.innerHTML = "Upload file: <input type='file' name='file' /><br /><input type='submit' value='Upload' />";
	form.id = "popup";
	form.addEventListener("submit", function(e) {
		e.preventDefault();
		var formData = new FormData(form);
		fetch("/api/upload/" + (file == "/" ? "" : file), {
			method: "POST",
			body: formData
		}).then(function(response) {
			location.reload();
		});
		document.getElementById("popup").remove();
		popup = false;
	});
	popup = true;
	document.body.appendChild(form);
}

function GenerateFileDiv(file)
{
	var div = document.createElement("div");
	div.innerHTML = file;

	let button = document.createElement("button");
	if (file.endsWith("/"))
	{
		div.className = "folder";

		button.innerHTML = "Upload";
		button.addEventListener("click", function() {
			UploadFile(file);
		});
	}
	else
	{
		div.className = "file";

		button.innerHTML = "Delete";
		button.addEventListener("click", function() {
			DeleteFile(file);
		});
	}
	div.appendChild(button);
	return div;
}
fetch('/api/list/')
	.then(function(response) {
		// When the page is loaded convert it to text
		return response.text()
	})
	.then(function(html) {
		var parser = new DOMParser();
		var doc = parser.parseFromString(html, "text/html");


		document.getElementById("files").appendChild(GenerateFileDiv('/'));

		doc.querySelectorAll('a').forEach(function(a) {
			console.log(a.text);
			if (a.id != "parent")
				document.getElementById("files").appendChild(GenerateFileDiv(a.text));
		});
		// var docArticle = doc.querySelector('article').innerHTML;
	})
	.catch(function(err) {  
		console.log('Failed to fetch page: ', err);  
	});