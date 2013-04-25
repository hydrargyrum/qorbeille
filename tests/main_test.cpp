
#include <QTest>
#include "tst_dirs.h"
#include "tst_trash.h"

int main(int n, char **a) {
	int result = 0;

	{
		DirsTest t;
		result |= QTest::qExec(&t);
	}

	{
		TrashTest t;
		result |= QTest::qExec(&t);
	}

	return result;
}
