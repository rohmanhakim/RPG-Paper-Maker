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
//  CLASS GameItem
//
//  An item in the inventory.
//
//  @k          -> Kind of item (item, weapon, or armor)
//  @id         -> The id of the item
//  @nb         -> The occurence of the item in the inventory
//
// -------------------------------------------------------

function GameItem(kind, id, nb){
    this.k = kind;
    this.id = id;
    this.nb = nb;
}

GameItem.prototype = {

    getItemInformations: function(){
        switch (this.k){
        case ItemKind.Item:
            return $datasGame.items.list[this.id];
        case ItemKind.Weapon:
            return $datasGame.weapons.list[this.id];
        case ItemKind.Armor:
            return $datasGame.armors.list[this.id];
        }

        return null;
    }
}
