#ifndef QSETTING_UTF8_H
#define QSETTING_UTF8_H

#include <QObject>
#include <QSettings>

#include "QSetting_UTF8_global.h"

class QSETTING_UTF8_EXPORT QSetting_UTF8 : public QSettings
{
	Q_OBJECT
	Q_CLASSINFO("作者", "Pablo Lee")
	Q_CLASSINFO("描述", "支援中文字的 QSettings")
	Q_CLASSINFO("版本號碼", "1.0.0.20200308")

public:
	QSetting_UTF8(const QString &fileName, QObject *parent = nullptr);

private:
	Q_DISABLE_COPY(QSetting_UTF8)

};

#endif // QSETTING_UTF8_H
