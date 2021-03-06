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

/**
 * Enum for the different items kind.
 * @enum {number}
 * @readonly
 */
var ItemKind = {
     Item: 0,
     Weapon: 1,
     Armor: 2
};
Object.freeze(ItemKind);

var CharacterKind = Object.freeze(
  {
     Hero: 0,
     Monster: 1
  }
)

var GroupKind = Object.freeze(
  {
     Team: 0,
     Reserve: 1,
     Hidden: 2
  }
)

// -------------------------------------------------------
//
//  CLASS CurrentGameDatas
//
//  All the global informations of a particular game.
//
//  @playTime               -> The current time played since the beginning of the game in seconds.
//  @teamHeroes             -> List of all the heroes in the team.
//  @reserveHeroes          -> List of all the heroes in the reserve.
//  @hiddenHeroes           -> List of all the hidden heroes.
//  @items                  -> List of all the items, weapons, and armors in the inventory.
//  @charactersInstances    -> Id of the last instance character.
//  @hero                   -> Hero informations.
//  @mapsDatas              -> All the informations for each datas.
//
// -------------------------------------------------------

function Game(){
    this.currentSlot = -1;
    this.hero = $modelHero;
}

Game.prototype = {

    initializeDefault: function(){
        this.playTime = 0;
        this.teamHeroes = [];
        this.reserveHeroes = [];
        this.hiddenHeroes = [];
        this.items = [];
        this.charactersInstances = 0;
        this.initializeVariables();
        this.currentMapId = $datasGame.system.idMapStartHero.getValue();
        this.heroStates = [1];
        EventCommandModifyTeam.instanciateTeam(GroupKind.Team,
                                               CharacterKind.Hero, 1, 1, 1);
        this.mapsDatas = {};
    },

    // -------------------------------------------------------

    initializeVariables: function(){
        this.listVariables = new Array($datasGame.variablesNumbers);
        for (var i = 0; i < $datasGame.variablesNumbers; i++)
            this.listVariables[i] = 0;
    },

    // -------------------------------------------------------

    read: function(slot, callback, base){
        this.currentSlot = slot;
        Wanok.openFile(this, Wanok.fileSave(slot), true, function(res){
            var json = JSON.parse(res);
            this.playTime = json.t;
            this.charactersInstances = json.inst;
            this.listVariables = json.vars;

            // Items
            var itemsJson = json.itm;
            var i, l = itemsJson.length;
            this.items = new Array(l);
            for (i = 0; i < l; i++){
                var itemJson = itemsJson[i];
                this.items[i] = new Item(itemJson.k, itemJson.id, itemJson.nb);
            }

            // Heroes
            var heroesJson = json.th;
            l = heroesJson.length
            this.teamHeroes = new Array(l);
            var heroJson, character;
            for (i = 0; i < l; i++){
                heroJson = heroesJson[i];
                character = new Player(heroJson.k, heroJson.id, heroJson.instid,
                                       heroJson.sk);
                character.readJSON(heroJson, this.items);
                this.teamHeroes[i] = character;
            }
            heroesJson = json.sh;
            l = heroesJson.length
            this.reserveHeroes = new Array(l);
            for (i = 0; i < l; i++){
                heroJson = heroesJson[i];
                character = new Player(heroJson.k, heroJson.id, heroJson.instid,
                                       heroJson.sk);
                character.readJSON(heroJson, this.items);
                this.reserveHeroes[i] = character;
            }
            heroesJson = json.hh;
            l = heroesJson.length
            this.hiddenHeroes = new Array(l);
            for (i = 0; i < l; i++){
                heroJson = heroesJson[i];
                character = new Player(heroJson.k, heroJson.id, heroJson.instid,
                                       heroJson.sk);
                character.readJSON(heroJson, this.items);
                this.hiddenHeroes[i] = character;
            }

            this.currentMapId = json.currentMapId;
            var positionHero = json.heroPosition;
            this.hero.mesh.position.set(positionHero[0],
                                        positionHero[1],
                                        positionHero[2]);
            this.heroStates = json.heroStates;
            this.readMapsDatas(json.mapsDatas);

            callback.call(base, this);
        });
    },

    readMapsDatas: function(json){
        this.mapsDatas = json;
    },

    write: function(slot){
        this.currentSlot = slot;
        var i, l = this.teamHeroes.length;
        var teamHeroes = new Array(l);
        for (i = 0; i < l; i++)
            teamHeroes[i] = this.teamHeroes[i].getSaveCharacter();
        l = this.reserveHeroes.length;
        var reserveHeroes = new Array(l);
        for (i = 0; i < l; i++)
            reserveHeroes[i] = this.reserveHeroes[i].getSaveCharacter();
        l = this.hiddenHeroes.length;
        var hiddenHeroes = new Array(l);
        for (i = 0; i < l; i++)
            hiddenHeroes[i] = this.hiddenHeroes[i].getSaveCharacter();

        Wanok.saveFile(Wanok.fileSave(slot),
            {
                t: this.playTime,
                th: teamHeroes,
                sh: reserveHeroes,
                hh: hiddenHeroes,
                itm: this.items,
                inst: this.charactersInstances,
                vars: this.listVariables,
                currentMapId: this.currentMapId,
                heroPosition: [this.hero.mesh.position.x,
                               this.hero.mesh.position.y,
                               this.hero.mesh.position.z],
                heroStates: this.heroStates,
                mapsDatas : this.getCompressedMapsDatas()
            }
        );
    },

    getCompressedMapsDatas: function(){
        var obj = {};

        return obj;
    }
}
