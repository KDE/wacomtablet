#include "multidbuspendingcallwatcher.h"
#include <QDBusPendingCallWatcher>

MultiDBusPendingCallWatcher::MultiDBusPendingCallWatcher(const QList< QDBusPendingCall >& calls, QObject* parent)
    : QObject(parent)
     ,m_unfinished(0)
{
    Q_FOREACH(const QDBusPendingCall& call, calls) {
        m_watchers << new QDBusPendingCallWatcher(call, this);
        if (!m_watchers.last()->isFinished()) {
            m_unfinished++;
        }

        connect(m_watchers.last(), &QDBusPendingCallWatcher::finished, [this](QDBusPendingCallWatcher* watcher) {
            Q_UNUSED(watcher);
            m_unfinished--;
            if (m_unfinished == 0) {
                emit finished(m_watchers);
            }
        });
    }
}

