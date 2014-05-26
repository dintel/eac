#ifndef DOXYGEN_H
#define DOXYGEN_H

#error "doxygen.h must not be included in any file!"

/* This header is only for doxygen comments. It should never be included in any
 * source file. */

/**
 * \defgroup bit_string bit_string
 * \details
 * This is implementation of bit string. Internally they store data using array
 * of bytes, but provide interface for easy access and manipulation of bits.
 */

/**
 * \defgroup bit_string_writer bit_string_writer
 * \details
 * Writer that allows writing of bit strings into file. Since file abstraction
 * layer does not allow writing less than a byte into file, writer has buffer
 * where it keeps unwritten bits. When bit string writer is closed, bits in
 * buffer are padded with zeroes and written to file.
 */

/**
 * \defgroup block block
 * \details
 * Block represents a single block of data from file. It includes data, best
 * compressed variant of data and best window size. It also includes a mutex
 * that is used for thread safety and pointers to next and previous blocks
 * (so all blocks form a double linked list)
 */

/**
 * \defgroup cfc cfc
 * \details
 * This is implementation of Comma Free Coding algorithm of storing binary number.
 */

/**
 * \defgroup delta_nw delta_nw
 * \details
 * This module is responsible for encoding/decoding window size changes between
 * blocks. Currently window size change is codes simply by encoding log2 of new
 * window size between blocks. More efficient way would be to code change of
 * this log2.
 */

/**
 * \defgroup lz77 lz77
 * \details
 * This is a reference Lempel Ziv 77 algrorithm implementation.
 */

/**
 * \defgroup queue queue
 * \details
 * Queue is a queue of compression jobs. Each job tries to compress block of
 * data using some window size. After compressing block is updated with new
 * results. Jobs run asynchronously and number of max threads can be specified
 * when running queue.
 */

/**
 * \mainpage
 *
 * \author Dima Zbarski <dmitry.zbarski@gmail.com> 
 * 
 * \details 
 * EAC - Adaptive Entropy Coder
 * 
 * This is a test implementation of algorithm that tries to adapt itself to
 * changing entropy of it's source. It does it by retesting compression ratio
 * after each coded block and adjusting it's coding window. For more information
 * see \ref algorithms
 *
 * For testing information see \ref testing<br />
 * For algorithm details see \ref algorithms
 */

/**
 * \page algorithms Algorithms details
 * \tableofcontents
 * \section cfcsect Comma Free Coding
 * Comma Free Coding - CFC. This is a way to write arbitrary length number in
 * binary form to allow it to be easily decoded. This algorithm is used in \ref
 * lz77sect.
 *
 * Algorithm for CFC encoding works as follows (input is k):
 * 1. Let l = count_bits(k)
 * 2. Let l_len = count_bits(l)
 * 3. Write l_len-1 0's and 1 after them
 * 4. Write l
 * 5. Write k
 *
 * Algorithm for CFC decoding works as follows:
 * 1. Read bits until first 1 was read. Store number of bits read in l_len
 * 2. Read another l_len bits into l (l is now l_len bits number)
 * 3. Read another l_len bits and return as number
 * \section lz77sect Lempel Ziv 77
 * Lempel Ziv 77 algorithm is a sliding window compression algorithm. For a
 * details about LZ77 see
 * <a href="http://en.wikipedia.org/wiki/LZ77_and_LZ78" target="_blank">LZ77 and
 * LZ78 on wikipedia</a>. 
 *
 * Our implementation uses \ref cfcsect. Algorithm is working as follows:
 * 1. For each chunk of \ref \f$BLOCK\_SIZE\f$ in file to encode (last chunk can be
 * less then \ref \f$BLOCK\_SIZE\f$).
 *   1. If first chunk, write \f$CFC(WINDW\_SIZE)\f$ and than \f$WINDOW\_SIZE\f$ of bits
 * of data.
 *   2. For each subsequent bit
 *     1. If skipped bits equals to \f$\log_2(WINDOW\_SIZE)\f$ then write pair
 * \f$(CFC(\log_2(WINDOW\_SIZE)),skipped\_bits)\f$
 *     2. If largest found match in previous \f$WINDOW\_SIZE\f$ bits is more than
 * \f$\log_2(WINDOW\_SIZE)\f$, then write uncompressed data
 * \f$(CFC(count(skipped\_bits)), skipped\_bits)\f$ and write pair
 * \f$(CFC(length\_of\_match),offset\_of\_match)\f$
 *     3. Else save bit into \f$skipped\_bits\f$ and continue.
 * \section eacsect EAC - Entropy adaptive coding
 * Entropy adaptive coding algorithm splits file into blocks of data. Each block
 * is then compressed separately using any available window size with LZ77
 * algorithm to choose window size that provides most efficient compression
 * ratio. To prevent loss of training sequence (first sliding window),
 * initial sliding window of each block lies in previous block (except first
 * block).
 * 
 * Since window size of each block can be different from previous block, change
 * of window size is coded between blocks (we code \f$\log_2(window\ size)\f$).
 *
 * It's worth to note that unlike LZ77, EAC compression can be parallelized
 * because each block can be compressed separately without compressing previous
 * block. Further all window sizes compression tests can also be parallelized.
 */

/**
 * \page testing Testing
 * \tableofcontents
 * All tests are located in tests directory. They are written in bash and are
 * intended to be run from tests directory.
 * \section s1 Correctness tests
 * Currently only one basic sanity testing is implemented.
 * 
 * Sanity test compresses each file in tests/files directory (limited to sizes
 * from 32K to 50K)  using LZ77 and LZ77 EAC algorithms. After compressing, each
 * file is decompressed and checked against original file.
 * 
 * To test run sanity.sh script.
 * \section s2 Performance test
 * Additionally there is a performance.sh test that runs on all files in
 * tests/files and tries to compress them with LZ77 with every possible window
 * size and than LZ77 with EAC with every possible block size.
 *
 * During these tests each file is also decompressed and the result is checked
 * against original file. During compress and decompress time is measured and
 * later saved into results file.
 *
 * For each compress/decompress cycle following results are saved:
 * - Filename
 * - Block size (0 if not relevant)
 * - Window size (0 if not relevant)
 * - Algorithm (lz77 or eac)
 * - Encode time
 * - Decode time
 * - Result (SUCCESS if decoded file matched original, FAILURE otherwise)
 * - File size (original file size)
 * - Compressed size (compressed file size)
 * - Ratio (\f$= \frac{Original}{Compressed}\f$)
 *
 * All results are aggregated to result.csv file which is semicolon(;)
 * separated. This file can easily be imported into Excel to analyze
 * results. But another option to view result.csv graphically is to run
 * jsonReport.php that converts result.csv into JSON format and saves it into
 * viewer directory. There it can be viewed using index.html file.
 */

#endif
