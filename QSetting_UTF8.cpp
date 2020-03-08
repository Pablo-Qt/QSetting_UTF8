#include "QSetting_UTF8.h"
#include "QSetting_UTF8Private.h"

QSetting_UTF8::QSetting_UTF8(const QString& fileName, QObject* parent)
	: QSettings(fileName, QSettings::registerFormat("ini", QSetting_UTF8Private::ReadFunc, QSetting_UTF8Private::WriteFunc), parent)
{

}
