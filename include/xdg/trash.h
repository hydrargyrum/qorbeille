#ifndef XDG_TRASH_H
#define XDG_TRASH_H

#include <QString>

namespace Qorbeille {

QString findMountedRoot(const QString &originalPath);

QString obtainTopDirCan(const QString &originalPath);

QString obtainTrashCan(const QString &originalPath);

bool trashFile(const QString &originalPath, QString *infoName = 0);

bool restoreFile(const QString &trashCan, const QString &infoName, const QString &destination);

}


#endif // XDG_TRASH_H
