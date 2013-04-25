#include "xdg/dirs.h"
#include <QProcessEnvironment>
#include <QDir>
#include <QStringList>
#include <QFileInfo>
#include <sys/stat.h>
#include <errno.h>

namespace XdgDirs {

namespace {
QString envValue(const QString &key, const QString &default_ = QString()) {
	QString value = QProcessEnvironment::systemEnvironment().value(key);

	if (!value.isEmpty())
		return value;
	else
		return default_;
}
}

QString configHome() {
	return envValue("XDG_CONFIG_HOME", QDir::homePath() + "/.config");
}

QString dataHome() {
	return envValue("XDG_DATA_HOME", QDir::homePath() + "/.local/share");
}

QString cacheHome() {
	return envValue("XDG_CACHE_HOME", QDir::homePath() + "/.cache");
}

void createHomes() {
	makeDirs(configHome());
	makeDirs(dataHome());
	makeDirs(cacheHome());
	makeDirs(trashHome());
}

QStringList configDirs() {
	return envValue("XDG_CONFIG_DIRS", "/etc/xdg").split(":");
}

QStringList dataDirs() {
	return envValue("XDG_DATA_DIRS", "/usr/local/share:/usr/share").split(":");
}


QString findExistingFile(const QString &relativePath, const QString &relatedHome, const QStringList &relatedDirs) {
	QString filePath = relatedHome + "/" + relativePath;
	if (QFile(filePath).exists())
		return filePath;
	foreach (const QString &dir, relatedDirs) {
		filePath = dir + "/" + relativePath;
		if (QFile(filePath).exists())
			return filePath;
	}

	return QString();
}

bool makeDirs(const QString &path) {
	int pos = -1;

	while (path != 0) {
		if (QFile(path.left(pos)).exists())
			break;
		pos = path.lastIndexOf('/', (pos < 0) ? pos : (pos - 1));
	}
	// pos contains the position of the last '/' where a directory does not exist
	// path.left(pos) doesn't include the '/'

	/*while (true) {
		QString dirMake = path.left((pos >= 0) ? pos : (pos - 1));
		if (::mkdir(QFile::encodeName(dirMake), 0700)) {
			if (errno != EEXIST)
				return false;
		}
		if (pos < 0)
			break;
		pos = path.indexOf('/', pos + 1);
	}*/

	while (pos >= 0) {
		pos = path.indexOf('/', pos + 1);

		QString dirMake = path.left((pos >= 0) ? pos : (pos - 1));
		if (::mkdir(QFile::encodeName(dirMake), 0700)) {
			if (errno != EEXIST)
				return false;
		}
	}

	QFileInfo info(path);
	return (info.exists() && info.isDir());
}

QString trashHome() {
	return dataHome() + "/Trash";
}

}
