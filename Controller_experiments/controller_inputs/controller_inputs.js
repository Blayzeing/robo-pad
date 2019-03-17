// [Source 1] http://beej.us/blog/data/javascript-gamepad/ 

// Return true if gamepad support exists on this browser
function supportsGamepads() {
    return !!(navigator.getGamepads);
}

// Display the properties of all connected gamepads.
function gamePadProperties(){
	// Get the state of all gamepads
	let gamepads = navigator.getGamepads();
	//iterate over each gamepad and display its properties. 
		//Note: unplugging and plugging in controllers will alter its gamepads value, but the browser will attempt to keep the index value constant. 
	for (let i = 0; i < gamepads.length; i++) {
		console.log("Gamepad " + i + ":");
		//If there is no gamepad connected at this index, return null and iterate loop.
		if (gamepads[i] === null) {
			console.log("[null]");
			continue;
		}
		//If there is no gamepad connected at this index, return null and iterate loop.
		if (!gamepads[i].connected) {
			console.log("[disconnected]");
			continue;
		}
		//Display all gamepad properties. 
		console.log("    Index: " + gamepads[i].index);
		console.log("    ID: " + gamepads[i].id);
		console.log("    Axes: " + gamepads[i].axes.length);
		console.log("    Buttons: " + gamepads[i].buttons.length);
		console.log("    Mapping: " + gamepads[i].mapping);
		//Display if gamepad is standard.
		if (gamepads[i].mapping == "standard") {	//https://w3c.github.io/gamepad/#remapping
			console.log("Controller " + gamepads[i].index + " has standard mapping");
		} else {
			console.log("Controller does not have standard mapping");
		}
	}
}

/*// Handle controls and drawing a new frame
function handleFrame(timestamp) {
    let gamepads = navigator.getGamepads();
    handleInput(gamepads); // Exercise for the reader
    drawScene(); // Exercise for the reader
    // Render it again, Sam
    requestAnimationFrame(handleFrame);
}*/


// Returns either a positive number of connections, a negative number of disconnections, or zero for no change.
let testForConnections = (function() {
    let connectionCount = 0;	// Keep track of the connection count
    // Return a function that does the actual tracking
    return function () {
        let gamepads = navigator.getGamepads();
        let count = 0;
        let diff;

        for (let i = gamepads.length - 1; i >= 0; i--) {
            let g = gamepads[i];

            // Make sure they're not null and connected
            if (g && g.connected) {
                count++;
            }
        }
        // Return any changes
        diff = count - connectionCount;
        connectionCount = count;
        return diff;
    }
}());

//Displays how many controllers have been connected/disconnected and the number of controllers currently connected. 
function DisplayConnections() {
    let tc = testForConnections();
	let count = 0;
	let gamepads = navigator.getGamepads();
	for (let i = gamepads.length - 1; i >= 0; i--) {
		let g = gamepads[i];
		if (g && g.connected) {
			count++;
		}
	}
    if (tc > 0) {
        console.log(tc + " gamepads connected, total of " + count + " gamepads.");
    } else if (tc < 0) {
        console.log((-tc) + " gamepads disconnected, total of " + count + " gamepads.");
    } else {
        // Gamepads neither connected nor disconnected.
    }

    requestAnimationFrame(DisplayConnections);
}

//Returns the number of buttons currently pressed.
function numberOfPressedButtons(index){
	let axisThreshold = 0.1;
	let gamepads = navigator.getGamepads();
	let gp = gamepads[index];
	if ((gamepads[index] === null)){
		console.log("Controller at index " + index + " disconnected.");
		return -1;
	}
	if ((!gamepads[index].connected)){
		console.log("Controller at index " + index + " disconnected.");
		return -1;
	}
	let count = 0;
	for (let i = 0; i <= 15; i++) {
		if(gp.buttons[i].pressed)	count++;
	}
	for (let i = 0; i <= 3; i++) {
		try{	
			if((gp.axis[i] > Threshold) || (gp.axis < -Threshold))	count++;
		}catch{
		}
	}
	return count;
}

