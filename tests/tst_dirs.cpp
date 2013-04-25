#include "tst_dirs.h"
#include <QDir>
#include <QTest>
#include <QTemporaryFile>
#include <QUuid>
#include "xdg/dirs.h"

void DirsTest::withoutEnv() {
	setenv("XDG_CONFIG_HOME", "", 1);
	setenv("XDG_CONFIG_DIRS", "", 1);
	QCOMPARE(XdgDirs::configHome(), QDir::homePath() + "/.config");
	QCOMPARE(XdgDirs::configDirs(), QStringList() << "/etc/xdg");

	setenv("XDG_CACHE_HOME", "", 1);
	QCOMPARE(XdgDirs::cacheHome(), QDir::homePath() + "/.cache");

	setenv("XDG_DATA_HOME", "", 1);
	setenv("XDG_DATA_DIRS", "", 1);
	QCOMPARE(XdgDirs::dataHome(), QDir::homePath() + "/.local/share");
	QCOMPARE(XdgDirs::dataDirs(), QStringList() << "/usr/local/share" << "/usr/share");
}

void DirsTest::fromEnv() {
	setenv("XDG_CONFIG_HOME", "/tmp/x1", 1);
	setenv("XDG_CONFIG_DIRS", "/tmp/x2:/tmp/x3", 1);
	QCOMPARE(XdgDirs::configHome(), QString("/tmp/x1"));
	QCOMPARE(XdgDirs::configDirs(), QStringList() << "/tmp/x2" << "/tmp/x3");

	setenv("XDG_CACHE_HOME", "/tmp/y", 1);
	QCOMPARE(XdgDirs::cacheHome(), QString("/tmp/y"));

	setenv("XDG_DATA_HOME", "/tmp/z1", 1);
	setenv("XDG_DATA_DIRS", "/tmp/z2:/tmp/z3", 1);
	QCOMPARE(XdgDirs::dataHome(), QString("/tmp/z1"));
	QCOMPARE(XdgDirs::dataDirs(), QStringList() << "/tmp/z2" << "/tmp/z3");
}

void DirsTest::findFile() {
	QTemporaryFile f;
	QVERIFY(f.open());

	QString path = f.fileName();
	QString dir = QFileInfo(path).path();
	QString base = QFileInfo(path).fileName();

	// file is not in the standard paths
	setenv("XDG_CONFIG_HOME", "", 1);
	setenv("XDG_CONFIG_DIRS", "", 1);
	QCOMPARE(XdgDirs::findExistingFile(base, XdgDirs::configHome(), XdgDirs::configDirs()), QString());

	// file is in the home path
	setenv("XDG_CONFIG_HOME", QFile::encodeName(dir).constData(), 1);
	setenv("XDG_CONFIG_DIRS", "", 1);
	QCOMPARE(XdgDirs::findExistingFile(base, XdgDirs::configHome(), XdgDirs::configDirs()), path);

	// file is in the system paths
	setenv("XDG_CONFIG_HOME", "", 1);
	setenv("XDG_CONFIG_DIRS", QFile::encodeName(dir).constData(), 1);
	QCOMPARE(XdgDirs::findExistingFile(base, XdgDirs::configHome(), XdgDirs::configDirs()), path);
}

static QByteArray uuidPath() {
	return QFile::encodeName(QDir::tempPath() + "/" + QUuid::createUuid().toString());
}

void DirsTest::createDirs() {
	// use temp paths
	setenv("XDG_CONFIG_HOME", uuidPath().constData(), 1);
	setenv("XDG_CACHE_HOME", uuidPath().constData(), 1);
	setenv("XDG_DATA_HOME", uuidPath().constData(), 1);

	// ensure they don't exist yet
	QVERIFY(!QFile(XdgDirs::configHome()).exists());
	QVERIFY(!QFile(XdgDirs::cacheHome()).exists());
	QVERIFY(!QFile(XdgDirs::dataHome()).exists());
	QVERIFY(!QFile(XdgDirs::trashHome()).exists());

	// create
	XdgDirs::createHomes();

	// verify they were created
	QVERIFY(QFile(XdgDirs::configHome()).exists());
	QVERIFY(QFile(XdgDirs::cacheHome()).exists());
	QVERIFY(QFile(XdgDirs::dataHome()).exists());
	QVERIFY(QFile(XdgDirs::trashHome()).exists());

	// cleanup test
	QDir().rmdir(XdgDirs::configHome());
	QDir().rmdir(XdgDirs::cacheHome());
	QDir().rmdir(XdgDirs::trashHome());
	QDir().rmdir(XdgDirs::dataHome());
}

// TODO test makedirs
// TODO test with phoney paths with "//" etc.
