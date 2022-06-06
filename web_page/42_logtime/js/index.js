/*
https://stackoverflow.com/questions/45697176/send-simple-http-request-with-html-submit-button
    function submit() {
        var xhr = new XMLHttpRequest();
        xhr.onreadystatechange = function () {
            if (xhr.readyState === 4) {
                alert(xhr.response);
            }
        }
        xhr.open('get', 'https://google.com', true);
        xhr.setRequestHeader('Content-Type', 'application/x-www-form-urlencoded; charset=UTF-8');
        xhr.send();
    }
*/

var test = document.getElementById("get_time");
test.onclick = function(){
	const xhr = new XMLHttpRequest()
	xhr.onreadystatechange = function () {
		if (xhr.readyState === 4) {
			alert("NO CONNECTION FOUND");
		}
	}
	var input = document.getElementById("input");
	xhr.open("GET", "new.html");
	xhr.setRequestHeader('Content-Type', 'application/x-www-form-urlencoded; charset=UTF-8');
    xhr.send();
 };