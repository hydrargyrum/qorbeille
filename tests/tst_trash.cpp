#include "tst_trash.h"
#include <QtTest>
#include <QTemporaryFile>
#include "xdg/dirs.h"
#include "xdg/trash.h"


static QByteArray uuidPath() {
	return QFile::encodeName(QDir::tempPath() + "/" + QUuid::createUuid().toString());
}

void TrashTest::init() {
	QByteArray dataPath = uuidPath();

	setenv("XDG_DATA_HOME", dataPath.constData(), 1);
	QVERIFY(!QFile(XdgDirs::dataHome()).exists());
	QVERIFY(!QFile(XdgDirs::trashHome()).exists());

	XdgDirs::createHomes();

	QVERIFY(QFile(XdgDirs::trashHome()).exists());
}

void TrashTest::cleanup() {
	// TODO better cleanup
}

void TrashTest::basicTrash() {
	// create
	QTemporaryFile f;
	QVERIFY(f.open());
	QString originalPath = f.fileName();

	// trash
	QVERIFY(Qorbeille::trashFile(originalPath));
	QVERIFY(!QFile(originalPath).exists());
}

void TrashTest::restore() {
	// create file
	QTemporaryFile f;
	QVERIFY(f.open());
	QVERIFY(f.write("foobar"));
	QVERIFY(f.flush());
	QString originalPath = f.fileName();

	// trash file
	QString infoName;
	QVERIFY(Qorbeille::trashFile(originalPath, &infoName));
	QVERIFY(!QFile(originalPath).exists());
	QVERIFY(!infoName.isEmpty());

	QString can = XdgDirs::trashHome();

	// restore
	QVERIFY(Qorbeille::restoreFile(can, infoName, originalPath));

	// check restored
	QVERIFY(QFile(originalPath).exists());
	QFile restored(originalPath);
	QVERIFY(restored.open(QIODevice::ReadOnly));
	QCOMPARE(restored.readAll(), QByteArray("foobar"));
}

void TrashTest::cans() {
	// TODO test
	//XdgTrash::obtainTrashCan()
}
