#ifndef SONGPLAYER_H
#define SONGPLAYER_H

#include <QObject>
#include <QMediaPlayer>
#include <QMediaMetaData>

#include "SongList.h"

class SongPlayer : public QObject
{
    Q_OBJECT
    Q_PROPERTY(qint64 currentDuration READ currentDuration)
    Q_PROPERTY(QString currentName READ currentName)
    Q_PROPERTY(QString currentPath READ currentPath)
    Q_PROPERTY(QString currentArtist READ currentArtist)
    Q_PROPERTY(int currentSongIndex READ currentSongIndex)
    Q_PROPERTY(bool playing READ playing NOTIFY playingChanged)

public:
    explicit SongPlayer(QObject *parent = nullptr);

    qint64 currentDuration() const { return m_currentSong ? m_currentSong->duration : 0; }
    QString currentName() const { return m_currentSong ? m_currentSong->name : ""; }
    QString currentPath() const { return m_currentSong ? m_currentSong->path : ""; }
    QString currentArtist() const { return m_currentSong ? m_currentSong->artist : ""; }
    int currentSongIndex() const { return m_currentSong ? m_currentSongIndex : -1; }
    bool playing() const { return m_playing; }

    Q_INVOKABLE void loadFolder(const QString &_path);
    void setList(SongList *_list);

private:
    void fetchMetaData();
    void triggerFetchMetaData();

signals:
    void songChanged();
    void elapsedDutionChanged(qint64 elapsed);
    void playingChanged(bool playing);

public slots:
    bool chooseSong(int _index);
    bool play();
    bool pause();
    bool stop();
    bool next();
    bool prev();
    bool seek(qint64 _ms);

    void onMediaStatusChanged(QMediaPlayer::MediaStatus _status);
    void onFetchMetaDataAvailableChanged(bool _available);

private:
    SongItem * m_currentSong = nullptr;
    SongList * m_list = nullptr;
    int m_currentSongIndex = -1;
    QString m_folderPath;
    int m_currentMetaFetchIndex = 0;
    bool m_playing = false;

    QMediaPlayer * m_player = new QMediaPlayer();
    QMediaPlayer * m_playerMetaFetcher = new QMediaPlayer();
};

#endif // SONGPLAYER_H
