/*
    RPG Paper Maker Copyright (C) 2017 Marie Laporte

    This file is part of RPG Paper Maker.

    RPG Paper Maker is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    RPG Paper Maker is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
*/

// -------------------------------------------------------
//
//  [MAIN]
//
//  Main file that always needs to be on the bottom of the other scripts.
//  Contains all the global functions and variables.
//
// -------------------------------------------------------

// -------------------------------------------------------
//  GLOBAL VARIABLES
// -------------------------------------------------------

/** The game stack containing all the different states of the game. Can only
*   contains GameState instances.
*   @type {GameState[]} */
var $gameStack;

/** All the game datas.
*   @type {DatasGame} */
var $datasGame;

/** The current game datas.
*   @type {CurrentGameDatas} */
var $game;

/** Defines the size (in pixels) of a square in a map.
*   @constant
*   @type {number} */
var $SQUARE_SIZE;

/** Defines the near ray size (in portions). The map will be drawn according to
*   the camera's position and to this ray.
*   @constant
*   @type {number} */
var $PORTIONS_RAY_NEAR;

/** Defines the far ray size (in portions). The map will not be drawn between
*   $PORTIONS_RAY_NEAR and $PORTIONS_RAY_FAR but will take account of objects
*   doings.
*   @constant
*   @type {number} */
var $PORTIONS_RAY_FAR = 1;

/** Defines the size (in squares) of a portion in a map.
*   @constant
*   @type {number}
*   @default 16 */
var $PORTION_SIZE = 16;

/** A number representing the milliseconds elapsed between 1 January 1970
*   00:00:00 UTC and the given date. It is updated at the end of each update in
*   order to get the elapsed time between each update.
*   @type {number} */
var $elapsedTime = new Date().getTime();

/** The list of all the keys that are currently pressed (for multi-key handling)
*   .
*   @type {number[]} */
var $keysPressed = new Array;

/** The number of files that still needs to be loaded asynchonously.
*   @type {number} */
var $filesToLoad = 0;

/** The number of loaded files (needs to be compared to $filesToLoad).
*   @type {number} */
var $loadedFiles = 0;

/** The font size of your texts.
*   @type {number}
*   @default 13 */
var $fontSize = 13;

/**  The font name of your texts.
*   @type {string}
*   @default "sans-serif" */
var $fontName = "sans-serif";

/** The canvas managing HUD.
*   @type {Canvas} */
var $canvasHUD;

/** Indicates if the hero is blocked by an event.
*   @type {boolean} */
var $blockingHero = false;

/** The model of the hero.
*   @type {MapObject} */
var $modelHero;

/** The width of the window.
*   @type {number} */
var $canvasWidth;

/** The height of the window.
*   @type {number} */
var $canvasHeight;

/** Coefficient of window width (for resizing HUD).
*   @type {number} */
var $windowX;

/** Coefficient of window height (for resizing HUD).
*   @type {number} */
var $windowY;

/** Defines the default width of the screen.
*   @type {number} */
var $SCREEN_X = 640;

/** Defines the default height of the screen.
*   @type {number} */
var $SCREEN_Y = 480;

/** The global renderer.
*   @type {THREE.WebGLRenderer} */
var $renderer;

/** The current game party.
*   @type {Game} */
var $game = null;

// -------------------------------------------------------
//  FUNCTIONS
// -------------------------------------------------------

/** Initialize the game stack and datas.
*/
function initialize(){
    $datasGame = new DatasGame();
    $gameStack = new GameStack();
}

// -------------------------------------------------------

/** Initialize the openGL stuff.
*/
function initializeGL(canvas){

    // Create the renderer
    if ($DESKTOP)
        $renderer = new THREE.Canvas3DRenderer({
                                                   canvas: canvas,
                                                   devicePixelRatio:
                                                   canvas.devicePixelRatio
                                               });
    else{
        $renderer = new THREE.WebGLRenderer();
        $renderer.autoClear = false;
    }

    $renderer.setSize($canvasWidth, $canvasHeight);
}

// -------------------------------------------------------

/** Set the camera aspect while resizing the window.
*/
function resizeGL(canvas){
    var camera = $gameStack.camera;
    if (typeof camera !== 'undefined'){
        camera.threeCamera.aspect = $canvasWidth / $canvasHeight;
        camera.threeCamera.updateProjectionMatrix();
        $renderer.setSize($canvasWidth, $canvasHeight);
    }
}

// -------------------------------------------------------

/** Update the current stack.
*/
function update(){

    // Repeat keypress as long as not blocking
    var continuePressed = true;
    for (var i = 0, l = $keysPressed.length; i < l; i++){
        continuePressed = onKeyPressedRepeat($keysPressed[i]);
        if (!continuePressed)
            break;
    }

    // Update the top of the stack
    $gameStack.update();

    $elapsedTime = new Date().getTime();
}

// -------------------------------------------------------

/** First key press handle for the current stack.
*/
function onKeyPressed(key){
    $gameStack.onKeyPressed(key)
}

// -------------------------------------------------------

/** First key release handle for the current stack.
*/
function onKeyReleased(key){
    $gameStack.onKeyReleased(key)
}

// -------------------------------------------------------

/** Key pressed repeat handle for the current stack.
*   @returns {boolean} false if the other keys are blocked after it.
*/
function onKeyPressedRepeat(key){
    return $gameStack.onKeyPressedRepeat(key)
}

// -------------------------------------------------------

/** Key pressed repeat handle for the current stack, but with
*   a small wait after the first pressure (generally used for menus).
*   @returns {boolean} false if the other keys are blocked after it.
*/
function onKeyPressedAndRepeat(key){
    return $gameStack.onKeyPressedAndRepeat(key)
}

// -------------------------------------------------------

/** Draw the 3D for the current stack.
*/
function draw3D(canvas){
    $gameStack.draw3D(canvas);
}

// -------------------------------------------------------

/** Draw HUD for the current stack.
*/
function drawHUD(canvas){

    // Initialize context
    var context = canvas.getContext('2d');
    context.clearRect(0, 0, canvas.width, canvas.height);
    context.lineWidth = 1;
    context.webkitImageSmoothingEnabled = false;
    context.imageSmoothingEnabled = false;

    // Draw current stack
    $gameStack.drawHUD(context);
}
