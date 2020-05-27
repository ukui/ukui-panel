/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * Copyright: 2010-2011 Razor team
 * Authors:
 *   Alexander Sokoloff <sokoloff.a@gmail.com>
 *   Petr Vanek <petr@scribus.info>
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

#include "ukuisettings.h"
#include <QDebug>
#include <QEvent>
#include <QDir>
#include <QStringList>
#include <QMutex>
#include <QFileSystemWatcher>
#include <QSharedData>
#include <QTimerEvent>

#include <XdgDirs>
#if QT_VERSION < QT_VERSION_CHECK(5, 6, 0)
#include <algorithm>
#endif

using namespace UKUi;

class UKUi::SettingsPrivate
{
public:
    SettingsPrivate(Settings* parent, bool useXdgFallback):
        mFileChangeTimer(0),
        mAppChangeTimer(0),
        mAddWatchTimer(0),
        mParent(parent)
    {
        // HACK: we need to ensure that the user (~/.config/ukui/<module>.conf)
        //       exists to have functional mWatcher
        if (!mParent->contains(QL1S("__userfile__")))
        {
            mParent->setValue(QL1S("__userfile__"), true);
#if defined(WITH_XDG_DIRS_FALLBACK)
            if (useXdgFallback)
            {
                //Note: Qt doesn't support the xdg spec regarding the XDG_CONFIG_DIRS
                //https://bugreports.qt.io/browse/QTBUG-34919
                //(Partial) workaround: if the the user specific config file doesn't exist
                //we try to find some system-wide configuration file and copy all settings into
                //the user specific file
                const QString org = mParent->organizationName();
                const QString file_name = QFileInfo{mParent->fileName()}.fileName();
                QStringList dirs = XdgDirs::configDirs();
#if QT_VERSION < QT_VERSION_CHECK(5, 6, 0)
                std::reverse(dirs.begin(), dirs.end());
                for (auto dir_i = dirs.begin(), dir_e = dirs.end(); dir_i != dir_e; ++dir_i)
#else // QT_VERSION
                for (auto dir_i = dirs.rbegin(), dir_e = dirs.rend(); dir_i != dir_e; ++dir_i)
#endif

                {
                    QDir dir{*dir_i};
                    if (dir.cd(mParent->organizationName()) && dir.exists(file_name))
                    {
                        QSettings system_settings{dir.absoluteFilePath(file_name), QSettings::IniFormat};
                        const QStringList keys = system_settings.allKeys();
                        for (const QString & key : keys)
                        {
                            mParent->setValue(key, system_settings.value(key));
                        }
                    }
                }
            }
#endif
            mParent->sync();
        }
        mWatcher.addPath(mParent->fileName());
        QObject::connect(&(mWatcher), &QFileSystemWatcher::fileChanged, mParent, &Settings::_fileChanged);
    }

    QString localizedKey(const QString& key) const;

    QFileSystemWatcher mWatcher;
    int mFileChangeTimer;
    int mAppChangeTimer;
    int mAddWatchTimer;

private:
    Settings* mParent;
};


UKUiTheme* UKUiTheme::mInstance = 0;

class UKUi::UKUiThemeData: public QSharedData {
public:
    UKUiThemeData(): mValid(false) {}
    QString loadQss(const QString& qssFile) const;
    QString findTheme(const QString &themeName);

    QString mName;
    QString mPath;
    QString mPreviewImg;
    bool mValid;

};


class UKUi::GlobalSettingsPrivate
{
public:
    GlobalSettingsPrivate(GlobalSettings *parent):
        mParent(parent),
        mThemeUpdated(0ull)
    {

    }

    GlobalSettings *mParent;
    QString mIconTheme;
    QString mUKUiTheme;
    qlonglong mThemeUpdated;

};


/************************************************

 ************************************************/
Settings::Settings(const QString& module, QObject* parent) :
    QSettings(QL1S("ukui"), module, parent),
    d_ptr(new SettingsPrivate(this, true))
{
}


