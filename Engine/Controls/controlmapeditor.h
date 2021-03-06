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

#ifndef CONTROLMAPEDITOR_H
#define CONTROLMAPEDITOR_H

#include <QMouseEvent>
#include "map.h"
#include "grid.h"
#include "camera.h"
#include "mapeditorselectionkind.h"
#include "cursor.h"

// -------------------------------------------------------
//
//  CLASS ControlMapEditor
//
//  The controler of the map editor. This part is handling the mouse
//  effects (adding/removing items from the map) and loading/saving
//  map.
//
// -------------------------------------------------------

class ControlMapEditor
{
public:
    ControlMapEditor();
    virtual ~ControlMapEditor();
    Map* map() const;
    Grid* grid() const;
    Cursor* cursor() const;
    Camera* camera() const;
    void setTreeMapNode(QStandardItem* item);
    void initialize();
    Map* loadMap(int idMap, QVector3D *position);
    void deleteMap();
    void onResize(int width, int height);

    void update();
    void updateRaycasting();
    void updateMovingPortions();
    void updateMovingPortionsEastWest(Portion& newPortion);
    void updateMovingPortionsNorthSouth(Portion& newPortion);
    void updateMovingPortionsUpDown(Portion&);
    void removePortion(int i, int j, int k);
    void setPortion(int i, int j, int k, int m, int n, int o);
    void loadPortion(Portion& currentPortion, int i, int j, int k);
    void updatePortions();
    void saveTempPortions();
    void saveTempPortion(Portion portion);
    void clearPortionsToUpdate();
    void setToNotSaved();
    void save();

    QVector3D transformToNormalizedCoords(const QPoint& mouse);
    QVector4D transformToHomogeneousClip(QVector3D& normalized);
    QVector4D transformToEyeCoords(QVector4D& rayClip, QMatrix4x4& projection);
    QVector3D transformToWorldCoords(QVector4D& rayEye, QMatrix4x4& view);
    QVector3D getRayWorld(const QPoint& mouse, QMatrix4x4& projection,
                          QMatrix4x4& view);
    QVector3D getPositionOnRay(QVector3D& ray, int distance);
    void getCorrectPositionOnRay(Position& position, QVector3D &ray,
                                 int distance);

    void addRemove(MapEditorSelectionKind selection, QMouseEvent *event);
    Position getPositionSelected(MapEditorSelectionKind selection) const;
    void add(MapEditorSelectionKind selection, Position& p);
    void remove(MapEditorSelectionKind selection, Position& p);
    void addFloor(Position& p);
    void stockFloor(Position& p);
    void removeFloor(Position& p);
    void eraseFloor(Position& p);
    void addSprite(Position& p);
    void stockSprite(Position& p);
    void removeSprite(Position& p);
    void eraseSprite(Position& p);
    void addObject(Position& p);
    void removeObject(Position& p);

    Portion getGlobalPortion(Position& position) const;
    Portion getLocalPortion(Position& position) const;
    Portion getGlobalFromLocalPortion(Portion& portion) const;

    void paintGL(QMatrix4x4& modelviewProjection);

    void onMouseWheelMove(QWheelEvent *event);
    void onMouseMove(Qt::MouseButton k, QMouseEvent* event);
    void onMousePressed(MapEditorSelectionKind selection, QMouseEvent* event);
    void onKeyPressed(int k);
    void onKeyPressedWithoutRepeat(int k);
    void onKeyReleased(int);

private:

    // Widgets
    Map* m_map;
    Grid* m_grid;
    Cursor* m_cursor;
    Camera* m_camera;

    // Others
    int m_width;
    int m_height;
    QPoint m_mouse;
    QPoint m_mouseBeforeUpdate;
    Position m_positionOnPlane;
    float m_distancePlane;
    bool m_isGridOnTop;
    Position m_previousMouseCoords;
    Portion m_currentPortion;
    QSet<Portion> m_portionsToUpdate;
    QSet<Portion> m_portionsToSave;
    bool m_displayGrid;
    QStandardItem* m_treeMapNode;
};

#endif // CONTROLMAPEDITOR_H
