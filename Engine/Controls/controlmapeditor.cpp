﻿/*
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

#include "controlmapeditor.h"
#include "dialogobject.h"
#include "wanok.h"

// -------------------------------------------------------
//
//  CONSTRUCTOR / DESTRUCTOR / GET / SET
//
// -------------------------------------------------------

ControlMapEditor::ControlMapEditor() :
    m_map(nullptr),
    m_grid(nullptr),
    m_cursor(nullptr),
    m_camera(new Camera),
    m_displayGrid(true)
{

}

ControlMapEditor::~ControlMapEditor(){
    deleteMap();
    delete m_camera;
    delete m_cursor;
}

Map* ControlMapEditor::map() const { return m_map; }

Grid* ControlMapEditor::grid() const { return m_grid; }

Cursor* ControlMapEditor::cursor() const { return m_cursor; }

Camera* ControlMapEditor::camera() const { return m_camera; }

void ControlMapEditor::setTreeMapNode(QStandardItem* item) {
    m_treeMapNode = item;
}

// -------------------------------------------------------
//
//  INTERMEDIARY FUNCTIONS
//
// -------------------------------------------------------

Map* ControlMapEditor::loadMap(int idMap, QVector3D* position){

    // Map
    m_map = new Map(idMap);
    m_map->initializeGL();
    m_map->loadPortions();

    // Grid
    m_grid = new Grid;
    m_grid->initializeVertices(m_map->mapProperties()->length(),
                               m_map->mapProperties()->width(), 16);
    m_grid->initializeGL();

    // Cursor
    m_cursor = new Cursor(position);
    m_cursor->initializeSquareSize(m_map->squareSize());
    m_cursor->initialize();

    // Camera
    m_camera->setDistance(710);
    m_camera->setHeight(350);
    m_camera->update(m_cursor, m_map->squareSize());

    return m_map;
}

// -------------------------------------------------------

void ControlMapEditor::deleteMap(){

    // Cursor
    if (m_cursor != nullptr){
        delete m_cursor;
        m_cursor = nullptr;
    }

    // Grid
    if (m_grid != nullptr){
        delete m_grid;
        m_grid = nullptr;
    }

    // Map
    if (m_map != nullptr){
        delete m_map;
        m_map = nullptr;
    }
}

// -------------------------------------------------------

void ControlMapEditor::onResize(int width, int height){
    m_width = width;
    m_height = height;
    m_camera->setProjection(width, height);
}

// -------------------------------------------------------
//
//  UPDATE
//
// -------------------------------------------------------

void ControlMapEditor::update(){
    updateRaycasting();

    // Update portions
    updatePortions();
    saveTempPortions();
    clearPortionsToUpdate();
    updateMovingPortions();

    // Camera
    m_camera->update(m_cursor, m_map->squareSize());

    // Mouse update
    m_mouseBeforeUpdate = m_mouse;
}

// -------------------------------------------------------

void ControlMapEditor::updateRaycasting(){

    // Raycasting plane
    QMatrix4x4 projection = m_camera->projection();
    QMatrix4x4 view = m_camera->view();
    QVector3D ray = getRayWorld(m_mouse, projection, view);
    int height = 0;
    m_distancePlane = (height - m_camera->positionY()) / ray.y();
    getCorrectPositionOnRay(m_positionOnPlane, ray, m_distancePlane);
}

// -------------------------------------------------------

void ControlMapEditor::updatePortions(){
    QSet<Portion>::iterator i;
    for (i = m_portionsToUpdate.begin(); i != m_portionsToUpdate.end(); i++){
        Portion p = *i;
        m_map->updatePortion(p);
    }
}

// -------------------------------------------------------

void ControlMapEditor::updateMovingPortions(){
    Portion newPortion = m_cursor->getPortion();

    updateMovingPortionsEastWest(newPortion);
    updateMovingPortionsNorthSouth(newPortion);
    updateMovingPortionsUpDown(newPortion);

    m_currentPortion = newPortion;
}

// -------------------------------------------------------

void ControlMapEditor::updateMovingPortionsEastWest(Portion& newPortion){
    int r = m_map->portionsRay();
    if (newPortion.x() > m_currentPortion.x()){
        int k = 0;
        for (int j = -r - 1; j <= r + 1; j++){
            int i = -r - 1;
            removePortion(i, k, j);
            for (; i < r + 1; i++)
                setPortion(i, k, j, i + 1, k, j);

            loadPortion(newPortion, r + 1, k, j);
        }
    }
    else if (newPortion.x() < m_currentPortion.x()){
        int k = 0;
        for (int j = -r - 1; j <= r + 1; j++){
            int i = r + 1;
            removePortion(i, k, j);
            for (; i > -r - 1; i--)
                setPortion(i, k, j, i - 1, k, j);

            loadPortion(newPortion, -r - 1, k, j);
        }
    }
}

// -------------------------------------------------------

void ControlMapEditor::updateMovingPortionsNorthSouth(Portion& newPortion){
    int r = m_map->portionsRay();
    if (newPortion.z() > m_currentPortion.z()){
        int k = 0;
        for (int i = -r - 1; i <= r + 1; i++){
            int j = -r - 1;
            removePortion(i, k, j);
            for (; j < r + 1; j++)
                setPortion(i, k, j, i, k, j + 1);

            loadPortion(newPortion, i, k, r + 1);
        }
    }
    else if (newPortion.z() < m_currentPortion.z()){
        int k = 0;
        for (int i = -r - 1; i <= r + 1; i++){
            int j = r + 1;
            removePortion(i, k, j);
            for (; j > -r - 1; j--)
                setPortion(i, k, j, i, k, j - 1);

            loadPortion(newPortion, i, k, -r - 1);
        }
    }
}

// -------------------------------------------------------

void ControlMapEditor::updateMovingPortionsUpDown(Portion&){
    // TODO
}

// -------------------------------------------------------

void ControlMapEditor::removePortion(int i, int j, int k){
    MapPortion* mapPortion = m_map->mapPortion(i, j, k);
    if (mapPortion != nullptr)
        delete mapPortion;
}

// -------------------------------------------------------

void ControlMapEditor::setPortion(int i, int j, int k, int m, int n, int o){
    Portion previousPortion(i, j, k);
    Portion newPortion(m, n, o);

    m_map->replacePortion(previousPortion, newPortion);
}

// -------------------------------------------------------

void ControlMapEditor::loadPortion(Portion& currentPortion, int i, int j,
                                   int k)
{
    m_map->loadPortion(currentPortion.x() + i, currentPortion.y() + j,
                       currentPortion.z() + k, i, j, k);
}

// -------------------------------------------------------

void ControlMapEditor::saveTempPortions(){
    QSet<Portion>::iterator i;
    for (i = m_portionsToSave.begin(); i != m_portionsToSave.end(); i++)
        saveTempPortion(*i);
}

// -------------------------------------------------------

void ControlMapEditor::saveTempPortion(Portion portion){
    Portion globalPortion = getGlobalFromLocalPortion(portion);
    m_map->savePortionMap(m_map->mapPortion(portion), globalPortion);
}

// -------------------------------------------------------

void ControlMapEditor::clearPortionsToUpdate(){
    m_portionsToUpdate.clear();
    m_portionsToSave.clear();
}

// -------------------------------------------------------

void ControlMapEditor::setToNotSaved(){
    m_map->setSaved(false);
    Wanok::mapsToSave.insert(m_map->mapProperties()->id());
    m_treeMapNode->setText(m_map->mapProperties()->name() + " *");
}

// -------------------------------------------------------

void ControlMapEditor::save(){
    m_treeMapNode->setText(m_map->mapProperties()->name());
}

// -------------------------------------------------------
//
//  MOUSE RAYCASTING
//
// -------------------------------------------------------

QVector3D ControlMapEditor::transformToNormalizedCoords(const QPoint& mouse){
    float x = (2.0f * mouse.x()) / m_width - 1.0f;
    float y = 1.0f - (2.0f * mouse.y()) / m_height;
    float z = 1.0f;

    return QVector3D(x, y, z);
}

// -------------------------------------------------------

QVector4D ControlMapEditor::transformToHomogeneousClip(QVector3D& normalized){
    return QVector4D(normalized.x(), normalized.y(), -1.0, 1.0);
}

// -------------------------------------------------------

QVector4D ControlMapEditor::transformToEyeCoords(QVector4D& rayClip,
                                                 QMatrix4x4& projection){
    QVector4D rayEye = projection.inverted() * rayClip;

    return QVector4D(rayEye.x(), rayEye.y(), -1.0, 0.0);
}

// -------------------------------------------------------

QVector3D ControlMapEditor::transformToWorldCoords(QVector4D& rayEye,
                                                   QMatrix4x4& view){
    QVector4D rayWorld = view.inverted() * rayEye;

    return QVector3D(rayWorld.x(), rayWorld.y(), rayWorld.z()).normalized();
}

// -------------------------------------------------------

QVector3D ControlMapEditor::getRayWorld(const QPoint& mouse,
                                        QMatrix4x4& projection,
                                        QMatrix4x4& view)
{
    QVector3D normalized = transformToNormalizedCoords(mouse);
    QVector4D homogeneous = transformToHomogeneousClip(normalized);
    QVector4D eye = transformToEyeCoords(homogeneous, projection);

    return transformToWorldCoords(eye, view);
}

// -------------------------------------------------------

QVector3D ControlMapEditor::getPositionOnRay(QVector3D &ray, int distance){
    return QVector3D(
                    ray.x() * distance + m_camera->positionX(),
                    ray.y() * distance + m_camera->positionY(),
                    ray.z() * distance + m_camera->positionZ()
                );
}

// -------------------------------------------------------

void ControlMapEditor::getCorrectPositionOnRay(Position &position,
                                               QVector3D &ray, int distance){
    QVector3D point = getPositionOnRay(ray, distance);
    int x = ((int) point.x()) / m_map->squareSize();
    int z = ((int) point.z()) / m_map->squareSize();
    if (point.x() < 0) x--;
    if (point.z() < 0) z--;

    position.setX(x);
    position.setZ(z);
}

// -------------------------------------------------------
//
//  ADD / REMOVE
//
// -------------------------------------------------------

void ControlMapEditor::addRemove(MapEditorSelectionKind selection,
                                 QMouseEvent* event)
{
    Position p = getPositionSelected(selection);
    if (m_map->isInGrid(p)){
        if (event->button() == Qt::MouseButton::LeftButton)
            add(selection, p);
        else if (event->button() == Qt::MouseButton::RightButton)
            remove(selection, p);
    }
}

// -------------------------------------------------------

Position ControlMapEditor::getPositionSelected(MapEditorSelectionKind
                                               selection) const
{
    switch (selection){
    case MapEditorSelectionKind::Land:
        return m_positionOnPlane;
    case MapEditorSelectionKind::Sprites:
        return m_positionOnPlane;
    case MapEditorSelectionKind::Objects:
        return m_positionOnPlane;
    default:
        return m_positionOnPlane;
    }
}

// -------------------------------------------------------

void ControlMapEditor::add(MapEditorSelectionKind selection, Position& p){
    switch (selection){
    case MapEditorSelectionKind::Land:
        addFloor(p);
        break;
    case MapEditorSelectionKind::Sprites:
        addSprite(p);
        break;
    case MapEditorSelectionKind::Objects:
        addObject(p); break;
    }
}

// -------------------------------------------------------

void ControlMapEditor::remove(MapEditorSelectionKind selection, Position& p){
    switch (selection){
    case MapEditorSelectionKind::Land:
        removeFloor(p);
        break;
    case MapEditorSelectionKind::Sprites:
        removeSprite(p);
        break;
    case MapEditorSelectionKind::Objects:
        removeObject(p);
        break;
    }
}

// -------------------------------------------------------
//
//  Floors
//
// -------------------------------------------------------

void ControlMapEditor::addFloor(Position& p){
    stockFloor(p);
}

// -------------------------------------------------------

void ControlMapEditor::stockFloor(Position& p){
    if (m_map->isInGrid(p)){
        Portion portion = getLocalPortion(p);
        if (m_map->isInPortion(portion)){
            MapPortion* mapPortion = m_map->mapPortion(portion);
            if (mapPortion == nullptr)
                mapPortion = m_map->createMapPortion(portion);
            if (mapPortion->addFloor(p) && m_map->saved())
                setToNotSaved();
            m_portionsToUpdate += portion;
            m_portionsToSave += portion;
        }
    }
}

// -------------------------------------------------------

void ControlMapEditor::removeFloor(Position& p){
    eraseFloor(p);
}

// -------------------------------------------------------

void ControlMapEditor::eraseFloor(Position& p){
    if (m_map->isInGrid(p)){
        Portion portion = getLocalPortion(p);
        if (m_map->isInPortion(portion)){
            MapPortion* mapPortion = m_map->mapPortion(portion);
            if (mapPortion != nullptr){
                if (mapPortion->deleteFloor(p) && m_map->saved())
                    setToNotSaved();
                m_portionsToUpdate += portion;
                m_portionsToSave += portion;
            }
        }
    }
}

// -------------------------------------------------------
//
//  Sprites
//
// -------------------------------------------------------

void ControlMapEditor::addSprite(Position& p){
    stockSprite(p);
}

// -------------------------------------------------------

void ControlMapEditor::stockSprite(Position& p){
    if (m_map->isInGrid(p)){
        Portion portion = getLocalPortion(p);
        if (m_map->isInPortion(portion)){
            MapPortion* mapPortion = m_map->mapPortion(portion);
            if (mapPortion == nullptr)
                mapPortion = m_map->createMapPortion(portion);
            if (mapPortion->addSprite(p) && m_map->saved())
                setToNotSaved();
            m_portionsToUpdate += portion;
            m_portionsToSave += portion;
        }
    }
}

// -------------------------------------------------------

void ControlMapEditor::removeSprite(Position& p){
    eraseSprite(p);
}

// -------------------------------------------------------

void ControlMapEditor::eraseSprite(Position& p){
    if (m_map->isInGrid(p)){
        Portion portion = getLocalPortion(p);
        if (m_map->isInPortion(portion)){
            MapPortion* mapPortion = m_map->mapPortion(portion);
            if (mapPortion != nullptr){
                if (mapPortion->deleteSprite(p) && m_map->saved())
                    setToNotSaved();
                m_portionsToUpdate += portion;
                m_portionsToSave += portion;
            }
        }
    }
}

// -------------------------------------------------------
//
//  Objects
//
// -------------------------------------------------------

void ControlMapEditor::addObject(Position& p){
    if (m_map->isInGrid(p)){
        Portion portion = getLocalPortion(p);
        if (m_map->isInPortion(portion)){
            Portion globalPortion = getGlobalPortion(p);
            SystemCommonObject* originalObject = nullptr;
            MapObjects* mapObjects = m_map->objectsPortion(portion);
            SystemCommonObject* object = new SystemCommonObject;

            // Generate object informations
            if (mapObjects != nullptr)
                originalObject = mapObjects->getObjectAt(p);

            if (originalObject != nullptr)
                object->setCopy(*originalObject);
            else{
                object->setDefault(Wanok::get()->project()->gameDatas()
                                   ->commonEventsDatas()->modelEventsUser());
                int id = m_map->generateObjectId();
                object->setId(id);
                object->setName(Map::generateObjectName(id));
            }

            // Open the dialog box
            DialogObject dialog(object);
            if (dialog.exec() == QDialog::Accepted){
                MapPortion* mapPortion = m_map->mapPortion(portion);
                if (mapPortion == nullptr)
                    mapPortion = m_map->createMapPortion(portion);
                if (m_map->addObject(p, mapPortion, globalPortion, object) &&
                    m_map->saved())
                {
                    setToNotSaved();
                }

                m_map->writeObjects(true);
                m_portionsToUpdate += portion;
                m_portionsToSave += portion;
            }
            else
                delete object;
        }
    }
}

// -------------------------------------------------------

void ControlMapEditor::removeObject(Position& p){
    if (m_map->isInGrid(p)){
        Portion portion = getLocalPortion(p);
        if (m_map->isInPortion(portion)){
            MapPortion* mapPortion = m_map->mapPortion(portion);
            MapObjects* mapObjects = m_map->objectsPortion(portion);
            SystemCommonObject* object;
            if (mapObjects != nullptr)
                object = mapObjects->getObjectAt(p);

            if (object != nullptr){
                if (m_map->deleteObject(p, mapPortion, object) &&
                    m_map->saved())
                {
                    setToNotSaved();
                }

                m_map->writeObjects(true);
                m_portionsToUpdate += portion;
                m_portionsToSave += portion;
            }
        }
    }
}

// -------------------------------------------------------
//
//  OTHERS
//
// -------------------------------------------------------

Portion ControlMapEditor::getGlobalPortion(Position& position) const{
    return Portion(
                position.x() / Wanok::portionSize,
                position.y() / Wanok::portionSize,
                position.z() / Wanok::portionSize);
}

// -------------------------------------------------------

Portion ControlMapEditor::getLocalPortion(Position& position) const{
    return Portion(
                (position.x() / Wanok::portionSize) -
                (m_cursor->getSquareX() / Wanok::portionSize),
                (position.y() / Wanok::portionSize) -
                (m_cursor->getSquareY() / Wanok::portionSize),
                (position.z() / Wanok::portionSize) -
                (m_cursor->getSquareZ() / Wanok::portionSize));
}

// -------------------------------------------------------

Portion ControlMapEditor::getGlobalFromLocalPortion(Portion& portion) const{
    return Portion(
                portion.x() + (m_cursor->getSquareX() / Wanok::portionSize),
                portion.y() + (m_cursor->getSquareY() / Wanok::portionSize),
                portion.z() + (m_cursor->getSquareZ() / Wanok::portionSize));
}

// -------------------------------------------------------
//
//  GL
//
// -------------------------------------------------------

void ControlMapEditor::paintGL(QMatrix4x4 &modelviewProjection){
    m_map->paintFloors(modelviewProjection);

    if (m_displayGrid){
        glDisable(GL_DEPTH_TEST);
        m_grid->paintGL(modelviewProjection);
        glEnable(GL_DEPTH_TEST);
    }

    m_map->paintOthers(modelviewProjection);

    m_cursor->paintGL(modelviewProjection);
}

// -------------------------------------------------------
//
//  EVENTS
//
// -------------------------------------------------------

void ControlMapEditor::onMouseWheelMove(QWheelEvent* event){
    if (event->delta() > 0)
        m_camera->zoomPlus(0);
    else
        m_camera->zoomLess(0);
}

// -------------------------------------------------------

void ControlMapEditor::onMouseMove(Qt::MouseButton k, QMouseEvent* event){
    m_mouse = event->pos();

    if (k == Qt::MouseButton::MiddleButton)
        m_camera->onMouseWheelPressed(m_mouse, m_mouseBeforeUpdate);
}

// -------------------------------------------------------

void ControlMapEditor::onMousePressed(MapEditorSelectionKind selection,
                                      QMouseEvent* event)
{
    // Update mouse position
    m_mouse = event->pos();

    // General updates
    update();

    // Add/Remove something
    addRemove(selection, event);
}

// -------------------------------------------------------

void ControlMapEditor::onKeyPressed(int k){
    m_cursor->onKeyPressed(
                k, m_camera->horizontalAngle(), m_map->mapProperties()->width(),
                m_map->mapProperties()->height());
}

// -------------------------------------------------------

void ControlMapEditor::onKeyPressedWithoutRepeat(int k){
    if (k == Qt::Key_G){
        m_displayGrid = !m_displayGrid;
    }
}

// -------------------------------------------------------

void ControlMapEditor::onKeyReleased(int ){

}
