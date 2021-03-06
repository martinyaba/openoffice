/**************************************************************
 * 
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 * 
 *   http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 * 
 *************************************************************/



#include <osl/interlck.h>

#ifndef _RTL_STRING_HXX_
#include <rtl/ustrbuf.hxx>
#endif
#include <rtl/memory.h>

/*
#include <rtl/alloc.h>
*/



/*************************************************************************
 *  rtl_uStringbuffer_newFromStr_WithLength
 */
void SAL_CALL rtl_uStringbuffer_newFromStr_WithLength( rtl_uString ** newStr,
                                                       const sal_Unicode * value,
                                                       sal_Int32 count)
{
    if (!value)
    {
        rtl_uString_new_WithLength( newStr, 16 );
        return;
    }

    rtl_uString_new_WithLength( newStr, count + 16 );
    (*newStr)->length = count;
    rtl_copyMemory( (*newStr)->buffer, value, count * sizeof(sal_Unicode));
    return;
}

/*************************************************************************
 *  rtl_uStringbuffer_newFromStringBuffer
 */
sal_Int32 SAL_CALL rtl_uStringbuffer_newFromStringBuffer( rtl_uString ** newStr,
                                                          sal_Int32 capacity,
                                                          rtl_uString * oldStr )
{
    sal_Int32 newCapacity = capacity;

    if (newCapacity < oldStr->length)
        newCapacity = oldStr->length;

    rtl_uString_new_WithLength( newStr, newCapacity );
    
    if (oldStr->length > 0) {
        (*newStr)->length = oldStr->length;    
        rtl_copyMemory( (*newStr)->buffer, oldStr->buffer, oldStr->length * sizeof(sal_Unicode));
    }
    return newCapacity;
}

/*************************************************************************
 *  rtl_uStringbuffer_ensureCapacity
 */
void SAL_CALL rtl_uStringbuffer_ensureCapacity
    (rtl_uString ** This, sal_Int32* capacity, sal_Int32 minimumCapacity)
{
    if (minimumCapacity > *capacity)
    {
        rtl_uString * pTmp = *This;
        rtl_uString * pNew = NULL;
        *capacity = ((*This)->length + 1) * 2;
        if (minimumCapacity > *capacity)
            /* still lower, set to the minimum capacity */
            *capacity = minimumCapacity;

        rtl_uString_new_WithLength(&pNew, *capacity);
        pNew->length = (*This)->length;
        *This = pNew;

        rtl_copyMemory( (*This)->buffer, pTmp->buffer, pTmp->length * sizeof(sal_Unicode) );
        rtl_uString_release( pTmp );
    }
}

/*************************************************************************
 *  rtl_uStringbuffer_insert
 */
void SAL_CALL rtl_uStringbuffer_insert( rtl_uString ** This,
                                        sal_Int32 * capacity,
                                        sal_Int32 offset,
                                        const sal_Unicode * str,
                                        sal_Int32 len)
{
    sal_Int32 nOldLen;
    sal_Unicode * pBuf;
    sal_Int32 n;
    if( len != 0 )
    {
        if (*capacity < (*This)->length + len)
            rtl_uStringbuffer_ensureCapacity( This, capacity, (*This)->length + len );

        /*
        if( len == 1 )
            This->buffer
        */
        nOldLen = (*This)->length;
        pBuf = (*This)->buffer;

        /* copy the tail */
        n = (nOldLen - offset);
        if( n == 1 )
                            /* optimized for 1 character */
            pBuf[offset + len] = pBuf[offset];
        else if( n > 1 )
            rtl_moveMemory( pBuf + offset + len, pBuf + offset, n * sizeof(sal_Unicode) );

        /* insert the new characters */
        if( len == 1 )
            /* optimized for 1 character */
            pBuf[offset] = *str;
        else if( len > 1 )
            rtl_copyMemory( pBuf + offset, str, len * sizeof(sal_Unicode) );
        (*This)->length = nOldLen + len;
        pBuf[ nOldLen + len ] = 0;
    }
}

void rtl_uStringbuffer_insertUtf32(
    rtl_uString ** pThis, sal_Int32 * capacity, sal_Int32 offset, sal_uInt32 c)
{
    sal_Unicode buf[2];
    sal_Int32 len;
    OSL_ASSERT(c <= 0x10FFFF && !(c >= 0xD800 && c <= 0xDFFF));
    if (c <= 0xFFFF) {
        buf[0] = (sal_Unicode) c;
        len = 1;
    } else {
        c -= 0x10000;
        buf[0] = (sal_Unicode) ((c >> 10) | 0xD800);
        buf[1] = (sal_Unicode) ((c & 0x3FF) | 0xDC00);
        len = 2;
    }
    rtl_uStringbuffer_insert(pThis, capacity, offset, buf, len);
}

/*************************************************************************
 *  rtl_uStringbuffer_insert_ascii
 */
void SAL_CALL rtl_uStringbuffer_insert_ascii(   /*inout*/rtl_uString ** This,
                                                /*inout*/sal_Int32 * capacity,
                                                sal_Int32 offset,
                                                const sal_Char * str,
                                                sal_Int32 len)
{
    sal_Int32 nOldLen;
    sal_Unicode * pBuf;
    sal_Int32 n;
    if( len != 0 )
    {
        if (*capacity < (*This)->length + len)
            rtl_uStringbuffer_ensureCapacity( This, capacity, (*This)->length + len );

        nOldLen = (*This)->length;
        pBuf = (*This)->buffer;

        /* copy the tail */
        n = (nOldLen - offset);
        if( n == 1 )
            /* optimized for 1 character */
            pBuf[offset + len] = pBuf[offset];
        else if( n > 1 )
            rtl_moveMemory( pBuf + offset + len, pBuf + offset, n * sizeof(sal_Unicode) );

        /* insert the new characters */
        for( n = 0; n < len; n++ )
        {
            /* Check ASCII range */
            OSL_ENSURE( (*str & 0x80) == 0, "Found ASCII char > 127");

            pBuf[offset + n] = (sal_Unicode)*(str++);
        }

        (*This)->length = nOldLen + len;
        pBuf[ nOldLen + len ] = 0;
    }
}


