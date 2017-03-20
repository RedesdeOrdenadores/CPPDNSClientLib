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

#include "DomainName.h"
#include <stdexcept>

namespace ro1617 {

    uint8_t *DomainName::toByteArray(uint8_t *buffer, size_t len) const {
        if (len < getRequiredBufferSize()) {
            throw std::overflow_error("Insuficient buffer size to hold domain name.");
        }
        for (const auto& s : labels) {
            if (s.size() > 255) {
                throw std::overflow_error("Domain labels must be shorter than 256 characteres.");
            }
            *buffer++ = static_cast<uint8_t> (s.size());
            std::copy(s.cbegin(), s.cend(), buffer);
            buffer += s.size();
        }
        *buffer++ = 0;
        
        return buffer;
    }

    DomainName::DomainName(const uint8_t *cbegin, const uint8_t *cend, const uint8_t *cache, const uint8_t **lastbyte) {        
        auto lastbyte_updated = false;
        
        while (true) {
            uint8_t size = *cbegin++;
            if (size == 0)
                break;
            
            // Check compression 
            if ((size & 0xc0) == 0xc0) {
                uint16_t offset = ((size & 0x37) << 8) | *cbegin++;
                if (lastbyte != nullptr && lastbyte_updated == false) {
                    *lastbyte = cbegin;
                    lastbyte_updated = true;
                }
                
                cbegin = cache + offset;
                if (cbegin >= cend || cache == nullptr) {
                    throw std::underflow_error("Incorrect compression in data.");
                }                                
                                
                continue;
            }
            
            if (cbegin >= cend || cbegin + size > cend) {
                throw std::underflow_error("Insuficent data to get all labels.");
            }
            
            std::string label;
            std::copy(cbegin, cbegin + size, std::back_inserter(label));
            labels.push_back(label);
            cbegin += size;
        }
        
        if (lastbyte != nullptr && lastbyte_updated == false)  // In case cbegin goes backwards because of compression
            *lastbyte = std::max(cbegin, *lastbyte);
    }
}