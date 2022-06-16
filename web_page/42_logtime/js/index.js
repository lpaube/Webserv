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


  (function(){
	const userinput = document.getElementById("input");
	var get_time = document.getElementById("get_time");
	get_time.onclick = () =>	{
		fetch("http://127.0.0.1:8000/cgi-bin/get_time.py?" + new URLSearchParams(
			{ user: userinput.value, test: "test"}
		), {
			method: "GET"
		}).then(response => response.text())
		.then((content) => {
			document.write(content);
		});
	}

    var get_time = document.getElementById("test_html");
    get_time.onclick = () =>	{
      fetch("./test.html", {
        method: "GET"
      }).then(response => response.text())
        .then((content) => {
          document.write(content);
        });
    }
  })();
  
