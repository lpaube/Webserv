

const sendHTTPRequest = (method, url, data) => {
	const promise = new Promise((resolve, reject) => {
	const xhr =  new XMLHttpRequest();
	xhr.open(method, url);
	xhr.responseType = "json";
	xhr.onload = () => {
		resolve(xhr.response);
	};
	xhr.send(data);
	});
	return promise;
};


const uploadFile = () => {
	const file = document.getElementById("upload-file").files[0];
	let req = new XMLHttpRequest();
	let formData = new FormData();
	formData.append("file", file);
};

(function(){

	const upBtn = document.getElementById("send-btn");
	upBtn.addEventListener("click", uploadFile);
})();

