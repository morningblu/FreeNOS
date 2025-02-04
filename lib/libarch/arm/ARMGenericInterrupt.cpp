/*
 * Copyright (C) 2019 Niek Linnenbank
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <Log.h>
#include "ARMGenericInterrupt.h"

ARMGenericInterrupt::ARMGenericInterrupt(
    Address distRegisterBase,
    Address cpuRegisterBase)
    : IntController()
{
    // Set I/O register bases
    m_dist.setBase(distRegisterBase);
    m_cpu.setBase(cpuRegisterBase);

    // Read number of interrupt lines available
    m_numIrqs = 32 * ((m_dist.read(GICD_TYPER) & DistTypeIrqsMask) + 1);
    NOTICE(m_numIrqs << " IRQ lines");

    // Enable all groups
    m_dist.write(GICD_CTRL, DistCtrlGroup0 | DistCtrlGroup1);
    m_cpu.write(GICC_CTRL, CpuCtrlGroup0 | CpuCtrlGroup1);

    // Set all interrupts in group 0
    for (uint i = 0; i < numRegisters(1); i++)
    {
        m_dist.write(GICD_GROUPR + i, 0);
    }

    // All interrupts assigned to core0
    for (uint i = 0; i < (32 / 4); i++)
    {
        m_dist.write(GICD_ITARGETSR + i, (1 << 0) | (1 << 8) | (1 << 16) | (1 << 24));
    }

    // Initially disable forwarding of all interrupts
    for (uint i = 0; i < numRegisters(1); i++)
    {
        m_dist.write(GICD_ISENABLER + i, 0);
    }
}

ARMGenericInterrupt::Result ARMGenericInterrupt::enable(uint irq)
{
    m_dist.set(GICD_ISENABLER + (irq % 32), irq % 32);
    return Success;
}

ARMGenericInterrupt::Result ARMGenericInterrupt::disable(uint irq)
{
    m_dist.set(GICD_ICENABLER + (irq % 32), irq % 32);
    return Success;
}

ARMGenericInterrupt::Result ARMGenericInterrupt::clear(uint irq)
{
    m_cpu.write(GICC_EOIR, irq);
    return Success;
}

ARMGenericInterrupt::Result ARMGenericInterrupt::nextPending(uint & irq)
{
    irq = m_cpu.read(GICC_IAR) & CpuIrqAckMask;

    // Is this a spurious (unexpected) interrupt?
    if (irq == 1023)
    {
        return NotFound;
    }
    else
        return Success;
}

bool ARMGenericInterrupt::isTriggered(uint irq)
{
    // Unused
    return false;
}

Size ARMGenericInterrupt::numRegisters(Size bits) const
{
    return (m_numIrqs * bits) / 32;
}
