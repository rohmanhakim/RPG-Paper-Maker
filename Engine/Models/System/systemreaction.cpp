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

#include "systemreaction.h"
#include "widgettreecommands.h"
#include "wanok.h"

// -------------------------------------------------------
//
//  CONSTRUCTOR / DESTRUCTOR / GET / SET
//
// -------------------------------------------------------

SystemReaction::SystemReaction() :
    SystemReaction(1, "", new QStandardItemModel, false, false)
{

}

SystemReaction::SystemReaction(int i, QString n, QStandardItemModel* commands,
                               bool bHero, bool bObjects) :
    SuperListItem(i,n),
    m_modelCommands(commands),
    m_blockingHero(bHero),
    m_blockingObjects(bObjects)
{
    addEmptyCommand(m_modelCommands->invisibleRootItem());
}

SystemReaction::~SystemReaction()
{
    deleteCommands(m_modelCommands->invisibleRootItem());
    delete m_modelCommands;
}

QStandardItemModel* SystemReaction::modelCommands() const {
    return m_modelCommands;
}

bool SystemReaction::blockingHero() const { return m_blockingHero; }

void SystemReaction::setBlockingHero(bool b) { m_blockingHero = b; }

bool SystemReaction::blockingObjects() const { return m_blockingObjects; }

void SystemReaction::setBlockingObjects(bool b) { m_blockingObjects = b; }

// -------------------------------------------------------
//
//  INTERMEDIARY FUNCTIONS
//
// -------------------------------------------------------

void SystemReaction::setCopy(const SystemReaction& copy){
    copyCommands(copy.m_modelCommands, m_modelCommands);
    m_blockingHero = copy.m_blockingHero;
    m_blockingObjects = copy.m_blockingObjects;
}

// -------------------------------------------------------

void SystemReaction::addEmptyCommand(QStandardItem *root){
    addCommand(root, new EventCommand());
}

// -------------------------------------------------------

void SystemReaction::addCommand(QStandardItem *root, EventCommand* command){
    QStandardItem * item = new QStandardItem();
    item->setData(QVariant::fromValue(reinterpret_cast<quintptr>(command)));
    item->setText(command->toString());
    root->appendRow(item);
}

// -------------------------------------------------------

void SystemReaction::addCommandWithoutText(QStandardItem *root,
                                           EventCommand* command)
{
    QStandardItem * item = new QStandardItem();
    item->setData(QVariant::fromValue(reinterpret_cast<quintptr>(command)));
    root->appendRow(item);
}
// -------------------------------------------------------

void SystemReaction::copyCommands(const QStandardItemModel* from,
                                  QStandardItemModel* to)
{
    deleteCommands(to->invisibleRootItem());
    to->clear();
    copyCommandsItem(from->invisibleRootItem(), to->invisibleRootItem());
}

// -------------------------------------------------------

void SystemReaction::copyCommandsItem(const QStandardItem* from,
                                      QStandardItem* to)
{
    for (int i = 0; i < from->rowCount(); i++){
        // Copy the current row
        EventCommand* command = (EventCommand*) from->child(i)->data()
                .value<quintptr>();
        EventCommand* copyCommand = new EventCommand;
        copyCommand->setCopy(*command);
        QStandardItem * item = new QStandardItem();
        item->setData(QVariant::fromValue(
                          reinterpret_cast<quintptr>(copyCommand)));
        to->appendRow(item);

        // Copy child
        copyCommandsItem(from->child(i), to->child(i));
    }
}

// -------------------------------------------------------

void SystemReaction::deleteCommands(QStandardItem* item){
    for (int i = 0; i < item->rowCount(); i++){
        deleteCommands(item->child(i));
        delete (EventCommand*) item->child(i)->data().value<quintptr>();
    }
}

// -------------------------------------------------------
//
//  READ / WRITE
//
// -------------------------------------------------------

void SystemReaction::read(const QJsonObject &json){

    // Commands
    readRoot(json["c"].toArray(), m_modelCommands->invisibleRootItem());

    // Options
    m_blockingHero = json["bh"].toBool();
    m_blockingObjects = json["bo"].toBool();
}

// -------------------------------------------------------

void SystemReaction::readRoot(const QJsonArray &json, QStandardItem* root){
    for (int i = 0; i < json.size(); i++) {
        QJsonObject obj = json[i].toObject();

        // Reading the event command and adding it to the Tree View
        EventCommand* ev = new EventCommand();
        ev->read(obj);
        QStandardItem * item = new QStandardItem();
        item->setData(QVariant::fromValue(reinterpret_cast<quintptr>(ev)));
        if (obj.contains("children")){
            readRoot(obj["children"].toArray(), item);
            addEmptyCommand(item);
        }
        root->insertRow(i, item);
    }
}

// -------------------------------------------------------

void SystemReaction::write(QJsonObject & json) const{

    // Commands
    json["c"] = getChildrenJSON(m_modelCommands->invisibleRootItem());

    // Options
    json["bh"] = m_blockingHero;
    json["bo"] = m_blockingObjects;
}

// -------------------------------------------------------

QJsonArray SystemReaction::getChildrenJSON(QStandardItem* item) const{
    QJsonArray tab;
    for (int i = 0; i < item->rowCount(); i++){
        if (((EventCommand*) item->child(i)->data().value<quintptr>())->kind()
                != EventCommandKind::None)
        {
            QJsonObject json = ((EventCommand*) item->child(i)->data()
                                .value<quintptr>())->getJSON();
            if (item->child(i)->hasChildren()){
                json["children"] = getChildrenJSON(item->child(i));
            }
            tab.append(json);
        }
    }
    return tab;
}
