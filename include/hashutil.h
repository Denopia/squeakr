/*
 * =====================================================================================
 *
 *       Filename:  hashutil.h
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/18/2016 04:49:32 PM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Prashant Pandey (ppandey@cs.stonybrook.edu)
 *                  Rob Patro (rob.patro@cs.stonybrook.edu)
 *                  Rob Johnson (rob@cs.stonybrook.edu)
 *   Organization:  Stony Brook University
 *
 * =====================================================================================
 */

#ifndef _HASHUTIL_H_
#define _HASHUTIL_H_

#include <sys/types.h>
#include <stdlib.h>
#include <stdint.h>

class HashUtil {
	public:
		static uint64_t MurmurHash64B(const void * key, int len, unsigned int
																	seed);
		static uint64_t MurmurHash64A(const void * key, int len, unsigned int
																	seed);

		static uint64_t hash_64(uint64_t key, uint64_t mask);
		static uint64_t hash_64i(uint64_t key, uint64_t mask);

	private:
		HashUtil();
};

#endif  // #ifndef _HASHUTIL_H_


