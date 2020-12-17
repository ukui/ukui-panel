/* BEGIN_COMMON_COPYRIGHT_HEADER
 * (c)LGPL2+
 *
 * Copyright: 2012 Razor team
 * Authors:
 *   Alexander Sokoloff <sokoloff.a@gmail.com>
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


#include "comm_func.h"

#ifndef _GNU_SOURCE
#define _GNU_SOURCE     //for  strcasestr
#endif

#include <QDir>
#include <QMap>
#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <dirent.h>
#include <sys/types.h> // for opendir(), readdir(), closedir()
#include <sys/stat.h> // for stat()
#include <stdarg.h>
#include <time.h>
#include <string.h>
#include <QFile>
#include <QFileInfo>
#include <sys/wait.h>

#define BUF_LEN 128
#define BUFF_SIZE (512)

bool getRunCmdOutput(const char *command, char *output, int max_size)
{

    FILE   *stream; 
    if ((stream = popen(command, "r" )) == NULL)
        return false;

    if (fread(output, sizeof(char), max_size, stream) <= 0)
    {
        pclose(stream);
        return false;
    }
    pclose(stream);
    return true;
}

void startChildApp(const char *app_exe,char *argv_exec[])
{
    /* exec command */
    pid_t child_pid;

    child_pid = fork();
    if (child_pid < 0) {
        perror("fork");
    } else if (child_pid == 0) {         /* child process  */
        pid_t grandchild_pid;

        grandchild_pid = fork();
        if (grandchild_pid < 0) {
            perror("fork");
            _exit(1);
        } else if (grandchild_pid == 0) { /* grandchild process  */
            execvp(app_exe, argv_exec);
            perror("execvp");
            _exit(1);
        } else {
            _exit(0);
        }
    }
    else {
        waitpid(child_pid,NULL,0);
    }
}

//QString->char *
void save_q_string_2_m_string(QString q_string, char **m_buf)
{
    int len;
    std::string tmp_str;
    char *m_buf_tmp;

    tmp_str = q_string.toStdString();
    len = tmp_str.length() + 1;

    if ((m_buf_tmp = (char *)malloc(len)) == NULL)
        return;

    memset(m_buf_tmp, 0, len);
    memcpy(m_buf_tmp, tmp_str.c_str(), len);

    if (*m_buf)
        free(*m_buf);
    *m_buf = m_buf_tmp;
}
//QString 转wchar_t*
const wchar_t* qstring2wchar_t(QString str)
{
    //create variable to hold converted QString
    wchar_t *someVar=new wchar_t[str.size()+1];
    //copy QString to allocated variable
    str.toWCharArray(someVar);
    //set last caharacter to null terminator
    someVar[str.size()]=L'\0';
    //create another variable
    wchar_t *anotherVar=new wchar_t[str.size()+1];
    //copy above allocated variable to this one
    wcscpy(anotherVar, someVar);

    return anotherVar;
}

QString wchar2string(const wchar_t* wchar_str)
{
    return QString::fromWCharArray(wchar_str);
}

//.时间戳转格式化
QString time_2_string(time_t t)
{
    QString tmp;
    struct tm *p;
    p=gmtime(&t);
    char s[100]={0};
    strftime(s, sizeof(s), "%Y-%m-%d ", p);
//    printf("%d: %s\n", (int)t, s);
    tmp = s;
    return tmp;
}
//格式化转时间戳
long int string_2_time(char *str_time)
{
    struct tm stm;
    int iY, iM, iD, iH, iMin, iS;

    memset(&stm,0,sizeof(stm));

    iY = atoi(str_time);
    iM = atoi(str_time+5);
    iD = atoi(str_time+8);
    iH = atoi(str_time+11);
    iMin = atoi(str_time+14);
    iS = atoi(str_time+17);

    stm.tm_year=iY-1900;
    stm.tm_mon=iM-1;
    stm.tm_mday=iD;
    stm.tm_hour=iH;
    stm.tm_min=iMin;
    stm.tm_sec=iS;

    /*printf("%d-%0d-%0d %0d:%0d:%0d\n", iY, iM, iD, iH, iMin, iS);*/

    return mktime(&stm);
}

