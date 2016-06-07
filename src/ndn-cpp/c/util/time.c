/**
 * Copyright (C) 2013-2016 Regents of the University of California.
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version, with the additional exemption that
 * compiling, linking, and/or using OpenSSL is allowed.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * A copy of the GNU Lesser General Public License is in the file COPYING.
 */

#include <ndn-cpp/ndn-cpp-config.h>
#if NDN_CPP_HAVE_TIME_H
#include <time.h>
#endif
#if NDN_CPP_HAVE_SYS_TIME_H
#include <sys/time.h>
#endif
#if defined(_WIN32)
#include <Windows.h>
#endif
#include <math.h>
#include <string.h>
#include <stdio.h>
#include "time.h"

ndn_MillisecondsSince1970
ndn_getNowMilliseconds()
{
#if defined(_WIN32)
  // From http://stackoverflow.com/questions/1676036/what-should-i-use-to-replace-gettimeofday-on-windows
  static const unsigned __int64 epoch = ((unsigned __int64)116444736000000000ULL);
  FILETIME fileTime;
  ULARGE_INTEGER ularge;

  GetSystemTimeAsFileTime(&fileTime);
  ularge.LowPart = fileTime.dwLowDateTime;
  ularge.HighPart = fileTime.dwHighDateTime;
  return (double)(ularge.QuadPart - epoch) / 10000.0;
#else
  struct timeval t;
  // Note: configure.ac requires gettimeofday.
  gettimeofday(&t, 0);
  return t.tv_sec * 1000.0 + t.tv_usec / 1000.0;
#endif
}

ndn_Error
ndn_toIsoString(ndn_MillisecondsSince1970 milliseconds, char *isoString)
{
#if NDN_CPP_HAVE_GMTIME_SUPPORT
  if (milliseconds < 0)
    return NDN_ERROR_Calendar_time_value_out_of_range;
  else if (milliseconds > 2e14)
    // 2e14 is about the year 8300.  We don't want to go over a 4-digit year.
    return NDN_ERROR_Calendar_time_value_out_of_range;

  double secondsSince1970 = milliseconds / 1000.0;
  char fractionBuffer[10];
  sprintf(fractionBuffer, "%.06lf", fmod(secondsSince1970, 1.0));
  const char *fraction = strchr(fractionBuffer, '.');
  if (!fraction)
    // Don't expect this to happen.
    fraction = ".000000";

  time_t seconds = (time_t)floor(secondsSince1970);
  struct tm* gmt = gmtime(&seconds);
  sprintf(isoString, "%04d%02d%02dT%02d%02d%02d%s", 1900 + gmt->tm_year, gmt->tm_mon + 1, gmt->tm_mday,
    gmt->tm_hour, gmt->tm_min, gmt->tm_sec, fraction);

  return NDN_ERROR_success;
#else
  return NDN_ERROR_Time_functions_are_not_supported_by_the_standard_library;
#endif
}

ndn_Error
ndn_fromIsoString(const char* isoString, ndn_MillisecondsSince1970 *milliseconds)
{
#if NDN_CPP_HAVE_GMTIME_SUPPORT
  // Initialize time zone, etc.
  time_t dummyTime = 0;
  struct tm tm1 = *gmtime(&dummyTime);

  sscanf(isoString, "%4d%2d%2dT%2d%2d", &tm1.tm_year, &tm1.tm_mon, &tm1.tm_mday, &tm1.tm_hour, &tm1.tm_min);
  // Skip the time past minutes and get the float seconds.
  double seconds;
  sscanf(isoString + (4 + 2 + 2 + 1 + 2 + 2), "%lf", &seconds);

  // tm_year starts from 1900.
  tm1.tm_year -= 1900;
  // tm_mon starts from 0, not 1.
  tm1.tm_mon -= 1;
  tm1.tm_sec = 0;

  *milliseconds = (timegm(&tm1) + seconds) * 1000.0;
  return NDN_ERROR_success;
#else
  return NDN_ERROR_Time_functions_are_not_supported_by_the_standard_library;
#endif
}
