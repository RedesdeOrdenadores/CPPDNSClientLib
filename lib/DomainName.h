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

#ifndef DOMAINNAME_H
#define DOMAINNAME_H

#include <string>
#include <vector>
#include <sstream>
#include <iterator>
#include <algorithm>
#include <regex>

namespace ro1617 {

    class DomainName {
    public:

        DomainName(const std::string& domain) noexcept {
            std::regex ps("\\.");           
            std::copy(std::sregex_token_iterator(domain.begin(), domain.end(), ps, -1), std::sregex_token_iterator(), std::back_inserter(labels));
        }

        template<typename Iterator>
        DomainName(Iterator begin, Iterator end) noexcept : labels(begin, end)  {
        }
        
        DomainName(const uint8_t *cbegin, const uint8_t *cend, const uint8_t *cache, const uint8_t **lastbyte = nullptr);
        
        auto getRequiredBufferSize() const noexcept {
            // 1 byte for each label (its size)
            // 1 byte for termination
            // sum of labels length
            
            auto size = 1;
            size += labels.size();
            
            for (const auto& s : labels) {
                size += s.size();
            }
            
            return size;
        }
        
        auto begin() noexcept {
            return labels.begin();
        }
        
        auto end() noexcept {
            return labels.end();
        }
        
        auto cbegin() const noexcept {
            return labels.cbegin();
        }
        
        auto cend() const noexcept {
            return labels.cend();
        }
        
        uint8_t *toByteArray(uint8_t *buffer, size_t len) const;
        
    private:
        std::vector<std::string> labels;
    };        
}

#endif /* DOMAINNAME_H */