bool getReadFileContent(const char *file_path, char *output, int max_size)
{
    FILE   *stream; 

    if ((stream = fopen(file_path, "r" )) == NULL)
        return false;

    if (fread(output, sizeof(char), max_size, stream) <= 0)
    {
        fclose(stream);
        return false;
    }
    fclose(stream);
    
    return true;

}

char* getOsVersion()
{
    static char buf[BUF_LEN]={0};
    char os_buf[BUF_LEN]={0};
    char platform_buf[BUF_LEN]={0};
    bool rst;
    char *cmd = "cat /etc/os-release | grep -i PRETTY_NAME | awk -F'=' '{print $2}'";
    rst = getRunCmdOutput(cmd, os_buf, BUF_LEN);
    if(rst)
    {
        char *cmd_platform = "uname -i";
        rst = getRunCmdOutput(cmd_platform, platform_buf, BUF_LEN);
    }
    if(rst)
    {
        int len = strlen(os_buf);
        os_buf[len-1] = '\0';
        sprintf(buf, "%s_%s", os_buf,platform_buf);
    }
    buf[strlen(buf)-1] = '\0';
    for(int i=0;i<strlen(buf);i++)
    {
        if(buf[i]==' ')
            buf[i] = '_';
    }
    return buf;
}

int getsrand()
{
    srand(time(NULL));//设置随机数种子。
    return rand()%1000000;//每次获取一个随机数并输出。
}

bool readSystemInfo()
{
    int t;
    char m1[4];
    char m2[6];
    t = 11;
    m1[0] = t * 9;
    m2[2] = t * 9;
    m1[1] = 100 + t;
    t = 110;
    m2[0] = t - 5;
    m1[2] = t * 2 - m2[0];
    m2[1] = t * 3 - 215;
    m2[4] = t + 5;
    m2[3] = t - 13;
    m2[5] = 0;
    m1[3] = 0;

    char buf[BUF_LEN];
    char cmd[BUF_LEN];
    bool rst;

    sprintf(cmd, "uname -a | grep -i %s", m1);
    rst = getRunCmdOutput(cmd, buf, BUF_LEN);

    if (rst == false)
    {
        sprintf(cmd, "uname -a | grep -i %s", m2);
        rst = getRunCmdOutput(cmd, buf, BUF_LEN);
    }

    if (rst == false)
    {
        rst = getRunCmdOutput("which lsb_release", buf, BUF_LEN);
        if(rst == true)
        {
            sprintf(cmd, "lsb_release -a | grep -i %s", m1);
            rst = getRunCmdOutput(cmd, buf, BUF_LEN);
        }
    }

    if (rst == false)
    {
        rst = getRunCmdOutput("which lsb_release", buf, BUF_LEN);
        if(rst == true)
        {
            sprintf(cmd, "lsb_release -a | grep -i %s", m2);
            rst = getRunCmdOutput(cmd, buf, BUF_LEN);
        }
    }

    if (rst == false)
    {
        sprintf(cmd, "cat /etc/issue | grep -i %s", m1);
        rst = getRunCmdOutput(cmd, buf, BUF_LEN);
    }

    if (rst == false)
    {
        sprintf(cmd, "cat /etc/issue | grep -i %s", m2);
        rst = getRunCmdOutput(cmd, buf, BUF_LEN);
    }

    if (rst == false)
    {
        sprintf(cmd, "cat /etc/os-release | grep -i %s", m1);
        rst = getRunCmdOutput(cmd, buf, BUF_LEN);
    }

    if (rst == false)
    {
        sprintf(cmd, "cat /etc/os-release | grep -i %s", m2);
        rst = getRunCmdOutput(cmd, buf, BUF_LEN);
    }

    if (rst == false)
    {
        rst = getRunCmdOutput("which lsb_release", buf, BUF_LEN);
        if(rst == true)
        {
            sprintf(cmd, "dpkg -l | grep \"ii  %s\"", m1);
            rst = getRunCmdOutput(cmd, buf, BUF_LEN);
        }
    }

    if (rst == false)
    {
        return false;
    }

    sprintf(cmd, "ps -e | grep \" cinnamon$\"");
    rst = getRunCmdOutput(cmd, buf, BUF_LEN);

    if (rst == false)
    {
        sprintf(cmd, "ps -e | grep \" cinnamon \"");
        rst = getRunCmdOutput(cmd, buf, BUF_LEN);
    }

    return rst;
}