function printPressedButtons(index){
	let gamepads = navigator.getGamepads();
	if ((gamepads[index] === null)){
		console.log("Controller at index " + index + " disconnected.");
		return;
	}
	if ((!gamepads[index].connected)){
		console.log("Controller at index " + index + " disconnected.");
		return;
	}
	let gp = gamepads[index];
	// Blindly assuming there are buttons. 
	try{
		console.log(Number(gp.buttons[0].pressed) + " " + Number(gp.buttons[1].pressed) + " " + Number(gp.buttons[2].pressed) + " " + 
		Number(gp.buttons[3].pressed) + " " + Number(gp.buttons[4].pressed) + " " + Number(gp.buttons[5].pressed) + " " + Number(gp.buttons[6].pressed) + " " + 
		Number(gp.buttons[7].pressed) + " " + Number(gp.buttons[8].pressed) + " " + Number(gp.buttons[9].pressed) + " " + Number(gp.buttons[10].pressed) + " " +
		Number(gp.buttons[11].pressed) + " " + Number(gp.buttons[12].pressed) + " " + Number(gp.buttons[13].pressed) + " " + 
		Number(gp.buttons[14].pressed) + " " + Number(gp.buttons[15].pressed) + " " + 
		gamepads.axis[0] + " " + gamepads.axis[1] + " " + gamepads.axis[2] + " " + gamepads.axis[3]);
	}catch(err){
		console.log(Number(gp.buttons[0].pressed) + " " + Number(gp.buttons[1].pressed) + " " + Number(gp.buttons[2].pressed) + " " + 
		Number(gp.buttons[3].pressed) + " " + Number(gp.buttons[4].pressed) + " " + Number(gp.buttons[5].pressed) + " " + Number(gp.buttons[6].pressed) + " " + 
		Number(gp.buttons[7].pressed) + " " + Number(gp.buttons[8].pressed) + " " + Number(gp.buttons[9].pressed) + " " + Number(gp.buttons[10].pressed) + " " +
		Number(gp.buttons[11].pressed) + " " + Number(gp.buttons[12].pressed) + " " + Number(gp.buttons[13].pressed) + " " + 
		Number(gp.buttons[14].pressed) + " " + Number(gp.buttons[15].pressed) + " (no axis)");
	}
}

function printPressedSNESButtons(index){
	let gamepads = navigator.getGamepads();
	if ((gamepads[index] === null)){
		console.log("Controller at index " + index + " disconnected.");
		return;
	}
	if ((!gamepads[index].connected)){
		console.log("Controller at index " + index + " disconnected.");
		return;
	}
	let gp = gamepads[index];
	// Blindly assuming a SNES controller is being used with the standard gamepad mapping. 
	let buttons = "";
	if(gp.buttons[1].pressed)	buttons += "A ";		else	buttons += "_ ";
	if(gp.buttons[0].pressed)	buttons += "B ";		else	buttons += "_ ";
	if(gp.buttons[3].pressed)	buttons += "X ";		else	buttons += "_ ";
	if(gp.buttons[2].pressed)	buttons += "Y ";		else	buttons += "_ ";
	if(gp.buttons[4].pressed)	buttons += "L ";		else	buttons += "_ ";
	if(gp.buttons[5].pressed)	buttons += "R ";		else	buttons += "_ ";
	if(gp.buttons[9].pressed)	buttons += "START ";		else	buttons += "_____ ";
	if(gp.buttons[8].pressed)	buttons += "SELECT ";		else	buttons += "______ ";
	if(gp.buttons[12].pressed)	buttons += "^ ";		else	buttons += "_ ";
	if(gp.buttons[13].pressed)	buttons += "v ";		else	buttons += "_ ";
	if(gp.buttons[14].pressed)	buttons += "< ";		else	buttons += "_ ";
	if(gp.buttons[15].pressed)	buttons += "> ";		else	buttons += "_ ";
	console.log(buttons);
}

function init ()
{
	console.log("Hello Blayze!");
	console.log("Browser supports GamePad? " + supportsGamepads());
	console.log("GamePad Information:"); gamePadProperties();
	//setInterval(function () { gamePadProperties() }, 30);
	
	/*// Kick off the rendering
	requestAnimationFrame(handleFrame);*/
	DisplayConnections();
	setInterval(function() {	DisplayConnections();	}, 1000);
	console.log("Gamepad 0:\n[0] [1] [2] [3] [4] [5] [6] [7] [8] [9] [10] [11] [12] [13] [14] [15] [A0] [A1] [A2] [A3]");
	setInterval(function() {	console.log(numberOfPressedButtons(0));	}, 30);
}

//Only do any of the above code *after* the page has loaded.
if (document.getElementById) window.onload = init;