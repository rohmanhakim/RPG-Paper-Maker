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

#include <QDebug>
#include <QJsonArray>
#include <QJsonDocument>
#include <cmath>
#include <QDir>
#include "map.h"
#include "wanok.h"
#include "widgettreelocalmaps.h"
#include "systemmapobject.h"

// -------------------------------------------------------
//
//  CONSTRUCTOR / DESTRUCTOR / GET / SET
//
// -------------------------------------------------------

Map::Map() :
    m_mapProperties(new MapProperties),
    m_modelObjects(new QStandardItemModel),
    m_saved(true),
    m_programStatic(nullptr),
    m_textureTileset(nullptr)
{

}

Map::Map(int id) :
    m_modelObjects(new QStandardItemModel),
    m_programStatic(nullptr),
    m_textureTileset(nullptr)
{
    QString realName = WidgetTreeLocalMaps::generateMapName(id);
    QString pathMaps = Wanok::pathCombine(Wanok::get()->project()
                                          ->pathCurrentProject(),
                                          Wanok::pathMaps);
    m_pathMap = Wanok::pathCombine(pathMaps, realName);
    QString pathTemp = Wanok::pathCombine(m_pathMap, Wanok::tempMapFolderName);

    // Reading map infos
    if (!Wanok::mapsToSave.contains(id)){
        Wanok::deleteAllFiles(pathTemp);
        Wanok::copyAllFiles(m_pathMap, pathTemp);
    }
    m_mapProperties = new MapProperties(pathTemp);
    readObjects();
    m_saved = !Wanok::mapsToSave.contains(id);
    m_portionsRay = Wanok::get()->getPortionsRay();
    m_squareSize = Wanok::get()->getSquareSize();

    // Loading textures
    // (Temporaire)
    m_textureTileset = new QOpenGLTexture(
                QImage(QString(
                           Wanok::pathCombine(Wanok::get()->project()
                                              ->pathCurrentProject(),
                           Wanok::pathCombine(Wanok::pathTilesets,
                                              "plains.png")))));
    m_textureTileset->setMinificationFilter(QOpenGLTexture::Filter::Nearest);
    m_textureTileset->setMagnificationFilter(QOpenGLTexture::Filter::Nearest);

    QOpenGLTexture* texture = new QOpenGLTexture(
                QImage(QString(
                           Wanok::pathCombine(Wanok::get()->project()
                                              ->pathCurrentProject(),
                           Wanok::pathCombine(Wanok::pathCharacters,
                                              "lucas.png")))));
    texture->setMinificationFilter(QOpenGLTexture::Filter::Nearest);
    texture->setMagnificationFilter(QOpenGLTexture::Filter::Nearest);
    m_texturesCharacters[1] = texture;
}

Map::Map(MapProperties* properties) :
    m_mapProperties(properties),
    m_modelObjects(new QStandardItemModel),
    m_programStatic(nullptr)
{

}

Map::~Map() {
    delete m_mapProperties;

    QHash<Portion, MapPortion*>::iterator i;
    for (i = m_mapPortions.begin(); i != m_mapPortions.end(); i++){
        MapPortion* portion = i.value();
        if (portion != nullptr)
            delete portion;
    }

    SuperListItem::deleteModel(m_modelObjects);

    if (m_programStatic != nullptr)
        delete m_programStatic;


    delete m_textureTileset;

    QHash<int, QOpenGLTexture*>::iterator j;
    for (j = m_texturesCharacters.begin(); j != m_texturesCharacters.end(); j++)
        delete j.value();
}

MapProperties* Map::mapProperties() const { return m_mapProperties; }

void Map::setMapProperties(MapProperties* p) { m_mapProperties = p; }

int Map::squareSize() const { return m_squareSize; }

int Map::portionsRay() const { return m_portionsRay; }

bool Map::saved() const { return m_saved; }

void Map::setSaved(bool b){ m_saved = b; }

QStandardItemModel* Map::modelObjects() const { return m_modelObjects; }

MapPortion* Map::mapPortion(Portion &p){
    return m_mapPortions.value(p);
}

MapPortion* Map::mapPortion(int x, int y, int z){
    Portion p(x,y,z);
    return mapPortion(p);
}

MapObjects* Map::objectsPortion(Portion &p){
    MapPortion* mapPortion = this->mapPortion(p);
    if (mapPortion != nullptr)
        return mapPortion->mapObjects();

    return nullptr;
}

MapObjects* Map::objectsPortion(int x, int y, int z){
    Portion p(x,y,z);
    return objectsPortion(p);
}

