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

#include "ResourceRecord.h"
#include "AResourceRecord.h"
#include "AAAAResourceRecord.h"
#include "NSResourceRecord.h"

#include <stdexcept>
#include <string.h>

namespace ro1617 {

    uint8_t *ResourceRecord::commonToByteArray(uint8_t *buffer, size_t len) const {
        const auto *cend = buffer + len;
        const uint16_t type = htons(type_);
        const uint16_t rrclass = htons(rrclass_);
        const uint32_t ttl = htonl(ttl_);
        const uint16_t rdlength = htons(rdlength_);

        if (buffer + commonRequiredBufferSize() > cend) {
            throw std::overflow_error("Buffer too short to hold RR");
        }

        buffer = domain_.toByteArray(buffer, len);
        memcpy(buffer, &type, 2);
        buffer += 2;
        memcpy(buffer, &rrclass, 2);
        buffer += 2;
        memcpy(buffer, &ttl, 4);
        buffer += 4;
        memcpy(buffer, &rdlength, 2);

        return buffer + 2;
    }

    std::unique_ptr<ResourceRecord> makeResourceRecord(const uint8_t *cbegin, const uint8_t *cend, const uint8_t *message, const uint8_t **lastbyte) {
        RRType type;
        RRClass rrclass;
        uint32_t ttl;
        uint16_t rdlength;
        auto buffer = cbegin;

        DomainName domain(buffer, cend, message, &buffer);

        if (buffer + 2 + 2 + 4 + 2 > cend) {
            throw std::underflow_error("Insuficient data to construct RR");
        }
        memcpy(&type, buffer, 2);
        memcpy(&rrclass, buffer + 2, 2);
        memcpy(&ttl, buffer + 4, 4);
        memcpy(&rdlength, buffer + 8, 2);
        type = static_cast<RRType> (ntohs(type));
        rrclass = static_cast<RRClass> (ntohs(rrclass));
        ttl = ntohl(ttl);
        rdlength = ntohs(rdlength);

        buffer += 2 + 2 + 4 + 2;
        if (buffer + rdlength > cend) {
            throw std::underflow_error("Insuficient data to construct RR");
        }
        if (rrclass != IN) {
            throw std::runtime_error("Unsupported RR class");
        }

        if (lastbyte != nullptr)
            *lastbyte = buffer + rdlength;

        switch (type) {
            case A:
                return std::make_unique<AResourceRecord>(AResourceRecord(domain, buffer, buffer + rdlength, ttl, rdlength));
                break;
            case AAAA:
                return std::make_unique<AAAAResourceRecord>(AAAAResourceRecord(domain, buffer, buffer + rdlength, ttl, rdlength));
                break;
            case NS:
                return std::make_unique<NSResourceRecord>(NSResourceRecord(domain, buffer, buffer + rdlength, ttl, rdlength, message));
                break;
            default:
                throw std::runtime_error("Unsupported RRType");
        }
    }

}