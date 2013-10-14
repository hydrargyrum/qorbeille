#include "xdg/trash.h"
#include <QUuid>
#include <QDateTime>
#include <QFile>
#include <QDir>
#include <QUrl>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include "xdg/dirs.h"

/* http://www.ramendik.ru/docs/trashspec.html (0.7) */

namespace Qorbeille {

// TODO handle trashing a file within trash

/* "Obtain" means find and create. If it doesn't exist, it is not "obtained".
"Get" means only finding, it may not exist. */

/* Create a file atomically (fail if it already exists) */
bool createUnexistingFile(const QString &path) {
	int fd = ::open(QFile::encodeName(path), O_WRONLY | O_EXCL | O_CREAT, 0600);
	if (fd < 0)
		return false;
	return (::close(fd) == 0);
}

/* Get path to "trashinfo" file */
QString getInfoFile(const QString &trashCan, const QString &name) {
	return QString("%1/info/%2.trashinfo").arg(trashCan, name);
}

/* Obtain a unique "trashinfo" name */
QString obtainUniqueInfoFile(const QString &trashCan, const QString &originalPath) {
	QString name = QDir(originalPath).dirName();
	if (createUnexistingFile(getInfoFile(trashCan, name)))
		return name;

	QDateTime now = QDateTime::currentDateTime();
	name = name + "_" + now.toString("dd-MM-yy-zzz");
	if (createUnexistingFile(getInfoFile(trashCan, name)))
		return name;

	name = QUuid::createUuid().toString();
	if (createUnexistingFile(getInfoFile(trashCan, name)))
		return name;

	return QString();
}

/* Write required contents of a "trashinfo" file */
bool fillInfoFile(const QString &infoFile, const QString &deletedPath) {
	QFile info(infoFile);
	if (!info.open(QIODevice::ReadWrite))
		return false;

	QString date = QDateTime::currentDateTime().toString(Qt::ISODate);
	QUrl escaper;
	escaper.setPath(deletedPath);
	QString data = "[Trash Info]\nDeletionDate=%1\nPath=%2\n";
	data = data.arg(date, QString(escaper.encodedPath()));

	if (info.write(data.toLatin1()) < 0)
		return false;
	if (!info.flush())
		return false;
	return true;
}

dev_t getDeviceOf(const QString &path) {
	struct stat info;
	if (::stat(QFile::encodeName(path), &info) != 0)
		return 0;
	return info.st_dev;
}

QString findMountedRoot(const QString &originalPath) {
	dev_t originalDev = getDeviceOf(originalPath);
	if (originalDev == 0)
		return QString();

	QString upperPath = originalPath, currentPath = originalPath;

	while (true) {
		upperPath = QDir::cleanPath(upperPath + "/..");

		if (upperPath == "/")
			return "/";
		dev_t pathDev = getDeviceOf(upperPath);
		if (pathDev != originalDev)
			return currentPath;

		currentPath = upperPath;
	}

}

bool isTopDirCanValid(const QString &topDir) {
	// TODO spec says: report to admin? syslog?
	struct stat info;
	if (::stat(QFile::encodeName(topDir), &info) != 0)
		return false;
	if (!(S_ISVTX & info.st_mode) || (S_IFLNK & info.st_mode))
		return false;
	return true;
}

QString obtainTopDirCan(const QString &originalPath) {
	QString topDir = findMountedRoot(originalPath);
	if (topDir.isEmpty())
		return QString();

	QString trashCan = topDir + "/.Trash";
	if (isTopDirCanValid(trashCan)) {
		trashCan = trashCan + "/" + QString::number(::getuid());
		if (XdgDirs::makeDirs(trashCan))
			return trashCan;
	}

	trashCan = topDir + "/.Trash-" + QString::number(::getuid());
	if (XdgDirs::makeDirs(trashCan))
		return trashCan;

	return QString();
}

/** \brief Find or create a suitable trash can
The trash can directory is also created. If no suitable trash can is found,
or the directories can't be created, empty string is returned.
\param originalPath the path we ultimately want to trash
\return the path of trash can to which \a originalPath should be trashed.
*/
QString obtainTrashCan(const QString &originalPath) {
	QString trashCan;

	QString homeTrash = XdgDirs::trashHome();
	if (!QFile(homeTrash).exists())
		if (XdgDirs::makeDirs(homeTrash))
			return QString(); // that's right, we don't go further
		// TODO: loop every dir up and try getDeviceOf? where to stop?

	if (getDeviceOf(originalPath) == getDeviceOf(homeTrash))
		trashCan = homeTrash;
	else
		trashCan = obtainTopDirCan(originalPath);

	if (trashCan.isEmpty())
		return QString();

	if (!XdgDirs::makeDirs(trashCan + "/files") ||
			!XdgDirs::makeDirs(trashCan + "/info"))
		return QString();

	return trashCan;
}

QString pathSinceTrash(const QString &trashCan, const QString &originalPath) {
	QString acceptableRoot = QDir::cleanPath(QDir(trashCan).absoluteFilePath(".."));
	QString originalAbsolute = QDir(originalPath).absolutePath();
	if (!originalAbsolute.startsWith(acceptableRoot + '/'))
		return originalAbsolute;
	else
		return originalAbsolute.mid(acceptableRoot.size() + 1);
}

// QDir::cleanPath
/** \brief trash a file or directory
\return true if the path was successfully trashed
*/
bool trashFile(const QString &originalPath, QString *infoName) {
	QString trashCan = obtainTrashCan(originalPath);
	if (trashCan.isEmpty())
		return false;

	QString uniqueId = obtainUniqueInfoFile(trashCan, originalPath);
	if (uniqueId.isEmpty())
		return false;

	if (!fillInfoFile(getInfoFile(trashCan, uniqueId), pathSinceTrash(trashCan, originalPath)))
		return false;

	bool ok = (::rename(QFile::encodeName(originalPath), QFile::encodeName(trashCan + "/files/" + uniqueId)) == 0);
	if (!ok)
		QFile::remove(getInfoFile(trashCan, uniqueId));
	else if (infoName)
		*infoName = uniqueId;
	return ok;
}

bool restoreFile(const QString &trashCan, const QString &infoName, const QString &destination) {
	QString infoFile = getInfoFile(trashCan, infoName);
	QString dataFile = QString("%1/files/%2").arg(trashCan, infoName);
	bool ok = (::rename(QFile::encodeName(dataFile), QFile::encodeName(destination)) == 0);
	if (ok)
		QFile::remove(infoFile);
	return ok;
}

} // namespace Qorbeille
