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



// MARKER(update_precomp.py): autogen include statement, do not remove
#include "precompiled_vcl.hxx"

#include <X11/Xatom.h>
#include <X11_clipboard.hxx>
#include <X11_transferable.hxx>
#include <com/sun/star/lang/DisposedException.hpp>
#include <com/sun/star/datatransfer/clipboard/RenderingCapabilities.hpp>
#include <com/sun/star/lang/XMultiServiceFactory.hpp>
#include <com/sun/star/lang/XSingleServiceFactory.hpp>
#include <com/sun/star/registry/XRegistryKey.hpp>
#include <uno/dispatcher.h> // declaration of generic uno interface
#include <uno/mapping.hxx> // mapping stuff
#include <cppuhelper/factory.hxx>
#include <rtl/tencinfo.h>

#if OSL_DEBUG_LEVEL > 1
#include <stdio.h>
#endif

using namespace com::sun::star::datatransfer;
using namespace com::sun::star::datatransfer::clipboard;
using namespace com::sun::star::lang;
using namespace com::sun::star::uno;
using namespace com::sun::star::awt;
using namespace cppu;
using namespace osl;
using namespace rtl;
using namespace x11;

X11Clipboard::X11Clipboard( SelectionManager& rManager, Atom aSelection ) :
        ::cppu::WeakComponentImplHelper4<
    ::com::sun::star::datatransfer::clipboard::XClipboardEx,
    ::com::sun::star::datatransfer::clipboard::XClipboardNotifier,
    ::com::sun::star::lang::XServiceInfo,
    ::com::sun::star::lang::XInitialization
    >( rManager.getMutex() ),

		m_rSelectionManager( rManager ),
		m_xSelectionManager( & rManager ),
		m_aSelection( aSelection )
{
#if OSL_DEBUG_LEVEL > 1
	fprintf( stderr, "creating instance of X11Clipboard (this=%p)\n", this );
#endif

	if( m_aSelection != None )
    {
		m_rSelectionManager.registerHandler( m_aSelection, *this );
    }
	else
	{
		m_rSelectionManager.registerHandler( XA_PRIMARY, *this );
		m_rSelectionManager.registerHandler( m_rSelectionManager.getAtom( OUString::createFromAscii( "CLIPBOARD" ) ), *this );
	}
}

// ------------------------------------------------------------------------

X11Clipboard::~X11Clipboard()
{
	MutexGuard aGuard( *Mutex::getGlobalMutex() );

#if OSL_DEBUG_LEVEL > 1
	fprintf( stderr, "shutting down instance of X11Clipboard (this=%p, Selecttion=\"%s\")\n", this, OUStringToOString( m_rSelectionManager.getString( m_aSelection ), RTL_TEXTENCODING_ISO_8859_1 ).getStr() );
#endif
	if( m_aSelection != None )
		m_rSelectionManager.deregisterHandler( m_aSelection );
	else
	{
		m_rSelectionManager.deregisterHandler( XA_PRIMARY );
		m_rSelectionManager.deregisterHandler( m_rSelectionManager.getAtom( OUString::createFromAscii( "CLIPBOARD" ) ) );
	}
}


// ------------------------------------------------------------------------

void X11Clipboard::fireChangedContentsEvent()
{
    ClearableMutexGuard aGuard( m_rSelectionManager.getMutex() );
#if OSL_DEBUG_LEVEL > 1
    fprintf( stderr, "X11Clipboard::fireChangedContentsEvent for %s (%d listeners)\n",
             OUStringToOString( m_rSelectionManager.getString( m_aSelection ), RTL_TEXTENCODING_ISO_8859_1 ).getStr(), m_aListeners.size() );
#endif
    ::std::list< Reference< XClipboardListener > > listeners( m_aListeners );
    aGuard.clear();

    ClipboardEvent aEvent( static_cast<OWeakObject*>(this), m_aContents);
    while( listeners.begin() != listeners.end() )
    {
        if( listeners.front().is() )
            listeners.front()->changedContents(aEvent);
        listeners.pop_front();
    }
}

// ------------------------------------------------------------------------

void X11Clipboard::clearContents()
{
	ClearableMutexGuard aGuard(m_rSelectionManager.getMutex());
    // protect against deletion during outside call
    Reference< XClipboard > xThis( static_cast<XClipboard*>(this));
    // copy member references on stack so they can be called
    // without having the mutex
    Reference< XClipboardOwner > xOwner( m_aOwner );
    Reference< XTransferable > xTrans( m_aContents );
	// clear members
    m_aOwner.clear();
    m_aContents.clear();
    
    // release the mutex
    aGuard.clear();

    // inform previous owner of lost ownership
	if ( xOwner.is() )
		xOwner->lostOwnership(xThis, m_aContents);
}

