/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * Copyright: 2013 LXQt team
 * Authors:
 *   Alexander Sokoloff <sokoloff.a@gmail.com>
     Luís Pereira <luis.artur.pereira@gmail.com>
 *
 * Copyright: 2019 Tianjin KYLIN Information Technology Co., Ltd. *
 *
 * This program or library is free software; you can redistribute it
 * and/or modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.

 * You should have received a copy of the GNU Lesser General
 * Public License along with this library; if not, write to the
 * Free Software Foundation, Inc., 51 Franklin Street, Fifth Floor,
 * Boston, MA 02110-1301 USA
 *
 * END_COMMON_COPYRIGHT_HEADER */

#include "ukuitranslator.h"
#include <QTranslator>
#include <QLocale>
#include <QDebug>
#include <QCoreApplication>
#include <QLibraryInfo>
#include <QStringList>
#include <QStringBuilder>
#include <QFileInfo>

#include <XdgDirs>

using namespace UKUi;

bool translate(const QString &name, const QString &owner = QString());
/************************************************

 ************************************************/
QStringList *getSearchPaths()
{
    static QStringList *searchPath = 0;

    if (searchPath == 0)
    {
        searchPath = new QStringList();
        *searchPath << XdgDirs::dataDirs(QL1C('/') + QL1S(UKUI_RELATIVE_SHARE_TRANSLATIONS_DIR));
        *searchPath << QL1S(UKUI_SHARE_TRANSLATIONS_DIR);
        searchPath->removeDuplicates();
    }

    return searchPath;
}


/************************************************

 ************************************************/
QStringList UKUi::Translator::translationSearchPaths()
{
    return *(getSearchPaths());
}


/************************************************

 ************************************************/
void Translator::setTranslationSearchPaths(const QStringList &paths)
{
    QStringList *p = getSearchPaths();
    p->clear();
    *p << paths;
}


/************************************************

 ************************************************/

#if (QT_VERSION < QT_VERSION_CHECK(5,7,0))
bool translate(const QString &name, const QString &owner)
{
    const QString locale = QLocale::system().name();
    QTranslator *appTranslator = new QTranslator(qApp);

    QStringList *paths = getSearchPaths();
//    for(const QString &path : qAsConst(*paths))
    for(int i=0 ;i<(*paths).size();i++)
    {
        QStringList subPaths;

        if (!owner.isEmpty())
        {
            subPaths << (*paths)[i] + QL1C('/') + owner + QL1C('/') + name;
        }
        else
        {
            subPaths << (*paths)[i] + QL1C('/') + name;
            subPaths << (*paths)[i];
        }

//        for(const QString &p : qAsConst(subPaths))
        for(int i=0;i<subPaths.size();i++)
        {
            const QString &p=subPaths[i];
            if (appTranslator->load(name + QL1C('_') + locale, p))
            {
                QCoreApplication::installTranslator(appTranslator);
                return true;
            }
            else if (locale == QLatin1String("C") ||
                        locale.startsWith(QLatin1String("en")))
            {
                // English is the default. Even if there isn't an translation
                // file, we return true. It's translated anyway.
                delete appTranslator;
                return true;
            }
        }
    }

    // If we got here, no translation was loaded. appTranslator has no use.
    delete appTranslator;
    return false;
}
#endif

#if (QT_VERSION >= QT_VERSION_CHECK(5,7,0))
bool translate(const QString &name, const QString &owner)
{
    const QString locale = QLocale::system().name();
    QTranslator *appTranslator = new QTranslator(qApp);

    QStringList *paths = getSearchPaths();
    for(const QString &path : qAsConst(*paths))
    {
        QStringList subPaths;

        if (!owner.isEmpty())
        {
            subPaths << path + QL1C('/') + owner + QL1C('/') + name;
        }
        else
        {
            subPaths << path + QL1C('/') + name;
            subPaths << path;
        }

        for(const QString &p : qAsConst(subPaths))
        {
            if (appTranslator->load(name + QL1C('_') + locale, p))
            {
                QCoreApplication::installTranslator(appTranslator);
                return true;
            }
            else if (locale == QLatin1String("C") ||
                        locale.startsWith(QLatin1String("en")))
            {
                // English is the default. Even if there isn't an translation
                // file, we return true. It's translated anyway.
                delete appTranslator;
                return true;
            }
        }
    }

    // If we got here, no translation was loaded. appTranslator has no use.
    delete appTranslator;
    return false;
}
#endif


/************************************************

 ************************************************/
bool Translator::translateApplication(const QString &applicationName)
{
    const QString locale = QLocale::system().name();
    QTranslator *qtTranslator = new QTranslator(qApp);

    if (qtTranslator->load(QL1S("qt_") + locale, QLibraryInfo::location(QLibraryInfo::TranslationsPath)))
    {
        qApp->installTranslator(qtTranslator);
    }
    else
    {
        delete qtTranslator;
    }

    if (!applicationName.isEmpty())
        return translate(applicationName);
    else
        return translate(QFileInfo(QCoreApplication::applicationFilePath()).baseName());
}


/************************************************

 ************************************************/
bool Translator::translateLibrary(const QString &libraryName)
{
    static QSet<QString> loadedLibs;

    if (loadedLibs.contains(libraryName))
        return true;

    loadedLibs.insert(libraryName);

    return translate(libraryName);
}

bool Translator::translatePlugin(const QString &pluginName, const QString& type)
{
    static QSet<QString> loadedPlugins;

    const QString fullName = type % QL1C('/') % pluginName;
    if (loadedPlugins.contains(fullName))
        return true;

    loadedPlugins.insert(pluginName);
    return translate(pluginName, type);
}

static void loadSelfTranslation()
{
    Translator::translateLibrary(QLatin1String("libukui"));
}

Q_COREAPP_STARTUP_FUNCTION(loadSelfTranslation)
