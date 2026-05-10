# msstyles documentation
Please help me

## Resource information

### CMAP
Class map.
Each letter is separated by one null byte, while the names themselves are separated from each other by at least two null bytes.

### BCMAP
Base class map.
The first 4 bytes are the amount of parent indexes. The data that follows is an `uint8` array padded to 4 bytes.

### MINCOLORDEPTH
Pretty self explanatory.

### VARIANT_*X*
Visual style variant.
Can usually be colors change, font change, etc (you can tell I have not looked into this like At All).

### VMAP
Variants map.
Stores the name of every variant available in the visual style.

### RMAP
TODO

## Structure
The visual style structure is as follows:

- Base class (it can be the class name after the `::`)
- Class (example: `Button`)
	- Common Properties (parts inherit props from here)
	- Part (example: `PUSHBUTTON`)
		- Common (states inherit props from here) 
		- State (example: `NORMAL`)
			- Property

The reason why base class (the class to inherit properties from) *can be* the class name after the `::` is because the author of the visual style can choose to simply override that *and* keep the same name by using `BCMAP`. 
An example of this would be the `TrayNotifyVertOpenComposited::Button` within the official Windows 7 msstyles. This class' name tells you that it inherits from `Button`, yet this class actually overrides that with `TrayNotifyVertOpen::Button`. Just something important to note.

