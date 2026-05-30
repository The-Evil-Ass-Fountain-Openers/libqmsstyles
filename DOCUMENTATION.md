# msstyles documentation

Most information here has been taken from [msstyleEditor's wiki](https://github.com/nptr/msstyleEditor/wiki/.msstyles-Format). 
This is also unfinished.

## Resource information

### CMAP

Class map.

Each letter is separated by one null byte, while the names themselves are separated from each other by at least two null bytes.

### BCMAP

Base class map.

This resource contains class inheritance information. The first 4 bytes are the amount of parent indexes. The data that follows is an `uint32` array.

### MINCOLORDEPTH

Pretty self explanatory.

### VARIANT_*X*

Visual style variant.

Contains the properties for each state and part.

The first 32 bytes is the property header, which contains important information about the property. The rest is the property data itself. Each property is separated by 4 null bytes. 

The following table shows the offset, size and description of each section in the header.

| Offset | Size | Description |
|--------|------|-------------|
| 0 | 4 | Contains the ID of the name. |
| 4 | 4 | Contains the ID of the type. |
| 8 | 4 | Contains the ID of the class it's part of. |
| 12 | 4 | Contains the ID of the part it's part of. |
| 16 | 4 | Contains the ID of the state it's part of. |
| 20 | 4 | Unknown. If it's any value but 0 then the property is empty and must be skipped. |
| 24 | 4 | Unknown. Seems to always be 4 bytes of 0. |
| 28 | 4 | Contains the size of the data that follows this header. |
 

### VMAP

Variants map.

Stores the name of every variant available in the visual style.

### RMAP

Root map.

Contains visual style metadata such as name, display name, description, company, author, etc.

In `aero.msstyles`, only name and last updated date are set. Name and tooltip also repeat twice in that order specifically after last updated date.

### PACKTHEM_VERSION

The version of the official Microsoft msstyles packaging tool used to package the msstyles file.

XP's version is 0x0300 (3). Vista and later's version is 0x0400 (4).

### AMAP

TODO

## Structure

The visual style structure is as follows:

- Base class (TODO: look into how class inheritance works exactly)
- Class (example: `Button`)
	- Common Properties (parts inherit props from here)
	- Part (example: `PUSHBUTTON`)
		- Common (states inherit props from here) 
		- State (example: `NORMAL`)
			- Property

