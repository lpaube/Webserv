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
const userinput = document.querySelector(".user_input").value;
console.log(userinput);
var get_time = document.getElementById("get_time");
get_time.onclick = () =>{
		console.log("HERE");
		fetch("http://127.0.0.1:8000/cgi-bin/get_time.py?user=" + userinput, {
			method: "GET"
		}).then((result) => {
			return result.text();
		}).then((content) => {
			alert(content);
		});
	}
})();