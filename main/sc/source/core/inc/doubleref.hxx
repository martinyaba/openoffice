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



#ifndef SC_DOUBLEREF_HXX
#define SC_DOUBLEREF_HXX

#include "address.hxx"
#include "scmatrix.hxx"

class ScDocument;
class ScBaseCell;
struct ScDBQueryParamBase;
struct ScQueryParamBase;

// ============================================================================

/** 
 * Base class for abstracting range data backends for database functions. 
 */
class ScDBRangeBase
{
public:
    enum RefType { INTERNAL, EXTERNAL }; // TODO: We may not need this after all... (kohei)

    virtual ~ScDBRangeBase() = 0;

    bool fillQueryEntries(ScQueryParamBase* pParam, const ScDBRangeBase* pDBRef) const;

    virtual SCCOL getColSize() const = 0;
    virtual SCROW getRowSize() const = 0;
    virtual SCSIZE getVisibleDataCellCount() const = 0;

    /** 
     * Get a string value of a specified cell position.  Note that the 
     * position of the upper left cell of the range is always (0, 0) even if 
     * the reference type is of internal range.
     *
     * @param nCol column position (0 to column size-1)
     * @param nRow row position (0 to row size-1)
     */
    virtual ::rtl::OUString getString(SCCOL nCol, SCROW nRow) const = 0;

    virtual SCCOL getFirstFieldColumn() const = 0;

    /** 
     * Get a <i>0-based</i> column index that corresponds with the passed field
     * index.  Note that the field index passed as the 1st parameter is 
     * <i>1-based.</i> 
     *
     * @param nIndex 1-based field index.
     * 
     * @return 0-based column index
     */
    virtual SCCOL findFieldColumn(SCCOL nIndex) const = 0;
    virtual SCCOL findFieldColumn(const ::rtl::OUString& rStr, sal_uInt16* pErr = NULL) const = 0;
    virtual ScDBQueryParamBase* createQueryParam(const ScDBRangeBase* pQueryRef) const = 0;
    virtual bool isRangeEqual(const ScRange& rRange) const = 0;

protected:
    ScDBRangeBase(ScDocument* pDoc, RefType eType);
    ScDocument* getDoc() const;

    /** 
     * Populate query options that are always the same for all database 
     * queries.
     */
    static void fillQueryOptions(ScQueryParamBase* pParam);

private:
    ScDBRangeBase(); // disabled

    ScDocument* mpDoc;
    RefType meType;
};

// ============================================================================

class ScDBInternalRange : public ScDBRangeBase
{
public:
    explicit ScDBInternalRange(ScDocument* pDoc, const ScRange& rRange);
    virtual ~ScDBInternalRange();

    const ScRange& getRange() const;

    virtual SCCOL getColSize() const;
    virtual SCROW getRowSize() const;
    virtual SCSIZE getVisibleDataCellCount() const;

    /** 
     * Get a string value of a specified cell position.  Note that the 
     * position of the upper left cell of the range is always (0, 0) even if 
     * the reference type is of internal range.
     *
     * @param nCol column position (0 to column size-1)
     * @param nRow row position (0 to row size-1)
     */
    virtual ::rtl::OUString getString(SCCOL nCol, SCROW nRow) const;

    virtual SCCOL getFirstFieldColumn() const;
    /** 
     * Get a <i>0-based</i> column index that corresponds with the passed field
     * index.  Note that the field index passed as the 1st parameter is 
     * <i>1-based.</i> 
     *
     * @param nIndex 1-based field index.
     * 
     * @return 0-based column index
     */
    virtual SCCOL findFieldColumn(SCCOL nIndex) const;
    virtual SCCOL findFieldColumn(const ::rtl::OUString& rStr, sal_uInt16* pErr = NULL) const;
    virtual ScDBQueryParamBase* createQueryParam(const ScDBRangeBase* pQueryRef) const;
    virtual bool isRangeEqual(const ScRange& rRange) const;

private:
    ScRange maRange;
};

// ============================================================================

class ScDBExternalRange : public ScDBRangeBase
{
public:
    explicit ScDBExternalRange(ScDocument* pDoc, const ScMatrixRef& pMat);
    virtual ~ScDBExternalRange();

    virtual SCCOL getColSize() const;
    virtual SCROW getRowSize() const;
    virtual SCSIZE getVisibleDataCellCount() const;

    /** 
     * Get a string value of a specified cell position.  Note that the 
     * position of the upper left cell of the range is always (0, 0) even if 
     * the reference type is of internal range.
     *
     * @param nCol column position (0 to column size-1)
     * @param nRow row position (0 to row size-1)
     */
    virtual ::rtl::OUString getString(SCCOL nCol, SCROW nRow) const;

    virtual SCCOL getFirstFieldColumn() const;

    /** 
     * Get a <i>0-based</i> column index that corresponds with the passed field
     * index.  Note that the field index passed as the 1st parameter is 
     * <i>1-based.</i> 
     *
     * @param nIndex 1-based field index.
     * 
     * @return 0-based column index
     */
    virtual SCCOL findFieldColumn(SCCOL nIndex) const;
    virtual SCCOL findFieldColumn(const ::rtl::OUString& rStr, sal_uInt16* pErr = NULL) const;
    virtual ScDBQueryParamBase* createQueryParam(const ScDBRangeBase* pQueryRef) const;
    virtual bool isRangeEqual(const ScRange& rRange) const;

private:
    const ScMatrixRef mpMatrix;
    SCCOL mnCols;
    SCROW mnRows;
};

#endif
