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

#include "systemelement.h"
#include "dialogsystemelement.h"
#include "main.h"

// -------------------------------------------------------
//
//  CONSTRUCTOR / DESTRUCTOR / GET / SET
//
// -------------------------------------------------------

SystemElement::SystemElement() : SystemLang()
{
    m_efficiency = new QStandardItemModel();
}

SystemElement::SystemElement(int i, LangsTranslation* names,
                             QStandardItemModel* efficiency) :
    SystemLang(i,names),
    m_efficiency(efficiency)
{

}

SystemElement::~SystemElement(){
    delete m_efficiency;
}

QStandardItemModel* SystemElement::efficiency() const { return m_efficiency; }

// -------------------------------------------------------
//
//  INTERMEDIARY FUNCTIONS
//
// -------------------------------------------------------

bool SystemElement::openDialog(){
    SystemElement element;
    element.setCopy(*this);
    DialogSystemElement dialog(element);
    if (dialog.exec() == QDialog::Accepted){
        setCopy(element);
        return true;
    }
    return false;
}

// -------------------------------------------------------

void SystemElement::setCopy(const SystemElement& element){
    SystemLang::setCopy(element);
    m_efficiency->setHorizontalHeaderLabels(QStringList({"Elements",
                                                         "Efficiency(%)"}));
    QStandardItem* itemElement;
    QStandardItem* itemEfficiency;
    QList<QStandardItem*> row;
    SuperListItem* sysElement;
    int efficiencyElement;
    int l;

    l = element.efficiency()->invisibleRootItem()->rowCount();
    for (int i = 0; i < l; i++){
        itemElement = new QStandardItem;
        itemEfficiency = new QStandardItem;
        row = QList<QStandardItem*>();
        sysElement = (SuperListItem*) element.efficiency()->item(i)->data()
                .value<quintptr>();
        efficiencyElement = element.efficiency()->item(i,1)->data()
                .value<int>();
        itemElement->setData(QVariant::fromValue(
                                 reinterpret_cast<quintptr>(sysElement)));
        itemElement->setText(sysElement->toString());
        itemEfficiency->setData(QVariant::fromValue(efficiencyElement));
        itemEfficiency->setText(QString::number(efficiencyElement));

        row.append(itemElement);
        row.append(itemEfficiency);
        m_efficiency->appendRow(row);
    }
}

// -------------------------------------------------------
//
//  READ / WRITE
//
// -------------------------------------------------------

void SystemElement::read(const QJsonObject &json){
    SystemLang::read(json);
}

// -------------------------------------------------------

void SystemElement::readEfficiency(QStandardItemModel* model,
                                   const QJsonObject &json)
{
    QJsonArray jsonEfficiencies = json["efficiency"].toArray();
    m_efficiency->setHorizontalHeaderLabels(QStringList({"Elements",
                                                         "Efficiency(%)"}));

    QStandardItem* itemElement;
    QStandardItem* itemEfficiency;
    SuperListItem* element;
    QList<QStandardItem*> row;
    int efficiencyElement;
    for (int j = 0; j < model->invisibleRootItem()->rowCount(); j++){
        row = QList<QStandardItem*>();
        efficiencyElement = jsonEfficiencies[j].toInt();
        element = SuperListItem::getById(model->invisibleRootItem(), j+1);
        itemElement = new QStandardItem;
        itemElement->setData(QVariant::fromValue(
                                 reinterpret_cast<quintptr>(element)));
        itemElement->setText(element->toString());
        itemEfficiency = new QStandardItem;
        itemEfficiency->setData(QVariant::fromValue(efficiencyElement));
        itemEfficiency->setText(QString::number(efficiencyElement));

        row.append(itemElement);
        row.append(itemEfficiency);
        m_efficiency->appendRow(row);
    }
}

// -------------------------------------------------------

void SystemElement::write(QJsonObject &json) const{
    SystemLang::write(json);
    QJsonArray tab;
    for (int i = 0; i < m_efficiency->invisibleRootItem()->rowCount(); i++)
        tab.append(0);

    for (int i = 0; i < m_efficiency->invisibleRootItem()->rowCount(); i++){
        int id = ((SuperListItem*)(m_efficiency->item(i)->data()
                                   .value<quintptr>()))->id();
        tab[id-1] = m_efficiency->item(i,1)->data().value<int>();
    }
    json["efficiency"] = tab;
}
