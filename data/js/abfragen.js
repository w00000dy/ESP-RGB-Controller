/*
    https://www.w3schools.com/js/js_cookies.asp
    Then it checks if the String says balck and if thats is true it turns the page into dark-mode.
    It calls a function the  calls a another function that checks wich the site and tells the scipt which fuction to use.
*/
function anfang() {
    var name = "modus=";
    var decodedCookie = decodeURIComponent(document.cookie);
    var ca = decodedCookie.split(';');
    for (var i = 0; i < ca.length; i++) {
        var c = ca[i];
        while (c.charAt(0) == ' ') {
            c = c.substring(1);
        }
        if (c.indexOf(name) == 0) {
            var x = c.substring(name.length, c.length);
        }
    }

    console.log(x);

    if (x == "schwarz") {
        callSchwarz();
    }
    else {
        callWeiss();
    }

}
/*
    Function which sets the main page to black. First body black than it loops through
    the buttons. After that the select element and the input button, gets set into dark mode.
    The darkmode is set through adding the elements to the class dark-mode.
*/
function setSchwarz() {
    document.body.classList.add("dark-mode");
    var buttons = document.getElementsByTagName('button');
    for (var i = 0; i < buttons.length; i++) {
        var button = buttons[i];
        button.classList.add("dark-mode");
    }
    var select = document.getElementById("farbeweb");
    select.classList.add("dark-mode");
    var input = document.getElementById("pommes");
    input.classList.add("dark-mode");
}
/*
    Its like getSchwarz with the diffrence that its used to set modi.html and farben.html into darkmode
    without the select and input element.
*/
function Schwarz() {
    document.body.classList.add("dark-mode");
    var buttons = document.getElementsByTagName('button');
    for (var i = 0; i < buttons.length; i++) {
        var button = buttons[i];
        button.classList.add("dark-mode");
    }
}
/*
    It´s a Method to set a cookie with the input from the select element from Settings with the id "farbeweb".
    The cookie expieres after 1000d. The cookie is samesite stric set.
*/
function setCookie() {
    var e = document.getElementById("farbeweb")
    var modus = e.options[e.selectedIndex].value;
    document.cookie = "modus=" + modus + "; max-age=86400000; samesite=strict; path=/";
    anfang();

}
/*
    It revertes the changes from getSchwarz through removeing them from the class dark-mode.
*/

function setWeiss() {
    document.body.classList.remove("dark-mode");
    var buttons = document.getElementsByTagName('button');
    for (var i = 0; i < buttons.length; i++) {
        var button = buttons[i];
        button.classList.remove("dark-mode");
    }
    var select = document.getElementById("farbeweb");
    select.classList.remove("dark-mode");
    var input = document.getElementById("pommes");
    input.classList.remove("dark-mode");
}
/*
    Like Schawrz().
*/
function Weiss() {
    document.body.classList.remove("dark-mode");
    var buttons = document.getElementsByTagName('button');
    for (var i = 0; i < buttons.length; i++) {
        var button = buttons[i];
        button.classList.remove("dark-mode");
    }
}


function sync() {
    var xhr = new XMLHttpRequest();
    xhr.open("GET", '/sync', true);
    //Send the proper header information along with the request
    xhr.setRequestHeader("Content-Type", "application/x-www-form-urlencoded");
    xhr.onreadystatechange = function () { // Call a function when the state changes.
        if (this.readyState === XMLHttpRequest.DONE && this.status === 200) {
            var data = JSON.parse(xhr.response);
            for (let index = 0; index < data.length; index++) {
                const element = data[index];


            }
            setTimeout(sync, 1000);
        }
    }
    xhr.send();
}
function aendern2() {
    var x = JSON.parse(xhr.response);
    if (x[1].acitve == "true") {
        var r = x[1].red;
        var g = x[1].green;
        var b = x[1].blue;
        aendern(r, g, b);
    }
    else if (x[2] == "true") {
        RGB();
    }
    else if (x[3] == "true") {
        Fire();
    }
    else if (x[4] == "true") {
        Random();
    }
}
function aendern(r, g, b) {
    document.getElementById("dings").innerHTML = "Color";
    document.getElementById("dings").style.color = "rgb(" + r + "," + g + "," + b + ")";
    document.getElementById("dings").classList.remove('colorchangeFast');
    document.getElementById("dings").classList.remove('Fire');
}
function RGB() {
    document.getElementById("dings").innerHTML = "RGB";
    document.getElementById("dings").classList.add('colorchangeFast');
    document.getElementById("dings").classList.remove('Fire');

}
function Fire() {
    document.getElementById("dings").innerHTML = "Fire";
    document.getElementById("dings").classList.remove = 'colorchangeFast';
    document.getElementById("dings").classList.add('Fire');


}
function Random() {
    document.getElementById("dings").innerHTML = "Randome";
    document.getElementById("dings").classList.remove = 'colorchangeFast';
    document.getElementById("dings").style.color = "Lime";
    document.getElementById("dings").classList.remove('Fire');

}
//Methods caller
aendern2();
anfang();