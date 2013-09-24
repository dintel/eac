#include "lz77.h"

bit_string_t *lz77_encode(bit_string_t *src, size_t nw, bit_string_t *window)
{
    bit_string_t *result = bit_string_init(src->size * 4);
    size_t offset = nw;         /* Next bit to code */
    size_t first = nw;          /* First bit that was not coded yet */
    size_t lognw = ceil(log2(nw));
    size_t best_match, best_offset, current_match;
    
    if(window == NULL) {
        /* Write first nw bits as is without compression                     */
        /* We write nw bits as is and decoder later uses this information to */
        /* recover initial nw                                                */
        bit_string_concat_and_destroy(result, cfc_encode(nw));
        bit_string_copy(result,src,0,nw);
        //DEBUG_PRINT("lz77_encode start nw = %zu lognw = %zu offset = %zu first = %zu src->offset %zu\n",nw,lognw,offset,first,src->offset);
    } else {
        /* This is not a first block, so the window was passed to this
         * function. Decode next block using that window as initial window. We
         * do it simply by adding window in front of source bitstring. */
        bit_string_t *tmp = bit_string_init(src->size + window->size);
        bit_string_concat(tmp,window);
        bit_string_concat(tmp,src);
        src = tmp;
        offset = window->offset; /* Next bit to code and first uncoded bit are */
        first = window->offset;  /* now right after window                     */
    }
    
    /* After copying first nw bits we start main cycle. We go through our bit */
    /* string and search for matches in previous nw bits. If match is found   */
    /* and it is big enough >log(nw), then we compress it. Otherwise we check */
    /* if number of bits we have is not too big. If it becomes bigger than    */
    /* log(nw), we have to write them uncompressed.                           */
    while(offset < src->offset) {
        PRINT_DEBUG("lz77_encode offset %zu first %zu\n",offset,first);
        if(offset - first >= lognw) {
            PRINT_DEBUG("lz77_encode %zu bits without matches, sending them uncompressed\n",lognw);
            bit_string_concat_and_destroy(result, cfc_encode(lognw));
            bit_string_copy(result,src,first,lognw);
            first += lognw;
        }
        best_match = best_offset = 0;
        for(int i = 1; i < nw; ++i) {
            current_match = bit_string_sub_cmp(src,src,offset,offset - i);
            if(current_match > best_match) {
                best_match = current_match;
                best_offset = i;
            }
        }
        PRINT_DEBUG("lz77_encode best_match %zu best_offset %zu\n",best_match,best_offset);
        if(best_match > lognw) {
            if(first != offset) {
                bit_string_concat_and_destroy(result, cfc_encode(offset - first));
                bit_string_copy(result,src,first,offset - first);
            }
            bit_string_concat_and_destroy(result, cfc_encode(best_match));
            for(int i = lognw - 1; i >= 0; --i) {
                bit_string_append_bit(result, best_offset >> i & 1);
            }
            offset += best_match;
            first = offset;
        } else {
            offset++;
        }
    }

    if(offset != first) {
        PRINT_DEBUG("lz77_encode write %zu last bits\n",offset - first);
        bit_string_concat_and_destroy(result, cfc_encode(offset - first));
        bit_string_copy(result,src,first,offset - first);
    }

    if(window != NULL) {
        bit_string_destroy(src);
    }
    
    return result;
}

bit_string_t *lz77_decode(bit_string_t *src, size_t size, size_t *enc_size, bit_string_t *window,size_t *window_size)
{
    if(window != NULL) {
        size += window->offset;
    }
    bit_string_t *result = bit_string_init(size);
    size_t len, data_offset, offset = 0;

    if(window == NULL) {
        *window_size = cfc_decode(src,&offset);
    }

    size_t nw = *window_size;
    size_t lognw = ceil(log2(nw));


    PRINT_DEBUG("lz77_decode nw %zu lognw %zu\n",nw,lognw);

    if(window == NULL) {
        for(int i = 0; i < nw; ++i) {
            bit_string_append_bit(result,bit_string_get_bit(src,offset++));
        }
    } else {
        for(int i = 0; i < window->offset; ++i) {
            bit_string_append_bit(result,bit_string_get_bit(window,i));
        }
    }

    while(offset < src->offset && result->offset < size) {
        PRINT_DEBUG("lz77_decode offset %zu src->offset %zu\n",offset,src->offset);
        len = cfc_decode(src,&offset);
        if(len == 0 || offset > src->offset) {
            PRINT_DEBUG("lz77_decode 0 length block or out of bounds - stopping\n");
            break;
        }
        if(len <= lognw) {
            // Uncompressed data follows
            PRINT_DEBUG("lz77_decode uncompressed data length %zu\n",len);
            bit_string_copy(result,src,offset,len);
            offset += len;
        } else {
            // Offset follows (lognw bits number)
            data_offset = 0;
            for(int i = 0; i < lognw; ++i) {
                data_offset = (data_offset << 1) | bit_string_get_bit(src,offset++);
            }
            PRINT_DEBUG("lz77_decode compressed data legnth %zu offset %zu\n",len,data_offset);
            for(int i = 0; i < len; ++i) {
                bit_string_append_bit(result,bit_string_get_bit(result,result->offset - data_offset));
            }
        }
    }

    *enc_size = offset;

    if(window != NULL) {
        bit_string_t *tmp = bit_string_substr(result,window->offset,result->offset - window->offset);
        bit_string_destroy(result);
        result = tmp;
    }
    
    return result;
}