// -------------------------------------------------------
//
//  INTERMEDIARY FUNCTIONS
//
// -------------------------------------------------------

void Map::writeNewMap(QString path, MapProperties& properties){
    QJsonArray jsonObject;
    writeMap(path, properties, jsonObject);
}

// -------------------------------------------------------

void Map::writeDefaultMap(QString path){
    MapProperties properties;
    QJsonArray jsonObject;
    QJsonObject json;

    SystemMapObject super(1, "Hero", Portion(0, 0, 0));
    super.write(json);
    jsonObject.append(json);
    QString pathMap = writeMap(path, properties, jsonObject);

    // Portion
    MapPortion mapPortion;
    Position position(8, 0, 0, 8, 0);
    SystemCommonObject* o = new SystemCommonObject(1, "Hero", 1,
                                                   new QStandardItemModel,
                                                   new QStandardItemModel);
    mapPortion.addObject(position, o);
    Wanok::writeJSON(Wanok::pathCombine(pathMap, getPortionPathMap(0, 0, 0)),
                     mapPortion);
}

// -------------------------------------------------------

QString Map::writeMap(QString path, MapProperties& properties,
                      QJsonArray& jsonObject)
{
    QString dirMaps = Wanok::pathCombine(path, Wanok::pathMaps);
    QString mapName = properties.realName();
    QDir(dirMaps).mkdir(mapName);
    QString dirMap = Wanok::pathCombine(dirMaps, mapName);

    // Properties
     Wanok::writeJSON(Wanok::pathCombine(dirMap, Wanok::fileMapInfos), properties);

    // Objects
    QJsonObject json;
    json["objs"] = jsonObject;
    Wanok::writeOtherJSON(Wanok::pathCombine(dirMap, Wanok::fileMapObjects), json);

    QDir(dirMap).mkdir(Wanok::tempMapFolderName);

    return dirMap;
}

// -------------------------------------------------------

QString Map::getPortionPathMap(int i, int j, int k){
    return QString::number(i) + "_" + QString::number(j) + "_" +
            QString::number(k) + ".json";
}

// -------------------------------------------------------

QString Map::getPortionPath(int i, int j, int k){
    return Wanok::pathCombine(m_pathMap, Wanok::pathCombine(
                                  Wanok::tempMapFolderName,
                                  getPortionPathMap(i, j, k)));
}

// -------------------------------------------------------

void Map::setModelObjects(QStandardItemModel* model){
    QStandardItem* item;
    SystemMapObject* super;

    item = new QStandardItem;
    super = new SystemMapObject(-1, "This object", Portion());
    item->setData(QVariant::fromValue(reinterpret_cast<quintptr>(super)));
    item->setText(super->name());
    model->appendRow(item);
    item = new QStandardItem;
    super = new SystemMapObject(0, "Hero", Portion());
    item->setData(QVariant::fromValue(reinterpret_cast<quintptr>(super)));
    item->setText(super->name());
    model->appendRow(item);
}

// -------------------------------------------------------

MapPortion* Map::loadPortionMap(int i, int j, int k){
    QString path = getPortionPath(i, j, k);
    if (QFile(path).exists()){
        MapPortion* portion = new MapPortion;
        Wanok::readJSON(path, *portion);

        // Static update
        portion->initializeVertices(m_squareSize);
        portion->initializeGL(m_programStatic);
        m_programStatic->bind();
        portion->updateGL();
        m_programStatic->release();

        return portion;
    }

    return nullptr;
}


// -------------------------------------------------------

void Map::savePortionMap(MapPortion* mapPortion, Portion& portion){
    QString path = getPortionPath(portion.x(), portion.y(), portion.z());
    if (mapPortion == nullptr){
        QFile file(path);
        if (file.exists())
            file.remove();
    }
    else
        Wanok::writeJSON(path, *mapPortion);
}

// -------------------------------------------------------

QString Map::getMapInfosPath() const{
    return Wanok::pathCombine(m_pathMap,
                              Wanok::pathCombine(Wanok::tempMapFolderName,
                                                 Wanok::fileMapInfos));
}

// -------------------------------------------------------

QString Map::getMapObjectsPath() const{
    return Wanok::pathCombine(m_pathMap,
                              Wanok::pathCombine(Wanok::tempMapFolderName,
                                                 Wanok::fileMapObjects));
}

// -------------------------------------------------------

void Map::loadPortion(int realX, int realY, int realZ, int x, int y, int z){
    Portion localPortion(x, y, z);
    MapPortion* newMapPortion = loadPortionMap(realX, realY, realZ);

    m_mapPortions[localPortion] = newMapPortion;
}

