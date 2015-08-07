/*
 * Copyright (C) 2015  Malte Veerman <maldela@halloarsch.de>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 *
 */

#ifndef SENSORS_H
#define SENSORS_H

#include <QObject>
#include <QTextStream>
#include <QTimer>

#include "hwmon.h"
#include "fancontrol_gui_lib_export.h"

class Hwmon;

class Sensor : public QObject
{
    Q_OBJECT
    Q_PROPERTY(uint index READ index CONSTANT)
    Q_PROPERTY(QString name READ name WRITE setName NOTIFY nameChanged)
    Q_PROPERTY(Hwmon * parent READ parent CONSTANT)

public:

    explicit Sensor(Hwmon *parent, uint index);

    virtual QString name() const = 0;
    virtual void setName(const QString &name) = 0;
    Hwmon * parent() const { return m_parent; }
    uint index() const { return m_index; }


public slots:

    virtual void update() = 0;


signals:

    void nameChanged();


protected:

    Hwmon *const m_parent;
    const uint m_index;
};


class FANCONTROL_GUI_LIB_EXPORT Temp : public Sensor
{
    Q_OBJECT
    Q_PROPERTY(QString label READ label NOTIFY labelChanged)
    Q_PROPERTY(int value READ value NOTIFY valueChanged)

public:

    explicit Temp(Hwmon *parent, uint index);

    QString label() const { return m_label; }
    int value() const { return m_value; }
    QString name() const;
    void setName(const QString &name);


public slots:

    void update();


signals:

    void labelChanged();
    void valueChanged();


protected:

    QString m_label;
    int m_value;
    QTextStream m_valueStream;
};


class FANCONTROL_GUI_LIB_EXPORT Fan : public Sensor
{
    Q_OBJECT
    Q_PROPERTY(int rpm READ rpm NOTIFY rpmChanged)

public:

    explicit Fan(Hwmon *parent, uint index);

    int rpm() const { return m_rpm; }
    QString name() const;
    void setName(const QString &name);

    virtual int pwm() const { return 255; }
    virtual void setPwm(int, bool) { }


signals:

    void rpmChanged();


public slots:

    void update();


protected:

    int m_rpm;
    QTextStream m_rpmStream;
};


class FANCONTROL_GUI_LIB_EXPORT PwmFan : public Fan
{
    Q_OBJECT
    Q_PROPERTY(int pwm READ pwm WRITE setPwm NOTIFY pwmChanged)
    Q_PROPERTY(Temp * temp READ temp WRITE setTemp NOTIFY tempChanged)
    Q_PROPERTY(bool hasTemp READ hasTemp WRITE setHasTemp NOTIFY hasTempChanged)
    Q_PROPERTY(int minTemp READ minTemp WRITE setMinTemp NOTIFY minTempChanged)
    Q_PROPERTY(int maxTemp READ maxTemp WRITE setMaxTemp NOTIFY maxTempChanged)
    Q_PROPERTY(int minPwm READ minPwm WRITE setMinPwm NOTIFY minPwmChanged)
    Q_PROPERTY(int maxPwm READ maxPwm WRITE setMaxPwm NOTIFY maxPwmChanged)
    Q_PROPERTY(int minStart READ minStart WRITE setMinStart NOTIFY minStartChanged)
    Q_PROPERTY(int minStop READ minStop WRITE setMinStop NOTIFY minStopChanged)
    Q_PROPERTY(bool active READ active WRITE setActive NOTIFY activeChanged)
    Q_PROPERTY(bool testing READ testing NOTIFY testingChanged)
    Q_PROPERTY(int pwmMode READ pwmMode WRITE setPwmMode NOTIFY pwmModeChanged)

public:

    explicit PwmFan(Hwmon *parent, uint index);

    int pwm() const { return m_pwm; }
    Temp * temp() const { return m_temp; }
    bool hasTemp() const { return m_hasTemp; }
    int minTemp() const { return m_minTemp; }
    int maxTemp() const { return m_maxTemp; }
    int minPwm() const { return m_minPwm; }
    int maxPwm() const { return m_maxPwm; }
    int minStart() const { return m_minStart; }
    int minStop() const { return m_minStop; }
    int pwmMode() const { return m_pwmMode; }
    bool active() const;
    bool testing() const { return m_testing; }
    void setPwm(int pwm, bool write = true);
    void setTemp(Temp *temp) { setHasTemp(temp != Q_NULLPTR); if (temp != m_temp) { m_temp = temp; emit tempChanged(); } }
    void setHasTemp(bool hasTemp) { if (hasTemp != m_hasTemp) { m_hasTemp = hasTemp; emit hasTempChanged(); } }
    void setMinTemp(int minTemp) { if (minTemp != m_minTemp) { m_minTemp = minTemp; emit minTempChanged(); } }
    void setMaxTemp(int maxTemp) { if (maxTemp != m_maxTemp) { m_maxTemp = maxTemp; emit maxTempChanged(); } }
    void setMinPwm(int minPwm) { if (minPwm != m_minPwm) { m_minPwm = minPwm; emit minPwmChanged(); } }
    void setMaxPwm(int maxPwm) { if (maxPwm != m_maxPwm) { m_maxPwm = maxPwm; emit maxPwmChanged(); } }
    void setMinStart(int minStart) { if (minStart != m_minStart) { m_minStart = minStart; emit minStartChanged(); } }
    void setMinStop(int minStop) { if (minStop != m_minStop) { m_minStop = minStop; emit minStopChanged(); } }
    void setPwmMode(int pwmMode, bool write = true);
    void setActive(bool active);
    void reset();
    Q_INVOKABLE void test();
    Q_INVOKABLE void abortTest();


signals:

    void pwmChanged();
    void tempChanged();
    void hasTempChanged();
    void minTempChanged();
    void maxTempChanged();
    void minPwmChanged();
    void maxPwmChanged();
    void minStartChanged();
    void minStopChanged();
    void activeChanged();
    void testingChanged();
    void pwmModeChanged();


protected slots:

    void update();
    void continueTest();


protected:

    int m_pwm;
    QTextStream m_pwmStream;
    QTextStream m_modeStream;
    QTimer m_testTimer;
    Temp *m_temp;
    bool m_hasTemp;
    bool m_testing;
    int m_minTemp;
    int m_maxTemp;
    int m_minPwm;
    int m_maxPwm;
    int m_minStart;
    int m_minStop;
    int m_pwmMode;

    enum
    {
        findingStop1,
        findingStop2,
        findingStart,
        notTesting
    } m_testStatus;
};

#endif // SENSORS_H