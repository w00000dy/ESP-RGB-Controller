/*
    Then it checks if the String says black and if thats is true it turns the page into dark-mode.
    It calls a function the  calls a another function that checks wich the site and tells the scipt which fuction to use.
*/
function anfang() {
    var x = document.cookie;
    console.log(x);

    if (x == "darkmode=true") {
        setSchwarz();
    }
    else {
        setWeiss();
    }

}
/*
    Function which sets the pages to black. First body black than it loops through
    the buttons. 
*/
function setSchwarz() {

    document.body.classList.add("dark-mode");
    var buttons = document.getElementsByTagName('button');
    for (var i = 0; i < buttons.length; i++) {
        var button = buttons[i];
        button.classList.add("dark-mode");
    }




}
/*
    It´s a Method to set a cookie with the input from the select element from Settings with the id "farbeweb".
    The cookie expieres after 100 years. The cookie is samesite stric set.
*/
function setCookie(modus) {
    document.cookie = "darkmode=" + modus + "; max-age=3153600000; samesite=strict; path=/";
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

}

/*
    Darkmode function for settings.html and index.html it works like schwarz.

*/
function darkmode() {
    if (document.cookie == "darkmode=true") {
        document.body.classList.add("darkmode");
        document.getElementsByTagName("input")[0].classList.add("darkmode");
        console.log("yep");
        document.getElementsByTagName("input")[1].classList.add("darkmode");
        var lever = document.getElementsByClassName("lever");
        for (let index = 0; index < lever.length; index++) {
            const element = lever[index];
            element.classList.add("darkmodeLever");
        }
    } else {
        document.body.classList.remove("darkmode");
        document.getElementsByTagName("input")[0].classList.remove("darkmode");
        document.getElementsByTagName("input")[1].classList.remove("darkmode");
        var lever = document.getElementsByClassName("lever");
        for (let index = 0; index < lever.length; index++) {
            const element = lever[index];
            element.classList.remove("darkmodeLever");
        }
    }
}