// -------------------------------------------------------

void Map::replacePortion(Portion& previousPortion, Portion& newPortion){
    m_mapPortions[previousPortion] = m_mapPortions[newPortion];
}

// -------------------------------------------------------

void Map::updatePortion(Portion& p){
    MapPortion* mapPortion = m_mapPortions[p];
    if (mapPortion->isEmpty()){
        delete mapPortion;
        m_mapPortions[p] = nullptr;
    }
    else{
        mapPortion->initializeVertices(m_squareSize);
        mapPortion->initializeGL(m_programStatic);
        m_programStatic->bind();
        mapPortion->updateGL();
        m_programStatic->release();
    }
}

// -------------------------------------------------------

void Map::loadPortions(){
    for (int i = -m_portionsRay - 1; i <= m_portionsRay + 1; i++){
        for (int j = -m_portionsRay - 1; j <= m_portionsRay + 1; j++){
            for (int k = -m_portionsRay - 1; k <= m_portionsRay + 1; k++){
                loadPortion(i, j, k, i, j, k);
            }
        }
    }
}

// -------------------------------------------------------

bool Map::isInGrid(Position& position) const{
    return (position.x() >= 0 && position.x() < m_mapProperties->length() &&
            position.z() >= 0 && position.z() < m_mapProperties->width());
}

// -------------------------------------------------------

bool Map::isInPortion(Portion& portion, int offset) const{
    return (portion.x() <= (m_portionsRay + offset) &&
            portion.x() >= -(m_portionsRay + offset) &&
            portion.y() <= (m_portionsRay + offset) &&
            portion.y() >= -(m_portionsRay + offset) &&
            portion.z() <= (m_portionsRay + offset) &&
            portion.z() >= -(m_portionsRay + offset));
}

// -------------------------------------------------------

MapPortion* Map::createMapPortion(Portion &p){
    MapPortion* portion = new MapPortion;
    m_mapPortions[p] = portion;
    return portion;
}

// -------------------------------------------------------

bool Map::addObject(Position& p, MapPortion* mapPortion, Portion &globalPortion,
                    SystemCommonObject *object)
{
    bool b = mapPortion->addObject(p, object);

    int row = removeObject(object);
    SystemMapObject* newObject = new SystemMapObject(object->id(),
                                                     object->name(),
                                                     globalPortion);
    QStandardItem* item = new QStandardItem;
    item->setData(QVariant::fromValue(reinterpret_cast<quintptr>(newObject)));
    item->setText(newObject->toString());
    m_modelObjects->insertRow(row, item);

    return b;
}

int Map::removeObject(SystemCommonObject *object){
    SystemMapObject* super;

    for (int i = 0; i < m_modelObjects->invisibleRootItem()->rowCount(); i++){
        super = ((SystemMapObject*) m_modelObjects->item(i)->data()
                 .value<quintptr>());
        if (object->id() == super->id()){
            m_modelObjects->removeRow(i);
            delete super;
            return i;
        }
    }

    return m_modelObjects->invisibleRootItem()->rowCount();
}

// -------------------------------------------------------

bool Map::deleteObject(Position& p, MapPortion *mapPortion,
                       SystemCommonObject *object)
{
    removeObject(object);

    bool b = mapPortion->deleteObject(p);

    return b;
}

// -------------------------------------------------------

void Map::save(Map* currentMap){
    QString pathTemp = Wanok::pathCombine(m_pathMap, Wanok::tempMapFolderName);

    Wanok::deleteAllFiles(m_pathMap);
    Wanok::copyAllFiles(pathTemp, m_pathMap);

    if (currentMap != nullptr &&
            currentMap->mapProperties()->id() != m_mapProperties->id())
    {
        Wanok::deleteAllFiles(pathTemp);
    }
}

// -------------------------------------------------------

bool Map::isObjectIdExisting(int id) const{
    SystemMapObject* super;

    for (int i = 0; i < m_modelObjects->invisibleRootItem()->rowCount(); i++){
        super = ((SystemMapObject*) m_modelObjects->item(i)->data()
                 .value<quintptr>());
        if (id == super->id())
            return true;
    }

    return false;
}

// -------------------------------------------------------

int Map::generateObjectId() const{
    int id, l;

    l = m_modelObjects->invisibleRootItem()->rowCount();
    for (id = 1; id < l + 1; id++){
        if (!isObjectIdExisting(id))
            break;
    }

    return id;
}

// -------------------------------------------------------

QString Map::generateObjectName(int id){
    return "OBJ" + Wanok::getFormatNumber(id);
}

