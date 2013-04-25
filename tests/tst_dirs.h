#ifndef TST_DIRS_H
#define TST_DIRS_H

#include <QObject>

class DirsTest : public QObject {
	Q_OBJECT

private slots:
	void withoutEnv();
	void fromEnv();
	void findFile();
	void createDirs();
};


#endif // TST_DIRS_H
