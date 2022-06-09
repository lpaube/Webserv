
//EXEMPLE DE POST
	/*let data;
	sendHTTPRequest("POST", "https://reqres.in/api/register", {
		email: 'test@test.com',
		passord: 'testpwd'
	}).then(data => {
	console.log(data);*/

//EXEMPLE DE FILE UPLOAD
/*let photo = document.getElementById("image-file").files[0];  // file from input
let req = new XMLHttpRequest();
let formData = new FormData();

formData.append("photo", photo);                                
req.open("POST", '/upload/image');
req.send(formData);*/

const sendHTTPRequest = (method, url, data) => {
	const promise = new Promise((resolve, reject) => {
	const xhr =  new XMLHttpRequest();
	xhr.open(method, url);
	xhr.responseType = "json";
	xhr.onload = () => {
		resolve(xhr.response);
	};
	xhr.send();
	});
	return promise;
};

const getDownload = () => {
	let data;
	sendHTTPRequest('GET', "download.html").then(data => {
	console.log(data);
});
};

const getUpload = () => {
	sendHTTPRequest('POST', 'http://localhost:8000', "upload.html").then(data => {
		console.log(data);
	});
};

const	uploadFile = () => {
	const file = document.getElementById("upload-field").files[0];
	console.log(file);
}

(function(){
	let downBtn = document.getElementById("download-button");
	let upBtn = document.getElementById("upload-button");
	downBtn.addEventListener("click", getDownload);
	upBtn.addEventListener("click", getUpload);
})();
