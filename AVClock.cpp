#include "AVClock.h"

//COSTRUTTORE, specifico il tipo di clock
AVClock::AVClock(AVClock::ClockType c, QObject *parent)
    :QObject(parent),auto_clock(true),clock_type(c)
{
    pts_ = pts_v = delay_ = 0;
}

//COSTRUTTORE
/* non specifico il tipo di clock, verra scelto di default AudioClock */
AVClock::AVClock(QObject *parent)
    :QObject(parent),auto_clock(true),clock_type(AudioClock)
{
    pts_ = pts_v = delay_ = 0;
}

AVClock::~AVClock(void){

};

void AVClock::setClockType(ClockType ct)
{
    clock_type = ct;
}

AVClock::ClockType AVClock::clockType() const
{
    return clock_type;
}

bool AVClock::isActive() const
{
    return clock_type == AudioClock || timer.isValid();
}

void AVClock::setClockAuto(bool a)
{
    auto_clock = a;
}

bool AVClock::isClockAuto() const
{
    return auto_clock;
}

void AVClock::updateExternalClock(qint64 msecs)
{
    if (clock_type != ExternalClock)
        return;
    qDebug("External clock change: %f ==> %f", value(), double(msecs) * kThousandth);
    pts_ = double(msecs) * kThousandth; //can not use msec/1000.
    timer.restart();
}

void AVClock::updateExternalClock(const AVClock &clock)
{
    if (clock_type != ExternalClock)
        return;
    qDebug("External clock change: %f ==> %f", value(), clock.value());
    pts_ = clock.value();
    timer.restart();
}

///////////////////////////////////////////////////////////////////////////////////////
// SLOT

void AVClock::start()
{
    qDebug("AVClock started!!!!!!!!");
    timer.start();
    emit started();
}
//remember last value because we don't reset  pts_, pts_v, delay_
void AVClock::pause(bool p)
{
    if (clock_type != ExternalClock)
        return;
    if (p) {
#if QT_VERSION >= QT_VERSION_CHECK(4, 7, 0)
        timer.invalidate();
#else
        timer.stop();
#endif //QT_VERSION >= QT_VERSION_CHECK(4, 7, 0)
        emit paused();
    } else {
        timer.start();
        emit resumed();
    }
    emit paused(p);
}

void AVClock::reset()
{
    pts_ = pts_v = delay_ = 0;
#if QT_VERSION >= QT_VERSION_CHECK(4, 7, 0)
    timer.invalidate();
#else
    timer.stop();
#endif //QT_VERSION >= QT_VERSION_CHECK(4, 7, 0)
    emit resetted();
}
