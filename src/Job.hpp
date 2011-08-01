/*
Copyright_License {

  XCSoar Glide Computer - http://www.xcsoar.org/
  Copyright (C) 2000-2011 The XCSoar Project
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

#ifndef XCSOAR_JOB_HPP
#define XCSOAR_JOB_HPP

class OperationEnvironment;

/**
 * Base class for a job that can be offloaded into a separate thread.
 */
class Job {
public:
  virtual void Run(OperationEnvironment &env) = 0;
};

/**
 * An environment that can run one or more jobs.
 */
class JobRunner {
public:
  /**
   * @return true if the job has finished (may have failed), false if
   * the job was cancelled by the user
   */
  virtual bool Run(Job &job) = 0;
};

#endif