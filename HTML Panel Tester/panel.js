window.addEventListener("gamepadconnected", setup);
window.addEventListener("gamepaddisconnected", setdown);
var temp;
var bits = [0,0,0,0,0,0,0,0,0,0,0,0,0];
var sending = false;
var count = 0;

function setup(){
	temp  = navigator.getGamepads();
	document.getElementById('stat').innerHTML = "Gamepad Connected"
	console.log(temp[0]);
	testForNewData();
}
function setdown(){
	document.getElementById('stat').innerHTML = "Gamepad Disconnected"
}
function testForNewData(){
	for(i = 0; i < 12; i++){
		bits[i] = temp[0].buttons[i].pressed;
		if(bits[i]){
			sending = true;
		}
	}
	if(sending){
		document.getElementById("num").innerHTML = "";
		for(i = 0; i < 8; i++){
			if(bits[i]){
				document.getElementById("num").innerHTML += 1;
			}
			else{
				document.getElementById("num").innerHTML += 0;
			}
		}
		if(bits[8]){
			document.getElementById("sec").innerHTML = 1;
		}
		else if(bits[9]){
			document.getElementById("sec").innerHTML = 2;
		}
		else if(bits[10]){
			document.getElementById("sec").innerHTML = 3;
		}
		if(bits[11]){
			document.getElementById("pof").innerHTML = "PASS";
		}
		else{
			document.getElementById("pof").innerHTML = "FAIL";
		}
		sending = false;
		setTimeout(testForNewData, 150);
	}
	else{
		setTimeout(testForNewData, 50);
	}
}