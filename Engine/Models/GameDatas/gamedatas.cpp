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

#include "gamedatas.h"
#include "wanok.h"

// -------------------------------------------------------
//
//  CONSTRUCTOR / DESTRUCTOR / GET / SET
//
// -------------------------------------------------------

GameDatas::GameDatas() :
    m_commonEventsDatas(nullptr),
    m_variablesSwitchesDatas(nullptr),
    m_systemDatas(nullptr),
    m_battleSystemDatas(nullptr),
    m_itemsDatas(nullptr),
    m_skillsDatas(nullptr),
    m_weaponsDatas(nullptr),
    m_armorsDatas(nullptr),
    m_heroesDatas(nullptr),
    m_monstersDatas(nullptr),
    m_troopsDatas(nullptr),
    m_classesDatas(nullptr)
{

}

GameDatas::~GameDatas()
{
    delete m_commonEventsDatas;
    delete m_variablesSwitchesDatas;
    delete m_systemDatas;
    delete m_battleSystemDatas;
    delete m_itemsDatas;
    delete m_skillsDatas;
    delete m_weaponsDatas;
    delete m_armorsDatas;
    delete m_heroesDatas;
    delete m_monstersDatas;
    delete m_troopsDatas;
    delete m_classesDatas;
}

CommonEventsDatas* GameDatas::commonEventsDatas() const {
    return m_commonEventsDatas;
}

VariablesSwitchesDatas* GameDatas::variablesSwitchesDatas() const {
    return m_variablesSwitchesDatas;
}

SystemDatas* GameDatas::systemDatas() const {
    return m_systemDatas;
}

BattleSystemDatas* GameDatas::battleSystemDatas() const {
    return m_battleSystemDatas;
}

ItemsDatas* GameDatas::itemsDatas() const {
    return m_itemsDatas;
}

SkillsDatas* GameDatas::skillsDatas() const {
    return m_skillsDatas;
}

WeaponsDatas* GameDatas::weaponsDatas() const {
    return m_weaponsDatas;
}

ArmorsDatas* GameDatas::armorsDatas() const {
    return m_armorsDatas;
}

HeroesDatas* GameDatas::heroesDatas() const {
    return m_heroesDatas;
}

MonstersDatas* GameDatas::monstersDatas() const {
    return m_monstersDatas;
}

TroopsDatas* GameDatas::troopsDatas() const {
    return m_troopsDatas;
}

ClassesDatas* GameDatas::classesDatas() const {
    return m_classesDatas;
}

// -------------------------------------------------------
//
//  INTERMEDIARY FUNCTIONS
//
// -------------------------------------------------------

void GameDatas::setDefault(){
    m_commonEventsDatas = new CommonEventsDatas;
    m_variablesSwitchesDatas = new VariablesSwitchesDatas;
    m_systemDatas = new SystemDatas;
    m_battleSystemDatas = new BattleSystemDatas;
    m_itemsDatas = new ItemsDatas;
    m_skillsDatas = new SkillsDatas;
    m_weaponsDatas = new WeaponsDatas;
    m_armorsDatas = new ArmorsDatas;
    m_heroesDatas = new HeroesDatas;
    m_monstersDatas = new MonstersDatas;
    m_troopsDatas = new TroopsDatas;
    m_classesDatas = new ClassesDatas;
    m_commonEventsDatas->setDefault();
    m_variablesSwitchesDatas->setDefault();
    m_systemDatas->setDefault();
    m_battleSystemDatas->setDefault();
    m_itemsDatas->setDefault();
    m_skillsDatas->setDefault();
    m_weaponsDatas->setDefault();
    m_armorsDatas->setDefault();
    m_heroesDatas->setDefault();
    m_monstersDatas->setDefault(m_systemDatas->modelCurrencies()
                                ->invisibleRootItem(),
                                m_itemsDatas->model()->invisibleRootItem(),
                                m_weaponsDatas->model()->invisibleRootItem(),
                                m_armorsDatas->model()->invisibleRootItem());
    m_troopsDatas->setDefault(m_monstersDatas->model()->invisibleRootItem());
    m_classesDatas->setDefault(m_skillsDatas->model()->invisibleRootItem(),
                               m_battleSystemDatas->modelCommonStatistics()
                               ->invisibleRootItem());
}

// -------------------------------------------------------
//
//  READ / WRITE
//
// -------------------------------------------------------

void GameDatas::read(QString path){
    readVariablesSwitches(path);
    m_commonEventsDatas = new CommonEventsDatas();
    m_systemDatas = new SystemDatas();
    m_itemsDatas = new ItemsDatas();
    m_skillsDatas = new SkillsDatas();
    m_battleSystemDatas = new BattleSystemDatas();
    m_weaponsDatas = new WeaponsDatas();
    m_armorsDatas = new ArmorsDatas();
    m_heroesDatas = new HeroesDatas();
    m_monstersDatas = new MonstersDatas();
    m_troopsDatas = new TroopsDatas();
    m_classesDatas = new ClassesDatas();
    m_commonEventsDatas->read(path);
    m_systemDatas->read(path);
    m_itemsDatas->read(path);
    m_skillsDatas->read(path);
    m_battleSystemDatas->read(path);
    m_weaponsDatas->read(path);
    m_armorsDatas->read(path);
    m_heroesDatas->read(path);
    m_monstersDatas->read(path);
    m_troopsDatas->read(path);
    m_classesDatas->read(path);
}

// -------------------------------------------------------

void GameDatas::readVariablesSwitches(QString path){
    if (m_variablesSwitchesDatas != nullptr) delete m_variablesSwitchesDatas;
    m_variablesSwitchesDatas = new VariablesSwitchesDatas();
    m_variablesSwitchesDatas->read(path);
}

// -------------------------------------------------------

void GameDatas::write(QString path){
    Wanok::writeJSON(Wanok::pathCombine(path, Wanok::pathCommonEvents),
                     *m_commonEventsDatas);
    Wanok::writeJSON(Wanok::pathCombine(path, Wanok::pathVariablesSwitches),
                     *m_variablesSwitchesDatas);
    Wanok::writeJSON(Wanok::pathCombine(path, Wanok::pathSystem),
                     *m_systemDatas);
    Wanok::writeJSON(Wanok::pathCombine(path, Wanok::pathBattleSystem),
                     *m_battleSystemDatas);
    Wanok::writeJSON(Wanok::pathCombine(path, Wanok::pathItems),
                     *m_itemsDatas);
    Wanok::writeJSON(Wanok::pathCombine(path, Wanok::pathSkills),
                     *m_skillsDatas);
    Wanok::writeJSON(Wanok::pathCombine(path, Wanok::pathWeapons),
                     *m_weaponsDatas);
    Wanok::writeJSON(Wanok::pathCombine(path, Wanok::pathArmors),
                     *m_armorsDatas);
    Wanok::writeJSON(Wanok::pathCombine(path, Wanok::pathHeroes),
                     *m_heroesDatas);
    Wanok::writeJSON(Wanok::pathCombine(path, Wanok::pathMonsters),
                     *m_monstersDatas);
    Wanok::writeJSON(Wanok::pathCombine(path, Wanok::pathTroops),
                     *m_troopsDatas);
    Wanok::writeJSON(Wanok::pathCombine(path, Wanok::pathClasses),
                     *m_classesDatas);
}
