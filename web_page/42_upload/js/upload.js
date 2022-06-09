

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
	let photo = document.getElementById("upload-file").files[0];
	let formData = new FormData();
	formData.append("photo", photo);
	fetch('http://localhost:8000', {method: "POST", body: formData});
};

(function(){

	const upBtn = document.getElementById("send-btn");
	upBtn.addEventListener("click", uploadFile);
})();

