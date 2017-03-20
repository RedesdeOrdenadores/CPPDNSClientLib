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

#include "Message.h"

#include <fstream>
#include <arpa/inet.h>
#include <iostream>

namespace ro1617 {

    Message::Message() {

        union {
            uint16_t gen_id;
            char bytes[2];
        } mixed_id;
        std::ifstream random_source("/dev/urandom");

        random_source.read(mixed_id.bytes, 2);
        id = mixed_id.gen_id;

        flags.OPCODE = flags.RCODE = flags.QR = flags.AA = flags.RA = flags.RD = flags.TC = flags.Z = 0;
    }

    size_t Message::getRequiredBufferSize() const noexcept {
        size_t size = 12; // Header size

        if (qdcount()) {
            size += qname->getRequiredBufferSize() + 2 + 2;
        }

        for (const auto& an : answers) {
            size += an->getRequiredBufferSize();
        }

        for (const auto& ns : nsrecords) {
            size += ns->getRequiredBufferSize();
        }

        for (const auto& ad : aditonalrecords) {
            size += ad->getRequiredBufferSize();
        }

        return size;
    }

    uint8_t *Message::toByteArray(uint8_t *buffer, size_t len) const {
        const auto *cend = buffer + len;

        if (len < getRequiredBufferSize())
            throw std::overflow_error("Not enough capacity in buffer for message");

        memcpy(buffer, &id, 2);
        buffer += 2;
        uint16_t rflags;
        rflags = htons(flags.asint16());
        memcpy(buffer, &rflags, 2);
        buffer += 2;

        const uint16_t qcount = htons(qdcount()),
                acount = htons(ancount()),
                ncount = htons(nscount()),
                dcount = htons(adcount());

        memcpy(buffer, &qcount, 2);
        buffer += 2;
        memcpy(buffer, &acount, 2);
        buffer += 2;
        memcpy(buffer, &ncount, 2);
        buffer += 2;
        memcpy(buffer, &dcount, 2);
        buffer += 2;

        if (qdcount()) {
            buffer = qname->toByteArray(buffer, cend - buffer);
            const uint16_t nqtype = htons(qtype),
                    nqclass = htons(qclass);
            memcpy(buffer, &nqtype, 2);
            buffer += 2;
            memcpy(buffer, &nqclass, 2);
            buffer += 2;
        }

        for (const auto& an : answers) {
            buffer = an->toByteArray(buffer, cend - buffer);
        }

        for (const auto& ns : nsrecords) {
            buffer = ns->toByteArray(buffer, cend - buffer);
        }

        for (const auto& ad : aditonalrecords) {
            buffer = ad->toByteArray(buffer, cend - buffer);
        }

        return buffer;
    }

    Message makeMessage(const uint8_t *cbegin, const uint8_t *cend) {
        Message msg;
        auto buffer = cbegin;

        if (cend - cbegin < 12)
            throw std::underflow_error("Not enough bytes for the header.");

        uint16_t rflags;

        memcpy(&msg.id, buffer, 2);
        buffer += 2;
        memcpy(&rflags, buffer, 2);
        msg.flags.fromint16(ntohs(rflags));
        buffer += 2;

        if (msg.flags.TC)
            throw std::runtime_error("Cannot handle truncated responses yet.");

        uint16_t qcount, acount, ncount, dcount;
        memcpy(&qcount, buffer, 2);
        qcount = ntohs(qcount);
        buffer += 2;
        memcpy(&acount, buffer, 2);
        acount = ntohs(acount);
        buffer += 2;
        memcpy(&ncount, buffer, 2);
        ncount = ntohs(ncount);
        buffer += 2;
        memcpy(&dcount, buffer, 2);
        dcount = ntohs(dcount);
        buffer += 2;

        if (qcount > 1) {
            throw std::runtime_error("We do not support messages with more than 1 query.");
        }

        if (qcount == 1) {
            msg.qname = std::make_unique<DomainName>(buffer, cend, cbegin, &buffer);
            uint16_t nqtype, nqclass;
            memcpy(&nqtype, buffer, 2);
            nqtype = ntohs(nqtype);
            memcpy(&msg.qtype, &nqtype, 2);
            buffer += 2;
            memcpy(&nqclass, buffer, 2);
            nqclass = ntohs(nqclass);
            memcpy(&msg.qclass, &nqclass, 2);
            buffer += 2;
        }

        if (buffer > cend)
            throw std::underflow_error("Not enough data in buffer");

        for (auto i = 0; i < acount; i++) {
            try {
                msg.answers.push_back(makeResourceRecord(buffer, cend, cbegin, &buffer));
            } catch (const std::runtime_error& e) {
                std::cerr << "Ignoring RR because: " << e.what() << '\n';
            }
        }
        for (auto i = 0; i < ncount; i++) {
            try {
                msg.nsrecords.push_back(makeResourceRecord(buffer, cend, cbegin, &buffer));
            } catch (const std::runtime_error& e) {
                std::cerr << "Ignoring RR because: " << e.what() << '\n';
            }
        }
        for (auto i = 0; i < dcount; i++) {
            try {
                msg.aditonalrecords.push_back(makeResourceRecord(buffer, cend, cbegin, &buffer));
            } catch (const std::runtime_error& e) {
                std::cerr << "Ignoring RR because: " << e.what() << '\n';
            }
        }

        return msg;
    }
}
