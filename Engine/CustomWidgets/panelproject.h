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

#ifndef PANELPROJECT_H
#define PANELPROJECT_H

#include <QWidget>
#include <widgetmapeditor.h>
#include "project.h"
#include "widgettreelocalmaps.h"

// -------------------------------------------------------
//
//  CLASS PanelProject
//
//  The main menu panel when a project is opened.
//
// -------------------------------------------------------

class WidgetTreeLocalMaps;

namespace Ui {
class PanelProject;
}

class PanelProject : public QWidget
{
    Q_OBJECT

public:
    explicit PanelProject(QWidget *parent = 0);
    PanelProject(QWidget *parent, Project* p);
    ~PanelProject();
    Project* project() const;
    WidgetMapEditor* widgetMapEditor() const;
    WidgetTreeLocalMaps* widgetTreeLocalMaps() const;

private:
    Ui::PanelProject *ui;
    Project* m_project;
};

#endif // PANELPROJECT_H
