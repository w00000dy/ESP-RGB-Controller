document.getElementById("light").addEventListener("click", light);
document.getElementById("rainbow").addEventListener("click", rainbow);

function light() {
    document.getElementById('rainbow').checked = false;
    sendData("light");
}

function rainbow() {
    document.getElementById('light').checked = false;
    sendData("rainbow");
}

function sendData(effect) {
    var data = document.getElementById(effect).checked;

    console.log(effect + ": " + data);

    var xhr = new XMLHttpRequest();
    xhr.open("POST", '/effect', true);
    //Send the proper header information along with the request
    xhr.setRequestHeader("Content-Type", "application/x-www-form-urlencoded");
    xhr.onreadystatechange = function () { // Call a function when the state changes.
        if (this.readyState === XMLHttpRequest.DONE && this.status === 200) {
            // Request finished. Do processing here.
        }
    }
    xhr.send(effect + "=" + data);
}

function syncLight() {
    var xhr = new XMLHttpRequest();
    xhr.open("POST", '/sync', true);
    //Send the proper header information along with the request
    xhr.setRequestHeader("Content-Type", "application/x-www-form-urlencoded");
    xhr.onreadystatechange = function () { // Call a function when the state changes.
        if (this.readyState === XMLHttpRequest.DONE && this.status === 200) {
            if (xhr.response == "true") {
                document.getElementById('light').checked = true;
            }
            else {
                document.getElementById('light').checked = false;
            }
            syncRainbow();
        }
    }
    xhr.send("effect=light");
}

function syncRainbow() {
    var xhr = new XMLHttpRequest();
    xhr.open("POST", '/sync', true);
    //Send the proper header information along with the request
    xhr.setRequestHeader("Content-Type", "application/x-www-form-urlencoded");
    xhr.onreadystatechange = function () { // Call a function when the state changes.
        if (this.readyState === XMLHttpRequest.DONE && this.status === 200) {
            if (xhr.response == "true") {
                document.getElementById('rainbow').checked = true;
            }
            else {
                document.getElementById('rainbow').checked = false;
            }
            setTimeout(syncLight, 1000);
        }
    }
    xhr.send("effect=rainbow");
}

syncLight();