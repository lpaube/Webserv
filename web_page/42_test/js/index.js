  (function(){
    const userinput = document.getElementById("input");
    const apikey = document.getElementById("apikey");
    const uid = document.getElementById("uid");

    var get_test_html = document.getElementById("test_html");
    get_test_html.onclick = () =>	{
      fetch("http://127.0.0.1:8000/web_page/42_test/test.html", {
        method: "GET"
      }).then(response => response.text())
        .then((content) => {
          document.write(content);
        });
    }

    var image_test = document.getElementById("image_test");
    image_test.onclick = () =>	{
      fetch("http://127.0.0.1:8000/web_page/42_test/test_image.png", {
        method: "GET"
      }).then(response => response.text())
        .then((content) => {
          document.write(content);
        });
    }

    var error_404 = document.getElementById("error_404");
    error_404.onclick = () =>	{
      fetch("http://127.0.0.1:8000/web_page/42_test/thisfiledoesntexit", {
        method: "GET"
      }).then(response => response.text())
        .then((content) => {
          document.write(content);
        });
    }

  })();
