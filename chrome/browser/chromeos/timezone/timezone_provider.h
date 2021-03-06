// Copyright 2014 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_CHROMEOS_TIMEZONE_TIMEZONE_PROVIDER_H_
#define CHROME_BROWSER_CHROMEOS_TIMEZONE_TIMEZONE_PROVIDER_H_

#include "base/macros.h"
#include "base/memory/ref_counted.h"
#include "base/memory/scoped_ptr.h"
#include "base/memory/scoped_vector.h"
#include "base/memory/weak_ptr.h"
#include "base/threading/thread_checker.h"
#include "base/time/time.h"
#include "chrome/browser/chromeos/timezone/timezone_request.h"
#include "url/gurl.h"

namespace net {
class URLRequestContextGetter;
}

namespace content {
struct Geoposition;
}

namespace chromeos {

// This class implements Google TimeZone API.
//
// Note: this should probably be a singleton to monitor requests rate.
// But as it is used only from WizardController, it can be owned by it for now.
class TimeZoneProvider {
 public:
  TimeZoneProvider(net::URLRequestContextGetter* url_context_getter,
                   const GURL& url);
  virtual ~TimeZoneProvider();

  // Initiates new request (See TimeZoneRequest for parameters description.)
  void RequestTimezone(const content::Geoposition& position,
                       bool sensor,
                       base::TimeDelta timeout,
                       TimeZoneRequest::TimeZoneResponseCallback callback);

 private:
  // Deletes request from requests_.
  void OnTimezoneResponse(TimeZoneRequest* request,
                          TimeZoneRequest::TimeZoneResponseCallback callback,
                          scoped_ptr<TimeZoneResponseData> timezone,
                          bool server_error);

  scoped_refptr<net::URLRequestContextGetter> url_context_getter_;
  const GURL url_;

  // Requests in progress.
  // TimeZoneProvider owns all requests, so this vector is deleted on destroy.
  ScopedVector<TimeZoneRequest> requests_;

  // Creation and destruction should happen on the same thread.
  base::ThreadChecker thread_checker_;

  DISALLOW_COPY_AND_ASSIGN(TimeZoneProvider);
};

}  // namespace chromeos

#endif  // CHROME_BROWSER_CHROMEOS_TIMEZONE_TIMEZONE_PROVIDER_H_
