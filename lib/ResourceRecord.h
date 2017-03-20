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

#ifndef RESOURCERECORD_H
#define RESOURCERECORD_H

#include "DomainName.h"
#include <memory>

namespace ro1617 {

    enum RRType : uint16_t {
        A = 1, // a host address
        NS = 2, // an authoritative name server
        CNAME = 5, // the canonical name for an alias
        SOA = 6, // marks the start of a zone of authority
        PTR = 12, // a domain name pointer
        HINFO = 13, // host information
        MX = 15, // mail exchange
        TXT = 16, // text strings
        AAAA = 28, // an IPv6 host address
    };

    enum RRClass : uint16_t {
        IN = 1, // the Internet
        CH = 3, // the CHAOS class
        HS = 4, // Hesiod        
    };

    class ResourceRecord {
    protected:

        ResourceRecord(const DomainName& domain, RRType type, uint32_t ttl, uint16_t rdlength) : domain_(domain), type_(type), ttl_(ttl), rdlength_(rdlength) {
        }

        uint8_t *commonToByteArray(uint8_t *buffer, size_t len) const;

        size_t commonRequiredBufferSize() const noexcept {
            return domain_.getRequiredBufferSize() + sizeof (RRType) + sizeof (RRClass) + sizeof (ttl_) + 2 /* rdlength */;
        }

    public:
        virtual uint8_t *toByteArray(uint8_t *buffer, size_t len) const = 0;
        virtual size_t getRequiredBufferSize() const noexcept = 0;
        auto rrtype() const noexcept {
            return type_;
        }

    private:
        DomainName domain_;
        const enum RRType type_;
        static constexpr enum RRClass rrclass_ = IN;
        const uint32_t ttl_;
        const uint16_t rdlength_;
    };

    std::unique_ptr<ResourceRecord> makeResourceRecord(const uint8_t *cbegin, const uint8_t *cend, const uint8_t *message = nullptr, const uint8_t **lastbyte = nullptr);
}

#endif /* RESOURCERECORD_H */