// -------------------------------------------------------
//
//  GL
//
// -------------------------------------------------------

void Map::initializeGL(){
    initializeOpenGLFunctions();

    // Create Shader
    m_programStatic = new QOpenGLShaderProgram();
    m_programStatic->addShaderFromSourceFile(QOpenGLShader::Vertex,
                                             ":/Shaders/static.vert");
    m_programStatic->addShaderFromSourceFile(QOpenGLShader::Fragment,
                                             ":/Shaders/static.frag");
    m_programStatic->link();
    m_programStatic->bind();

    // Uniform location of camera
    u_modelviewProjectionStatic = m_programStatic
            ->uniformLocation("modelviewProjection");

    // Release
    m_programStatic->release();
}

// -------------------------------------------------------

void Map::paintFloors(QMatrix4x4& modelviewProjection){

    m_programStatic->bind();
    m_programStatic->setUniformValue(u_modelviewProjectionStatic,
                                     modelviewProjection);
    m_textureTileset->bind();

    for (int i = -m_portionsRay; i <= m_portionsRay; i++){
        for (int j = -m_portionsRay; j <= m_portionsRay; j++){
            for (int k = -m_portionsRay; k <= m_portionsRay; k++){
                Portion portion(i, j, k);
                MapPortion* mapPortion = m_mapPortions[portion];
                if (mapPortion != nullptr){
                    mapPortion->paintFloors();
                }
            }
        }
    }

    m_programStatic->release();
}

// -------------------------------------------------------

void Map::paintOthers(QMatrix4x4 &modelviewProjection){
    m_programStatic->bind();
    m_programStatic->setUniformValue(u_modelviewProjectionStatic,
                                     modelviewProjection);

    for (int i = -m_portionsRay; i <= m_portionsRay; i++){
        for (int j = -m_portionsRay; j <= m_portionsRay; j++){
            for (int k = -m_portionsRay; k <= m_portionsRay; k++){
                Portion portion(i, j, k);
                MapPortion* mapPortion = m_mapPortions[portion];
                if (mapPortion != nullptr){
                    mapPortion->paintSprites();
                }
            }
        }
    }

    m_texturesCharacters[1]->bind();
    for (int i = -m_portionsRay; i <= m_portionsRay; i++){
        for (int j = -m_portionsRay; j <= m_portionsRay; j++){
            for (int k = -m_portionsRay; k <= m_portionsRay; k++){
                Portion portion(i, j, k);
                MapPortion* mapPortion = m_mapPortions[portion];
                if (mapPortion != nullptr){
                    mapPortion->paintObjects();
                }
            }
        }
    }

    m_programStatic->release();
}

// -------------------------------------------------------
//
//  READ / WRITE
//
// -------------------------------------------------------

void Map::readObjects(){
    QString pathTemp = Wanok::pathCombine(m_pathMap, Wanok::tempMapFolderName);
    QString path = Wanok::pathCombine(pathTemp, Wanok::fileMapObjects);
    QJsonDocument loadDoc;
    Wanok::readOtherJSON(path, loadDoc);
    QJsonObject json = loadDoc.object();
    readJSONArray(json["objs"].toArray());
}

// -------------------------------------------------------

void Map::writeObjects(bool temp) const{
    QString pathTemp = temp ?
                Wanok::pathCombine(m_pathMap, Wanok::tempMapFolderName)
              : m_pathMap;
    QString path = Wanok::pathCombine(pathTemp, Wanok::fileMapObjects);
    QJsonObject json;
    QJsonArray portions;
    writeJSONArray(portions);
    json["objs"] = portions;
    Wanok::writeOtherJSON(path, json);
}

// -------------------------------------------------------

void Map::readJSONArray(const QJsonArray & tab){
    QStandardItem* item;
    SystemMapObject* super;

    setModelObjects(m_modelObjects);
    for (int i = 0; i < tab.size(); i++){
        item = new QStandardItem;
        super = new SystemMapObject;
        super->read(tab.at(i).toObject());
        item->setData(QVariant::fromValue(reinterpret_cast<quintptr>(super)));
        item->setText(super->toString());
        m_modelObjects->appendRow(item);
    }
}

// -------------------------------------------------------

void Map::writeJSONArray(QJsonArray & tab) const{
    SystemMapObject* super;

    for (int i = 2; i < m_modelObjects->invisibleRootItem()->rowCount(); i++){
        QJsonObject obj;
        super = ((SystemMapObject*) m_modelObjects->item(i)->data()
                 .value<quintptr>());
        super->write(obj);
        tab.append(obj);
    }
}
