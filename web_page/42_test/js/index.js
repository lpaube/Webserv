  (function(){
    const userinput = document.getElementById("input");
    const apikey = document.getElementById("apikey");
    const uid = document.getElementById("uid");
	const env = document.getElementById("env");
    var get_test_html = document.getElementById("test_html");

	env.onclick = () => {
		fetch("http://127.0.0.1:9000/web_page/42_test/cgi-bin/env.sh", {
			method: "GET"
		  }).then(response => response.text())
			.then((content) => {
			  document.write(content);
			});
	}
	
    get_test_html.onclick = () =>	{
      fetch("test.html", {
        method: "GET"
      }).then(response => response.text())
        .then((content) => {
          document.write(content);
        });
    }

    var error_404 = document.getElementById("error_404");
    error_404.onclick = () =>	{
      fetch("thisfiledoesntexit", {
        method: "GET"
      }).then(response => response.text())
        .then((content) => {
          document.write(content);
        });
    }

    var delete_method = document.getElementById("delete_method");
    delete_method.onclick = () =>	{
      fetch("/42_test/special_directory/test1.txt", {
        method: "DELETE"
      }).then(response => response.text())
        .then((content) => {
          document.write(content);
        });
    }

    var redirection = document.getElementById("redirection");
    redirection.onclick = () =>	{
      fetch("/42_test/special_directory/special_path/", {
        method: "GET"
      }).then(response => response.text())
        .then((content) => {
          document.write(content);
        });
    }

    var index_test = document.getElementById("index_test");
    index_test.onclick = () =>	{
      fetch("/42_test/index_directory/", {
        method: "GET"
      }).then(response => response.text())
        .then((content) => {
          document.write(content);
        });
    }

    var autoindex_test = document.getElementById("autoindex_test");
    autoindex_test.onclick = () =>	{
      fetch("/42_test/autoindex_directory/", {
        method: "GET"
      }).then(response => response.text())
        .then((content) => {
          document.write(content);
        });
    }

    var test_button = document.getElementById("test_button");
    test_button.onclick = () =>	{
      fetch("/42_test/fasdfafsa", {
        method: "GET"
      }).then(response => response.text())
        .then((content) => {
          document.write(content);
        });
    }

  })();
