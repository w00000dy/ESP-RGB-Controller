document.getElementById("switch0").addEventListener("click", light); // Light
document.getElementById("switch1").addEventListener("click", color); // Color
document.getElementById("color").addEventListener("change", colorChange); // Color
document.getElementById("switch2").addEventListener("click", rainbow);  // Rainbow
document.getElementById("switch3").addEventListener("click", fire); // Fire
document.getElementById("switch4").addEventListener("click", random); // Random

function light() {
    var value = document.getElementById("switch0").checked;
    sendData("light", value);
}

function color() {
    var value = document.getElementById("switch1").checked;
    if (value == true) {
        document.getElementById("color").value = "#00ff00";
        value = "#00ff00";

        var xhr = new XMLHttpRequest();
        xhr.open("POST", '/effect', true);
        //Send the proper header information along with the request
        xhr.setRequestHeader("Content-Type", "application/x-www-form-urlencoded");
        xhr.onreadystatechange = function () { // Call a function when the state changes.
            if (this.readyState === XMLHttpRequest.DONE && this.status === 200) {
                getStatus();
            }
        }
        xhr.send("color=" + value + "&red=" + hexToRgb(value).r + "&green=" + hexToRgb(value).g + "&blue=" + hexToRgb(value).b);
    }
    else {
        sendData("color", value);
    }
}

function colorChange() {
    var value = document.getElementById("switch1").checked;
    if (value == true) {
        value = document.getElementById("color").value;

        var xhr = new XMLHttpRequest();
        xhr.open("POST", '/effect', true);
        //Send the proper header information along with the request
        xhr.setRequestHeader("Content-Type", "application/x-www-form-urlencoded");
        xhr.onreadystatechange = function () { // Call a function when the state changes.
            if (this.readyState === XMLHttpRequest.DONE && this.status === 200) {
                getStatus();
            }
        }
        xhr.send("color=" + value + "&red=" + hexToRgb(value).r + "&green=" + hexToRgb(value).g + "&blue=" + hexToRgb(value).b);
    }
}

function rainbow() {
    var value = document.getElementById("switch2").checked;
    sendData("rainbow", value);
}

function fire() {
    var value = document.getElementById("switch3").checked;
    sendData("fire", value);
}

function random() {
    var value = document.getElementById("switch4").checked;
    sendData("random", value);
}

function sendData(effect, data) {
    console.log(effect + ": " + data);

    var xhr = new XMLHttpRequest();
    xhr.open("POST", '/effect', true);
    //Send the proper header information along with the request
    xhr.setRequestHeader("Content-Type", "application/x-www-form-urlencoded");
    xhr.onreadystatechange = function () { // Call a function when the state changes.
        if (this.readyState === XMLHttpRequest.DONE && this.status === 200) {
            getStatus();
        }
    }
    xhr.send(effect + "=" + data);
}

// converts hex colors to rgb
function hexToRgb(hex) {
    var result = /^#?([a-f\d]{2})([a-f\d]{2})([a-f\d]{2})$/i.exec(hex);
    return result ? {
        r: parseInt(result[1], 16),
        g: parseInt(result[2], 16),
        b: parseInt(result[3], 16)
    } : null;
}

function rgbToHex(r, g, b) {
    function componentToHex(c) {
        var hex = c.toString(16);
        return hex.length == 1 ? "0" + hex : hex;
    }
    return "#" + componentToHex(r) + componentToHex(g) + componentToHex(b);
}

// this is for synchronizing the buttons on multiple devices
function getStatus() {
    var xhr = new XMLHttpRequest();
    xhr.open("GET", '/sync', true);
    //Send the proper header information along with the request
    xhr.setRequestHeader("Content-Type", "application/x-www-form-urlencoded");
    xhr.onreadystatechange = function () { // Call a function when the state changes.
        if (this.readyState === XMLHttpRequest.DONE && this.status === 200) {
            var data = JSON.parse(xhr.response);
            for (let index = 0; index < data.length; index++) {
                const element = data[index];
                if (index == 1) {
                    document.getElementById('switch' + String(index)).checked = element.active;
                    document.getElementById("color").value = rgbToHex(element.red, element.green, element.blue);

                } else {
                    document.getElementById('switch' + String(index)).checked = element;
                }
            }
        }
    }
    xhr.send();
}

function sync() {
    getStatus();
    setTimeout(sync, 1000);
}

sync();