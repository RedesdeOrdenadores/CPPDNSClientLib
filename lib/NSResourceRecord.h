/*
 * dnsclient: A simple client for teachiog DNS
 * Ⓒ 2016 Miguel Rodríguez Pérez <miguel@det.uvigo.gal>
 */

/* 
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

#ifndef NSRESOURCERECORD_H
#define NSRESOURCERECORD_H

#include "ResourceRecord.h"
#include "DomainName.h"

namespace ro1617 {

    class NSResourceRecord : public ResourceRecord {
    public:

        NSResourceRecord(const DomainName& domain, const DomainName& rrdata, uint32_t ttl) : ResourceRecord(domain, NS, ttl, rrdata.getRequiredBufferSize()), nsdomain_(rrdata) {
        }

        NSResourceRecord(const DomainName& domain, const uint8_t *rdata_cbegin, const uint8_t *rdata_cend, uint32_t ttl, uint16_t rdlength, const uint8_t *cache);

        uint8_t *toByteArray(uint8_t *buffer, size_t len) const override;

        size_t getRequiredBufferSize() const noexcept override {
            return 16 + commonRequiredBufferSize();
        }

    private:
        DomainName nsdomain_;
    };
}

#endif /* NSRESOURCERECORD_H */