/************************************************

 ************************************************/
Settings::Settings(const QString &fileName, QSettings::Format format, QObject *parent):
    QSettings(fileName, format, parent),
    d_ptr(new SettingsPrivate(this, false))
{
}


/************************************************

 ************************************************/
Settings::Settings(const QSettings* parentSettings, const QString& subGroup, QObject* parent):
    QSettings(parentSettings->organizationName(), parentSettings->applicationName(), parent),
    d_ptr(new SettingsPrivate(this, false))
{
    beginGroup(subGroup);
}


/************************************************

 ************************************************/
Settings::Settings(const QSettings& parentSettings, const QString& subGroup, QObject* parent):
    QSettings(parentSettings.organizationName(), parentSettings.applicationName(), parent),
    d_ptr(new SettingsPrivate(this, false))
{
    beginGroup(subGroup);
}


/************************************************

 ************************************************/
Settings::~Settings()
{
    // because in the Settings::Settings(const QString& module, QObject* parent)
    // constructor there is no beginGroup() called...
    if (!group().isEmpty())
        endGroup();

    delete d_ptr;
}

bool Settings::event(QEvent *event)
{
    if (event->type() == QEvent::UpdateRequest)
    {
        // delay the settingsChanged* signal emitting for:
        //  - checking in _fileChanged
        //  - merging emitting the signals
        if(d_ptr->mAppChangeTimer)
            killTimer(d_ptr->mAppChangeTimer);
        d_ptr->mAppChangeTimer = startTimer(100);
    }
    else if (event->type() == QEvent::Timer)
    {
        const int timer = static_cast<QTimerEvent*>(event)->timerId();
        killTimer(timer);
        if (timer == d_ptr->mFileChangeTimer)
        {
            d_ptr->mFileChangeTimer = 0;
            fileChanged(); // invoke the real fileChanged() handler.
        } else if (timer == d_ptr->mAppChangeTimer)
        {
            d_ptr->mAppChangeTimer = 0;
            // do emit the signals
            emit settingsChangedByApp();
            emit settingsChanged();
        } else if (timer == d_ptr->mAddWatchTimer)
        {
            d_ptr->mAddWatchTimer = 0;
            //try to re-add filename for watching
            addWatchedFile(fileName());
        }
    }

    return QSettings::event(event);
}

void Settings::fileChanged()
{
    sync();
    emit settingsChangedFromExternal();
    emit settingsChanged();
}

void Settings::_fileChanged(QString path)
{
    // check if the file isn't changed by our logic
    // FIXME: this is poor implementation; should we rather compute some hash of values if changed by external?
    if (0 == d_ptr->mAppChangeTimer)
    {
        // delay the change notification for 100 ms to avoid
        // unnecessary repeated loading of the same config file if
        // the file is changed for several times rapidly.
        if(d_ptr->mFileChangeTimer)
            killTimer(d_ptr->mFileChangeTimer);
        d_ptr->mFileChangeTimer = startTimer(1000);
    }

    addWatchedFile(path);
}

void Settings::addWatchedFile(QString const & path)
{
    // D*mn! yet another Qt 5.4 regression!!!
    // See the bug report: https://github.com/ukui/ukui/issues/441
    // Since Qt 5.4, QSettings uses QSaveFile to save the config files.
    // https://github.com/qtproject/qtbase/commit/8d15068911d7c0ba05732e2796aaa7a90e34a6a1#diff-e691c0405f02f3478f4f50a27bdaecde
    // QSaveFile will save the content to a new temp file, and replace the old file later.
    // Hence the existing config file is not changed. Instead, it's deleted and then replaced.
    // This new behaviour unfortunately breaks QFileSystemWatcher.
    // After file deletion, we can no longer receive any new change notifications.
    // The most ridiculous thing is, QFileSystemWatcher does not provide a
    // way for us to know if a file is deleted. WT*?
    // Luckily, I found a workaround: If the file path no longer exists
    // in the watcher's files(), this file is deleted.
    if(!d_ptr->mWatcher.files().contains(path))
        // in some situations adding fails because of non-existing file (e.g. editting file in external program)
        if (!d_ptr->mWatcher.addPath(path) && 0 == d_ptr->mAddWatchTimer)
            d_ptr->mAddWatchTimer = startTimer(100);

}


