#include "AVClock.h"
#include <qtimer.h>
#include <qdatetime.h>
#include <QTimerEvent>

enum {
	kRunning,
	kPaused,
	kStopped
};

AVClock::AVClock(AVClock::ClockType c, QObject* parent) :	QObject(parent), auto_clock(true), m_state(kStopped), clock_type(c), mSpeed(1.0)
	, value0(0), avg_err(0), nb_restarted(0), nb_sync(0), sync_id(0)
{
	last_pts = pts_ = pts_v = delay_ = 0;
}

AVClock::AVClock(QObject* parent) :	QObject(parent), auto_clock(true), m_state(kStopped), clock_type(AudioClock), mSpeed(1.0)
	, value0(0), avg_err(0), nb_restarted(0), nb_sync(0), sync_id(0)
{
	last_pts = pts_ = pts_v = delay_ = 0;
}



void AVClock::setClockType(ClockType ct)
{
	if (clock_type == ct)
		return;
	clock_type = ct;
	QTimer::singleShot(0, this, SLOT(restartCorrectionTimer()));
}


void AVClock::updateExternalClock(qint64 msecs)
{
	if (clock_type == AudioClock)
		return;
	qDebug("External clock change: %f ==> %f", value(), static_cast<double>(msecs) * kThousandth);
	pts_ = static_cast<double>(msecs) * kThousandth; //can not use msec/1000.
	if (!isPaused())
		timer.restart();

	last_pts = pts_;
	t = QDateTime::currentMSecsSinceEpoch();
	if (clockType() == VideoClock)
		pts_v = pts_;
}

void AVClock::updateExternalClock(const AVClock& clock)
{
	if (clock_type != ExternalClock)
		return;
	qDebug("External clock change: %f ==> %f", value(), clock.value());
	pts_ = clock.value();
	if (!isPaused())
		timer.restart();

	last_pts = pts_;
	t = QDateTime::currentMSecsSinceEpoch();
}

bool AVClock::isPaused() const
{
	return m_state == kPaused; 
}

int AVClock::syncStart(int count)
{
	static int sId = 0;
	nb_sync = count;
	if (sId == -1)
		sId = 0;
	sync_id = ++sId;
	return sId;
}

bool AVClock::syncEndOnce(int id)
{
	if (id != sync_id) {
		qWarning("bad sync id: %d, current: %d", id, sync_id);
		return true;
	}
	if (!nb_sync.deref())
		sync_id = 0;
	return sync_id;
}

void AVClock::start()
{
	m_state = kRunning;
	qDebug("AVClock started!!!!!!!!");
	timer.start();
	QTimer::singleShot(0, this, SLOT(restartCorrectionTimer()));
	Q_EMIT started();
}

void AVClock::pause(bool p)
{
	if (isPaused() == p)
		return;
	if (clock_type == AudioClock)
		return;
	m_state = p ? kPaused : kRunning;
	if (p) 
	{
		QTimer::singleShot(0, this, SLOT(stopCorrectionTimer()));
		timer.invalidate();
		Q_EMIT paused();
	}
	else 
	{
		timer.start();
		QTimer::singleShot(0, this, SLOT(restartCorrectionTimer()));
		Q_EMIT resumed();
	}
	t = QDateTime::currentMSecsSinceEpoch();
	Q_EMIT paused(p);
}

void AVClock::reset()
{
	nb_sync = 0;
	sync_id = 0;
	// keep mSpeed
	m_state = kStopped;
	value0 = 0;
	pts_ = pts_v = delay_ = 0;
	QTimer::singleShot(0, this, SLOT(stopCorrectionTimer()));
	timer.invalidate();
	t = QDateTime::currentMSecsSinceEpoch();
	Q_EMIT resetted();
}

void AVClock::timerEvent(QTimerEvent* event)  
{
	Q_ASSERT_X(clockType() != AudioClock, "AVClock::timerEvent", "Internal error. AudioClock can not call this");
	if (event->timerId() != correction_schedule_timer.timerId())
		return;
	if (isPaused())
		return;
	const double delta_pts = (value() - last_pts) / speed();
	const qint64 now = QDateTime::currentMSecsSinceEpoch();
	const double err =  static_cast<double>(now - t) * kThousandth - delta_pts;
	t = now;	
	if (qAbs(err* 10.0) < kCorrectionInterval || clock_type == VideoClock)
	{
		avg_err += err / (nb_restarted + 1);
	}	
	last_pts = value();
	nb_restarted = 0;
}

void AVClock::restartCorrectionTimer()
{
	nb_restarted = 0;
	avg_err = 0;
	correction_schedule_timer.stop();
	if (clockType() == AudioClock) // TODO: for all clock type
		return;
	// parameters are reset. do not start correction timer if not running
	if (m_state != kRunning)
		return;
	// timer is always started in AVClock::start()
	if (!timer.isValid())
		return;
	t = QDateTime::currentMSecsSinceEpoch();
	correction_schedule_timer.start(kCorrectionInterval * 1000, this);
}

void AVClock::stopCorrectionTimer()
{
	nb_restarted = 0;
	avg_err = 0;
	correction_schedule_timer.stop();
}