document.getElementById("light").addEventListener("click", light);
document.getElementById("rainbow").addEventListener("click", rainbow);
document.getElementById("fire").addEventListener("click", fire);
document.getElementById("weAreNumberOne").addEventListener("click", weAreNumberOne);

function light() {
    document.getElementById('rainbow').checked = false;
    document.getElementById('fire').checked = false;
    sendData("light");
}

function rainbow() {
    document.getElementById('light').checked = false;
    document.getElementById('fire').checked = false;
    sendData("rainbow");
}

function fire() {
    document.getElementById('light').checked = false;
    document.getElementById('rainbow').checked = false;
    sendData("fire");
}

function weAreNumberOne() {
    var data = document.getElementById("weAreNumberOne").checked;

    var xhr = new XMLHttpRequest();
    xhr.open("POST", '/music', true);
    //Send the proper header information along with the request
    xhr.setRequestHeader("Content-Type", "application/x-www-form-urlencoded");
    xhr.onreadystatechange = function () { // Call a function when the state changes.
        if (this.readyState === XMLHttpRequest.DONE && this.status === 200) {
            // Request finished. Do processing here.
        }
    }
    xhr.send("weAreNumberOne=" + data);
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
            syncFire();
        }
    }
    xhr.send("effect=rainbow");
}

function syncFire() {
    var xhr = new XMLHttpRequest();
    xhr.open("POST", '/sync', true);
    //Send the proper header information along with the request
    xhr.setRequestHeader("Content-Type", "application/x-www-form-urlencoded");
    xhr.onreadystatechange = function () { // Call a function when the state changes.
        if (this.readyState === XMLHttpRequest.DONE && this.status === 200) {
            if (xhr.response == "true") {
                document.getElementById('fire').checked = true;
            }
            else {
                document.getElementById('fire').checked = false;
            }
            syncWeAreNumberOne();
        }
    }
    xhr.send("effect=fire");
}

function syncWeAreNumberOne() {
    var xhr = new XMLHttpRequest();
    xhr.open("POST", '/sync', true);
    //Send the proper header information along with the request
    xhr.setRequestHeader("Content-Type", "application/x-www-form-urlencoded");
    xhr.onreadystatechange = function () { // Call a function when the state changes.
        if (this.readyState === XMLHttpRequest.DONE && this.status === 200) {
            if (xhr.response == "true") {
                document.getElementById('weAreNumberOne').checked = true;
            }
            else {
                document.getElementById('weAreNumberOne').checked = false;
            }
            setTimeout(syncLight, 1000);
        }
    }
    xhr.send("music=weAreNumberOne");
}

syncLight();