#ifndef WORKER_H
#define WORKER_H

#include <QWidget>
#include <QtNetwork/QNetworkAccessManager>
#include <QtNetwork/QNetworkReply>
#include <QtNetwork/QNetworkRequest>
#include <QApplication>
#include <QJsonObject>
#include <QJsonDocument>
#include <QJsonArray>
#include <QMutex>
#include <QWaitCondition>
#include <QSemaphore>
#include <QThread>

class Worker : public QThread
{
    Q_OBJECT
public:
    explicit Worker(QObject *parent = nullptr);
    void fetchCharacters(QString episodeId);

protected:
    void run() override;

signals:
    void resultReady(QString result);

private slots:
    void handle_requests_result(QNetworkReply *reply);

private:
    QString episodeId;
    QList<QUrl> charUrlList;
    QStringList charNames;
    QSemaphore smProcessEpisode;
    QSemaphore smHttpRequestDone;

    QJsonObject to_json(QString jsonString);

    void notify_process_episode();
    void wait_process_episode();
    void notify_request_done();
    void wait_request_done();
    void wait_request_done(QNetworkReply *l_reply);

};

#endif // WORKER_H
