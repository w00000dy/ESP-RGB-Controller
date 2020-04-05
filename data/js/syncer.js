document.getElementById("switch0").addEventListener("click", light); // Light
document.getElementById("switch1").addEventListener("click", rainbow);  // Rainbow
document.getElementById("switch2").addEventListener("click", fire); // Fire
document.getElementById("switch3").addEventListener("click", weAreNumberOne); // We are number one

function light() {
    document.getElementById('switch1').checked = false;
    document.getElementById('switch2').checked = false;
    var value = document.getElementById("switch0").checked;
    sendData("light", value);
}

function rainbow() {
    document.getElementById('switch0').checked = false;
    document.getElementById('switch2').checked = false;
    var value = document.getElementById("switch1").checked;
    sendData("rainbow", value);
}

function fire() {
    document.getElementById('switch0').checked = false;
    document.getElementById('switch1').checked = false;
    var value = document.getElementById("switch2").checked;
    sendData("fire", value);
}

function weAreNumberOne() {
    var data = document.getElementById("switch3").checked;

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

function sendData(effect, data) {
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

// this is for synchronizing the buttons on multiple devices
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
                if (element == "true") {
                    document.getElementById('switch' + String(index)).checked = true;
                }
                else {
                    document.getElementById('switch' + String(index)).checked = false;
                }
            }
            setTimeout(sync, 1000);
        }
    }
    xhr.send();
}

sync();