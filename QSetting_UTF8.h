/*
 * 出處： https://github.com/pablo-new17/QSetting_UTF8
 * 說明： 讓 INI 檔 可以支援 UTF-8 字元，也就是說，直接以人類可以悅讀的型態呈現。
 * 版權： MIT License
 */

#ifndef QSETTING_UTF8_H
#define QSETTING_UTF8_H

#include <QObject>
#include <QSettings>

#ifdef QSETTING_UTF8_LIBRARY
#include "QSetting_UTF8_global.h"

class QSETTING_UTF8_EXPORT QSetting_UTF8 : public QSettings
#else

class QSetting_UTF8 : public QSettings
#endif
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
