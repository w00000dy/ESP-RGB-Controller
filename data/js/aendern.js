function aendern2() {
    var x = JSON.parse(xhr.response);
    if (x[1].acitve =="true"){

    
    var r = x[1].red;
    var g = x[1].green;
    var b = x[1].blue;
    aendern(r,g,b);
    }
    else if(x.RGB =="true")
    {
        RGB();
    }
    else if(x.fire =="true")
    {
        Fire();
    }
    
}
aendern2();
function aendern(r,g,b) {
    document.getElementById("dings").innerHTML = Color;
    document.getElementById("dings").style.color = "rgb("+r+","+g+","+b+")";
    document.getElementById("dings").classList.remove('colorchangeFast');
}
function RGB()
{
    document.getElementById("dings").innerHTML = "RGB";
    document.getElementById("dings").classList.add('colorchangeFast');

}
function Fire()
{
    document.getElementById("dings").innerHTML = "Fire";
    document.getElementById("dings").classList.remove'colorchangeFast');
    document.getElementById("dings").style.color ="Maroon";

}