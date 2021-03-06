/*
Copyright_License {

  XCSoar Glide Computer - http://www.xcsoar.org/
  Copyright (C) 2000-2012 The XCSoar Project
  A detailed list of copyright holders can be found in the file "AUTHORS".

  This program is free software; you can redistribute it and/or
  modify it under the terms of the GNU General Public License
  as published by the Free Software Foundation; either version 2
  of the License, or (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program; if not, write to the Free Software
  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
}
*/

#include "BarographRenderer.hpp"
#include "ChartRenderer.hpp"
#include "Look/CrossSectionLook.hpp"
#include "Screen/Canvas.hpp"
#include "Units/Units.hpp"
#include "Task/ProtectedTaskManager.hpp"
#include "NMEA/Info.hpp"
#include "NMEA/Derived.hpp"
#include "FlightStatistics.hpp"
#include "Language/Language.hpp"

void
BarographCaption(TCHAR *sTmp, const FlightStatistics &fs)
{
  ScopeLock lock(fs.mutexStats);
  if (fs.Altitude_Ceiling.sum_n < 2) {
    sTmp[0] = _T('\0');
  } else if (fs.Altitude_Ceiling.sum_n < 4) {
    _stprintf(sTmp, _T("%s:\r\n  %.0f-%.0f %s"),
              _("Working band"),
              (double)Units::ToUserAltitude(fixed(fs.Altitude_Base.y_ave)),
              (double)Units::ToUserAltitude(fixed(fs.Altitude_Ceiling.y_ave)),
              Units::GetAltitudeName());
  } else {
    _stprintf(sTmp, _T("%s:\r\n  %.0f-%.0f %s\r\n\r\n%s:\r\n  %.0f %s/hr"),
              _("Working band"),
              (double)Units::ToUserAltitude(fixed(fs.Altitude_Base.y_ave)),
              (double)Units::ToUserAltitude(fixed(fs.Altitude_Ceiling.y_ave)),
              Units::GetAltitudeName(),
              _("Ceiling trend"),
              (double)Units::ToUserAltitude(fixed(fs.Altitude_Ceiling.m)),
              Units::GetAltitudeName());
  }
}

static bool
IsTaskLegVisible(const OrderedTaskPoint &tp)
{
  switch (tp.GetType()) {
  case TaskPoint::START:
    return tp.HasExited();

  case TaskPoint::FINISH:
  case TaskPoint::AAT:
  case TaskPoint::AST:
    return tp.HasEntered();

  case TaskPoint::UNORDERED:
  case TaskPoint::ROUTE:
    break;
  }

  /* not reachable */
  assert(false);
  return false;
}

static void
DrawLegs(ChartRenderer &chart,
         const TaskManager &task_manager,
         const NMEAInfo& basic,
         const DerivedInfo& calculated,
         const bool task_relative)
{
  if (!calculated.common_stats.task_started)
    return;

  const fixed start_time = task_relative
    ? basic.time - calculated.common_stats.task_time_elapsed
    : calculated.flight.takeoff_time;

  const OrderedTask &task = task_manager.GetOrderedTask();
  for (unsigned i = 0, n = task.TaskSize(); i < n; ++i) {
    const OrderedTaskPoint &tp = task.GetTaskPoint(i);
    if (!IsTaskLegVisible(tp))
      continue;

    fixed x = tp.GetEnteredState().time - start_time;
    if (!negative(x)) {
      x /= 3600;
      chart.DrawLine(x, chart.getYmin(), x, chart.getYmax(),
                     ChartLook::STYLE_REDTHICK);
    }
  }
}

void
RenderBarographSpark(Canvas &canvas, const PixelRect rc,
                     const ChartLook &chart_look,
                     const CrossSectionLook &cross_section_look,
                     bool inverse,
                     const FlightStatistics &fs,
                     const NMEAInfo &nmea_info,
                     const DerivedInfo &derived_info,
                     const ProtectedTaskManager *_task)
{
  ScopeLock lock(fs.mutexStats);
  ChartRenderer chart(chart_look, canvas, rc);
  chart.PaddingBottom = 0;
  chart.PaddingLeft = 0;

  if (fs.Altitude.sum_n < 2)
    return;

  chart.ScaleXFromData(fs.Altitude);
  chart.ScaleYFromData(fs.Altitude);
  chart.ScaleYFromValue(fixed_zero);

  if (_task != NULL) {
    ProtectedTaskManager::Lease task(*_task);
    canvas.SelectHollowBrush();
    DrawLegs(chart, task, nmea_info, derived_info, false);
  }

  canvas.SelectNullPen();
  canvas.Select(cross_section_look.terrain_brush);

  chart.DrawFilledLineGraph(fs.Altitude_Terrain);

  Pen pen(2, inverse ? COLOR_WHITE : COLOR_BLACK);
  chart.DrawLineGraph(fs.Altitude, pen);
}

