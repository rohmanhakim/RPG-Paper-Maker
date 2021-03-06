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

#ifndef SYSTEMSTATE_H
#define SYSTEMSTATE_H

#include <QMetaType>
#include "superlistitem.h"
#include "systemcommonobject.h"
#include "systemobjectevent.h"

// -------------------------------------------------------
//
//  CLASS SystemState
//
//  A particulary state (datas).
//
// -------------------------------------------------------

class SystemState : public SuperListItem
{
public:
    SystemState();
    SystemState(int i, QString n);
    virtual ~SystemState();
    virtual void setId(int i);
    virtual void setName(QString n);
    SystemCommonObject* object() const;
    void setDefault();
    void setDefaultJson();
    void updateName();
    void setObject(SystemCommonObject* object);
    QString getJsonName();
    void updateModel();
    virtual SuperListItem* createCopy() const;
    virtual void setCopy(const SystemState &state);

    virtual void read(const QJsonObject &json);
    virtual void write(QJsonObject &json) const;

protected:
    SystemCommonObject* m_object;
    //QJsonObject m_json;
};

Q_DECLARE_METATYPE(SystemState)

#endif // SYSTEMSTATE_H
