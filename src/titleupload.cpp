#include <QBuffer>
#include <QCoreApplication>
#include <QDebug>
#include <QImage>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QSharedPointer>
#include <QTimer>

#include <optional>
#include "cppc4group.hpp"

#include "titleupload.h"

void TitleUpload::upload()
{
	auto manager = QSharedPointer<QNetworkAccessManager>::create();
	QNetworkReply *last = nullptr;

	for (const QFileInfo &entry : path.entryInfoList(QDir::AllEntries | QDir::NoDotAndDotDot))
	{
		QImage image;
		if (entry.isFile())
		{
			auto group = QSharedPointer<CppC4Group>::create();
			if (!group->openExisting(path.filePath(entry.fileName()).toLocal8Bit().constData()))
			{
				continue;
			}
			std::optional<std::vector<CppC4Group::EntryInfo>> infos = group->getEntryInfos();
			if (!infos)
			{
				continue;
			}
			for (const CppC4Group::EntryInfo &info : *infos)
			{
				if (QString::fromStdString(info.fileName).toLower().startsWith(QLatin1String("title.")))
				{
					std::optional<CppC4Group::Data> data = group->getEntryData(info.fileName);
					if (data)
					{
						if (image.loadFromData(reinterpret_cast<const uint8_t *>(data->data), static_cast<int>(data->size)))
						{
							break;
						}
					}
				}
			}
		}
		else
		{
			QDir subdir(entry.absoluteFilePath());
			for (const QString &subentry : QDir(entry.absoluteFilePath()).entryList(QStringList() << "title.*" << "Title.*"))
			{
				if (image.load(subdir.filePath(subentry)))
				{
					break;
				}
			}
		}

		if (image.isNull())
		{
			continue;
		}

		if (image.width() > MaxWidth)
		{
			image = image.scaledToWidth(MaxWidth, Qt::SmoothTransformation);
		}
		if (image.height() > MaxHeight)
		{
			image = image.scaledToHeight(MaxHeight, Qt::SmoothTransformation);
		}
#if 0
		if (image.sizeInBytes() > 100 * 1024)
		{
			qWarning() << "Skipping" << entry.fileName() << "(title image too large)";
			continue;
		}
#endif

		QBuffer buffer;
		image.save(&buffer, "PNG");
		qDebug() << "posting";

		QNetworkReply *reply = last = manager->post(
					QNetworkRequest(QUrl(QStringLiteral("http://games-title-upload.clonkspot.org/%1)").arg(entry.fileName()))),
					buffer.data()
					);
		QObject::connect(manager.get(), &QNetworkAccessManager::finished, [last, name = entry.fileName()](QNetworkReply *reply)
		{
			qInfo() << name;
			reply->deleteLater();
			if (reply == last)
			{
				QCoreApplication::quit();
			}
		});
		QObject::connect(
					reply,
					qOverload<QNetworkReply::NetworkError>(&QNetworkReply::error),
					[last, name = entry.fileName(), reply](QNetworkReply::NetworkError error)
		{
			Q_UNUSED(error)
			qWarning() << name << "failed (" << reply->errorString() << ")";
			if (reply == last)
			{
				QCoreApplication::quit();
			}
		});
	}
}
