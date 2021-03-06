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
//  CLASS Bitmap
//
// -------------------------------------------------------

/** @class
*   A bitmap is something that can be drawn on the HUD. It can also be moved,
*   zoom... It can be a window, a text, an image...
*   @property {number} x Coords of battler.
*   @property {number} y Coords of battler.
*   @property {number} w Coords of battler.
*   @property {number} h Coords of battler.
*   @param {number} x Coords of battler.
*   @param {number} y Coords of battler.
*   @param {number} w Coords of battler.
*   @param {number} h Coords of battler.
*/
function Bitmap(x, y, w, h){

    // Default values
    if (typeof x === 'undefined') x = 0;
    if (typeof y === 'undefined') y = 0;
    if (typeof w === 'undefined') w = 0;
    if (typeof h === 'undefined') h = 0;

    this.setX(x);
    this.setY(y);
    this.setW(w);
    this.setH(h);
}

Bitmap.prototype = {

    /** Set the x value.
    *   @param {number} The x value.
    */
    setX: function(x){
        this.x = Wanok.getScreenX(x);
    },

    /** Set the y value.
    *   @param {number} The y value.
    */
    setY: function(y){
        this.y = Wanok.getScreenY(y);
    },

    /** Set the w value.
    *   @param {number} The w value.
    */
    setW: function(w){
        this.w = Wanok.getScreenX(w);
    },

    /** Set the h value.
    *   @param {number} The h value.
    */
    setH: function(h){
        this.h = Wanok.getScreenY(h);
    },

    /** Set all the coords values.
    *   @param {number} The x value.
    *   @param {number} The y value.
    *   @param {number} The w value.
    *   @param {number} The h value.
    */
    setCoords: function(x, y, w, h){
        this.setX(x);
        this.setY(y);
        this.setW(w);
        this.setH(h);
    }
}