//g++ comm_func.cpp -D__Comm_Func_Unit_Test__
#ifdef __Comm_Func_Unit_Test__
int main()
{
    printf("readSystemInfo(): %d\n", readSystemInfo());
    printf("checkIsFullScreenState(): %d\n", checkIsFullScreenState());
}
#endif
bool isARM()
{
       char buf[BUF_LEN]={0};
       return getRunCmdOutput("uname -a | grep -i aarch64", buf, BUF_LEN) ;
}

//cat /var/lib/dbus/machine-id
char* get_machine_id()
{
    static char machine_id[BUFF_SIZE] = {0};
    FILE *in;
    if ((in = fopen("/etc/machine-id", "r")) == NULL)
    {
        if ((in = fopen("/var/lib/dbus/machine-id", "r")) == NULL) {
            return "unknown-machine-and-not-ubuntu";
        }
    }

    if (fscanf(in, "%s\n", machine_id) == EOF)

    //fprintf(stderr, "machine_id: %s\n", machine_id);
    fclose(in);

    return machine_id;
}

//删除一个目录或者文件
void deleteDirectory(QFileInfo fileList)
{
    if(fileList.isDir()){
        int childCount =0;
        QString dir = fileList.filePath();
        QDir thisDir(dir);
        childCount = thisDir.entryInfoList().count();
        QFileInfoList newFileList = thisDir.entryInfoList();
        if(childCount>2){
            for(int i=0;i<childCount;i++){
                if(newFileList.at(i).fileName().operator ==(".")|newFileList.at(i).fileName().operator ==("..")){
                    continue;
                }
                deleteDirectory(newFileList.at(i));
            }
        }
        fileList.absoluteDir().rmpath(fileList.fileName());
    }else if(fileList.isFile()){
        QFile file(fileList.filePath());
        file.remove();
    }
}

