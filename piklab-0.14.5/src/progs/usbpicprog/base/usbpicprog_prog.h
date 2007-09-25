/***************************************************************************
 *   Copyright (C) 2005 Nicolas Hadacek <hadacek@kde.org>                  *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 ***************************************************************************/
#ifndef USBPICPROG_PROG_H
#define USBPICPROG_PROG_H

#include "common/global/global.h"
#include "progs/base/prog_group.h"
#include "devices/pic/pic/pic_prog.h"
#include "devices/mem24/mem24/mem24_prog.h"
#include "usbpicprog.h"

namespace Usbpicprog
{
  extern bool isSupported(const QString &device);
  class Hardware;

//----------------------------------------------------------------------------
class PicBase : public ::Programmer::PicBase
{
Q_OBJECT
public:
  PicBase(const ::Programmer::Group &group, const Pic::Data *data)
    : ::Programmer::PicBase(group, data, "pic_usbpicprog_programmer") {}

private:
  Hardware &hardware() { return static_cast<Hardware &>(*_hardware); }
};

//----------------------------------------------------------------------------
class Mem24Base : public ::Programmer::Mem24Base
{
Q_OBJECT
public:
  Mem24Base(const ::Programmer::Group &group, const Mem24::Data *data)
    : ::Programmer::Mem24Base(group, data, "mem24_usbpicprog_programmer") {}

private:
  Hardware &hardware() { return static_cast<Hardware &>(*_hardware); }
};

//----------------------------------------------------------------------------
class Group : public ::Programmer::PicGroup // methods defined in usbpicprog_data.cpp
{
protected:
  virtual void initSupported();
  virtual ::Programmer::DeviceSpecific *createDeviceSpecific(::Programmer::Base &base) const;
};

class UGroup : public Group
{
public:
  virtual QString name() const { return "usbpicprog"; }
  virtual QString label() const { return i18n("Usbpicprog Programmer"); }
  virtual QString statusLabel() const;
  virtual ::Programmer::Properties properties() const { return ::Programmer::Programmer | ::Programmer::CanReadMemory | ::Programmer::HasConnectedState; }
  virtual ::Programmer::TargetPowerMode targetPowerMode() const { return ::Programmer::TargetExternallyPowered; }
  virtual bool isPortSupported(Port::Type type) const { return ( type==Port::SerialType || type==Port::ParallelType ); }

protected:
  virtual void initSupported();
  virtual ::Programmer::Base *createBase(const Device::Data *data) const;
  virtual ::Programmer::Hardware *createHardware(::Programmer::Base &base, const Port::Description &pd) const;
  virtual ::Programmer::DeviceSpecific *createDeviceSpecific(::Programmer::Base &base) const;
};

} // namespace

#endif
