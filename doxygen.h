#ifndef DOXYGEN_H
#define DOXYGEN_H

#error "doxygen.h must not be included in any file!"

/* This header is only for doxygen comments. It should never be included in any
 * source file. */

/**
 * \defgroup cfc cfc
 * \details
 * This is implementation of Comma Free Coding algorithm of storing binary number.
 */

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
 * \defgroup lz77 lz77
 * \details
 * This is a reference Lempel Ziv 77 algrorithm implementation.
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
 * 1. For each chunk of \ref BLOCK_SIZE in file to encode (last chunk can be
 * less then \ref BLOCK_SIZE).
 *   1. If first chunk, write CFC(WINDW_SIZE), than window size of bits
 * of data.
 *   2. For each subsequent bit
 *     1. If skipped bits equals to log2(WINDOW_SIZE) then write pair
 * (CFC(log2(WINDOW_SIZE)),skipped_bits)
 *     2. If largest found match in previous WINDOW_SIZE bits is more than
 * log2(WINDOW_SIZE), then write uncompressed data
 * (CFC(count(ckipped_bits), skipped_bits)) and write pair
 * (CFC(length_of_match),offset_of_match)
 *     3. Else save bit into skipped_bits and continue.
 */

/**
 * \page testing Testing
 * \tableofcontents
 * All tests are located in tests directory. They are written in bash and are
 * intended to be run from tests directory.
 * \section s1 Correctness tests
 * Currently only one basic sanity testing is implemented.
 * 
 * Sanity test compress each file in tests/files directory using LZ77 and LZ77
 * EAC algorithms. After compressing, each file is decompressed and checked
 * against original file.
 * 
 * To test run sanity.sh script.
 * \section s2 Performance tests
 * \subsection s21 Ratio comparing tests
 * \subsection s22 Time comparing tests
 */

#endif
