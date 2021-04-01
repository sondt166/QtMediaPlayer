#include "SongPlayer.h"

SongPlayer::SongPlayer(QObject *parent) : QObject(parent)
{
    connect(m_player, SIGNAL(mediaStatusChanged(QMediaPlayer::MediaStatus)),
            this, SLOT(onMediaStatusChanged(QMediaPlayer::MediaStatus)));
    connect(m_playerMetaFetcher, SIGNAL(metaDataAvailableChanged(bool)),
            this, SLOT(onFetchMetaDataAvailableChanged(bool)));

    connect(m_player, &QMediaPlayer::mediaChanged,
            this, &SongPlayer::songChanged);

    connect(m_player, &QMediaPlayer::positionChanged,
            this, &SongPlayer::elapsedDutionChanged);

    connect(m_playerMetaFetcher, &QMediaPlayer::mediaStatusChanged, this, [=](const QMediaPlayer::MediaStatus & _status){
        if(_status == QMediaPlayer::MediaStatus::InvalidMedia) {
//            m_currentMetaFetchIndex++;
            m_list->removeItemAt(m_currentMetaFetchIndex);
            fetchMetaData();
        }
    });

    m_player->setVolume(100);
}

void SongPlayer::loadFolder(const QString &_path)
{
    if (!m_list)
        return;
    stop();

    m_list->clear();

    m_folderPath = _path;

    QDir dir(_path);
    QStringList songs = dir.entryList(QStringList() << "*.mp3" << "*.MP3", QDir::Files);

    foreach (QString filename, songs) {
        SongItem song;
        song.name = filename;
        song.path = dir.absoluteFilePath(filename);
        m_list->appendItem(song);
    }
    triggerFetchMetaData();
}

void SongPlayer::setList(SongList *_list)
{
    if(m_list)
        m_list->disconnect(this);
    m_list = _list;
    connect(m_list, &SongList::postItemRemoved, this, [=](int _idx){
        if(_idx < m_currentSongIndex) {
            m_currentSongIndex--;
        }
        else if(_idx == m_currentSongIndex) {
            m_currentSongIndex = -1;
            m_currentSong = nullptr;
        }
        emit songChanged();
    });
}

void SongPlayer::fetchMetaData()
{
    if(m_list->items()->size() == 0 || m_currentMetaFetchIndex >= m_list->items()->size()) {
        return;
    }

    QDir dir(m_folderPath);
    QString filepath = dir.absoluteFilePath(m_list->items()->at(m_currentMetaFetchIndex).name);
    m_playerMetaFetcher->setMedia(QUrl::fromLocalFile(filepath));
}

void SongPlayer::triggerFetchMetaData()
{
    m_currentMetaFetchIndex = 0;
    fetchMetaData();
}

bool SongPlayer::chooseSong(int _index)
{
    qDebug() << "choose song index " << _index;
    if (_index >= m_list->items()->size())
        return false;

    m_player->stop();
    m_currentSong = &m_list->items()->operator [](_index);
    m_currentSongIndex = _index;
    m_player->setMedia(QUrl::fromLocalFile(m_currentSong->path));
    emit songChanged();
    return play();
}

bool SongPlayer::play()
{
    qDebug() << "play";
    if(!m_currentSong){
        // play first song at startup
        m_currentSongIndex = 0;
        chooseSong(m_currentSongIndex);
    }

    m_playing = true;
    m_player->play();
    emit playingChanged(m_playing);
    return m_playing;
}

bool SongPlayer::pause()
{
    qDebug() << "pause";
    if(!m_currentSong)
        return false;

    m_player->pause();
    m_playing = false;
    emit playingChanged(m_playing);
    return true;
}

bool SongPlayer::stop()
{
    qDebug() << "stop";

    m_player->stop();
    m_currentSong = nullptr;
    m_currentSongIndex = -1;
    m_playing = false;

    emit songChanged();
    emit playingChanged(m_playing);
    return true;
}

bool SongPlayer::next()
{
    qDebug() << "next";
    m_currentSongIndex++;
    if (m_currentSongIndex >= m_list->items()->size()){
        stop();
        return false;
    }
    return chooseSong(m_currentSongIndex);
}

bool SongPlayer::prev()
{
    qDebug() << "prev";
    m_currentSongIndex--;
    if (m_currentSongIndex < 0){
        stop();
        return false;
    }

    return chooseSong(m_currentSongIndex);
}

bool SongPlayer::seek(qint64 _ms)
{
    qDebug() << "seek to " << _ms;
    if (!m_currentSong)
        return false;

    m_player->setPosition(_ms);
    return true;
}

void SongPlayer::onMediaStatusChanged(QMediaPlayer::MediaStatus _status)
{
    // handle status message
    switch (_status) {
    case QMediaPlayer::MediaStatus::UnknownMediaStatus:
    case QMediaPlayer::MediaStatus::NoMedia:
    case QMediaPlayer::MediaStatus::LoadedMedia:
    case QMediaPlayer::MediaStatus::BufferingMedia:
    case QMediaPlayer::MediaStatus::BufferedMedia:
        qDebug() << _status;
        break;
    case QMediaPlayer::MediaStatus::LoadingMedia:
        qDebug() << "Loading...";
        break;
    case QMediaPlayer::MediaStatus::StalledMedia:
        qDebug() << "StalledMedia";
        break;
    case QMediaPlayer::MediaStatus::EndOfMedia:
        qDebug() << "EndOfMedia";
        next();
        break;
    case QMediaPlayer::MediaStatus::InvalidMedia:
        qDebug() << "InvalidMedia";
        break;
    }
}

void SongPlayer::onFetchMetaDataAvailableChanged(bool _available)
{
    if(_available) {
//        qDebug() << "";
        auto item =  m_list->items()->operator [](m_currentMetaFetchIndex);
        foreach(QString metaKey, m_playerMetaFetcher->availableMetaData()){
//            qDebug() << metaKey << ": " << m_playerMetaFetcher->metaData(metaKey);
            if (metaKey == "Title"){
                item.name =  m_playerMetaFetcher->metaData(metaKey).toString();
            }
            if (metaKey == "Duration"){
                item.duration =  m_playerMetaFetcher->metaData(metaKey).value<qint64>();
            }
            if (metaKey == "ContributingArtist"){
                item.artist =  m_playerMetaFetcher->metaData(metaKey).toStringList().join(", ");
            }
            if (metaKey == "ThumbnailImage"){
                item.image =  m_playerMetaFetcher->metaData(metaKey).value<QImage>();
            }
        }

        m_list->setItemAt(m_currentMetaFetchIndex, item);
        m_currentMetaFetchIndex++;

        if(m_currentMetaFetchIndex >= m_list->items()->size()) {
            qDebug() << "Metadata fetching finished";
        } else
            fetchMetaData();
    } else {
//        qDebug() << "metadata not available";
    }
}
