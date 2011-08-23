/* Copyright_License {

  XCSoar Glide Computer - http://www.xcsoar.org/
  Copyright (C) 2000-2010 The XCSoar Project
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

#include "Weather/METARParser.hpp"
#include "Weather/METAR.hpp"
#include "Weather/ParsedMETAR.hpp"
#include "TestUtil.hpp"

int
main(int argc, char **argv)
{
  plan_tests(7);

  METAR metar;
  ParsedMETAR parsed;

  metar.content = _T("EDDL 231050Z 31007KT 9999 FEW020 SCT130 23/18 Q1015 NOSIG");
  if (!ok1(METARParser::Parse(metar, parsed)))
    return exit_status();

  ok1(parsed.icao_code == _T("EDDL"));
  ok1(parsed.qnh_available);
  ok1(equals(parsed.qnh.get_QNH(), 1015));

  metar.content = _T("METAR KTTN 051853Z 04011KT 1/2SM VCTS SN FZFG BKN003 OVC010 M02/M02 A3006 RMK AO2 TSB40 SLP176 P0002 T10171017=");
  if (!ok1(METARParser::Parse(metar, parsed)))
    return exit_status();

  ok1(parsed.icao_code == _T("KTTN"));
  ok1(!parsed.qnh_available);

  return exit_status();
}