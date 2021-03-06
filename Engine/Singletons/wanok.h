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

#ifndef WANOK_H
#define WANOK_H

#include <QString>
#include <QKeyEvent>
#include "singleton.h"
#include "project.h"
#include "map.h"
#include "enginesettings.h"

// -------------------------------------------------------
//
//  CLASS Wanok
//
//  Wanok is a special class (singleton) containing the current project,
//  and also a set of static functions (utils).
//
// -------------------------------------------------------

class Wanok : public Singleton<Wanok>
{
public:
    Wanok();
    virtual ~Wanok();
    Project* project() const;
    EngineSettings* engineSettings() const;
    void setProject(Project* p);
    void setEngineSettings(EngineSettings* e);
    int getPortionsRay() const;
    int getSquareSize() const;
    static QSet<int> mapsToSave;
    static bool isInConfig;

    // PATHS
    const static QString pathDatas;
    const static QString pathMaps;
    const static QString pathScriptsDir;
    const static QString pathCommonEvents;
    const static QString pathVariablesSwitches;
    const static QString pathSystem;
    const static QString pathBattleSystem;
    const static QString pathItems;
    const static QString pathSkills;
    const static QString pathWeapons;
    const static QString pathArmors;
    const static QString pathHeroes;
    const static QString pathMonsters;
    const static QString pathTroops;
    const static QString pathClasses;
    const static QString pathTreeMap;
    const static QString pathLangs;
    const static QString pathScripts;
    const static QString pathKeyBoard;
    const static QString pathPictures;
    const static QString pathHUD;
    const static QString pathTextures2D;
    const static QString pathBars;
    const static QString pathIcons;
    const static QString pathAutotiles;
    const static QString pathCharacters;
    const static QString pathReliefs;
    const static QString pathTilesets;
    const static QString pathEngineSettings;
    const static QString fileMapInfos;
    const static QString fileMapObjects;
    const static QString gamesFolderName;
    const static QString tempMapFolderName;
    const static QString dirGames;
    const static QString dirDesktop;

    // INTEGERS
    const static int portionSize;

    // METHODS
    static QString pathCombine(const QString& p1, const QString& p2);
    static void writeJSON(QString path, const Serializable &obj);
    static void readJSON(QString path, Serializable &obj);
    static void writeOtherJSON(QString path, const QJsonObject &obj);
    static void readOtherJSON(QString path, QJsonDocument& loadDoc);
    static bool copyPath(QString src, QString dst);
    static QString getDirectoryPath(QString& file);
    static bool isDirEmpty(QString path);
    static void copyAllFiles(QString pathSource, QString pathTarget);
    static void copyAll(QString pathSource, QString pathTarget);
    static void deleteAllFiles(QString pathSource);
    static QString getFormatNumber(int number, int format = 4, int type = 10);
    static QKeySequence getKeySequence(QKeyEvent *event);

protected:
    Project* p_project;
    EngineSettings* m_engineSettings;
};

#endif // WANOK_H
