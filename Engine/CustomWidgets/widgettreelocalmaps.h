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

#ifndef WIDGETTREELOCALMAPS_H
#define WIDGETTREELOCALMAPS_H

#include <QTreeView>
#include <QStandardItemModel>
#include "contextmenulist.h"
#include "mapproperties.h"
#include "panelproject.h"

// -------------------------------------------------------
//
//  CLASS WidgetTreeLocalMaps
//
//  A widget representing the tree of local maps.
//
// -------------------------------------------------------

class PanelProject;

class WidgetTreeLocalMaps : public QTreeView
{
    Q_OBJECT

public:
    explicit WidgetTreeLocalMaps(QWidget *parent = 0);
    ~WidgetTreeLocalMaps();
    void initializeWidgetMapEditor(WidgetMapEditor* w);
    void initializeWidgetMenuBar(WidgetMenuBarMapEditor* w);
    void initializeProject(Project* p);
    void initializeModel(QStandardItemModel* m);
    QStandardItem* getSelected() const;
    QStandardItemModel* getModel() const;
    static bool isMapIdExisting(int id);
    static int generateMapId();
    static QString generateMapName(int id);
    static void updateNodeSaved(QStandardItem* item);
    void updateAllNodesSaved();
    void deleteAllMapTemp();
    void showMap(QStandardItem* item, int idMap, QVector3D *position);
    void hideMap();
    void setMap(int id, QVector3D& position);

private:
    QStandardItemModel* m_model;
    ContextMenuList* m_contextMenuMap;
    ContextMenuList* m_contextMenuDirectory;
    WidgetMapEditor* m_widgetMapEditor;
    WidgetMenuBarMapEditor* m_widgetMenuMapEditor;
    Project* m_project;

    void updateNodesSaved(QStandardItem* item);
    void deleteMapTemp(QString &path, QStandardItem* item);
    QStandardItem* getMap(int id, QStandardItem* item);

private slots:
    void on_selectionChanged(QModelIndex, QModelIndex);
    void showContextMenu(const QPoint & p);
    void contextNewMap();
    void contextNewDirectory();
    void contextEditMap();
    void contextEditDirectory();
    void contextCopyMap();
    void contextCopyDirectory();
    void contextPasteMap();
    void contextPasteDirectory();
    void contextDeleteMap();
    void contextDeleteDirectory();
};

#endif // WIDGETTREELOCALMAPS_H
