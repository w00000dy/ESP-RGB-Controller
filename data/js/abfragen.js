
function sync() {
    var xhr = new XMLHttpRequest();
    xhr.open("GET", '/sync', true);
    //Send the proper header information along with the request
    xhr.setRequestHeader("Content-Type", "application/x-www-form-urlencoded");
    xhr.onreadystatechange = function () { // Call a function when the state changes.
        if (this.readyState === XMLHttpRequest.DONE && this.status === 200) {
            var x = JSON.parse(xhr.response);
            for (let index = 0; index < x.length; index++) {
                const element = x[index];
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
            setTimeout(sync, 1000);
        }
    }
    xhr.send();
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
