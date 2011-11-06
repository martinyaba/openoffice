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



#ifndef OOX_XLS_BIFFCODEC_HXX
#define OOX_XLS_BIFFCODEC_HXX

#include <vector>
#include <comphelper/docpasswordhelper.hxx>
#include "oox/core/binarycodec.hxx"
#include "oox/xls/workbookhelper.hxx"

namespace oox {
namespace xls {

// ============================================================================

const sal_Int64 BIFF_RCF_BLOCKSIZE          = 1024;

// ============================================================================

/** Base class for BIFF stream decoders. */
class BiffDecoderBase : public ::comphelper::IDocPasswordVerifier
{
public:
    explicit            BiffDecoderBase();
    virtual             ~BiffDecoderBase();

    /** Derived classes return a clone of the decoder for usage in new streams. */
    inline BiffDecoderBase* clone() { return implClone(); }

    /** Implementation of the ::comphelper::IDocPasswordVerifier interface. */
    virtual ::comphelper::DocPasswordVerifierResult verifyPassword( const ::rtl::OUString& rPassword, ::com::sun::star::uno::Sequence< ::com::sun::star::beans::NamedValue >& o_rEncryptionData );
    virtual ::comphelper::DocPasswordVerifierResult verifyEncryptionData( const ::com::sun::star::uno::Sequence< ::com::sun::star::beans::NamedValue >& o_rEncryptionData );

    /** Returns true, if the decoder has been initialized correctly. */
    inline bool         isValid() const { return mbValid; }

    /** Decodes nBytes bytes and writes encrypted data into the buffer pnDestData. */
    void                decode(
                            sal_uInt8* pnDestData,
                            const sal_uInt8* pnSrcData,
                            sal_Int64 nStreamPos,
                            sal_uInt16 nBytes );

private:
    /** Derived classes return a clone of the decoder for usage in new streams. */
    virtual BiffDecoderBase* implClone() = 0;

    /** Derived classes implement password verification and initialization of
        the decoder. */
    virtual ::com::sun::star::uno::Sequence< ::com::sun::star::beans::NamedValue > implVerifyPassword( const ::rtl::OUString& rPassword ) = 0;
    virtual bool implVerifyEncryptionData( const ::com::sun::star::uno::Sequence< ::com::sun::star::beans::NamedValue >& rEncryptionData ) = 0;

    /** Implementation of decryption of a memory block. */
    virtual void        implDecode(
                            sal_uInt8* pnDestData,
                            const sal_uInt8* pnSrcData,
                            sal_Int64 nStreamPos,
                            sal_uInt16 nBytes ) = 0;

private:
    bool                mbValid;        /// True = decoder is correctly initialized.
};

typedef ::boost::shared_ptr< BiffDecoderBase > BiffDecoderRef;

// ============================================================================

/** Decodes BIFF stream contents that are encoded using the old XOR algorithm. */
class BiffDecoder_XOR : public BiffDecoderBase
{
public:
    explicit            BiffDecoder_XOR( sal_uInt16 nKey, sal_uInt16 nHash );

private:
    /** Copy constructor for cloning. */
                        BiffDecoder_XOR( const BiffDecoder_XOR& rDecoder );

    /** Returns a clone of the decoder for usage in new streams. */
    virtual BiffDecoder_XOR* implClone();

    /** Implements password verification and initialization of the decoder. */
    virtual ::com::sun::star::uno::Sequence< ::com::sun::star::beans::NamedValue > implVerifyPassword( const ::rtl::OUString& rPassword );
    virtual bool implVerifyEncryptionData( const ::com::sun::star::uno::Sequence< ::com::sun::star::beans::NamedValue >& rEncryptionData );


    /** Implementation of decryption of a memory block. */
    virtual void        implDecode(
                            sal_uInt8* pnDestData,
                            const sal_uInt8* pnSrcData,
                            sal_Int64 nStreamPos,
                            sal_uInt16 nBytes );

private:
    ::oox::core::BinaryCodec_XOR maCodec;   /// Cipher algorithm implementation.
    ::com::sun::star::uno::Sequence< ::com::sun::star::beans::NamedValue > maEncryptionData;
    sal_uInt16          mnKey;
    sal_uInt16          mnHash;
};

// ============================================================================

/** Decodes BIFF stream contents that are encoded using the RC4 algorithm. */
class BiffDecoder_RCF : public BiffDecoderBase
{
public:
    explicit            BiffDecoder_RCF(
                            sal_uInt8 pnSalt[ 16 ],
                            sal_uInt8 pnVerifier[ 16 ],
                            sal_uInt8 pnVerifierHash[ 16 ] );

private:
    /** Copy constructor for cloning. */
                        BiffDecoder_RCF( const BiffDecoder_RCF& rDecoder );

    /** Returns a clone of the decoder for usage in new streams. */
    virtual BiffDecoder_RCF* implClone();

    /** Implements password verification and initialization of the decoder. */
    virtual ::com::sun::star::uno::Sequence< ::com::sun::star::beans::NamedValue > implVerifyPassword( const ::rtl::OUString& rPassword );
    virtual bool implVerifyEncryptionData( const ::com::sun::star::uno::Sequence< ::com::sun::star::beans::NamedValue >& rEncryptionData );

    /** Implementation of decryption of a memory block. */
    virtual void        implDecode(
                            sal_uInt8* pnDestData,
                            const sal_uInt8* pnSrcData,
                            sal_Int64 nStreamPos,
                            sal_uInt16 nBytes );

private:
    ::oox::core::BinaryCodec_RCF maCodec;   /// Cipher algorithm implementation.
    ::com::sun::star::uno::Sequence< ::com::sun::star::beans::NamedValue > maEncryptionData;
    ::std::vector< sal_uInt8 > maSalt;
    ::std::vector< sal_uInt8 > maVerifier;
    ::std::vector< sal_uInt8 > maVerifierHash;
};

// ============================================================================

/** Helper for BIFF stream codecs. Holds the used codec object. */
class BiffCodecHelper : public WorkbookHelper
{
public:
    explicit            BiffCodecHelper( const WorkbookHelper& rHelper );

    /** Implementation helper, reads the FILEPASS and returns a decoder object. */
    static BiffDecoderRef implReadFilePass( BiffInputStream& rStrm, BiffType eBiff );

    /** Imports the FILEPASS record, asks for a password and sets a decoder at the stream. */
    bool                importFilePass( BiffInputStream& rStrm );
    /** Clones the contained decoder object if existing and sets it at the passed stream. */
    void                cloneDecoder( BiffInputStream& rStrm );

private:
    BiffDecoderRef      mxDecoder;          /// The decoder for import filter.
};

// ============================================================================

} // namespace xls
} // namespace oox

#endif
