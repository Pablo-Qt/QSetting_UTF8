#include <QTextStream>
#include <QStack>

#include "QSetting_UTF8Private.h"

bool QSetting_UTF8Private::ReadFunc(QIODevice& device, QSettings::SettingsMap& map)
{
	QString currentSection;
	QTextStream stream(&device);
	stream.setCodec("UTF-8");
	QString data;
	bool ok = true;

	while (!stream.atEnd())
	{
		data = stream.readLine();
		if (data.trimmed().isEmpty())	continue;

		if (data[0] == QChar('['))
		{
			// INI檔 的 Section 部分
			QString iniSection;
			int inx = data.indexOf(QChar(']'));
			if (inx == -1)
			{
				// 沒有對應的 ']' 結尾，但仍然繼續處理下去
				ok = false;
				iniSection = data.mid(1);
			}
			else
			{
				iniSection = data.mid(1, inx - 1);
			}

			iniSection = iniSection.trimmed();
			if (iniSection.compare(QString("general"), Qt::CaseInsensitive) == 0)
			{
				// 若是 [general] 表示這是 Qt 內建的 section, 暫存於 INI 檔裡，
				// 讀取後，要將復原成 '空' 的 section
				currentSection.clear();
			}
			else
			{
				if (iniSection.compare(QString("%general"), Qt::CaseInsensitive) == 0)
				{
					// 為了避免跟 Qt 內建的 [general] section，起衝突；
					// 若是 [%general] 表示這是 原本在 INI 檔裡的 [general]
					// 所以，要將復原成 [general] section
					currentSection = QString("general");
				}
				else
				{
					currentSection = iniSection;
				}
				currentSection += QChar('/');
			}
		}
		else
		{
			// INI檔 的 Key = Value 部分
			bool inQuotes = false;
			int equalsPos = -1;
			QList<int> commaPos;
			int i = 0;
			while (i < data.size())
			{
				QChar ch = data.at(i);
				if (ch == QChar('='))
				{
					if (!inQuotes && equalsPos == -1)
					{
						equalsPos = i;
					}
				}
				else if (ch == QChar('"'))
				{
					inQuotes = !inQuotes;
				}
				else if (ch == QChar(','))
				{
					if (!inQuotes && equalsPos != -1)
					{
						commaPos.append(i);
					}
				}
				else if (ch == QChar(';') || ch == QChar('#'))
				{
					if (!inQuotes)
					{
						data.resize(i);
						break;
					}
				}
				else if (ch == QChar('\\'))
				{
					if (++i < data.size())
					{
					}
					else
					{
						ok = false;
						break;
					}
				}
				i++;
			}

			if (equalsPos == -1)
			{
				ok = false;
				break;
			}
			else
			{
				QString key = data.mid(0, equalsPos).trimmed();
				if (key.isEmpty())
				{
					ok = false;
					break;
				}
				else
				{
					key = currentSection + key;
				}
				if (commaPos.isEmpty())  //value
				{
					QString v = data.mid(equalsPos+1).trimmed();
					if (v.startsWith("\"") && v.endsWith("\"") && v.length()>1)
					{
						v = v.mid(1, v.length()-2);
					}
					map[key] = v;
				}
				else
				{ //value list
					commaPos.prepend(equalsPos);
					commaPos.append(-1);
					QVariantList vals;
					for (int i=1; i<commaPos.size(); ++i)
					{
						QString d = data.mid(commaPos.at(i-1)+1, commaPos.at(i)-commaPos.at(i-1)-1);
						QString v = d.trimmed();
						if (v.startsWith("\"") && v.endsWith("\"") && v.length()>1)
						{
							v = v.mid(1, v.length()-2);
						}
						vals.append(v);
					}
					map[key] = vals;
				}
			}
		}
	}

	return ok;
}

bool QSetting_UTF8Private::WriteFunc(QIODevice& device, const QSettings::SettingsMap& map)
{
#ifdef Q_OS_WIN
	const char * const eol = "\r\n";
#else
	const char eol = '\n';
#endif
	bool ok = true;

	QString lastSection;
	QMapIterator<QString,QVariant> it(map);

	while(it.hasNext() && ok)
	{
		it.next();

		QString section;
		QString key = it.key();

		int idx = key.lastIndexOf(QChar('/'));
		if (idx == -1)
		{
			section = QString("[General]");
		}
		else
		{
			section = key.left(idx);
			key = key.mid(idx+1);
			if (section.compare(QString("General"), Qt::CaseInsensitive) == 0)
			{
				section = QString("[%General]");
			}
			else
			{
				section.prepend(QChar('['));
				section.append(QChar(']'));
			}
		}

		if (section.compare(lastSection, Qt::CaseInsensitive))
		{
			if (!lastSection.isEmpty())
			{
				device.write(eol);
			}
			lastSection = section;
			if (device.write(section.toUtf8() + eol) == -1)
			{
				ok = false;
			}
		}

		QByteArray block = key.toUtf8();
		block += " = ";

		if (it.value().type() == QVariant::StringList)
		{
			foreach (QString s, it.value().toStringList())
			{
				block += s;
				block += ", ";
			}
			if (block.endsWith(", "))
			{
				block.chop(2);
			}
		}
		else if (it.value().type() == QVariant::List)
		{
			foreach (QVariant v, it.value().toList())
			{
				block += v.toString();
				block += ", ";
			}
			if (block.endsWith(", "))
			{
				block.chop(2);
			}
		}
		else
		{
			block += it.value().toString();
		}

		block += eol;
		if (device.write(block) == -1)
		{
			ok = false;
		}
	}

	return ok;
}
