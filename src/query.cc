/*
 * =====================================================================================
 *
 *       Filename:  kmer_query.cc
 *
 *    Description:  
 *
 *        Version:  1.0
 *        Created:  04/27/2016 08:48:26 AM
 *       Revision:  none
 *       Compiler:  gcc
 *
 *         Author:  Prashant Pandey (), ppandey@cs.stonybrook.edu
 *   Organization:  Stony Brook University
 *
 * =====================================================================================
 */
#include <iostream>
#include <algorithm>
#include <cstring>
#include <vector>
#include <set>
#include <bitset>
#include <cassert>
#include <fstream>

#include <boost/thread/thread.hpp>
#include <boost/lockfree/queue.hpp>
#include <boost/lockfree/spsc_queue.hpp>
#include <boost/atomic.hpp>

#include <time.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/resource.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <sys/mman.h>

#include "clipp.h"
#include "gqf_cpp.h"
#include "hashutil.h"
#include "chunk.h"
#include "kmer.h"

#include <zlib.h>
#include <bzlib.h>

#define BITMASK(nbits) ((nbits) == 64 ? 0xffffffffffffffff : (1ULL << (nbits)) - 1ULL)

// A=1, C=0, T=2, G=3
void getRandomKmers(int n, uint64_t range, uint32_t seed, std::vector<uint64_t>& kmers, uint32_t K)
{
	uint64_t kmer;
	for (int j = 0; j < n; j++) {
		kmer = 0;
		for (uint i = 0; i < K; i++) {
			uint8_t c = rand()%4;
			kmer = kmer | c;
			kmer = kmer << 2;
		}
		kmer = kmer >> 2;
		kmer = HashUtil::MurmurHash64A(((void*)&kmer), sizeof(kmer), seed);
		kmers.push_back(kmer%range);
	}
}

/*
 * ===  FUNCTION  ======================================================================
 *         Name:  main
 *  Description:  
 * =====================================================================================
 */
int query_main(int argc, char *argv[])
{

	std::string ds_file;
	int ksize;
	uint32_t num_query;
	int random;
	struct timeval start, end;
	struct timezone tzp;
	std::vector<uint64_t> kmers;

	using namespace clipp;
	auto cli = (
							required("-f", "--cqf-file") & value("cqf-file", ds_file) % "input CQF file",
							required("-k","--kmer") & value("k-size", ksize) % "length of k-mers to query. Must be same the as the size of counted k-mers",
							required("-n","--num-query") & value("num-query", num_query) % "number of queries",
							required("-r","--random") & value("random-queries", random) % "random queries",
							option("-h", "--help")  % "show help"
						 );

	auto res = parse(argc, argv, cli);

	if (!res) {
		std::cout << make_man_page(cli, argv[0]) << "\n";
		return 1;
	}

	srand(time(NULL));

	//Initialize the QF
	std::cout << "Reading kmers into the QF off the disk" << std::endl;
	CQF<KeyObject> cqf(ds_file, LOCKS_FORBIDDEN, FREAD);

	if (random) {
		getRandomKmers(num_query, cqf.range(), cqf.seed(), kmers, ksize);
	} else {
		CQF<KeyObject>::Iterator it = cqf.begin();
		do {
			KeyObject k = *it;
			//freq_file << key << " " << count << std::endl;
			kmers.push_back(k.key);
			++it;
		} while (!it.done());
	}

	std::cout << "Querying kmers in the QF" << std::endl;
	random_shuffle ( kmers.begin(), kmers.end() );
	uint64_t num_not_found = 0;
	gettimeofday(&start, &tzp);
	for (uint32_t i = 0; i < num_query || i < kmers.size(); i++) {
		/*std::cout << "index: " << id << std::endl;*/
		if (!cqf.query(KeyObject(kmers[i], 0, 0))) {
			num_not_found++;
			//std::cout << "Can not find the kmer: " << kmers[id] << std::endl;
			//abort();
		}
	}
	gettimeofday(&end, &tzp);
	print_time_elapsed("", &start, &end);

	std::cout << "Not find: " << num_not_found << std::endl;

	return EXIT_SUCCESS;
}				/* ----------  end of function main  ---------- */
