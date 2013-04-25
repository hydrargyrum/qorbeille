#ifndef TST_TRASH_H
#define TST_TRASH_H

#include <QObject>

class TrashTest : public QObject {
	Q_OBJECT

	QString trashPath;

private slots:
	void init();
	void cleanup();

	void basicTrash();
	void restore();
	void cans();
};

#endif // TST_TRASH_H
