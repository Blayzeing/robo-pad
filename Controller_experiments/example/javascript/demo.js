/* Javascript code demonstration for assignment for COM1008.
 * Written by: Blayze Millward
 */

// Global Variables
var canvas;
var context;
var img;
var Env = { upDraft: 0.12, windX: 0, windY: 0 };// To be used in simulation


Particles.smallestSize = 0.2;
Particles.biggestSize = 0.3;
Particles.startXpos = -255;//The starting position of the particle cloud
Particles.startYpos = 350;
Particles.baseScaler = 1.015;//The original rate at which the particles enlarge
Particles.baseScalerRange = 0.03;//The variation on the rate at which the particles enlarge
Particles.maxAngularRot = 0.2; //Maximum angular rotation, in radians per 30ms.
Particles.outputAngle = -3.141/2;//0; //Angle in which the particles should be emitted.
Particles.outputRange = 3.1415926;//0.2;//3.1415926; //The range of rotation around the output angle.
Particles.maxMag = 1;// The maximum magnitude of the output
Particles.minMag = 0;// The minimum magnitude of the output
Particles.lifespan = 3000;// The lifespan of each particle

// Individual Particle Constructor
function Particle (image)
{
	this.image = image;// Set the image to draw the particle as
	this.spawned = false;// This variable is needed in order to stop premature drawing of the particle.
	var that = this;// Sore a reference to this so that it's individual functions and variables can be accessed through a setTimeout.
	setTimeout(function(){ that.start(); }, Particles.lifespan * Math.random());
}
// The function called at the start of each particle's life
Particle.prototype.start = function()
{
	this.spawned = true;
	var that = this// See line 30.
	this.reset();// Set all parameters on the particle on it's birth, or 'first spawn'.
	setInterval(function(){ that.reset(); }, Particles.lifespan);// Start a continuous cycle of re-spawning after a certain age.
}
// The function to reset all of a particles' variables
Particle.prototype.reset = function()
{
	this.x = Particles.startXpos;//Set it's start x and y coordinates
	this.y = Particles.startYpos;
	//Randomly generate the scale, scale speed, angular rotation, rotation speed and initialise the alpha variable (used for fading the particle out)
	this.scaler = Particles.baseScaler + Math.random() * Particles.baseScalerRange - Particles.baseScalerRange/2;
	this.currentScale = Particles.smallestSize + Math.random() * (Particles.biggestSize - Particles.smallestSize);
	this.angularRot = Math.random() * Particles.maxAngularRot - Particles.maxAngularRot/2;
	this.rotation = Math.random() * 3.1415926 * 2;
	this.alpha = 1.0;
	
	//Store the current time to use to calculate this particle's alpha variable later on.
	var d = new Date();
	this.birthday = d.getTime();
	
	//Use trig to calculate a starting velocity
	var angle = Particles.outputAngle + Math.random() * Particles.outputRange - Particles.outputRange/2;
	var magnitude = Particles.minMag + Math.random() * (Particles.maxMag - Particles.minMag);
	this.xVel = Math.cos(angle) * magnitude;
	this.yVel = Math.sin(angle) * magnitude;
}
//The function that actually draws the particle to the canvas.
Particle.prototype.draw = function()
{
	context.save();// Save the canvas so we can un-do all the transformations to come
	context.translate(this.x, this.y);//Finally draw move the coordinate system.
	context.rotate(this.rotation);//Rotate the coordinate system;
	context.scale(this.currentScale, this.currentScale);//First, scale the coordinate system.
	context.globalAlpha = this.alpha;//set the drawing tool's alpha amount.
	context.drawImage(this.image, -this.image.width/2, -this.image.height/2);// Draw the image at (0,0)
	context.globalAlpha = 1.0;// Apparently, alpha is not stored in the save-able properties.
	context.restore();
}
//The function to change all variables of the particle subject to change under simulation.
Particle.prototype.simulate = function()
{
	this.rotation += this.angularRot;
	this.currentScale *= this.scaler;
	this.x += this.xVel;
	this.y += this.yVel;
	this.yVel -= Env.upDraft;
	this.alpha = 1 - this.getAge()/Particles.lifespan;// Calculate new alpha using age.
}
//The function to get age
Particle.prototype.getAge = function()// Returns the age of a particle in milliseconds
{
	var d = new Date();
	return(d.getTime() - this.birthday);
}