/************************************************

 ************************************************/
const GlobalSettings *Settings::globalSettings()
{
    static QMutex mutex;
    static GlobalSettings *instance = 0;
    if (!instance)
    {
        mutex.lock();

        if (!instance)
            instance = new GlobalSettings();

        mutex.unlock();
    }

    return instance;
}


/************************************************
 LC_MESSAGES value      Possible keys in order of matching
 lang_COUNTRY@MODIFIER  lang_COUNTRY@MODIFIER, lang_COUNTRY, lang@MODIFIER, lang,
                        default value
 lang_COUNTRY           lang_COUNTRY, lang, default value
 lang@MODIFIER          lang@MODIFIER, lang, default value
 lang                   lang, default value
 ************************************************/
QString SettingsPrivate::localizedKey(const QString& key) const
{

    QString lang = QString::fromLocal8Bit(qgetenv("LC_MESSAGES"));

    if (lang.isEmpty())
        lang = QString::fromLocal8Bit(qgetenv("LC_ALL"));

    if (lang.isEmpty())
         lang = QString::fromLocal8Bit(qgetenv("LANG"));


    QString modifier = lang.section(QL1C('@'), 1);
    if (!modifier.isEmpty())
        lang.truncate(lang.length() - modifier.length() - 1);

    QString encoding = lang.section(QL1C('.'), 1);
    if (!encoding.isEmpty())
        lang.truncate(lang.length() - encoding.length() - 1);


    QString country = lang.section(QL1C('_'), 1);
    if (!country.isEmpty())
        lang.truncate(lang.length() - country.length() - 1);



    //qDebug() << "LC_MESSAGES: " << getenv("LC_MESSAGES");
    //qDebug() << "Lang:" << lang;
    //qDebug() << "Country:" << country;
    //qDebug() << "Encoding:" << encoding;
    //qDebug() << "Modifier:" << modifier;

    if (!modifier.isEmpty() && !country.isEmpty())
    {
        QString k = QString::fromLatin1("%1[%2_%3@%4]").arg(key, lang, country, modifier);
        //qDebug() << "\t try " << k << mParent->contains(k);
        if (mParent->contains(k))
            return k;
    }

    if (!country.isEmpty())
    {
        QString k = QString::fromLatin1("%1[%2_%3]").arg(key, lang, country);
        //qDebug() << "\t try " << k  << mParent->contains(k);
        if (mParent->contains(k))
            return k;
    }

    if (!modifier.isEmpty())
    {
        QString k = QString::fromLatin1("%1[%2@%3]").arg(key, lang, modifier);
        //qDebug() << "\t try " << k  << mParent->contains(k);
        if (mParent->contains(k))
            return k;
    }

    QString k = QString::fromLatin1("%1[%2]").arg(key, lang);
    //qDebug() << "\t try " << k  << mParent->contains(k);
    if (mParent->contains(k))
        return k;


    //qDebug() << "\t try " << key  << mParent->contains(key);
    return key;
}

/************************************************

 ************************************************/
QVariant Settings::localizedValue(const QString& key, const QVariant& defaultValue) const
{
    Q_D(const Settings);
    return value(d->localizedKey(key), defaultValue);
}


/************************************************

 ************************************************/
void Settings::setLocalizedValue(const QString &key, const QVariant &value)
{
    Q_D(const Settings);
    setValue(d->localizedKey(key), value);
}


/************************************************

 ************************************************/
UKUiTheme::UKUiTheme():
    d(new UKUiThemeData)
{
}


/************************************************

 ************************************************/