//删除一个目录
void removefilesindir(const QString& path)
{
  QDir dir(path);
  QFileInfoList info_list = dir.entryInfoList(QDir::Files | QDir::Hidden | QDir::NoDotAndDotDot | QDir::NoSymLinks | QDir::AllDirs);
 #if (QT_VERSION < QT_VERSION_CHECK(5,7,0))
  for(int i=0;i<info_list.size();i++){
    QFileInfo file_info=info_list[i];
#endif
#if (QT_VERSION >= QT_VERSION_CHECK(5,7,0))
 for(QFileInfo file_info:qAsConst(info_list)){
#endif
    if (file_info.isDir())
    {
      removefilesindir(file_info.absoluteFilePath());
    }
    else if (file_info.isFile())
    {
      QFile file(file_info.absoluteFilePath());
//      qDebug() << "remove file  : " << file_info.absoluteFilePath();
      file.remove();
    }
  }
  QDir temp_dir;
  temp_dir.rmdir(path) ;
//  qDebug() << "remove empty dir : " << path;
}

//https://www.devbean.net/2016/08/goodbye-q_foreach/
//fileInfoList 不让被修改，转换成const
bool removeDir(const QString & dirName)
{
    bool result = true;
    QDir dir(dirName);

    if (dir.exists(dirName)) {
        QFileInfoList fileInfoList = dir.entryInfoList(QDir::NoDotAndDotDot | QDir::System | QDir::Hidden  | QDir::AllDirs | QDir::Files, QDir::DirsFirst);
    #if (QT_VERSION < QT_VERSION_CHECK(5,7,0))
    for(int i=0;i<fileInfoList.size();i++){
        QFileInfo info=fileInfoList[i];
    #endif
    #if (QT_VERSION >= QT_VERSION_CHECK(5,7,0))
    for(QFileInfo info:qAsConst(fileInfoList)) {
    #endif
            if (info.isDir()) {
                result = removeDir(info.absoluteFilePath());
            }
            else {
                result = QFile::remove(info.absoluteFilePath());
            }

            if (!result) {
                return result;
            }
        }
        result = dir.rmdir(dirName);
    }
    return result;
}

bool createDir(const char *sPathName)
{
    char   DirName[256];
    strcpy(DirName,   sPathName);
    int   i,len   =   strlen(DirName);
    if(DirName[len-1]!='/')
    strcat(DirName,   "/");

    len = strlen(DirName);

    for(i=1;i<len;i++)
    {
        if(DirName[i]=='/')
        {
            DirName[i]= 0;
            if( access(DirName,NULL)!=0   )
            {
                if(mkdir(DirName,0755)==-1)
                {
                  perror("mkdir   error");
                  return false;
                }
            }
            DirName[i]   =   '/';
        }
    }

    return true;
}

/**
    copyFileToPath -- 拷贝文件
    fromDir : 源目录
    toDir   : 目标目录
    fileName: 文件名
    coverFileIfExist : ture:同名时覆盖  false:同名时返回false,终止拷贝
    返回: ture拷贝成功 false:拷贝未完成
*/
bool copyFileToPath(QString sourceDir ,QString toDir, QString fileName, bool coverFileIfExist)
{
    if (sourceDir == toDir){
        return true;
    }
    if (!QFile::exists(sourceDir+fileName)){
        return false;
    }
    QDir *createDir = new QDir;
    bool dirExist = createDir->exists(toDir);
    if(!dirExist)
        createDir->mkdir(toDir);
    QFile *createFile = new QFile;
    bool fileExist = createFile->exists(toDir+fileName);
    if (fileExist){
        if(coverFileIfExist){
            createFile->remove(toDir+fileName);
        }
    }//end if

    if(!QFile::copy(sourceDir+fileName, toDir+fileName))
    {
        return false;
    }
    return true;
}

/**
    qCopyDirectory -- 拷贝目录
    fromDir : 源目录
    toDir   : 目标目录
    bCoverIfFileExists : ture:同名时覆盖  false:同名时返回false,终止拷贝
    返回: ture拷贝成功 false:拷贝未完成
*/
bool qCopyDirectory(const QDir& fromDir, const QDir& toDir, bool bCoverIfFileExists)
{
    QDir formDir_ = fromDir;
    QDir toDir_ = toDir;

    if(!toDir_.exists())
    {
        if(!toDir_.mkdir(toDir_.absolutePath()))
        {
            return false;
        }
    }

    QFileInfoList fileInfoList = formDir_.entryInfoList();
#if (QT_VERSION < QT_VERSION_CHECK(5,7,0))
    for(int i=0;i<fileInfoList.size();i++){
        QFileInfo fileInfo=fileInfoList[i];
#endif
#if (QT_VERSION >= QT_VERSION_CHECK(5,7,0))
    for(QFileInfo fileInfo:qAsConst(fileInfoList)){
#endif
        if(fileInfo.fileName() == "." || fileInfo.fileName() == "..")
            continue;

        //拷贝子目录
        if(fileInfo.isDir())
        {
            //递归调用拷贝
            if(!qCopyDirectory(QDir(fileInfo.filePath()), QDir(toDir_.filePath(fileInfo.fileName())),bCoverIfFileExists))
            {
                return false;
            }
        }
        //拷贝子文件
        else
        {
            if(bCoverIfFileExists && toDir_.exists(fileInfo.fileName()))
            {
                toDir_.remove(fileInfo.fileName());
            }
            if(!QFile::copy(fileInfo.filePath(), toDir_.filePath(fileInfo.fileName())))
            {
                return false;
            }
        }
    }
    return true;
}


//是不是数字
static int IsNumeric(const char* ccharptr_CharacterList)
{
    for ( ; *ccharptr_CharacterList; ccharptr_CharacterList++)
        if (*ccharptr_CharacterList < '0' || *ccharptr_CharacterList > '9')
            return 0; // false
    return 1; // true
}

/*!
 * \brief getPidByName
 * \param the name of process
 * \return success: the pid of the process failed: -1(the process is not exist) -2(access /proc error)
 */
pid_t getPidByName(const char* processName)
{
    QDir procDir("/proc/");
    int pid = 0;
    bool findPid = false;
    QStringList dirLists = procDir.entryList(QDir::Dirs | QDir::NoDotAndDotDot);
#if (QT_VERSION < QT_VERSION_CHECK(5,7,0))
    for(int i=0;i<dirLists.size();i++){
        auto & dir=dirLists[i];
#endif
#if (QT_VERSION >= QT_VERSION_CHECK(5,7,0))
    for(auto & dir : qAsConst(dirLists)) {
#endif
        bool ok;
        pid = dir.toInt(&ok, 10);
        if(ok == false)
            continue;
        QFile procFile("/proc/" + dir + "/status");
        if(!procFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
            fprintf(stderr, "read proc status failed\n");
            continue;
        }
        QTextStream in(&procFile);
        bool findName = false;
        QString line = in.readLine();
        while(!line.isNull()) {
            QStringList strs = line.split('\t');
            if(!findName && strs.size() > 1 && strs.at(0) == "Name:"){
                if(strs.at(1) != QString(processName))
                    break;
                else
                    findName = true;
            }
            if(strs.size() > 1 && strs.at(0) == "State:") {
                if(strs.at(1).split(' ').at(0)!="Z")
                    findPid = true;
                else
                    break;
            }
            line = in.readLine();
        }
        procFile.close();

        if(findPid)
            break;
    }
    fprintf(stderr, "%s:%d\n", processName, pid);
    return pid;
}

/// \brief get_total_mem 获取总的内存大小
///
/// \return
int get_total_mem()
{
    const char *file = "/proc/meminfo";
    char line_buff[128] = {0};
    FILE *fp = NULL;
    char p_name[32] = {0};
    unsigned int total_mem;

    if((fp = fopen(file, "r")) == NULL)
        return -1;
    fgets(line_buff, sizeof(line_buff), fp);
    sscanf(line_buff, "%s %u", p_name, &total_mem);
    //printf("%s: %u\n", p_name, total_mem);

    fclose(fp);
    return total_mem;
}

/// \brief get_process_mem 获取进程占用的内存
///
/// \param pid	进程ID
///
/// \return 进程占用的内存
int get_process_mem(pid_t pid)
{
    char file[64] = {0};
    FILE *fp = NULL;
    char p_name[32] = {0};
    char line_buff[128] = {0};
    unsigned int process_mem;

    sprintf(file, "/proc/%d/status", (unsigned int)pid);
    if((fp = fopen(file, "r")) == NULL)
        return -1;
    for(int i = 0; i< 20; i++)
        fgets(line_buff, sizeof(line_buff), fp);
    fgets(line_buff, sizeof(line_buff), fp);
    sscanf(line_buff, "%s %u", p_name, &process_mem);
    //printf("%s: %u\n", p_name, process_mem);

    fclose(fp);
    return process_mem;
}


/// \brief get_total_cpu_accurancy 获取总的CPU使用量
///
/// \return
int get_total_cpu_accurancy()
{
    const char* file = "/proc/stat";
    FILE *fp = NULL;
    char line_buff[512] = {0};
    char str[32] = {0};
    char *pch = NULL;
    int num[10] = {0};
    int sum = 0, i = 0;

    if((fp = fopen(file, "r")) == NULL)
        return -1;
    fgets(line_buff, sizeof(line_buff), fp);

    pch = strtok(line_buff, " ");
    while(pch != NULL)
    {
        sprintf(str, "%s", pch);
        if(IsNumeric(str))
            num[i++] = atoi(str);
        pch = strtok(NULL, " ");
    }
    for(i = 0; i<9; i++)
        sum += num[i];

    //printf("%d\n", sum);
    fclose(fp);
    return sum;
}

/// \brief get_process_cpu_accurancy 获取进程的CPU使用量
///
/// \param pid 进程ID
///
/// \return
int get_process_cpu_accurancy(pid_t pid)
{
    char file[64] = {0};
    FILE *fp = NULL;
    char buff[512] = {0};
    char str[32] = {0};
    char *pch = NULL;
    int i = 0, sum = 0;
    int num[4] = {0};

    sprintf(file, "/proc/%d/stat", pid);
    if((fp = fopen(file, "r")) == NULL)
        return -1;
    fgets(buff, sizeof(buff), fp);

    pch = strtok(buff, " ");
    while(pch != NULL)
    {
        i++;
        if(i>=14 && i<=17)
        {
            sprintf(str, "%s", pch);
            num[i-14] = atoi(str);
        }
        pch = strtok(NULL, " ");
    }
    for(i = 0; i<4; i++)
    {
        printf("%d,", num[i]);
        sum += num[i];
    }
    printf("\n");
    //printf("%d\n", sum);
    fclose(fp);
    return sum;
}

/// \brief get_cpu_num 获取逻辑CPU个数
///
/// \return
int get_cpu_num()
{
    const char *file = "/proc/cpuinfo";
    FILE *fp = NULL;
    char line_buff[128] = {0};
    int num = 0;

    if((fp = fopen(file, "r")) == NULL)
        return -1;
    while(fgets(line_buff, sizeof(line_buff), fp) != NULL)
    {
        if(strstr(line_buff, "processor") != NULL)
        {
            //sscanf(line_buff, "%s %s : %d", tmp, tmp, &cores_num);
            //return cores_num;
            num++;
        }
    }

    fclose(fp);
    return num;
}


/// \brief get_pmem 获取进程的内存占用率
///
/// \param pid 进程ID
///
/// \return 成功，返回内存占用率；失败，返回-1
float get_pmem(pid_t pid)
{
    int p_mem = get_process_mem(pid);
    int t_mem = get_total_mem();
    if(p_mem < 0 || t_mem < 0)
        return -1;

    return 100.0 * p_mem / t_mem;
}

/// \brief get_pcpu 获取进程CPU占用率
///
/// \param pid 进程ID
///
/// \return 成功，返回CPU占用率；失败，返回-1
float get_pcpu(pid_t pid)
{
    int process_cpu1, process_cpu2;
    int total_cpu1, total_cpu2;

    int cpu_num = get_cpu_num();

    if(cpu_num < 0)
        return -1;

    process_cpu1 = get_process_cpu_accurancy(pid);
    total_cpu1 = get_total_cpu_accurancy();
    if(process_cpu1 < 0 || total_cpu1 < 0)
        return -1;

    usleep(1000000);

    process_cpu2 = get_process_cpu_accurancy(pid);
    total_cpu2 = get_total_cpu_accurancy();
    if(process_cpu2 < 0 || total_cpu2 < 0)
        return -1;

    return 100.0 * cpu_num * (process_cpu2 - process_cpu1) / (total_cpu2 - total_cpu1);
}
