#include "traydynamicgsetting.h"
/* qt会将glib里的signals成员识别为宏，所以取消该宏
 * 后面如果用到signals时，使用Q_SIGNALS代替即可
 **/
#ifdef signals
#undef signals
#endif
extern "C" {
#include <glib.h>
#include <gio/gio.h>
#include <dconf/dconf.h>
}

#define MAX_CUSTOM_SHORTCUTS 100
#define KEYBINDINGS_CUSTOM_SCHEMA "org.ukui.panel.tray"
#define KEYBINDINGS_CUSTOM_DIR "/org/ukui/tray/keybindings/"

#define ACTION_KEY "action"
#define RECORD_KEY "record"
#define BINDING_KEY "binding"
#define NAME_KEY "name"

QString findFreePath(){
    int i = 0;
    char * dir;
    bool found;
    QList<char *> existsdirs;

    existsdirs = listExistsPath();

    for (; i < MAX_CUSTOM_SHORTCUTS; i++){
        found = true;
        dir = QString("custom%1/").arg(i).toLatin1().data();
        for (int j = 0; j < existsdirs.count(); j++)
            if (!g_strcmp0(dir, existsdirs.at(j))){
                found = false;
                break;
            }
        if (found)
            break;
    }

    if (i == MAX_CUSTOM_SHORTCUTS){
        qDebug() << "Keyboard Shortcuts" << "Too many custom shortcuts";
        return "";
    }
    return QString("%1%2").arg(KEYBINDINGS_CUSTOM_DIR).arg(QString(dir));
}

QList<char *> listExistsPath(){
    char ** childs;
    int len;

    DConfClient * client = dconf_client_new();
    childs = dconf_client_list (client, KEYBINDINGS_CUSTOM_DIR, &len);
    g_object_unref (client);

    QList<char *> vals;

    for (int i = 0; childs[i] != NULL; i++){
        if (dconf_is_rel_dir (childs[i], NULL)){
            gchar *val = g_strdup (childs[i]);
            vals.append(val);
        }
    }
    g_strfreev (childs);
    return vals;
}

void freezeApp()
{
    QList<char *> existsPath = listExistsPath();
    for (char * path : existsPath){
        QString p =KEYBINDINGS_CUSTOM_DIR;
        std::string str = p.toStdString();
        const int len = str.length();
        char * prepath = new char[len+1];
        strcpy(prepath,str.c_str());
        char * allpath = strcat(prepath, path);

        const QByteArray ba(KEYBINDINGS_CUSTOM_SCHEMA);
        const QByteArray bba(allpath);

        QGSettings *settings;
        const QByteArray id(KEYBINDINGS_CUSTOM_SCHEMA);
        if(QGSettings::isSchemaInstalled(id)) {
            settings= new QGSettings(ba, bba);
            settings->set(ACTION_KEY,"freeze");
        }
    }
}

void freezeTrayApp(int winId)
{
    QList<char *> existsPath = listExistsPath();
    int bingdingStr;

    for (char * path : existsPath)
    {
        QString p =KEYBINDINGS_CUSTOM_DIR;
        std::string str = p.toStdString();
        const int len = str.length();
        char * prepath = new char[len+1];
        strcpy(prepath,str.c_str());
        char * allpath = strcat(prepath, path);

        const QByteArray ba(KEYBINDINGS_CUSTOM_SCHEMA);
        const QByteArray bba(allpath);

        QGSettings *settings = NULL;
        const QByteArray id(KEYBINDINGS_CUSTOM_SCHEMA);
        if(QGSettings::isSchemaInstalled(id))
        {
            if(bba.isEmpty()){
                continue;
            }
            settings= new QGSettings(ba, bba);
            if(settings){
                if(settings->keys().contains(BINDING_KEY)){
                    bingdingStr=settings->get(BINDING_KEY).toInt();
                }
            }

            if(winId==bingdingStr){
                settings->set(ACTION_KEY,"freeze");
            }
        }

        if(settings){
            settings->deleteLater();
        }
    }
}