void
RenderBarograph(Canvas &canvas, const PixelRect rc,
                const ChartLook &chart_look,
                const CrossSectionLook &cross_section_look,
                const FlightStatistics &fs,
                const NMEAInfo &nmea_info,
                const DerivedInfo &derived_info,
                const ProtectedTaskManager *_task)
{
  ChartRenderer chart(chart_look, canvas, rc);

  if (fs.Altitude.sum_n < 2) {
    chart.DrawNoData();
    return;
  }

  chart.ScaleXFromData(fs.Altitude);
  chart.ScaleYFromData(fs.Altitude);
  chart.ScaleYFromValue(fixed_zero);
  chart.ScaleXFromValue(fs.Altitude.x_min + fixed_one); // in case no data
  chart.ScaleXFromValue(fs.Altitude.x_min);

  if (_task != NULL) {
    ProtectedTaskManager::Lease task(*_task);
    DrawLegs(chart, task, nmea_info, derived_info, false);
  }

  canvas.SelectNullPen();
  canvas.Select(cross_section_look.terrain_brush);

  chart.DrawFilledLineGraph(fs.Altitude_Terrain);
  canvas.SelectWhitePen();
  canvas.SelectWhiteBrush();

  chart.DrawXGrid(fixed_half, fs.Altitude.x_min,
                  ChartLook::STYLE_THINDASHPAPER,
                  fixed_half, true);
  chart.DrawYGrid(Units::ToSysAltitude(fixed(1000)),
                  fixed_zero, ChartLook::STYLE_THINDASHPAPER, fixed(1000), true);
  chart.DrawLineGraph(fs.Altitude, ChartLook::STYLE_MEDIUMBLACK);

  chart.DrawTrend(fs.Altitude_Base, ChartLook::STYLE_BLUETHIN);
  chart.DrawTrend(fs.Altitude_Ceiling, ChartLook::STYLE_BLUETHIN);

  chart.DrawXLabel(_T("t"), _T("hr"));
  chart.DrawYLabel(_T("h"), Units::GetAltitudeName());
}

void
RenderSpeed(Canvas &canvas, const PixelRect rc,
            const ChartLook &chart_look,
            const FlightStatistics &fs,
            const NMEAInfo &nmea_info,
            const DerivedInfo &derived_info,
            const TaskManager &task)
{
  ChartRenderer chart(chart_look, canvas, rc);

  if ((fs.Task_Speed.sum_n < 2) || !task.CheckOrderedTask()) {
    chart.DrawNoData();
    return;
  }

  chart.ScaleXFromData(fs.Task_Speed);
  chart.ScaleYFromData(fs.Task_Speed);
  chart.ScaleYFromValue(fixed_zero);
  chart.ScaleXFromValue(fs.Task_Speed.x_min + fixed_one); // in case no data
  chart.ScaleXFromValue(fs.Task_Speed.x_min);

  DrawLegs(chart, task, nmea_info, derived_info, true);

  chart.DrawXGrid(fixed_half, fs.Task_Speed.x_min,
                  ChartLook::STYLE_THINDASHPAPER, fixed_half, true);
  chart.DrawYGrid(Units::ToSysTaskSpeed(fixed_ten),
                  fixed_zero, ChartLook::STYLE_THINDASHPAPER, fixed(10), true);
  chart.DrawLineGraph(fs.Task_Speed, ChartLook::STYLE_MEDIUMBLACK);
  chart.DrawTrend(fs.Task_Speed, ChartLook::STYLE_BLUETHIN);

  chart.DrawXLabel(_T("t"), _T("hr"));
  chart.DrawYLabel(_T("h"), Units::GetTaskSpeedName());
}
