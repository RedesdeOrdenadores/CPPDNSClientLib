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

#include "AResourceRecord.h"    

#include <string.h>

namespace ro1617 {

    AResourceRecord::AResourceRecord(const DomainName& domain, const uint8_t *rdata_cbegin, const uint8_t *rdata_cend, uint32_t ttl, uint16_t rdlength) :
    ResourceRecord(domain, A, ttl, rdlength) {
        if (rdlength != 4 || rdata_cend - rdata_cbegin != rdlength)
            throw std::runtime_error("Incorrect RDData or RDLength for A RR");

        memcpy(&address_, rdata_cbegin, rdlength);
    }

    uint8_t *AResourceRecord::toByteArray(uint8_t *buffer, size_t len) const {
        const auto *cend = buffer + len;
        buffer = commonToByteArray(buffer, len);

        if (buffer + 4 > cend) {
            throw std::overflow_error("Buffer too short to hold RRDATA");
        }

        memcpy(buffer, &address_, 4);

        return buffer + 4;
    }
}