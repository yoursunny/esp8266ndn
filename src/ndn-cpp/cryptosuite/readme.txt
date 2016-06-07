Cryptosuite - A cryptographic library for Arduino
Copyright (C) 2010 Peter Knight (Cathedrow)

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.


SHA implements secure hash functions that can be used for cryptography, data integrity and security purposes.

Sha covers the following standards:
	SHA-1 (FIPS 180-2)
	SHA-256 (FIPS 180-2)
	HMAC-SHA-1 (FIPS 198a)
	HMAC-SHA-256 (FIPS 198a)

What is a hash function?
	A hash function takes a message, and generates a number.
	A good hash function has the following properties:
		The number is large enough that you will never find two messages with the same number (a 'collision')
		It is computationally unfeasible to extract message information from its hash (without trying every possible combination)
		A small (1 bit) change in the message will produce a huge (approximately half of all bits) change in the hash.
		Fast to calculate

	SHA is slower than simple hashes (eg. parity), but has very high security - high enough to be used in currency transactions and confidential documents.
	SHA-1 is currently secure, but there is some suggestion it may not be for much longer.
	SHA-256 is slightly slower, but has higher security.

What is an HMAC?
	HMACs are Hashed Message Authentication Codes. Using them, it is possible to prove that you have a secret key without actually disclosing it.
