// Copyright CERN and copyright holders of ALICE O2. This software is
// distributed under the terms of the GNU General Public License v3 (GPL
// Version 3), copied verbatim in the file "COPYING".
//
// See http://alice-o2.web.cern.ch/license for full licensing information.
//
// In applying this license CERN does not waive the privileges and immunities
// granted to it by virtue of its status as an Intergovernmental Organization
// or submit itself to any jurisdiction.
#ifndef O2_EPN_OUTPUTCONNECTION_H
#define O2_EPN_OUTPUTCONNECTION_H


#include <O2/Balancer/Devices/Connection.h>

namespace O2 {
    namespace EPN {

        class EPNSettings;

        class OutputConnection : public Balancer::Connection {
        public:
            OutputConnection(Balancer::AbstractDevice *device,
                             std::shared_ptr<EPNSettings> settings);
        };
    } // namespace EPN
} // namespace O2

#endif // O2_EPN_OUTPUTCONNECTION_H
