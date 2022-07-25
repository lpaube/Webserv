/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   upload.js                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: mafortin <mafortin@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2022/06/09 17:17:01 by mafortin          #+#    #+#             */
/*   Updated: 2022/07/25 15:04:16 by mafortin         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */


var ADD = "http://localhost:8000/web_page/42_upload";
const form = document.querySelector("form");
const check = document.querySelector(".uploaded-area");
const body = document.querySelector("body");
const progressArea = document.querySelector(".progress-area");

const	checkFile = () => {
	fetch("/upload/", {
		method: "GET",
	}).then((result) => { 
		return result.text();
	}).then((content) => {
		console.log(content);
		document.write(content);
	});
}



function showFile(file){
	let progressHTML =  `<li id="injected" class="row">
										<img class="file" src="assets/icons8-file-64.png">
										<div class="content">
											<div class="details">
												<span class="name">${file.name} • Uploading</span>
												<span class="percent">100%</span>
											</div>
											<div class="delete">
												<img class="del_cross" src="assets/icons8-close-48.png">
											</div>
											<div class="progress-bar">
												<div class="progress" style="width:100%"></div>
											</div>
										</div>
											</li>`
	progressArea.innerHTML = progressHTML;
	del = document.querySelector(".del_cross");
	const injected = document.getElementById("injected");
	del.addEventListener("click", () => deleteFile(file, del, injected));
	
}

const uploadFile = () => {
	console.log("Upload file event");
	const fileInput = document.querySelector(".file-input");
	let data = new FormData();
	fileInput.click();
	fileInput.onchange = ({target}) =>{
		let file = target.files[0];
		data.append("file", file);
		console.log("UploadFile: file data done");
		fetch(ADD + "/cgi-bin/upload.py", {
			method: "POST",
			body: data
		}).then(response => {
			console.log("fetch response received");
			if (!response.ok){
				console.log("uploadfile response not ok");
				return response.text();
			}
			else{
				showFile(file);
				return "OK";
			}
		}).then(content =>{
			if (content != "OK"){
				console.log(content);
				document.write(content);
			}
		}).catch(e => {
			console.error(e);
			fetch("error413.html", {
				method: "GET"
			}).then(response => {
				return response.text();
			}).then( content => {
				document.write(content);
			})
		});
	}
}

const deleteFile = (file, del) => {
	console.log("IN1");
	fetch("/upload/" + file.name, {
			method: "DELETE"
		}).then(response => {
			if (!response.ok){
				console.log(response.text());
				return response.text();
			}
			else{
				injected.remove();
				del.remove();
			}
		}).catch(e => alert(e.message));
	}

	/*fileInput.click();
	fileInput.onchange = ({target}) =>
	{
		let request = new XMLHttpRequest();
		let file = target.files[0];
		if (file){
			request.open('POST', "http://127.0.0.1:8000/upload");
			request.upload.addEventListener("progress", ({loaded, total}) => {
				let fileLoaded = Math.floor(loaded / total ) * 100; // getting the progress %
				let fileTotal = Math.floor(total / 1000); //getting the file size in kb
				console.log("FILE SIZE");
				console.log(fileLoaded, fileTotal);
				let progressHTML =  `<li class="row">
										<img class="file" src="assets/icons8-file-64.png">
										<div class="content">
											<div class="details">
												<span class="name">${file.name} • Uploading</span>
												<span class="percent">${fileLoaded}%</span>
											</div>
											<div class="progress-bar">
												<div class="progress" style="width:${fileLoaded}%"></div>
											</div>
										</div>
											</li>`
			progressArea.innerHTML = progressHTML;
			let uploadedArea = `<li class="row">
									<div class="content">
										<img class="file" src="assets/icons8-file-64.png">
										<div class="content">
											<div class="details">
												<span class="name">image_01.png • Uploading</span>
												<span class="size">70 kb</span>
											</div>
											<img class="checkbox" src="assets/icons8-checked-checkbox-100.png">
									</div>
								</li>`
		uploadedArea.innerHTML = uploadedArea;
			});
			let formData = new FormData(form);
			request.send(formData);
		}
	};*/

(function(){
	form.addEventListener("click", uploadFile);
	check.addEventListener("click", checkFile);
})();

