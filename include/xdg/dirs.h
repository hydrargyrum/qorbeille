#ifndef XDG_DIRS_H
#define XDG_DIRS_H

#include <QString>
#include <QStringList>

/** http://standards.freedesktop.org/basedir-spec/basedir-spec-latest.html (0.7) */
namespace XdgDirs {

QString configHome();
QStringList configDirs();

QString dataHome();
QStringList dataDirs();

QString cacheHome();

QString trashHome();

QString findExistingFile(const QString &relativePath, const QString &relatedHome, const QStringList &relatedDirs);
bool makeDirs(const QString &path);
void createHomes();

}

#endif // XDG_DIRS_H