// ------------------------------------------------------------------------

Reference< XTransferable > SAL_CALL X11Clipboard::getContents()
	throw(RuntimeException)
{
	MutexGuard aGuard(m_rSelectionManager.getMutex());

	if( ! m_aContents.is() )
		m_aContents = new X11Transferable( SelectionManager::get(), static_cast< OWeakObject* >(this), m_aSelection );
	return m_aContents;
}

// ------------------------------------------------------------------------

void SAL_CALL X11Clipboard::setContents(
	const Reference< XTransferable >& xTrans,
	const Reference< XClipboardOwner >& xClipboardOwner )
	throw(RuntimeException)
{
	// remember old values for callbacks before setting the new ones.
	ClearableMutexGuard aGuard(m_rSelectionManager.getMutex());

	Reference< XClipboardOwner > oldOwner( m_aOwner );
	m_aOwner = xClipboardOwner;

	Reference< XTransferable > oldContents( m_aContents );
	m_aContents = xTrans;

	aGuard.clear();

	// for now request ownership for both selections
	if( m_aSelection != None )
		m_rSelectionManager.requestOwnership( m_aSelection );
	else
	{
		m_rSelectionManager.requestOwnership( XA_PRIMARY );
		m_rSelectionManager.requestOwnership( m_rSelectionManager.getAtom( OUString::createFromAscii( "CLIPBOARD" ) ) );
	}

	// notify old owner on loss of ownership
	if( oldOwner.is() )
		oldOwner->lostOwnership(static_cast < XClipboard * > (this), oldContents);

	// notify all listeners on content changes
	fireChangedContentsEvent();
}

// ------------------------------------------------------------------------

OUString SAL_CALL X11Clipboard::getName()
	throw(RuntimeException)
{
	return m_rSelectionManager.getString( m_aSelection );
}

// ------------------------------------------------------------------------

sal_Int8 SAL_CALL X11Clipboard::getRenderingCapabilities()
	throw(RuntimeException)
{
	return RenderingCapabilities::Delayed;
}


// ------------------------------------------------------------------------
void SAL_CALL X11Clipboard::addClipboardListener( const Reference< XClipboardListener >& listener )
	throw(RuntimeException)
{
    MutexGuard aGuard( m_rSelectionManager.getMutex() );
    m_aListeners.push_back( listener );
}

// ------------------------------------------------------------------------

void SAL_CALL X11Clipboard::removeClipboardListener( const Reference< XClipboardListener >& listener )
	throw(RuntimeException)
{
    MutexGuard aGuard( m_rSelectionManager.getMutex() );
    m_aListeners.remove( listener );
}


// ------------------------------------------------------------------------

Reference< XTransferable > X11Clipboard::getTransferable()
{
	return getContents();
}

// ------------------------------------------------------------------------

void X11Clipboard::clearTransferable()
{
	clearContents();
}

// ------------------------------------------------------------------------

void X11Clipboard::fireContentsChanged()
{
    fireChangedContentsEvent();
}

// ------------------------------------------------------------------------

Reference< XInterface > X11Clipboard::getReference() throw()
{
    return Reference< XInterface >( static_cast< OWeakObject* >(this) );
}

// ------------------------------------------------------------------------

OUString SAL_CALL X11Clipboard::getImplementationName(  )
	throw(RuntimeException)
{
	return OUString::createFromAscii(X11_CLIPBOARD_IMPLEMENTATION_NAME);
}

// ------------------------------------------------------------------------

sal_Bool SAL_CALL X11Clipboard::supportsService( const OUString& ServiceName )
	throw(RuntimeException)
{
	Sequence < OUString > SupportedServicesNames = X11Clipboard_getSupportedServiceNames();

	for ( sal_Int32 n = SupportedServicesNames.getLength(); n--; )
		if (SupportedServicesNames[n].compareTo(ServiceName) == 0)
			return sal_True;

	return sal_False;
}

// ------------------------------------------------------------------------

void SAL_CALL X11Clipboard::initialize( const Sequence< Any >& ) throw( ::com::sun::star::uno::Exception )
{
}

// ------------------------------------------------------------------------

Sequence< OUString > SAL_CALL X11Clipboard::getSupportedServiceNames(	 )
	throw(RuntimeException)
{
	return X11Clipboard_getSupportedServiceNames();
}

