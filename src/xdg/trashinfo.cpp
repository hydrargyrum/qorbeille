#include "xdg/trashinfo.h"
#include <QSettings>
#include <QDir>
#include "xdg/trash.h"

namespace Qorbeille {
QString getInfoFile(const QString&, const QString&); // private impl in trash.cpp
}

namespace QorbeilleInfo {


Info getInfoFrom(const QString &trashCan, const QString &infoName) {
	QSettings iniData(Qorbeille::getInfoFile(trashCan, infoName), QSettings::IniFormat);
	if (iniData.status() != QSettings::NoError)
		return Info();
	Info info;
	info.infoName = infoName;
	info.deletionDate = iniData.value("Trash Info/DeletionDate").toString();
	info.relativePath = iniData.value("Trash Info/Path").toString();
	return info;
}

QStringList getListInfo(const QString &trashCan) {
	return QDir(trashCan).entryList(QStringList("*.trashinfo"));
}

} // namespace QorbeilleInfo
