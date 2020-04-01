document.getElementById("light").addEventListener("click", light);
document.getElementById("rainbow").addEventListener("click", rainbow);

function light() {
    document.getElementById('rainbow').checked = false;
    sendData();
}

function rainbow() {
    document.getElementById('light').checked = false;
    sendData();
}

function sendData() {
    var light = document.getElementById('light').checked;
    var rainbow = document.getElementById('rainbow').checked;

    console.log("Light: " + light);
    console.log("Rainbow: " + rainbow);
}