//The constructor for a Particles object; an object that holds many particles.
function Particles (numberOfParticles)
{
	//I was thinking of storing the particles individually here, and passing them but in this demo there is no point.
	this.particles = new Array(numberOfParticles);
	//Initialise every particle with the correct image.
	for(var i = 0; i<numberOfParticles; i++)
	{
		this.particles[i] = new Particle(img);
	}
}
// The function that draws every particle
Particles.prototype.draw = function()
{
	context.clearRect(0,0,canvas.width, canvas.height);
	for(var i = 0; i<this.particles.length; i++)
	{
		if(this.particles[i].spawned)// Only if spawned...
		{
			this.particles[i].draw();
		}
	}
}
// The function that simulates every particle
Particles.prototype.simulate = function()
{
	for(var i = 0; i<this.particles.length; i++)
	{
		if(this.particles[i].spawned)// Only if spawned...
		{
			this.particles[i].simulate();
		}
	}
}
//The function that both draws and simulates every particle
Particles.prototype.simudraw = function()
{
	context.clearRect(0,0,canvas.width, canvas.height);
	//Using a new loop instead of calling draw and simulate functions for efficiency...
	for(var i = 0; i<this.particles.length; i++)
	{
		if(this.particles[i].spawned)// Only if spawned...
		{
			this.particles[i].draw();
			this.particles[i].simulate();
		}
	}
}

//The mouse moved event handler
function onMouseMoved (event)
{
	//Variables to store the x and y relative to the canvas
	var xPos;
	var yPos;
	
	if(event.offsetX) {// Try to find the offsetX and Y values, use them if able
		xPos = event.offsetX;
		yPos = event.offsetY;
	}else if(event.layerX) {// If not, then try using the layerX and layerY values.
		xPos = event.layerX;
		yPos = event.layerY;
	}
	
	//Set the particles' spawn position to the current mouse position.
	Particles.startXpos = xPos;
	Particles.startYpos = yPos;
}
// The mouse out event handler
function onMouseOut (event)
{
	//When the mouse has been removed, move the smoke off-screen.
	//This is cheap and dirty; really they should stop generating all together.
	Particles.startXpos = -255;
	Particles.startYpos = 350;
	console.log("beans");
}
// The function that fixes the canvas displaying over the div
function checkHeight (event)
{
	//Calculate the height left (the window height minus the topbar height.
	var heightleft = window.innerHeight - 269;
	heightleft = Math.max(heightleft, 0);// Only use if the height left is a reasonable amount (ie, not negative).
	if(heightleft < 600)// If there's no room for the canvas
	{
		canvas.height = heightleft;// Redraw it shorter
	}else{
		canvas.height = 600;// Keep it at it's proper height;
	}
}
// The function that is called when the web page has loaded.
function init ()
{
	// initialise img and the particle list and get the canvas and set up the context.
	canvas = document.getElementById("jsGame");
	context = canvas.getContext("2d");
	txt = document.getElementById("words");
	txt.innerHTML = "more eggs";
	img = new Image();
	var particleList = new Particles(1000);
	
	checkHeight ()
	
	//Add a function to execute when the image has loaded, to make sure the image is only drawn when loaded.
	img.onload = function()
	{
		setInterval(function () { particleList.simudraw() }, 30);
	}
	
	//Load in the image
	img.src = "images/demo/smoke.png";
	
	//Add a listener for mouse movement, in order to get the X and Y offset and reposition the centre of the smoke.
	canvas.addEventListener("mousemove", onMouseMoved);
	//Add a listener for when the mouse leaves the canvas:
	canvas.addEventListener("mouseout", onMouseOut);
	//Add a listener for when the window is resized, to fix canvas-over-div glitch:
	window.addEventListener("resize", checkHeight);
}

//Only do any of the above code *after* the page has loaded.
if (document.getElementById) window.onload = init;