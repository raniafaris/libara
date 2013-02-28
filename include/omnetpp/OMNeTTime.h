/*
 * $FU-Copyright$
 */

#ifndef OMNET_TIME_H_
#define OMNET_TIME_H_

#include <omnetpp.h>
#include <cmath>

#include "Time.h"

namespace ARA {
namespace omnetpp {
    /**
     * The class provides methods for determining the time difference between 
     * two timestamps in OMNeT++.
     */
    class OMNeTTime : public Time {
        public:
          OMNeTTime();
          OMNeTTime(SimTime timestamp);

          virtual void setToCurrentTime();
          virtual Time* subtract(const Time* right) const;
          virtual long getSeconds() const;
          virtual long getMilliSeconds() const;

          SimTime getTimestamp() const;

        private:
          int convertSimulationTime(int scaleExponent) const;

          SimTime timestamp;
    };
}
}

#endif 