UKUiTheme::UKUiTheme(const QString &path):
    d(new UKUiThemeData)
{
    if (path.isEmpty())
        return;

    QFileInfo fi(path);
    if (fi.isAbsolute())
    {
        d->mPath = path;
        d->mName = fi.fileName();
        d->mValid = fi.isDir();
    }
    else
    {
        d->mName = path;
        d->mPath = d->findTheme(path);
        d->mValid = !(d->mPath.isEmpty());
    }

    if (QDir(path).exists(QL1S("preview.png")))
        d->mPreviewImg = path + QL1S("/preview.png");
}


/************************************************

 ************************************************/
QString UKUiThemeData::findTheme(const QString &themeName)
{
    if (themeName.isEmpty())
        return QString();

    QStringList paths;
    QLatin1String fallback(UKUI_INSTALL_PREFIX);

    paths << XdgDirs::dataHome(false);
    paths << XdgDirs::dataDirs();

    if (!paths.contains(fallback))
        paths << fallback;
#if (QT_VERSION < QT_VERSION_CHECK(5,7,0))
    for(int i=0;i<paths.size();i++){
        const QString &path=paths[i];
#endif
#if (QT_VERSION >= QT_VERSION_CHECK(5,7,0))
    for(const QString &path : qAsConst(paths)){
#endif
        QDir dir(QString::fromLatin1("%1/ukui/themes/%2").arg(path, themeName));
        if (dir.isReadable())
            return dir.absolutePath();
    }

    return QString();
}


/************************************************

 ************************************************/
UKUiTheme::UKUiTheme(const UKUiTheme &other):
    d(other.d)
{
}


/************************************************

 ************************************************/
UKUiTheme::~UKUiTheme()
{
}


/************************************************

 ************************************************/
UKUiTheme& UKUiTheme::operator=(const UKUiTheme &other)
{
    d = other.d;
    return *this;
}


/************************************************

 ************************************************/
bool UKUiTheme::isValid() const
{
    return d->mValid;
}


/************************************************

 ************************************************/
QString UKUiTheme::name() const
{
    return d->mName;
}

/************************************************

 ************************************************/
QString UKUiTheme::path() const
{
    return d->mPath;
}


/************************************************

 ************************************************/
QString UKUiTheme::previewImage() const
{
    return d->mPreviewImg;
}


/************************************************

 ************************************************/
QString UKUiTheme::qss(const QString& module) const
{
    return d->loadQss(QStringLiteral("%1/%2.qss").arg(d->mPath, module));
}


/************************************************

 ************************************************/
QString UKUiThemeData::loadQss(const QString& qssFile) const
{
    QFile f(qssFile);
    if (! f.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return QString();
    }

    QString qss = QString::fromLocal8Bit(f.readAll());
    f.close();

    if (qss.isEmpty())
        return QString();

    // handle relative paths
    QString qssDir = QFileInfo(qssFile).canonicalPath();
    qss.replace(QRegExp(QL1S("url.[ \\t\\s]*"), Qt::CaseInsensitive, QRegExp::RegExp2), QL1S("url(") + qssDir + QL1C('/'));

    return qss;
}


/************************************************

 ************************************************/
QString UKUiTheme::desktopBackground(int screen) const
{
    QString wallpaperCfgFileName = QString::fromLatin1("%1/wallpaper.cfg").arg(d->mPath);

    if (wallpaperCfgFileName.isEmpty())
        return QString();

    QSettings s(wallpaperCfgFileName, QSettings::IniFormat);
    QString themeDir = QFileInfo(wallpaperCfgFileName).absolutePath();
    // There is something strange... If I remove next line the wallpapers array is not found...
    s.childKeys();
    s.beginReadArray(QL1S("wallpapers"));

    s.setArrayIndex(screen - 1);
    if (s.contains(QL1S("file")))
        return QString::fromLatin1("%1/%2").arg(themeDir, s.value(QL1S("file")).toString());

    s.setArrayIndex(0);
    if (s.contains(QL1S("file")))
        return QString::fromLatin1("%1/%2").arg(themeDir, s.value(QL1S("file")).toString());

    return QString();
}


/************************************************

 ************************************************/
const UKUiTheme &UKUiTheme::currentTheme()
{
    static UKUiTheme theme;
    QString name = Settings::globalSettings()->value(QL1S("theme")).toString();
    if (theme.name() != name)
    {
        theme = UKUiTheme(name);
    }
    return theme;
}


/************************************************

 ************************************************/
QList<UKUiTheme> UKUiTheme::allThemes()
{
    QList<UKUiTheme> ret;
    QSet<QString> processed;

    QStringList paths;
    paths << XdgDirs::dataHome(false);
    paths << XdgDirs::dataDirs();

#if (QT_VERSION < QT_VERSION_CHECK(5,7,0))
    for(int i=0;i<paths.size();i++){
        const QString &path=paths[i];
#endif
#if (QT_VERSION >= QT_VERSION_CHECK(5,7,0))
    for(const QString &path : qAsConst(paths)){
#endif
        QDir dir(QString::fromLatin1("%1/ukui/themes").arg(path));
        const QFileInfoList dirs = dir.entryInfoList(QDir::AllDirs | QDir::NoDotAndDotDot);

        for(const QFileInfo &dir : dirs)
        {
            if (!processed.contains(dir.fileName()) &&
                 QDir(dir.absoluteFilePath()).exists(QL1S("ukui-panel.qss")))
            {
                processed << dir.fileName();
                ret << UKUiTheme(dir.absoluteFilePath());
            }

        }
    }

    return ret;
}


/************************************************

 ************************************************/
SettingsCache::SettingsCache(QSettings &settings) :
    mSettings(settings)
{
    loadFromSettings();
}


/************************************************

 ************************************************/
SettingsCache::SettingsCache(QSettings *settings) :
    mSettings(*settings)
{
    loadFromSettings();
}


/************************************************

 ************************************************/
void SettingsCache::loadFromSettings()
{
    const QStringList keys = mSettings.allKeys();

    const int N = keys.size();
    for (int i = 0; i < N; ++i) {
        mCache.insert(keys.at(i), mSettings.value(keys.at(i)));
    }
}


/************************************************

 ************************************************/
void SettingsCache::loadToSettings()
{
    QHash<QString, QVariant>::const_iterator i = mCache.constBegin();

    while(i != mCache.constEnd())
    {
        mSettings.setValue(i.key(), i.value());
        ++i;
    }

    mSettings.sync();
}


/************************************************

 ************************************************/
GlobalSettings::GlobalSettings():
    Settings(QL1S("ukui")),
    d_ptr(new GlobalSettingsPrivate(this))
{
    if (value(QL1S("icon_theme")).toString().isEmpty())
    {
        qWarning() << QString::fromLatin1("Icon Theme not set. Fallbacking to Oxygen, if installed");
        const QString fallback(QLatin1String("oxygen"));

        const QDir dir(QLatin1String(UKUI_DATA_DIR) + QLatin1String("/icons"));
        if (dir.exists(fallback))
        {
            setValue(QL1S("icon_theme"), fallback);
            sync();
        }
        else
        {
            qWarning() << QString::fromLatin1("Fallback Icon Theme (Oxygen) not found");
        }
    }

    fileChanged();
}

GlobalSettings::~GlobalSettings()
{
    delete d_ptr;
}


/************************************************

 ************************************************/
void GlobalSettings::fileChanged()
{
    Q_D(GlobalSettings);
    sync();


    QString it = value(QL1S("icon_theme")).toString();
    if (d->mIconTheme != it)
    {
        emit iconThemeChanged();
    }

    QString rt = value(QL1S("theme")).toString();
    qlonglong themeUpdated = value(QL1S("__theme_updated__")).toLongLong();
    if ((d->mUKUiTheme != rt) || (d->mThemeUpdated != themeUpdated))
    {
        d->mUKUiTheme = rt;
        emit ukuiThemeChanged();
    }

    emit settingsChangedFromExternal();
    emit settingsChanged();
}

