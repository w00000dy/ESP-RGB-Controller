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
        Schwarz();
    }
    else {
        weiss();
    }

}
function Schwarz() {
    document.body.classList.add("dark-mode");

    var buttons = document.getElementsByTagName('button');
    for (var i = 0; i < buttons.length; i++) {
        var button = buttons[i];
        button.classList.add("dark-mode");
    }

}
function weiss() {
    document.body.classList.remove("dark-mode");
    var buttons = document.getElementsByTagName('button');
    for (var i = 0; i < buttons.length; i++) {
        var button = buttons[i];
        button.classList.remove("dark-mode");
    }

}


function setColor(rot, gruen, blau) {
    var xhr = new XMLHttpRequest();
    xhr.open("POST", '/effect', true);
    //Send the proper header information along with the request
    xhr.setRequestHeader("Content-Type", "application/x-www-form-urlencoded");
    xhr.onreadystatechange = function () { // Call a function when the state changes.
        if (this.readyState === XMLHttpRequest.DONE && this.status === 200) {
            window.location.href = '/';
        }
    }
    if (rot == 0 && gruen == 0 && blau == 0) {
        xhr.send("color=false");
    } else {
        xhr.send("color=true&red=" + rot + "&green=" + gruen + "&blue=" + blau);
    }
}

function setEffect(effekt) {
    var xhr = new XMLHttpRequest();
    xhr.open("POST", '/effect', true);
    //Send the proper header information along with the request
    xhr.setRequestHeader("Content-Type", "application/x-www-form-urlencoded");
    xhr.onreadystatechange = function () { // Call a function when the state changes.
        if (this.readyState === XMLHttpRequest.DONE && this.status === 200) {
            window.location.href = '/';
        }
    }
    xhr.send(effekt);

}

anfang();