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

#ifndef MESSAGE_H
#define MESSAGE_H

#include <cstdint>
#include <vector>
#include <memory>
#include "ResourceRecord.h"

namespace ro1617 {

    class Message {
    public:
        Message();

        auto qdcount() const noexcept {
            if (qname != nullptr)
                return 1;
            else
                return 0;
        }

        auto ancount() const noexcept {
            return answers.size();
        }

        auto nscount() const noexcept {
            return nsrecords.size();
        }

        auto adcount() const noexcept {
            return aditonalrecords.size();
        }

        auto setQuestion(std::unique_ptr<DomainName> name, RRType type) {
            qname = std::move(name);
            qtype = type;
            qclass = IN;

            flags.QR = 0; // This is a question
            flags.OPCODE = 0; // Standard question, for sure
        }

        auto setQuestion(const DomainName& name, RRType type) {
            return setQuestion(std::make_unique<DomainName>(name), type);
        }

        auto getQuestionName() const {
            if (qdcount() == 0)
                throw std::underflow_error("There is no question to be returned");

            return *qname;
        }

        auto getQuestionType() const noexcept {
            return qtype;
        }

        const auto& getAnswers() const noexcept {
            return answers;
        }

        const auto& getNSRecords() const noexcept {
            return nsrecords;
        }

        const auto& getAdRecords() const noexcept {
            return aditonalrecords;
        }

        auto rd() const noexcept {
            return flags.RD;
        }

        auto rd(bool enabled) noexcept {
            return flags.RD = enabled;
        }

        auto rcode() const noexcept {
            return flags.RCODE;
        }

        auto ra() const noexcept {
            return flags.RA;
        }

        auto ndanswers() const noexcept {
            return answers.size();
        }

        /*
         * buffer must point to a previously allocated memory region
         * of size equal or greater than len.
         */ 
        uint8_t *toByteArray(uint8_t *buffer, size_t len) const;
        size_t getRequiredBufferSize() const noexcept;

    private:
        uint16_t id;

        struct {
            unsigned int QR : 1;
            unsigned int OPCODE : 4;
            unsigned int AA : 1;
            unsigned int TC : 1;
            unsigned int RD : 1;
            unsigned int RA : 1;
            unsigned int Z : 3;
            unsigned int RCODE : 4;

            uint16_t asint16() const noexcept {
                return RCODE | Z << 4 | RA << 7 | RD << 8 | TC << 9
                        | AA << 10 | OPCODE << 11 | QR << 15;
            }

            void fromint16(uint16_t value) noexcept {
                RCODE = value & 0x000F;
                Z = (value & 0x0070) >> 4;
                RA = (value & 0x0080) >> 7;
                RD = (value & 0x0100) >> 8;
                TC = (value & 0x0200) >> 9;
                AA = (value & 0x0400) >> 10;
                OPCODE = (value & 0x7800) >> 11;
                QR = (value & 0x8000) >> 15;
            }
        } flags;

        std::unique_ptr<DomainName> qname = nullptr;
        RRType qtype;
        RRClass qclass;

        std::vector<std::unique_ptr<ResourceRecord>> answers;
        std::vector<std::unique_ptr<ResourceRecord>> nsrecords;
        std::vector<std::unique_ptr<ResourceRecord>> aditonalrecords;

        friend Message makeMessage(const uint8_t *cbegin, const uint8_t *cend);
    };

    /*
     * cbegin must point to the first byte of the message and cend
     * to one bytes past the end of the message
     */
    Message makeMessage(const uint8_t *cbegin, const uint8_t *cend);
}

#endif /* MESSAGE_H */
