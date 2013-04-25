#ifndef XDG_TRASHINFO_H_
#define XDG_TRASHINFO_H_

#include <QStringList>

namespace XdgTrashInfo {

struct Info {
	QString deletionDate;
	QString relativePath;
	QString infoName;
};

Info getInfoFrom(const QString &trashCan, const QString &infoName);
QStringList getListInfo(const QString &trashCan);

} // namespace XdgTrashInfo

#endif /* XDG_TRASHINFO_H_ */
