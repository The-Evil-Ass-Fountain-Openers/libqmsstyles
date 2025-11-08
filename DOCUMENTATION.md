# Introduction

## WARNING: very unfinished and can be inaccurate

Windows Vista+ msstyles is what contains almost every asset you see in the file explorer, desktop, start menu, taskbar, window frames and common controls, such as the context menu and buttons.

## General information

msstyles are basically DLL files, but with a different extension. This means that you can grab any msstyles and just open them in a resource editor like ResourceHacker.

The order of msstyles is as follows:

1. Class (the category of the asset)
2. Part (the exact asset type)
3. State
4. Properties

``Common Properties`` seems to be the base state, which then the other states can override some of the properties set in it.

The data in msstyles are split between many resources. The resources are:

### PACKTHEM_VERSION

Unknown. Very small in size.

### MINCOLORDEPTH

Unknown. Also very small in size.

### 16

Contains DLL-like info.

### MUI

Contains the language it was made for at the end of the resource data. Before that, every resource is listed there. The rest is unknown.

### RMAP

Unknown.

### CMAP

This is where the class map of the msstyles is stored in.

### VARIANT NORMAL

From the looks of it, this is where every property value is stored in.

### VMAP

Unknown.

### BCMAP

According to an msstyleEditor comment, this is where every class' part is stored in.

### AMAP

This is where the animation map is stored at in Windows 8 and later.

### STREAM

This is where the atlas image for DWM windows is stored in.

## 9-slicing

This is a technique which opts for dividing the texture into 9 parts rather than stretching the whole texture. These parts are known as:

```
top right,    top,    top left 
right,        center, left
bottom right, bottom, bottom left 
```

Top right, top left, bottom right and bottom left corners are left untouched.
Top and bottom borders are stretched horizontally, while right and left borders are stretched vertically.
Lastly, the center part gets stretched both vertically and horizontally.

This is the technique used by Windows for rendering most control textures.


## Rendering engines

Windows mostly uses GDI+ as the main rendering engine. Except for DWM windows. DWM windows use DirectX due to blur being needed. Non-DWM rendered windows use the default GDI+ rendering engine.
This matters because both the engines have different methods for stretching textures. GDI+ uses nearest neighbor, while DirectX uses bilinear antialiasing.


# Properties

msstyles properties dictate how the assets inside the file should be treated; whether they should be tiled, if they should be stretched, how the content inside of it should be positioned, how the texture should be *9-sliced*, if it should be transparent, if it should have a background color, a gradient, etc.

## IMAGEFILE

This property contains the ID of the pixmap to be used for the control. If this property is set, the IMAGEFILE will overlap every other background that could possibly be set for the control.

## CONTENTMARGINS

This kind of property controls the position of the control contents. You can make any content inside a control be pushed towards the left, the right, or anywhere you'd like with this.
The format is as follows:

`CONTENTMARGINS: 2, 3, 4, 1`

`2`: left margin
`3`: right margin
`4`: top margin
`1`: bottom margin

**NOTE:** From my testing, some controls can outright ignore this property, like the line edit control in Windows Vista.

## SIZINGMARGINS

This kind of property controls how the *9-slicing* should be done. Every number in this property is the border size for the texture. The format is the same as the **CONTENTMARGINS** format:

`SIZINGMARGINS: 4, 3, 5, 6`

`4`: left margin
`3`: right margin
`5`: top margin
`6`: bottom margin

## IMAGECOUNT

This property contains how many textures are present in the image file. The provided **IMAGEFILE**'s height is then divided by this property to get the height of each texture. In the case that the **ORIENTATION** property is set to horizontal, the **IMAGEFILE**'s width is divided by this property to get the width of each texture.

## ORIENTATION

This property contains how the textures are positioned in the provided **IMAGEFILE**. This property can have 2 possible values:

- **HORIZONTAL**
- **VERTICAL**
