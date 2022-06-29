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
		if (userinput.value.length == 0 ){
			alert("Empty field!");
			return ;
		}
		fetch("http://127.0.0.1:7000/web_page/42_logtime/cgi-bin/get_time.py?" + new URLSearchParams(
			{ user: userinput.value, test: "test"}
		), {
			method: "GET",
			headers: {}
		}).then(response => response.text())
		.then((content) => {
			alert("Nombre d'heure(s) ce mois-ci : " + content);
		});
	}
  })();
  
