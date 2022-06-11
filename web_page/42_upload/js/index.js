

var ADD = "http://localhost:8000";

const getUp = () => {
var url = new URL(ADD + "/upload.html");
//var params = [['lat', '35.696233'], ['long', '139.570431']];
var params = [];
url.search = new URLSearchParams(params).toString();

fetch(url)

};

const getShow = () => {
	var url = new URL(ADD + "/download.html");
	//var params = [['lat', '35.696233'], ['long', '139.570431']];
	var params = [];
	url.search = new URLSearchParams(params).toString();

fetch(url)
};



(function(){
	let downBtn = document.getElementById("download-button");
	let upBtn = document.getElementById("upload-button");
	downBtn.addEventListener("click", getShow);
	upBtn.addEventListener("click", getUp);
})();
