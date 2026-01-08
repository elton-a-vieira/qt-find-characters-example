#include "worker.h"
#include "findcharacters.h"

Worker::Worker(QObject *parent) : QThread(parent) {

}

void Worker::fetchCharacters(QString episodeId) {
    this->episodeId = episodeId;
    notify_process_episode();
}

void Worker::run() {
    forever {
        QString result;
        QNetworkAccessManager manager;
        QNetworkRequest request;
        charNames.clear();
        charUrlList.clear();

        QObject::connect(
            &manager,
            &QNetworkAccessManager::finished,
            this,
            &Worker::handle_requests_result
        );

        wait_process_episode();

        QString url = "https://rickandmortyapi.com/api/episode/" + this->episodeId;
        request.setUrl(QUrl(url));

        wait_request_done(manager.get(request));

        for (auto &url : charUrlList) {
            request.setUrl(url);

            wait_request_done(manager.get(request));
        }

        for (auto &name : charNames) {
            result += name + tr("\n");
        }

        emit resultReady(result);
    }
}

void Worker::handle_requests_result(QNetworkReply *reply) {
    if (reply->error()) {
        // qDebug() << reply->errorString();
        return;
    }

    auto url = reply->url().toString();

    if (url.contains("https://rickandmortyapi.com/api/episode")) {

        /*
         * Extract the characteres list
         */

        QJsonObject epJson = to_json(reply->readAll());
        if (epJson.value("characters") != QJsonValue::Undefined) {
            QJsonArray charList = epJson.value("characters").toArray();
            for (QJsonValueRef c : charList) {
                charUrlList.push_back(QUrl(c.toString()));
            }
        }

    } else if (url.contains("https://rickandmortyapi.com/api/character")) {
        /*
         * Get the character name
         */

        QJsonObject charJson = to_json(reply->readAll());
        if (charJson.value("name") != QJsonValue::Undefined) {
            charNames.append(charJson.value("name").toString());
        }
    }

    notify_request_done();
}

QJsonObject Worker::to_json(QString jsonString) {
    QJsonObject result;

    if (jsonString.isEmpty()) {
        return result;
    }

    QJsonParseError parseError;
    QJsonDocument doc = QJsonDocument::fromJson(jsonString.toUtf8(), &parseError);

    if (!doc.isNull()) {
        result = doc.object();
    }

    return result;
}

void Worker::notify_process_episode()
{
    smProcessEpisode.release();
}

void Worker::wait_process_episode()
{
    smProcessEpisode.acquire();
}

void Worker::notify_request_done()
{
    smHttpRequestDone.release();
}

void Worker::wait_request_done()
{
    smHttpRequestDone.acquire();
}

void Worker::wait_request_done(QNetworkReply *l_reply)
{
    QEventLoop l_event_loop;
    QObject::connect(l_reply, SIGNAL(finished()), &l_event_loop, SLOT(quit()));
    l_event_loop.exec();
}

