/*
 * Copyright (C) 2019 Tianjin KYLIN Information Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, see <http://www.gnu.org/licenses/&gt;.
 *
 */

#ifndef _COMM_FUNC_H_
#define _COMM_FUNC_H_
//放公共功能函数
#include <QString>
#include <QStringList>
#include <QFileInfo>
#include <QTextStream>
#include <time.h>
#include <string>

bool getRunCmdOutput(const char *command, char *output, int max_size);
bool getReadFileContent(const char *file_path, char *output, int max_size);
bool readSystemInfo();
bool isARM();
char* get_machine_id();
char* getOsVersion();
void startChildApp(const char *app_exe,char *argv_exec[]);
const wchar_t* qstring2wchar_t(QString str);
QString wchar2string(const wchar_t* wchar_str);
void save_q_string_2_m_string(QString q_string, char **m_buf);
QString time_2_string(time_t t);
long int string_2_time(char *str_time);
void deleteDirectory(QFileInfo fileList);
void removefilesindir(const QString& path);
bool removeDir(const QString & dirName);
bool createDir(const char *sPathName);
bool copyFileToPath(QString sourceDir ,QString toDir, QString copyFileToPath, bool coverFileIfExist);
bool qCopyDirectory(const QDir& fromDir, const QDir& toDir, bool bCoverIfFileExists);
char *getLoginCookie(char *userid);
int getsrand();
int getPidByName(const char* pName);
float get_pmem(pid_t pid);
float get_pcpu(pid_t pid);

#endif
