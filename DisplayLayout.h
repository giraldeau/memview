/*
   This file is part of memview, a real-time memory trace visualization
   application.

   Copyright (C) 2013 Andrew Clinton

   This program is free software; you can redistribute it and/or
   modify it under the terms of the GNU General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.

   This program is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
   02111-1307, USA.

   The GNU General Public License is contained in the file COPYING.
*/

#ifndef DisplayLayout_H
#define DisplayLayout_H

#include "MemoryState.h"
#include "Math.h"
#include "GLImage.h"
#include <vector>
#include <stdio.h>


class DisplayLayout {
public:
     DisplayLayout();
    ~DisplayLayout();

    enum Visualization {
	LINEAR,
	BLOCK,
	HILBERT
    };

    Visualization   getVisualization() const	{ return myVisualization; }
    void	    setVisualization(Visualization vis)
		    { myVisualization = vis; }

    void	    setCompact(bool compact)
		    { myCompact = compact; }

    // Build the block display layout from state
    void	    update(MemoryState &state,
			   int width, int zoom);

    // Get the resolution of the full layout
    int		    width() const { return myWidth; }
    int		    height() const { return myHeight; }

    // Fill an entire image, starting at the given row and column offset.
    // The Source type determines what data is put in the image.  Currently
    // there are explicit instantiations for:
    //	- uint32, StateSource
    //	- uint64, AddressSource
    template <typename T, typename Source>
    void	    fillImage(GLImage<T> &image,
			  const Source &src,
			  int roff, int coff) const;

    // Look up the memory address that corresponds to a given pixel
    uint64	    queryPixelAddress(
			  MemoryState &state,
			  int roff, int coff) const;

private:
    struct DisplayBlock {
	DisplayBlock(uint64 addr, uint64 size)
	    : myAddr(addr)
	    , mySize(size) {}

	uint64	begin() const { return myAddr; }
	uint64	end() const { return myAddr + mySize; }

	uint64	myAddr;
	uint64	mySize;

	Box	myBox;
	uint64	myDisplayAddr;
	int	myStartCol;
    };

    Visualization		myVisualization;
    std::vector<DisplayBlock>	myBlocks;
    int				myStartLevel;
    int				myWidth;
    int				myHeight;
    bool			myCompact;
};

// Fill State values from the given MemoryState
class StateSource {
public:
    StateSource(MemoryState &state) : myState(state) {}

    MemoryState::DisplayPage getPage(uint64 addr, uint64 &off) const
    { return myState.getPage(addr, off); }

    inline bool exists(const MemoryState::DisplayPage &page) const
    { return page.exists(); }

    inline void setPixel(GLImage<uint32> &image, int c, int r,
	    const MemoryState::DisplayPage &page, uint64 off) const
    { image.setPixel(c, r, page.state(off).uval); }

    inline void setScanline(uint32 *scan,
	    MemoryState::DisplayPage &page, uint64 off, int n) const
    {
	memcpy(scan, page.stateArray() + off, n*sizeof(uint32));
    }

private:
    MemoryState	&myState;
};

// Fill memory addresses
class AddressSource {
public:
    AddressSource(MemoryState &state) : myState(state) {}

    MemoryState::DisplayPage getPage(uint64 addr, uint64 &off) const
    { return myState.getPage(addr, off); }

    inline bool exists(const MemoryState::DisplayPage &) const
    { return true; }

    inline void setPixel(GLImage<uint64> &image, int c, int r,
	    const MemoryState::DisplayPage &page, uint64 off) const
    { image.setPixel(c, r, page.addr() + off); }

    inline void setScanline(uint64 *scan,
	    MemoryState::DisplayPage &page, uint64 off, int n) const
    {
	for (int i = 0; i < n; i++)
	    scan[i] = page.addr() + off + i;
    }

private:
    MemoryState	&myState;
};

#endif
