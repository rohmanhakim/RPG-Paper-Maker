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
//  [CLASS SceneMap : GameState]
//
//  A local map.
//
//  @id                             -> The ID of the map.
//  @mapName                        -> The map name.
//  @scene                          -> The 3D scene of the map.
//  @camera                         -> The camera of the state.
//  @mapInfos                       -> General map informations (real name, name, width, height)
//  @allObjects                     -> All the objects portions according to ID.
//  @mapPortions                    -> All the portions in the visible ray of the map
//  @lastTriggeredObjectPosition    -> The last object triggered position
//
// -------------------------------------------------------

function SceneMap(id){
    SceneGame.call(this);

    this.id = id;
    this.mapName = Wanok.generateMapName(id);
    this.scene = new THREE.Scene();
    this.camera = new Camera(120, 75);
    this.readMapInfos();
    this.initializePortions();
    this.initializeObjects();
    this.lastTriggeredObjectPosition = null;
    this.loadTextures();
}

// -------------------------------------------------------
//  [getPortionName x y z] Initialize the map objects.
// -------------------------------------------------------

SceneMap.getPortionName = function(x, y, z){
    return (x + "_" + y + "_" + z + ".json");
}

SceneMap.prototype = {

    // -------------------------------------------------------
    //  [readMapInfos realName] Read the map infos file.
    // -------------------------------------------------------

    readMapInfos: function(){
        Wanok.openFile(this, Wanok.FILE_MAPS + this.mapName +
                       Wanok.FILE_MAP_INFOS, true, function(res)
        {
            var json = JSON.parse(res);
            this.mapInfos = {
                name: json.name,
                length: json.l,
                width: json.w,
                height: json.h,
                depth: json.d
            };

            // Now that we have map dimensions, we can initialize object portion
            if (!$game.mapsDatas.hasOwnProperty(this.id)){
                this.initializePortionsObjects();
            }
        });
    },

    // -------------------------------------------------------
    //  [initializePortions] Initialize the map portions.
    // -------------------------------------------------------

    initializePortions: function(){
        var ray = ($PORTIONS_RAY_NEAR + $PORTIONS_RAY_FAR) * 2 + 1;
        this.mapPortions = new Array(ray);
        for (var i = 0; i < ray; i++){
            this.mapPortions[i] = new Array(ray);
            for (var j = 0; j < ray; j++){
                this.mapPortions[i][j] = new Array(ray);
                for (var k = 0; k < ray; k++){
                    this.mapPortions[i][j][k] = null;
                    this.loadPortion(i, j, k, i, j, k);
                }
            }
        }

        this.scene.add($game.hero.mesh);
    },

    // -------------------------------------------------------
    //  [loadPortion realX realY realZ x y z] Initialize the map objects.
    // -------------------------------------------------------

    loadPortion: function(realX, realY, realZ, x, y, z){
        var fileName = SceneMap.getPortionName(realX, realY, realZ);

        if (realX === 0 && realY === 0 && realZ === 0){
            Wanok.openFile(this, Wanok.FILE_MAPS + this.mapName + "/" +
                           fileName, false, function(res)
            {
                var json = JSON.parse(res);
                var mapPortion = new MapPortion(realX, realY, realZ);
                mapPortion.read(json,
                                this.id ===
                                $datasGame.system.idMapStartHero.getValue());
                this.mapPortions[x][y][z] = mapPortion;
            });
        }
        else{
            this.mapPortions[x][y][z] = null;
        }
    },

    // -------------------------------------------------------
    //  [initializeObjects] Initialize the map objects
    // -------------------------------------------------------

    initializeObjects: function(){
        Wanok.openFile(this, Wanok.FILE_MAPS + this.mapName +
                       Wanok.FILE_MAP_OBJECTS, true, function(res)
        {
            var json = JSON.parse(res).objs;
            var i, l;

            var jsonObject;
            l = json.length;
            this.allObjects = new Array(l+1);
            for (i = 0; i < l; i++){
                jsonObject = json[i];
                this.allObjects[jsonObject.id] = jsonObject.p;
            }
        });
    },

    // -------------------------------------------------------
    //  [initializePortionsObjects] All the objects moved or/and with changed
    //  states
    // -------------------------------------------------------

    initializePortionsObjects: function(){
        var l = Math.ceil(this.mapInfos.length / $PORTION_SIZE);
        var w = Math.ceil(this.mapInfos.width / $PORTION_SIZE);
        var h = Math.ceil(this.mapInfos.height / $PORTION_SIZE) +
                Math.ceil(this.mapInfos.depth / $PORTION_SIZE);

        var objectsPortions = new Array(l);
        for (var i = 0; i < l; i++){
            objectsPortions[i] = new Array(w);
            for (var j = 0; j < w; j++){
                objectsPortions[i][j] = new Array(h);
                for (var k = 0; k < h; k++){
                    objectsPortions[i][j][k] =
                    {
                        mr: [],  // Ids of all the moved objects that are out
                                 // of the portion
                        ma: [],  // All the moved objects that are from other
                                 // portions
                        m:  [],  // All the moved objects that are from this
                                 // portion
                        si: [],  // Ids of the objects that have modified states
                        s: []    // States of the objects according to id
                    };
                }
            }
        }

        $game.mapsDatas[this.id] = objectsPortions;
    },

    // -------------------------------------------------------
    //  [loadTextures] Load all the textures of the map.
    // -------------------------------------------------------

    loadTextures: function(){

        // Load textures
        var textureLoader = new THREE.TextureLoader();
        this.textureTileset = this.loadTexture(textureLoader,
                                               Wanok.PATH_TILESETS +
                                               "/plains.png");
        this.texturesCharacters = new Array(2);
        this.texturesCharacters[1] = this.loadTexture(textureLoader,
                                                      Wanok.PATH_CHARACTERS +
                                                      "/lucas.png");

        // Update hero material
        $game.hero.mesh.material = this.texturesCharacters[1];
    },

    // -------------------------------------------------------
    //  [loadTexture textureLoader path] Load a texture.
    // -------------------------------------------------------

    loadTexture: function(textureLoader, path){
        var texture = textureLoader.load(path);
        texture.magFilter = THREE.NearestFilter;
        texture.minFilter = THREE.NearestFilter;
        texture.flipY = false;
        return new THREE.MeshBasicMaterial(
                    {
                        map: texture,
                        transparent: true,
                        side: THREE.DoubleSide
                    });
    },

    // -------------------------------------------------------
    //  [closeMap] Close the map.
    // -------------------------------------------------------

    closeMap: function(){
        var l = Math.ceil(this.mapInfos.length / $PORTION_SIZE);
        var w = Math.ceil(this.mapInfos.width / $PORTION_SIZE);
        var h = Math.ceil(this.mapInfos.height / $PORTION_SIZE) +
                Math.ceil(this.mapInfos.depth / $PORTION_SIZE);

        var objectsPortions = $game.mapsDatas[this.id];
        for (var i = 0; i < l; i++){
            for (var j = 0; j < w; j++){
                for (var k = 0; k < h; k++){
                    var portion = objectsPortions[i][j][k];
                    portion.mr = [];
                    portion.ma = [];
                    portion.m = [];
                }
            }
        }
    },

    // -------------------------------------------------------

    update: function(){

        // Update
        SceneGame.prototype.update.call(this);

        // Update camera
        this.camera.update();
    },

    // -------------------------------------------------------

    getPortion: function(x,y){
        return [0,0]; // TODO
    },

    // -------------------------------------------------------

    onKeyPressed: function(key){

        // Send keyPressEvent to all the objects
        if (!$blockingHero){
            EventCommandSendEvent.sendEvent(null, 4, 1, true, 3,
                                            [null,
                                            SystemValue.createNumber(key),
                                            SystemValue.createSwitch(false),
                                            SystemValue.createSwitch(false)]);
        }

        SceneGame.prototype.onKeyPressed.call(this, key);
    },

    // -------------------------------------------------------

    onKeyReleased: function(key){
        SceneGame.prototype.onKeyReleased.call(this, key);
    },

    // -------------------------------------------------------

    onKeyPressedRepeat: function(key){
        if (!$blockingHero){
            EventCommandSendEvent.sendEvent(null, 4, 1, true, 3,
                                            [null,
                                            SystemValue.createNumber(key),
                                            SystemValue.createSwitch(true),
                                            SystemValue.createSwitch(true)]);
        }

        var block = SceneGame.prototype.onKeyPressedRepeat.call(this, key);

        return block;
    },

    // -------------------------------------------------------

    onKeyPressedAndRepeat: function(key){
        if (!$blockingHero){
            EventCommandSendEvent.sendEvent(null, 4, 1, true, 3,
                                            [null,
                                            SystemValue.createNumber(key),
                                            SystemValue.createSwitch(true),
                                            SystemValue.createSwitch(false)]);
        }

        SceneGame.prototype.onKeyPressedAndRepeat.call(this, key);
    },

    // -------------------------------------------------------

    draw3D: function(canvas){
        $renderer.render(this.scene, this.camera.threeCamera);
    },

    // -------------------------------------------------------

    drawHUD: function(context){
        SceneGame.prototype.drawHUD.call(this, context);
    }
}
