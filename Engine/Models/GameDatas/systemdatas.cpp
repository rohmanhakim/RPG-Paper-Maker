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

#include "systemdatas.h"
#include "wanok.h"
#include "systemcurrency.h"

// -------------------------------------------------------
//
//  CONSTRUCTOR / DESTRUCTOR / GET / SET
//
// -------------------------------------------------------

SystemDatas::SystemDatas() :
    m_idMapHero(new PrimitiveValue(1)),
    m_idObjectHero(new PrimitiveValue(1)),
    m_modelCurrencies(new QStandardItemModel),
    m_modelItemsTypes(new QStandardItemModel)
{

}

SystemDatas::~SystemDatas(){
    delete m_idMapHero;
    delete m_idObjectHero;
    SuperListItem::deleteModel(m_modelCurrencies);
    SuperListItem::deleteModel(m_modelItemsTypes);
}

void SystemDatas::read(QString path){
    Wanok::readJSON(Wanok::pathCombine(path, Wanok::pathSystem), *this);
}

int SystemDatas::portionsRay() const { return m_portionsRay; }

int SystemDatas::squareSize() const { return m_squareSize; }

QStandardItemModel* SystemDatas::modelCurrencies() const {
    return m_modelCurrencies;
}

QStandardItemModel* SystemDatas::modelItemsTypes() const {
    return m_modelItemsTypes;
}

// -------------------------------------------------------
//
//  INTERMEDIARY FUNCTIONS
//
// -------------------------------------------------------

void SystemDatas::setDefault(){
    int length;
    QStandardItem* item;

    m_portionsRay = 1;
    m_squareSize = 16;

    // Currencies
    SystemCurrency* currency;
    QString namesCurrencies[] = {"G"};
    length = (sizeof(namesCurrencies)/sizeof(*namesCurrencies));
    for (int i = 0; i < length; i++){
        item = new QStandardItem;
        currency = new SystemCurrency(i+1,
                                      new LangsTranslation(namesCurrencies[i]));
        item->setData(QVariant::fromValue(
                          reinterpret_cast<quintptr>(currency)));
        item->setFlags(item->flags() ^ (Qt::ItemIsDropEnabled));
        item->setText(currency->toString());
        m_modelCurrencies->appendRow(item);
    }

    // Items kind
    SuperListItem* sys;
    QString namesItemsKind[] = {"ingredients", "key items"};
    length = (sizeof(namesItemsKind)/sizeof(*namesItemsKind));
    for (int i = 0; i < length; i++){
        item = new QStandardItem;
        sys = new SuperListItem(i+1, namesItemsKind[i]);
        item->setData(QVariant::fromValue(reinterpret_cast<quintptr>(sys)));
        item->setFlags(item->flags() ^ (Qt::ItemIsDropEnabled));
        item->setText(sys->toString());
        m_modelItemsTypes->appendRow(item);
    }

}

// -------------------------------------------------------
//
//  READ / WRITE
//
// -------------------------------------------------------

void SystemDatas::read(const QJsonObject &json){
    QJsonArray jsonList;

    // Other options
    m_portionsRay = json["pr"].toInt();
    m_squareSize = json["ss"].toInt();
    m_idMapHero->read(json["idMapHero"].toObject());
    m_idObjectHero->read(json["idObjHero"].toObject());

    // Currencies
    jsonList = json["currencies"].toArray();
    for (int i = 0; i < jsonList.size(); i++){
        QStandardItem* item = new QStandardItem;
        SystemCurrency* sys = new SystemCurrency;
        sys->read(jsonList[i].toObject());
        item->setData(QVariant::fromValue(reinterpret_cast<quintptr>(sys)));
        item->setFlags(item->flags() ^ (Qt::ItemIsDropEnabled));
        item->setText(sys->toString());
        m_modelCurrencies->appendRow(item);
    }

    // Items kind
    jsonList = json["itemsTypes"].toArray();
    for (int i = 0; i < jsonList.size(); i++){
        QStandardItem* item = new QStandardItem;
        SuperListItem* sys = new SuperListItem;
        sys->read(jsonList[i].toObject());
        item->setData(QVariant::fromValue(reinterpret_cast<quintptr>(sys)));
        item->setFlags(item->flags() ^ (Qt::ItemIsDropEnabled));
        item->setText(sys->toString());
        m_modelItemsTypes->appendRow(item);
    }
}

// -------------------------------------------------------

void SystemDatas::write(QJsonObject &json) const{
    QJsonArray jsonArray;
    QJsonObject obj;
    int l;

    // Other options
    json["pr"] = m_portionsRay;
    json["ss"] = m_squareSize;
    m_idMapHero->write(obj);
    json["idMapHero"] = obj;
    obj = QJsonObject();
    m_idObjectHero->write(obj);
    json["idObjHero"] = obj;

    // Currencies
    jsonArray = QJsonArray();
    l = m_modelCurrencies->invisibleRootItem()->rowCount();
    for (int i = 0; i < l; i++){
        QJsonObject jsonCommon;
        SystemCurrency* sys = ((SystemCurrency*)m_modelCurrencies->item(i)
                               ->data().value<quintptr>());
        sys->write(jsonCommon);
        jsonArray.append(jsonCommon);
    }
    json["currencies"] = jsonArray;

    // Items kind
    jsonArray = QJsonArray();
    l = m_modelItemsTypes->invisibleRootItem()->rowCount();
    for (int i = 0; i < l; i++){
        QJsonObject jsonCommon;
        SuperListItem* sys = ((SuperListItem*)m_modelItemsTypes->item(i)
                              ->data().value<quintptr>());
        sys->write(jsonCommon);
        jsonArray.append(jsonCommon);
    }
    json["itemsTypes"] = jsonArray;
}
