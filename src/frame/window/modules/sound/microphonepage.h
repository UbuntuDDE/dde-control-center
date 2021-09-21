/*
 * Copyright (C) 2011 ~ 2019 Deepin Technology Co., Ltd.
 *
 * Author:     lq <longqi_cm@deepin.com>
 *
 * Maintainer: lq <longqi_cm@deepin.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef MICROPHONEPAGE_H_V20
#define MICROPHONEPAGE_H_V20

#include "interface/namespace.h"
#include "modules/sound/soundmodel.h"

#include <QStandardItemModel>
#include <QWidget>

#define ICON_SIZE 24

QT_BEGIN_NAMESPACE
class QVBoxLayout;
QT_END_NAMESPACE

namespace dcc {

namespace widgets {
class TitledSliderItem;
class SwitchWidget;
class ComboxWidget;
class Port;
}

namespace sound {
class SoundModel;
}
}

namespace DCC_NAMESPACE {

namespace sound {

class MicrophonePage : public QWidget
{
    Q_OBJECT
public:
    MicrophonePage(QWidget *parent = nullptr);

    ~MicrophonePage();
    void resetUi();
    void showDevice();
    void setDeviceVisible(bool visable);
public:
    void setModel(dcc::sound::SoundModel *model);

Q_SIGNALS:
    void requestSwitchMicrophone(bool on);
    void requestSetMicrophoneVolume(double vol);
    void requestSetPort(const dcc::sound::Port *);
    //请求降噪
   void requestReduceNoise(bool value);
   //请求静音切换,flag为false时请求直接取消静音
   void requestMute(bool flag = true);

private Q_SLOTS:
    void removePort(const QString &portId, const uint &cardId);
    void addPort(const dcc::sound::Port *port);
    void toggleMute();
    void changeComboxIndex(const int idx);
    void changeComboxStatus();

private:
    void initSlider();
    void initCombox();
    void refreshIcon();
    void showWaitSoundPortStatus(bool showStatus);
    void refreshActivePortShow(const dcc::sound::Port *port);

private:
    dcc::sound::SoundModel *m_model{nullptr};
    QVBoxLayout *m_layout{nullptr};
    dcc::widgets::TitledSliderItem *m_inputSlider{nullptr};
    dcc::widgets::TitledSliderItem *m_feedbackSlider{nullptr};
    QMetaObject::Connection m_conn;
    //输入列表的下拉框列表
    dcc::widgets::ComboxWidget *m_inputSoundCbx;
    //噪音抑制
    dcc::widgets::SwitchWidget *m_noiseReductionsw{nullptr};

    QStandardItemModel *m_inputModel{nullptr};
    const dcc::sound::Port  *m_currentPort{nullptr};

    dcc::sound::SoundLabel *m_volumeBtn;
    int m_waitTimerValue;
    int m_lastRmPortIndex;
    bool m_mute;
    //启用端口但未设置为默认端口判断
    bool m_enablePort;
    bool m_enable;
    // 确保第一次点击没有延时
    bool m_fristChangePort;
    bool m_currentBluetoothPortStatus;
    bool m_fristStatusChangePort;
    QTimer *m_waitStatusChangeTimer;
    QTimer *m_waitCurrentPortRemove;
};

}
}

#endif // MICROPHONEPAGE_H_V20
