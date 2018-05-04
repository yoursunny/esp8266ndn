/* -*- Mode:C++; c-file-style:"gnu"; indent-tabs-mode:nil -*- */
/**
 * Copyright (C) 2016-2018 Regents of the University of California.
 * @author: Jeff Thompson <jefft0@remap.ucla.edu>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version, with the additional exemption that
 * compiling, linking, and/or using OpenSSL is allowed.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 * A copy of the GNU Lesser General Public License is in the file COPYING.
 */

#ifndef NDN_DELEGATION_SET_LITE_HPP
#define NDN_DELEGATION_SET_LITE_HPP

#include "name-lite.hpp"
#include "../c/delegation-set-types.h"

namespace ndn {

class DelegationSetLite {
public:
  class Delegation : private ndn_DelegationSet_Delegation {
  public:
    /**
     * Create a DelegationSetLite::Delegation with the pre-allocated
     * nameComponents, and defaults for all the values.
     * @param nameComponents The pre-allocated array of ndn_NameComponent.
     * Instead of an array of NameLite::Component, this is an array of the
     * underlying ndn_NameComponent struct so that it doesn't run the default
     * constructor unnecessarily.
     * @param maxNameComponents The number of elements in the allocated
     * nameComponents array.
     */
    Delegation(ndn_NameComponent* nameComponents, size_t maxNameComponents);

    /**
     * Get the preference number.
     * @return The preference number.
     */
    int
    getPreference() const { return preference; }

    /**
     * Get the delegation name.
     * @return The delegation name.
     */
    NameLite&
    getName() { return NameLite::downCast(name); }

    const NameLite&
    getName() const { return NameLite::downCast(name); }

    void
    setPreference(int preference) { this->preference = preference; }

    /**
     * Set this delegation to have the values from the other delegation.
     * @param other The other DelegationSetLite::Delegation to get values from.
     * @return 0 for success, or an error code if there is not enough room in
     * this object's keyName components array.
     */
    ndn_Error
    set(const Delegation& other);

    /**
     * Downcast the reference to the ndn_DelegationSet_Delegation struct to a
     * DelegationSetLite::Delegation.
     * @param delegation A reference to the ndn_DelegationSet_Delegation struct.
     * @return The same reference as DelegationSetLite::Delegation.
     */
    static Delegation&
    downCast(ndn_DelegationSet_Delegation& delegation)
    {
      return *(Delegation*)&delegation;
    }

    static const Delegation&
    downCast(const ndn_DelegationSet_Delegation& delegation)
    {
      return *(Delegation*)&delegation;
    }

  private:
    // Declare friends who can downcast to the private base.
    friend class Tlv0_2WireFormatLite;

    /**
     * Don't allow the copy constructor. Instead use set(const Delegation&)
     * which can return an error if there is no more room in the name components
     * array.
     */
    Delegation(const Delegation& other);

    /**
     * Don't allow the assignment operator. Instead use set(const Delegation&)
     * which can return an error if there is no more room in the name components
     * array.
     */
    Delegation& operator=(const Delegation& other);
  };
};

}

#endif
