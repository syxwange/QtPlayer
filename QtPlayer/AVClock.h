#pragma once

#include <QObject>
#include <qelapsedtimer.h>
#include <qbasictimer.h>
#include <qatomic.h>


static const double kThousandth = 0.001;

class AVClock : public QObject
{
	Q_OBJECT

public:
	typedef enum {
		AudioClock,
		ExternalClock,
		VideoClock     //sync to video timestamp
	} ClockType;

	AVClock(ClockType c, QObject* parent = 0) ;
	AVClock(QObject* parent = 0) ;
	
	void setClockType(ClockType ct);
	inline ClockType clockType() const  noexcept  {	return clock_type;}
	inline bool isActive() const noexcept {	return clock_type == AudioClock || timer.isValid();	}


	//Usually for ExternalClock. For example, media start time is not 0, clock have to set initial value as media start time	
	inline void setInitialValue(double v) {value0 = v;	qDebug("Clock initial value: %f", v);}
	inline double initialValue() const noexcept {	return value0;}

	//auto clock: use audio clock if audio stream found, otherwise use external clock	
	inline void setClockAuto(bool a) noexcept {auto_clock = a;}
	bool isClockAuto() const noexcept {return auto_clock;}

	//in seconds
	inline double pts() const noexcept { return pts_; }

	// the real timestamp in seconds: pts + delay
	inline double value() const noexcept 
	{
		if (clock_type == AudioClock)
		{
			// TODO: audio clock need a timer too	£» timestamp from media stream is >= value0
			return pts_ == 0 ? value0 : pts_ + delay_;
		}
		else if (clock_type == ExternalClock) 
		{
			if (timer.isValid())
			{
				++nb_restarted;
				pts_ += (static_cast<double>(timer.restart()) * kThousandth + avg_err) * speed();
			}			
			return pts_ + value0;
		}
		else
		{
			return pts_v; // value0 is 1st video pts_v already
		}
	}

	inline void updateValue(double pts) noexcept  //update the pts
	{
		if (clock_type == AudioClock)
			pts_ = pts;
	}
	/*used when seeking and correcting from external*/
	void updateExternalClock(qint64 msecs);
	/*external clock outside still running, so it's more accurate for syncing multiple clocks serially*/
	void updateExternalClock(const AVClock& clock);

	inline void updateVideoTime(double pts) noexcept
	{
		pts_v = pts;
		if (clock_type == VideoClock)
			timer.restart();
	} 
	inline double videoTime() const noexcept {return pts_v;}
	inline double delay() const noexcept {return delay_;	}
	inline void updateDelay(double delay) noexcept {	delay_ = delay;	}
	inline qreal diff() const noexcept {return value() - videoTime();}

	void setSpeed(qreal speed) noexcept {mSpeed = speed;}
	inline qreal speed() const noexcept {	return mSpeed;}
	bool isPaused() const;

	
	int syncStart(int count);
	int syncId() const noexcept { return sync_id; }
	bool syncEndOnce(int id);

Q_SIGNALS:	
	void paused(bool);
	void paused(); //equals to paused(true)
	void resumed();//equals to paused(false)
	void started();
	void resetted();

public Q_SLOTS:
	//these slots are not frequently used. so not inline
	/*start the external clock*/
	void start();
	/*pause external clock*/
	void pause(bool p);
	/*reset clock intial value and external clock parameters (and stop timer). keep speed() and isClockAuto()*/
	void reset();

protected:
	virtual void timerEvent(QTimerEvent* event) final;
private Q_SLOTS:
	/// make sure QBasic timer start/stop in a right thread
	void restartCorrectionTimer();
	void stopCorrectionTimer();

private:
	bool auto_clock;
	int m_state;
	ClockType clock_type;
	mutable double pts_;
	mutable double pts_v;
	double delay_;
	mutable QElapsedTimer timer;
	qreal mSpeed;
	double value0;	
	QBasicTimer correction_schedule_timer;
	qint64 t; // absolute time for elapsed timer correction
	static const int kCorrectionInterval = 1; // 1000ms
	double last_pts;
	double avg_err; // average error of restart()
	mutable int nb_restarted;
	QAtomicInt nb_sync;
	int sync_id;

};
