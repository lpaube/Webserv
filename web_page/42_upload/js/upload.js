
var ADD = "http://localhost:8000";

const uploadFile = () => {
	const file = document.getElementById("upload-file").files[0];
	let formData = new FormData();
	formData.append("file", file);
	fetch("ADD", { 
		method: "POST"
		headers: {
			"Content-Type": "files",

		}
};

(function(){

	const upBtn = document.getElementById("send-btn");
	upBtn.addEventListener("click", uploadFile);
})();

